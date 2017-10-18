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

#include "blz_peer_int.h"
#include "blz_connection_int.h"
#include "blz_subscription_int.h"

namespace vlg {

class sbs_event_inst_collector : public vlg::collector {
    public:
        sbs_event_inst_collector() : vlg::collector("subscription_event_int") {}
};

vlg::collector *sbs_event_int_inst_coll_ = NULL;
vlg::collector &sbs_event_int_inst_collector()
{
    if(sbs_event_int_inst_coll_) {
        return *sbs_event_int_inst_coll_;
    }
    if(!(sbs_event_int_inst_coll_ = new sbs_event_inst_collector())) {
        EXIT_ACTION("failed creating subscription_event_int::sbs_event_int_inst_coll_\n")
    }
    return *sbs_event_int_inst_coll_;
}

vlg::collector &subscription_event_int::get_collector()
{
    return sbs_event_int_inst_collector();
}

// BLZ_SBS_EVENT CTORS - INIT - DESTROY

vlg::RetCode subscription_event_int::new_instance(subscription_event_int
                                                    **ptr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, ptr))
    if(!((*ptr) = new subscription_event_int())) {
        IFLOG(cri(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_MEMERR;
    } else {
        IFLOG(trc(TH_ID, LS_CLO "%s(new_inst:%p)", __func__, *ptr))
        return vlg::RetCode_OK;
    }
}

nclass_logger *subscription_event_int::log_ = NULL;

subscription_event_int::subscription_event_int() :
    sbs_sbsid_(0),
    sbs_evtid_(0),
    sbs_evttype_(SubscriptionEventType_UNDEFINED),
    sbs_protocode_(ProtocolCode_SUCCESS),
    sbs_tmstp0_(0),
    sbs_tmstp1_(0),
    sbs_act_(Action_NONE),
    sbs_data_(NULL)
{
    log_ = get_nclass_logger("subscription_event_int");
    IFLOG(trc(TH_ID, LS_CTR "%s(ptr:%p)", __func__, this))
}

subscription_event_int::~subscription_event_int()
{
    if(sbs_data_) {
        /************************
        RELEASE_ID: SBE_DTA_01
        ************************/
        vlg::collector &c = sbs_data_->get_collector();
        c.release(sbs_data_);
    }
    IFLOG(trc(TH_ID, LS_DTR "%s(ptr:%p)", __func__, this))
}

// BLZ_SBS_EVENT GETTERS / SETTERS

unsigned int subscription_event_int::get_sbsid()
{
    return sbs_sbsid_;
}

unsigned int subscription_event_int::get_evtid()
{
    return sbs_evtid_;
}

SubscriptionEventType subscription_event_int::get_evttype()
{
    return sbs_evttype_;
}

ProtocolCode subscription_event_int::get_protocode()
{
    return sbs_protocode_;
}

unsigned int subscription_event_int::get_tmstp0()
{
    return sbs_tmstp0_;
}

unsigned int subscription_event_int::get_tmstp1()
{
    return sbs_tmstp1_;
}

Action subscription_event_int::get_act()
{
    return sbs_act_;
}

nclass *subscription_event_int::get_obj()
{
    return sbs_data_;
}

void subscription_event_int::set_sbsid(unsigned int val)
{
    sbs_sbsid_ = val;
}

void subscription_event_int::set_evtid(unsigned int val)
{
    sbs_evtid_ = val;
}

void subscription_event_int::set_evttype(SubscriptionEventType val)
{
    sbs_evttype_ = val;
}

void subscription_event_int::set_protocode(ProtocolCode val)
{
    sbs_protocode_ = val;
}

void subscription_event_int::set_tmstp0(unsigned int val)
{
    sbs_tmstp0_ = val;
}

void subscription_event_int::set_tmstp1(unsigned int val)
{
    sbs_tmstp1_ = val;
}

void subscription_event_int::set_act(Action val)
{
    sbs_act_ = val;
}

void subscription_event_int::set_obj(const nclass *obj)
{
    if(sbs_data_) {
        /************************
        RELEASE_ID: SBE_DTA_01
        ************************/
        vlg::collector &c = sbs_data_->get_collector();
        c.release(sbs_data_);
        sbs_data_ = NULL;
    }
    if(obj) {
        if((sbs_data_ = obj->clone())) {
            vlg::collector &c = sbs_data_->get_collector();
            /************************
            ADOPT_ID: SBE_DTA_01
            ************************/
            c.retain(sbs_data_);
        }
    }
}

void subscription_event_int::set_obj_on_event_receive(nclass *val)
{
    if(sbs_data_) {
        /************************
        RELEASE_ID: SBE_DTA_01
        ************************/
        vlg::collector &c = sbs_data_->get_collector();
        c.release(sbs_data_);
        sbs_data_ = NULL;
    }
    if((sbs_data_ = val)) {
        vlg::collector &c = sbs_data_->get_collector();
        /************************
        ADOPT_ID: SBE_DTA_01
        ************************/
        c.retain(sbs_data_);
    }
}

// BLZ_SBS_EVENT_WRPR

sbs_event_wrapper::sbs_event_wrapper(subscription_event_int *evt) :
    evt_(evt)
{
    if(evt_) {
        vlg::collector &c = evt_->get_collector();
        /************************
        ADOPT_ID: SEW_SBE_01
        ************************/
        c.retain(evt_);
    }
}

sbs_event_wrapper::~sbs_event_wrapper()
{
    if(evt_) {
        vlg::collector &c = evt_->get_collector();
        /************************
        RELEASE_ID: SEW_SBE_01
        ************************/
        c.release(evt_);
    }
}

void sbs_event_wrapper::set_evt(subscription_event_int *evt)
{
    if(evt_) {
        vlg::collector &c = evt_->get_collector();
        /************************
        RELEASE_ID: SEW_SBE_01
        ************************/
        c.release(evt_);
        evt_ = NULL;
    }
    if((evt_ = evt)) {
        vlg::collector &c = evt_->get_collector();
        /************************
        ADOPT_ID: SEW_SBE_01
        ************************/
        c.retain(evt_);
    }
}

subscription_event_int *sbs_event_wrapper::get_evt()
{
    return evt_;
}

// BLZ_SUBSCRIPTION CTORS - INIT - DESTROY

//BLZ_SUBSCRIPTION MEMORY

class subscription_inst_collector : public vlg::collector {
    public:
        subscription_inst_collector() : vlg::collector("subscription_int") {}

};

vlg::collector *sbs_inst_coll_ = NULL;
vlg::collector &sbs_inst_collector()
{
    if(sbs_inst_coll_) {
        return *sbs_inst_coll_;
    }
    if(!(sbs_inst_coll_ = new subscription_inst_collector())) {
        EXIT_ACTION("failed creating subscription_int::sbs_inst_coll_\n")
    }
    return *sbs_inst_coll_;
}

vlg::collector &subscription_int::get_collector()
{
    return sbs_inst_collector();
}

nclass_logger *subscription_int::log_ = NULL;

subscription_int::subscription_int(connection_int &conn) :
    peer_(conn.peer()),
    conn_(conn),
    bem_(conn.peer().get_em()),
    sbsid_(0),
    reqid_(0),
    status_(SubscriptionStatus_EARLY),
    ssc_hndl_(NULL),
    ssc_hndl_ud_(NULL),
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
    last_blzcod_(ProtocolCode_SUCCESS),
    sen_hndl_(NULL),
    sen_hndl_ud_(NULL),
    cli_evt_sts_(BLZ_SBS_Evt_Undef),
    cli_evt_q_(vlg::sngl_ptr_obj_mng()),
    srv_sbs_last_evt_ack_(true), //at the beginning we consider true this.
    srv_sbs_to_ack_evtid_(0),
    srv_sbs_last_ack_evtid_(0),
    srv_sbs_evt_glob_q_(vlg::sngl_ptr_obj_mng()),
    srv_sbs_classkey_evt_q_hm_(vlg::sngl_ptr_obj_mng(),
                               vlg::sngl_cstr_obj_mng()),
    initial_query_(NULL),
    initial_query_ended_(true)
{
    log_ = get_nclass_logger("subscription_int");
    IFLOG(trc(TH_ID, LS_CTR "%s(ptr:%p)", __func__, this))
}

subscription_int::~subscription_int()
{
    vlg::collector &c = get_collector();
    if(c.is_instance_collected(this)) {
        IFLOG(cri(TH_ID, LS_DTR "%s(ptr:%p)" D_W_R_COLL LS_EXUNX,
                  __func__,
                  this))
    }
    /*additional safety*/
    release_initial_query();
    pthread_rwlock_destroy(&lock_srv_sbs_rep_asynch_);
    IFLOG(trc(TH_ID, LS_DTR "%s(ptr:%p)", __func__, this))
}

vlg::RetCode subscription_int::init()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    RETURN_IF_NOT_OK(cli_evt_q_.init(1))
    CHK_PTH_ERR_2(pthread_rwlock_init, &lock_srv_sbs_rep_asynch_, NULL)
    RETURN_IF_NOT_OK(srv_sbs_evt_glob_q_.init())
    RETURN_IF_NOT_OK(srv_sbs_classkey_evt_q_hm_.init(HM_SIZE_SMALL))
    set_status(SubscriptionStatus_INITIALIZED);
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return vlg::RetCode_OK;
}

