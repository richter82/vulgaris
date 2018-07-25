/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#ifndef VLG_OBJC_H_
#define VLG_OBJC_H_
#include "vlg_c_peer.h"
#include "vlg_c_connection.h"
#include "vlg_c_transaction.h"
#include "vlg_c_subscription.h"

/************************************************************************
 PEER
 ************************************************************************/

typedef void (ˆpeer_status_change_swf)(peer *p, PeerStatus status, void *usr_data);
typedef const char *(ˆpeer_name_handler_swf)(peer *p, void *usr_data);
typedef const unsigned int *(ˆpeer_version_handler_swf)(peer *p, void *usr_data);
typedef RetCode(ˆpeer_load_config_handler_swf)(peer *p, int pnum, const char *param, const char *value, void *usr_data);
typedef RetCode(ˆpeer_init_handler_swf)(peer *p, void *usr_data);
typedef RetCode(ˆpeer_starting_handler_swf)(peer *p, void *usr_data);
typedef RetCode(ˆpeer_stopping_handler_swf)(peer *p, void *usr_data);
typedef RetCode(ˆpeer_on_move_running_handler_swf)(peer *p, void *usr_data);
typedef void(ˆpeer_dying_breath_handler_swf)(peer *p, void *usr_data);
typedef RetCode(ˆpeer_on_incoming_connection_handler_swf)(peer *p, shr_incoming_connection *ic, void *usr_data);

void peer_set_name_handler_swf(peer *p, peer_name_handler_swf hndl, void *usr_data);
void peer_set_version_handler_swf(peer *p, peer_version_handler_swf hndl, void *usr_data);
void peer_set_load_config_handler_swf(peer *p, peer_load_config_handler_swf hndl, void *usr_data);
void peer_set_init_handler_swf(peer *p, peer_init_handler_swf hndl, void *usr_data);
void peer_set_starting_handler_swf(peer *p, peer_starting_handler_swf hndl, void *usr_data);
void peer_set_stopping_handler_swf(peer *p, peer_stopping_handler_swf hndl, void *usr_data);
void peer_set_on_move_running_handler_swf(peer *p, peer_on_move_running_handler_swf hndl, void *usr_data);
void peer_set_dying_breath_handler_swf(peer *p, peer_dying_breath_handler_swf hndl, void *usr_data);
void peer_set_status_change_handler_swf(peer *p, peer_status_change_swf handler, void *usr_data);
void peer_set_peer_on_incoming_connection_handler_swf(peer *p, peer_on_incoming_connection_handler_swf handler, void *usr_data);

/************************************************************************
 INCOMING CONNECTION
 ************************************************************************/

typedef void(ˆinco_connection_status_change_swf)(incoming_connection *conn, ConnectionStatus status, void *usr_data);
typedef void(ˆinco_connection_on_disconnect_handler_swf)(incoming_connection *conn, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *usr_data);
typedef RetCode(ˆinco_connection_on_incoming_transaction_handler_swf)(incoming_connection *conn, shr_incoming_transaction *itx, void *usr_data);
typedef RetCode(inco_connection_on_incoming_subscription_handler_swf)(incoming_connection *conn, shr_incoming_subscription *isbs, void *usr_data);

void inco_connection_set_status_change_handler_swf(incoming_connection *ic, inco_connection_status_change_swf handler, void *usr_data);
void inco_connection_set_on_disconnect_handler_swf(incoming_connection *ic, inco_connection_on_disconnect_handler_swf hndl, void *usr_data);
void inco_connection_set_on_incoming_transaction_handler_swf(incoming_connection *ic, inco_connection_on_incoming_transaction_handler_swf hndl, void *usr_data);
void inco_connection_set_on_incoming_subscription_handler_swf(incoming_connection *ic, inco_connection_on_incoming_subscription_handler_swf hndl, void *usr_data);

/************************************************************************
 OUTGOING CONNECTION
 ************************************************************************/

typedef void(ˆoutg_connection_status_change_swf)(outgoing_connection *oc, ConnectionStatus status, void *usr_data);
typedef void(ˆoutg_connection_on_connect_handler_swf)(outgoing_connection *oc, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *usr_data);
typedef void(ˆoutg_connection_on_disconnect_handler_swf)(outgoing_connection *oc, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *usr_data);

