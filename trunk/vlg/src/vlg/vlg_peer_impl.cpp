/*
 *
 * (C) 2017 - giuseppe.baccini@gmail.com
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "vlg_peer_impl.h"
#include "vlg_connection_impl.h"
#include "vlg_subscription_impl.h"

//-----------------------------
// INTERNAL TIMEOUT VALUE FOR AWAIT OPERATIONS (SECONDS)
//-----------------------------
#define VLG_INT_AWT_TIMEOUT 1

namespace vlg {

bool status_running_superseeded(peer_automa *peer)
{
    return peer->peer_status() > PeerStatus_RUNNING;
}

//-----------------------------
// VLG_PEER_RECV_TASK
//-----------------------------
class peer_recv_task : public vlg::p_task {
    public:
        //---ctor
        peer_recv_task(unsigned int id,
                       peer_impl &rcvn_peer,
                       connection_impl &conn,
                       vlg::grow_byte_buffer *pkt_body,
                       vlg_hdr_rec *pkt_hdr = NULL) :
            vlg::p_task(id),
            rcvn_peer_(rcvn_peer),
            conn_(conn),
            pkt_body_(pkt_body),
            pkt_hdr_(pkt_hdr) {
            log_ = vlg::logger::get_logger("peer_recv_task");
            IFLOG(trc(TH_ID, LS_CTR "%s(taskid:%d, sockid:%d, host:%s, port:%d)",
                      __func__,
                      id,
                      conn_.get_socket(),
                      conn_.get_host_ip(),
                      conn_.get_host_port()))
        }

        ~peer_recv_task() {
            IFLOG(trc(TH_ID, LS_DTR "%s(taskid:%d)", __func__, get_id()))
            delete pkt_body_;
            delete pkt_hdr_;
        }

        /*this method will be called when this task will be run by an executor*/
        virtual vlg::RetCode execute() {
            vlg::RetCode rcode = vlg::RetCode_OK;
            if((rcode = rcvn_peer_.recv_and_route_pkt(conn_, pkt_hdr_, pkt_body_))) {
                IFLOG(dbg(TH_ID, LS_EXE "[recv task:%d - execution failed - res:%d]", get_id(),
                          rcode))
            } else {
                IFLOG(trc(TH_ID, LS_EXE "[recv task:%d - executed]", get_id()))
            }
            /************************
            RELEASE_ID: CONN_BTH_01
            ************************/
            vlg::collector &c = conn_.get_collector();
            if(c.release(&conn_) == RetCode_MEMNOTR) {
                /*forcing SocketDisconnected when disconnecting
                 this case can occur when selector thread has already managed
                 socket disconnection*/
                if(conn_.status() == ConnectionStatus_DISCONNECTING) {
                    conn_.set_status(ConnectionStatus_DISCONNECTED);
                }
            }
            return rcode;
        }

    private:
        peer_impl                &rcvn_peer_;
        connection_impl          &conn_;
        vlg::grow_byte_buffer *pkt_body_;
        vlg_hdr_rec             *pkt_hdr_;
    protected:
        static vlg::logger    *log_;
};

vlg::logger *peer_recv_task::log_ = NULL;

//-----------------------------
// VLG_PEER
//-----------------------------

nclass_logger *peer_impl::log_ = NULL;

nclass_logger *peer_impl::logger()
{
    return log_;
}