void subscription_int::release_initial_query()
{
    if(!initial_query_ended_) {
        if(initial_query_) {
            initial_query_->release();
        }
        initial_query_ = NULL;
        initial_query_ended_ = true;
    }
}

// BLZ_SUBSCRIPTION GETTERS / SETTERS

peer_int &subscription_int::peer()
{
    return peer_;
}

connection_int &subscription_int::get_connection()
{
    return conn_;
}

SubscriptionStatus subscription_int::status()
{
    return status_;
}

unsigned int subscription_int::sbsid()
{
    return sbsid_;
}

unsigned int subscription_int::nclass_id()
{
    return nclassid_;
}

inline void subscription_int::set_sbs_to_ack_evt_id(unsigned int sbs_evtid)
{
    srv_sbs_last_evt_ack_ = false;
    srv_sbs_to_ack_evtid_ = sbs_evtid;
}

inline void subscription_int::set_sbs_last_ack_evt_id()
{
    srv_sbs_last_evt_ack_ = true;
    srv_sbs_last_ack_evtid_ = srv_sbs_to_ack_evtid_;
    srv_sbs_to_ack_evtid_ = 0;
}

SubscriptionType subscription_int::get_sbstyp() const
{
    return sbstyp_;
}

SubscriptionMode subscription_int::get_sbsmod() const
{
    return sbsmod_;
}

SubscriptionFlowType subscription_int::get_flotyp() const
{
    return flotyp_;
}

SubscriptionDownloadType subscription_int::get_dwltyp() const
{
    return dwltyp_;
}

Encode subscription_int::get_enctyp() const
{
    return enctyp_;
}

unsigned int subscription_int::get_open_tmstp0() const
{
    return open_tmstp0_;
}

unsigned int subscription_int::get_open_tmstp1() const
{
    return open_tmstp1_;
}

persistence_query_int *subscription_int::get_initial_query()
{
    return initial_query_;
}

bool subscription_int::is_initial_query_ended()
{
    return initial_query_ended_;
}

void subscription_int::set_nclassid(unsigned int nclass_id)
{
    nclassid_ = nclass_id;
}

void subscription_int::set_sbstyp(SubscriptionType val)
{
    sbstyp_ = val;
}

void subscription_int::set_sbsmod(SubscriptionMode val)
{
    sbsmod_ = val;
}

void subscription_int::set_flotyp(SubscriptionFlowType val)
{
    flotyp_ = val;
}

void subscription_int::set_dwltyp(SubscriptionDownloadType val)
{
    dwltyp_ = val;
}

void subscription_int::set_enctyp(Encode val)
{
    enctyp_ = val;
}

void subscription_int::set_open_tmstp0(unsigned int val)
{
    open_tmstp0_ = val;
}

void subscription_int::set_open_tmstp1(unsigned int val)
{
    open_tmstp1_ = val;
}

