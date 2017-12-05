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
#include "vlg/vlg_peer_impl.h"
#include "vlg/vlg_connection_impl.h"

namespace vlg {

// CLASS connection_impl_pub
class connection_impl_pub {
    private:
        class cimpl_conn_impl : public connection_impl {
            public:
                cimpl_conn_impl(connection &publ,
                                peer_impl &peer,
                                ConnectionType con_type,
                                unsigned int connid) :
                    connection_impl(publ,
                                    peer,
                                    con_type,
                                    connid),
                    publ_(publ) {
                }

                virtual void on_connect(ConnectivityEventResult con_evt_res,
                                        ConnectivityEventType connectivity_evt_type) {
                    publ_.on_connect(con_evt_res, connectivity_evt_type);
                }
                virtual void on_disconnect(ConnectivityEventResult con_evt_res,
                                           ConnectivityEventType connectivity_evt_type) {
                    publ_.on_disconnect(con_evt_res, connectivity_evt_type);
                }

            private:
                connection &publ_;
        };

        static connection_impl *vlg_conn_factory_cimpl(peer_impl &peer,
                                                       ConnectionType con_type,
                                                       unsigned int connid,
                                                       void *ud) {
            connection *publ = static_cast<connection *>(ud);
            return new cimpl_conn_impl(*publ, peer, con_type, connid);
        }

        static void connection_impl_status_change_hndlr_cimpl(connection_impl &conn,
                                                              ConnectionStatus status,
                                                              void *ud) {
            connection_impl_pub *cimpl = static_cast<connection_impl_pub *>(ud);
            if(cimpl->csh_) {
                cimpl->csh_(cimpl->publ_, status, cimpl->csh_ud_);
            }
        }

    public:
        connection_impl_pub(connection &publ) :
            publ_(publ),
            peer_(nullptr),
            impl_(nullptr),
            csh_(nullptr),
            csh_ud_(nullptr),
            tx_factory_(&transaction_factory::default_factory()),
            sbs_factory_(&subscription_factory::default_factory()) {}
        ~connection_impl_pub() {
            if(impl_) {
                //destroy of underling object must be done only for outgoing-client connections
                if(impl_->conn_type() == ConnectionType_OUTGOING) {
                    impl_->clean_best_effort();
                    if(impl_->status() > ConnectionStatus_DISCONNECTED) {
                        ConnectivityEventResult cres = ConnectivityEventResult_KO;
                        ConnectivityEventType cevttyp = ConnectivityEventType_UNDEFINED;
                        impl_->disconnect(DisconnectionResultReason_APPLICATIVE);
                        impl_->await_for_disconnection_result(cres, cevttyp);
                    }
                    vlg::collector &c = impl_->get_collector();
                    c.release(impl_);
                } else {
                }
            }
        }

        peer *get_peer() const {
            return peer_;
        }
        void set_peer(peer &val) {
            peer_ = &val;
        }

        connection_impl *get_conn() const {
            return impl_;
        }
        void set_conn(connection_impl *val) {
            impl_ = val;
        }

        connection::status_change get_csh() const {
            return csh_;
        }
        void set_csh(connection::status_change val) {
            csh_ = val;
        }

        void *get_csh_ud() const {
            return csh_ud_;
        }
        void set_csh_ud(void *val) {
            csh_ud_ = val;
        }

        transaction_factory &tx_factory() {
            return *tx_factory_;
        }

        void tx_factory(transaction_factory &val) {
            tx_factory_ = &val;
            impl_->set_tx_factory(transaction_factory::transaction_impl_factory_f);
            impl_->set_tx_factory_ud(tx_factory_);
        }

        subscription_factory &sbs_factory() const {
            return *sbs_factory_;
        }

        void sbs_factory(subscription_factory &val) {
            sbs_factory_ = &val;
            impl_->set_sbs_factory(subscription_factory::subscription_impl_factory_f);
            impl_->set_sbs_factory_ud(sbs_factory_);
        }

