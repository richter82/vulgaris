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

#ifndef VLG_CONNECTION_H_
#define VLG_CONNECTION_H_

#include "vlg_memory.h"
#include "vlg_logger.h"
#include "vlg/vlg_proto.h"
#include "vlg_globint.h"

#define VLG_RECV_BUFF_SZ 512

namespace vlg {

//-----------------------------
// CONNECTION
//-----------------------------
class connection_impl : public vlg::collectable {
        friend class connection_impl_pub;
        friend class acceptor;
        friend class selector;
        friend class peer_impl;
        friend class peer_recv_task;

    public:
        typedef void (*connection_impl_status_change_hndlr)(connection_impl &conn,
                                                            ConnectionStatus status,
                                                            void *ud);

        /*************************************************************
        -Factory function types
        *************************************************************/
        typedef transaction_impl *(*vlg_tx_factory_func)(connection_impl &connection,
                                                         void *ud);

        typedef subscription_impl *(*vlg_sbs_factory_func)(connection_impl &connection,
                                                           void *ud);

    private:
        static transaction_impl *vlg_tx_factory_default_func(connection_impl
                                                             &connection,
                                                             void *ud);

        static subscription_impl *vlg_sbs_factory_default_func(connection_impl
                                                               &connection, void *ud);

        //---ctors
    protected:
        connection_impl(peer_impl &peer,
                        ConnectionType con_type,
                        unsigned int connid = 0);

        virtual ~connection_impl();

    public:
        virtual vlg::collector &get_collector();

        //-----------------------------
        // INIT
        //-----------------------------
    private:
        vlg::RetCode    init(unsigned int pkt_sending_q_capcty = 0);

        //-----------------------------
        // CLEAN BEST EFFORT
        //-----------------------------
    private:
        vlg::RetCode  clean_best_effort();
        void            clean_packet_snd_q();

        //-----------------------------
        // CONNECTIVITY
        //-----------------------------
    public:
        vlg::RetCode    server_send_connect_res();
        vlg::RetCode    client_connect(sockaddr_in &params);

        /* this function must be called from same thread that called ClientConnect()*/
        vlg::RetCode    await_for_connection_result(ConnectivityEventResult
                                                    &con_evt_res,
                                                    ConnectivityEventType &connectivity_evt_type,
                                                    time_t sec = -1,
                                                    long nsec = 0);

    public:
        vlg::RetCode    disconnect(DisconnectionResultReason disres);

        /* this function must be called from same thread that called Disconnect()*/
        vlg::RetCode    await_for_disconnection_result(ConnectivityEventResult
                                                       &con_evt_res,
                                                       ConnectivityEventType &connectivity_evt_type,
                                                       time_t sec = -1,
                                                       long nsec = 0);

    private:
        vlg::RetCode    notify_for_connectivity_result(ConnectivityEventResult
                                                       con_evt_res,
                                                       ConnectivityEventType connectivity_evt_type);

        //-----------------------------
        // APPLICATIVE HANDLERS
        //-----------------------------
    protected:
        virtual void on_connect(ConnectivityEventResult con_evt_res,
                                ConnectivityEventType connectivity_evt_type);

        virtual void on_disconnect(ConnectivityEventResult con_evt_res,
                                   ConnectivityEventType connectivity_evt_type);

        //-----------------------------
        // TRANSACTIONAL
        //-----------------------------
    public:
        vlg::RetCode    next_tx_id(tx_id &txid);

        vlg::RetCode    new_transaction(transaction_impl **new_transaction,
                                        vlg_tx_factory_func vlg_tx_factory_f = NULL,
                                        bool compute_txid = true,
                                        void *ud = NULL);

        /*******************************************************************
        CLIENT:
        no effect.

        SERVER:
        it [auto] release-safedestroy related transaction.
        *******************************************************************/
        vlg::RetCode    release_transaction(transaction_impl *transaction);

        //-----------------------------
        // SUBSCRIPTION
        //-----------------------------
    public:
        vlg::RetCode    new_subscription(subscription_impl **new_subscription,
                                         vlg_sbs_factory_func vlg_sbs_factory_f = NULL,
                                         void *ud = NULL);


        //client only
        vlg::RetCode    detach_subscription(subscription_impl *subscription);

