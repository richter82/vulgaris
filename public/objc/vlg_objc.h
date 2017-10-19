//
//  vlg_os_support.h
//
//  Created by Giuseppe Baccini on 27/04/15.
//  Copyright (c) 2015 vlg. All rights reserved.
//

#ifndef vlg_os_support_h
#define vlg_os_support_h

#import "vlg_c_peer.h"
#import "vlg_c_connection.h"
#import "vlg_c_transaction.h"
#import "vlg_c_subscription.h"
#import "vlg_c_persistence.h"

/*******************************************************************************
 peer
 ******************************************************************************/

typedef void                (^peer_status_change_swift)(peer_wr p, PeerStatus status);
typedef const char          *(^peer_name_handler_swift)(peer_wr p);
typedef const unsigned int  *(^peer_version_handler_swift)(peer_wr p);
typedef RetCode                (^peer_load_config_handler_swift)(peer_wr p, int pnum, const char *param, const char *value);
typedef RetCode                (^peer_init_handler_swift)(peer_wr p);
typedef RetCode                (^peer_starting_handler_swift)(peer_wr p);
typedef RetCode                (^peer_stopping_handler_swift)(peer_wr p);
typedef RetCode                (^peer_transit_on_air_handler_swift)(peer_wr p);
typedef void                (^peer_error_handler_swift)(peer_wr p);
typedef void                (^peer_dying_breath_handler_swift)(peer_wr p);

//------------------------------------------------------------------------------
// NAME / VERSION
//------------------------------------------------------------------------------
void peer_set_name_handler_swift(peer_wr p, peer_name_handler_swift cllbk);
void peer_set_version_handler_swift(peer_wr p, peer_version_handler_swift cllbk);

//------------------------------------------------------------------------------
// LIFECYCLE
//------------------------------------------------------------------------------
void peer_set_load_config_handler_swift(peer_wr p, peer_load_config_handler_swift cllbk);
void peer_set_init_handler_swift(peer_wr p, peer_init_handler_swift cllbk);
void peer_set_starting_handler_swift(peer_wr p, peer_starting_handler_swift cllbk);
void peer_set_stopping_handler_swift(peer_wr p, peer_stopping_handler_swift cllbk);
void peer_set_transit_on_air_handler_swift(peer_wr p, peer_transit_on_air_handler_swift cllbk);
void peer_set_error_handler_swift(peer_wr p, peer_error_handler_swift cllbk);
void peer_set_dying_breath_handler_swift(peer_wr p, peer_dying_breath_handler_swift cllbk);

//------------------------------------------------------------------------------
// STATUS
//------------------------------------------------------------------------------
void peer_set_status_change_handler_swift(peer_wr p, peer_status_change_swift cllbk);


/*******************************************************************************
 connection
 ******************************************************************************/

typedef void (^connection_status_change_swift)	(connection_wr conn, ConnectionStatus status, void *ud);
typedef void (^on_connect_handler_swift)		(connection_wr conn, ConnectivityEventResult con_evt_res, ConnectivityEventType connectivity_evt_type, void *ud);
typedef void (^on_disconnect_handler_swift)		(connection_wr conn, ConnectivityEventResult con_evt_res, ConnectivityEventType connectivity_evt_type, void *ud);

//------------------------------------------------------------------------------
// CONNECTIVITY
//------------------------------------------------------------------------------
void connection_set_on_connect_handler_swift(connection_wr conn, on_connect_handler_swift cllbk, void *ud);
void connection_set_on_disconnect_handler_swift(connection_wr conn, on_disconnect_handler_swift cllbk, void *ud);

//------------------------------------------------------------------------------
// STATUS
//------------------------------------------------------------------------------
void connection_set_status_change_handler_swift(connection_wr conn, connection_status_change_swift cllbk, void *ud);


/*******************************************************************************
 transaction
 ******************************************************************************/

typedef void (^transaction_status_change_swift)	(transaction_wr tx, TransactionStatus status, void *ud);
typedef void (^transaction_closure_swift)		(transaction_wr tx, void *ud);

//------------------------------------------------------------------------------
// STATUS
//------------------------------------------------------------------------------
void transaction_set_transaction_status_change_handler_swift(transaction_wr tx, transaction_status_change_swift cllbk, void *ud);

//------------------------------------------------------------------------------
// TX CLOSURE
//------------------------------------------------------------------------------
void transaction_set_transaction_closure_handler_swift(transaction_wr tx, transaction_closure_swift cllbk, void *ud);


/*******************************************************************************
 subscription
 ******************************************************************************/

typedef void (^subscription_status_change_swift)(subscription_wr sbs, SubscriptionStatus status, void *ud);
typedef void (^subscription_event_notify_swift)(subscription_wr sbs, subscription_event_wr sbs_evt, void *ud);

//------------------------------------------------------------------------------
// STATUS
//------------------------------------------------------------------------------
void subscription_set_status_change_handler_swift(subscription_wr sbs, subscription_status_change_swift cllbk, void *ud);

//------------------------------------------------------------------------------
// EVENT NOTIFY
//------------------------------------------------------------------------------
void subscription_set_event_notify_handler_swift(subscription_wr sbs, subscription_event_notify_swift cllbk, void *ud);


#endif
