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

#ifndef BLZ_SEL_H_
#define BLZ_SEL_H_
#include "blz_acceptor_int.h"

namespace vlg {

class peer;
class peer_int;

//-----------------------------
// selector_event
//-----------------------------
class connection_int;
struct selector_event {
    selector_event(BLZ_SELECTOR_Evt evt, connection_int *conn);

    BLZ_SELECTOR_Evt    evt_;
    ConnectionType      con_type_;
    connection_int      *conn_;
    //not really safe rely on this (what if socket is reused by the system?).
    SOCKET              socket_;
    unsigned int        connid_;
    sockaddr_in         saddr_;
};

#define BLZ_DEF_SRV_EXEC_NO     1
#define BLZ_DEF_SRV_EXEC_Q_LEN  0
#define BLZ_DEF_CLI_EXEC_NO     1
#define BLZ_DEF_CLI_EXEC_Q_LEN  0
#define BLZ_DEF_SEL_EVT_Q_LEN   0
#define BLZ_ADDR_LEN            100
#define BUFF_SIZE               1024

//-----------------------------
// BLZ_ASYNCH_SELECTOR_STATUS
//-----------------------------
enum BLZ_ASYNCH_SELECTOR_STATUS {
    BLZ_ASYNCH_SELECTOR_STATUS_UNDEF,
    BLZ_ASYNCH_SELECTOR_STATUS_TO_INIT,
    BLZ_ASYNCH_SELECTOR_STATUS_INIT,
    BLZ_ASYNCH_SELECTOR_STATUS_REQUEST_READY,    // <<-- asynch request to transit into Ready status.
    BLZ_ASYNCH_SELECTOR_STATUS_READY,
    BLZ_ASYNCH_SELECTOR_STATUS_REQUEST_SELECT,   // <<-- asynch request to transit into Selecting status.
    BLZ_ASYNCH_SELECTOR_STATUS_SELECT,
    BLZ_ASYNCH_SELECTOR_STATUS_REQUEST_STOP,     // <<-- asynch request to transit into Stopped status.
    BLZ_ASYNCH_SELECTOR_STATUS_STOPPING,
    BLZ_ASYNCH_SELECTOR_STATUS_STOPPED,
    BLZ_ASYNCH_SELECTOR_STATUS_ERROR = 500,
};

//-----------------------------
// selector
//-----------------------------
class selector : public vlg::p_thread {
    public:
        //---ctors
        selector(peer_int &peer, unsigned int id);
        ~selector();

    public:
        vlg::RetCode    init(unsigned int srv_executors,
                               unsigned int srv_pkt_q_len,
                               unsigned int cli_executors,
                               unsigned int cli_pkt_q_len);


        vlg::RetCode    on_peer_start_actions();
        vlg::RetCode    on_peer_move_running_actions();

    public:
        vlg::RetCode
        await_for_status_reached_or_outdated(
            BLZ_ASYNCH_SELECTOR_STATUS
            test,
            BLZ_ASYNCH_SELECTOR_STATUS &current,
            time_t sec = -1,
            long nsec = 0);

        vlg::RetCode    evt_enqueue_and_notify(const selector_event *evt);
        vlg::RetCode    interrupt();

    public:
        BLZ_ASYNCH_SELECTOR_STATUS  status() const;
        vlg::RetCode              set_status(BLZ_ASYNCH_SELECTOR_STATUS status);

    public:
        peer_int        &peer();
        sockaddr_in     get_srv_sock_addr();
        timeval         get_select_timeout();
        SOCKET          get_UDP_notify_srv_sock();
        SOCKET          get_UDP_notify_cli_sock();

        uint32_t        inco_conn_count();
        uint32_t        outg_conn_count();

    public:
        void            set_srv_sock_addr(sockaddr_in srv_sockaddr_in);
        void            set_select_timeout(timeval sel_timeout);

        virtual void   *run();

    private:
        vlg::RetCode  create_UDP_notify_srv_sock();
        vlg::RetCode  connect_UDP_notify_cli_sock();
        bool            is_still_valid_connection(const selector_event *evt);
        vlg::RetCode  asynch_notify(const selector_event *evt);
        vlg::RetCode  consume_asynch_events();

        vlg::RetCode  start_exec_services();
        vlg::RetCode  process_inco_sock_inco_events();
        vlg::RetCode  process_outg_sock_inco_events();
        vlg::RetCode  process_sock_outg_events();

        vlg::RetCode  start_conn_objs();

        vlg::RetCode  FDSET_sockets();
        vlg::RetCode  FDSET_incoming_sockets();   //for server side sockets
        vlg::RetCode  FDSET_outgoing_sockets();   //for client side sockets
        vlg::RetCode  FDSET_write_pending_sockets();
        vlg::RetCode  server_socket_shutdown();

        vlg::RetCode  consume_events();
        vlg::RetCode  consume_inco_sock_events();

        vlg::RetCode  add_early_outg_conn(selector_event *conn_evt);
        vlg::RetCode  promote_early_outg_conn(connection_int *conn);
        vlg::RetCode  delete_early_outg_conn(connection_int *conn);

        vlg::RetCode  manage_disconnect_conn(selector_event *conn_evt);

        vlg::RetCode  stop_and_clean();

    private:
        //---gen_rep
        peer_int                    &peer_;
        unsigned int                id_;
        BLZ_ASYNCH_SELECTOR_STATUS  status_;
        fd_set                      read_FDs_,
                                    write_FDs_,
                                    excep_FDs_;

        int                         nfds_;      //used in select
        vlg::RetCode              last_err_;  //last generic error
        int                         sel_res_;   //last select() result
        timeval                     sel_timeout_;
        sockaddr_in                 udp_ntfy_sa_in_;
        SOCKET                      udp_ntfy_srv_socket_;
        SOCKET                      udp_ntfy_cli_socket_;
        mutable vlg::synch_monitor mon_;

        //---srv_rep
        SOCKET                      srv_listen_socket_;
        sockaddr_in                 srv_sockaddr_in_;
        acceptor                    srv_acceptor_;

        //SERVER MAP connid --> BLZ_CONNECTION
        vlg::hash_map             srv_incoming_sock_map_;
        vlg::p_executor_service   srv_exec_serv_; //srv executor service.

        //---cli_rep
        //CLIENT MAP socketid --> BLZ_CONNECTION
        vlg::hash_map             cli_early_outgoing_sock_map_;
        //CLIENT MAP connid --> BLZ_CONNECTION
        vlg::hash_map             cli_outgoing_sock_map_;
        vlg::p_executor_service   cli_exec_serv_; //cli executor service.

        //CLIENT MAP socket --> BLZ_CONNECTION
        vlg::hash_map             write_pending_sockets_;

        //--logger
    protected:
        static vlg::logger   *log_;
};

}

#endif