peer_impl::peer_impl(unsigned int id) :
    peer_automa(id),
    personality_(PeerPersonality_BOTH),
    srv_pkt_q_len_(VLG_DEF_SRV_EXEC_Q_LEN),
    srv_exectrs_(VLG_DEF_SRV_EXEC_NO),
    cli_pkt_q_len_(VLG_DEF_CLI_EXEC_Q_LEN),
    cli_exectrs_(VLG_DEF_CLI_EXEC_NO),
    tskid_(0),
    srv_sbs_evt_q_len_(VLG_DEF_SRV_SBS_EXEC_Q_LEN),
    srv_sbs_exectrs_(VLG_DEF_SRV_SBS_EXEC_NO),
    selector_(*this, id*2),
    prgr_conn_id_(0),
    model_map_(vlg::sngl_cstr_obj_mng(), vlg::sngl_cstr_obj_mng()),
    pers_enabled_(false),
    pers_mng_(persistence_manager_impl::get_instance()),
    pers_dri_load_(vlg::sngl_cstr_obj_mng(), vlg::sngl_cstr_obj_mng()),
    pers_schema_create_(false),
    drop_existing_schema_(false),
    srv_sbs_exec_serv_(id*4, true),
    srv_sbs_nclassid_condesc_set_(vlg::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    conn_factory_(NULL),
    conn_factory_ud_(NULL)
{
    log_ = get_nclass_logger("peer");
    IFLOG(trc(TH_ID, LS_CTR "%s(peerid:%d)", __func__, id))
}

peer_impl::~peer_impl()
{
    IFLOG(trc(TH_ID, LS_DTR "%s(peerid:%d)", __func__, peer_id_))
}

vlg::RetCode peer_impl::set_params_file_dir(const char *dir)
{
    return peer_conf_ldr_.set_params_file_dir(dir);
}

vlg::RetCode peer_impl::init_peer()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(peer_id_:%d)", __func__, peer_id_))
    vlg::RetCode rcode = vlg::RetCode_OK;
    RETURN_IF_NOT_OK(selector_.init(srv_exectrs_,
                                    srv_pkt_q_len_,
                                    cli_exectrs_,
                                    cli_pkt_q_len_))
    RETURN_IF_NOT_OK(srv_sbs_exec_serv_.init(srv_sbs_exectrs_, srv_sbs_evt_q_len_))
#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
    //dynamic linking not allowed
#elif TARGET_OS_IPHONE
    //dynamic linking not allowed
#elif TARGET_OS_MAC
    rcode = init_peer_dyna();
#else
#endif
#else
    rcode = init_peer_dyna();
#endif
    if(pers_enabled_) {
        IFLOG(dbg(TH_ID, LS_TRL "%s() - loading persistence-configuration..", __func__))
        if((rcode = pers_mng_.load_cfg())) {
            if(rcode == vlg::RetCode_IOERR) {
                IFLOG(wrn(TH_ID, LS_TRL
                          "%s() - no persistence-configuration file available. proceeding anyway.",
                          __func__))
                rcode = vlg::RetCode_OK;
            } else {
                IFLOG(cri(TH_ID, LS_TRL
                          "%s(res:%d) - critical error loading persistence-configuration.", __func__,
                          rcode))
            }
        } else {
            IFLOG(inf(TH_ID, LS_TRL "%s() - persistence configuration loaded.", __func__))
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::init_peer_dyna()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(peer_id_:%d)", __func__, peer_id_))
    vlg::RetCode rcode = vlg::RetCode_OK;
    IFLOG(dbg(TH_ID, LS_TRL "%s() - extending model..", __func__))
    char model_name[VLG_MDL_NAME_LEN] = {0};
    if(model_map_.size() > 0) {
        model_map_.start_iteration();
        while(!model_map_.next(model_name, NULL)) {
            RETURN_IF_NOT_OK(extend_model(model_name))
        }
        IFLOG(inf(TH_ID, LS_TRL "%s() - model extended.", __func__))
    }
    if(pers_enabled_ && pers_dri_load_.size() > 0) {
        IFLOG(dbg(TH_ID, LS_TRL "%s() - loading persistence drivers..", __func__))
        RETURN_IF_NOT_OK(persistence_manager_impl::load_pers_driver_dyna(
                             pers_dri_load_))
        IFLOG(inf(TH_ID, LS_TRL "%s() - persistence drivers loaded.", __func__))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

selector &peer_impl::get_selector()
{
    return selector_;
}

const nentity_manager &peer_impl::get_em() const
{
    return nem_;
}

nentity_manager &peer_impl::get_em_m()
{
    return nem_;
}

persistence_manager_impl &peer_impl::get_pers_mng()
{
    return pers_mng_;
}

bool peer_impl::persistent()
{
    return pers_enabled_;
}

bool peer_impl::pers_schema_create()
{
    return pers_schema_create_;
}

vlg::synch_hash_map &peer_impl::get_srv_classid_condesc_set()
{
    return srv_sbs_nclassid_condesc_set_;
}

//-----------------------------
// CONFIG GETTERS
//-----------------------------

PeerPersonality peer_impl::get_cfg_personality()
{
    return personality_;
}

sockaddr_in peer_impl::get_cfg_srv_sock_addr()
{
    return selector_.get_srv_sock_addr();
}

unsigned int peer_impl::get_cfg_srv_pkt_q_len()
{
    return srv_pkt_q_len_;
}

unsigned int peer_impl::get_cfg_srv_exectrs()
{
    return srv_exectrs_;
}

unsigned int peer_impl::get_cfg_cli_pkt_q_len()
{
    return cli_pkt_q_len_;
}

unsigned int peer_impl::get_cfg_cli_exectrs()
{
    return cli_exectrs_;
}

unsigned int peer_impl::get_cfg_srv_sbs_evt_q_len()
{
    return srv_sbs_evt_q_len_;
}

unsigned int peer_impl::get_cfg_srv_sbs_exectrs()
{
    return srv_sbs_exectrs_;
}

bool peer_impl::get_cfg_pers_enabled()
{
    return pers_enabled_;
}

bool peer_impl::get_cfg_pers_schema_create()
{
    return pers_schema_create_;
}

bool peer_impl::get_cfg_drop_existing_schema()
{
    return drop_existing_schema_;
}

//-----------------------------
// CONFIG SETTERS
//-----------------------------

void peer_impl::set_cfg_personality(PeerPersonality personality)
{
    personality_ = personality;
}

void peer_impl::set_cfg_load_model(const char *model)
{
    if(!model_map_.contains_key(model)) {
        IFLOG(wrn(TH_ID, LS_PAR "[model already specified:%s]", model))
    } else {
        model_map_.put(model, "");
    }
}

void peer_impl::set_cfg_srv_sin_addr(const char *addr)
{
    sockaddr_in srv_sock_addr = selector_.get_srv_sock_addr();
    srv_sock_addr.sin_addr.s_addr = inet_addr(addr);
    selector_.set_srv_sock_addr(srv_sock_addr);
}

void peer_impl::set_cfg_srv_sin_port(int port)
{
    sockaddr_in srv_sock_addr = selector_.get_srv_sock_addr();
    srv_sock_addr.sin_port = htons(port);
    selector_.set_srv_sock_addr(srv_sock_addr);
}

void peer_impl::set_cfg_srv_pkt_q_len(unsigned int srv_pkt_q_len)
{
    srv_pkt_q_len_ = srv_pkt_q_len;
}

void peer_impl::set_cfg_srv_exectrs(unsigned int srv_exectrs)
{
    srv_exectrs_ = srv_exectrs;
}

void peer_impl::set_cfg_cli_pkt_q_len(unsigned int cli_pkt_q_len)
{
    cli_pkt_q_len_ = cli_pkt_q_len;
}

void peer_impl::set_cfg_cli_exectrs(unsigned int cli_exectrs)
{
    cli_exectrs_ = cli_exectrs;
}

void peer_impl::set_cfg_srv_sbs_evt_q_len(unsigned int srv_sbs_evt_q_len)
{
    srv_sbs_evt_q_len_ = srv_sbs_evt_q_len;
}

void peer_impl::set_cfg_srv_sbs_exectrs(unsigned int srv_sbs_exectrs)
{
    srv_sbs_exectrs_ = srv_sbs_exectrs;
}

void peer_impl::set_cfg_pers_enabled(bool pers_enabled)
{
    pers_enabled_ = pers_enabled;
}

void peer_impl::set_cfg_pers_schema_create(bool pers_schema_create)
{
    pers_schema_create_ = pers_schema_create;
}

void peer_impl::set_cfg_drop_existing_schema(bool drop_existing_schema)
{
    drop_existing_schema_ = drop_existing_schema;
}

void peer_impl::set_cfg_load_pers_driv(const char *driv)
{
    if(!pers_dri_load_.contains_key(driv)) {
        IFLOG(wrn(TH_ID, LS_PAR "[persistent driver already specified:%s]", driv))
    } else {
        pers_dri_load_.put(driv, "");
    }
}

vlg::p_task *peer_impl::new_peer_recv_task(connection_impl &conn,
                                           vlg_hdr_rec *pkt_hdr,
                                           vlg::grow_byte_buffer *pkt_body)
{
    mon_.lock();
    peer_recv_task *nbtsk = new peer_recv_task(++tskid_,
                                               *this,
                                               conn,
                                               pkt_body,
                                               pkt_hdr);
    /************************
    RETAIN_ID: CONN_BTH_01
    ************************/
    vlg::collector &c = conn.get_collector();
    c.retain(&conn);
    mon_.unlock();
    return nbtsk;
}

vlg::RetCode peer_impl::next_connid(unsigned int &connid)
{
    CHK_MON_ERR_0(lock)
    connid = ++prgr_conn_id_;
    CHK_MON_ERR_0(unlock)
    return vlg::RetCode_OK;
}

vlg::RetCode peer_impl::extend_model(nentity_manager *emng)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(emng:%p)", __func__, emng))
    vlg::RetCode rcode = vlg::RetCode_OK;
    if((rcode = nem_.extend(emng))) {
        IFLOG(cri(TH_ID, LS_MDL "%s(res:%d) - [failed to extend nem]", __func__,
                  rcode))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(emng:%p)", __func__, emng))
    return rcode;
}

vlg::RetCode peer_impl::extend_model(const char *model_name)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(model:%s)", __func__, model_name))
    vlg::RetCode rcode = vlg::RetCode_OK;
    if((rcode = nem_.extend(model_name))) {
        IFLOG(cri(TH_ID, LS_MDL "%s(model:%s, res:%d) - [failed to extend nem]",
                  __func__, model_name, rcode))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(model:%s)", __func__, model_name))
    return rcode;
}

connection_impl *peer_impl::vlg_conn_factory_default(peer_impl &peer,
                                                     ConnectionType con_type,
                                                     unsigned int connid,
                                                     void *ud)
{
    connection_impl *new_connection = new connection_impl(peer,
                                                          con_type,
                                                          connid);
    return new_connection;
}

vlg::RetCode peer_impl::new_connection(connection_impl **new_connection,
                                       vlg_conn_factory vlg_conn_factory_f,
                                       ConnectionType con_type,
                                       unsigned int connid,
                                       void *ud)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(new_connection:%p, con_type:%d, connid:%d)",
              __func__,
              new_connection,
              con_type,
              con_type))

    if(!new_connection) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADARG;
    }
    if(!vlg_conn_factory_f) {
        *new_connection = vlg_conn_factory_default(*this, con_type, connid, NULL);
    } else {
        *new_connection = vlg_conn_factory_f(*this, con_type, connid, ud);
    }
    if((*new_connection)) {
        (*new_connection)->init();
    } else {
        IFLOG(cri(TH_ID, LS_CLO "%s() -Memory-", __func__))
        return vlg::RetCode_MEMERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(new_connection:%p)", __func__, *new_connection))
    return vlg::RetCode_OK;
}

vlg::RetCode peer_impl::release_connection(connection_impl *connection)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode rcode = vlg::RetCode_OK;
    if(!connection) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADARG;
    }
    ConnectionStatus con_status = connection->status();
    if(con_status == ConnectionStatus_ESTABLISHED ||
            con_status == ConnectionStatus_PROTOCOL_HANDSHAKE ||
            con_status == ConnectionStatus_AUTHENTICATED) {
        IFLOG(err(TH_ID, LS_CLO
                  "%s(connection:%p) - [connection not released: state:%d]", __func__, connection,
                  con_status))
        return vlg::RetCode_KO;
    }
    vlg::collector &c = connection->get_collector();
    if((rcode = c.release(connection)) == vlg::RetCode_MEMNOTR) {
        IFLOG(err(TH_ID, LS_CLO
                  "%s(connection:%p) - [connection not released: refcount not zero]",
                  __func__,
                  connection))
        return vlg::RetCode_KO;
    }
    IFLOG(inf(TH_ID, LS_TRL "%s(connection:%p) - [connection released]", __func__,
              connection))
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return rcode;
}

