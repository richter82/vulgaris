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

#include "vlg_connection.h"
using namespace vlg;

extern "C" {

    typedef struct own_outgoing_connection own_outgoing_connection;

    typedef void(*inco_connection_status_change)(incoming_connection *ic,
                                                 ConnectionStatus status,
                                                 void *ud);

    typedef void(*inco_connection_on_disconnect_handler)(incoming_connection *ic,
                                                         ConnectivityEventResult con_evt_res,
                                                         ConnectivityEventType c_evt_type,
                                                         void *ud);

    typedef void(*outg_connection_status_change)(outgoing_connection *oc,
                                                 ConnectionStatus status,
                                                 void *ud);

    typedef void(*outg_connection_on_connect_handler)(outgoing_connection *oc,
                                                      ConnectivityEventResult con_evt_res,
                                                      ConnectivityEventType c_evt_type,
                                                      void *ud);

    typedef void(*outg_connection_on_disconnect_handler)(outgoing_connection *oc,
                                                         ConnectivityEventResult con_evt_res,
                                                         ConnectivityEventType c_evt_type,
                                                         void *ud);
}

//inco_connection

extern "C" {

    peer *inco_connection_get_peer(incoming_connection *ic)
    {
        return &(ic->get_peer());
    }

    unsigned int inco_connection_get_connection_id(incoming_connection *ic)
    {
        return ic->get_id();
    }

    unsigned short inco_connection_get_client_heartbeat(incoming_connection *ic)
    {
        return ic->get_client_heartbeat();
    }

    unsigned short inco_connection_get_server_agreed_heartbeat(incoming_connection *ic)
    {
        return ic->get_server_agreed_heartbeat();
    }

    ConnectionStatus inco_connection_get_status(incoming_connection *ic)
    {
        return ic->get_status();
    }

    RetCode inco_connection_await_for_status_reached_or_outdated(incoming_connection *ic,
                                                                 ConnectionStatus test,
                                                                 ConnectionStatus *current,
                                                                 time_t sec,
                                                                 long nsec)
    {
        return ic->await_for_status_reached_or_outdated(test, *current, sec, nsec);
    }

    RetCode inco_connection_await_for_status_change(incoming_connection *ic,
                                                    ConnectionStatus *status,
                                                    time_t sec,
                                                    long nsec)
    {
        return ic->await_for_status_change(*status, sec, nsec);
    }



    RetCode inco_connection_disconnect(incoming_connection *ic,
                                       ProtocolCode reason_code)
    {
        return ic->disconnect(reason_code);
    }

    RetCode inco_connection_await_for_disconnection_result(incoming_connection *ic,
                                                           ConnectivityEventResult *con_evt_res,
                                                           ConnectivityEventType *connectivity_evt_type,
                                                           time_t sec,
                                                           long nsec)
    {
        return ic->await_for_disconnection_result(*con_evt_res, *connectivity_evt_type, sec, nsec);
    }

    SOCKET inco_connection_get_socket(incoming_connection *ic)
    {
        return ic->get_socket();
    }

    const char *inco_connection_get_host_ip(incoming_connection *ic)
    {
        return ic->get_host_ip();
    }

    unsigned short inco_connection_get_host_port(incoming_connection *ic)
    {
        return ic->get_host_port();
    }
}

//c_outg_conn

struct c_outg_conn : public outgoing_connection {
    c_outg_conn() :
        ocsc_(nullptr),
        ococh_(nullptr),
        ocodh_(nullptr),
        ocsc_ud_(nullptr),
        ococh_ud_(nullptr),
        ocodh_ud_(nullptr) {}

    virtual void on_status_change(ConnectionStatus current) override {
        ocsc_(this, current, ocsc_ud_);
    }

    virtual void on_connect(ConnectivityEventResult con_evt_res,
                            ConnectivityEventType c_evt_type) override {
        ococh_(this, con_evt_res, c_evt_type, ococh_ud_);
    }

    virtual void on_disconnect(ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type) override {
        ocodh_(this, con_evt_res, c_evt_type, ocodh_ud_);
    }

    outg_connection_status_change ocsc_;
    outg_connection_on_connect_handler ococh_;
    outg_connection_on_disconnect_handler ocodh_;

    void *ocsc_ud_;
    void *ococh_ud_;
    void *ocodh_ud_;
};

//outg_connection

