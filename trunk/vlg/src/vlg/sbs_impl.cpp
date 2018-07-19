/*
* vulgaris
* (C) 2018 - giuseppe.baccini@gmail.com
*
*/

#include "pr_impl.h"
#include "conn_impl.h"
#include "sbs_impl.h"

namespace vlg {

struct se_std_shared_ptr_obj_mng : public std_shared_ptr_obj_mng<subscription_event> {

    static int se_cmp_obj(const void *obj1, const void *obj2, size_t len) {
        std::shared_ptr<subscription_event> &sh_se_1 = *(std::shared_ptr<subscription_event> *)(obj1);
        std::shared_ptr<subscription_event> &sh_se_2 = *(std::shared_ptr<subscription_event> *)(obj2);
        std::unique_ptr<char> kv1, kv2;
        sh_se_1->get_data()->get_primary_key_value_as_string(kv1);
        sh_se_2->get_data()->get_primary_key_value_as_string(kv2);
        return strcmp(kv1.get(), kv2.get());
    }

    static void se_hash_obj(const void *key, int len, uint32_t seed, void *out) {
        std::shared_ptr<subscription_event> &sh_se = *(std::shared_ptr<subscription_event> *)(key);
        std::unique_ptr<char> kv;
        sh_se->get_data()->get_primary_key_value_as_string(kv);
        MurmurHash3_x86_32(key, (int)strlen(kv.get()), seed, out);
    }

    explicit se_std_shared_ptr_obj_mng() : std_shared_ptr_obj_mng<subscription_event>(0,
                                                                                          shared_ptr_alloc_func,
                                                                                          shared_ptr_dealloc_func,
                                                                                          se_cmp_obj,
                                                                                          shared_ptr_cpy_func,
                                                                                          se_hash_obj) {}
};

const se_std_shared_ptr_obj_mng se_std_shp_omng;

}

namespace vlg {

const std_shared_ptr_obj_mng<incoming_subscription> sbs_std_shp_omng;

}

