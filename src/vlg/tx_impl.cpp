/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "pr_impl.h"
#include "conn_impl.h"
#include "tx_impl.h"

#define TX_RES_COMMT    "COMMITTED"
#define TX_RES_FAIL     "FAILED"
#define TX_RES_ABORTED  "ABORTED"
#define TX_NO_OBJ       "NO-OBJ"

namespace vlg {
const std_shared_ptr_obj_mng<incoming_transaction> tx_std_shp_omng;
}

namespace vlg {

tx_impl::tx_impl(incoming_transaction &publ, incoming_connection &conn) :
    conn_(conn.impl_.get()),
    status_(TransactionStatus_INITIALIZED),
    tx_res_(TransactionResult_UNDEFINED),
    result_code_(ProtocolCode_SUCCESS),
    txtype_(TransactionRequestType_UNDEFINED),
    txactn_(Action_NONE),
    req_nclassid_(0),
    req_clsenc_(Encode_UNDEFINED),
    res_nclassid_(0),
    res_clsenc_(Encode_UNDEFINED),
    rsclrq_(false),
    rescls_(false),
    start_mark_tim_(0),
    ipubl_(&publ),
    opubl_(nullptr)
{}


tx_impl::tx_impl(outgoing_transaction &publ) :
    conn_(nullptr),
    status_(TransactionStatus_INITIALIZED),
    tx_res_(TransactionResult_UNDEFINED),
    result_code_(ProtocolCode_SUCCESS),
    txtype_(TransactionRequestType_UNDEFINED),
    txactn_(Action_NONE),
    req_nclassid_(0),
    req_clsenc_(Encode_UNDEFINED),
    res_nclassid_(0),
    res_clsenc_(Encode_UNDEFINED),
    rsclrq_(false),
    rescls_(false),
    start_mark_tim_(0),
    ipubl_(nullptr),
    opubl_(&publ)
{}

RetCode tx_impl::await_for_status_reached(TransactionStatus test,
                                          TransactionStatus &current,
                                          time_t sec,
                                          long nsec)
{
    scoped_mx smx(mon_);
    if(status_ < TransactionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(status_ < test) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode = RetCode_TIMEOUT;
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

RetCode tx_impl::await_for_closure(time_t sec, long nsec)
{
    scoped_mx smx(mon_);
    if(status_ < TransactionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(status_ < TransactionStatus_CLOSED) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode = RetCode_TIMEOUT;
                break;
            }
        }
    }
    IFLOG(log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO "[res:%d][closed %s]",
              __func__, rcode, rcode ? "not reached" : "reached"))
    return rcode;
}

void tx_impl::set_request_obj(const nclass &val)
{
    request_obj_ = val.clone();
    req_nclassid_ = request_obj_->get_id();
}

void tx_impl::set_current_obj(const nclass &val)
{
    current_obj_ = val.clone();
    req_nclassid_ = current_obj_->get_id();
}

void tx_impl::set_result_obj(const nclass &val)
{
    result_obj_ = val.clone();
    res_nclassid_ = result_obj_->get_id();
    rescls_ = true;
}

