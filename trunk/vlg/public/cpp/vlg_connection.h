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

#pragma once
#include "vlg.h"

namespace vlg {

/** @brief connection_factory.
*/
struct incoming_connection_factory {
    explicit incoming_connection_factory();
    virtual ~incoming_connection_factory();
    virtual incoming_connection &make_incoming_connection(peer &p);

    static incoming_connection_factory &default_factory();
};

/** @brief incoming_connection.
*/
struct incoming_connection {
    explicit incoming_connection(peer &);
    virtual ~incoming_connection();

    peer &get_peer();
    unsigned int get_id() const;
    unsigned short get_client_heartbeat() const;
    unsigned short get_server_agreed_heartbeat() const;
    ConnectionStatus get_status();

    RetCode await_for_status_reached_or_outdated(ConnectionStatus test,
                                                 ConnectionStatus &current,
                                                 time_t sec = -1,
                                                 long nsec = 0);

    RetCode await_for_status_change(ConnectionStatus &status,
                                    time_t sec = -1,
                                    long nsec = 0);

    RetCode disconnect(ProtocolCode reason_code);

    RetCode await_for_disconnection_result(ConnectivityEventResult &con_evt_res,
                                           ConnectivityEventType &c_evt_type,
                                           time_t sec = -1,
                                           long nsec = 0);

    virtual void on_status_change(ConnectionStatus current);

    virtual void on_disconnect(ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type);

    /**
    @param incoming_transaction the brand new incoming transaction.
    @return default implementation always returns RetCode_OK,
            so all incoming transactions will be accepted.
    */
    virtual RetCode on_incoming_transaction(std::shared_ptr<incoming_transaction> &);

    /**
    @param incoming_subscription the brand new incoming subscription.
    @return default implementation always returns RetCode_OK,
            so all incoming subscriptions will be accepted.
    */
    virtual RetCode on_incoming_subscription(std::shared_ptr<incoming_subscription> &);

    incoming_transaction_factory &get_incoming_transaction_factory();
    void set_incoming_transaction_factory(incoming_transaction_factory &);

    incoming_subscription_factory &get_incoming_subscription_factory();
    void set_incoming_subscription_factory(incoming_subscription_factory &);

    SOCKET get_socket() const;
    const char *get_host_ip() const;
    unsigned short get_host_port() const;

    std::unique_ptr<incoming_connection_impl> impl_;
};

}

namespace vlg {

/** @brief outgoing_connection.
*/
struct outgoing_connection {
    explicit outgoing_connection();
    virtual ~outgoing_connection();

    RetCode bind(peer &);

    peer &get_peer();
    ConnectionType get_connection_type() const;
    unsigned int get_id() const;
    ConnectionResult get_connection_response() const;
    ProtocolCode get_connection_result_code() const;
    unsigned short get_client_heartbeat() const;
    unsigned short get_server_agreed_heartbeat() const;
    ProtocolCode get_disconnection_reason_code() const;
    ConnectionStatus get_status();

    RetCode await_for_status_reached_or_outdated(ConnectionStatus test,
                                                 ConnectionStatus &current,
                                                 time_t sec = -1,
                                                 long nsec = 0);

    RetCode await_for_status_change(ConnectionStatus &status,
                                    time_t sec = -1,
                                    long nsec = 0);

    RetCode connect(sockaddr_in &);

    RetCode await_for_connection_result(ConnectivityEventResult &con_evt_res,
                                        ConnectivityEventType &c_evt_type,
                                        time_t sec = -1,
                                        long nsec = 0);

    RetCode disconnect(ProtocolCode reason_code);

    RetCode await_for_disconnection_result(ConnectivityEventResult &con_evt_res,
                                           ConnectivityEventType &c_evt_type,
                                           time_t sec = -1,
                                           long nsec = 0);

    virtual void on_status_change(ConnectionStatus current);

    virtual void on_connect(ConnectivityEventResult con_evt_res,
                            ConnectivityEventType c_evt_type);

    virtual void on_disconnect(ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type);

    SOCKET get_socket() const;
    const char *get_host_ip() const;
    unsigned short get_host_port() const;

    std::unique_ptr<outgoing_connection_impl> impl_;
};

}
