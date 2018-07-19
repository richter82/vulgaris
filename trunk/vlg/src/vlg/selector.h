/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
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
    SOCKET socket_;
    sockaddr_in saddr_;
};

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
    explicit selector(peer_impl &);
    ~selector();

    RetCode init(unsigned int srv_executors,
                 unsigned int cli_executors);

    RetCode on_peer_start_actions();
    RetCode on_peer_move_running_actions();

    RetCode await_for_status_reached(SelectorStatus test,
                                     SelectorStatus &current,
                                     time_t sec = -1,
                                     long nsec = 0);

    RetCode asynch_notify(const selector_event *);
    RetCode consume_asynch_events();
    RetCode interrupt();
    RetCode set_status(SelectorStatus);
    virtual void *run() override;
    RetCode create_UDP_notify_srv_sock();
    RetCode connect_UDP_notify_cli_sock();
    bool is_still_valid_connection(const selector_event *);
    RetCode start_exec_services();
    RetCode process_inco_sock_inco_events();
    RetCode process_inco_sock_outg_events();
    RetCode process_outg_sock_inco_events();
    RetCode process_outg_sock_outg_events();
    RetCode start_conn_objs();
    void FDSET_sockets();
    void FDSET_incoming_sockets();
    void FDSET_outgoing_sockets();
    void FDSET_write_incoming_pending_sockets();
    void FDSET_write_outgoing_pending_sockets();
    RetCode server_socket_shutdown();
    RetCode consume_events();
    RetCode consume_inco_sock_events();
    RetCode add_early_outg_conn(selector_event *);
    RetCode promote_early_outg_conn(conn_impl *);
    RetCode delete_early_outg_conn(conn_impl *);
    RetCode manage_disconnect_conn(selector_event *);
    RetCode stop_and_clean();

    //rep
    peer_impl &peer_;
    SelectorStatus status_;
    fd_set read_FDs_, write_FDs_, excep_FDs_;

    int nfds_;
    int sel_res_;
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
