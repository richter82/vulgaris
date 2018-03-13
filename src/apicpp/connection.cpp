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

#include "vlg_peer.h"
#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg_subscription.h"
#include "vlg/pr_impl.h"
#include "vlg/conn_impl.h"

namespace vlg {

connection::connection() : impl_(new connection_impl(*this))
{
    CTOR_TRC
}

connection::~connection()
{
    DTOR_TRC
}

RetCode connection::bind(peer &p)
{
    impl_->peer_ = &*p.impl_;
    return RetCode_OK;
}

peer &connection::get_peer()
{
    return impl_->peer_->publ_;
}

ConnectionType connection::get_connection_type() const
{
    return impl_->con_type_;
}

unsigned int connection::get_id() const
{
    return impl_->connid_;
}

ConnectionResult connection::get_connection_response() const
{
    return impl_->conres_;
}

ProtocolCode connection::get_connection_result_code() const
{
    return impl_->conrescode_;
}

unsigned short connection::get_client_heartbeat() const
{
    return impl_->cli_agrhbt_;
}

unsigned short connection::get_server_agreed_heartbeat() const
{
    return impl_->srv_agrhbt_;
}

ProtocolCode connection::get_disconnection_reason_code() const
{
    return impl_->disconrescode_;
}

ConnectionStatus connection::get_status()
{
    return impl_->status_;
}

RetCode connection::await_for_status_reached_or_outdated(ConnectionStatus test,
                                                         ConnectionStatus &current,
                                                         time_t sec,
                                                         long nsec)
{
    return impl_->await_for_status_reached_or_outdated(test,
                                                       current,
                                                       sec,
                                                       nsec);
}

RetCode connection::await_for_status_change(ConnectionStatus &status,
                                            time_t sec,
                                            long nsec)
{
    return impl_->await_for_status_change(status, sec, nsec);
}

RetCode connection::connect(sockaddr_in &connection_params)
{
    return impl_->client_connect(connection_params);
}

RetCode connection::await_for_connection_result(ConnectivityEventResult &con_evt_res,
                                                ConnectivityEventType &connectivity_evt_type,
                                                time_t sec,
                                                long nsec)
{
    return impl_->await_for_connection_result(con_evt_res,
                                              connectivity_evt_type,
                                              sec,
                                              nsec);
}

RetCode connection::disconnect(ProtocolCode reason_code)
{
    return impl_->disconnect(reason_code);
}

RetCode connection::await_for_disconnection_result(ConnectivityEventResult &con_evt_res,
                                                   ConnectivityEventType &connectivity_evt_type,
                                                   time_t sec,
                                                   long nsec)
{
    return impl_->await_for_disconnection_result(con_evt_res,
                                                 connectivity_evt_type,
                                                 sec,
                                                 nsec);
}

void connection::on_status_change(ConnectionStatus current)
{}

void connection::on_connect(ConnectivityEventResult con_evt_res,
                            ConnectivityEventType connectivity_evt_type)
{}

void connection::on_disconnect(ConnectivityEventResult con_evt_res,
                               ConnectivityEventType connectivity_evt_type)
{}

RetCode connection::on_incoming_transaction(std::shared_ptr<vlg::transaction> &incoming_transaction)
{
    return RetCode_OK;
}

RetCode connection::on_incoming_subscription(std::shared_ptr<vlg::subscription> &incoming_subscription)
{
    return RetCode_OK;
}

incoming_transaction_factory &connection::get_incoming_transaction_factory()
{
    return *impl_->tx_factory_publ_;
}

void connection::set_incoming_transaction_factory(incoming_transaction_factory &tx_factory)
{
    impl_->tx_factory_publ_ = &tx_factory;
}

incoming_subscription_factory &connection::get_incoming_subscription_factory()
{
    return *impl_->sbs_factory_publ_;
}

void connection::set_incoming_subscription_factory(incoming_subscription_factory &sbs_factory)
{
    impl_->sbs_factory_publ_ = &sbs_factory;
}

SOCKET connection::get_socket() const
{
    return impl_->socket_;
}

const char *connection::get_host_ip() const
{
    return impl_->get_host_ip();
}

unsigned short connection::get_host_port() const
{
    return impl_->get_host_port();
}

}
