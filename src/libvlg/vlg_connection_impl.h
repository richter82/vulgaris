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

#ifndef BLZ_CONNECTION_H_
#define BLZ_CONNECTION_H_

#include "blaze_memory.h"
#include "blaze_logger.h"
#include "blz_glob_int.h"

#define BLZ_RECV_BUFF_SZ 512

namespace blaze {

//-----------------------------
// CONNECTION
//-----------------------------
class connection_int : public blaze::collectable {
        friend class connection_impl;
        friend class acceptor;
        friend class selector;
        friend class peer_int;
        friend class peer_recv_task;

    public:
        typedef void (*connection_impl_status_change_hndlr)(connection_int &conn,
                                                            ConnectionStatus status,
                                                            void *ud);

        /*************************************************************
        -Factory function types
        *************************************************************/
        typedef transaction_int *(*blz_tx_factory_func)(connection_int &connection,
                                                        void *ud);

        typedef subscription_int *(*blz_sbs_factory_func)(connection_int &connection,
                                                          void *ud);

    private:
        static transaction_int *blz_tx_factory_default_func(connection_int &connection,
                                                            void *ud);

        static subscription_int *blz_sbs_factory_default_func(connection_int
                                                              &connection, void *ud);

        //---ctors
    protected:
        connection_int(peer_int &peer,
                       ConnectionType con_type,
                       unsigned int connid = 0);

        virtual ~connection_int();

    public:
        virtual blaze::collector &get_collector();

        //-----------------------------
        // INIT
        //-----------------------------
    private:
        blaze::RetCode    init(unsigned int pkt_sending_q_capcty = 0);

        //-----------------------------
        // CLEAN BEST EFFORT
        //-----------------------------
    private:
        blaze::RetCode  clean_best_effort();
        void            clean_packet_snd_q();

        //-----------------------------
        // CONNECTIVITY
        //-----------------------------
    public:
        blaze::RetCode    server_send_connect_res();
        blaze::RetCode    client_connect(sockaddr_in &params);

        /* this function must be called from same thread that called ClientConnect()*/
        blaze::RetCode    await_for_connection_result(ConnectivityEventResult
                                                      &con_evt_res,
                                                      ConnectivityEventType &connectivity_evt_type,
                                                      time_t sec = -1,
                                                      long nsec = 0);

    public:
        blaze::RetCode    disconnect(DisconnectionResultReason disres);

        /* this function must be called from same thread that called Disconnect()*/
        blaze::RetCode    await_for_disconnection_result(ConnectivityEventResult
                                                         &con_evt_res,
                                                         ConnectivityEventType &connectivity_evt_type,
                                                         time_t sec = -1,
                                                         long nsec = 0);

