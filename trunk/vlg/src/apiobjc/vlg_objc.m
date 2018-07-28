/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#import "vlg_objc.h"

/************************************************************************
 PEER
 ************************************************************************/

void peer_status_change_c(peer *p, PeerStatus status, void *usr_data)
{
    peer_status_change_swf pscs = (__bridge id)usr_data;
    return pscs(p, status, NULL);
}

const char *peer_name_handler_c(peer *p, void *usr_data)
{
    peer_name_handler_swf pnhs = (__bridge id)usr_data;
    return pnhs(p, NULL);
}

const unsigned int *peer_version_handler_c(peer *p, void *usr_data)
{
    peer_version_handler_swf pvhs = (__bridge id)usr_data;
    return pvhs(p, NULL);
}

RetCode peer_load_config_handler_c(peer *p, int pnum, const char *param, const char *value, void *usr_data)
{
    peer_load_config_handler_swf plchs = (__bridge id)usr_data;
    return plchs(p, pnum, param, value, NULL);
}

RetCode peer_init_handler_c(peer *p, void *usr_data)
{
    peer_init_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

RetCode peer_starting_handler_c(peer *p, void *usr_data)
{
    peer_starting_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

RetCode peer_stopping_handler_c(peer *p, void *usr_data)
{
    peer_stopping_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

RetCode peer_on_move_running_handler_c(peer *p, void *usr_data)
{
    peer_move_running_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

void peer_dying_breath_handler_c(peer *p, void *usr_data)
{
    peer_dying_breath_handler_swf pihs = (__bridge id)usr_data;
    return pihs(p, NULL);
}

RetCode peer_on_incoming_connection_handler_c(peer *p, shr_incoming_connection *ic, void *usr_data)
{
    peer_on_incoming_connection_handler_swf poichs = (__bridge id)usr_data;
    return poichs(p, ic, NULL);
}

void peer_set_name_handler_swf(peer *p, peer_name_handler_swf hndl, void *usr_data)
{
    peer_set_name_handler(p, peer_set_name_handler_c, (__bridge_retained void *)hndl);
}

void peer_set_version_handler_swf(peer *p, peer_version_handler_swf hndl, void *usr_data){
    peer_set_version_handler(p, peer_version_handler_c, (__bridge_retained void *)hndl);
}

void peer_set_load_config_handler_swf(peer *p, peer_load_config_handler_swf hndl, void *usr_data){
    peer_set_load_config_handler(p, peer_load_config_handler_c, (__bridge_retained void *)hndl);
}

void peer_set_init_handler_swf(peer *p, peer_init_handler_swf hndl, void *usr_data){
    peer_set_init_handler(p, peer_init_handler_c, (__bridge_retained void *)hndl);
}

void peer_set_starting_handler_swf(peer *p, peer_starting_handler_swf hndl, void *usr_data){
    peer_set_starting_handler(p, peer_starting_handler_c, (__bridge_retained void *)hndl);
}

void peer_set_stopping_handler_swf(peer *p, peer_stopping_handler_swf hndl, void *usr_data){
    peer_set_stopping_handler(p, peer_stopping_handler_c, (__bridge_retained void *)hndl);
}

void peer_set_on_move_running_handler_swf(peer *p, peer_on_move_running_handler_swf hndl, void *usr_data){
    peer_set_on_move_running_handler(p, peer_on_move_running_handler_c, (__bridge_retained void *)hndl);
}

void peer_set_dying_breath_handler_swf(peer *p, peer_dying_breath_handler_swf hndl, void *usr_data){
    peer_set_error_handler(p, peer_dying_breath_handler_c, (__bridge_retained void *)hndl);
}

void peer_set_status_change_handler_swf(peer *p, peer_status_change_swf hndl, void *usr_data){
    peer_set_status_change_handler(p, peer_status_change_c, (__bridge_retained void *)hndl);
}

void peer_set_peer_on_incoming_connection_handler_swf(peer *p, peer_on_incoming_connection_handler_swf hndl, void *usr_data){
    peer_set_peer_on_incoming_connection_handler(p, peer_on_incoming_connection_handler_c, (__bridge_retained void *)hndl);
}

/************************************************************************
 INCOMING CONNECTION
 ************************************************************************/

void inco_connection_status_change_c(incoming_connection *conn, ConnectionStatus status, void *usr_data){
    inco_connection_status_change_swf icscs = (__bridge id)usr_data;
    icscs(conn, status, NULL);
}

void inco_connection_on_disconnect_handler_c(incoming_connection *conn, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *usr_data){
    inco_connection_on_disconnect_handler_swf icodhs = (__bridge id)usr_data;
    icodhs(conn, con_evt_res, c_evt_type, NULL);
}

RetCode inco_connection_on_incoming_transaction_handler_c(incoming_connection *conn, shr_incoming_transaction *itx, void *usr_data){
    inco_connection_on_incoming_transaction_handler_swf icoiths = (__bridge id)usr_data;
    icoiths(conn, itx, NULL);
}

RetCode inco_connection_on_incoming_subscription_handler_c(incoming_connection *conn, shr_incoming_subscription *isbs, void *usr_data){
    inco_connection_on_incoming_subscription_handler_swf icoishs = (__bridge id)usr_data;
    icoishs(conn, isbs, NULL);
}

void inco_connection_set_status_change_handler_swf(incoming_connection *ic, inco_connection_status_change_swf hndl, void *usr_data){
    inco_connection_set_status_change_handler(ic, inco_connection_set_status_change_handler_c, (__bridge_retained void *)hndl);
}

void inco_connection_set_on_disconnect_handler_swf(incoming_connection *ic, inco_connection_on_disconnect_handler_swf hndl, void *usr_data){
    inco_connection_set_on_disconnect_handler(ic, inco_connection_set_on_disconnect_handler_c, (__bridge_retained void *)hndl);
}

void inco_connection_set_on_incoming_transaction_handler_swf(incoming_connection *ic, inco_connection_on_incoming_transaction_handler_swf hndl, void *usr_data){
    inco_connection_set_on_incoming_transaction_handler(ic, inco_connection_on_incoming_transaction_handler_c, (__bridge_retained void *)hndl);
}

void inco_connection_set_on_incoming_subscription_handler_swf(incoming_connection *ic, inco_connection_on_incoming_subscription_handler_swf hndl, void *usr_data){
    inco_connection_set_on_incoming_subscription_handler(ic, inco_connection_on_incoming_subscription_handler_c, (__bridge_retained void *)hndl);
}



















