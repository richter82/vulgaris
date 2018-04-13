/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 * 
 */

#ifndef VLG_C_TRANSACTION_H_
#define VLG_C_TRANSACTION_H_
#include "vlg.h"

/************************************************************************
INCOMING TRANSACTION HANDLERS
************************************************************************/

typedef void(*inco_transaction_status_change)(incoming_transaction *tx, TransactionStatus status, void *ud);
typedef void(*inco_transaction_request)(incoming_transaction *tx, void *ud);
typedef void(*inco_transaction_closure)(incoming_transaction *tx, void *ud);

/************************************************************************
INCOMING TRANSACTION
************************************************************************/

void inco_transaction_release(shr_incoming_transaction *tx);
incoming_transaction *inco_transaction_get_ptr(shr_incoming_transaction *tx);
incoming_connection *inco_transaction_get_connection(incoming_transaction *tx);
TransactionResult inco_transaction_get_transaction_result(incoming_transaction *tx);
ProtocolCode inco_transaction_get_transaction_result_code(incoming_transaction *tx);
TransactionRequestType inco_transaction_get_transaction_request_type(incoming_transaction *tx);
Action inco_transaction_get_transaction_action(incoming_transaction *tx);
unsigned int inco_transaction_get_transaction_request_nclass_id(incoming_transaction *tx);
Encode inco_transaction_get_transaction_request_nclass_encode(incoming_transaction *tx);
unsigned int inco_transaction_get_transaction_result_nclass_id(incoming_transaction *tx);
Encode inco_transaction_get_transaction_result_nclass_encode(incoming_transaction *tx);
int inco_transaction_is_transaction_result_nclass_required(incoming_transaction *tx);
int inco_transaction_is_transaction_result_nclass_set(incoming_transaction *tx);
const nclass *inco_transaction_get_request_obj(incoming_transaction *tx);
const nclass *inco_transaction_get_current_obj(incoming_transaction *tx);
const nclass *inco_transaction_get_result_obj(incoming_transaction *tx);
void inco_transaction_set_transaction_result(incoming_transaction *tx, TransactionResult tx_res);
void inco_transaction_set_transaction_result_code(incoming_transaction *tx, ProtocolCode tx_res_code);
void inco_transaction_set_transaction_result_nclass_id(incoming_transaction *tx, unsigned int nclass_id);
void inco_transaction_set_transaction_result_nclass_encode(incoming_transaction *tx, Encode nclass_encode);
void inco_transaction_set_current_obj(incoming_transaction *tx, nclass *obj);
void inco_transaction_set_result_obj(incoming_transaction *tx, nclass *obj);
TransactionStatus inco_transaction_get_status(incoming_transaction *tx);
RetCode inco_transaction_await_for_status_reached(incoming_transaction *tx, TransactionStatus test, TransactionStatus *current, time_t sec, long nsec);
RetCode inco_transaction_await_for_closure(incoming_transaction *tx, time_t sec, long nsec);
void inco_transaction_set_transaction_status_change_handler(incoming_transaction *tx, inco_transaction_status_change handler, void *ud);
void inco_transaction_set_transaction_closure_handler(incoming_transaction *tx, inco_transaction_closure handler, void *ud);
void inco_transaction_set_inco_transaction_request_handler(incoming_transaction *tx, inco_transaction_request handler, void *ud);
tx_id *inco_transaction_get_transaction_id(incoming_transaction *tx);
void inco_transaction_set_transaction_id(incoming_transaction *tx, tx_id *txid);

/************************************************************************
OUTGOING TRANSACTION HANDLERS
************************************************************************/

typedef void(*outg_transaction_status_change)(outgoing_transaction *tx, TransactionStatus status, void *ud);
typedef void(*outg_transaction_closure)(outgoing_transaction *tx, void *ud);

/************************************************************************
OUTGOING TRANSACTION
************************************************************************/

own_outgoing_transaction *outg_transaction_create(void);
outgoing_transaction *outg_transaction_get_ptr(own_outgoing_transaction *tx);
void outg_transaction_destroy(own_outgoing_transaction *tx);
RetCode outg_transaction_bind(outgoing_transaction *tx, outgoing_connection *conn);
outgoing_connection *outg_transaction_get_connection(outgoing_transaction *tx);
TransactionResult outg_transaction_get_transaction_result(outgoing_transaction *tx);
ProtocolCode outg_transaction_get_transaction_result_code(outgoing_transaction *tx);
TransactionRequestType outg_transaction_get_transaction_request_type(outgoing_transaction *tx);
Action outg_transaction_get_transaction_action(outgoing_transaction *tx);
unsigned int outg_transaction_get_transaction_request_nclass_id(outgoing_transaction *tx);
Encode outg_transaction_get_transaction_request_nclass_encode(outgoing_transaction *tx);
unsigned int outg_transaction_get_transaction_result_nclass_id(outgoing_transaction *tx);
Encode outg_transaction_get_transaction_result_nclass_encode(outgoing_transaction *tx);
int outg_transaction_is_transaction_result_nclass_required(outgoing_transaction *tx);
int outg_transaction_is_transaction_result_nclass_set(outgoing_transaction *tx);
const nclass *outg_transaction_get_request_obj(outgoing_transaction *tx);
const nclass *outg_transaction_get_current_obj(outgoing_transaction *tx);
const nclass *outg_transaction_get_result_obj(outgoing_transaction *tx);
void outg_transaction_set_transaction_result(outgoing_transaction *tx, TransactionResult tx_res);
void outg_transaction_set_transaction_result_code(outgoing_transaction *tx, ProtocolCode tx_res_code);
void outg_transaction_set_transaction_request_type(outgoing_transaction *tx, TransactionRequestType tx_req_type);
void outg_transaction_set_transaction_action(outgoing_transaction *tx, Action tx_act);
void outg_transaction_set_transaction_request_nclass_id(outgoing_transaction *tx, unsigned int nclass_id);
void outg_transaction_set_transaction_request_nclass_encode(outgoing_transaction *tx, Encode nclass_encode);
void outg_transaction_set_transaction_result_nclass_required(outgoing_transaction *tx, int res_nclass_req);
void outg_transaction_set_request_obj(outgoing_transaction *tx, nclass *obj);
void outg_transaction_set_current_obj(outgoing_transaction *tx, nclass *obj);
TransactionStatus outg_transaction_get_status(outgoing_transaction *tx);
RetCode outg_transaction_await_for_status_reached(outgoing_transaction *tx, TransactionStatus test, TransactionStatus *current, time_t sec, long nsec);
RetCode outg_transaction_await_for_closure(outgoing_transaction *tx, time_t sec, long nsec);
void outg_transaction_set_transaction_status_change_handler(outgoing_transaction *tx, outg_transaction_status_change handler, void *ud);
void outg_transaction_set_transaction_closure_handler(outgoing_transaction *tx, outg_transaction_closure handler, void *ud);
tx_id *outg_transaction_get_transaction_id(outgoing_transaction *tx);
void outg_transaction_set_transaction_id(outgoing_transaction *tx, tx_id *txid);
RetCode outg_transaction_renew(outgoing_transaction *tx);
RetCode outg_transaction_send(outgoing_transaction *tx);

#endif
