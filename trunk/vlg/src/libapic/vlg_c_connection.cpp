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

#include "blaze_c_connection.h"
#include "blaze_connection.h"

namespace vlg {

class c_connection : public connection {
    private:
        static void connection_status_change_c_connection(connection &conn,
                                                          ConnectionStatus status,
                                                          void *ud) {
            c_connection &self = static_cast<c_connection &>(conn);
            self.csc_wr_((connection_wr)&conn, status, self.csc_ud_);
        }

    public:
        c_connection() :
            och_wr_(NULL),
            odh_wr_(NULL),
            csc_wr_(NULL),
            och_ud_(NULL),
            odh_ud_(NULL),
            csc_ud_(NULL) {
        }

        // CONNECTIVITY APPLICATIVE HANDLERS
    public:
        virtual void on_connect(ConnectivityEventResult con_evt_res,
                                ConnectivityEventType connectivity_evt_type) {
            if(och_wr_) {
                och_wr_((connection_wr)this, con_evt_res, connectivity_evt_type, och_ud_);
            }
        }

        virtual void on_disconnect(ConnectivityEventResult con_evt_res,
                                   ConnectivityEventType connectivity_evt_type) {
            if(odh_wr_) {
                odh_wr_((connection_wr)this, con_evt_res, connectivity_evt_type, odh_ud_);
            }
        }

        on_connect_handler_wr Och_wr() const {
            return och_wr_;
        }

        void Och_wr(on_connect_handler_wr val) {
            och_wr_ = val;
        }

        on_disconnect_handler_wr Odh_wr() const {
            return odh_wr_;
        }

        void Odh_wr(on_disconnect_handler_wr val) {
            odh_wr_ = val;
        }

        connection_status_change_wr Csc_wr() const {
            return csc_wr_;
        }

        void Csc_wr(connection_status_change_wr val) {
            csc_wr_ = val;
        }

        void *Csc_ud() const {
            return csc_ud_;
        }

        void Csc_ud(void *val) {
            csc_ud_ = val;
            set_connection_status_change_handler(connection_status_change_c_connection,
                                                 csc_ud_);
        }

        void *Och_ud() const {
            return och_ud_;
        }
        void Och_ud(void *val) {
            och_ud_ = val;
        }

        void *Odh_ud() const {
            return odh_ud_;
        }
        void Odh_ud(void *val) {
            odh_ud_ = val;
        }

    private:
        on_connect_handler_wr och_wr_;
        on_disconnect_handler_wr odh_wr_;
        connection_status_change_wr csc_wr_;
        void *och_ud_;
        void *odh_ud_;
        void *csc_ud_;
};

extern "C" {
    connection_wr connection_create()
    {
        return new c_connection();
    }

    void connection_destroy(connection_wr conn)
    {
        delete static_cast<c_connection *>(conn);
    }

    RetCode connection_bind(connection_wr conn, peer_wr p)
    {
        return static_cast<connection *>(conn)->bind(*(peer *)p);
    }

    peer_wr connection_get_peer(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_peer();
    }

    ConnectionType connection_get_connection_type(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_connection_type();
    }

    unsigned int connection_get_connection_id(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_connection_id();
    }

    ConnectionResult connection_get_connection_response(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_connection_response();
    }

    ConnectionResultReason connection_get_connection_result_code(
        connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_connection_result_code();
    }

    unsigned short connection_get_client_heartbeat(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_client_heartbeat();
    }

    unsigned short connection_get_server_agreed_heartbeat(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_server_agreed_heartbeat();
    }

    DisconnectionResultReason connection_get_disconnection_reason_code(
        connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_disconnection_reason_code();
    }

    ConnectionStatus connection_get_status(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_status();
    }

    RetCode connection_await_for_status_reached_or_outdated(connection_wr conn,
                                                            ConnectionStatus test,
                                                            ConnectionStatus *current,
                                                            time_t sec,
                                                            long nsec)
    {
        return static_cast<connection *>(conn)->await_for_status_reached_or_outdated(
                   test, *current,
                   sec,
                   nsec);
    }

    RetCode connection_await_for_status_change(connection_wr conn,
                                               ConnectionStatus *status,
                                               time_t sec,
                                               long nsec)
    {
        return static_cast<connection *>(conn)->await_for_status_change(*status, sec,
                                                                        nsec);
    }

    void connection_set_status_change_handler(connection_wr conn,
                                              connection_status_change_wr handler, void *ud)
    {
        static_cast<c_connection *>(conn)->Csc_wr(handler);
        static_cast<c_connection *>(conn)->Csc_ud(ud);
    }

    RetCode connection_connect(connection_wr conn, sockaddr_in *connection_params)
    {
        return static_cast<connection *>(conn)->connect(*connection_params);
    }

    RetCode connection_await_for_connection_result(connection_wr conn,
                                                   ConnectivityEventResult *con_evt_res,
                                                   ConnectivityEventType *connectivity_evt_type,
                                                   time_t sec,
                                                   long nsec)
    {
        return static_cast<connection *>(conn)->await_for_connection_result(
                   *con_evt_res, *connectivity_evt_type, sec, nsec);
    }

    RetCode connection_disconnect(connection_wr conn,
                                  DisconnectionResultReason reason_code)
    {
        return static_cast<connection *>(conn)->disconnect(reason_code);
    }

    RetCode connection_await_for_disconnection_result(connection_wr conn,
                                                      ConnectivityEventResult *con_evt_res,
                                                      ConnectivityEventType *connectivity_evt_type,
                                                      time_t sec,
                                                      long nsec)
    {
        return static_cast<connection *>(conn)->await_for_disconnection_result(
                   *con_evt_res, *connectivity_evt_type, sec, nsec);
    }

    void connection_set_on_connect_handler(connection_wr conn,
                                           on_connect_handler_wr hndl, void *ud)
    {
        static_cast<c_connection *>(conn)->Och_wr(hndl);
        static_cast<c_connection *>(conn)->Och_ud(ud);
    }

    void connection_set_on_disconnect_handler(connection_wr conn,
                                              on_disconnect_handler_wr hndl, void *ud)
    {
        static_cast<c_connection *>(conn)->Odh_wr(hndl);
        static_cast<c_connection *>(conn)->Odh_ud(ud);
    }

    SOCKET connection_get_socket(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_socket();
    }

    const char *connection_get_host_ip(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_host_ip();
    }

    unsigned short connection_get_host_port(connection_wr conn)
    {
        return static_cast<connection *>(conn)->get_host_port();
    }
}
}


