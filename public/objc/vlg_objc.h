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

typedef void (^peer_status_change_swf)(peer *p, PeerStatus status, void *ud);
typedef const char *(^peer_name_swf)(peer *p, void *ud);
typedef const unsigned int *(^peer_version_swf)(peer *p, void *ud);
typedef RetCode(^peer_load_config_swf)(peer *p, int pnum, const char *param, const char *value, void *ud);
typedef RetCode(^peer_init_swf)(peer *p, void *ud);
typedef RetCode(^peer_starting_swf)(peer *p, void *ud);
typedef RetCode(^peer_stopping_swf)(peer *p, void *ud);
typedef RetCode(^peer_on_move_running_swf)(peer *p, void *ud);
typedef void(^peer_dying_breath_swf)(peer *p, void *ud);
typedef RetCode(^peer_on_incoming_connection_swf)(peer *p, shr_incoming_connection *ic, void *ud);

void peer_set_name_swf(peer *p, peer_name_swf hndl, void *ud);
void peer_set_version_swf(peer *p, peer_version_swf hndl, void *ud);
void peer_set_load_config_swf(peer *p, peer_load_config_swf hndl, void *ud);
void peer_set_init_swf(peer *p, peer_init_swf hndl, void *ud);
void peer_set_starting_swf(peer *p, peer_starting_swf hndl, void *ud);
void peer_set_stopping_swf(peer *p, peer_stopping_swf hndl, void *ud);
void peer_set_on_move_running_swf(peer *p, peer_on_move_running_swf hndl, void *ud);
void peer_set_dying_breath_swf(peer *p, peer_dying_breath_swf hndl, void *ud);
void peer_set_status_change_swf(peer *p, peer_status_change_swf hndl, void *ud);
void peer_set_peer_on_incoming_connection_swf(peer *p, peer_on_incoming_connection_swf hndl, void *ud);

/************************************************************************
 INCOMING CONNECTION
 ************************************************************************/

typedef void(^inco_connection_status_change_swf)(incoming_connection *conn, ConnectionStatus status, void *ud);
typedef void(^inco_connection_on_disconnect_swf)(incoming_connection *conn, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *ud);
typedef RetCode(^inco_connection_on_incoming_transaction_swf)(incoming_connection *conn, shr_incoming_transaction *itx, void *ud);
typedef RetCode(^inco_connection_on_incoming_subscription_swf)(incoming_connection *conn, shr_incoming_subscription *isbs, void *ud);
typedef void(^inco_connection_on_destroy_swf)(incoming_connection *conn, void *ud);

void inco_connection_set_status_change_swf(incoming_connection *ic, inco_connection_status_change_swf hndl, void *ud);
void inco_connection_set_on_disconnect_swf(incoming_connection *ic, inco_connection_on_disconnect_swf hndl, void *ud);
void inco_connection_set_on_incoming_transaction_swf(incoming_connection *ic, inco_connection_on_incoming_transaction_swf hndl, void *ud);
void inco_connection_set_on_incoming_subscription_swf(incoming_connection *ic, inco_connection_on_incoming_subscription_swf hndl, void *ud);
void inco_connection_set_on_destroy_swf(incoming_connection *ic, inco_connection_on_destroy_swf hndl, void *ud);

/************************************************************************
 OUTGOING CONNECTION
 ************************************************************************/

typedef void(^outg_connection_status_change_swf)(outgoing_connection *oc, ConnectionStatus status, void *ud);
typedef void(^outg_connection_on_connect_swf)(outgoing_connection *oc, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *ud);
typedef void(^outg_connection_on_disconnect_swf)(outgoing_connection *oc, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *ud);

void outg_connection_set_status_change_swf(outgoing_connection *oc, outg_connection_status_change_swf hndl, void *ud);
void outg_connection_set_on_connect_swf(outgoing_connection *oc, outg_connection_on_connect_swf hndl, void *ud);
void outg_connection_set_on_disconnect_swf(outgoing_connection *oc, outg_connection_on_disconnect_swf hndl, void *ud);

