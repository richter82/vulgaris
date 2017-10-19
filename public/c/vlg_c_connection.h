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

#ifndef VLG_C_CONNECTION_H_
#define VLG_C_CONNECTION_H_
#include "vlg.h"
#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#ifdef __GNUG__
#define SOCKET int
#define INVALID_SOCKET (~0)
#define SOCKET_ERROR   (-1)
#elif defined(__MACH__) || defined(__APPLE__)
#define SOCKET int
#endif

#if defined(__cplusplus)
using namespace vlg;
extern "C" {
#endif

typedef void(*connection_status_change_wr)(connection_wr        conn,
                                           ConnectionStatus     status,
                                           void                 *ud);

typedef void(*on_connect_handler_wr)(connection_wr              conn,
                                     ConnectivityEventResult    con_evt_res,
                                     ConnectivityEventType      c_evt_type,
                                     void                       *ud);

typedef void(*on_disconnect_handler_wr)(connection_wr           conn,
                                        ConnectivityEventResult con_evt_res,
                                        ConnectivityEventType   c_evt_type,
                                        void                    *ud);

connection_wr           connection_create(void);
void                    connection_destroy(connection_wr conn);

RetCode                 connection_bind(connection_wr conn,
                                        peer_wr p);

peer_wr                 connection_get_peer(connection_wr conn);

ConnectionType          connection_get_connection_type(connection_wr conn);

unsigned int            connection_get_connection_id(connection_wr conn);

ConnectionResult        connection_get_connection_response(connection_wr conn);

ConnectionResultReason
connection_get_connection_result_code(connection_wr conn);

unsigned short          connection_get_client_heartbeat(connection_wr conn);

unsigned short
connection_get_server_agreed_heartbeat(connection_wr conn);

DisconnectionResultReason
connection_get_disconnection_reason_code(connection_wr conn);

ConnectionStatus        connection_get_status(connection_wr conn);

RetCode
connection_await_for_status_reached_or_outdated(connection_wr       conn,
                                                ConnectionStatus    test,
                                                ConnectionStatus    *current,
                                                time_t              sec,
                                                long                nsec);

RetCode
connection_await_for_status_change(connection_wr    conn,
                                   ConnectionStatus *status,
                                   time_t           sec,
                                   long             nsec);

void
connection_set_status_change_handler(connection_wr                  conn,
                                     connection_status_change_wr    handler,
                                     void                           *ud);

RetCode                 connection_connect(connection_wr        conn,
                                           struct sockaddr_in   *conn_params);

/* this function must be called from same thread that called Connect()*/
RetCode
connection_await_for_connection_result(connection_wr            conn,
                                       ConnectivityEventResult  *con_evt_res,
                                       ConnectivityEventType    *c_evt_type,
                                       time_t                   sec,
                                       long                     nsec);

RetCode connection_disconnect(connection_wr conn,
                              DisconnectionResultReason reason_code);

/* this function must be called from same thread that called Disconnect()*/
RetCode
connection_await_for_disconnection_result(connection_wr             conn,
                                          ConnectivityEventResult   *con_evt_res,
                                          ConnectivityEventType     *c_evt_type,
                                          time_t                    sec,
                                          long                      nsec);

void connection_set_on_connect_handler(connection_wr            conn,
                                       on_connect_handler_wr    hndl,
                                       void                     *ud);

void connection_set_on_disconnect_handler(connection_wr             conn,
                                          on_disconnect_handler_wr  hndl,
                                          void                      *ud);

SOCKET              connection_get_socket(connection_wr conn);
const char          *connection_get_host_ip(connection_wr conn);
unsigned short      connection_get_host_port(connection_wr conn);

#if defined(__cplusplus)
}
#endif

#endif
