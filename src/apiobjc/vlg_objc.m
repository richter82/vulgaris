/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#import "vlg_objc.h"

/************************************************************************
 PEER
 ************************************************************************/

void (peer_status_change_c)(peer *p, PeerStatus status, void *usr_data)
{
    peer_status_change_swf pscs = (__bridge id)usr_data;
    return pscs(p, status, NULL);
}

const char *(peer_name_handler_c)(peer *p, void *usr_data)
{
    peer_name_handler_swf pnhs = (__bridge id)usr_data;
    return pnhs(p, NULL);
}

const unsigned int *(peer_version_handler_c)(peer *p, void *usr_data)
{
    peer_version_handler_swf pvhs = (__bridge id)usr_data;
    return pvhs(p, NULL);
}

RetCode(peer_load_config_handler_c)(peer *p, int pnum, const char *param, const char *value, void *usr_data)
{
    peer_load_config_handler_swf plchs = (__bridge id)usr_data;
    return plchs(p, pnum, param, value, NULL);
}

RetCode(peer_init_handler_c)(peer *p, void *usr_data)
{
    peer_init_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

RetCode(peer_starting_handler_c)(peer *p, void *usr_data)
{
    peer_starting_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

RetCode(peer_stopping_handler_c)(peer *p, void *usr_data)
{
    peer_stopping_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

RetCode(peer_on_move_running_handler_c)(peer *p, void *usr_data)
{
    peer_move_running_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

void(peer_dying_breath_handler_c)(peer *p, void *usr_data)
{
    peer_dying_breath_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

RetCode(peer_on_incoming_connection_handler_c)(peer *p, shr_incoming_connection *ic, void *usr_data)
{
    peer_on_incoming_connection_handler_swf poichs = (__bridge id)usr_data;
    return poichs(p, ic, NULL);
}

void peer_set_name_handler_swf(peer *p, peer_name_handler_swf hndl, void *usr_data)
{
    peer_set_name_handler(p, peer_set_name_handler_c, (__bridge_retained void *)hndl);
}

void peer_set_version_handler_swf(peer *p, peer_version_handler_swf hndl, void *usr_data);
void peer_set_load_config_handler_swf(peer *p, peer_load_config_handler_swf hndl, void *usr_data);
void peer_set_init_handler_swf(peer *p, peer_init_handler_swf hndl, void *usr_data);
void peer_set_starting_handler_swf(peer *p, peer_starting_handler_swf hndl, void *usr_data);
void peer_set_stopping_handler_swf(peer *p, peer_stopping_handler_swf hndl, void *usr_data);
void peer_set_on_move_running_handler_swf(peer *p, peer_on_move_running_handler_swf hndl, void *usr_data);
void peer_set_dying_breath_handler_swf(peer *p, peer_dying_breath_handler_swf hndl, void *usr_data);
void peer_set_status_change_handler_swf(peer *p, peer_status_change_swf handler, void *usr_data);
void peer_set_peer_on_incoming_connection_handler_swf(peer *p, peer_on_incoming_connection_handler_swf handler, void *usr_data);


void peer_set_name_handler_swift(peer_wr p, peer_name_handler_swift cllbk)
{
    peer_set_name_handler(p, peer_name_handler_wr_native,
                          (__bridge_retained void *)cllbk);
}

void peer_set_version_handler_swift(peer_wr p, peer_version_handler_swift cllbk)
{
    peer_set_version_handler(p, peer_version_handler_wr_native,
                             (__bridge_retained void *)cllbk);
}

void peer_set_load_config_handler_swift(peer_wr p,
                                        peer_load_config_handler_swift cllbk)
{
    peer_set_load_config_handler(p, peer_load_config_handler_wr_native,
                                 (__bridge_retained void *)cllbk);
}

void peer_set_init_handler_swift(peer_wr p, peer_init_handler_swift cllbk)
{
    peer_set_init_handler(p, peer_init_handler_wr_native,
                          (__bridge_retained void *)cllbk);
}

void peer_set_starting_handler_swift(peer_wr p,
                                     peer_starting_handler_swift cllbk)
{
    peer_set_starting_handler(p, peer_starting_handler_wr_native,
                              (__bridge_retained void *)cllbk);
}

void peer_set_stopping_handler_swift(peer_wr p,
                                     peer_stopping_handler_swift cllbk)
{
    peer_set_stopping_handler(p, peer_stopping_handler_wr_native,
                              (__bridge_retained void *)cllbk);
}

void peer_set_transit_on_air_handler_swift(peer_wr p,
                                           peer_transit_on_air_handler_swift cllbk)
{
    peer_set_transit_on_air_handler(p, peer_transit_on_air_handler_wr_native,
                                    (__bridge_retained void *)cllbk);
}

void peer_set_error_handler_swift(peer_wr p, peer_error_handler_swift cllbk)
{
    peer_set_error_handler(p, peer_error_handler_wr_native,
                           (__bridge_retained void *)cllbk);
}

void peer_set_dying_breath_handler_swift(peer_wr p,
                                         peer_dying_breath_handler_swift cllbk)
{
    peer_set_dying_breath_handler(p, peer_dying_breath_handler_wr_native,
                                  (__bridge_retained void *)cllbk);
}

void peer_set_status_change_handler_swift(peer_wr p,
                                          peer_status_change_swift cllbk)
{
    peer_set_status_change_handler(p, peer_status_change_wr_native,
                                   (__bridge_retained void *)cllbk);
}

/*******************************************************************************
 connection
 ******************************************************************************/

void connection_status_change_wr_native(connection_wr conn,
                                        ConnectionStatus status, void *ud)
{
    connection_status_change_swift cscs = (__bridge id)ud;
    cscs(conn, status, NULL);
}

void on_connect_handler_wr_native(connection_wr conn,
                                  ConnectivityEventResult con_evt_res,
                                  ConnectivityEventType connectivity_evt_type, void *ud)
{
    on_connect_handler_swift ochs = (__bridge id)ud;
    ochs(conn, con_evt_res, connectivity_evt_type, NULL);
}

void on_disconnect_handler_wr_native(connection_wr conn,
                                     ConnectivityEventResult con_evt_res,
                                     ConnectivityEventType connectivity_evt_type, void *ud)
{
    on_disconnect_handler_swift odhs = (__bridge id)ud;
    odhs(conn, con_evt_res, connectivity_evt_type, NULL);
}

//------------------------------------------------------------------------------
// CONNECTIVITY
//------------------------------------------------------------------------------
void connection_set_on_connect_handler_swift(connection_wr conn,
                                             on_connect_handler_swift cllbk, void *ud)
{
    connection_set_on_connect_handler(conn, on_connect_handler_wr_native,
                                      (__bridge_retained void *)cllbk);
}

void connection_set_on_disconnect_handler_swift(connection_wr conn,
                                                on_disconnect_handler_swift cllbk, void *ud)
{
    connection_set_on_disconnect_handler(conn, on_disconnect_handler_wr_native,
                                         (__bridge_retained void *)cllbk);
}

//------------------------------------------------------------------------------
// STATUS
//------------------------------------------------------------------------------
void connection_set_status_change_handler_swift(connection_wr conn,
                                                connection_status_change_swift cllbk, void *ud)
{
    connection_set_status_change_handler(conn, connection_status_change_wr_native,
                                         (__bridge_retained void *)cllbk);
}

/*******************************************************************************
 transaction
 ******************************************************************************/

void transaction_status_change_wr_native(transaction_wr tx,
                                         TransactionStatus status, void *ud)
{
    transaction_status_change_swift tscs = (__bridge id)ud;
    tscs(tx, status, NULL);
}

void transaction_closure_wr_native(transaction_wr tx, void *ud)
{
    transaction_closure_swift tcs = (__bridge id)ud;
    tcs(tx, NULL);
}

//------------------------------------------------------------------------------
// STATUS
//------------------------------------------------------------------------------
void transaction_set_transaction_status_change_handler_swift(transaction_wr tx,
                                                             transaction_status_change_swift cllbk, void *ud)
{
    transaction_set_transaction_status_change_handler(tx,
                                                      transaction_status_change_wr_native, (__bridge_retained void *)cllbk);
}

//------------------------------------------------------------------------------
// TX CLOSURE
//------------------------------------------------------------------------------
void transaction_set_transaction_closure_handler_swift(transaction_wr tx,
                                                       transaction_closure_swift cllbk, void *ud)
{
    transaction_set_transaction_closure_handler(tx, transaction_closure_wr_native,
                                                (__bridge_retained void *)cllbk);
}

/*******************************************************************************
 subscription
 ******************************************************************************/

void subscription_status_change_wr_native(subscription_wr sbs,
                                          SubscriptionStatus status, void *ud)
{
    subscription_status_change_swift sscs = (__bridge id)ud;
    sscs(sbs, status, NULL);
}

void subscription_event_notify_wr_native(subscription_wr sbs,
                                         subscription_event_wr sbs_evt, void *ud)
{
    subscription_event_notify_swift sens = (__bridge id)ud;
    sens(sbs, sbs_evt, NULL);
}

//------------------------------------------------------------------------------
// STATUS
//------------------------------------------------------------------------------
void subscription_set_status_change_handler_swift(subscription_wr sbs,
                                                  subscription_status_change_swift cllbk, void *ud)
{
    subscription_set_status_change_handler(sbs,
                                           subscription_status_change_wr_native, (__bridge_retained void *)cllbk);
}

//------------------------------------------------------------------------------
// EVENT NOTIFY
//------------------------------------------------------------------------------
void subscription_set_event_notify_handler_swift(subscription_wr sbs,
                                                 subscription_event_notify_swift cllbk, void *ud)
{
    subscription_set_event_notify_handler(sbs, subscription_event_notify_wr_native,
                                          (__bridge_retained void *)cllbk);
}


















