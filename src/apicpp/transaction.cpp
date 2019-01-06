/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg/brk_impl.h"
#include "vlg/conn_impl.h"
#include "vlg/tx_impl.h"

namespace vlg {

struct default_incoming_transaction_listener : public incoming_transaction_listener {
    virtual void on_status_change(incoming_transaction &, TransactionStatus) override {}
    virtual void on_request(incoming_transaction &) override {}
    virtual void on_close(incoming_transaction &) override {}
    virtual void on_releaseable(vlg::incoming_transaction &) override {}
};

static default_incoming_transaction_listener ditl;

incoming_transaction_listener &incoming_transaction_listener::default_listener()
{
    return ditl;
}

incoming_transaction::incoming_transaction(std::shared_ptr<incoming_connection> &conn,
                                           incoming_transaction_listener &listener) :
    impl_(new incoming_transaction_impl(*this, conn, listener))
{}

incoming_transaction::~incoming_transaction()
{
    DTOR_TRC(impl_->conn_->broker_->log_)
}

incoming_connection &incoming_transaction::get_connection()
{
    return *impl_->conn_sh_;
}

TransactionResult incoming_transaction::get_close_result()
{
    return impl_->tx_res_;
}

ProtocolCode incoming_transaction::get_close_result_code()
{
    return impl_->result_code_;
}

TransactionRequestType incoming_transaction::get_request_type()
{
    return impl_->txtype_;
}

Action incoming_transaction::get_request_action()
{
    return impl_->txactn_;
}

unsigned int incoming_transaction::get_request_nclass_id()
{
    return impl_->req_nclassid_;
}

Encode incoming_transaction::get_request_nclass_encode()
{
    return impl_->req_clsenc_;
}

unsigned int incoming_transaction::get_result_nclass_id()
{
    return impl_->res_nclassid_;
}

Encode incoming_transaction::get_result_nclass_encode()
{
    return impl_->res_clsenc_;
}

bool incoming_transaction::is_result_obj_required()
{
    return impl_->rsclrq_;
}

bool incoming_transaction::is_result_obj_set()
{
    return impl_->rescls_;
}

const nclass *incoming_transaction::get_request_obj()
{
    return impl_->request_obj_.get();
}

const nclass *incoming_transaction::get_current_obj()
{
    return impl_->current_obj_.get();
}

const nclass *incoming_transaction::get_result_obj()
{
    return impl_->result_obj_.get();
}

void incoming_transaction::set_result(TransactionResult tx_res)
{
    impl_->tx_res_ = tx_res;
}

void incoming_transaction::set_result_code(ProtocolCode tx_res_code)
{
    impl_->result_code_ = tx_res_code;
}

void incoming_transaction::set_result_nclass_id(unsigned int nclass_id)
{
    impl_->res_nclassid_ = nclass_id;
}

void incoming_transaction::set_result_nclass_encode(Encode nclass_encode)
{
    impl_->res_clsenc_ = nclass_encode;
}

void incoming_transaction::set_current_obj(const nclass &obj)
{
    impl_->set_current_obj(obj);
}

void incoming_transaction::set_result_obj(const nclass &obj)
{
    impl_->set_result_obj(obj);
}

TransactionStatus incoming_transaction::get_status()
{
    return impl_->status_;
}

RetCode incoming_transaction::await_for_status_reached(TransactionStatus test,
                                                       TransactionStatus &current,
                                                       time_t sec,
                                                       long nsec)
{
    return impl_->await_for_status_reached(test,
                                           current,
                                           sec,
                                           nsec);
}

RetCode incoming_transaction::await_for_close(time_t sec, long nsec)
{
    return  impl_->await_for_closure(sec, nsec);
}

tx_id &incoming_transaction::get_id()
{
    return  impl_->txid_;
}

void incoming_transaction::set_id(tx_id &txid)
{
    impl_->txid_ = txid;
}

}