/************************************************************************
 INCOMING TRANSACTION
 ************************************************************************/

typedef void(^inco_transaction_status_change_swf)(incoming_transaction *tx, TransactionStatus status, void *ud);
typedef void(^inco_transaction_request_swf)(incoming_transaction *tx, void *ud);
typedef void(^inco_transaction_closure_swf)(incoming_transaction *tx, void *ud);
typedef void(^inco_transaction_on_destroy_swf)(incoming_transaction *tx, void *ud);

void inco_transaction_set_transaction_status_change_swf(incoming_transaction *tx, inco_transaction_status_change_swf hndl, void *ud);
void inco_transaction_set_transaction_closure_swf(incoming_transaction *tx, inco_transaction_closure_swf hndl, void *ud);
void inco_transaction_set_inco_transaction_request_swf(incoming_transaction *tx, inco_transaction_request_swf hndl, void *ud);
void inco_transaction_set_on_destroy_swf(incoming_transaction *tx, inco_transaction_on_destroy_swf hndl, void *ud);

/************************************************************************
 OUTGOING TRANSACTION
 ************************************************************************/

typedef void(^outg_transaction_status_change_swf)(outgoing_transaction *tx, TransactionStatus status, void *ud);
typedef void(^outg_transaction_closure_swf)(outgoing_transaction *tx, void *ud);

void outg_transaction_set_transaction_status_change_swf(outgoing_transaction *tx, outg_transaction_status_change_swf hndl, void *ud);
void outg_transaction_set_transaction_closure_swf(outgoing_transaction *tx, outg_transaction_closure_swf hndl, void *ud);

/************************************************************************
 INCOMING SUBSCRIPTION
 ************************************************************************/

typedef void(^inco_subscription_status_change_swf)(incoming_subscription *isbs, SubscriptionStatus status, void *ud);
typedef RetCode(^inco_subscription_accept_distribution_swf)(incoming_subscription *isbs, const subscription_event *sbs_evt, void *ud);
typedef void(^inco_subscription_on_stop_swf)(incoming_subscription *isbs, void *ud);
typedef void(^inco_subscription_on_destroy_swf)(incoming_subscription *isbs, void *ud);

void inco_subscription_set_status_change_swf(incoming_subscription *sbs, inco_subscription_status_change_swf hndl, void *ud);
void inco_subscription_set_accept_distribution_swf(incoming_subscription *sbs, inco_subscription_accept_distribution_swf hndl, void *ud);
void inco_subscription_set_on_stop_swf(incoming_subscription *sbs, inco_subscription_on_stop_swf hndl, void *ud);
void inco_subscription_set_on_destroy_swf(incoming_subscription *sbs, inco_subscription_on_destroy_swf hndl, void *ud);

/************************************************************************
 OUTGOING SUBSCRIPTION
 ************************************************************************/

typedef void(^outg_subscription_status_change_swf)(outgoing_subscription *osbs, SubscriptionStatus status, void *ud);
typedef void(^outg_subscription_notify_event_swf)(outgoing_subscription *osbs, const subscription_event *sbs_evt, void *ud);
typedef void(^outg_subscription_on_start_swf)(outgoing_subscription *osbs, void *ud);
typedef void(^outg_subscription_on_stop_swf)(outgoing_subscription *osbs, void *ud);

void outg_subscription_set_status_change_swf(outgoing_subscription *sbs, outg_subscription_status_change_swf hndl, void *ud);
void outg_subscription_set_event_notify_swf(outgoing_subscription *sbs, outg_subscription_notify_event_swf hndl, void *ud);
void outg_subscription_set_on_start_swf(outgoing_subscription *sbs, outg_subscription_on_start_swf hndl, void *ud);
void outg_subscription_set_on_stop_swf(outgoing_subscription *sbs, outg_subscription_on_stop_swf hndl, void *ud);

#endif