        RetCode bind_internal(peer &p) {
            RetCode rcode = vlg::RetCode_OK;
            if(!impl_) { //ugly test to detect outgoing/ingoing connection type..
                connection_impl *c_impl = nullptr;
                if((rcode = p.get_opaque()->new_connection(&c_impl,
                                                           vlg_conn_factory_cimpl,
                                                           ConnectionType_OUTGOING,
                                                           0,
                                                           &publ_)) == vlg::RetCode_OK) {
                    impl_ = c_impl;
                    vlg::collector &c = impl_->get_collector();
                    c.retain(impl_);
                }
            } else {
            }
            impl_->set_connection_status_change_handler(connection_impl_status_change_hndlr_cimpl, this);
            return rcode;
        }

    private:
        connection &publ_;
        peer *peer_;
        connection_impl *impl_;
        connection::status_change csh_;
        void *csh_ud_;

        //factories cannot be references
        transaction_factory *tx_factory_;
        subscription_factory *sbs_factory_;
};

//*************************************
//connection MEMORY MNGMENT BEGIN
//*************************************
class connection_collector : public vlg::collector {
    public:
        connection_collector() : vlg::collector("connection") {}
};

vlg::collector *inst_connection_collector = nullptr;
vlg::collector &get_inst_connection_collector()
{
    if(inst_connection_collector) {
        return *inst_connection_collector;
    }
    if(!(inst_connection_collector = new connection_collector())) {
        EXIT_ACTION
    }
    return *inst_connection_collector;
}

vlg::collector &connection::get_collector()
{
    return get_inst_connection_collector();
}

// CLASS connection
nclass_logger *connection::log_ = nullptr;

connection::connection()
{
    log_ = get_nclass_logger("connection");
    impl_ = new connection_impl_pub(*this);
    IFLOG(trc(TH_ID, LS_CTR "%s(ptr:%p)", __func__, this))
}

connection::~connection()
{
    vlg::collector &c = get_collector();
    if((c.is_instance_collected(this))) {
        IFLOG(cri(TH_ID, LS_DTR "%s(ptr:%p)" D_W_R_COLL LS_EXUNX,
                  __func__,
                  this))
    }
    if(impl_) {
        delete impl_;
    }
    IFLOG(trc(TH_ID, LS_DTR "%s(ptr:%p)", __func__, this))
}

RetCode connection::bind(peer &p)
{
    impl_->set_peer(p);
    return impl_->bind_internal(p);
}

peer &connection::get_peer()
{
    return *impl_->get_peer();
}

ConnectionType connection::get_connection_type() const
{
    return impl_->get_conn()->conn_type();
}

unsigned int connection::get_connection_id() const
{
    return impl_->get_conn()->connid();
}

ConnectionResult connection::get_connection_response() const
{
    return impl_->get_conn()->conn_response();
}

ConnectionResultReason connection::get_connection_result_code() const
{
    return impl_->get_conn()->conn_res_code();
}

unsigned short connection::get_client_heartbeat() const
{
    return impl_->get_conn()->client_agrhbt();
}

unsigned short connection::get_server_agreed_heartbeat() const
{
    return impl_->get_conn()->server_agrhbt();
}

DisconnectionResultReason connection::get_disconnection_reason_code()
const
{
    return impl_->get_conn()->discon_res_code();
}

ConnectionStatus connection::get_status()
{
    return impl_->get_conn()->status();
}

RetCode connection::await_for_status_reached_or_outdated(
    ConnectionStatus test, ConnectionStatus &current,
    time_t sec, long nsec)
{
    return impl_->get_conn()->await_for_status_reached_or_outdated(test,
                                                                   current,
                                                                   sec,
                                                                   nsec);
}

RetCode connection::await_for_status_change(ConnectionStatus
                                            &status,
                                            time_t sec,
                                            long nsec)
{
    return impl_->get_conn()->await_for_status_change(status, sec, nsec);
}

void connection::set_connection_status_change_handler(status_change
                                                      handler, void *ud)
{
    impl_->set_csh(handler);
    impl_->set_csh_ud(ud);
}

RetCode connection::connect(sockaddr_in &connection_params)
{
    return impl_->get_conn()->client_connect(connection_params);
}

RetCode connection::await_for_connection_result(
    ConnectivityEventResult
    &con_evt_res,
    ConnectivityEventType &connectivity_evt_type,
    time_t sec,
    long nsec)
{
    return impl_->get_conn()->await_for_connection_result(con_evt_res,
                                                          connectivity_evt_type, sec, nsec);
}

RetCode connection::disconnect(DisconnectionResultReason
                               reason_code)
{
    return impl_->get_conn()->disconnect(reason_code);
}

RetCode connection::await_for_disconnection_result(
    ConnectivityEventResult &con_evt_res,
    ConnectivityEventType &connectivity_evt_type,
    time_t sec,
    long nsec)
{
    return impl_->get_conn()->await_for_disconnection_result(con_evt_res,
                                                             connectivity_evt_type, sec, nsec);
}

void connection::on_connect(ConnectivityEventResult con_evt_res,
                            ConnectivityEventType connectivity_evt_type)
{
}

void connection::on_disconnect(ConnectivityEventResult con_evt_res,
                               ConnectivityEventType connectivity_evt_type)
{
}

transaction_factory &connection::get_transaction_factory()
{
    return impl_->tx_factory();
}

void connection::set_transaction_factory(transaction_factory &tx_factory)
{
    impl_->tx_factory(tx_factory);
}

subscription_factory &connection::get_subscription_factory()
{
    return impl_->sbs_factory();
}

void connection::set_subscription_factory(subscription_factory &sbs_factory)
{
    impl_->sbs_factory(sbs_factory);
}

connection_impl *connection::get_opaque()
{
    return impl_->get_conn();
}

void connection::set_opaque(connection_impl *conn)
{
    return impl_->set_conn(conn);
}

SOCKET connection::get_socket() const
{
    return impl_->get_conn()->get_socket();
}

const char *connection::get_host_ip() const
{
    return impl_->get_conn()->get_host_ip();
}

unsigned short connection::get_host_port() const
{
    return impl_->get_conn()->get_host_port();
}

// CLASS cimpl_conn_impl_server
class cimpl_conn_impl_server : public connection_impl {
    public:
        cimpl_conn_impl_server(connection &publ,
                               peer_impl &peer,
                               ConnectionType con_type,
                               unsigned int connid) :
            connection_impl(publ,
                            peer,
                            con_type,
                            connid),
            publ_(publ) {
        }
        virtual ~cimpl_conn_impl_server() {
            void *self = this;
            /************************
            RELEASE_ID: CPB_SRV_01
            ************************/
            vlg::collector &c = publ_.get_collector();
            c.release(&publ_);
        }
        virtual void on_connect(ConnectivityEventResult con_evt_res,
                                ConnectivityEventType connectivity_evt_type) {
            publ_.on_connect(con_evt_res, connectivity_evt_type);
        }
        virtual void on_disconnect(ConnectivityEventResult con_evt_res,
                                   ConnectivityEventType connectivity_evt_type) {
            publ_.on_disconnect(con_evt_res, connectivity_evt_type);
        }