RetCode tx_impl::set_flying()
{
    if(status_ != TransactionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    IFLOG(dbg(TH_ID, LS_TXO"[%08x%08x%08x%08x][FLYING]",
              txid_.txplid,
              txid_.txsvid,
              txid_.txcnid,
              txid_.txprid))
    set_status(TransactionStatus_FLYING);
    return RetCode_OK;
}

inline void tx_impl::trace_tx_closure(const char *tx_res_str)
{
    std::stringstream ss;
    rt_time_t fin_mark_tim, dt_mark_tim;
    rt_mark_time(&fin_mark_tim);
    dt_mark_tim = rt_diff_time(start_mark_tim_, fin_mark_tim);
    ss << std::setw(14) << dt_mark_tim;
    if(ipubl_) {
        if(request_obj_) {
            IFLOG(inf_nclass(TH_ID,
                             request_obj_.get(),
                             true,
                             LS_TXI"[%08x%08x%08x%08x]",
                             txid_.txplid,
                             txid_.txsvid,
                             txid_.txcnid,
                             txid_.txprid))
        } else {
            IFLOG(inf(TH_ID,
                      LS_TXI"[%08x%08x%08x%08x]{%s}",
                      txid_.txplid,
                      txid_.txsvid,
                      txid_.txcnid,
                      txid_.txprid,
                      TX_NO_OBJ))
        }
        if(result_obj_) {
            IFLOG(inf_nclass(TH_ID,
                             result_obj_.get(),
                             true,
                             LS_TXO"[%08x%08x%08x%08x]",
                             txid_.txplid,
                             txid_.txsvid,
                             txid_.txcnid,
                             txid_.txprid))
        } else {
            IFLOG(inf(TH_ID,
                      LS_TXO"[%08x%08x%08x%08x]{%s}",
                      txid_.txplid,
                      txid_.txsvid,
                      txid_.txcnid,
                      txid_.txprid,
                      TX_NO_OBJ))
        }
    } else {
        if(request_obj_) {
            IFLOG(inf_nclass(TH_ID,
                             request_obj_.get(),
                             true,
                             LS_TXO"[%08x%08x%08x%08x]",
                             txid_.txplid,
                             txid_.txsvid,
                             txid_.txcnid,
                             txid_.txprid))
        } else {
            IFLOG(inf(TH_ID,
                      LS_TXO"[%08x%08x%08x%08x]{%s}",
                      txid_.txplid,
                      txid_.txsvid,
                      txid_.txcnid,
                      txid_.txprid,
                      TX_NO_OBJ))
        }
        if(result_obj_) {
            IFLOG(inf_nclass(TH_ID,
                             result_obj_.get(),
                             true,
                             LS_TXI"[%08x%08x%08x%08x]",
                             txid_.txplid,
                             txid_.txsvid,
                             txid_.txcnid,
                             txid_.txprid))
        } else {
            IFLOG(inf(TH_ID, LS_TXI"[%08x%08x%08x%08x]{%s}",
                      txid_.txplid,
                      txid_.txsvid,
                      txid_.txcnid,
                      txid_.txprid,
                      TX_NO_OBJ))
        }
    }
    IFLOG(inf(TH_ID,
              LS_TRX"[%08x%08x%08x%08x][%s][TXRES:%d, TXRESCODE:%d, RESCLS:%d][RTT-NS:%s]",
              txid_.txplid,
              txid_.txsvid,
              txid_.txcnid,
              txid_.txprid,
              tx_res_str,
              tx_res_,
              result_code_,
              rescls_,
              ss.str().c_str()))
}

RetCode tx_impl::set_closed()
{
    if(status_ != TransactionStatus_FLYING) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    const char *tx_res_str = (tx_res_ == TransactionResult_COMMITTED) ? TX_RES_COMMT : TX_RES_FAIL;
    trace_tx_closure(tx_res_str);
    set_status(TransactionStatus_CLOSED);
    if(ipubl_) {
        ipubl_->on_close();
    } else {
        opubl_->on_close();
    }
    return RetCode_OK;
}

RetCode tx_impl::set_aborted()
{
    tx_res_ = TransactionResult_ABORTED;
    const char *tx_res_str = TX_RES_ABORTED;
    trace_tx_closure(tx_res_str);
    set_status(TransactionStatus_CLOSED);
    if(ipubl_) {
        ipubl_->on_close();
    } else {
        opubl_->on_close();
    }
    return RetCode_OK;
}

RetCode tx_impl::set_status(TransactionStatus status)
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

}