        /*******************************************************************
        CLIENT:
        This function detaches passed subscription from the underlying
        [peer-connection] data-structures.
        Once you successfully called ReleaseSubscription, you can
        safedestroy related subscription.

        SERVER:
        same as client, but it also [auto] safedestroy related subscripion.
        *******************************************************************/
        vlg::RetCode    release_subscription(subscription_impl *subscription);

        //-----------------------------
        // GETTERS
        //-----------------------------
    public:
        peer_impl                    &peer();
        ConnectionType              conn_type() const;
        unsigned int                connid() const;
        unsigned int                next_prid();
        unsigned int                next_reqid();
        unsigned int                next_sbsid();
        ConnectionResult            conn_response() const;
        ConnectionResultReason      conn_res_code() const;
        unsigned short              client_agrhbt() const;
        unsigned short              server_agrhbt() const;
        DisconnectionResultReason   discon_res_code() const;
        vlg::blocking_queue       &pkt_snd_q();
        vlg::synch_hash_map       &client_fly_tx_map();
        vlg::synch_hash_map       &server_fly_tx_map();
        vlg::synch_hash_map       &class_id_sbs_map();
        vlg::synch_hash_map       &sbsid_sbs_map();
        vlg::synch_hash_map       &reqid_sbs_map();
        vlg_tx_factory_func         tx_factory() const;
        void                        *tx_factory_ud() const;
        vlg_sbs_factory_func        sbs_factory() const;
        void                        *sbs_factory_ud() const;

        //-----------------------------
        // SETTERS
        //-----------------------------
    private:
        vlg::RetCode  set_connid(unsigned int connid);
        void            set_conn_response(ConnectionResult val);
        void            set_conn_res_code(ConnectionResultReason val);
        void            set_client_agrhbt(unsigned short val);
        void            set_server_agrhbt(unsigned short val);
        void            set_tx_factory(vlg_tx_factory_func val);
        void            set_tx_factory_ud(void *ud);
        void            set_sbs_factory(vlg_sbs_factory_func val);
        void            set_sbs_factory_ud(void *ud);

        //-----------------------------
        // STATUS SYNCHRO
        //-----------------------------
    public:
        vlg::RetCode    await_for_status_reached_or_outdated(ConnectionStatus
                                                             test,
                                                             ConnectionStatus &current,
                                                             time_t sec = -1,
                                                             long nsec = 0);

        vlg::RetCode    await_for_status_change(ConnectionStatus &status,
                                                time_t sec = -1,
                                                long nsec = 0);

        //-----------------------------
        // STATUS ASYNCHRO HNDLRS
        //-----------------------------
    public:
        void set_connection_status_change_handler(connection_impl_status_change_hndlr
                                                  hndlr,
                                                  void *ud);

        //-----------------------------
        // STATUS
        //-----------------------------
    public:
        ConnectionStatus status();

    private:
        vlg::RetCode    set_connection_established();
        vlg::RetCode    set_connection_established(SOCKET socket);
        vlg::RetCode    set_proto_connected();
        vlg::RetCode    set_appl_connected();
        vlg::RetCode    set_disconnecting();
        vlg::RetCode    set_socket_disconnected();

        vlg::RetCode    set_proto_error(vlg::RetCode cause_res =
                                            vlg::RetCode_UNKERR);

        vlg::RetCode    set_socket_error(vlg::RetCode cause_res =
                                             vlg::RetCode_UNKERR);

        vlg::RetCode    set_implernal_error(vlg::RetCode cause_res =
                                                vlg::RetCode_UNKERR);

        vlg::RetCode    set_status(ConnectionStatus status);

        //-----------------------------
        // vlg PROTOCOL RCVNG INTERFACE
        //-----------------------------
    private:
        vlg::RetCode recv_connection_request(const vlg_hdr_rec *pkt_hdr);
        vlg::RetCode recv_connection_response(const vlg_hdr_rec *pkt_hdr);
        vlg::RetCode recv_test_request(const vlg_hdr_rec *pkt_hdr);
        vlg::RetCode recv_disconnection(const vlg_hdr_rec *pkt_hdr);

        //TX
    private:
        vlg::RetCode recv_tx_req(const vlg_hdr_rec *pkt_hdr,
                                 vlg::grow_byte_buffer *pkt_body);

