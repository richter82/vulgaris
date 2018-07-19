/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "vlg_peer.h"
#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg_subscription.h"
#include "vlg/pr_impl.h"
#include "vlg/conn_impl.h"

namespace vlg {

struct default_incoming_connection_listener : public incoming_connection_listener {
    virtual void on_status_change(incoming_connection &,
                                  ConnectionStatus current) override {}

    virtual void on_disconnect(incoming_connection &,
                               ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type) override {}

    virtual RetCode on_incoming_transaction(incoming_connection &,
                                            std::shared_ptr<incoming_transaction> &) override {
        return RetCode_OK;
    }

    virtual RetCode on_incoming_subscription(incoming_connection &,
                                             std::shared_ptr<incoming_subscription> &) override {
        return RetCode_OK;
    }
};

static default_incoming_connection_listener dicl;

incoming_connection_listener &incoming_connection_listener::default_listener()
{
    return dicl;
}

incoming_connection::incoming_connection(peer &p, incoming_connection_listener &listener) :
    impl_(new incoming_connection_impl(*this, p, listener))
{
    CTOR_TRC
}

incoming_connection::~incoming_connection()
{
    DTOR_TRC
}

peer &incoming_connection::get_peer()
{
    return impl_->peer_->publ_;
}

unsigned int incoming_connection::get_id() const
{
    return impl_->connid_;
}

unsigned short incoming_connection::get_client_heartbeat() const
{
    return impl_->cli_agrhbt_;
}

unsigned short incoming_connection::get_server_agreed_heartbeat() const
{
    return impl_->srv_agrhbt_;
}

ConnectionStatus incoming_connection::get_status()
{
    return impl_->status_;
}

RetCode incoming_connection::await_for_status_reached(ConnectionStatus test,
                                                      ConnectionStatus &current,
                                                      time_t sec,
                                                      long nsec)
{
    return impl_->await_for_status_reached(test,
                                           current,
                                           sec,
                                           nsec);
}

RetCode incoming_connection::await_for_status_change(ConnectionStatus &status,
                                                     time_t sec,
                                                     long nsec)
{
    return impl_->await_for_status_change(status, sec, nsec);
}

RetCode incoming_connection::disconnect(ProtocolCode reason_code)
{
    return impl_->disconnect(reason_code);
}

RetCode incoming_connection::await_for_disconnection_result(ConnectivityEventResult &con_evt_res,
                                                            ConnectivityEventType &connectivity_evt_type,
                                                            time_t sec,
                                                            long nsec)
{
    return impl_->await_for_disconnection_result(con_evt_res,
                                                 connectivity_evt_type,
                                                 sec,
                                                 nsec);
}

incoming_transaction_factory &incoming_connection::get_incoming_transaction_factory()
{
    return *impl_->tx_factory_publ_;
}

void incoming_connection::set_incoming_transaction_factory(incoming_transaction_factory &tx_factory)
{
    impl_->tx_factory_publ_ = &tx_factory;
}

incoming_subscription_factory &incoming_connection::get_incoming_subscription_factory()
{
    return *impl_->sbs_factory_publ_;
}

void incoming_connection::set_incoming_subscription_factory(incoming_subscription_factory &sbs_factory)
{
    impl_->sbs_factory_publ_ = &sbs_factory;
}

SOCKET incoming_connection::get_socket() const
{
    return impl_->socket_;
}

const char *incoming_connection::get_host_ip() const
{
    return impl_->get_host_ip();
}

unsigned short incoming_connection::get_host_port() const
{
    return impl_->get_host_port();
}

}

namespace vlg {

struct default_outgoing_connection_listener : public outgoing_connection_listener {
    virtual void on_status_change(outgoing_connection &,
                                  ConnectionStatus current) override {}

    virtual void on_connect(outgoing_connection &,
                            ConnectivityEventResult con_evt_res,
                            ConnectivityEventType c_evt_type) override {}

    virtual void on_disconnect(outgoing_connection &,
                               ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type) override {}
};

static default_outgoing_connection_listener docl;

outgoing_connection_listener &outgoing_connection_listener::default_listener()
{
    return docl;
}

outgoing_connection::outgoing_connection(outgoing_connection_listener &listener) :
    impl_(new outgoing_connection_impl(*this, listener))
{
    CTOR_TRC
}

outgoing_connection::~outgoing_connection()
{
    DTOR_TRC
}

RetCode outgoing_connection::bind(peer &p)
{
    impl_->peer_ = &*p.impl_;
    return RetCode_OK;
}

peer &outgoing_connection::get_peer()
{
    return impl_->peer_->publ_;
}

ConnectionType outgoing_connection::get_connection_type() const
{
    return impl_->con_type_;
}

unsigned int outgoing_connection::get_id() const
{
    return impl_->connid_;
}

ConnectionResult outgoing_connection::get_connection_response() const
{
    return impl_->conres_;
}

ProtocolCode outgoing_connection::get_connection_result_code() const
{
    return impl_->conrescode_;
}

unsigned short outgoing_connection::get_client_heartbeat() const
{
    return impl_->cli_agrhbt_;
}

unsigned short outgoing_connection::get_server_agreed_heartbeat() const
{
    return impl_->srv_agrhbt_;
}

ProtocolCode outgoing_connection::get_disconnection_reason_code() const
{
    return impl_->disconrescode_;
}

ConnectionStatus outgoing_connection::get_status()
{
    return impl_->status_;
}

RetCode outgoing_connection::await_for_status_reached(ConnectionStatus test,
                                                      ConnectionStatus &current,
                                                      time_t sec,
                                                      long nsec)
{
    return impl_->await_for_status_reached(test,
                                           current,
                                           sec,
                                           nsec);
}

RetCode outgoing_connection::await_for_status_change(ConnectionStatus &status,
                                                     time_t sec,
                                                     long nsec)
{
    return impl_->await_for_status_change(status, sec, nsec);
}

RetCode outgoing_connection::connect(sockaddr_in &connection_params)
{
    return impl_->client_connect(connection_params);
}

RetCode outgoing_connection::await_for_connection_result(ConnectivityEventResult &con_evt_res,
                                                         ConnectivityEventType &connectivity_evt_type,
                                                         time_t sec,
                                                         long nsec)
{
    return impl_->await_for_connection_result(con_evt_res,
                                              connectivity_evt_type,
                                              sec,
                                              nsec);
}

RetCode outgoing_connection::disconnect(ProtocolCode reason_code)
{
    return impl_->disconnect(reason_code);
}

RetCode outgoing_connection::await_for_disconnection_result(ConnectivityEventResult &con_evt_res,
                                                            ConnectivityEventType &connectivity_evt_type,
                                                            time_t sec,
                                                            long nsec)
{
    return impl_->await_for_disconnection_result(con_evt_res,
                                                 connectivity_evt_type,
                                                 sec,
                                                 nsec);
}

SOCKET outgoing_connection::get_socket() const
{
    return impl_->socket_;
}

const char *outgoing_connection::get_host_ip() const
{
    return impl_->get_host_ip();
}

unsigned short outgoing_connection::get_host_port() const
{
    return impl_->get_host_port();
}

}