void subscription_int::set_subscription_status_change_handler(
    subscription_status_change_hndlr hndlr, void *ud)
{
    ssc_hndl_ = hndlr;
    ssc_hndl_ud_ = ud;
}

void subscription_int::set_subscription_event_notify_handler(
    subscription_evt_notify_hndlr hndlr, void *ud)
{
    sen_hndl_ = hndlr;
    sen_hndl_ud_ = ud;
}

void subscription_int::set_initial_query(persistence_query_int *initial_query)
{
    initial_query_ = initial_query;
}

void subscription_int::set_initial_query_ended(bool val)
{
    initial_query_ended_ = val;
}

// BLZ_SUBSCRIPTION STATUS

vlg::RetCode subscription_int::set_req_sent()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    IFLOG(inf(TH_ID,
              LS_SBO"[CONNID:%010u-REQID:%010u][SBSTYP:%d, SBSMOD:%d, FLOTYP:%d, DWLTYP:%d, ENCTYP:%d, NCLSSID:%d, TMSTP0:%u, TMSTP1:%u]",
              conn_.connid(),
              reqid_,
              sbstyp_,
              sbsmod_,
              flotyp_,
              dwltyp_,
              enctyp_,
              nclassid_,
              open_tmstp0_,
              open_tmstp1_))
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    set_status(SubscriptionStatus_REQUEST_SENT);
    return vlg::RetCode_OK;
}

vlg::RetCode subscription_int::set_started()
{
    IFLOG(inf(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][STARTED]", conn_.connid(),
              sbsid_))
    set_status(SubscriptionStatus_STARTED);
    on_start();
    return vlg::RetCode_OK;
}

vlg::RetCode subscription_int::set_stopped()
{
    IFLOG(inf(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][STOPPED]", conn_.connid(),
              sbsid_))
    set_status(SubscriptionStatus_STOPPED);
    on_stop();
    return vlg::RetCode_OK;
}

vlg::RetCode subscription_int::set_released()
{
    IFLOG(dbg(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][RELEASED]", conn_.connid(),
              sbsid_))
    set_status(SubscriptionStatus_RELEASED);
    return vlg::RetCode_OK;
}

vlg::RetCode subscription_int::set_error()
{
    IFLOG(err(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][ERROR]", conn_.connid(),
              sbsid_))
    set_status(SubscriptionStatus_ERROR);
    on_stop();
    return vlg::RetCode_OK;
}

vlg::RetCode subscription_int::set_status(SubscriptionStatus status)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, status:%d)", __func__, this, status))
    CHK_MON_ERR_0(lock)
    status_ = status;
    if(ssc_hndl_) {
        ssc_hndl_(*this, status, ssc_hndl_ud_);
    }
    CHK_MON_ERR_0(notify_all)
    CHK_MON_ERR_0(unlock)
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return vlg::RetCode_OK;
}

vlg::RetCode subscription_int::await_for_status_reached_or_outdated(
    SubscriptionStatus test,
    SubscriptionStatus &current,
    time_t sec,
    long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, test:%d)", __func__, this, test))
    CHK_MON_ERR_0(lock)
    if(status_ < SubscriptionStatus_INITIALIZED) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADSTTS;
    }
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    while(status_ < test) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    current = status_;
    IFLOG(log(cdrs_res ? vlg::TL_WRN : vlg::TL_DBG, TH_ID,
              LS_CLO "%s(ptr:%p, res:%d) - test:%d [reached or outdated] current:%d",
              __func__, this, cdrs_res,
              test, status_))
    CHK_MON_ERR_0(unlock)
    return cdrs_res;
}

vlg::RetCode subscription_int::await_for_start_result(
    SubscriptionResponse
    &sbs_start_result,
    ProtocolCode &sbs_start_protocode,
    time_t sec,
    long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    CHK_MON_ERR_0(lock)
    if(status_ < SubscriptionStatus_INITIALIZED) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADSTTS;
    }
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    while(!start_stop_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    sbs_start_result = sbresl_;
    sbs_start_protocode = last_blzcod_;
    IFLOG(log(cdrs_res ? vlg::TL_WRN : vlg::TL_DBG, TH_ID, LS_CLO
              "%s(ptr:%p, res:%d, status:%d) - [subscription start result available] - sbs_start_result:%d, sbs_start_protocode:%d",
              __func__,
              this,
              cdrs_res,
              status_,
              sbresl_,
              last_blzcod_))
    start_stop_evt_occur_ = false;
    CHK_MON_ERR_0(unlock)
    return cdrs_res;
}

vlg::RetCode subscription_int::await_for_stop_result(SubscriptionResponse
                                                       &sbs_stop_result,
                                                       ProtocolCode &sbs_stop_protocode,
                                                       time_t sec,
                                                       long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    CHK_MON_ERR_0(lock)
    if(status_ < SubscriptionStatus_INITIALIZED) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADSTTS;
    }
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    while(!start_stop_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    sbs_stop_result = sbresl_;
    sbs_stop_protocode = last_blzcod_;
    IFLOG(log(cdrs_res ? vlg::TL_WRN : vlg::TL_DBG, TH_ID, LS_CLO
              "%s(ptr:%p, res:%d, status:%d) - [subscription stop result available] - sbs_stop_result:%d, sbs_stop_protocode:%d",
              __func__,
              this,
              cdrs_res,
              status_,
              sbresl_,
              last_blzcod_))
    start_stop_evt_occur_ = false;
    CHK_MON_ERR_0(unlock)
    return cdrs_res;
}

vlg::RetCode subscription_int::notify_for_start_stop_result()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    mon_.lock();
    start_stop_evt_occur_ = true;
    mon_.notify_all();
    mon_.unlock();
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

// BLZ_SUBSCRIPTION ACTIONS

