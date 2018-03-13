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
 *sbs_event_wrapper
 */

#include "pr_impl.h"
#include "conn_impl.h"
#include "sbs_impl.h"

namespace vlg {

const std_shared_ptr_obj_mng<subscription_event> sbse_std_shp_omng;
const std_shared_ptr_obj_mng<subscription> sbs_std_shp_omng;

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
                                                 nclass *data) :
    sbs_sbsid_(sbsid),
    sbs_evtid_(evtid),
    sbs_evttype_(set),
    sbs_protocode_(pc),
    sbs_tmstp0_(ts0),
    sbs_tmstp1_(ts1),
    sbs_act_(act),
    sbs_data_(data ? data : nullptr)
{}

//subscription_impl

subscription_impl::subscription_impl(subscription &publ) :
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
    cli_evt_sts_(SBSEvt_Undef),
    srv_sbs_last_evt_ack_(true),
    srv_sbs_to_ack_evtid_(0),
    srv_sbs_last_ack_evtid_(0),
    srv_sbs_evt_glob_q_(vlg::sngl_ptr_obj_mng()),
    initial_query_(nullptr),
    initial_query_ended_(true),
    publ_(publ)
{
    pthread_rwlock_init(&lock_srv_sbs_rep_deferred_, 0);
    set_status(SubscriptionStatus_INITIALIZED);
}

subscription_impl::~subscription_impl()
{
    release_initial_query();
    pthread_rwlock_destroy(&lock_srv_sbs_rep_deferred_);
}

void subscription_impl::release_initial_query()
{
    if(!initial_query_ended_) {
        if(initial_query_) {
            initial_query_->release();
        }
        initial_query_ = nullptr;
        initial_query_ended_ = true;
    }
}

inline void subscription_impl::set_sbs_to_ack_evt_id(unsigned int sbs_evtid)
{
    srv_sbs_last_evt_ack_ = false;
    srv_sbs_to_ack_evtid_ = sbs_evtid;
}

inline void subscription_impl::set_sbs_last_ack_evt_id()
{
    srv_sbs_last_evt_ack_ = true;
    srv_sbs_last_ack_evtid_ = srv_sbs_to_ack_evtid_;
    srv_sbs_to_ack_evtid_ = 0;
}

void subscription_impl::set_connection(connection &val)
{
    conn_ = &val;
}

void subscription_impl::set_connection_sh(std::shared_ptr<connection> &val)
{
    conn_sh_ = val;
    conn_ = val.get();
}

RetCode subscription_impl::set_req_sent()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    IFLOG(inf(TH_ID,
              LS_SBO"[CONNID:%010u-REQID:%010u][SBSTYP:%d, SBSMOD:%d, FLOTYP:%d, DWLTYP:%d, ENCTYP:%d, NCLSSID:%d, TMSTP0:%u, TMSTP1:%u]",
              conn_->get_id(),
              reqid_,
              sbstyp_,
              sbsmod_,
              flotyp_,
              dwltyp_,
              enctyp_,
              nclassid_,
              open_tmstp0_,
              open_tmstp1_))
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    set_status(SubscriptionStatus_REQUEST_SENT);
    return vlg::RetCode_OK;
}