namespace vlg {

//subscription_event_impl

subscription_event_impl::subscription_event_impl(unsigned int sbsid,
                                                 unsigned int evtid,
                                                 SubscriptionEventType set,
                                                 ProtocolCode pc,
                                                 unsigned int ts0,
                                                 unsigned int ts1,
                                                 Action act,
                                                 const nclass &data) :
    sbs_sbsid_(sbsid),
    sbs_evtid_(evtid),
    sbs_evttype_(set),
    sbs_protocode_(pc),
    sbs_tmstp0_(ts0),
    sbs_tmstp1_(ts1),
    sbs_act_(act),
    sbs_data_(data.clone())
{}

subscription_event_impl::subscription_event_impl(unsigned int sbsid,
                                                 unsigned int evtid,
                                                 SubscriptionEventType set,
                                                 ProtocolCode pc,
                                                 unsigned int ts0,
                                                 unsigned int ts1,
                                                 Action act,
                                                 std::unique_ptr<nclass> &data) :
    sbs_sbsid_(sbsid),
    sbs_evtid_(evtid),
    sbs_evttype_(set),
    sbs_protocode_(pc),
    sbs_tmstp0_(ts0),
    sbs_tmstp1_(ts1),
    sbs_act_(act),
    sbs_data_(std::move(data))
{}

sbs_impl::sbs_impl(incoming_subscription &publ, incoming_connection &conn) :
    conn_(conn.impl_.get()),
    sbsid_(0),
    reqid_(0),
    status_(SubscriptionStatus_EARLY),
    start_stop_evt_occur_(false),
    sbstyp_(SubscriptionType_UNDEFINED),
    sbsmod_(SubscriptionMode_UNDEFINED),
    flotyp_(SubscriptionFlowType_UNDEFINED),
    dwltyp_(SubscriptionDownloadType_UNDEFINED),
    enctyp_(Encode_UNDEFINED),
    nclassid_(0),
    open_tmstp0_(0),
    open_tmstp1_(0),
    sbresl_(SubscriptionResponse_UNDEFINED),
    last_vlgcod_(ProtocolCode_SUCCESS),
    ipubl_(&publ),
    opubl_(nullptr)
{}

sbs_impl::sbs_impl(outgoing_subscription &publ) :
    conn_(nullptr),
    sbsid_(0),
    reqid_(0),
    status_(SubscriptionStatus_EARLY),
    start_stop_evt_occur_(false),
    sbstyp_(SubscriptionType_UNDEFINED),
    sbsmod_(SubscriptionMode_UNDEFINED),
    flotyp_(SubscriptionFlowType_UNDEFINED),
    dwltyp_(SubscriptionDownloadType_UNDEFINED),
    enctyp_(Encode_UNDEFINED),
    nclassid_(0),
    open_tmstp0_(0),
    open_tmstp1_(0),
    sbresl_(SubscriptionResponse_UNDEFINED),
    last_vlgcod_(ProtocolCode_SUCCESS),
    ipubl_(nullptr),
    opubl_(&publ)
{}

RetCode sbs_impl::set_started()
{
    IFLOG(inf(TH_ID, LS_SBS"[CONNID:%u-SBSID:%u][started]", conn_->connid_, sbsid_))
    set_status(SubscriptionStatus_STARTED);
    if(opubl_) {
        opubl_->on_start();
    }
    return RetCode_OK;
}

RetCode sbs_impl::set_stopped()
{
    IFLOG(inf(TH_ID, LS_SBS"[CONNID:%u-SBSID:%u][stopped]", conn_->connid_, sbsid_))
    set_status(SubscriptionStatus_STOPPED);
    if(ipubl_) {
        ipubl_->on_stop();
    } else {
        opubl_->on_stop();
    }
    return RetCode_OK;
}

RetCode sbs_impl::set_released()
{
    IFLOG(dbg(TH_ID, LS_SBS"[CONNID:%u-SBSID:%u][released]", conn_->connid_, sbsid_))
    set_status(SubscriptionStatus_RELEASED);
    return RetCode_OK;
}

RetCode sbs_impl::set_error()
{
    IFLOG(err(TH_ID, LS_SBS"[CONNID:%u-SBSID:%u][error]", conn_->connid_, sbsid_))
    set_status(SubscriptionStatus_ERROR);
    if(ipubl_) {
        ipubl_->on_stop();
    } else {
        opubl_->on_stop();
    }
    return RetCode_OK;
}

inline RetCode sbs_impl::set_status(SubscriptionStatus status)
{
    scoped_mx smx(mon_);
    status_ = status;
    if(ipubl_) {
        ipubl_->on_status_change(status_);
    } else {
        opubl_->on_status_change(status_);
    }
    mon_.notify_all();
    return RetCode_OK;
}

RetCode sbs_impl::await_for_status_reached(SubscriptionStatus test,
                                           SubscriptionStatus &current,
                                           time_t sec,
                                           long nsec)
{
    scoped_mx smx(mon_);
    if(status_ < SubscriptionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(status_ < test) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  RetCode_TIMEOUT;
                break;
            }
        }
    }
    current = status_;
    IFLOG(log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO "test:%d [reached] current:%d",
              __func__,
              test,
              status_))
    return rcode;
}

RetCode sbs_impl::await_for_start_result(SubscriptionResponse &sbs_start_result,
                                         ProtocolCode &sbs_start_protocode,
                                         time_t sec,
                                         long nsec)
{
    scoped_mx smx(mon_);
    if(status_ < SubscriptionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(!start_stop_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  RetCode_TIMEOUT;
                break;
            }
        }
    }
    sbs_start_result = sbresl_;
    sbs_start_protocode = last_vlgcod_;
    IFLOG(log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO
              "[sbsid:%d, res:%d, status:%d][incoming_subscription start result available][sbs_start_result:%d, sbs_start_protocode:%d]",
              __func__,
              sbsid_,
              rcode,
              status_,
              sbresl_,
              last_vlgcod_))
    start_stop_evt_occur_ = false;
    return rcode;
}

