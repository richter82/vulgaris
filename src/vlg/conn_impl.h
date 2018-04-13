/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "glob.h"

namespace vlg {

struct conn_impl {
    conn_impl(incoming_connection &ipubl, peer &p);
    conn_impl(outgoing_connection &opubl);

    RetCode set_connection_established();
    RetCode set_connection_established(SOCKET socket);
    RetCode set_proto_connected();
    RetCode set_appl_connected();
    RetCode set_disconnecting();
    RetCode set_socket_disconnected();
    RetCode set_proto_error(RetCode cause_res = RetCode_UNKERR);
    RetCode set_socket_error(RetCode cause_res = RetCode_UNKERR);
    RetCode set_internal_error(RetCode cause_res = RetCode_UNKERR);
    RetCode set_status(ConnectionStatus status);

    RetCode await_for_status_reached(ConnectionStatus
                                     test,
                                     ConnectionStatus &current,
                                     time_t sec = -1,
                                     long nsec = 0);

    RetCode await_for_status_change(ConnectionStatus &status,
                                    time_t sec = -1,
                                    long nsec = 0);

    RetCode establish_connection(sockaddr_in &params);

    RetCode disconnect(ProtocolCode disres);

    /* this function must be called from same thread that called Disconnect()*/
    RetCode await_for_disconnection_result(ConnectivityEventResult &con_evt_res,
                                           ConnectivityEventType &connectivity_evt_type,
                                           time_t sec = -1,
                                           long nsec = 0);

    RetCode notify_for_connectivity_result(ConnectivityEventResult con_evt_res,
                                           ConnectivityEventType connectivity_evt_type);

    RetCode notify_disconnection(ConnectivityEventResult con_evt_res,
                                 ConnectivityEventType connectivity_evt_type);


    RetCode set_socket_blocking_mode(bool blocking);

    RetCode socket_shutdown();

    RetCode send_single_pkt(g_bbuf *pkt_bbuf);

    RetCode recv_single_pkt(vlg_hdr_rec *pkt_hdr,
                            g_bbuf *pkt_body);

    RetCode recv_and_decode_hdr(vlg_hdr_rec *pkt_hdr);

    RetCode recv_body(unsigned int bodylen,
                      g_bbuf *pkt_body);

    RetCode recv_single_hdr_row(unsigned int *hdr_row);

    RetCode socket_excptn_hndl(long sock_op_res);

    const char *get_host_ip() const;
    unsigned short get_host_port() const;

    //rep
    ConnectionType con_type_;

    SOCKET socket_;
    int last_socket_err_;
    ConnectivityEventResult con_evt_res_;
    ConnectivityEventType connectivity_evt_type_;

    unsigned int connid_;
    ConnectionStatus status_;
    ConnectionResult conres_;
    ProtocolCode conrescode_;
    unsigned short cli_agrhbt_;
    unsigned short srv_agrhbt_;
    ProtocolCode disconrescode_;

    //--synch status
    bool connect_evt_occur_;

    //---packet sending queue
    b_qu pkt_sending_q_;

    mutable mx mon_;

    incoming_connection *ipubl_;
    outgoing_connection *opubl_;

    //associated peer.
    //cannot be ref, because bind is after construction.
    peer_impl *peer_;
};

struct incoming_connection_impl : public conn_impl {
    explicit incoming_connection_impl(incoming_connection &publ, peer &p);
    virtual ~incoming_connection_impl();

    RetCode server_send_connect_res(std::shared_ptr<incoming_connection> &inco_conn);

    RetCode new_incoming_transaction(std::shared_ptr<incoming_transaction> &incoming_transaction,
                                     std::shared_ptr<incoming_connection> &inco_conn);

    RetCode new_incoming_subscription(std::shared_ptr<incoming_subscription> &incoming_sbs,
                                      std::shared_ptr<incoming_connection> &inco_conn);

    RetCode release_subscription(incoming_subscription_impl *subscription);

    RetCode recv_connection_request(const vlg_hdr_rec *pkt_hdr,
                                    std::shared_ptr<incoming_connection> &inco_conn);

    RetCode recv_test_request(const vlg_hdr_rec *pkt_hdr);
    RetCode recv_disconnection(const vlg_hdr_rec *pkt_hdr);

    RetCode recv_tx_request(const vlg_hdr_rec *pkt_hdr,
                            g_bbuf *pkt_body,
                            std::shared_ptr<incoming_connection> &inco_conn);

    RetCode recv_sbs_start_request(const vlg_hdr_rec *pkt_hdr,
                                   std::shared_ptr<incoming_connection> &inco_conn);

    RetCode recv_sbs_evt_ack(const vlg_hdr_rec *hdr);
    RetCode recv_sbs_stop_request(const vlg_hdr_rec *pkt_hdr);

    unsigned int next_sbsid();

    //rep
    s_hm inco_flytx_map_;

    //srv subscription repo
    s_hm inco_nclassid_sbs_map_;
    s_hm inco_sbsid_sbs_map_;

    unsigned int sbsid_;

    //factory for incoming transactions
    //factories cannot be references
    incoming_transaction_factory *tx_factory_publ_;
    //factory for incoming subscriptions
    //factories cannot be references
    incoming_subscription_factory *sbs_factory_publ_;
};

}

namespace vlg {

struct outgoing_connection_impl : public conn_impl {
    explicit outgoing_connection_impl(outgoing_connection &publ);
    virtual ~outgoing_connection_impl();

    RetCode client_connect(sockaddr_in &params);

    RetCode await_for_connection_result(ConnectivityEventResult &con_evt_res,
                                        ConnectivityEventType &connectivity_evt_type,
                                        time_t sec = -1,
                                        long nsec = 0);

    RetCode notify_connection(ConnectivityEventResult con_evt_res,
                              ConnectivityEventType connectivity_evt_type);

    RetCode next_tx_id(tx_id &txid);
    RetCode detach_subscription(outgoing_subscription_impl *subscription);
    RetCode release_subscription(outgoing_subscription_impl *subscription);

    RetCode recv_connection_response(const vlg_hdr_rec *pkt_hdr);
    RetCode recv_test_request(const vlg_hdr_rec *pkt_hdr);
    RetCode recv_disconnection(const vlg_hdr_rec *pkt_hdr);

    RetCode recv_tx_response(const vlg_hdr_rec *pkt_hdr,
                             g_bbuf *pkt_body);

    RetCode recv_sbs_start_response(const vlg_hdr_rec *pkt_hdr);

    RetCode recv_sbs_evt(const vlg_hdr_rec *pkt_hdr,
                         g_bbuf *pkt_body);

    RetCode recv_sbs_stop_response(const vlg_hdr_rec *pkt_hdr);

    unsigned int next_prid();
    unsigned int next_reqid();

    //rep
    s_hm outg_flytx_map_;

    //cli subscription repo
    s_hm outg_reqid_sbs_map_;
    s_hm outg_sbsid_sbs_map_;

    unsigned int prid_;
    unsigned int reqid_;
};

}
