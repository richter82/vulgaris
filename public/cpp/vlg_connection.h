/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
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

/** @brief incoming connection listener.
*/
struct incoming_connection_listener {
    virtual void on_status_change(incoming_connection &,
                                  ConnectionStatus current) = 0;

    virtual void on_disconnect(incoming_connection &,
                               ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type) = 0;

    /**
    @return default implementation always returns RetCode_OK,
            so all incoming transactions will be accepted.
    */
    virtual RetCode on_incoming_transaction(incoming_connection &,
                                            std::shared_ptr<incoming_transaction> &) = 0;

    /**
    @return default implementation always returns RetCode_OK,
            so all incoming subscriptions will be accepted.
    */
    virtual RetCode on_incoming_subscription(incoming_connection &,
                                             std::shared_ptr<incoming_subscription> &) = 0;

    static incoming_connection_listener &default_listener();
};

/** @brief incoming_connection.
*/
struct incoming_connection {
    explicit incoming_connection(peer &,
                                 incoming_connection_listener &listener =
                                     incoming_connection_listener::default_listener());

    virtual ~incoming_connection();

    peer &get_peer();
    unsigned int get_id() const;
    unsigned short get_client_heartbeat() const;
    unsigned short get_server_agreed_heartbeat() const;
    ConnectionStatus get_status();

    RetCode await_for_status_reached(ConnectionStatus test,
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

/** @brief outgoing connection listener.
*/
struct outgoing_connection_listener {
    virtual void on_status_change(outgoing_connection &,
                                  ConnectionStatus current) = 0;

    virtual void on_connect(outgoing_connection &,
                            ConnectivityEventResult con_evt_res,
                            ConnectivityEventType c_evt_type) = 0;

    virtual void on_disconnect(outgoing_connection &,
                               ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type) = 0;

    static outgoing_connection_listener &default_listener();
};

/** @brief outgoing_connection.
*/
struct outgoing_connection {
    explicit outgoing_connection(outgoing_connection_listener &listener =
                                     outgoing_connection_listener::default_listener());

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

    RetCode await_for_status_reached(ConnectionStatus test,
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

    SOCKET get_socket() const;
    const char *get_host_ip() const;
    unsigned short get_host_port() const;

    std::unique_ptr<outgoing_connection_impl> impl_;
};

}
