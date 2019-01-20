/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
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

typedef void (^peer_on_status_change_oc)(peer *p, PeerStatus status, void *ud);
typedef const char *(^peer_name_oc)(peer *p, void *ud);
typedef const unsigned int *(^peer_version_oc)(peer *p, void *ud);
typedef RetCode(^peer_on_load_config_oc)(peer *p, int pnum, const char *param, const char *value, void *ud);
typedef RetCode(^peer_on_init_oc)(peer *p, void *ud);
typedef RetCode(^peer_on_starting_oc)(peer *p, void *ud);
typedef RetCode(^peer_on_stopping_oc)(peer *p, void *ud);
typedef RetCode(^peer_on_move_running_oc)(peer *p, void *ud);
typedef void(^peer_on_dying_breath_oc)(peer *p, void *ud);
typedef RetCode(^peer_on_incoming_connection_oc)(peer *p, shr_incoming_connection *ic, void *ud);

void peer_set_name_oc(peer *p, peer_name_oc hndl, void *ud);
void peer_set_version_oc(peer *p, peer_version_oc hndl, void *ud);
void peer_set_on_load_config_oc(peer *p, peer_on_load_config_oc hndl, void *ud);
void peer_set_on_init_oc(peer *p, peer_on_init_oc hndl, void *ud);
void peer_set_on_starting_oc(peer *p, peer_on_starting_oc hndl, void *ud);
void peer_set_on_stopping_oc(peer *p, peer_on_stopping_oc hndl, void *ud);
void peer_set_on_move_running_oc(peer *p, peer_on_move_running_oc hndl, void *ud);
void peer_set_on_dying_breath_oc(peer *p, peer_on_dying_breath_oc hndl, void *ud);
void peer_set_on_status_change_oc(peer *p, peer_on_status_change_oc hndl, void *ud);
void peer_set_on_incoming_connection_oc(peer *p, peer_on_incoming_connection_oc hndl, void *ud);

/************************************************************************
 INCOMING CONNECTION
 ************************************************************************/

typedef void(^inco_connection_on_status_change_oc)(incoming_connection *conn, ConnectionStatus status, void *ud);
typedef void(^inco_connection_on_disconnect_oc)(incoming_connection *conn, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *ud);
typedef RetCode(^inco_connection_on_incoming_transaction_oc)(incoming_connection *conn, shr_incoming_transaction *itx, void *ud);
typedef RetCode(^inco_connection_on_incoming_subscription_oc)(incoming_connection *conn, shr_incoming_subscription *isbs, void *ud);
typedef void(^inco_connection_on_releaseable_oc)(incoming_connection *conn, void *ud);

void inco_connection_set_on_status_change_oc(incoming_connection *ic, inco_connection_on_status_change_oc hndl, void *ud);
void inco_connection_set_on_disconnect_oc(incoming_connection *ic, inco_connection_on_disconnect_oc hndl, void *ud);
void inco_connection_set_on_incoming_transaction_oc(incoming_connection *ic, inco_connection_on_incoming_transaction_oc hndl, void *ud);
void inco_connection_set_on_incoming_subscription_oc(incoming_connection *ic, inco_connection_on_incoming_subscription_oc hndl, void *ud);
void inco_connection_set_on_releaseable_oc(incoming_connection *ic, inco_connection_on_releaseable_oc hndl, void *ud);

/************************************************************************
 OUTGOING CONNECTION
 ************************************************************************/

typedef void(^outg_connection_on_status_change_oc)(outgoing_connection *oc, ConnectionStatus status, void *ud);
typedef void(^outg_connection_on_connect_oc)(outgoing_connection *oc, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *ud);
typedef void(^outg_connection_on_disconnect_oc)(outgoing_connection *oc, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *ud);

void outg_connection_set_on_status_change_oc(outgoing_connection *oc, outg_connection_on_status_change_oc hndl, void *ud);
void outg_connection_set_on_connect_oc(outgoing_connection *oc, outg_connection_on_connect_oc hndl, void *ud);
void outg_connection_set_on_disconnect_oc(outgoing_connection *oc, outg_connection_on_disconnect_oc hndl, void *ud);