vlg::RetCode subscription_int::start()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ != SubscriptionStatus_INITIALIZED &&
            status_ != SubscriptionStatus_STOPPED) {
        IFLOG(err(TH_ID, LS_CLO "%s() - [status:%d]", __func__, status_))
        return vlg::RetCode_BADSTTS;
    }
    //client side adoption to avoid userspace deletion.
    /************************
    RETAIN_ID: SBS_CLI_01
    ************************/
    vlg::collector &c = get_collector();
    c.retain(this);

    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    reqid_ = conn_.next_reqid();
    vlg::synch_hash_map &reqid_sbs_map = conn_.reqid_sbs_map();
    subscription_int *self = this;
    if((cdrs_res = reqid_sbs_map.put(&reqid_, &self))) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - hash put, res:%d", __func__, cdrs_res))
        return cdrs_res;
    }
    if((cdrs_res = send_start_request())) {
        IFLOG(err(TH_ID, LS_TRL "%s() - send request failed, res:%d", __func__,
                  cdrs_res))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::start(SubscriptionType sbscr_type,
                                       SubscriptionMode sbscr_mode,
                                       SubscriptionFlowType sbscr_flow_type,
                                       SubscriptionDownloadType  sbscr_dwnld_type,
                                       Encode sbscr_class_encode,
                                       unsigned int nclass_id,
                                       unsigned int start_timestamp_0,
                                       unsigned int start_timestamp_1)
{
    IFLOG(trc(TH_ID, LS_OPN
              "%s(ptr:%p, sbscr_type:%d, sbscr_mode:%d, sbscr_flow_type:%d, sbscr_dwnld_type:%d, sbscr_class_encode:%d, nclass_id:%u, ts0:%u, ts1:%u)",
              __func__,
              this,
              sbscr_type,
              sbscr_mode,
              sbscr_flow_type,
              sbscr_dwnld_type,
              sbscr_class_encode,
              nclass_id,
              start_timestamp_0,
              start_timestamp_1))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if(status_ != SubscriptionStatus_INITIALIZED &&
            status_ != SubscriptionStatus_STOPPED) {
        IFLOG(err(TH_ID, LS_CLO "%s() - [status:%d]", __func__, status_))
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
    cdrs_res = start();
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::stop()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ != SubscriptionStatus_STARTED) {
        IFLOG(err(TH_ID, LS_CLO "%s() - [status:%d]", __func__, status_))
        return vlg::RetCode_BADSTTS;
    }
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if(!initial_query_ended_) {
        if(initial_query_) {
            initial_query_->release();
        }
        initial_query_ = NULL;
        initial_query_ended_ = true;
    }
    start_stop_evt_occur_ = false;
    DECLINITH_GBB(gbb, BLZ_BUFF_DEF_SZ)
    build_PKT_SBSTOP(sbsid_,
                     gbb);
    gbb->flip();
    RETURN_IF_NOT_OK(conn_.pkt_snd_q().put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_SendPacket, &conn_);
    cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt);
    if(cdrs_res) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

// BLZ_SUBSCRIPTION SENDING METHS

vlg::RetCode subscription_int::send_start_request()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    set_req_sent();
    start_stop_evt_occur_ = false;
    DECLINITH_GBB(gbb, BLZ_BUFF_DEF_SZ)
    build_PKT_SBSREQ(sbstyp_,
                     sbsmod_,
                     flotyp_,
                     dwltyp_,
                     enctyp_,
                     nclassid_,
                     conn_.connid(),
                     reqid_,
                     open_tmstp0_,
                     open_tmstp1_,
                     gbb);
    gbb->flip();
    RETURN_IF_NOT_OK(conn_.pkt_snd_q().put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_SendPacket, &conn_);
    if((cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt))) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::send_start_response()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    DECLINITH_GBB(gbb, BLZ_BUFF_DEF_SZ)
    build_PKT_SBSRES(sbresl_,
                     last_blzcod_,
                     reqid_,
                     sbsid_,
                     gbb);
    gbb->flip();
    RETURN_IF_NOT_OK(conn_.pkt_snd_q().put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_SendPacket, &conn_);
    cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt);
    if(cdrs_res) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::send_event(const subscription_event_int
                                            *sbs_evt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, sbs_evt:%p)", __func__, this, sbs_evt))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    DECLINITH_GBB(gbb, BLZ_BUFF_DEF_SZ)
    build_PKT_SBSEVT(sbsid_,
                     sbs_evt->sbs_evttype_,
                     sbs_evt->sbs_act_,
                     sbs_evt->sbs_protocode_,
                     sbs_evt->sbs_evtid_,
                     sbs_evt->sbs_tmstp0_,
                     sbs_evt->sbs_tmstp1_,
                     gbb);

    long totbytes = sbs_evt->sbs_data_ ?
                    sbs_evt->sbs_data_->serialize(enctyp_, NULL, gbb) :
                    (long)gbb->position();

    totbytes = htonl(totbytes);
    if(sbs_evt->sbs_data_) {
        gbb->put(&totbytes, (6*4), 4);
    }
    gbb->flip();
    RETURN_IF_NOT_OK(conn_.pkt_snd_q().put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_SendPacket, &conn_);
    set_sbs_to_ack_evt_id(sbs_evt->sbs_evtid_);
    IFLOG(dbg(TH_ID, LS_SBS"[sbsid:[%d] - sbs event[%u] set to ack]", sbsid_,
              srv_sbs_to_ack_evtid_))
    cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt);
    if(cdrs_res) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::send_event_ack()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    DECLINITH_GBB(gbb, BLZ_BUFF_DEF_SZ)
    build_PKT_SBSACK(sbsid_,
                     cli_last_evt_->sbs_evtid_,
                     gbb);
    gbb->flip();
    RETURN_IF_NOT_OK(conn_.pkt_snd_q().put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_SendPacket, &conn_);
    cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt);
    if(cdrs_res) {
        set_status(SubscriptionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

// BLZ_SUBSCRIPTION ASYNCH SENDING

vlg::RetCode subscription_int::store_sbs_evt_srv_asynch(subscription_event_int
                                                          *evt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, evt:%p)", __func__, this, evt))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    vlg::ascii_string pkey;
    vlg::blocking_queue *classkeyvalue_inst_evt_q = NULL;
    sbs_event_wrapper *sev_wrpr = NULL;
    if(evt->get_evttype() == SubscriptionEventType_LIVE) {
        //class instance.
        evt->get_obj()->primary_key_string_value(&pkey);
        if((cdrs_res = srv_sbs_classkey_evt_q_hm_.get(pkey.internal_buff(),
                                                      &classkeyvalue_inst_evt_q))) {
            //not found.
            classkeyvalue_inst_evt_q =
                new vlg::blocking_queue(vlg::sngl_ptr_obj_mng());
            classkeyvalue_inst_evt_q->init();

            if((cdrs_res = srv_sbs_classkey_evt_q_hm_.put(pkey.internal_buff(),
                                                          &classkeyvalue_inst_evt_q))) {
                IFLOG(cri(TH_ID, LS_CLO
                          "%s(res:%d) - failed to put into srv_sbs_classkey_evt_q_hm_",
                          __func__,
                          cdrs_res))
                return cdrs_res;
            } else {
                IFLOG(trc(TH_ID, LS_TRL
                          "%s() - put into srv_sbs_classkey_evt_q_hm_ {%p} - [key:%s]",
                          __func__,
                          classkeyvalue_inst_evt_q, pkey.internal_buff()))
            }
        } else {
            IFLOG(trc(TH_ID, LS_TRL
                      "%s() - get from srv_sbs_classkey_evt_q_hm_ {%p} - [key:%s]",
                      __func__,
                      classkeyvalue_inst_evt_q, pkey.internal_buff()))
        }
        if(flotyp_ == SubscriptionFlowType_LAST) {
            if((cdrs_res = classkeyvalue_inst_evt_q->peek(0, 0, &sev_wrpr))) {
                //not found.
                if(!(sev_wrpr = new sbs_event_wrapper(evt))) {
                    IFLOG(cri(TH_ID, LS_CLO "%s - memory", __func__))
                    return vlg::RetCode_MEMERR;
                }
                if((cdrs_res = classkeyvalue_inst_evt_q->put(&sev_wrpr))) {
                    IFLOG(cri(TH_ID, LS_CLO
                              "%s(res:%d) - event:[%u] - failed to put sev_wrpr:%p into classkeyvalue_inst_evt_q",
                              __func__,
                              cdrs_res, evt->get_evtid(),
                              sev_wrpr))
                    return cdrs_res;
                } else {
                    IFLOG(trc(TH_ID, LS_TRL
                              "%s() - event:[%u] - put sev_wrpr:%p into classkeyvalue_inst_evt_q - [key:%s]",
                              __func__,
                              evt->get_evtid(),
                              sev_wrpr,
                              pkey.internal_buff()))
                }
                if((cdrs_res = srv_sbs_evt_glob_q_.put(&sev_wrpr))) {
                    IFLOG(cri(TH_ID, LS_CLO
                              "%s(res:%d) - event:[%u] - failed to put sev_wrpr:%p into srv_sbs_evt_glob_q_",
                              __func__,
                              cdrs_res,
                              evt->get_evtid(),
                              sev_wrpr))
                    return cdrs_res;
                } else {
                    IFLOG(trc(TH_ID, LS_TRL
                              "%s() - event:[%u] - put sev_wrpr:%p into srv_sbs_evt_glob_q_ - [key:%s]",
                              __func__,
                              evt->get_evtid(),
                              sev_wrpr,
                              pkey.internal_buff()))
                }
            } else {
                //snapshotting
                IFLOG(trc(TH_ID, LS_TRL
                          "%s() - snapshotting - replaced event:[%u] with event:[%u] - [key:%s]",
                          __func__,
                          sev_wrpr->get_evt()->get_evtid(),
                          evt->get_evtid(),
                          pkey.internal_buff()))
                sev_wrpr->set_evt(evt);
            }
        } else {
            //FLOW ALL
            /*evt adoption is implicit when evt is wrapped.*/
            if(!(sev_wrpr = new sbs_event_wrapper(evt))) {
                IFLOG(cri(TH_ID, LS_CLO "%s - memory", __func__))
                return vlg::RetCode_MEMERR;
            }
            if((cdrs_res = classkeyvalue_inst_evt_q->put(&sev_wrpr))) {
                IFLOG(cri(TH_ID, LS_CLO
                          "%s(res:%d) - failed to put sev_wrpr:%p into classkeyvalue_inst_evt_q",
                          __func__, cdrs_res,
                          sev_wrpr))
                return cdrs_res;
            } else {
                IFLOG(trc(TH_ID, LS_TRL "%s() - put sev_wrpr:%p into classkeyvalue_inst_evt_q",
                          __func__, sev_wrpr))
            }
            if((cdrs_res = srv_sbs_evt_glob_q_.put(&sev_wrpr))) {
                IFLOG(cri(TH_ID, LS_CLO
                          "%s(res:%d) - failed to put sev_wrpr:%p into srv_sbs_evt_glob_q_", __func__,
                          cdrs_res,
                          sev_wrpr))
                return cdrs_res;
            } else {
                IFLOG(trc(TH_ID, LS_TRL "%s() - put sev_wrpr:%p into srv_sbs_evt_glob_q_",
                          __func__, sev_wrpr))
            }
        }
    } else {
        //NOT-DATA EVENTS. (Live, End, Stop)
        //these events are always enqueued in glob queue
        if(!(sev_wrpr = new sbs_event_wrapper(evt))) {
            IFLOG(cri(TH_ID, LS_CLO "%s", __func__))
            return vlg::RetCode_MEMERR;
        }
        if((cdrs_res = srv_sbs_evt_glob_q_.put(&sev_wrpr))) {
            IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - failed to put into srv_sbs_evt_glob_q_",
                      __func__, cdrs_res))
            return cdrs_res;
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::consume_sbs_evt_srv_asynch(
    subscription_event_int **evt_out)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, evt_out:%p)", __func__, this, evt_out))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    sbs_event_wrapper *sev_wrpr_1 = NULL, *sev_wrpr_2 = NULL;
    if(!(cdrs_res = srv_sbs_evt_glob_q_.get(0, 0, &sev_wrpr_1))) {
        vlg::ascii_string pkey;
        sev_wrpr_1->get_evt()->get_obj()->primary_key_string_value(&pkey);
        IFLOG(trc(TH_ID, LS_TRL "%s() - new event[%u] available. - [key:%s]",
                  __func__,
                  sev_wrpr_1->get_evt()->get_evtid(),
                  pkey.internal_buff()))
        if(sev_wrpr_1->get_evt()->get_evttype() ==
                SubscriptionEventType_LIVE) {
            vlg::blocking_queue *classkeyvalue_inst_evt_q = NULL;
            if(!(cdrs_res = srv_sbs_classkey_evt_q_hm_.get(pkey.internal_buff(),
                                                           &classkeyvalue_inst_evt_q))) {
                if(!(cdrs_res = classkeyvalue_inst_evt_q->get(0, 0, &sev_wrpr_2))) {
                    if(sev_wrpr_1 != sev_wrpr_2) { //check for same obj.
                        IFLOG(cri(TH_ID, LS_CLO
                                  "%s(res:%d) - inconsistence: [sev_wrpr_1:%p != sev_wrpr_2:%p] - [key:%s] - [clsskeyval_q:%p] - aborting.",
                                  __func__,
                                  cdrs_res, sev_wrpr_1, sev_wrpr_2, pkey.internal_buff(),
                                  classkeyvalue_inst_evt_q))
                        return vlg::RetCode_GENERR;
                    }
                } else {
                    //error get event from queue.
                    IFLOG(cri(TH_ID, LS_CLO
                              "%s(res:%d) - failed to get sev_wrpr_2 from classkeyvalue_inst_evt_q - [key:%s] - aborting",
                              __func__,
                              cdrs_res, pkey.internal_buff()))
                    return vlg::RetCode_GENERR;
                }
            } else {
                //not found.
                IFLOG(cri(TH_ID, LS_CLO
                          "%s(res:%d) - failed to get event from srv_sbs_classkey_evt_q_hm_ - [key:%s] - aborting",
                          __func__,
                          cdrs_res, pkey.internal_buff()))
                return vlg::RetCode_GENERR;
            }
        }
        /************************
        RETAIN_ID: SBE_SRV_02
        ************************/
        vlg::collector &c = sev_wrpr_1->get_evt()->get_collector();
        c.retain(sev_wrpr_1->get_evt());
        *evt_out = sev_wrpr_1->get_evt();
        delete sev_wrpr_1;
    } else {
        IFLOG(trc(TH_ID, LS_TRL "%s() - no new event available.", __func__))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d, *evt_out:%p)", __func__, cdrs_res,
              &evt_out))
    return cdrs_res;
}

