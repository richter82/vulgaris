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
#include "blz_transaction_int.h"

namespace vlg {

// BLZ_TRANSACTION CTORS - INIT - DESTROY

//BLZ_TRANSACTION MEMORY

class transaction_inst_collector : public vlg::collector {
    public:
        transaction_inst_collector() : vlg::collector("transaction_int") {}
};

vlg::collector *inst_coll_ = NULL;
vlg::collector &tx_inst_collector()
{
    if(inst_coll_) {
        return *inst_coll_;
    }
    if(!(inst_coll_ = new transaction_inst_collector())) {
        EXIT_ACTION("failed creating transaction_int::inst_coll_\n")
    }
    return *inst_coll_;
}

vlg::collector &transaction_int::get_collector()
{
    return tx_inst_collector();
}

nclass_logger *transaction_int::log_ = NULL;

transaction_int::transaction_int(connection_int &conn) :
    peer_(conn.peer()),
    conn_(conn),
    bem_(conn.peer().get_em()),
    status_(TransactionStatus_EARLY),
    tx_res_(TransactionResult_UNDEFINED),
    result_code_(ProtocolCode_SUCCESS),
    txtype_(TransactionRequestType_UNDEFINED),
    txactn_(Action_NONE),
    req_classid_(0),
    req_clsenc_(Encode_UNDEFINED),
    res_classid_(0),
    res_clsenc_(Encode_UNDEFINED),
    rsclrq_(false),
    rescls_(false),
    request_obj_(NULL),
    current_obj_(NULL),
    result_obj_(NULL),
    tsc_hndl_(NULL),
    tsc_hndl_ud_(NULL),
    tres_hndl_(NULL),
    tres_hndl_ud_(NULL),
    start_mark_tim_(0)
{
    log_ = get_nclass_logger("transaction_int");
    IFLOG(trc(TH_ID, LS_CTR "%s(ptr:%p)", __func__, this))
}

transaction_int::~transaction_int()
{
    vlg::collector &c = get_collector();
    if(c.is_instance_collected(this)) {
        IFLOG(cri(TH_ID, LS_DTR "%s(ptr:%p)" D_W_R_COLL LS_EXUNX, __func__, this))
    }
    if(status_ ==  TransactionStatus_FLYING) {
        IFLOG(cri(TH_ID, LS_DTR
                  "%s(ptr:%p) - [transaction is not in a safe state::%d] "
                  LS_EXUNX,
                  __func__,
                  this,
                  status_))
    }
    objs_release();
    IFLOG(trc(TH_ID, LS_DTR "%s(ptr:%p)", __func__, this))
}

vlg::RetCode transaction_int::objs_release()
{
    if(request_obj_) {
        /************************
        RELEASE_ID: TRX_SOJ_01
        ************************/
        vlg::collector &c = request_obj_->get_collector();
        c.release(request_obj_);
        request_obj_ = NULL;
    }
    if(current_obj_) {
        /************************
        RELEASE_ID: TRX_COJ_01
        ************************/
        vlg::collector &c = current_obj_->get_collector();
        c.release(current_obj_);
        current_obj_ = NULL;
    }
    if(result_obj_) {
        /************************
        RELEASE_ID: TRX_ROJ_01
        ************************/
        vlg::collector &c = result_obj_->get_collector();
        c.release(result_obj_);
        result_obj_ = NULL;
    }
    return vlg::RetCode_OK;
}

vlg::RetCode transaction_int::init()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    set_status(TransactionStatus_INITIALIZED);
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

vlg::RetCode transaction_int::re_new()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ < TransactionStatus_PREPARED) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return vlg::RetCode_BADSTTS;
    }
    if(status_ == TransactionStatus_FLYING) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p) - transaction is flying, cannot renew.",
                  __func__, this))
        return vlg::RetCode_BADSTTS;
    }
    objs_release();
    conn_.next_tx_id(txid_);
    set_status(TransactionStatus_INITIALIZED);
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

//-----------------------------
// BLZ_TRANSACTION GETTERS / SETTERS
//
//-----------------------------
peer_int &transaction_int::peer()
{
    return peer_;
}

connection_int &transaction_int::get_connection()
{
    return conn_;
}

TransactionStatus transaction_int::status()
{
    return status_;
}

TransactionResult transaction_int::tx_res()
{
    return tx_res_;
}

ProtocolCode transaction_int::tx_result_code()
{
    return result_code_;
}

TransactionRequestType transaction_int::tx_req_type()
{
    return txtype_;
}

Action transaction_int::tx_act()
{
    return txactn_;
}

unsigned int transaction_int::tx_req_class_id()
{
    return req_classid_;
}

Encode transaction_int::tx_req_class_encode()
{
    return req_clsenc_;
}

unsigned int transaction_int::tx_res_class_id()
{
    return res_classid_;
}