vlg::RetCode peer_impl::new_incoming_connection_accept(connection_impl
                                                       &incoming_connection)
{
    return vlg::RetCode_OK;
}

vlg_conn_factory peer_impl::conn_factory() const
{
    return conn_factory_;
}

void peer_impl::set_conn_factory(vlg_conn_factory val)
{
    conn_factory_ = val;
}

void *peer_impl::conn_factory_ud() const
{
    return conn_factory_ud_;
}

void peer_impl::set_conn_factory_ud(void *val)
{
    conn_factory_ud_ = val;
}

//-----------------------------
// VLG_PEER_LFCYC HANDLERS
//-----------------------------

vlg::RetCode peer_impl::peer_load_cfg_usr(int pnum, const char *param,
                                          const char *value)
{
    if(!strcmp(param, "pure_server")) {
        if(personality_ != PeerPersonality_BOTH) {
            IFLOG(err(TH_ID, LS_PAR"[pure_server] check params."))
        }
        personality_ = PeerPersonality_PURE_SERVER;
    }
    if(!strcmp(param, "pure_client")) {
        if(personality_ != PeerPersonality_BOTH) {
            IFLOG(err(TH_ID, LS_PAR"[pure_client] check params."))
        }
        personality_ = PeerPersonality_PURE_CLIENT;
    }
    if(!strcmp(param, "load_model")) {
        if(value) {
            if(!model_map_.contains_key(value)) {
                IFLOG(err(TH_ID, LS_PAR"[load_model] model already specified:%s", value))
                return vlg::RetCode_BADCFG;
            } else {
                RETURN_IF_NOT_OK(model_map_.put(value, ""))
            }
        } else {
            IFLOG(err(TH_ID, LS_PAR"[load_model] requires argument."))
            return vlg::RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_sin_addr")) {
        if(value) {
            sockaddr_in srv_sock_addr = selector_.get_srv_sock_addr();
            srv_sock_addr.sin_addr.s_addr = inet_addr(value);
            selector_.set_srv_sock_addr(srv_sock_addr);
        } else {
            IFLOG(err(TH_ID, LS_PAR"[srv_sin_addr] requires argument."))
            return vlg::RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_sin_port")) {
        if(value) {
            sockaddr_in srv_sock_addr = selector_.get_srv_sock_addr();
            srv_sock_addr.sin_port = htons(atoi(value));
            selector_.set_srv_sock_addr(srv_sock_addr);
        } else {
            IFLOG(err(TH_ID, LS_PAR"[srv_sin_port] requires argument."))
            return vlg::RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_pkt_q_len")) {
        if(value) {
            srv_pkt_q_len_ = atoi(value);
        } else {
            IFLOG(err(TH_ID, LS_PAR"[srv_pkt_q_len] requires argument."))
            return vlg::RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_exectrs")) {
        if(value) {
            srv_exectrs_ = atoi(value);
        } else {
            IFLOG(err(TH_ID, LS_PAR"[srv_exectrs] requires argument."))
            return vlg::RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "cli_pkt_q_len")) {
        if(value) {
            cli_pkt_q_len_ = atoi(value);
        } else {
            IFLOG(err(TH_ID, LS_PAR"[cli_pkt_q_len] requires argument."))
            return vlg::RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "cli_exectrs")) {
        if(value) {
            cli_exectrs_ = atoi(value);
        } else {
            IFLOG(err(TH_ID, LS_PAR"[cli_exectrs] requires argument."))
            return vlg::RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_sbs_evt_q_len")) {
        if(value) {
            srv_sbs_evt_q_len_ = atoi(value);
        } else {
            IFLOG(err(TH_ID, LS_PAR"[srv_sbs_evt_q_len] requires argument."))
            return vlg::RetCode_BADCFG;
        }
    }
    if(!strcmp(param, "srv_sbs_exectrs")) {
        if(value) {
            srv_sbs_exectrs_ = atoi(value);
        } else {
            IFLOG(err(TH_ID, LS_PAR"[srv_sbs_exectrs] requires argument."))
            return vlg::RetCode_BADCFG;
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
            if(!pers_dri_load_.contains_key(value)) {
                IFLOG(wrn(TH_ID,
                          LS_PAR"[load_pers_driv] driver already specified:%s, skipping.", value))
                return vlg::RetCode_OK;
            } else {
                RETURN_IF_NOT_OK(pers_dri_load_.put(value, ""))
            }
        } else {
            IFLOG(err(TH_ID, LS_PAR"[load_pers_driv] requires argument."))
            return vlg::RetCode_BADCFG;
        }
    }
    return vlg::RetCode_OK;
}

vlg::RetCode peer_impl::peer_early_init_usr()
{
    IFLOG(trc(TH_ID, LS_APL"[CALLED VLG_PEER EARLYINIT HNDL]"))
    vlg::rt_init_timers();
    IFLOG(inf(TH_ID, LS_RTM "[RT-Timers initialized]"))
#ifdef WIN32
    RETURN_IF_NOT_OK(WSA_init(peer_log_))
#endif
    RETURN_IF_NOT_OK(model_map_.init(HM_SIZE_NANO))
    RETURN_IF_NOT_OK(pers_dri_load_.init(HM_SIZE_NANO))
    RETURN_IF_NOT_OK(srv_sbs_nclassid_condesc_set_.init(HM_SIZE_MIDI))
    return vlg::RetCode_OK;
}

vlg::RetCode peer_impl::peer_init_usr()
{
    IFLOG(inf(TH_ID, LS_APL"[CALLED VLG_PEER PEERINIT HNDL]"))
    return init_peer();
}

vlg::RetCode peer_impl::peer_start_usr()
{
    VLG_ASYNCH_SELECTOR_STATUS current = VLG_ASYNCH_SELECTOR_STATUS_UNDEF;
    vlg::RetCode rcode = vlg::RetCode_OK;
    IFLOG(inf(TH_ID, LS_APL"[CALLED VLG_PEER PEERSTART HNDL]"))
    IFLOG(inf(TH_ID, LS_APL"[PEER PERSONALITY: << %s >>]",
              (personality_ == PeerPersonality_BOTH) ? "BOTH" :
              (personality_ == PeerPersonality_PURE_SERVER) ? "PURE-SERVER" :
              "PURE-CLIENT"))
    if(personality_ == PeerPersonality_PURE_SERVER ||
            personality_ == PeerPersonality_BOTH) {
        vlg::PEXEC_SERVICE_STATUS current_exc_srv = vlg::PEXEC_SERVICE_STATUS_ZERO;
        if((rcode = srv_sbs_exec_serv_.start())) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - starting subscription executor service, res:%d",
                      __func__, rcode))
            return rcode;
        }
        srv_sbs_exec_serv_.await_for_status_reached_or_outdated(
            vlg::PEXEC_SERVICE_STATUS_STARTED, current_exc_srv);
    }
    //persitence driv. bgn
    if(pers_enabled_) {
        IFLOG(dbg(TH_ID, LS_TRL "%s() - starting all persistence drivers..", __func__))
        RETURN_IF_NOT_OK(pers_mng_.start_all_drivers())
        IFLOG(inf(TH_ID, LS_TRL "%s() - persistence drivers started.", __func__))
        if(pers_schema_create_) {
            vlg::RetCode db_res = vlg::RetCode_OK;
            IFLOG(dbg(TH_ID, LS_TRL "%s() - creating persistence schema..", __func__))
            db_res = pers_schema_create(drop_existing_schema_ ?
                                        PersistenceAlteringMode_DROP_IF_EXIST :
                                        PersistenceAlteringMode_CREATE_ONLY);
            if(db_res) {
                if(db_res != vlg::RetCode_DBOPFAIL) {
                    IFLOG(cri(TH_ID, LS_TRL "%s() - bad error creating pers schema, res:%d",
                              __func__, db_res))
                    return db_res;
                }
            }
            IFLOG(inf(TH_ID, LS_TRL "%s() - persistence schema created.", __func__))
        }
    }
    //persistence driv. end
    IFLOG(dbg(TH_ID, LS_TRL "%s() - start selector thread.", __func__))
    if((rcode = selector_.on_peer_start_actions())) {
        IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - selector failed to move started",
                  __func__, rcode))
        return rcode;
    }
    if((selector_.start())) {
        IFLOG(err(TH_ID, LS_CLO "%s() - selector failed to start", __func__))
        return vlg::RetCode_PTHERR;
    }
    IFLOG(dbg(TH_ID, LS_TRL "%s() - wait selector go init.", __func__))
    RETURN_IF_NOT_OK(selector_.await_for_status_reached_or_outdated(
                         VLG_ASYNCH_SELECTOR_STATUS_INIT, current,
                         VLG_INT_AWT_TIMEOUT,
                         0))
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::peer_move_running_usr()
{
    VLG_ASYNCH_SELECTOR_STATUS current = VLG_ASYNCH_SELECTOR_STATUS_UNDEF;
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[CALLED VLG_PEER MOVE RUNNING HNDL]"))
    vlg::RetCode rcode = vlg::RetCode_OK;
    if((rcode = selector_.on_peer_move_running_actions())) {
        IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - selector failed to move running",
                  __func__,
                  rcode))
        return rcode;
    }
    IFLOG(dbg(TH_ID, LS_TRL "%s() - request selector go ready.", __func__))
    selector_.set_status(VLG_ASYNCH_SELECTOR_STATUS_REQUEST_READY);
    IFLOG(dbg(TH_ID, LS_TRL "%s() - wait selector go ready.", __func__))
    RETURN_IF_NOT_OK(selector_.await_for_status_reached_or_outdated(
                         VLG_ASYNCH_SELECTOR_STATUS_READY, current,
                         VLG_INT_AWT_TIMEOUT,
                         0))
    IFLOG(dbg(TH_ID, LS_TRL "%s() - request selector go selecting.", __func__))
    selector_.set_status(VLG_ASYNCH_SELECTOR_STATUS_REQUEST_SELECT);
    return rcode;
}