    private:
        connection &publ_;
};

// CLASS connection_factory

connection_factory *default_conn_factory = nullptr;
connection_factory &connection_factory::default_factory()
{
    if(default_conn_factory  == nullptr) {
        default_conn_factory  = new connection_factory();
        if(!default_conn_factory) {
            EXIT_ACTION
        }
    }
    return *default_conn_factory;
}

connection_impl *connection_factory::connection_impl_factory_f(peer_impl &peer_internal,
                                                               ConnectionType con_type,
                                                               unsigned int connid,
                                                               void *ud)
{
    connection_factory *csf = static_cast<connection_factory *>(ud);
    peer &p_publ = peer_internal.get_public();
    connection &publ = csf->make_connection(p_publ);

    vlg::collector &c = publ.get_collector();
    /************************
    RETAIN_ID: CPB_SRV_01
    ************************/
    c.retain(&publ);

    connection_impl *impl_conn = new cimpl_conn_impl_server(publ,
                                                            peer_internal,
                                                            con_type,
                                                            connid);
    publ.set_opaque(impl_conn);
    publ.bind(p_publ);
    return impl_conn;
}

connection_factory::connection_factory()
{}

connection_factory::~connection_factory()
{}

connection &connection_factory::make_connection(peer &p)
{
    return *new connection();
}

}