vlg::Encode transaction_int::tx_res_class_encode()
{
    return res_clsenc_;
}

bool transaction_int::is_result_class_req()
{
    return rsclrq_;
}

bool transaction_int::is_result_class_set()
{
    return rescls_;
}

const nclass *transaction_int::request_obj()
{
    return request_obj_;
}

const nclass *transaction_int::current_obj()
{
    return current_obj_;
}

const nclass *transaction_int::result_obj()
{
    return result_obj_;
}

void transaction_int::set_tx_res(TransactionResult val)
{
    tx_res_ = val;
}

void transaction_int::set_tx_result_code(ProtocolCode val)
{
    result_code_ = val;
}

void transaction_int::set_tx_req_type(TransactionRequestType val)
{
    txtype_ = val;
}

void transaction_int::set_tx_act(Action val)
{
    txactn_ = val;
}

void transaction_int::set_tx_req_class_id(unsigned int val)
{
    req_classid_ = val;
}

void transaction_int::set_tx_req_class_encode(Encode val)
{
    req_clsenc_ = val;
}

void transaction_int::set_tx_res_class_id(unsigned int val)
{
    res_classid_ = val;
}

void transaction_int::set_tx_res_class_encode(Encode val)
{
    res_clsenc_ = val;
}

void transaction_int::set_result_class_req(bool val)
{
    rsclrq_ = val;
}

void transaction_int::set_result_class_set(bool val)
{
    rescls_ = val;
}

void transaction_int::set_request_obj(const nclass *val)
{
    if(request_obj_) {
        /************************
        RELEASE_ID: TRX_SOJ_01
        ************************/
        vlg::collector &c = request_obj_->get_collector();
        c.release(request_obj_);
        request_obj_ = NULL;
    }
    if(val) {
        if((request_obj_ = val->clone())) {
            vlg::collector &c = request_obj_->get_collector();
            /************************
            RETAIN_ID: TRX_SOJ_01
            ************************/
            c.retain(request_obj_);
            set_tx_req_class_id(request_obj_->get_nclass_id());
        } else {
            set_tx_req_class_id(0);
        }
    } else {
        set_tx_req_class_id(0);
    }
}

void transaction_int::set_current_obj(const nclass *val)
{
    if(current_obj_) {
        /************************
        RELEASE_ID: TRX_COJ_01
        ************************/
        vlg::collector &c = current_obj_->get_collector();
        c.release(current_obj_);
        current_obj_ = NULL;
    }
    if(val) {
        if((current_obj_ = val->clone())) {
            vlg::collector &c = current_obj_->get_collector();
            /************************
            RETAIN_ID: TRX_COJ_01
            ************************/
            c.retain(current_obj_);
            set_tx_req_class_id(current_obj_->get_nclass_id());
        }
    }
}

void transaction_int::set_result_obj(const nclass *val)
{
    if(result_obj_) {
        /************************
        RELEASE_ID: TRX_ROJ_01
        ************************/
        vlg::collector &c = result_obj_->get_collector();
        c.release(result_obj_);
        result_obj_ = NULL;
    }
    if(val) {
        if((result_obj_ = val->clone())) {
            vlg::collector &c = result_obj_->get_collector();
            /************************
            RETAIN_ID: TRX_ROJ_01
            ************************/
            c.retain(result_obj_);
            set_result_class_set(true);
            set_tx_res_class_id(result_obj_->get_nclass_id());
        } else {
            set_result_class_set(false);
            set_tx_res_class_id(0);
        }
    } else {
        set_result_class_set(false);
        set_tx_res_class_id(0);
    }
}

void transaction_int::set_request_obj_on_request(nclass *val)
{
    if(request_obj_) {
        /************************
        RELEASE_ID: TRX_SOJ_01
        ************************/
        vlg::collector &c = request_obj_->get_collector();
        c.release(request_obj_);
        request_obj_ = NULL;
    }
    if((request_obj_ = val)) {
        vlg::collector &c = request_obj_->get_collector();
        /************************
        RETAIN_ID: TRX_SOJ_01
        ************************/
        c.retain(request_obj_);
    }
}

void transaction_int::set_result_obj_on_response(nclass *val)
{
    if(result_obj_) {
        /************************
        RELEASE_ID: TRX_ROJ_01
        ************************/
        vlg::collector &c = result_obj_->get_collector();
        c.release(result_obj_);
        result_obj_ = NULL;
    }
    if((result_obj_ = val)) {
        vlg::collector &c = result_obj_->get_collector();
        /************************
        RETAIN_ID: TRX_ROJ_01
        ************************/
        c.retain(result_obj_);
        set_result_class_set(true);
    } else {
        set_result_class_set(false);
    }
}

void transaction_int::set_tx_id(tx_id &txid)
{
    txid_ = txid;
}