namespace vlg {

incoming_transaction_impl::incoming_transaction_impl(incoming_transaction &publ,
                                                     std::shared_ptr<incoming_connection> &conn) :
    tx_impl(publ, *conn),
    conn_sh_(conn)
{}

incoming_transaction_impl::~incoming_transaction_impl()
{
    if(status_ ==  TransactionStatus_FLYING) {
        IFLOG(cri(TH_ID, LS_DTR"[transaction is not in a safe state::%d]" LS_EXUNX, __func__, status_))
    }
}

void incoming_transaction_impl::set_request_obj_on_request(nclass &val)
{
    request_obj_.reset(&val);
    req_nclassid_ = request_obj_->get_id();
}

// VLG_TRANSACTION SENDING METHS

RetCode incoming_transaction_impl::send_response()
{
    RetCode rcode = RetCode_OK;
    if(status_ != TransactionStatus_FLYING) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    g_bbuf *gbb = new g_bbuf();
    build_PKT_TXRESP(tx_res_,
                     result_code_,
                     &txid_,
                     rescls_,
                     res_clsenc_,
                     res_nclassid_,
                     gbb);
    int totbytes = result_obj_ ? result_obj_->serialize(res_clsenc_, nullptr, gbb) : 0;
    totbytes = htonl(totbytes);
    if(result_obj_) {
        gbb->put(&totbytes, (6*4), 4);
    }
    gbb->flip();
    RET_ON_KO(conn_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_sh_);
    rcode = conn_->peer_->selector_.asynch_notify(evt);
    if(rcode) {
        set_status(TransactionStatus_ERROR);
    }
    return rcode;
}

}

namespace vlg {

outgoing_transaction_impl::outgoing_transaction_impl(outgoing_transaction &publ) :
    tx_impl(publ)
{}

outgoing_transaction_impl::~outgoing_transaction_impl()
{
    if(status_ ==  TransactionStatus_FLYING) {
        IFLOG(cri(TH_ID, LS_DTR"[transaction is not in a safe state::%d]" LS_EXUNX, __func__, status_))
    }
}

RetCode outgoing_transaction_impl::re_new()
{
    if(status_ == TransactionStatus_FLYING) {
        IFLOG(err(TH_ID, LS_CLO "[transaction is flying, cannot renew]", __func__))
        return RetCode_BADSTTS;
    }
    conn_->impl_->next_tx_id(txid_);
    set_status(TransactionStatus_INITIALIZED);
    return RetCode_OK;
}

void outgoing_transaction_impl::set_result_obj_on_response(nclass &val)
{
    result_obj_.reset(&val);
    res_nclassid_ = result_obj_->get_id();
    rescls_ = true;
}

// VLG_TRANSACTION SENDING METHS

RetCode outgoing_transaction_impl::send()
{
    RetCode rcode = RetCode_OK;
    if(status_ != TransactionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }

    rt_mark_time(&start_mark_tim_);
    set_flying();
    outgoing_transaction_impl *self = this;
    if((rcode = conn_->impl_->outg_flytx_map_.put(&txid_, &self))) {
        set_status(TransactionStatus_ERROR);
        IFLOG(err(TH_ID, LS_TRL"[error putting tx into flying map - res:%d]", rcode))
        return rcode;
    }
    IFLOG(inf(TH_ID,
              LS_OUT"[%08x%08x%08x%08x][TXTYPE:%d, TXACT:%d, CLSENC:%d, RSCLREQ:%d]",
              txid_.txplid,
              txid_.txsvid,
              txid_.txcnid,
              txid_.txprid,
              txtype_,
              txactn_,
              req_clsenc_,
              rsclrq_))
    g_bbuf *gbb = new g_bbuf();
    build_PKT_TXRQST(txtype_,
                     txactn_,
                     &txid_,
                     rsclrq_,
                     req_clsenc_,
                     req_nclassid_,
                     conn_->get_id(),
                     gbb);
    int totbytes = request_obj_ ? request_obj_->serialize(req_clsenc_, current_obj_.get(), gbb) : 0;
    totbytes = htonl(totbytes);
    if(request_obj_) {
        gbb->put(&totbytes, (6*4), 4);
    }
    gbb->flip();
    RET_ON_KO(conn_->impl_->pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, conn_->impl_.get());
    if((rcode = conn_->impl_->peer_->selector_.asynch_notify(evt))) {
        set_status(TransactionStatus_ERROR);
    }
    return rcode;
}

}