vlg::RetCode subscription_int::accept_event(subscription_event_int *sbs_evt)
{
    return vlg::RetCode_OK;
}

vlg::RetCode subscription_int::submit_live_event(subscription_event_int
                                                   *sbs_evt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, sbs_evt:%p)",
              __func__,
              this,
              sbs_evt))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if((cdrs_res = accept_event(sbs_evt))) {
        IFLOG(trc(TH_ID, LS_TRL
                  "%s() - unauthorized event:[%d] skipped for subscriber[%d].",
                  __func__,
                  sbs_evt->sbs_evtid_,
                  sbsid_))
    } else {
        pthread_rwlock_wrlock(&lock_srv_sbs_rep_asynch_);
        if(srv_sbs_last_evt_ack_ && initial_query_ended_) {
            //synchro path
            IFLOG(trc(TH_ID, LS_TRL "%s() - send new live event:[%d].",
                      __func__,
                      sbs_evt->sbs_evtid_))
            if((cdrs_res = send_event(sbs_evt))) {
                IFLOG(err(TH_ID, LS_TRL "%s() - new live event:[%d] failed to send.",
                          __func__,
                          sbs_evt->sbs_evtid_))
            }
        } else {
            //asynchro path
            IFLOG(trc(TH_ID, LS_TRL
                      "%s() - an event:[%d], has not been ack by client yet, storing new live event:[%d].",
                      __func__,
                      srv_sbs_to_ack_evtid_, sbs_evt->sbs_evtid_))
            if((cdrs_res = store_sbs_evt_srv_asynch(sbs_evt))) {
                IFLOG(cri(TH_ID, LS_TRL "%s() - event storing failed:[%d].", __func__,
                          cdrs_res))
            }
        }
        pthread_rwlock_unlock(&lock_srv_sbs_rep_asynch_);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

// BLZ_SUBSCRIPTION RECVING METHS

/*Client only*/
vlg::RetCode subscription_int::receive_event(const blz_hdr_rec *pkt_hdr,
                                               vlg::grow_byte_buffer *pkt_body,
                                               subscription_event_int *sbs_evt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(pkt_hdr:%p, pkt_body:%p, sbs_evt:%p)",
              __func__,
              pkt_hdr,
              pkt_body,
              sbs_evt))

    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if(sbs_evt->get_evttype() != SubscriptionEventType_DOWNLOAD_END) {
        nclass *nobj = NULL;
        if((cdrs_res = bem_.new_class_instance(nclass_id(), &nobj))) {
            IFLOG(cri(TH_ID,
                      LS_SBS"[sbs event receive failed - new class fail:%d, nclass_id:%d, aborting]",
                      cdrs_res, nclass_id()))
            return cdrs_res;
        } else {
            vlg::collector &c = nobj->get_collector();
            c.retain(nobj);
        }
        sbs_evt->set_obj_on_event_receive(nobj);
        if((cdrs_res = nobj->restore(&bem_, enctyp_, pkt_body))) {
            IFLOG(cri(TH_ID,
                      LS_SBS"[sbs event receive failed - class restore fail:%d, nclass_id:%d]",
                      cdrs_res,
                      nclass_id()))
        } else {
            IFLOG(inf_class(TH_ID,
                            nobj,
                            true,
                            LS_SBI"[ACT:%d] ",
                            sbs_evt->sbs_act_))
        }
    }

    if((cdrs_res = cli_evt_q_.put(&sbs_evt))) {
        switch(cdrs_res) {
            case vlg::RetCode_QFULL:
            case vlg::RetCode_TIMEOUT:
                IFLOG(cri(TH_ID, LS_CLO "%s() - queue full.", __func__))
            case vlg::RetCode_PTHERR:
                IFLOG(cri(TH_ID, LS_CLO "%s() - pthread error.", __func__))
            default:
                IFLOG(cri(TH_ID, LS_CLO "%s() - RetCode_CODE:%d", __func__, cdrs_res))
        }
    }
    mon_.lock();
    if(!cdrs_res) {
        cli_last_evt_ = sbs_evt;
        evt_ready();
        if(sen_hndl_) {
            consume_event(&cli_last_evt_);
            on_event(*cli_last_evt_);
            sen_hndl_(*this, *cli_last_evt_, sen_hndl_ud_);
            ack_event_priv();
        }
    }
    mon_.unlock();
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