        vlg::RetCode recv_tx_res(const vlg_hdr_rec *pkt_hdr,
                                 vlg::grow_byte_buffer *pkt_body);

        //SBS
        vlg::RetCode recv_sbs_start_req(const vlg_hdr_rec *pkt_hdr);
        vlg::RetCode recv_sbs_start_res(const vlg_hdr_rec *pkt_hdr);

        vlg::RetCode recv_sbs_evt(const vlg_hdr_rec *pkt_hdr,
                                  vlg::grow_byte_buffer *pkt_body);

        vlg::RetCode recv_sbs_evt_ack(const vlg_hdr_rec *hdr);
        vlg::RetCode recv_sbs_stop_req(const vlg_hdr_rec *pkt_hdr);
        vlg::RetCode recv_sbs_stop_res(const vlg_hdr_rec *pkt_hdr);

        //-----------------------------
        // TCP/IP
        //-----------------------------
    public:
        SOCKET              get_socket() const;
        const char         *get_host_ip() const;
        unsigned short      get_host_port() const;
        int                 get_last_socket_err() const;

        //TCP/IP SOCKET OPS
    private:
        vlg::RetCode            set_socket_blocking_mode(bool blocking);
        vlg::RetCode            establish_connection(sockaddr_in &params);
        vlg::RetCode            socket_shutdown();
        vlg::RetCode            socket_excptn_hndl(long sock_op_res);

        //TCP/IP SENDING
    private:
        vlg::RetCode            send_single_pkt(vlg::grow_byte_buffer *pkt_bbuf);

        //TCP/IP RECEIVING
    private:
        vlg::RetCode            recv_single_pkt(vlg_hdr_rec *pkt_hdr,
                                                vlg::grow_byte_buffer *pkt_body);

        vlg::RetCode            recv_and_decode_hdr(vlg_hdr_rec *pkt_hdr);

        vlg::RetCode            recv_body(unsigned int bodylen,
                                          vlg::grow_byte_buffer *pkt_body);

        vlg::RetCode            recv_single_hdr_row(unsigned int *hdr_row);

        //-----------------------------
        // REP
        //-----------------------------
    private:
        peer_impl                    &peer_; // associated peer.
        ConnectionType              con_type_;

        //--tcp/ip rep
        SOCKET                      socket_;
        int                         last_socket_err_;
        ConnectivityEventResult     con_evt_res_;
        ConnectivityEventType       connectivity_evt_type_;

    private:
        //---vlg proto rep
        unsigned int                connid_;
        ConnectionStatus            status_;
        ConnectionResult            conres_;
        ConnectionResultReason      conrescode_;
        unsigned short              cli_agrhbt_;
        unsigned short              srv_agrhbt_;
        DisconnectionResultReason   disconrescode_;

        //--synch status
        connection_impl_status_change_hndlr csc_hndl_;
        void                                *csc_hndl_ud_;
        bool                                connect_evt_occur_;

        //---packet sending queue
        vlg::blocking_queue   pkt_sending_q_;
        //srv tx repo
        vlg::synch_hash_map   srv_flytx_repo_; //server txid --> VLG_TRANSACTION
        //cli tx repo
        vlg::synch_hash_map   cli_flytx_repo_; //client txid --> VLG_TRANSACTION
        //srv sbs repo
        //srv nclass_id --> VLG_SUBSCRIPTION
        vlg::synch_hash_map   srv_classid_sbs_repo_;
        vlg::synch_hash_map   srv_sbsid_sbs_repo_; //srv sbsid --> VLG_SUBSCRIPTION
        //cli sbs repo
        vlg::synch_hash_map   cli_reqid_sbs_repo_; //cli reqid --> VLG_SUBSCRIPTION
        vlg::synch_hash_map   cli_sbsid_sbs_repo_; //cli sbsid --> VLG_SUBSCRIPTION

        vlg_tx_factory_func tx_factory_;  //factory for server tx
        void                *tx_factory_ud_; //factory for server tx ud

        vlg_sbs_factory_func sbs_factory_;  //factory for server sbs
        void                 *sbs_factory_ud_; //factory for server sbs ud

    private:
        mutable vlg::synch_monitor mon_;
        unsigned int                prid_;
        unsigned int                reqid_;
        unsigned int                sbsid_;

    protected:
        static vlg::logger    *log_;
};

}

#endif