extern "C" {

    own_outgoing_connection *outg_connection_create(void)
    {
        return (own_outgoing_connection *) new c_outg_conn();
    }

    outgoing_connection *outg_connection_get_ptr(own_outgoing_connection *oc)
    {
        return (outgoing_connection *) oc;
    }

    void outg_connection_destroy(own_outgoing_connection *oc)
    {
        delete(c_outg_conn *)oc;
    }

    RetCode outg_connection_bind(outgoing_connection *oc, peer *p)
    {
        return oc->bind(*p);
    }

    peer *outg_connection_get_peer(outgoing_connection *oc)
    {
        return &(oc->get_peer());
    }

    unsigned int outg_connection_get_connection_id(outgoing_connection *oc)
    {
        return oc->get_id();
    }

    ConnectionResult outg_connection_get_connection_response(outgoing_connection *oc)
    {
        return oc->get_connection_response();
    }

    ProtocolCode outg_connection_get_connection_result_code(outgoing_connection *oc)
    {
        return oc->get_connection_result_code();
    }

    unsigned short outg_connection_get_client_heartbeat(outgoing_connection *oc)
    {
        return oc->get_client_heartbeat();
    }

    unsigned short outg_connection_get_server_agreed_heartbeat(outgoing_connection *oc)
    {
        return oc->get_server_agreed_heartbeat();
    }

    ProtocolCode outg_connection_get_disconnection_reason_code(outgoing_connection *oc)
    {
        return oc->get_disconnection_reason_code();
    }

    ConnectionStatus outg_connection_get_status(outgoing_connection *oc)
    {
        return oc->get_status();
    }

    RetCode outg_connection_await_for_status_reached_or_outdated(outgoing_connection *oc,
                                                                 ConnectionStatus test,
                                                                 ConnectionStatus *current,
                                                                 time_t sec,
                                                                 long nsec)
    {
        return oc->await_for_status_reached_or_outdated(test, *current, sec, nsec);
    }

    RetCode outg_connection_await_for_status_change(outgoing_connection *oc,
                                                    ConnectionStatus *status,
                                                    time_t sec,
                                                    long nsec)
    {
        return oc->await_for_status_change(*status, sec, nsec);
    }

    void outg_connection_set_status_change_handler(outgoing_connection *oc,
                                                   outg_connection_status_change handler, void *ud)
    {
        static_cast<c_outg_conn *>(oc)->ocsc_ = handler;
        static_cast<c_outg_conn *>(oc)->ocsc_ud_ = ud;
    }

    RetCode outg_connection_connect(outgoing_connection *oc, sockaddr_in *connection_params)
    {
        return oc->connect(*connection_params);
    }

    RetCode outg_connection_await_for_connection_result(outgoing_connection *oc,
                                                        ConnectivityEventResult *con_evt_res,
                                                        ConnectivityEventType *connectivity_evt_type,
                                                        time_t sec,
                                                        long nsec)
    {
        return oc->await_for_connection_result(*con_evt_res, *connectivity_evt_type, sec, nsec);
    }

    RetCode outg_connection_disconnect(outgoing_connection *oc,
                                       ProtocolCode reason_code)
    {
        return oc->disconnect(reason_code);
    }

    RetCode outg_connection_await_for_disconnection_result(outgoing_connection *oc,
                                                           ConnectivityEventResult *con_evt_res,
                                                           ConnectivityEventType *connectivity_evt_type,
                                                           time_t sec,
                                                           long nsec)
    {
        return oc->await_for_disconnection_result(*con_evt_res, *connectivity_evt_type, sec, nsec);
    }

    void outg_connection_set_on_connect_handler(outgoing_connection *oc,
                                                outg_connection_on_connect_handler hndl, void *ud)
    {
        static_cast<c_outg_conn *>(oc)->ococh_ = hndl;
        static_cast<c_outg_conn *>(oc)->ococh_ud_ = ud;
    }

    void outg_connection_set_on_disconnect_handler(outgoing_connection *oc,
                                                   outg_connection_on_disconnect_handler hndl, void *ud)
    {
        static_cast<c_outg_conn *>(oc)->ocodh_ = hndl;
        static_cast<c_outg_conn *>(oc)->ocodh_ud_ = ud;
    }

    SOCKET outg_connection_get_socket(outgoing_connection *oc)
    {
        return oc->get_socket();
    }

    const char *outg_connection_get_host_ip(outgoing_connection *oc)
    {
        return oc->get_host_ip();
    }

    unsigned short outg_connection_get_host_port(outgoing_connection *oc)
    {
        return oc->get_host_port();
    }
}