vlg::RetCode peer_impl::peer_stop_usr()
{
    IFLOG(inf(TH_ID, LS_APL"[CALLED VLG_PEER PEERSTOP HNDL]"))
    vlg::RetCode rcode = vlg::RetCode_OK;
    if(selector_.inco_conn_count() || selector_.outg_conn_count()) {
        if(!force_disconnect_on_stop_) {
            IFLOG(err(TH_ID, LS_CLO "%s() - active connections detected. cannot stop peer.",
                      __func__))
            return vlg::RetCode_KO;
        }
    }
    IFLOG(dbg(TH_ID, LS_TRL "%s() - request selector to stop.", __func__))
    selector_.set_status(VLG_ASYNCH_SELECTOR_STATUS_REQUEST_STOP);
    selector_.interrupt();
    VLG_ASYNCH_SELECTOR_STATUS current = VLG_ASYNCH_SELECTOR_STATUS_UNDEF;
    selector_.await_for_status_reached_or_outdated(
        VLG_ASYNCH_SELECTOR_STATUS_STOPPED, current);
    IFLOG(dbg(TH_ID, LS_TRL "%s() - selector stopped.", __func__))
    selector_.set_status(VLG_ASYNCH_SELECTOR_STATUS_INIT);
    srv_sbs_exec_serv_.shutdown();
    srv_sbs_exec_serv_.await_termination();
    IFLOG(dbg(TH_ID, LS_TRL "%s() - srv_sbs_exec_serv_ terminated.", __func__))
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::peer_dying_breath_handler()
{
    IFLOG(inf(TH_ID,
              LS_APL"[CALLED VLG_PEER DYINGBRTH HNDL - DOING NO ACTIONS - (OVERRIDE IF NEEDED)]"))
    return vlg::RetCode_OK;
}

//-----------------------------
// PROTOCOL INTERFACE
//-----------------------------
vlg::RetCode peer_impl::recv_and_route_pkt(connection_impl &conn,
                                           vlg_hdr_rec *hdr,
                                           vlg::grow_byte_buffer *body)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, hdr:%p, body:%p)", __func__,
              conn.get_socket(), hdr, body))
    vlg::RetCode rcode = vlg::RetCode_OK;
    switch(hdr->phdr.pkttyp) {
        case VLG_PKT_TSTREQ_ID:
            /*TEST REQUEST******************************************************************/
            rcode = conn.recv_test_request(hdr);
            break;
        case VLG_PKT_CONREQ_ID:
            /*CONNECTION REQUEST************************************************************/
            rcode = conn.recv_connection_request(hdr);
            break;
        case VLG_PKT_CONRES_ID:
            /*CONNECTION RESPONSE***********************************************************/
            rcode = conn.recv_connection_response(hdr);
            break;
        case VLG_PKT_DSCOND_ID:
            /*DISCONNECTED******************************************************************/
            rcode = conn.recv_disconnection(hdr);
            break;
        case VLG_PKT_TXRQST_ID:
            /*TRANSACTION REQUEST***********************************************************/
            rcode = conn.recv_tx_req(hdr, body);
            break;
        case VLG_PKT_TXRESP_ID:
            /*TRANSACTION RESPONSE**********************************************************/
            rcode = conn.recv_tx_res(hdr, body);
            break;
        case VLG_PKT_SBSREQ_ID:
            /*SUBSCRIPTION REQUEST**********************************************************/
            rcode = conn.recv_sbs_start_req(hdr);
            break;
        case VLG_PKT_SBSRES_ID:
            /*SUBSCRIPTION RESPONSE*********************************************************/
            rcode = conn.recv_sbs_start_res(hdr);
            break;
        case VLG_PKT_SBSEVT_ID:
            /*SUBSCRIPTION EVENT************************************************************/
            rcode = conn.recv_sbs_evt(hdr, body);
            break;
        case VLG_PKT_SBSACK_ID:
            /*SUBSCRIPTION EVENT ACK********************************************************/
            rcode = conn.recv_sbs_evt_ack(hdr);
            break;
        case VLG_PKT_SBSTOP_ID:
            /*SUBSCRIPTION STOP REQUEST*****************************************************/
            rcode = conn.recv_sbs_stop_req(hdr);
            break;
        case VLG_PKT_SBSSPR_ID:
            /*SUBSCRIPTION STOP RESPONSE****************************************************/
            rcode = conn.recv_sbs_stop_res(hdr);
            break;
        default:
            break;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//-----------------------------
// MODEL MNG ************
//-----------------------------

per_nclassid_helper_rec::per_nclassid_helper_rec() :
    sbsevtid_(0),
    ts0_(0),
    ts1_(0),
    srv_connid_condesc_set_(vlg::sngl_ptr_obj_mng(), sizeof(unsigned int))
{}

per_nclassid_helper_rec::~per_nclassid_helper_rec()
{}

vlg::RetCode per_nclassid_helper_rec::init()
{
    RETURN_IF_NOT_OK(srv_connid_condesc_set_.init(HM_SIZE_SMALL))
    return vlg::RetCode_OK;
}

vlg::synch_hash_map &per_nclassid_helper_rec::get_srv_connid_condesc_set()
{
    return srv_connid_condesc_set_;
}

unsigned int per_nclassid_helper_rec::next_sbs_evt_id()
{
    mon_.lock();
    sbsevtid_++;
    mon_.unlock();
    return sbsevtid_;
}

void per_nclassid_helper_rec::next_time_stamp(unsigned int &ts0,
                                              unsigned int &ts1)
{
    mon_.lock();
    ts0 = (unsigned int)time(NULL);
    if(ts0 == ts0_) {
        ts1 = ++ts1_;
    } else {
        ts0_ = ts0;
        ts1 = ts1_ = 0;
    }
    mon_.unlock();
}

vlg::RetCode peer_impl::add_subscriber(subscription_impl *sbsdesc)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode rcode = vlg::RetCode_OK;
    unsigned int nclsid = sbsdesc->nclass_id();
    per_nclassid_helper_rec *sdrec = NULL;
    if((rcode = srv_sbs_nclassid_condesc_set_.get(&nclsid, &sdrec))) {
        if(!(sdrec = new per_nclassid_helper_rec())) {
            IFLOG(cri(TH_ID, LS_CLO "%s - memory", __func__))
            return vlg::RetCode_MEMERR;
        }
        RETURN_IF_NOT_OK(sdrec->init())
        RETURN_IF_NOT_OK(srv_sbs_nclassid_condesc_set_.put(&nclsid, &sdrec))
    }
    connection_impl *conn = &sbsdesc->conn_;
    RETURN_IF_NOT_OK(sdrec->get_srv_connid_condesc_set().put(
                         &sbsdesc->conn_.connid_, &conn))
    IFLOG(dbg(TH_ID, LS_SBS
              "%s() - [added subscriber: connid:%d for nclass_id:%d]",
              __func__, sbsdesc->conn_.connid_,
              nclsid))
    rcode = vlg::RetCode_OK;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::remove_subscriber(subscription_impl *sbsdesc)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode rcode = vlg::RetCode_OK;
    unsigned int nclsid = sbsdesc->nclass_id();
    per_nclassid_helper_rec *sdrec = NULL;
    if((rcode = srv_sbs_nclassid_condesc_set_.get(&nclsid, &sdrec))) {
        IFLOG(cri(TH_ID, LS_CLO
                  "%s() - [subscriber: connid:%d not found for nclass_id:%d, hash srv_sbs_clssid_condesc_set_]",
                  __func__,
                  sbsdesc->conn_.connid_, nclsid))
        return vlg::RetCode_GENERR;
    }
    if((rcode = sdrec->get_srv_connid_condesc_set().remove(
                    &sbsdesc->conn_.connid_, NULL))) {
        IFLOG(cri(TH_ID, LS_CLO
                  "%s() - [subscriber: connid:%d not found for nclass_id:%d, hash srv_connid_condesc_set]",
                  __func__,
                  sbsdesc->conn_.connid_, nclsid))
        return vlg::RetCode_GENERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//-----------------------------
// VLG_PEER_SBS_TASK
/*
This task is used in srv_sbs_exec_serv_ executor service, and it is used
to perform a server-side subscription task, tipically this means that
a subscription event has been triggered and it must be delivered to all
subscriptors.
*/
//-----------------------------
class peer_sbs_task;

struct srv_connid_condesc_set_ashsnd_rud {
    unsigned int    nclass_id;
    peer_sbs_task   *tsk;
    vlg::RetCode            rcode;
};

class peer_sbs_task : public vlg::p_task {
    public:
        //---ctor
        peer_sbs_task(unsigned int id,
                      peer_impl &peer,
                      subscription_event_impl &sbs_evt,
                      vlg::synch_hash_map &srv_connid_condesc_set) :
            vlg::p_task(id),
            peer_(peer),
            sbs_evt_(sbs_evt),
            srv_connid_connection_map_(srv_connid_condesc_set) {
            log_ = vlg::logger::get_logger("peer_sbs_task");
            IFLOG(trc(TH_ID, LS_CTR "%s(taskid:%d)", __func__, id))
        }

        ~peer_sbs_task() {
            IFLOG(trc(TH_ID, LS_DTR "%s(taskid:%d)", __func__, get_id()))
        }

        static void enum_srv_connid_connection_map_ashsnd(const vlg::synch_hash_map
                                                          &map,
                                                          const void      *key,
                                                          const void      *ptr,
                                                          void            *ud) {
            vlg::RetCode rcode = vlg::RetCode_OK;
            srv_connid_condesc_set_ashsnd_rud *rud =
                static_cast<srv_connid_condesc_set_ashsnd_rud *>(ud);
            connection_impl *conn = *(connection_impl **)ptr;
            subscription_impl *sbs_desc = NULL;
            if(conn->class_id_sbs_map().get(&rud->nclass_id, &sbs_desc)) {
                IFLOG(wrn(TH_ID, LS_EXE "%s() - [no more active sbs on connection:%u]",
                          __func__, conn->connid()))
                return;
            }
            IFLOG(trc(TH_ID, LS_EXE "%s() - [connid:%u, conndesc:%p, sbsdesc:%p]", __func__,
                      conn->connid(), conn,
                      sbs_desc))
            if((rcode = sbs_desc->submit_live_event(&rud->tsk->sbs_evt_))) {
                IFLOG(err(TH_ID, LS_EXE
                          "%s() - [sbs task:%d - asynch event send failed for sbsid:%d - res:%d]",
                          __func__,
                          rud->tsk->get_id(),
                          sbs_desc->sbsid(),
                          rcode))
                rud->rcode = rcode;
            }
        }

        /*this method will be called when this task will be run by an executor*/
        virtual vlg::RetCode execute() {
            vlg::RetCode rcode = vlg::RetCode_OK;
            vlg::collector &c = sbs_evt_.get_collector();
            /*
            External Event Adoption.
            This sbs_evt can eventually remain live in one or more
            subscriptions, until it is released by them.
            */
            /************************
            RETAIN_ID: SBE_SRV_01
            ************************/
            c.retain(&sbs_evt_);
            srv_connid_condesc_set_ashsnd_rud rud;
            rud.nclass_id = sbs_evt_.get_obj()->get_nclass_id();
            rud.tsk = this;
            rud.rcode = vlg::RetCode_OK;
            srv_connid_connection_map_.enum_elements_safe_read(
                enum_srv_connid_connection_map_ashsnd, &rud);
            if((rud.rcode)) {
                IFLOG(dbg(TH_ID, LS_EXE "%s() - [sbs task:%d - execution failed - res:%d]",
                          __func__, get_id(), rcode))
            } else {
                IFLOG(trc(TH_ID, LS_EXE "%s() - [sbs task:%d - executed]", __func__, get_id()))
            }
            /*
            External Event Release and Destroy.
            */
            /************************
            RELEASE_ID: SBE_SRV_01
            ************************/
            c.release(&sbs_evt_);
            return rcode;
        }

        peer_impl &get_peer() {
            return peer_;
        }

    private:
        peer_impl &peer_;
        subscription_event_impl &sbs_evt_;
        vlg::synch_hash_map
        &srv_connid_connection_map_;  //connid --> condesc [uint --> ptr]
    protected:
        static vlg::logger   *log_;
};

vlg::logger *peer_sbs_task::log_ = NULL;

vlg::p_task *peer_impl::new_sbs_evt_task(
    subscription_event_impl &sbs_evt,
    vlg::synch_hash_map &srv_connid_connection_map)
{
    mon_.lock();
    peer_sbs_task *nbtsk = new peer_sbs_task(++tskid_,
                                             *this,
                                             sbs_evt,
                                             srv_connid_connection_map);
    mon_.unlock();
    return nbtsk;
}

vlg::RetCode peer_impl::get_per_classid_helper_class(unsigned int nclass_id,
                                                     per_nclassid_helper_rec **out)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode rcode = vlg::RetCode_OK;
    per_nclassid_helper_rec *sdrec = NULL;
    if((rcode = srv_sbs_nclassid_condesc_set_.get(&nclass_id, &sdrec))) {
        if(!(sdrec = new per_nclassid_helper_rec())) {
            IFLOG(cri(TH_ID, LS_CLO "%s - memory", __func__))
            return vlg::RetCode_MEMERR;
        }
        if((rcode = sdrec->init())) {
            IFLOG(cri(TH_ID, LS_CLO "%s - init", __func__))
            return rcode;
        }
        if((rcode = srv_sbs_nclassid_condesc_set_.put(&nclass_id, &sdrec))) {
            IFLOG(cri(TH_ID, LS_CLO "%s - put", __func__))
            return rcode;
        }
    }
    *out = sdrec;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::build_sbs_event(unsigned int evt_id,
                                        SubscriptionEventType sbs_evttype,
                                        ProtocolCode sbs_protocode,
                                        unsigned int sbs_tmstp0,
                                        unsigned int sbs_tmstp1,
                                        Action sbs_act,
                                        const nclass *sbs_data,
                                        subscription_event_impl **new_sbs_event)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode rcode = vlg::RetCode_OK;
    if((subscription_event_impl::new_instance(new_sbs_event))) {
        IFLOG(cri(TH_ID, LS_CLO "%s - memory", __func__))
        return vlg::RetCode_MEMERR;
    }
    (*new_sbs_event)->set_evtid(evt_id);
    (*new_sbs_event)->set_evttype(sbs_evttype);
    (*new_sbs_event)->set_protocode(sbs_protocode);
    (*new_sbs_event)->set_tmstp0(sbs_tmstp0);
    (*new_sbs_event)->set_tmstp1(sbs_tmstp1);
    (*new_sbs_event)->set_act(sbs_act);
    (*new_sbs_event)->set_obj(sbs_data);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::submit_sbs_evt_task(subscription_event_impl &sbs_evt,
                                            vlg::synch_hash_map &srv_connid_condesc_set)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode rcode = vlg::RetCode_OK;
    vlg::p_task *sbs_tsk = new_sbs_evt_task(sbs_evt, srv_connid_condesc_set);
    if(!sbs_tsk) {
        IFLOG(cri(TH_ID, LS_CLO "%s - memory", __func__))
        return vlg::RetCode_MEMERR;
    }
    if((rcode = srv_sbs_exec_serv_.submit(sbs_tsk))) {
        IFLOG(cri(TH_ID, LS_TRL "%s() - submit failed, res:%d.", __func__, rcode))
    } else {
        IFLOG(dbg(TH_ID, LS_TRL "%s() - task sent.", __func__))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//-----------------------------
// #VER#
//-----------------------------
const char *PEERLIB_VER(void)
{
    static char str[] = "lib.vlg.ver.0.0.0.date:" __DATE__;
    return str;
}

}