RetCode sbs_impl::await_for_stop_result(SubscriptionResponse &sbs_stop_result,
                                        ProtocolCode &sbs_stop_protocode,
                                        time_t sec,
                                        long nsec)
{
    scoped_mx smx(mon_);
    if(status_ < SubscriptionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(!start_stop_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  RetCode_TIMEOUT;
                break;
            }
        }
    }
    sbs_stop_result = sbresl_;
    sbs_stop_protocode = last_vlgcod_;
    IFLOG(log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO
              "[sbsid:%d, res:%d, status:%d][incoming_subscription stop result available][sbs_stop_result:%d, sbs_stop_protocode:%d]",
              __func__,
              sbsid_,
              rcode,
              status_,
              sbresl_,
              last_vlgcod_))
    start_stop_evt_occur_ = false;
    return rcode;
}

RetCode sbs_impl::notify_for_start_stop_result()
{
    scoped_mx smx(mon_);
    start_stop_evt_occur_ = true;
    mon_.notify_all();
    return RetCode_OK;
}

RetCode sbs_impl::stop()
{
    if(status_ != SubscriptionStatus_STARTED) {
        IFLOG(err(TH_ID, LS_CLO "[status:%d]", __func__, status_))
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    start_stop_evt_occur_ = false;
    g_bbuf *gbb = new g_bbuf();
    build_PKT_SBSTOP(sbsid_, gbb);
    gbb->flip();
    RET_ON_KO(conn_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_);
    rcode = conn_->peer_->selector_.asynch_notify(evt);
    if(rcode) {
        set_status(SubscriptionStatus_ERROR);
    }
    return rcode;
}

//incoming_subscription_impl

incoming_subscription_impl::incoming_subscription_impl(incoming_subscription &publ,
                                                       std::shared_ptr<incoming_connection> &conn) :
    sbs_impl(publ, *conn),
    conn_sh_(conn),
    initial_query_(nullptr),
    initial_query_ended_(true)
{
    set_status(SubscriptionStatus_INITIALIZED);
}

incoming_subscription_impl::~incoming_subscription_impl()
{
    release_initial_query();
}

inline void incoming_subscription_impl::release_initial_query()
{
    if(!initial_query_ended_) {
        if(initial_query_) {
            initial_query_->release();
        }
        initial_query_ = nullptr;
        initial_query_ended_ = true;
    }
}

RetCode incoming_subscription_impl::send_start_response()
{
    RetCode rcode = RetCode_OK;
    g_bbuf *gbb = new g_bbuf();
    build_PKT_SBSRES(sbresl_,
                     last_vlgcod_,
                     reqid_,
                     sbsid_,
                     gbb);
    gbb->flip();
    RET_ON_KO(conn_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_sh_);
    rcode = conn_->peer_->selector_.asynch_notify(evt);
    if(rcode) {
        set_status(SubscriptionStatus_ERROR);
    }
    return rcode;
}

RetCode incoming_subscription_impl::send_event(std::shared_ptr<subscription_event> &sbs_evt)
{
    RetCode rcode = RetCode_OK;
    g_bbuf *gbb = new g_bbuf();
    build_PKT_SBSEVT(sbsid_,
                     sbs_evt->impl_->sbs_evttype_,
                     sbs_evt->impl_->sbs_act_,
                     sbs_evt->impl_->sbs_protocode_,
                     sbs_evt->impl_->sbs_evtid_,
                     sbs_evt->impl_->sbs_tmstp0_,
                     sbs_evt->impl_->sbs_tmstp1_,
                     gbb);

    long totbytes = sbs_evt->impl_->sbs_data_ ?
                    sbs_evt->impl_->sbs_data_->serialize(enctyp_, nullptr, gbb) :
                    (long)gbb->position();

    totbytes = htonl(totbytes);
    if(sbs_evt->impl_->sbs_data_) {
        gbb->put(&totbytes, (6*4), 4);
    }
    gbb->flip();
    RET_ON_KO(conn_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_sh_);
    rcode = conn_->peer_->selector_.asynch_notify(evt);
    if(rcode) {
        set_status(SubscriptionStatus_ERROR);
    }
    return rcode;
}

RetCode incoming_subscription_impl::submit_live_event(std::shared_ptr<subscription_event> &sbs_evt)
{
    RetCode rcode = RetCode_OK;
    if((rcode = ipubl_->accept_distribution(*sbs_evt))) {
        IFLOG(trc(TH_ID, LS_TRL"[unauthorized event:%d skipped for subscriber:%d]",
                  __func__, sbs_evt->impl_->sbs_evtid_, sbsid_))
    } else {
        if(initial_query_ended_) {
            //immediate sending
            if((rcode = send_event(sbs_evt))) {
                IFLOG(err(TH_ID, LS_TRL "[new live event:%d failed to send]", __func__, sbs_evt->impl_->sbs_evtid_))
            }
        }
    }
    return rcode;
}

RetCode incoming_subscription_impl::submit_dwnl_event()
{
    RetCode rcode = RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    subscription_event_impl *sbs_dwnl_evt_impl = nullptr;
    std::unique_ptr<nclass> dwnl_obj;
    unsigned int ts0 = 0, ts1 = 0;
    if((rcode = conn_->peer_->get_per_nclassid_helper_rec(nclassid_, &sdr))) {
        IFLOG(cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
        return rcode;
    }

    //we need to new instance here because we do not know if query has ended here.
    conn_->peer_->nem_.new_nclass_instance(nclassid_, dwnl_obj);

    if((rcode = initial_query_->load_next_entity(ts0, ts1, *dwnl_obj)) == RetCode_DBROW) {
        sbs_dwnl_evt_impl = new subscription_event_impl(sbsid_,
                                                        sdr->next_sbs_evt_id(),
                                                        SubscriptionEventType_DOWNLOAD,
                                                        ProtocolCode_SUCCESS,
                                                        ts0,
                                                        ts1,
                                                        Action_INSERT,
                                                        dwnl_obj);
        RetCode rrcode = RetCode_OK;
        std::shared_ptr<subscription_event> sbs_evt(new subscription_event(*sbs_dwnl_evt_impl));
        if((rrcode = ipubl_->accept_distribution(*sbs_evt))) {
            IFLOG(trc(TH_ID, LS_TRL "[unauthorized event:%d skipped for subscriber%d]",
                      __func__, sbs_dwnl_evt_impl->sbs_evtid_, sbsid_))
        } else {
            IFLOG(trc(TH_ID, LS_TRL "[send new download event:%d]",
                      __func__, sbs_dwnl_evt_impl->sbs_evtid_))

            if((rrcode = send_event(sbs_evt))) {
                IFLOG(err(TH_ID, LS_TRL "[new download event:%d failed to send]",
                          __func__, sbs_dwnl_evt_impl->sbs_evtid_))
            }
        }
    } else if(rcode == RetCode_QRYEND) {
        RetCode rrcode = RetCode_OK;
        dwnl_obj.release();
        sbs_dwnl_evt_impl = new subscription_event_impl(sbsid_,
                                                        sdr->next_sbs_evt_id(),
                                                        SubscriptionEventType_DOWNLOAD_END,
                                                        ProtocolCode_SUCCESS,
                                                        0,
                                                        0,
                                                        Action_NONE,
                                                        dwnl_obj);
        std::shared_ptr<subscription_event> sbs_evt(new subscription_event(*sbs_dwnl_evt_impl));
        if((rrcode = send_event(sbs_evt))) {
            IFLOG(err(TH_ID, LS_TRL "[download-end event:%d failed to send]", __func__, sbs_dwnl_evt_impl->sbs_evtid_))
        }
        if(!(rrcode = initial_query_->release())) {
            IFLOG(dbg(TH_ID, LS_TRL "[download query released]", __func__))
        } else {
            IFLOG(cri(TH_ID, LS_TRL "[download query releasing failed][res:%d]", __func__, rcode))
        }
        initial_query_ = nullptr;
        initial_query_ended_ = true;
    } else {
        IFLOG(cri(TH_ID, LS_TRL "[download query failed][res:%d]", __func__, rcode))
    }
    return rcode;
}

RetCode incoming_subscription_impl::execute_initial_query()
{
    RetCode rcode = RetCode_OK;
    const nentity_desc *nclass_desc = conn_->peer_->nem_.get_nentity_descriptor(nclassid_);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = conn_->peer_->pers_mng_.available_driver(nclassid_))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclassid_))) {
                    std::stringstream ss;
                    ss << "select * from " << nclass_desc->get_nentity_name();
                    if(dwltyp_ == SubscriptionDownloadType_PARTIAL) {
                        ss << " where (" P_F_TS0" = "
                           << open_tmstp0_
                           << " and " P_F_TS1" > "
                           << open_tmstp1_
                           << ") or (" P_F_TS0" > "
                           << open_tmstp0_
                           << ")";
                    }
                    ss << ";";
                    rcode = conn->execute_query(ss.str().c_str(), conn_->peer_->nem_, initial_query_);
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%u]", __func__, nclassid_))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%u]", __func__, nclassid_))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclassid_))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(cri(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclassid_))
    }
    if(rcode) {
        initial_query_ended_ = true;
        if(initial_query_) {
            initial_query_->release();
        }
        initial_query_ = nullptr;
        IFLOG(err(TH_ID, LS_CLO "[sbsid:%d - download query FAILED][res:%d].", __func__, sbsid_, rcode))
    }
    return rcode;
}

}

