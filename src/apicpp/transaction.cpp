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

#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg/conn_impl.h"
#include "vlg/tx_impl.h"

namespace vlg {

transaction::transaction() : impl_(new transaction_impl(*this))
{
    CTOR_TRC
}

transaction::~transaction()
{
    DTOR_TRC
}

RetCode transaction::bind(connection &conn)
{
    impl_->conn_ = &conn;
    return RetCode_OK;
}

connection &transaction::get_connection()
{
    return *impl_->conn_;
}

TransactionResult transaction::get_close_result()
{
    return impl_->tx_res_;
}

ProtocolCode transaction::get_close_result_code()
{
    return impl_->result_code_;
}

TransactionRequestType transaction::get_request_type()
{
    return impl_->txtype_;
}

Action transaction::get_request_action()
{
    return impl_->txactn_;
}

unsigned int transaction::get_request_nclass_id()
{
    return impl_->req_nclassid_;
}

Encode transaction::get_request_nclass_encode()
{
    return impl_->req_clsenc_;
}

unsigned int transaction::get_result_nclass_id()
{
    return impl_->res_nclassid_;
}

Encode transaction::get_result_nclass_encode()
{
    return impl_->res_clsenc_;
}

bool transaction::is_result_obj_required()
{
    return impl_->rsclrq_;
}

bool transaction::is_result_obj_set()
{
    return impl_->rescls_;
}

const nclass *transaction::get_request_obj()
{
    return impl_->request_obj_.get();
}

const nclass *transaction::get_current_obj()
{
    return impl_->current_obj_.get();
}

const nclass *transaction::get_result_obj()
{
    return impl_->result_obj_.get();
}

void transaction::set_result(TransactionResult tx_res)
{
    impl_->tx_res_ = tx_res;
}

void transaction::set_result_code(ProtocolCode tx_res_code)
{
    impl_->result_code_ = tx_res_code;
}

void transaction::set_request_type(TransactionRequestType tx_req_type)
{
    impl_->txtype_ = tx_req_type;
}

void transaction::set_request_action(Action tx_act)
{
    impl_->txactn_ = tx_act;
}

void transaction::set_request_nclass_id(unsigned int nclass_id)
{
    impl_->req_nclassid_ = nclass_id;
}

void transaction::set_request_nclass_encode(Encode nclass_encode)
{
    impl_->req_clsenc_ = nclass_encode;
}

void transaction::set_result_nclass_id(unsigned int nclass_id)
{
    impl_->res_nclassid_ = nclass_id;
}

void transaction::set_result_nclass_encode(Encode nclass_encode)
{
    impl_->res_clsenc_ = nclass_encode;
}

void transaction::set_result_obj_required(bool res_class_req)
{
    impl_->rescls_ = res_class_req;
}

void transaction::set_request_obj(const nclass &obj)
{
    impl_->txtype_ = TransactionRequestType_OBJECT;
    impl_->set_request_obj(obj);
}

void transaction::set_current_obj(const nclass &obj)
{
    impl_->set_current_obj(obj);
}

void transaction::set_result_obj(const nclass &obj)
{
    impl_->set_result_obj(obj);
}

TransactionStatus transaction::get_status()
{
    return impl_->status_;
}

RetCode transaction::await_for_status_reached_or_outdated(TransactionStatus test,
                                                          TransactionStatus &current,
                                                          time_t sec,
                                                          long nsec)
{
    return impl_->await_for_status_reached_or_outdated(test,
                                                       current,
                                                       sec,
                                                       nsec);
}

RetCode transaction::await_for_close(time_t sec, long nsec)
{
    return  impl_->await_for_closure(sec, nsec);
}

tx_id &transaction::get_id()
{
    return  impl_->txid_;
}

void transaction::set_id(tx_id &txid)
{
    impl_->txid_ = txid;
}

RetCode transaction::renew()
{
    return impl_->re_new();
}

RetCode transaction::send()
{
    return impl_->send();
}

void transaction::on_status_change(TransactionStatus current)
{}

void transaction::on_request()
{}

void transaction::on_close()
{}

}