/*Server only*/
vlg::RetCode subscription_int::receive_event_ack(const blz_hdr_rec *hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    IFLOG(dbg(TH_ID, LS_SBS"[sbsid:[%d] - sbs event[%u] ack received]", sbsid_,
              srv_sbs_to_ack_evtid_))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    pthread_rwlock_wrlock(&lock_srv_sbs_rep_asynch_);
    set_sbs_last_ack_evt_id();
    subscription_event_int *sbs_evt = NULL;
    if(!initial_query_ended_) {
        cdrs_res = submit_dwnl_event();
    }
    /***********************************
     if cdrs_res == OK initial query has ended.
     if cdrs_res == RetCode_ENMEND initial query has ended now.
     in both cases we must check for pending live events.
    ***********************************/
    if((!cdrs_res || cdrs_res == vlg::RetCode_ENMEND) &&
            !(cdrs_res = consume_sbs_evt_srv_asynch(&sbs_evt))) {
        //there is a new event to send.
        IFLOG(trc(TH_ID, LS_TRL
                  "%s() - last event:[%d], has been ack by client, there is a new stored event to send:[%d].",
                  __func__,
                  srv_sbs_last_ack_evtid_,
                  sbs_evt->sbs_evtid_))
        if((cdrs_res = send_event(sbs_evt))) {
            IFLOG(err(TH_ID, LS_TRL "%s() - new event:[%d] failed to send.",
                      __func__,
                      sbs_evt->sbs_evtid_))
        }
    } else if(cdrs_res == vlg::RetCode_EMPTY) {
        IFLOG(trc(TH_ID, LS_TRL
                  "%s() - last event:[%d], has been ack by client, no new event to send.",
                  __func__,
                  srv_sbs_last_ack_evtid_))
        cdrs_res = vlg::RetCode_OK;
    }
    pthread_rwlock_unlock(&lock_srv_sbs_rep_asynch_);
    if(sbs_evt) {
        vlg::collector &c = sbs_evt->get_collector();
        /************************
        RELEASE_ID: SBE_SRV_02
        ************************/
        c.release(sbs_evt);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

/*Server only*/
vlg::RetCode subscription_int::submit_dwnl_event()
{
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    per_nclassid_helper_rec *sdr = NULL;
    subscription_event_int *new_dwnl_sbs_event = NULL;
    const entity_manager &bem = peer_.get_em();
    nclass *dwnl_obj = NULL;
    unsigned int ts0 = 0, ts1 = 0;
    if((cdrs_res = peer_.get_per_classid_helper_class(nclassid_, &sdr))) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - failed get per-nclass_id helper class [res:%d]",
                  __func__, cdrs_res))
        return cdrs_res;
    }
    //we need to newinstance here because we do not know if query has ended here.
    bem.new_class_instance(nclassid_, &dwnl_obj);
    vlg::collector &c = dwnl_obj->get_collector();
    c.retain(dwnl_obj);

    if((cdrs_res = initial_query_->load_next_entity(ts0, ts1,
                                                    *dwnl_obj)) == vlg::RetCode_DBROW) {
        RETURN_IF_NOT_OK(peer_.build_sbs_event(sdr->next_sbs_evt_id(),
                                               SubscriptionEventType_DOWNLOAD,
                                               ProtocolCode_SUCCESS,
                                               ts0,
                                               ts1,
                                               Action_INSERT,
                                               dwnl_obj,
                                               &new_dwnl_sbs_event))
        /************************
        RETAIN_ID: SBE_SRV_03
        ************************/
        vlg::collector &c1 = new_dwnl_sbs_event->get_collector();
        c1.retain(new_dwnl_sbs_event);
        if((cdrs_res = accept_event(new_dwnl_sbs_event))) {
            IFLOG(trc(TH_ID, LS_TRL
                      "%s() - unauthorized event:[%d] skipped for subscriber[%d].",
                      __func__,
                      new_dwnl_sbs_event->sbs_evtid_,
                      sbsid_))
        } else {
            IFLOG(trc(TH_ID, LS_TRL "%s() - send new dwnl event:[%d].",
                      __func__,
                      new_dwnl_sbs_event->sbs_evtid_))

            if((cdrs_res = send_event(new_dwnl_sbs_event))) {
                IFLOG(err(TH_ID, LS_TRL "%s() - new dwnl event:[%d] failed to send.",
                          __func__,
                          new_dwnl_sbs_event->sbs_evtid_))
            }
        }
        /************************
        RELEASE_ID: SBE_SRV_03
        ************************/
        c1.release(new_dwnl_sbs_event);
    } else if(cdrs_res == vlg::RetCode_QRYEND) {
        IFLOG(dbg(TH_ID, LS_TRL "%s(res:%d) - initial query has ended.",
                  __func__,
                  cdrs_res))

        RETURN_IF_NOT_OK(peer_.build_sbs_event(sdr->next_sbs_evt_id(),
                                               SubscriptionEventType_DOWNLOAD_END,
                                               ProtocolCode_SUCCESS,
                                               0,
                                               0,
                                               Action_NONE,
                                               NULL,
                                               &new_dwnl_sbs_event))
        /************************
        RETAIN_ID: SBE_SRV_03b
        ************************/
        vlg::collector &c1 = new_dwnl_sbs_event->get_collector();
        c1.retain(new_dwnl_sbs_event);

        IFLOG(trc(TH_ID, LS_TRL "%s() - send dwnl-end event:[%d].", __func__,
                  new_dwnl_sbs_event->sbs_evtid_))

        if((cdrs_res = send_event(new_dwnl_sbs_event))) {
            IFLOG(err(TH_ID, LS_TRL "%s() - dwnl-end event:[%d] failed to send.", __func__,
                      new_dwnl_sbs_event->sbs_evtid_))
        }
        /************************
        RELEASE_ID: SBE_SRV_03b
        ************************/
        c1.release(new_dwnl_sbs_event);
        if(!(cdrs_res = initial_query_->release())) {
            IFLOG(dbg(TH_ID, LS_TRL "%s() - initial query released.", __func__))
        } else {
            IFLOG(cri(TH_ID, LS_TRL "%s(res:%d) - initial query releasing failed.",
                      __func__, cdrs_res))
        }
        initial_query_ = NULL;
        initial_query_ended_ = true;
    } else {
        IFLOG(cri(TH_ID, LS_TRL "%s(res:%d) - initial query failed.", __func__,
                  cdrs_res))
    }
    c.release(dwnl_obj);
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::execute_initial_query()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    const entity_desc *class_desc = NULL;
    if(!(cdrs_res = bem_.get_entity_descriptor(nclassid_, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_int *driv = NULL;
            if((driv = peer_.get_pers_mng().available_driver(nclassid_))) {
                persistence_connection_int *conn = NULL;
                if((conn = driv->available_connection(nclassid_))) {
                    vlg::ascii_string qry_s;
                    qry_s.assign("select * from ");
                    qry_s.append(class_desc->get_entity_name());
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
                    qry_s.append(';');
                    cdrs_res = conn->execute_query(qry_s.internal_buff(), peer_.get_em(),
                                                   &initial_query_);
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%u",
                              __func__, nclassid_))
                    cdrs_res = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%u",
                          __func__, nclassid_))
                cdrs_res = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclassid_))
            cdrs_res = vlg::RetCode_KO;
        }
    } else {
        IFLOG(cri(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclassid_))
    }
    if(cdrs_res) {
        initial_query_ended_ = true;
        if(initial_query_) {
            initial_query_->release();
        }
        initial_query_ = NULL;
        IFLOG(err(TH_ID, LS_CLO "%s() - sbsid:[%d] - Initial Query FAILED [res:%d].",
                  __func__, sbsid_, cdrs_res))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}