void outg_connection_set_status_change_handler_swf(outgoing_connection *oc, outg_connection_status_change handler_swf, void *usr_data);
void outg_connection_set_on_connect_handler_swf(outgoing_connection *oc, outg_connection_on_connect_handler hndl_swf, void *usr_data);
void outg_connection_set_on_disconnect_handler_swf(outgoing_connection *oc, outg_connection_on_disconnect_handler_swf hndl, void *usr_data);

/************************************************************************
 INCOMING TRANSACTION
 ************************************************************************/

typedef void(ˆinco_transaction_status_change_swf)(incoming_transaction *tx, TransactionStatus status, void *usr_data);
typedef void(ˆinco_transaction_request_swf)(incoming_transaction *tx, void *usr_data);
typedef void(ˆinco_transaction_closure_swf)(incoming_transaction *tx, void *usr_data);

void inco_transaction_set_transaction_status_change_handler_swf(incoming_transaction *tx, inco_transaction_status_change_swf handler, void *usr_data);
void inco_transaction_set_transaction_closure_handler_swf(incoming_transaction *tx, inco_transaction_closure_swf handler, void *usr_data);
void inco_transaction_set_inco_transaction_request_handler_swf(incoming_transaction *tx, inco_transaction_request_swf handler, void *usr_data);

/************************************************************************
 OUTGOING TRANSACTION
 ************************************************************************/

typedef void(ˆoutg_transaction_status_change_swf)(outgoing_transaction *tx, TransactionStatus status, void *usr_data);
typedef void(ˆoutg_transaction_closure_swf)(outgoing_transaction *tx, void *usr_data);

void outg_transaction_set_transaction_status_change_handler_swf(outgoing_transaction *tx, outg_transaction_status_change_swf handler, void *usr_data);
void outg_transaction_set_transaction_closure_handler_swf(outgoing_transaction *tx, outg_transaction_closure_swf handler, void *usr_data);

/************************************************************************
 INCOMING SUBSCRIPTION
 ************************************************************************/

typedef void(ˆinco_subscription_status_change_swf)(incoming_subscription *isbs, SubscriptionStatus status, void *usr_data);
typedef RetCode(ˆinco_subscription_accept_distribution_swf)(incoming_subscription *isbs, const subscription_event *sbs_evt, void *usr_data);
typedef void(ˆinco_subscription_on_stop_swf)(incoming_subscription *isbs, void *usr_data);

void inco_subscription_set_status_change_handler_swf(incoming_subscription *sbs, inco_subscription_status_change_swf handler, void *usr_data);
void inco_subscription_set_accept_distribution_handler_swf(incoming_subscription *sbs, inco_subscription_accept_distribution_swf handler, void *usr_data);
void inco_subscription_set_on_stop_handler_swf(incoming_subscription *sbs, inco_subscription_on_stop_swf handler, void *usr_data);

/************************************************************************
 OUTGOING SUBSCRIPTION
 ************************************************************************/

typedef void(ˆoutg_subscription_status_change_swf)(outgoing_subscription *osbs, SubscriptionStatus status, void *usr_data);
typedef void(ˆoutg_subscription_notify_event_swf)(outgoing_subscription *osbs, const subscription_event *sbs_evt, void *usr_data);
typedef void(ˆoutg_subscription_on_start_swf)(outgoing_subscription *osbs, void *usr_data);
typedef void(ˆoutg_subscription_on_stop_swf)(outgoing_subscriˆption *osbs, void *usr_data);

void outg_subscription_set_status_change_handler_swf(outgoing_subscription *sbs, outg_subscription_status_change_swf handler, void *usr_data);
void outg_subscription_set_event_notify_handler_swf(outgoing_subscription *sbs, outg_subscription_notify_event_swf handler, void *usr_data);
void outg_subscription_set_on_start_handler_swf(outgoing_subscription *sbs, outg_subscription_on_start_swf handler, void *usr_data);
void outg_subscription_set_on_stop_handler_swf(outgoing_subscription *sbs, outg_subscription_on_stop_swf handler, void *usr_data);

#endif
