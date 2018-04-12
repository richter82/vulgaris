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
#include "proto.h"
#include "acceptor.h"

namespace vlg {

// selector_event

struct selector_event {
    explicit selector_event(VLG_SELECTOR_Evt evt, conn_impl *conn);
    explicit selector_event(VLG_SELECTOR_Evt evt, std::shared_ptr<incoming_connection> &conn);

    VLG_SELECTOR_Evt evt_;
    ConnectionType con_type_;
    conn_impl *conn_;
    std::shared_ptr<incoming_connection> inco_conn_;

    //not really safe rely on this,
    //what if socket is reused by the system?.
    SOCKET socket_;
    sockaddr_in saddr_;
};

#define VLG_DEF_SRV_EXEC_NO     1
#define VLG_DEF_CLI_EXEC_NO     1
#define VLG_ADDR_LEN            100
#define BUFF_SIZE               1024

// SelectorStatus

enum SelectorStatus {
    SelectorStatus_UNDEF,
    SelectorStatus_TO_INIT,
    SelectorStatus_INIT,
    SelectorStatus_REQUEST_READY,    // <<-- asynch request to transit into Ready status.
    SelectorStatus_READY,
    SelectorStatus_REQUEST_SELECT,   // <<-- asynch request to transit into Selecting status.
    SelectorStatus_SELECT,
    SelectorStatus_REQUEST_STOP,     // <<-- asynch request to transit into Stopped status.
    SelectorStatus_STOPPING,
    SelectorStatus_STOPPED,
    SelectorStatus_ERROR = 500,
};

struct selector : public p_th {
    explicit selector(peer_impl &peer);
    ~selector();

    RetCode init(unsigned int srv_executors,
                 unsigned int cli_executors);

    RetCode on_peer_start_actions();
    RetCode on_peer_move_running_actions();

    RetCode await_for_status_reached_or_outdated(SelectorStatus test,
                                                 SelectorStatus &current,
                                                 time_t sec = -1,
                                                 long nsec = 0);

    RetCode evt_enqueue_and_notify(const selector_event *evt);
    RetCode interrupt();

    RetCode set_status(SelectorStatus status);

    SOCKET get_UDP_notify_srv_sock();
    SOCKET get_UDP_notify_cli_sock();

    virtual void *run();

    RetCode create_UDP_notify_srv_sock();
    RetCode connect_UDP_notify_cli_sock();
    bool is_still_valid_connection(const selector_event *evt);
    RetCode asynch_notify(const selector_event *evt);
    RetCode consume_asynch_events();

    RetCode start_exec_services();
    RetCode process_inco_sock_inco_events();
    RetCode process_inco_sock_outg_events();
    RetCode process_outg_sock_inco_events();
    RetCode process_outg_sock_outg_events();

    RetCode start_conn_objs();

    RetCode FDSET_sockets();
    RetCode FDSET_incoming_sockets();
    RetCode FDSET_outgoing_sockets();
    RetCode FDSET_write_incoming_pending_sockets();
    RetCode FDSET_write_outgoing_pending_sockets();
    RetCode server_socket_shutdown();

    RetCode consume_events();
    RetCode consume_inco_sock_events();

    RetCode add_early_outg_conn(selector_event *conn_evt);
    RetCode promote_early_outg_conn(conn_impl *conn);
    RetCode delete_early_outg_conn(conn_impl *conn);

    RetCode manage_disconnect_conn(selector_event *conn_evt);

    RetCode stop_and_clean();

    //rep
    peer_impl &peer_;
    SelectorStatus status_;
    fd_set read_FDs_, write_FDs_, excep_FDs_;

    int nfds_;          //used in select
    int sel_res_;       //last select() result
    timeval sel_timeout_;
    sockaddr_in udp_ntfy_sa_in_;
    SOCKET udp_ntfy_srv_socket_;
    SOCKET udp_ntfy_cli_socket_;
    mutable mx mon_;

    //---srv_rep
    SOCKET srv_listen_socket_;
    sockaddr_in srv_sockaddr_in_;
    acceptor srv_acceptor_;
    std::unordered_map<unsigned int, std::shared_ptr<incoming_connection>> inco_connid_conn_map_;
    std::unordered_map<SOCKET, std::shared_ptr<incoming_connection>> write_pending_sock_inco_conn_map_;
    p_exec_srv inco_exec_srv_;

    //---cli_rep
    std::unordered_map<SOCKET, conn_impl *> outg_early_sock_conn_map_;
    std::unordered_map<unsigned int, conn_impl *> outg_connid_conn_map_;
    std::unordered_map<SOCKET, conn_impl *> write_pending_sock_outg_conn_map_;
    p_exec_srv outg_exec_srv_;
};

}