RetCode subscription_impl::set_started()
{
    IFLOG(inf(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][started]", conn_->get_id(), sbsid_))
    set_status(SubscriptionStatus_STARTED);
    publ_.on_start();
    return vlg::RetCode_OK;
}

RetCode subscription_impl::set_stopped()
{
    IFLOG(inf(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][stopped]", conn_->get_id(), sbsid_))
    set_status(SubscriptionStatus_STOPPED);
    publ_.on_stop();
    return vlg::RetCode_OK;
}

RetCode subscription_impl::set_released()
{
    IFLOG(dbg(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][released]", conn_->get_id(), sbsid_))
    set_status(SubscriptionStatus_RELEASED);
    return vlg::RetCode_OK;
}

RetCode subscription_impl::set_error()
{
    IFLOG(err(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][error]", conn_->get_id(), sbsid_))
    set_status(SubscriptionStatus_ERROR);
    publ_.on_stop();
    return vlg::RetCode_OK;
}

RetCode subscription_impl::set_status(SubscriptionStatus status)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(mon_);
    status_ = status;
    publ_.on_status_change(status_);
    mon_.notify_all();
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode subscription_impl::await_for_status_reached_or_outdated(SubscriptionStatus test,
                                                                SubscriptionStatus &current,
                                                                time_t sec,
                                                                long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(mon_);
    if(status_ < SubscriptionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    while(status_ < test) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    current = status_;
    IFLOG(log(rcode ? vlg::TL_WRN : vlg::TL_DBG, TH_ID, LS_CLO "test:%d [reached or outdated] current:%d",
              __func__,
              test,
              status_))
    return rcode;
}

RetCode subscription_impl::await_for_start_result(SubscriptionResponse &sbs_start_result,
                                                  ProtocolCode &sbs_start_protocode,
                                                  time_t sec,
                                                  long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(mon_);
    if(status_ < SubscriptionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    while(!start_stop_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    sbs_start_result = sbresl_;
    sbs_start_protocode = last_vlgcod_;
    IFLOG(log(rcode ? vlg::TL_WRN : vlg::TL_DBG, TH_ID, LS_CLO
              "[sbsid:%d, res:%d, status:%d][subscription start result available][sbs_start_result:%d, sbs_start_protocode:%d]",
              __func__,
              sbsid_,
              rcode,
              status_,
              sbresl_,
              last_vlgcod_))
    start_stop_evt_occur_ = false;
    return rcode;
}

RetCode subscription_impl::await_for_stop_result(SubscriptionResponse &sbs_stop_result,
                                                 ProtocolCode &sbs_stop_protocode,
                                                 time_t sec,
                                                 long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(mon_);
    if(status_ < SubscriptionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    while(!start_stop_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    sbs_stop_result = sbresl_;
    sbs_stop_protocode = last_vlgcod_;
    IFLOG(log(rcode ? vlg::TL_WRN : vlg::TL_DBG, TH_ID, LS_CLO
              "[sbsid:%d, res:%d, status:%d][subscription stop result available][sbs_stop_result:%d, sbs_stop_protocode:%d]",
              __func__,
              sbsid_,
              rcode,
              status_,
              sbresl_,
              last_vlgcod_))
    start_stop_evt_occur_ = false;
    return rcode;
}

RetCode subscription_impl::notify_for_start_stop_result()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(mon_);
    start_stop_evt_occur_ = true;
    mon_.notify_all();
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    return vlg::RetCode_OK;
}

RetCode subscription_impl::start()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(status_ != SubscriptionStatus_INITIALIZED && status_ != SubscriptionStatus_STOPPED) {
        IFLOG(err(TH_ID, LS_CLO "[status:%d]", __func__, status_))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    reqid_ = conn_->impl_->next_reqid();
    subscription_impl *self = this;
    rcode = conn_->impl_->outg_reqid_sbs_map_.put(&reqid_, &self);
    if((rcode = send_start_request())) {
        IFLOG(err(TH_ID, LS_TRL "[send request failed][res:%d]", __func__, rcode))
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode subscription_impl::start(SubscriptionType sbscr_type,
                                 SubscriptionMode sbscr_mode,
                                 SubscriptionFlowType sbscr_flow_type,
                                 SubscriptionDownloadType  sbscr_dwnld_type,
                                 Encode sbscr_class_encode,
                                 unsigned int nclass_id,
                                 unsigned int start_timestamp_0,
                                 unsigned int start_timestamp_1)
{
    IFLOG(trc(TH_ID,
              LS_OPN"[sbsid:%d, sbscr_type:%d, sbscr_mode:%d, sbscr_flow_type:%d, sbscr_dwnld_type:%d, sbscr_class_encode:%d, nclass_id:%u, ts0:%u, ts1:%u]",
              __func__,
              sbsid_,
              sbscr_type,
              sbscr_mode,
              sbscr_flow_type,
              sbscr_dwnld_type,
              sbscr_class_encode,
              nclass_id,
              start_timestamp_0,
              start_timestamp_1))
    RetCode rcode = vlg::RetCode_OK;
    if(status_ != SubscriptionStatus_INITIALIZED && status_ != SubscriptionStatus_STOPPED) {
        IFLOG(err(TH_ID, LS_CLO "[status:%d]", __func__, status_))
        return vlg::RetCode_BADSTTS;
    }
    sbstyp_ = sbscr_type;
    sbsmod_ = sbscr_mode;
    flotyp_ = sbscr_flow_type;
    dwltyp_ = sbscr_dwnld_type;
    enctyp_ = sbscr_class_encode;
    nclassid_ = nclass_id;
    open_tmstp0_ = start_timestamp_0;
    open_tmstp1_ = start_timestamp_1;
    rcode = start();
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode subscription_impl::stop()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(status_ != SubscriptionStatus_STARTED) {
        IFLOG(err(TH_ID, LS_CLO "[status:%d]", __func__, status_))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    if(!initial_query_ended_) {
        if(initial_query_) {
            initial_query_->release();
        }
        initial_query_ = nullptr;
        initial_query_ended_ = true;
    }
    start_stop_evt_occur_ = false;
    g_bbuf *gbb = new g_bbuf();
    build_PKT_SBSTOP(sbsid_, gbb);
    gbb->flip();
    RET_ON_KO(conn_->impl_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_);
    rcode = conn_->impl_->peer_->selector_.evt_enqueue_and_notify(evt);
    if(rcode) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

//cli
RetCode subscription_impl::send_start_request()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    set_req_sent();
    start_stop_evt_occur_ = false;
    g_bbuf *gbb = new g_bbuf();
    build_PKT_SBSREQ(sbstyp_,
                     sbsmod_,
                     flotyp_,
                     dwltyp_,
                     enctyp_,
                     nclassid_,
                     conn_->get_id(),
                     reqid_,
                     open_tmstp0_,
                     open_tmstp1_,
                     gbb);
    gbb->flip();
    RET_ON_KO(conn_->impl_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_);
    if((rcode = conn_->impl_->peer_->selector_.evt_enqueue_and_notify(evt))) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

//srv
RetCode subscription_impl::send_start_response()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    g_bbuf *gbb = new g_bbuf();
    build_PKT_SBSRES(sbresl_,
                     last_vlgcod_,
                     reqid_,
                     sbsid_,
                     gbb);
    gbb->flip();
    RET_ON_KO(conn_->impl_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_sh_);
    rcode = conn_->impl_->peer_->selector_.evt_enqueue_and_notify(evt);
    if(rcode) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

//srv
RetCode subscription_impl::send_event(std::shared_ptr<subscription_event> &sbs_evt)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
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
    RET_ON_KO(conn_->impl_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_sh_);
    set_sbs_to_ack_evt_id(sbs_evt->impl_->sbs_evtid_);
    IFLOG(dbg(TH_ID, LS_SBS"[sbsid:%d, subscription event:%u set to ack]", sbsid_, srv_sbs_to_ack_evtid_))
    rcode = conn_->impl_->peer_->selector_.evt_enqueue_and_notify(evt);
    if(rcode) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

//cli
RetCode subscription_impl::send_event_ack(unsigned int evtid)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    g_bbuf *gbb = new g_bbuf();
    build_PKT_SBSACK(sbsid_, evtid, gbb);
    gbb->flip();
    RET_ON_KO(conn_->impl_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_);
    rcode = conn_->impl_->peer_->selector_.evt_enqueue_and_notify(evt);
    if(rcode) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode subscription_impl::store_sbs_evt(std::shared_ptr<subscription_event> &sbs_evt)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    std::unique_ptr<char> pkey;
    vlg::b_qu *nclass_key_value_evt_q = nullptr;
    sbs_event_wrapper *sev_wrpr = nullptr;
    if(sbs_evt->impl_->sbs_evttype_ == SubscriptionEventType_LIVE) {
        //class instance.
        sbs_evt->impl_->sbs_data_->get_primary_key_value_as_string(pkey);
        auto it = srv_sbs_nclasskey_evt_q_hm_.find(pkey.get());
        if(it == srv_sbs_nclasskey_evt_q_hm_.end()) {
            //not found.
            nclass_key_value_evt_q = new b_qu(vlg::sngl_ptr_obj_mng());
            srv_sbs_nclasskey_evt_q_hm_[pkey.get()] =
                std::move(std::unique_ptr<b_qu>(nclass_key_value_evt_q));
        }

        if(flotyp_ == SubscriptionFlowType_LAST) {
            if((rcode = nclass_key_value_evt_q->peek(0, 0, &sev_wrpr))) {
                //not found.
                sev_wrpr = new sbs_event_wrapper(sbs_evt);
                nclass_key_value_evt_q->put(&sev_wrpr);
                srv_sbs_evt_glob_q_.put(&sev_wrpr);
            } else {
                //snapshotting
                IFLOG(trc(TH_ID, LS_TRL"[snapshotting][replaced event:[%u] with event:[%u], key:%s]",
                          __func__,
                          sev_wrpr->sbs_evt_->impl_->sbs_evtid_,
                          sbs_evt->impl_->sbs_evtid_,
                          pkey.get()))
                sev_wrpr->sbs_evt_ = sbs_evt;
            }
        } else {
            //FLOW ALL
            /*evt adoption is implicit when evt is wrapped.*/
            sev_wrpr = new sbs_event_wrapper(sbs_evt);
            nclass_key_value_evt_q->put(&sev_wrpr);
            srv_sbs_evt_glob_q_.put(&sev_wrpr);
        }

    } else {
        //NOT-DATA EVENTS. (Live, End, Stop)
        //these events are always enqueued in glob queue
        sev_wrpr = new sbs_event_wrapper(sbs_evt);
        srv_sbs_evt_glob_q_.put(&sev_wrpr);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode subscription_impl::consume_sbs_evt(std::shared_ptr<subscription_event> &sbs_evt)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    sbs_event_wrapper *sev_wrpr_1 = nullptr, *sev_wrpr_2 = nullptr;
    if(!(rcode = srv_sbs_evt_glob_q_.get(0, 0, &sev_wrpr_1))) {
        std::unique_ptr<char> pkey;
        sev_wrpr_1->sbs_evt_->impl_->sbs_data_->get_primary_key_value_as_string(pkey);
        IFLOG(trc(TH_ID, LS_TRL "[new event[%u] available][key:%s]",
                  __func__, sev_wrpr_1->sbs_evt_->impl_->sbs_evtid_, pkey.get()))
        if(sev_wrpr_1->sbs_evt_->impl_->sbs_evttype_ == SubscriptionEventType_LIVE) {
            auto it = srv_sbs_nclasskey_evt_q_hm_.find(pkey.get());
            if(it != srv_sbs_nclasskey_evt_q_hm_.end()) {
                if(!(rcode = it->second->get(0, 0, &sev_wrpr_2))) {
                    if(sev_wrpr_1 != sev_wrpr_2) { //check for same obj.
                        IFLOG(cri(TH_ID, LS_CLO
                                  "[inconsistence: [sev_wrpr_1:%p != sev_wrpr_2:%p] - [key:%s] - aborting]",
                                  __func__, sev_wrpr_1, sev_wrpr_2, pkey.get()))
                        return vlg::RetCode_GENERR;
                    }
                } else {
                    //error get event from queue.
                    IFLOG(cri(TH_ID, LS_CLO "[failed to get sev_wrpr_2 from classkeyvalue_inst_evt_q - [key:%s] - aborting]",
                              __func__, pkey.get()))
                    return vlg::RetCode_GENERR;
                }
            } else {
                //not found.
                IFLOG(cri(TH_ID, LS_CLO "[failed to get event from srv_sbs_classkey_evt_q_hm_ - [key:%s] - aborting]",
                          __func__, pkey.get()))
                return vlg::RetCode_GENERR;
            }
        }
        sbs_evt = sev_wrpr_1->sbs_evt_;
        delete sev_wrpr_1;
    } else {
        IFLOG(trc(TH_ID, LS_TRL "[no new event available]", __func__))
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode subscription_impl::submit_live_event(std::shared_ptr<subscription_event> &sbs_evt)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    if((rcode = publ_.accept_distribution(*sbs_evt))) {
        IFLOG(trc(TH_ID, LS_TRL"[unauthorized event:%d skipped for subscriber:%d]",
                  __func__, sbs_evt->impl_->sbs_evtid_, sbsid_))
    } else {
        pthread_rwlock_wrlock(&lock_srv_sbs_rep_deferred_);
        if(srv_sbs_last_evt_ack_ && initial_query_ended_) {
            //immediate sending
            IFLOG(trc(TH_ID, LS_TRL "[send new live event:%d].", __func__, sbs_evt->impl_->sbs_evtid_))
            if((rcode = send_event(sbs_evt))) {
                IFLOG(err(TH_ID, LS_TRL "[new live event:%d failed to send]", __func__, sbs_evt->impl_->sbs_evtid_))
            }
        } else {
            //deferred sending
            IFLOG(trc(TH_ID, LS_TRL"[an event:%d, has not been ack by client yet, storing new live event:%d]",
                      __func__, srv_sbs_to_ack_evtid_, sbs_evt->impl_->sbs_evtid_))
            if((rcode = store_sbs_evt(sbs_evt))) {
                IFLOG(cri(TH_ID, LS_TRL "[event storing failed:%d].", __func__, rcode))
            }
        }
        pthread_rwlock_unlock(&lock_srv_sbs_rep_deferred_);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

// VLG_SUBSCRIPTION RECVING METHS

/*Client only*/
RetCode subscription_impl::receive_event(const vlg_hdr_rec *pkt_hdr,
                                         vlg::g_bbuf *pkt_body)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;

    nclass *nobj = nullptr;
    if(pkt_hdr->row_2.sevttp.sevttp != SubscriptionEventType_DOWNLOAD_END) {
        if((rcode = conn_->impl_->peer_->nem_.new_nclass_instance(nclassid_, &nobj))) {
            IFLOG(cri(TH_ID, LS_SBS"[subscription event receive failed: new_nclass_instance fail:%d, nclass_id:%d]",
                      rcode,
                      nclassid_))
            return rcode;
        }
        if((rcode = nobj->restore(&conn_->impl_->peer_->nem_, enctyp_, pkt_body))) {
            IFLOG(cri(TH_ID, LS_SBS"[subscription event receive failed: nclass restore fail:%d, nclass_id:%d]",
                      rcode,
                      nclassid_))
            return rcode;
        } else {
            IFLOG(inf_nclass(TH_ID, nobj, true, LS_SBI"[ACT:%d] ", pkt_hdr->row_2.sevttp.sbeact))
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
    if(!rcode) {
        evt_ready();
        unsigned int evtid = sbs_evt->get_id();
        publ_.on_incoming_event(sbs_evt);
        evt_to_ack();
        evt_ack(evtid);
    }

    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

/*Server only*/
RetCode subscription_impl::receive_event_ack(const vlg_hdr_rec *hdr)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    IFLOG(dbg(TH_ID, LS_SBS"[sbsid:%d - subscription event:%u ack received]", sbsid_, srv_sbs_to_ack_evtid_))
    RetCode rcode = vlg::RetCode_OK;

    pthread_rwlock_wrlock(&lock_srv_sbs_rep_deferred_);
    set_sbs_last_ack_evt_id();
    std::shared_ptr<subscription_event> sbs_evt;
    if(!initial_query_ended_) {
        rcode = submit_dwnl_event();
    }
    /***********************************
     if rcode == OK initial query has ended.
     if rcode == RetCode_ENMEND initial query has ended now.
     in both cases we must check for pending live events.
    ***********************************/
    if((!rcode || rcode == vlg::RetCode_ENMEND) && !(rcode = consume_sbs_evt(sbs_evt))) {
        //there is a new event to send.
        IFLOG(trc(TH_ID, LS_TRL "[last event:%d, has been ack by client, there is a new stored event to send:%d].",
                  __func__, srv_sbs_last_ack_evtid_, sbs_evt->impl_->sbs_evtid_))
        if((rcode = send_event(sbs_evt))) {
            IFLOG(err(TH_ID, LS_TRL "[new event:%d failed to send]", __func__, sbs_evt->impl_->sbs_evtid_))
        }
    } else if(rcode == vlg::RetCode_EMPTY) {
        IFLOG(trc(TH_ID, LS_TRL "[last event:%d, has been ack by client, no new event to send]",
                  __func__, srv_sbs_last_ack_evtid_))
        rcode = vlg::RetCode_OK;
    }
    pthread_rwlock_unlock(&lock_srv_sbs_rep_deferred_);
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

/*Server only*/
RetCode subscription_impl::submit_dwnl_event()
{
    RetCode rcode = vlg::RetCode_OK;
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    per_nclass_id_conn_set *sdr = nullptr;
    subscription_event_impl *sbs_dwnl_evt_impl = nullptr;
    nclass *dwnl_obj = nullptr;
    unsigned int ts0 = 0, ts1 = 0;
    if((rcode = conn_->impl_->peer_->get_per_nclassid_helper_rec(nclassid_, &sdr))) {
        IFLOG(cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
        return rcode;
    }

    //we need to new instance here because we do not know if query has ended here.
    conn_->impl_->peer_->nem_.new_nclass_instance(nclassid_, &dwnl_obj);

    if((rcode = initial_query_->load_next_entity(ts0, ts1, *dwnl_obj)) == vlg::RetCode_DBROW) {
        sbs_dwnl_evt_impl = new subscription_event_impl(sbsid_,
                                                        sdr->next_sbs_evt_id(),
                                                        SubscriptionEventType_DOWNLOAD,
                                                        ProtocolCode_SUCCESS,
                                                        ts0,
                                                        ts1,
                                                        Action_INSERT,
                                                        dwnl_obj);
        std::shared_ptr<subscription_event> sbs_evt(new subscription_event(*sbs_dwnl_evt_impl));
        if((rcode = publ_.accept_distribution(*sbs_evt))) {
            IFLOG(trc(TH_ID, LS_TRL "[unauthorized event:%d skipped for subscriber%d]",
                      __func__, sbs_dwnl_evt_impl->sbs_evtid_, sbsid_))
        } else {
            IFLOG(trc(TH_ID, LS_TRL "[send new download event:%d]",
                      __func__, sbs_dwnl_evt_impl->sbs_evtid_))

            if((rcode = send_event(sbs_evt))) {
                IFLOG(err(TH_ID, LS_TRL "[new download event:%d failed to send]",
                          __func__, sbs_dwnl_evt_impl->sbs_evtid_))
            }
        }
    } else if(rcode == vlg::RetCode_QRYEND) {
        IFLOG(dbg(TH_ID, LS_TRL "[initial query has ended]", __func__))
        sbs_dwnl_evt_impl = new subscription_event_impl(sbsid_,
                                                        sdr->next_sbs_evt_id(),
                                                        SubscriptionEventType_DOWNLOAD_END,
                                                        ProtocolCode_SUCCESS,
                                                        0,
                                                        0,
                                                        Action_NONE,
                                                        nullptr);
        std::shared_ptr<subscription_event> sbs_evt(new subscription_event(*sbs_dwnl_evt_impl));
        IFLOG(trc(TH_ID, LS_TRL "[send download-end event:%d]", __func__, sbs_dwnl_evt_impl->sbs_evtid_))
        if((rcode = send_event(sbs_evt))) {
            IFLOG(err(TH_ID, LS_TRL "[download-end event:%d failed to send]", __func__, sbs_dwnl_evt_impl->sbs_evtid_))
        }
        if(!(rcode = initial_query_->release())) {
            IFLOG(dbg(TH_ID, LS_TRL "[download query released]", __func__))
        } else {
            IFLOG(cri(TH_ID, LS_TRL "[download query releasing failed][res:%d]", __func__, rcode))
        }
        initial_query_ = nullptr;
        initial_query_ended_ = true;
        delete dwnl_obj;
    } else {
        IFLOG(cri(TH_ID, LS_TRL "[download query failed][res:%d]", __func__, rcode))
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode subscription_impl::execute_initial_query()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    const nentity_desc *nclass_desc = conn_->impl_->peer_->nem_.get_nentity_descriptor(nclassid_);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = conn_->impl_->peer_->pers_mng_.available_driver(nclassid_))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclassid_))) {
                    std::string qry_s;
                    qry_s.assign("select * from ");
                    qry_s.append(nclass_desc->get_nentity_name());
                    if(dwltyp_ == SubscriptionDownloadType_PARTIAL) {
                        char ts_buff[TMSTMP_BUFF_SZ];
                        qry_s.append(" where (" P_F_TS0" = ");
                        snprintf(ts_buff, TMSTMP_BUFF_SZ, "%u", open_tmstp0_);
                        qry_s.append(ts_buff);
                        qry_s.append(" and " P_F_TS1" > ");
                        snprintf(ts_buff, TMSTMP_BUFF_SZ, "%u", open_tmstp1_);
                        qry_s.append(ts_buff);
                        qry_s.append(") or (");
                        qry_s.append(P_F_TS0" > ");
                        snprintf(ts_buff, TMSTMP_BUFF_SZ, "%u", open_tmstp0_);
                        qry_s.append(ts_buff);
                        qry_s.append(")");
                    }
                    qry_s.append(";");
                    rcode = conn->execute_query(qry_s.c_str(), conn_->impl_->peer_->nem_, initial_query_);
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%u]", __func__, nclassid_))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%u]", __func__, nclassid_))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclassid_))
            rcode = vlg::RetCode_KO;
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
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode subscription_impl::safe_submit_dwnl_event()
{
    pthread_rwlock_wrlock(&lock_srv_sbs_rep_deferred_);
    RetCode rcode = submit_dwnl_event();
    pthread_rwlock_unlock(&lock_srv_sbs_rep_deferred_);
    return rcode;
}

RetCode subscription_impl::evt_ack(unsigned int evtid)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    if(cli_evt_sts_ != SBSEvt_ToAck) {
        IFLOG(wrn(TH_ID, LS_CLO "[status:%d]", __func__, cli_evt_sts_))
        return vlg::RetCode_BADSTTS;
    }
    if((rcode = send_event_ack(evtid))) {
        IFLOG(err(TH_ID, LS_TRL "[event ack sending failed][res:%d]", __func__, rcode))
    } else {
        if(!rcode) {
            evt_reset();
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "[sbsid:%d - event ack sent]", __func__, sbsid_))
    return rcode;
}

RetCode subscription_impl::evt_reset()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    cli_evt_sts_ = SBSEvt_Reset;
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    return vlg::RetCode_OK;
}

RetCode subscription_impl::evt_ready()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    cli_evt_sts_ = SBSEvt_Ready;
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    return vlg::RetCode_OK;
}

RetCode subscription_impl::evt_to_ack()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    cli_evt_sts_ = SBSEvt_ToAck;
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    return vlg::RetCode_OK;
}

}