tx_id &transaction_int::txid()
{
    return txid_;
}

tx_id *transaction_int::tx_id_ptr()
{
    return &txid_;
}

unsigned int transaction_int::tx_id_PLID()
{
    return txid_.txplid;
}

unsigned int transaction_int::tx_id_SVID()
{
    return txid_.txsvid;
}

unsigned int transaction_int::tx_id_CNID()
{
    return txid_.txcnid;
}

unsigned int transaction_int::tx_id_PRID()
{
    return txid_.txprid;
}

void transaction_int::set_tx_id_PLID(unsigned int val)
{
    txid_.txplid = val;
}

void transaction_int::set_tx_id_SVID(unsigned int val)
{
    txid_.txsvid = val;
}

void transaction_int::set_tx_id_CNID(unsigned int val)
{
    txid_.txcnid = val;
}

void transaction_int::set_tx_id_PRID(unsigned int val)
{
    txid_.txprid = val;
}

void transaction_int::set_transaction_status_change_handler(
    transaction_status_change_hndlr hndlr, void *ud)
{
    tsc_hndl_ = hndlr;
    tsc_hndl_ud_ = ud;
}

void transaction_int::set_transaction_closure_handler(transaction_closure_hndlr
                                                      hndlr, void *ud)
{
    tres_hndl_ = hndlr;
    tres_hndl_ud_ = ud;
}

//-----------------------------
// BLZ_TRANSACTION ACTIONS
//
//-----------------------------

vlg::RetCode transaction_int::prepare()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ != TransactionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return vlg::RetCode_BADSTTS;
    }
    set_status(TransactionStatus_PREPARED);
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

vlg::RetCode transaction_int::prepare(TransactionRequestType txtype,
                                        Action txactn,
                                        Encode clsenc,
                                        bool rsclrq,
                                        const nclass *request_obj,
                                        const nclass *current_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ != TransactionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return vlg::RetCode_BADSTTS;
    }
    txtype_ = txtype;
    txactn_ = txactn;
    req_clsenc_ = res_clsenc_ = clsenc;
    rsclrq_ = rsclrq;
    set_request_obj(request_obj);
    set_current_obj(current_obj);
    set_status(TransactionStatus_PREPARED);
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

//-----------------------------
// BLZ_TRANSACTION SENDING METHS
//
//-----------------------------

vlg::RetCode transaction_int::send()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if(status_ != TransactionStatus_PREPARED) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return vlg::RetCode_BADSTTS;
    }
    //client side adoption to avoid user-space deletion.
    /************************
    RETAIN_ID: TRX_CLI_01
    ************************/
    vlg::collector &c = get_collector();
    c.retain(this);
    vlg::rt_mark_time(&start_mark_tim_);
    set_flying();
    transaction_int *self = this;
    if((cdrs_res = conn_.client_fly_tx_map().put(&txid_, &self))) {
        set_status(TransactionStatus_ERROR);
        IFLOG(err(TH_ID, LS_TRL"[error putting tx into flying map - res:%d]", cdrs_res))
        return cdrs_res;
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
    DECLINITH_GBB(gbb, BLZ_BUFF_DEF_SZ)
    build_PKT_TXRQST(txtype_,
                     txactn_,
                     &txid_,
                     rsclrq_,
                     req_clsenc_,
                     req_classid_,
                     conn_.connid(),
                     gbb);
    int totbytes = request_obj_ ? request_obj_->serialize(req_clsenc_, current_obj_,
                                                          gbb) : 0;
    totbytes = htonl(totbytes);
    if(request_obj_) {
        gbb->put(&totbytes, (6*4), 4);
    }
    gbb->flip();
    RETURN_IF_NOT_OK(conn_.pkt_snd_q().put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_SendPacket, &conn_);
    if((cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt))) {
        set_status(TransactionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

vlg::RetCode transaction_int::send_response()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if(status_ != TransactionStatus_FLYING) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return vlg::RetCode_BADSTTS;
    }
    DECLINITH_GBB(gbb, BLZ_BUFF_DEF_SZ)
    build_PKT_TXRESP(tx_res_,
                     result_code_,
                     &txid_,
                     rescls_,
                     res_clsenc_,
                     res_classid_,
                     gbb);
    int totbytes = result_obj_ ? result_obj_->serialize(res_clsenc_, NULL, gbb) : 0;
    totbytes = htonl(totbytes);
    if(result_obj_) {
        gbb->put(&totbytes, (6*4), 4);
    }
    gbb->flip();
    RETURN_IF_NOT_OK(conn_.pkt_snd_q().put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_SendPacket, &conn_);
    cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt);
    if(cdrs_res) {
        set_status(TransactionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p, res:%d)", __func__, this, cdrs_res))
    return cdrs_res;
}