vlg::RetCode subscription_int::safe_submit_dwnl_event()
{
    pthread_rwlock_wrlock(&lock_srv_sbs_rep_asynch_);
    vlg::RetCode cdrs_res = submit_dwnl_event();
    pthread_rwlock_unlock(&lock_srv_sbs_rep_asynch_);
    return cdrs_res;
}

// BLZ_SUBSCRIPTION EVENT MNGMNT

vlg::RetCode subscription_int::await_for_next_event(
    subscription_event_int **sbs_evt,
    time_t sec,
    long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    CHK_MON_ERR_0(lock)
    if(status_ < SubscriptionStatus_STARTED) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADSTTS;
    }
    while(cli_evt_sts_ != BLZ_SBS_Evt_Rdy) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    if(!cdrs_res) {
        IFLOG(log(cdrs_res ? vlg::TL_WRN : vlg::TL_DBG, TH_ID,
                  LS_TRL "%s(ptr:%p) - [event occurred]",
                  __func__,
                  this))

        cdrs_res = consume_event(sbs_evt);
    }
    CHK_MON_ERR_0(unlock)
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::ack_event()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    CHK_MON_ERR_0(lock)
    if(status_ < SubscriptionStatus_STARTED) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s() - status[%d]", __func__, status_))
        return vlg::RetCode_BADSTTS;
    }
    cdrs_res = ack_event_priv();
    CHK_MON_ERR_0(unlock)
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d) - [event ack sent]", __func__,
              this, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::consume_event(subscription_event_int **sbs_evt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if((cdrs_res = cli_evt_q_.get(sbs_evt))) {
        switch(cdrs_res) {
            case vlg::RetCode_EMPTY:
            case vlg::RetCode_TIMEOUT:
                IFLOG(cri(TH_ID, LS_CLO "%s(ptr:%p) - empty queue.", __func__, this))
            case vlg::RetCode_PTHERR:
                IFLOG(cri(TH_ID, LS_CLO "%s(ptr:%p) - pthread error.", __func__, this))
            default:
                IFLOG(cri(TH_ID, LS_CLO "%s(ptr:%p) - RetCode_CODE:%d", __func__, this,
                          cdrs_res))
        }
    }
    if(!cdrs_res) {
        evt_to_ack();
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d) - [event consumed]",
              __func__,
              this,
              cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::ack_event_priv()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if(cli_evt_sts_ != BLZ_SBS_Evt_ToAck || !cli_last_evt_) {
        IFLOG(wrn(TH_ID, LS_CLO "%s() - cli_last_evt_[%p] evt_status[%d]",
                  __func__,
                  &cli_last_evt_,
                  cli_evt_sts_))
        return vlg::RetCode_BADSTTS;
    }
    if((cdrs_res = send_event_ack())) {
        IFLOG(err(TH_ID, LS_TRL "%s() - [event ack sending failed res:%d]",
                  __func__,
                  cdrs_res))
    } else {
        vlg::collector &c = cli_last_evt_->get_collector();
        /************************
        RELEASE_ID: SBE_CLI_01
        ************************/
        c.release(cli_last_evt_);
        cli_last_evt_ = NULL;
        if(!cdrs_res) {
            evt_reset();
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d) - [event ack sent]", __func__,
              this, cdrs_res))
    return cdrs_res;
}

vlg::RetCode subscription_int::evt_reset()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    cli_evt_sts_ = BLZ_SBS_Evt_Rst;
    CHK_MON_ERR_0(notify_all)
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

vlg::RetCode subscription_int::evt_ready()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    cli_evt_sts_ = BLZ_SBS_Evt_Rdy;
    CHK_MON_ERR_0(notify_all)
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

vlg::RetCode subscription_int::evt_to_ack()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    cli_evt_sts_ = BLZ_SBS_Evt_ToAck;
    CHK_MON_ERR_0(notify_all)
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

void subscription_int::on_start()
{}

void subscription_int::on_stop()
{}

void subscription_int::on_event(subscription_event_int &sbs_evt)
{}

}
