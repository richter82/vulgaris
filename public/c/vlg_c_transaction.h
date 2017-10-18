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

#ifndef BLZ_C_TRANSACTION_H_
#define BLZ_C_TRANSACTION_H_
#include "vlg.h"

#if defined(__cplusplus)
using namespace vlg;
extern "C" {
#endif

typedef void(*transaction_status_change_wr)(transaction_wr tx,
                                            TransactionStatus status,
                                            void *ud);

typedef void(*transaction_closure_wr)(transaction_wr tx,
                                      void *ud);

    transaction_wr transaction_create(void);

void transaction_destroy(transaction_wr tx);

RetCode transaction_bind(transaction_wr tx,
                         connection_wr conn);

connection_wr transaction_get_connection(transaction_wr tx);

TransactionResult transaction_get_transaction_result(transaction_wr tx);

ProtocolCode transaction_get_transaction_result_code(transaction_wr tx);

TransactionRequestType
transaction_get_transaction_request_type(transaction_wr tx);

Action transaction_get_transaction_action(transaction_wr tx);

unsigned int transaction_get_transaction_request_class_id(transaction_wr tx);

Encode transaction_get_transaction_request_class_encode(transaction_wr tx);

unsigned int transaction_get_transaction_result_class_id(transaction_wr tx);

Encode transaction_get_transaction_result_class_encode(transaction_wr tx);

int transaction_is_transaction_result_class_required(transaction_wr tx);

int transaction_is_transaction_result_class_set(transaction_wr tx);

net_class_wr  transaction_get_request_obj(transaction_wr tx);

net_class_wr  transaction_get_current_obj(transaction_wr tx);

net_class_wr  transaction_get_result_obj(transaction_wr tx);

void transaction_set_transaction_result(transaction_wr tx,
                                        TransactionResult tx_res);

void transaction_set_transaction_result_code(transaction_wr tx,
                                             ProtocolCode tx_res_code);

void transaction_set_transaction_request_type(transaction_wr tx,
                                              TransactionRequestType tx_req_type);

void transaction_set_transaction_action(transaction_wr tx,
                                        Action tx_act);

void transaction_set_transaction_request_class_id(transaction_wr tx,
                                                  unsigned int nclass_id);

void transaction_set_transaction_request_class_encode(transaction_wr tx,
                                                      Encode class_encode);

void transaction_set_transaction_result_class_id(transaction_wr tx,
                                                 unsigned int nclass_id);

void transaction_set_transaction_result_class_encode(transaction_wr tx,
                                                     Encode class_encode);

void transaction_set_transaction_result_class_required(transaction_wr tx,
                                                       int res_class_req);

void transaction_set_request_obj(transaction_wr tx, net_class_wr obj);

void transaction_set_current_obj(transaction_wr tx, net_class_wr obj);

void transaction_set_result_obj(transaction_wr tx, net_class_wr obj);

TransactionStatus transaction_get_status(transaction_wr tx);

RetCode transaction_await_for_status_reached_or_outdated(transaction_wr tx,
                                                         TransactionStatus test,
                                                         TransactionStatus *current,
                                                         time_t sec,
                                                         long nsec);

RetCode transaction_await_for_closure(transaction_wr tx,
                                      time_t sec,
                                      long nsec);

void transaction_set_transaction_status_change_handler(transaction_wr tx,
                                                       transaction_status_change_wr handler,
                                                       void *ud);

void transaction_set_transaction_closure_handler(transaction_wr tx,
                                                 transaction_closure_wr handler,
                                                 void *ud);

tx_id_wr *transaction_get_transaction_id(transaction_wr tx);

void transaction_set_transaction_id(transaction_wr tx,
                                    tx_id_wr *txid);

unsigned int transaction_get_transaction_id_PLID(transaction_wr tx);

unsigned int transaction_get_transaction_id_SVID(transaction_wr tx);

unsigned int transaction_get_transaction_id_CNID(transaction_wr tx);

unsigned int transaction_get_transaction_id_PRID(transaction_wr tx);

void transaction_set_transaction_id_PLID(transaction_wr tx,
                                         unsigned int plid);

void transaction_set_transaction_id_SVID(transaction_wr tx,
                                         unsigned int svid);

void transaction_set_transaction_id_CNID(transaction_wr tx,
                                         unsigned int cnid);

void transaction_set_transaction_id_PRID(transaction_wr tx,
                                         unsigned int prid);

RetCode transaction_renew(transaction_wr tx);

RetCode transaction_prepare(transaction_wr tx);

RetCode transaction_prepare_full(transaction_wr tx,
                                 TransactionRequestType tx_request_type,
                                 Action tx_action,
                                 net_class_wr sending_obj,
                                 net_class_wr current_obj);

RetCode transaction_send(transaction_wr tx);

#if defined(__cplusplus)
}
#endif

#endif
