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

#ifndef VLG_CPP_CONNECTION_H_
#define VLG_CPP_CONNECTION_H_
#include "vlg_memory.h"
#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#ifdef __GNUG__
#define SOCKET int
#define INVALID_SOCKET (~0)
#define SOCKET_ERROR   (-1)
#endif

namespace vlg {

/** @brief class connection_factory.
*/
class connection_factory {
    public:
        static connection_int       *conn_factory_int_f(peer_int &peer,
                                                        ConnectionType con_type,
                                                        unsigned int connid,
                                                        void *ud);
    public:
        connection_factory();
        virtual ~connection_factory();

    public:
        virtual connection          *new_connection(peer &p);

    public:
        static connection_factory   *default_connection_factory();
};

/** @brief class connection.
*/
class connection_impl;
class connection : public vlg::collectable {
    public:
        typedef void (*connection_status_change)(connection &conn,
                                                 ConnectionStatus status,
                                                 void *ud);

        //---ctors
    public:
        explicit connection();
        virtual ~connection();

        virtual vlg::collector &get_collector();

    public:
        vlg::RetCode          bind(peer &p);

    public:
        peer                        *get_peer();
        ConnectionType              get_connection_type()           const;
        unsigned int                get_connection_id()             const;
        ConnectionResult            get_connection_response()       const;
        ConnectionResultReason      get_connection_result_code()    const;
        unsigned short              get_client_heartbeat()          const;
        unsigned short              get_server_agreed_heartbeat()   const;
        DisconnectionResultReason   get_disconnection_reason_code() const;
        ConnectionStatus            get_status();

    public:
        vlg::RetCode
        await_for_status_reached_or_outdated(ConnectionStatus test,
                                             ConnectionStatus &current,
                                             time_t sec = -1,
                                             long nsec = 0);

        vlg::RetCode
        await_for_status_change(ConnectionStatus &status,
                                time_t sec = -1,
                                long nsec = 0);
        void
        set_connection_status_change_handler(connection_status_change handler,
                                             void *ud);

    public:
        vlg::RetCode    connect(sockaddr_in &connection_params);

        /* this function must be called from same thread that
        called connect()*/
        vlg::RetCode
        await_for_connection_result(ConnectivityEventResult
                                    &con_evt_res,
                                    ConnectivityEventType &c_evt_type,
                                    time_t sec = -1,
                                    long nsec = 0);

    public:
        vlg::RetCode    disconnect(DisconnectionResultReason reason_code);

        /* this function must be called from same thread that
        called disconnect()*/
        vlg::RetCode
        await_for_disconnection_result(ConnectivityEventResult
                                       &con_evt_res,
                                       ConnectivityEventType &c_evt_type,
                                       time_t sec = -1,
                                       long nsec = 0);

    public:
        virtual void on_connect(ConnectivityEventResult con_evt_res,
                                ConnectivityEventType c_evt_type);

        virtual void on_disconnect(ConnectivityEventResult con_evt_res,
                                   ConnectivityEventType c_evt_type);

    public:
        transaction_factory *get_transaction_factory();
        void set_transaction_factory(transaction_factory &tx_factory);

    public:
        subscription_factory *get_subscription_factory();
        void set_subscription_factory(subscription_factory &sbs_factory);

    public:
        SOCKET              get_socket()    const;
        const char         *get_host_ip()   const;
        unsigned short      get_host_port() const;

    public:
        connection_int *get_internal();
        void            set_internal(connection_int *conn);

    private:
        connection_impl *impl_;

    protected:
        static nclass_logger *log_;
};

}

#endif