    private:
        blaze::RetCode    notify_for_connectivity_result(ConnectivityEventResult
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
        blaze::RetCode    next_tx_id(tx_id &txid);

        blaze::RetCode    new_transaction(transaction_int **new_transaction,
                                          blz_tx_factory_func blz_tx_factory_f = NULL,
                                          bool compute_txid = true,
                                          void *ud = NULL);

        /*******************************************************************
        CLIENT:
        no effect.

        SERVER:
        it [auto] release-safedestroy related transaction.
        *******************************************************************/
        blaze::RetCode    release_transaction(transaction_int *transaction);

        //-----------------------------
        // SUBSCRIPTION
        //-----------------------------
    public:
        blaze::RetCode    new_subscription(subscription_int **new_subscription,
                                           blz_sbs_factory_func blz_sbs_factory_f = NULL,
                                           void *ud = NULL);


        //client only
        blaze::RetCode    detach_subscription(subscription_int *subscription);

        /*******************************************************************
        CLIENT:
        This function detaches passed subscription from the underlying
        [peer-connection] data-structures.
        Once you successfully called ReleaseSubscription, you can
        safedestroy related subscription.

        SERVER:
        same as client, but it also [auto] safedestroy related subscripion.
        *******************************************************************/
        blaze::RetCode    release_subscription(subscription_int *subscription);

        //-----------------------------
        // GETTERS
        //-----------------------------
    public:
        peer_int                    &peer();
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
        blaze::blocking_queue       &pkt_snd_q();
        blaze::synch_hash_map       &client_fly_tx_map();
        blaze::synch_hash_map       &server_fly_tx_map();
        blaze::synch_hash_map       &class_id_sbs_map();
        blaze::synch_hash_map       &sbsid_sbs_map();
        blaze::synch_hash_map       &reqid_sbs_map();
        blz_tx_factory_func         tx_factory() const;
        void                        *tx_factory_ud() const;
        blz_sbs_factory_func        sbs_factory() const;
        void                        *sbs_factory_ud() const;

        //-----------------------------
        // SETTERS
        //-----------------------------
    private:
        blaze::RetCode  set_connid(unsigned int connid);
        void            set_conn_response(ConnectionResult val);
        void            set_conn_res_code(ConnectionResultReason val);
        void            set_client_agrhbt(unsigned short val);
        void            set_server_agrhbt(unsigned short val);
        void            set_tx_factory(blz_tx_factory_func val);
        void            set_tx_factory_ud(void *ud);
        void            set_sbs_factory(blz_sbs_factory_func val);
        void            set_sbs_factory_ud(void *ud);

        //-----------------------------
        // STATUS SYNCHRO
        //-----------------------------
    public:
        blaze::RetCode    await_for_status_reached_or_outdated(ConnectionStatus
                                                               test,
                                                               ConnectionStatus &current,
                                                               time_t sec = -1,
                                                               long nsec = 0);

        blaze::RetCode    await_for_status_change(ConnectionStatus &status,
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
        blaze::RetCode    set_connection_established();
        blaze::RetCode    set_connection_established(SOCKET socket);
        blaze::RetCode    set_proto_connected();
        blaze::RetCode    set_appl_connected();
        blaze::RetCode    set_disconnecting();
        blaze::RetCode    set_socket_disconnected();

        blaze::RetCode    set_proto_error(blaze::RetCode cause_res =
                                              blaze::RetCode_UNKERR);

        blaze::RetCode    set_socket_error(blaze::RetCode cause_res =
                                               blaze::RetCode_UNKERR);

        blaze::RetCode    set_internal_error(blaze::RetCode cause_res =
                                                 blaze::RetCode_UNKERR);

        blaze::RetCode    set_status(ConnectionStatus status);

        //-----------------------------
        // BLAZE PROTOCOL RCVNG INTERFACE
        //-----------------------------
    private:
        blaze::RetCode recv_connection_request(const blz_hdr_rec *pkt_hdr);
        blaze::RetCode recv_connection_response(const blz_hdr_rec *pkt_hdr);
        blaze::RetCode recv_test_request(const blz_hdr_rec *pkt_hdr);
        blaze::RetCode recv_disconnection(const blz_hdr_rec *pkt_hdr);

        //TX
    private:
        blaze::RetCode recv_tx_req(const blz_hdr_rec *pkt_hdr,
                                   blaze::grow_byte_buffer *pkt_body);

        blaze::RetCode recv_tx_res(const blz_hdr_rec *pkt_hdr,
                                   blaze::grow_byte_buffer *pkt_body);

        //SBS
        blaze::RetCode recv_sbs_start_req(const blz_hdr_rec *pkt_hdr);
        blaze::RetCode recv_sbs_start_res(const blz_hdr_rec *pkt_hdr);

        blaze::RetCode recv_sbs_evt(const blz_hdr_rec *pkt_hdr,
                                    blaze::grow_byte_buffer *pkt_body);

        blaze::RetCode recv_sbs_evt_ack(const blz_hdr_rec *hdr);
        blaze::RetCode recv_sbs_stop_req(const blz_hdr_rec *pkt_hdr);
        blaze::RetCode recv_sbs_stop_res(const blz_hdr_rec *pkt_hdr);

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
        blaze::RetCode            set_socket_blocking_mode(bool blocking);
        blaze::RetCode            establish_connection(sockaddr_in &params);
        blaze::RetCode            socket_shutdown();
        blaze::RetCode            socket_excptn_hndl(long sock_op_res);

        //TCP/IP SENDING
    private:
        blaze::RetCode            send_single_pkt(blaze::grow_byte_buffer *pkt_bbuf);

        //TCP/IP RECEIVING
    private:
        blaze::RetCode            recv_single_pkt(blz_hdr_rec *pkt_hdr,
                                                  blaze::grow_byte_buffer *pkt_body);

        blaze::RetCode            recv_and_decode_hdr(blz_hdr_rec *pkt_hdr);

        blaze::RetCode            recv_body(unsigned int bodylen,
                                            blaze::grow_byte_buffer *pkt_body);

        blaze::RetCode            recv_single_hdr_row(unsigned int *hdr_row);

        //-----------------------------
        // REP
        //-----------------------------
    private:
        peer_int                    &peer_; // associated peer.
        ConnectionType              con_type_;

        //--tcp/ip rep
        SOCKET                      socket_;
        int                         last_socket_err_;
        ConnectivityEventResult     con_evt_res_;
        ConnectivityEventType       connectivity_evt_type_;

    private:
        //---blz proto rep
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
        blaze::blocking_queue   pkt_sending_q_;
        //srv tx repo
        blaze::synch_hash_map   srv_flytx_repo_; //server txid --> BLZ_TRANSACTION
        //cli tx repo
        blaze::synch_hash_map   cli_flytx_repo_; //client txid --> BLZ_TRANSACTION
        //srv sbs repo
        //srv nclass_id --> BLZ_SUBSCRIPTION
        blaze::synch_hash_map   srv_classid_sbs_repo_;
        blaze::synch_hash_map   srv_sbsid_sbs_repo_; //srv sbsid --> BLZ_SUBSCRIPTION
        //cli sbs repo
        blaze::synch_hash_map   cli_reqid_sbs_repo_; //cli reqid --> BLZ_SUBSCRIPTION
        blaze::synch_hash_map   cli_sbsid_sbs_repo_; //cli sbsid --> BLZ_SUBSCRIPTION

        blz_tx_factory_func tx_factory_;  //factory for server tx
        void                *tx_factory_ud_; //factory for server tx ud

        blz_sbs_factory_func sbs_factory_;  //factory for server sbs
        void                 *sbs_factory_ud_; //factory for server sbs ud

    private:
        mutable blaze::synch_monitor mon_;
        unsigned int                prid_;
        unsigned int                reqid_;
        unsigned int                sbsid_;

    protected:
        static blaze::logger    *log_;
};

}

#endif