//-----------------------------
// BLZ_TRANSACTION STATUS
//
//-----------------------------

vlg::RetCode transaction_int::set_flying()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(conn_.conn_type() == ConnectionType_INGOING) {
        if(status_ != TransactionStatus_INITIALIZED) {
            IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
            return vlg::RetCode_BADSTTS;
        }
        IFLOG(dbg(TH_ID, LS_TXI"[%08x%08x%08x%08x][FLYING]",
                  txid_.txplid,
                  txid_.txsvid,
                  txid_.txcnid,
                  txid_.txprid))
    } else {
        if(status_ != TransactionStatus_PREPARED) {
            IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
            return vlg::RetCode_BADSTTS;
        }
        IFLOG(dbg(TH_ID, LS_TXO"[%08x%08x%08x%08x][FLYING]",
                  txid_.txplid,
                  txid_.txsvid,
                  txid_.txcnid,
                  txid_.txprid))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    set_status(TransactionStatus_FLYING);
    return vlg::RetCode_OK;
}

inline void transaction_int::trace_tx_closure(const char *tx_res_str)
{
    char tim_buf[32];
    rt_time_t fin_mark_tim, dt_mark_tim;
    vlg::rt_mark_time(&fin_mark_tim);
    dt_mark_tim = vlg::rt_diff_time(start_mark_tim_, fin_mark_tim);
    snprintf(tim_buf, 32, "%14llu", dt_mark_tim);
    if(conn_.conn_type() == ConnectionType_INGOING) {
        if(request_obj_) {
            IFLOG(inf_class(TH_ID,
                            request_obj_,
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
            IFLOG(inf_class(TH_ID,
                            result_obj_,
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
            IFLOG(inf_class(TH_ID,
                            request_obj_,
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
            IFLOG(inf_class(TH_ID,
                            result_obj_,
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
              LS_TXR"[%08x%08x%08x%08x][%s][TXRES:%d, TXRESCODE:%d, RESCLS:%d][RTT-NS:%s]",
              txid_.txplid,
              txid_.txsvid,
              txid_.txcnid,
              txid_.txprid,
              tx_res_str,
              tx_res_,
              result_code_,
              rescls_,
              tim_buf))
}

vlg::RetCode transaction_int::set_closed()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ != TransactionStatus_FLYING) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return vlg::RetCode_BADSTTS;
    }
    const char *tx_res_str = (tx_res_ == TransactionResult_COMMITTED) ?
                             TX_RES_COMMT : TX_RES_FAIL;
    trace_tx_closure(tx_res_str);
    set_status(TransactionStatus_CLOSED);
    on_close();
    if(tres_hndl_) {
        tres_hndl_(*this, tres_hndl_ud_);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

vlg::RetCode transaction_int::set_aborted()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    set_tx_res(TransactionResult_ABORTED);
    const char *tx_res_str = TX_RES_ABORTED;
    trace_tx_closure(tx_res_str);
    set_status(TransactionStatus_CLOSED);
    on_close();
    if(tres_hndl_) {
        tres_hndl_(*this, tres_hndl_ud_);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

vlg::RetCode transaction_int::set_status(TransactionStatus status)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, status:%d)", __func__, this, status))
    CHK_MON_ERR_0(lock)
    status_ = status;
    if(tsc_hndl_) {
        tsc_hndl_(*this, status, tsc_hndl_ud_);
    }
    CHK_MON_ERR_0(notify_all)
    CHK_MON_ERR_0(unlock)
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return vlg::RetCode_OK;
}

vlg::RetCode transaction_int::await_for_status_reached_or_outdated(
    TransactionStatus test,
    TransactionStatus &current,
    time_t sec,
    long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, test:%d)", __func__, this, test))
    CHK_MON_ERR_0(lock)
    if(status_ < TransactionStatus_INITIALIZED) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return vlg::RetCode_BADSTTS;
    }
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    while(status_ < test) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res = vlg::RetCode_TIMEOUT;
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

vlg::RetCode transaction_int::await_for_closure(time_t sec, long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    CHK_MON_ERR_0(lock)
    if(status_ < TransactionStatus_INITIALIZED) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return vlg::RetCode_BADSTTS;
    }
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    while(status_ < TransactionStatus_CLOSED) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res = vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    IFLOG(log(cdrs_res ? vlg::TL_WRN : vlg::TL_DBG, TH_ID,
              LS_CLO "%s(ptr:%p, res:%d) - [closed %s]", __func__, this, cdrs_res,
              cdrs_res ? "not reached" : "reached"))
    CHK_MON_ERR_0(unlock)
    return cdrs_res;
}

//-----------------------------
// BLZ_TRANSACTION RECVING METHS
//
//-----------------------------

void transaction_int::on_request()
{}

void transaction_int::on_close()
{}

}
