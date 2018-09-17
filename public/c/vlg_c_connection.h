/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#ifndef VLG_C_CONNECTION_H_
#define VLG_C_CONNECTION_H_
#include "vlg.h"

/************************************************************************
INCOMING CONNECTION HANDLERS
************************************************************************/

typedef void(*inco_connection_status_change)(incoming_connection *conn, ConnectionStatus status, void *ud, void *ud2);
typedef void(*inco_connection_on_disconnect)(incoming_connection *conn, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *ud, void *ud2);
typedef RetCode(*inco_connection_on_incoming_transaction)(incoming_connection *conn, shr_incoming_transaction *itx, void *ud, void *ud2);
typedef RetCode(*inco_connection_on_incoming_subscription)(incoming_connection *conn, shr_incoming_subscription *isbs, void *ud, void *ud2);
typedef void(*inco_connection_on_destroy)(incoming_connection *conn, void *ud, void *ud2);

/************************************************************************
INCOMING CONNECTION
************************************************************************/

void inco_connection_release(shr_incoming_connection *ic);
incoming_connection *inco_connection_get_ptr(shr_incoming_connection *ic);
peer *inco_connection_get_peer(incoming_connection *ic);
unsigned int inco_connection_get_connection_id(incoming_connection *ic);
unsigned short inco_connection_get_client_heartbeat(incoming_connection *ic);
unsigned short inco_connection_get_server_agreed_heartbeat(incoming_connection *ic);
ConnectionStatus inco_connection_get_status(incoming_connection *ic);
RetCode inco_connection_await_for_status_reached(incoming_connection *ic, ConnectionStatus test, ConnectionStatus *current, time_t sec, long nsec);
RetCode inco_connection_await_for_status_change(incoming_connection *ic, ConnectionStatus *status, time_t sec, long nsec);
void inco_connection_set_status_change(incoming_connection *ic, inco_connection_status_change hndl, void *ud, void *ud2);
RetCode inco_connection_disconnect(incoming_connection *ic, ProtocolCode reason_code);
RetCode inco_connection_await_for_disconnection_result(incoming_connection *ic, ConnectivityEventResult *con_evt_res, ConnectivityEventType *c_evt_type, time_t sec, long nsec);
void inco_connection_set_on_disconnect(incoming_connection *ic, inco_connection_on_disconnect hndl, void *ud, void *ud2);
void inco_connection_set_on_incoming_transaction(incoming_connection *ic, inco_connection_on_incoming_transaction hndl, void *ud, void *ud2);
void inco_connection_set_on_incoming_subscription(incoming_connection *ic, inco_connection_on_incoming_subscription hndl, void *ud, void *ud2);
void inco_connection_set_on_destroy(incoming_connection *ic, inco_connection_on_destroy hndl, void *ud, void *ud2);
SOCKET inco_connection_get_socket(incoming_connection *ic);
const char *inco_connection_get_host_ip(incoming_connection *ic);
unsigned short inco_connection_get_host_port(incoming_connection *ic);

/************************************************************************
OUTGOING CONNECTION HANDLERS
************************************************************************/

typedef void(*outg_connection_status_change)(outgoing_connection *oc, ConnectionStatus status, void *ud, void *ud2);
typedef void(*outg_connection_on_connect)(outgoing_connection *oc, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *ud, void *ud2);
typedef void(*outg_connection_on_disconnect)(outgoing_connection *oc, ConnectivityEventResult con_evt_res, ConnectivityEventType c_evt_type, void *ud, void *ud2);

/************************************************************************
OUTGOING CONNECTION
************************************************************************/

own_outgoing_connection *outg_connection_create(void);
outgoing_connection *outg_connection_get_ptr(own_outgoing_connection *oc);
void outg_connection_destroy(own_outgoing_connection *oc);
RetCode outg_connection_bind(outgoing_connection *oc, peer *p);
peer *outg_connection_get_peer(outgoing_connection *oc);
unsigned int outg_connection_get_connection_id(outgoing_connection *oc);
ConnectionResult outg_connection_get_connection_response(outgoing_connection *oc);
ProtocolCode outg_connection_get_connection_result_code(outgoing_connection *oc);
unsigned short outg_connection_get_client_heartbeat(outgoing_connection *oc);
unsigned short outg_connection_get_server_agreed_heartbeat(outgoing_connection *oc);
ProtocolCode outg_connection_get_disconnection_reason_code(outgoing_connection *oc);
ConnectionStatus outg_connection_get_status(outgoing_connection *oc);
RetCode outg_connection_await_for_status_reached(outgoing_connection *oc, ConnectionStatus test, ConnectionStatus *current, time_t sec, long nsec);
RetCode outg_connection_await_for_status_change(outgoing_connection *oc, ConnectionStatus *status, time_t sec, long nsec);
void outg_connection_set_status_change(outgoing_connection *oc, outg_connection_status_change hndl, void *ud, void *ud2);
RetCode outg_connection_connect(outgoing_connection *oc, struct sockaddr_in *conn_params);
RetCode outg_connection_await_for_connection_result(outgoing_connection *oc, ConnectivityEventResult *con_evt_res, ConnectivityEventType *c_evt_type, time_t sec, long nsec);
RetCode outg_connection_disconnect(outgoing_connection *oc, ProtocolCode reason_code);
RetCode outg_connection_await_for_disconnection_result(outgoing_connection *oc, ConnectivityEventResult *con_evt_res, ConnectivityEventType *c_evt_type, time_t sec, long nsec);
void outg_connection_set_on_connect(outgoing_connection *oc, outg_connection_on_connect hndl, void *ud, void *ud2);
void outg_connection_set_on_disconnect(outgoing_connection *oc, outg_connection_on_disconnect hndl, void *ud, void *ud2);
SOCKET outg_connection_get_socket(outgoing_connection *oc);
const char *outg_connection_get_host_ip(outgoing_connection *oc);
unsigned short outg_connection_get_host_port(outgoing_connection *oc);

#endif