/************************************************************************
 INCOMING TRANSACTION
 ************************************************************************/

typedef void(^inco_transaction_on_status_change_oc)(incoming_transaction *tx, TransactionStatus status, void *ud);
typedef void(^inco_transaction_on_request_oc)(incoming_transaction *tx, void *ud);
typedef void(^inco_transaction_on_closure_oc)(incoming_transaction *tx, void *ud);
typedef void(^inco_transaction_on_releaseable_oc)(incoming_transaction *tx, void *ud);

void inco_transaction_set_on_status_change_oc(incoming_transaction *tx, inco_transaction_on_status_change_oc hndl, void *ud);
void inco_transaction_set_on_transaction_closure_oc(incoming_transaction *tx, inco_transaction_on_closure_oc hndl, void *ud);
void inco_transaction_set_on_request_oc(incoming_transaction *tx, inco_transaction_on_request_oc hndl, void *ud);
void inco_transaction_set_on_releaseable_oc(incoming_transaction *tx, inco_transaction_on_releaseable_oc hndl, void *ud);

/************************************************************************
 OUTGOING TRANSACTION
 ************************************************************************/

typedef void(^outg_transaction_on_status_change_oc)(outgoing_transaction *tx, TransactionStatus status, void *ud);
typedef void(^outg_transaction_on_closure_oc)(outgoing_transaction *tx, void *ud);

void outg_transaction_set_on_status_change_oc(outgoing_transaction *tx, outg_transaction_on_status_change_oc hndl, void *ud);
void outg_transaction_set_on_closure_oc(outgoing_transaction *tx, outg_transaction_on_closure_oc hndl, void *ud);

/************************************************************************
 INCOMING SUBSCRIPTION
 ************************************************************************/

typedef void(^inco_subscription_on_status_change_oc)(incoming_subscription *isbs, SubscriptionStatus status, void *ud);
typedef RetCode(^inco_subscription_on_accept_distribution_oc)(incoming_subscription *isbs, const subscription_event *sbs_evt, void *ud);
typedef void(^inco_subscription_on_stop_oc)(incoming_subscription *isbs, void *ud);
typedef void(^inco_subscription_on_releaseable_oc)(incoming_subscription *isbs, void *ud);

void inco_subscription_set_on_status_change_oc(incoming_subscription *sbs, inco_subscription_on_status_change_oc hndl, void *ud);
void inco_subscription_set_on_accept_distribution_oc(incoming_subscription *sbs, inco_subscription_on_accept_distribution_oc hndl, void *ud);
void inco_subscription_set_on_stop_oc(incoming_subscription *sbs, inco_subscription_on_stop_oc hndl, void *ud);
void inco_subscription_set_on_releaseable_oc(incoming_subscription *sbs, inco_subscription_on_releaseable_oc hndl, void *ud);

/************************************************************************
 OUTGOING SUBSCRIPTION
 ************************************************************************/

typedef void(^outg_subscription_on_status_change_oc)(outgoing_subscription *osbs, SubscriptionStatus status, void *ud);
typedef void(^outg_subscription_on_notify_event_oc)(outgoing_subscription *osbs, const subscription_event *sbs_evt, void *ud);
typedef void(^outg_subscription_on_start_oc)(outgoing_subscription *osbs, void *ud);
typedef void(^outg_subscription_on_stop_oc)(outgoing_subscription *osbs, void *ud);

void outg_subscription_set_on_status_change_oc(outgoing_subscription *sbs, outg_subscription_on_status_change_oc hndl, void *ud);
void outg_subscription_set_on_event_notify_oc(outgoing_subscription *sbs, outg_subscription_on_notify_event_oc hndl, void *ud);
void outg_subscription_set_on_start_oc(outgoing_subscription *sbs, outg_subscription_on_start_oc hndl, void *ud);
void outg_subscription_set_on_stop_oc(outgoing_subscription *sbs, outg_subscription_on_stop_oc hndl, void *ud);

#endif