namespace vlg {

//outgoing_subscription_impl

outgoing_subscription_impl::outgoing_subscription_impl(outgoing_subscription &publ) :
    sbs_impl(publ)
{
    set_status(SubscriptionStatus_INITIALIZED);
}

outgoing_subscription_impl::~outgoing_subscription_impl()
{
    if(status_ == SubscriptionStatus_REQUEST_SENT ||
            status_ == SubscriptionStatus_STARTED) {
        IFLOG(wrn(TH_ID, LS_DTR
                  "[subscription:%d in status:%d, stopping..]",
                  __func__,
                  sbsid_,
                  status_))
        stop();
        SubscriptionResponse sres = SubscriptionResponse_UNDEFINED;
        ProtocolCode spc = ProtocolCode_UNDEFINED;
        await_for_stop_result(sres, spc);
    }
    auto &oconn = *dynamic_cast<outgoing_connection_impl *>(conn_);
    oconn.release_subscription(this);
}

RetCode outgoing_subscription_impl::set_req_sent()
{
    IFLOG(inf(TH_ID,
              LS_SBO"[CONNID:%u-REQID:%u][SBSTYP:%d, SBSMOD:%d, FLOTYP:%d, DWLTYP:%d, ENCTYP:%d, NCLSSID:%d, TMSTP0:%u, TMSTP1:%u]",
              conn_->connid_,
              reqid_,
              sbstyp_,
              sbsmod_,
              flotyp_,
              dwltyp_,
              enctyp_,
              nclassid_,
              open_tmstp0_,
              open_tmstp1_))
    set_status(SubscriptionStatus_REQUEST_SENT);
    return RetCode_OK;
}

RetCode outgoing_subscription_impl::start()
{
    if(status_ != SubscriptionStatus_INITIALIZED && status_ != SubscriptionStatus_STOPPED) {
        IFLOG(err(TH_ID, LS_CLO "[status:%d]", __func__, status_))
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    auto &oconn = *dynamic_cast<outgoing_connection_impl *>(conn_);
    reqid_ = oconn.next_reqid();
    outgoing_subscription_impl *self = this;
    rcode = oconn.outg_reqid_sbs_map_.put(&reqid_, &self);
    if((rcode = send_start_request())) {
        IFLOG(err(TH_ID, LS_TRL "[send request failed][res:%d]", __func__, rcode))
    }
    return rcode;
}

RetCode outgoing_subscription_impl::start(SubscriptionType sbscr_type,
                                          SubscriptionMode sbscr_mode,
                                          SubscriptionFlowType sbscr_flow_type,
                                          SubscriptionDownloadType  sbscr_dwnld_type,
                                          Encode sbscr_nclass_encode,
                                          unsigned int nclass_id,
                                          unsigned int start_timestamp_0,
                                          unsigned int start_timestamp_1)
{
    IFLOG(trc(TH_ID,
              LS_OPN"[sbsid:%d, sbscr_type:%d, sbscr_mode:%d, sbscr_flow_type:%d, sbscr_dwnld_type:%d, sbscr_nclass_encode:%d, nclass_id:%u, ts0:%u, ts1:%u]",
              __func__,
              sbsid_,
              sbscr_type,
              sbscr_mode,
              sbscr_flow_type,
              sbscr_dwnld_type,
              sbscr_nclass_encode,
              nclass_id,
              start_timestamp_0,
              start_timestamp_1))
    if(status_ != SubscriptionStatus_INITIALIZED && status_ != SubscriptionStatus_STOPPED) {
        IFLOG(err(TH_ID, LS_CLO "[status:%d]", __func__, status_))
        return RetCode_BADSTTS;
    }
    sbstyp_ = sbscr_type;
    sbsmod_ = sbscr_mode;
    flotyp_ = sbscr_flow_type;
    dwltyp_ = sbscr_dwnld_type;
    enctyp_ = sbscr_nclass_encode;
    nclassid_ = nclass_id;
    open_tmstp0_ = start_timestamp_0;
    open_tmstp1_ = start_timestamp_1;
    return start();
}

RetCode outgoing_subscription_impl::send_start_request()
{
    RetCode rcode = RetCode_OK;
    set_req_sent();
    start_stop_evt_occur_ = false;
    g_bbuf *gbb = new g_bbuf();
    build_PKT_SBSREQ(sbstyp_,
                     sbsmod_,
                     flotyp_,
                     dwltyp_,
                     enctyp_,
                     nclassid_,
                     conn_->connid_,
                     reqid_,
                     open_tmstp0_,
                     open_tmstp1_,
                     gbb);
    gbb->flip();
    RET_ON_KO(conn_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_);
    if((rcode = conn_->peer_->selector_.asynch_notify(evt))) {
        set_status(SubscriptionStatus_ERROR);
    }
    return rcode;
}

RetCode outgoing_subscription_impl::receive_event(const vlg_hdr_rec *pkt_hdr,
                                                  g_bbuf *pkt_body)
{
    std::unique_ptr<nclass> nobj;
    if(pkt_hdr->row_2.sevttp.sevttp != SubscriptionEventType_DOWNLOAD_END) {
        RetCode rcode = RetCode_OK;
        if((rcode = conn_->peer_->nem_.new_nclass_instance(nclassid_, nobj))) {
            IFLOG(cri(TH_ID, LS_SBS"[incoming_subscription event receive failed: new_nclass_instance fail:%d, nclass_id:%d]",
                      rcode,
                      nclassid_))
            return rcode;
        }
        if((rcode = nobj->restore(&conn_->peer_->nem_, enctyp_, pkt_body))) {
            IFLOG(cri(TH_ID, LS_SBS"[incoming_subscription event receive failed: nclass restore fail:%d, nclass_id:%d]",
                      rcode,
                      nclassid_))
            return rcode;
        } else {
            IFLOG(inf_nclass(TH_ID, nobj.get(), true, LS_SBI"[ACT:%d] ", pkt_hdr->row_2.sevttp.sbeact))
        }
    }

    std::unique_ptr<subscription_event> sbs_evt(new subscription_event(*new subscription_event_impl(
                                                                           pkt_hdr->row_1.sbsrid.sbsrid,
                                                                           pkt_hdr->row_3.sevtid.sevtid,
                                                                           pkt_hdr->row_2.sevttp.sevttp,
                                                                           pkt_hdr->row_2.sevttp.vlgcod,
                                                                           pkt_hdr->row_4.tmstmp.tmstmp,
                                                                           pkt_hdr->row_5.tmstmp.tmstmp,
                                                                           pkt_hdr->row_2.sevttp.sbeact,
                                                                           nobj)));
    opubl_->on_incoming_event(sbs_evt);
    return RetCode_OK;
}

}