namespace vlg {

struct default_outgoing_transaction_listener : public outgoing_transaction_listener {
    virtual void on_status_change(outgoing_transaction &, TransactionStatus) override {}
    virtual void on_close(outgoing_transaction &) override {}
};

static default_outgoing_transaction_listener dotl;

outgoing_transaction_listener &outgoing_transaction_listener::default_listener()
{
    return dotl;
}

outgoing_transaction::outgoing_transaction(outgoing_transaction_listener &listener) :
    impl_(new outgoing_transaction_impl(*this, listener))
{}

outgoing_transaction::~outgoing_transaction()
{
    DTOR_TRC(impl_->conn_->broker_->log_)
}

RetCode outgoing_transaction::bind(outgoing_connection &conn)
{
    impl_->conn_ = conn.impl_.get();
    return RetCode_OK;
}

outgoing_connection &outgoing_transaction::get_connection()
{
    return *impl_->conn_->opubl_;
}

TransactionResult outgoing_transaction::get_close_result()
{
    return impl_->tx_res_;
}

ProtocolCode outgoing_transaction::get_close_result_code()
{
    return impl_->result_code_;
}

TransactionRequestType outgoing_transaction::get_request_type()
{
    return impl_->txtype_;
}

Action outgoing_transaction::get_request_action()
{
    return impl_->txactn_;
}

unsigned int outgoing_transaction::get_request_nclass_id()
{
    return impl_->req_nclassid_;
}

Encode outgoing_transaction::get_request_nclass_encode()
{
    return impl_->req_clsenc_;
}

unsigned int outgoing_transaction::get_result_nclass_id()
{
    return impl_->res_nclassid_;
}

Encode outgoing_transaction::get_result_nclass_encode()
{
    return impl_->res_clsenc_;
}

bool outgoing_transaction::is_result_obj_required()
{
    return impl_->rsclrq_;
}

bool outgoing_transaction::is_result_obj_set()
{
    return impl_->rescls_;
}

const nclass *outgoing_transaction::get_request_obj()
{
    return impl_->request_obj_.get();
}

const nclass *outgoing_transaction::get_current_obj()
{
    return impl_->current_obj_.get();
}

const nclass *outgoing_transaction::get_result_obj()
{
    return impl_->result_obj_.get();
}

void outgoing_transaction::set_result(TransactionResult tx_res)
{
    impl_->tx_res_ = tx_res;
}

void outgoing_transaction::set_result_code(ProtocolCode tx_res_code)
{
    impl_->result_code_ = tx_res_code;
}

void outgoing_transaction::set_request_type(TransactionRequestType tx_req_type)
{
    impl_->txtype_ = tx_req_type;
}

void outgoing_transaction::set_request_action(Action tx_act)
{
    impl_->txactn_ = tx_act;
}

void outgoing_transaction::set_request_nclass_id(unsigned int nclass_id)
{
    impl_->req_nclassid_ = nclass_id;
}

void outgoing_transaction::set_request_nclass_encode(Encode nclass_encode)
{
    impl_->req_clsenc_ = nclass_encode;
}

void outgoing_transaction::set_result_obj_required(bool res_nclass_req)
{
    impl_->rescls_ = res_nclass_req;
}

void outgoing_transaction::set_request_obj(const nclass &obj)
{
    impl_->txtype_ = TransactionRequestType_OBJECT;
    impl_->set_request_obj(obj);
}

void outgoing_transaction::set_current_obj(const nclass &obj)
{
    impl_->set_current_obj(obj);
}

TransactionStatus outgoing_transaction::get_status()
{
    return impl_->status_;
}

RetCode outgoing_transaction::await_for_status_reached(TransactionStatus test,
                                                       TransactionStatus &current,
                                                       time_t sec,
                                                       long nsec)
{
    return impl_->await_for_status_reached(test,
                                           current,
                                           sec,
                                           nsec);
}

RetCode outgoing_transaction::await_for_close(time_t sec, long nsec)
{
    return  impl_->await_for_closure(sec, nsec);
}

tx_id &outgoing_transaction::get_id()
{
    return  impl_->txid_;
}

void outgoing_transaction::set_id(tx_id &txid)
{
    impl_->txid_ = txid;
}

RetCode outgoing_transaction::renew()
{
    return impl_->re_new();
}

RetCode outgoing_transaction::send()
{
    return impl_->send();
}

}
