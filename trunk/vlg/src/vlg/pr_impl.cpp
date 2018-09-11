/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "pr_impl.h"
#include "conn_impl.h"
#include "sbs_impl.h"

#define VLG_INT_AWT_TIMEOUT 1

namespace vlg {

// incoming_connection_factory

incoming_connection_factory *default_conn_factory = nullptr;
incoming_connection_factory &incoming_connection_factory::default_factory()
{
    if(default_conn_factory == nullptr) {
        default_conn_factory = new incoming_connection_factory();
    }
    return *default_conn_factory;
}

incoming_connection_factory::incoming_connection_factory()
{}

incoming_connection_factory::~incoming_connection_factory()
{}

incoming_connection &incoming_connection_factory::make_incoming_connection(peer &p)
{
    return *new incoming_connection(p);
}

}

namespace vlg {

// peer_recv_task_inco_conn

peer_recv_task_inco_conn::peer_recv_task_inco_conn(std::shared_ptr<incoming_connection> &conn_sh,
                                                   vlg_hdr_rec &pkt_hdr,
                                                   std::unique_ptr<g_bbuf> &pkt_body) :
    conn_sh_(conn_sh),
    pkt_hdr_(pkt_hdr),
    pkt_body_(std::move(pkt_body))
{}

peer_recv_task_inco_conn::~peer_recv_task_inco_conn()
{}

RetCode peer_recv_task_inco_conn::execute()
{
    RetCode rcode = RetCode_OK;
    if((rcode = conn_sh_->impl_->peer_->recv_and_route_pkt(conn_sh_, &pkt_hdr_, pkt_body_.get()))) {
        IFLOG(conn_sh_->impl_->peer_->log_, trc(TH_ID, LS_EXE "[recv task:%d - execution failed - res:%d]", __func__, get_id(),
                                                rcode))
    }
    return rcode;
}

// peer_recv_task_outg_conn

peer_recv_task_outg_conn::peer_recv_task_outg_conn(outgoing_connection &conn,
                                                   vlg_hdr_rec &pkt_hdr,
                                                   std::unique_ptr<g_bbuf> &pkt_body) :
    conn_(conn),
    pkt_hdr_(pkt_hdr),
    pkt_body_(std::move(pkt_body))
{}

peer_recv_task_outg_conn::~peer_recv_task_outg_conn()
{}

RetCode peer_recv_task_outg_conn::execute()
{
    RetCode rcode = RetCode_OK;
    if((rcode = conn_.impl_->peer_->recv_and_route_pkt(conn_, &pkt_hdr_, pkt_body_.get()))) {
        IFLOG(conn_.impl_->peer_->log_, trc(TH_ID, LS_EXE "[recv task:%d - execution failed - res:%d]", __func__, get_id(),
                                            rcode))
    }
    return rcode;
}

// peer_impl

peer_impl::peer_impl(peer &publ, peer_listener &listener) :
    peer_automa(publ, listener),
    personality_(PeerPersonality_BOTH),
    srv_exectrs_(0),
    cli_exectrs_(0),
    srv_sbs_exectrs_(0),
    selector_(*this),
    prgr_conn_id_(0),
    nem_(log_),
    pers_enabled_(false),
    pers_mng_(persistence_manager_impl::get_instance()),
    pers_schema_create_(false),
    drop_existing_schema_(false),
    srv_sbs_exec_serv_(log_),
    srv_sbs_nclassid_condesc_set_(HMSz_1031, sngl_ptr_obj_mng(), sizeof(unsigned int)),
    inco_conn_factory_(nullptr)
{
    early_init();
}

RetCode peer_impl::set_params_file_dir(const char *dir)
{
    return peer_conf_ldr_.set_params_file_dir(dir);
}

RetCode peer_impl::init()
{
    RetCode rcode = RetCode_OK;
    selector_.init(srv_exectrs_, cli_exectrs_);
    srv_sbs_exec_serv_.init(srv_sbs_exectrs_);
#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
    //dynamic linking not allowed
#elif TARGET_OS_IPHONE
    //dynamic linking not allowed
#elif TARGET_OS_MAC
    rcode = init_dyna();
#else
#endif
#else
    rcode = init_dyna();
#endif
    if(pers_enabled_) {
        IFLOG(log_, dbg(TH_ID, LS_TRL "[loading persistence-configuration]", __func__))
        if((rcode = pers_mng_.load_cfg())) {
            if(rcode == RetCode_IOERR) {
                IFLOG(log_, wrn(TH_ID, LS_TRL"[no persistence-configuration file available, proceeding anyway]", __func__))
                rcode = RetCode_OK;
            } else {
                IFLOG(log_, cri(TH_ID, LS_TRL"[critical error:%d loading persistence-configuration]", __func__, rcode))
            }
        } else {
            IFLOG(log_, inf(TH_ID, LS_TRL "[persistence configuration loaded]", __func__))
        }
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::init_dyna()
{
    RetCode rcode = RetCode_OK;
    IFLOG(log_, dbg(TH_ID, LS_TRL "[extending model]", __func__))
    if(model_map_.size() > 0) {
        for(auto it = model_map_.begin(); it != model_map_.end(); it++) {
            RET_ON_KO(extend_model(it->c_str()))
        }
        IFLOG(log_, inf(TH_ID, LS_TRL "[model extended]", __func__))
    }
    if(pers_enabled_ && pers_dri_load_.size() > 0) {
        IFLOG(log_, dbg(TH_ID, LS_TRL "[loading persistence drivers]", __func__))
        RET_ON_KO(persistence_manager_impl::load_pers_driver_dyna(pers_dri_load_))
        IFLOG(log_, inf(TH_ID, LS_TRL "[persistence drivers loaded]", __func__))
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

// CONFIG SETTERS

void peer_impl::set_cfg_load_model(const char *model)
{
    if(model_map_.find(model) != model_map_.end()) {
        IFLOG(log_, wrn(TH_ID, LS_PAR "[model already specified:%s]", model))
    } else {
        model_map_.insert(model);
    }
}

void peer_impl::set_cfg_srv_sin_addr(const char *addr)
{
    selector_.srv_sockaddr_in_.sin_addr.s_addr = inet_addr(addr);
}

void peer_impl::set_cfg_srv_sin_port(int port)
{
    selector_.srv_sockaddr_in_.sin_port = htons(port);
}

void peer_impl::set_cfg_load_pers_driv(const char *driv)
{
    if(pers_dri_load_.find(driv) != pers_dri_load_.end()) {
        IFLOG(log_, wrn(TH_ID, LS_PAR "[persistent driver already specified:%s]", driv))
    } else {
        pers_dri_load_.insert(driv);
    }
}

RetCode peer_impl::extend_model(nentity_manager &nem)
{
    RetCode rcode = RetCode_OK;
    if((rcode = nem_.extend(nem))) {
        IFLOG(log_, cri(TH_ID, LS_MDL "[failed to extend nem][res:%d]", __func__, rcode))
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::extend_model(const char *model_name)
{
    IFLOG(log_, trc(TH_ID, LS_OPN "[model:%s]", __func__, model_name))
    RetCode rcode = RetCode_OK;
    if((rcode = nem_.extend(model_name))) {
        IFLOG(log_, cri(TH_ID, LS_MDL "[failed to extend nem][model:%s, res:%d]", __func__, model_name, rcode))
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::new_incoming_connection(std::shared_ptr<incoming_connection> &new_connection, unsigned int connid)
{
    incoming_connection &publ = inco_conn_factory_->make_incoming_connection(publ_);
    new_connection.reset(&publ);
    return RetCode_OK;
}

incoming_connection_factory &peer_impl::get_incoming_connection_factory() const
{
    return *inco_conn_factory_;
}

void peer_impl::set_incoming_connection_factory(incoming_connection_factory &val)
{
    inco_conn_factory_ = &val;
}

RetCode peer_impl::on_automa_load_config(int pnum,
                                         const char *param,
                                         const char *value)
{
    if(!strcmp(param, "pure_server")) {
        if(personality_ != PeerPersonality_BOTH) {
            IFLOG(log_, err(TH_ID, LS_PAR"[pure_server] check params"))
        }
        personality_ = PeerPersonality_PURE_SERVER;
    }
    if(!strcmp(param, "pure_client")) {
        if(personality_ != PeerPersonality_BOTH) {
            IFLOG(log_, err(TH_ID, LS_PAR"[pure_client] check params"))
        }
        personality_ = PeerPersonality_PURE_CLIENT;
    }
    if(!strcmp(param, "load_model")) {
        if(value) {
            if(model_map_.find(value) != model_map_.end()) {
                IFLOG(log_, err(TH_ID, LS_PAR"[load_model] model already specified:%s", value))
                return RetCode_BADCFG;
            } else {
                model_map_.insert(value);
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_PAR"[load_model] requires argument"))
            return RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_sin_addr")) {
        if(value) {
            selector_.srv_sockaddr_in_.sin_addr.s_addr = inet_addr(value);
        } else {
            IFLOG(log_, err(TH_ID, LS_PAR"[srv_sin_addr] requires argument"))
            return RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_sin_port")) {
        if(value) {
            selector_.srv_sockaddr_in_.sin_port = htons(atoi(value));
        } else {
            IFLOG(log_, err(TH_ID, LS_PAR"[srv_sin_port] requires argument"))
            return RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_exectrs")) {
        if(value) {
            srv_exectrs_ = atoi(value);
        } else {
            IFLOG(log_, err(TH_ID, LS_PAR"[srv_exectrs] requires argument"))
            return RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "cli_exectrs")) {
        if(value) {
            cli_exectrs_ = atoi(value);
        } else {
            IFLOG(log_, err(TH_ID, LS_PAR"[cli_exectrs] requires argument"))
            return RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_sbs_exectrs")) {
        if(value) {
            srv_sbs_exectrs_ = atoi(value);
        } else {
            IFLOG(log_, err(TH_ID, LS_PAR"[srv_sbs_exectrs] requires argument"))
            return RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "pers_enabled")) {
        pers_enabled_ = true;
    }
    if(!strcmp(param, "pers_schema_create")) {
        pers_schema_create_ = true;
    }
    if(!strcmp(param, "drop_existing_schema")) {
        drop_existing_schema_ = true;
    }
    if(!strcmp(param, "load_pers_driv")) {
        if(value) {
            if(pers_dri_load_.find(value) != pers_dri_load_.end()) {
                IFLOG(log_, wrn(TH_ID, LS_PAR"[load_pers_driv] driver already specified:%s, skipping", value))
                return RetCode_OK;
            } else {
                pers_dri_load_.insert(value);
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_PAR"[load_pers_driv] requires argument"))
            return RetCode_BADCFG;
        }
    }
    return listener_.on_load_config(publ_, pnum, param, value);
}

RetCode peer_impl::on_automa_early_init()
{
    rt_init_timers();
#if defined WIN32 && defined _MSC_VER
    RET_ON_KO(WSA_init(log_))
#endif
    return RetCode_OK;
}

const char *peer_impl::get_automa_name()
{
    return publ_.get_name();
}

const unsigned int *peer_impl::get_automa_version()
{
    return publ_.get_version();
}

RetCode peer_impl::on_automa_init()
{
    RetCode rcode = init();
    if(!rcode) {
        listener_.on_init(publ_);
    }
    return rcode;
}

RetCode peer_impl::on_automa_start()
{
    SelectorStatus current = SelectorStatus_UNDEF;
    RetCode rcode = RetCode_OK;
    IFLOG(log_, inf(TH_ID, LS_APL"[peer personality: << %s >>]",
                    (personality_ == PeerPersonality_BOTH) ? "both" :
                    (personality_ == PeerPersonality_PURE_SERVER) ? "pure-server" : "pure-client"))
    if(personality_ == PeerPersonality_PURE_SERVER || personality_ == PeerPersonality_BOTH) {
        PExecSrvStatus current_exc_srv = PExecSrvStatus_TOINIT;
        if((rcode = srv_sbs_exec_serv_.start())) {
            IFLOG(log_, cri(TH_ID, LS_CLO "[starting subscription executor service][res:%d]", __func__, rcode))
            return rcode;
        }
        srv_sbs_exec_serv_.await_for_status_reached(PExecSrvStatus_STARTED, current_exc_srv);
    }
    //persistence driv. begin
    if(pers_enabled_) {
        IFLOG(log_, dbg(TH_ID, LS_TRL "[starting all persistence drivers]", __func__))
        RET_ON_KO(pers_mng_.start_all_drivers())
        IFLOG(log_, inf(TH_ID, LS_TRL "[persistence drivers started]", __func__))
        if(pers_schema_create_) {
            RetCode db_res = RetCode_OK;
            IFLOG(log_, dbg(TH_ID, LS_TRL "[creating persistence schema]", __func__))
            db_res = create_persistent_schema(drop_existing_schema_ ?
                                              PersistenceAlteringMode_DROP_IF_EXIST :
                                              PersistenceAlteringMode_CREATE_ONLY);
            if(db_res) {
                if(db_res != RetCode_DBOPFAIL) {
                    IFLOG(log_, cri(TH_ID, LS_TRL "[error:%d creating persistence schema]", __func__, db_res))
                    return db_res;
                }
            }
            IFLOG(log_, inf(TH_ID, LS_TRL "[persistence schema created]", __func__))
        }
    }
    //persistence driv. end
    IFLOG(log_, dbg(TH_ID, LS_TRL "[start selector thread]", __func__))
    if((rcode = selector_.on_peer_start_actions())) {
        IFLOG(log_, err(TH_ID, LS_CLO "[selector failed starting actions][res:%d]", __func__, rcode))
        return rcode;
    }
    if((selector_.start())) {
        IFLOG(log_, err(TH_ID, LS_CLO "[selector failed to start]", __func__))
        return RetCode_PTHERR;
    }
    IFLOG(log_, dbg(TH_ID, LS_TRL "[wait selector go init]", __func__))
    selector_.await_for_status_reached(SelectorStatus_INIT,
                                       current,
                                       VLG_INT_AWT_TIMEOUT,
                                       0);
    if(!rcode) {
        rcode = listener_.on_starting(publ_);
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::on_automa_move_running()
{
    SelectorStatus current = SelectorStatus_UNDEF;
    RetCode rcode = RetCode_OK;
    if((rcode = selector_.on_peer_move_running_actions())) {
        IFLOG(log_, err(TH_ID, LS_CLO "[selector failed running actions][res:%d]", __func__, rcode))
        return rcode;
    }
    IFLOG(log_, dbg(TH_ID, LS_TRL "[request selector go ready]", __func__))
    selector_.set_status(SelectorStatus_REQUEST_READY);
    IFLOG(log_, dbg(TH_ID, LS_TRL "[wait selector go ready]", __func__))
    selector_.await_for_status_reached(SelectorStatus_READY,
                                       current,
                                       VLG_INT_AWT_TIMEOUT,
                                       0);
    IFLOG(log_, dbg(TH_ID, LS_TRL "[request selector go selecting]", __func__))
    selector_.set_status(SelectorStatus_REQUEST_SELECT);
    if(!rcode) {
        rcode = listener_.on_move_running(publ_);
    }
    return rcode;
}

RetCode peer_impl::on_automa_stop()
{
    RetCode rcode = RetCode_OK;
    if(!selector_.inco_conn_map_.empty() || !selector_.outg_conn_map_.empty()) {
        if(!force_disconnect_on_stop_) {
            IFLOG(log_, err(TH_ID, LS_CLO "[active connections detected, cannot stop peer]", __func__))
            return RetCode_KO;
        }
    }
    IFLOG(log_, dbg(TH_ID, LS_TRL "[request selector to stop]", __func__))
    selector_.set_status(SelectorStatus_REQUEST_STOP);
    selector_.interrupt();
    SelectorStatus current = SelectorStatus_UNDEF;
    selector_.await_for_status_reached(SelectorStatus_STOPPED, current);
    IFLOG(log_, dbg(TH_ID, LS_TRL "[selector stopped]", __func__))
    selector_.set_status(SelectorStatus_INIT);
    srv_sbs_exec_serv_.shutdown();
    srv_sbs_exec_serv_.await_termination();
    if(!rcode) {
        rcode = listener_.on_stopping(publ_);
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

void peer_impl::on_automa_dying_breath()
{
    listener_.on_dying_breath(publ_);
}

RetCode peer_impl::recv_and_route_pkt(outgoing_connection &conn,
                                      vlg_hdr_rec *hdr,
                                      g_bbuf *body)
{
    RetCode rcode = RetCode_OK;
    switch(hdr->phdr.pkttyp) {
        case VLG_PKT_TSTREQ_ID:
            /*TEST REQUEST******************************************************************/
            rcode = conn.impl_->recv_test_request(hdr);
            break;
        case VLG_PKT_CONRES_ID:
            /*CONNECTION RESPONSE***********************************************************/
            rcode = conn.impl_->recv_connection_response(hdr);
            break;
        case VLG_PKT_DSCOND_ID:
            /*DISCONNECTED******************************************************************/
            rcode = conn.impl_->recv_disconnection(hdr);
            break;
        case VLG_PKT_TXRESP_ID:
            /*TRANSACTION RESPONSE**********************************************************/
            rcode = conn.impl_->recv_tx_response(hdr, body);
            break;
        case VLG_PKT_SBSRES_ID:
            /*SUBSCRIPTION RESPONSE*********************************************************/
            rcode = conn.impl_->recv_sbs_start_response(hdr);
            break;
        case VLG_PKT_SBSEVT_ID:
            /*SUBSCRIPTION EVENT************************************************************/
            rcode = conn.impl_->recv_sbs_evt(hdr, body);
            break;
        case VLG_PKT_SBSSPR_ID:
            /*SUBSCRIPTION STOP RESPONSE****************************************************/
            rcode = conn.impl_->recv_sbs_stop_response(hdr);
            break;
        default:
            break;
    }
    return rcode;
}

RetCode peer_impl::recv_and_route_pkt(std::shared_ptr<incoming_connection> &inco_conn,
                                      vlg_hdr_rec *hdr,
                                      g_bbuf *body)
{
    RetCode rcode = RetCode_OK;
    switch(hdr->phdr.pkttyp) {
        case VLG_PKT_CONREQ_ID:
            /*CONNECTION REQUEST************************************************************/
            rcode = inco_conn->impl_->recv_connection_request(hdr, inco_conn);
            break;
        case VLG_PKT_DSCOND_ID:
            /*DISCONNECTED******************************************************************/
            rcode = inco_conn->impl_->recv_disconnection(hdr);
            break;
        case VLG_PKT_TXRQST_ID:
            /*TRANSACTION REQUEST***********************************************************/
            rcode = inco_conn->impl_->recv_tx_request(hdr, body, inco_conn);
            break;
        case VLG_PKT_SBSREQ_ID:
            /*SUBSCRIPTION REQUEST**********************************************************/
            rcode = inco_conn->impl_->recv_sbs_start_request(hdr, inco_conn);
            break;
        case VLG_PKT_SBSTOP_ID:
            /*SUBSCRIPTION STOP REQUEST*****************************************************/
            rcode = inco_conn->impl_->recv_sbs_stop_request(hdr, inco_conn);
            break;
        default:
            break;
    }
    return rcode;
}

// per_nclass_id_conn_set

per_nclass_id_conn_set::per_nclass_id_conn_set() :
    sbsevtid_(0),
    ts0_(0),
    ts1_(0),
    connid_condesc_set_(HMSz_1031, conn_std_shp_omng, sizeof(unsigned int))
{}

unsigned int per_nclass_id_conn_set::next_sbs_evt_id()
{
    scoped_mx smx(mon_);
    return ++sbsevtid_;
}

void per_nclass_id_conn_set::next_time_stamp(unsigned int &ts_0,
                                             unsigned int &ts_1)
{
    scoped_mx smx(mon_);
    ts_0 = (unsigned int)time(nullptr);
    if(ts_0 == ts0_) {
        ts_1 = ++ts1_;
    } else {
        ts0_ = ts_0;
        ts_1 = ts1_ = 0;
    }
}

RetCode peer_impl::add_subscriber(incoming_subscription_impl *sbsdesc)
{
    per_nclass_id_conn_set *sdrec = nullptr;
    if(srv_sbs_nclassid_condesc_set_.get(&sbsdesc->nclassid_, &sdrec)) {
        sdrec = new per_nclass_id_conn_set();
        srv_sbs_nclassid_condesc_set_.put(&sbsdesc->nclassid_, &sdrec);
    }
    sdrec->connid_condesc_set_.put(&sbsdesc->conn_->connid_, &sbsdesc->conn_sh_);
    IFLOG(log_, dbg(TH_ID, LS_SBS"[added subscriber: connid:%d, nclass_id:%d]",
                    sbsdesc->conn_->connid_,
                    sbsdesc->nclassid_))
    return RetCode_OK;
}

RetCode peer_impl::remove_subscriber(incoming_subscription_impl *sbsdesc)
{
    per_nclass_id_conn_set *sdrec = nullptr;
    if(srv_sbs_nclassid_condesc_set_.get(&sbsdesc->nclassid_, &sdrec)) {
        IFLOG(log_, cri(TH_ID, LS_CLO"[subscriber: connid:%d not found for nclass_id:%d]",
                        __func__,
                        sbsdesc->conn_->connid_,
                        sbsdesc->nclassid_))
        return RetCode_GENERR;
    }
    if(sdrec->connid_condesc_set_.remove(&sbsdesc->conn_->connid_, nullptr)) {
        IFLOG(log_, cri(TH_ID, LS_CLO"[subscriber: connid:%d not found for nclass_id:%d]",
                        __func__,
                        sbsdesc->conn_->connid_,
                        sbsdesc->nclassid_))
        return RetCode_GENERR;
    }
    return RetCode_OK;
}

struct peer_sbs_task;

struct srv_connid_condesc_set_ashsnd_rud {
    unsigned int nclass_id;
    peer_sbs_task *tsk;
    RetCode rcode;
};

struct peer_sbs_task : public p_tsk {
    peer_sbs_task(peer_impl &peer,
                  subscription_event_impl &sbs_evt,
                  s_hm &connid_condesc_set) :
        peer_(peer),
        sbs_evt_(new subscription_event(sbs_evt)),
        connid_condesc_set_(connid_condesc_set) {}

    ~peer_sbs_task() {}

    static void enum_srv_connid_connection_map_ashsnd(const s_hm &map,
                                                      const void *key,
                                                      void *ptr,
                                                      void *ud) {
        srv_connid_condesc_set_ashsnd_rud *rud = static_cast<srv_connid_condesc_set_ashsnd_rud *>(ud);
        std::shared_ptr<incoming_connection> *conn = (std::shared_ptr<incoming_connection> *)ptr;
        std::shared_ptr<incoming_subscription> sbs_sh;
        if((*conn)->impl_->inco_nclassid_sbs_map_.get(&rud->nclass_id, &sbs_sh)) {
            IFLOG((*conn)->impl_->peer_->log_, wrn(TH_ID, LS_EXE "[no more active subscriptions on connection:%u]", __func__,
                                                   (*conn)->get_id()))
            return;
        }
        sbs_sh->impl_->submit_evt(rud->tsk->sbs_evt_);
    }

    virtual RetCode execute() override {
        srv_connid_condesc_set_ashsnd_rud rud;
        rud.nclass_id = sbs_evt_->get_data()->get_id();
        rud.tsk = this;
        rud.rcode = RetCode_OK;
        connid_condesc_set_.enum_elements_safe_read(enum_srv_connid_connection_map_ashsnd, &rud);
        return RetCode_OK;
    }

    peer_impl &get_peer() {
        return peer_;
    }

    peer_impl &peer_;
    std::shared_ptr<subscription_event> sbs_evt_;
    s_hm &connid_condesc_set_;  //connid --> condesc [uint --> sh_ptr]
};

RetCode peer_impl::get_per_nclassid_helper_rec(unsigned int nclass_id, per_nclass_id_conn_set **out)
{
    RetCode rcode = RetCode_OK;
    per_nclass_id_conn_set *sdrec = nullptr;
    if((rcode = srv_sbs_nclassid_condesc_set_.get(&nclass_id, &sdrec))) {
        sdrec = new per_nclass_id_conn_set();
        rcode = srv_sbs_nclassid_condesc_set_.put(&nclass_id, &sdrec);
    }
    *out = sdrec;
    return rcode;
}

RetCode peer_impl::submit_sbs_evt_task(subscription_event_impl &sbs_evt,
                                       s_hm &connid_condesc_set)
{
    std::shared_ptr<p_tsk> stsk(new peer_sbs_task(*this, sbs_evt, connid_condesc_set));
    return srv_sbs_exec_serv_.submit(stsk);
}

// #VER#

const char *PEERLIB_VER(void)
{
    static char str[] = "lib.vlg.ver.0.0.0.date:" __DATE__;
    return str;
}

}

