/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "glob.h"

namespace vlg {

struct conn_pkt {
    conn_pkt(std::unique_ptr<char> *key,
             g_bbuf &&pkt_b,
             unsigned int ts_0 = 0,
             unsigned int ts_1 = 0) :
        key_(key ? std::move(*key) : nullptr),
        pkt_b_(pkt_b),
        ts0_(ts_0),
        ts1_(ts_1) {}

    std::unique_ptr<char> key_;
    g_bbuf pkt_b_;
    unsigned int ts0_;
    unsigned int ts1_;
};

struct conn_pkt_unqptr_obj_mng : public std_unique_ptr_obj_mng<conn_pkt> {

    static int cpkt_cmp_obj(const void *obj1, const void *obj2, size_t len) {
        std::unique_ptr<conn_pkt> &cpkt_1 = *(std::unique_ptr<conn_pkt> *)(obj1);
        std::unique_ptr<conn_pkt> &cpkt_2 = *(std::unique_ptr<conn_pkt> *)(obj2);
        if(cpkt_1 && cpkt_2) {
            return strcmp(cpkt_1.get()->key_.get(), cpkt_2.get()->key_.get());
        } else if(cpkt_1) {
            return 1;
        } else if(cpkt_2) {
            return -1;
        } else {
            return 0;
        }
    }

    static void cpkt_hash_obj(const void *key, int len, uint32_t seed, void *out) {
        std::unique_ptr<conn_pkt> &cpkt = *(std::unique_ptr<conn_pkt> *)(key);
        MurmurHash3_x86_32(cpkt.get()->key_.get(), (int)strlen(cpkt.get()->key_.get()), seed, out);
    }

    static void *cpkt_rplc_on_hit_func(void *hit, const void *rplcr, size_t type_size) {
        std::unique_ptr<conn_pkt> &h_cpkt = *(std::unique_ptr<conn_pkt> *)(hit);
        std::unique_ptr<conn_pkt> &r_cpkt = *(std::unique_ptr<conn_pkt> *)(rplcr);
        if((h_cpkt->ts0_ < r_cpkt->ts0_) || ((h_cpkt->ts0_ == r_cpkt->ts0_) && (h_cpkt->ts1_ < r_cpkt->ts1_))) {
            h_cpkt = std::move(r_cpkt);
        }
        return hit;
    }

    explicit conn_pkt_unqptr_obj_mng() : std_unique_ptr_obj_mng<conn_pkt>(0,
                                                                              unique_ptr_alloc_func,
                                                                              unique_ptr_dealloc_func,
                                                                              cpkt_cmp_obj,
                                                                              unique_ptr_cpy_func,
                                                                              cpkt_hash_obj,
                                                                              cpkt_rplc_on_hit_func) {}
};

const conn_pkt_unqptr_obj_mng conn_pkt_up_omng;

}

namespace vlg {

enum PktChasingStatus {
    PktChasingStatus_HDRLen,
    PktChasingStatus_HDR,
    PktChasingStatus_Body
};

struct conn_impl {
    conn_impl(incoming_connection &ipubl, peer &p, incoming_connection_listener &);
    conn_impl(outgoing_connection &opubl, outgoing_connection_listener &);

    virtual ~conn_impl() = default;
    virtual void release_all_children() = 0;

    RetCode set_connection_established();
    RetCode set_connection_established(SOCKET socket);
    RetCode set_proto_connected();
    RetCode set_appl_connected();
    RetCode set_disconnecting();
    RetCode set_socket_disconnected();
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

    RetCode aggr_msgs_and_send_pkt();
    RetCode send_acc_buff();

    RetCode recv_bytes();
    RetCode chase_pkt();
    RetCode read_decode_hdr();
    void clean_rdn_rep();

    RetCode sckt_hndl_err(long sock_op_res);

    const char *get_host_ip() const;
    unsigned short get_host_port() const;

    RetCode close_connection(ConnectivityEventResult cer,
                             ConnectivityEventType cet);

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
    bool connect_evt_occur_;

    //reading rep
    PktChasingStatus pkt_ch_st_;
    g_bbuf rdn_buff_;
    vlg_hdr_rec curr_rdn_hdr_;
    std::unique_ptr<g_bbuf> curr_rdn_body_;

    //sending rep
    //packet sending queue
    b_qu_hm pkt_sending_q_;
    //current sending packet
    std::unique_ptr<conn_pkt> cpkt_;
    //accumulating sending buffer
    g_bbuf acc_snd_buff_;

    mutable std::mutex mtx_;
    mutable std::condition_variable cv_;

    incoming_connection *ipubl_;
    outgoing_connection *opubl_;
    incoming_connection_listener *ilistener_;
    outgoing_connection_listener *olistener_;

    //associated peer.
    //cannot be ref, because bind is after construction.
    peer_impl *peer_;
};

struct incoming_connection_impl : public conn_impl {
    explicit incoming_connection_impl(incoming_connection &publ,
                                      peer &p,
                                      incoming_connection_listener &);

    virtual ~incoming_connection_impl();

    virtual void release_all_children() override;

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

    RetCode recv_sbs_stop_request(const vlg_hdr_rec *pkt_hdr,
                                  std::shared_ptr<incoming_connection> &inco_conn);

    unsigned int next_sbsid() {
        std::unique_lock<std::mutex> lck(mtx_);
        return ++sbsid_;
    }

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
    explicit outgoing_connection_impl(outgoing_connection &publ, outgoing_connection_listener &);
    virtual ~outgoing_connection_impl();

    virtual void release_all_children() override;

    RetCode client_connect(sockaddr_in &params);

    RetCode await_for_connection_result(ConnectivityEventResult &con_evt_res,
                                        ConnectivityEventType &connectivity_evt_type,
                                        time_t sec = -1,
                                        long nsec = 0);

    RetCode notify_connection(ConnectivityEventResult con_evt_res,
                              ConnectivityEventType connectivity_evt_type);

    RetCode next_tx_id(tx_id &txid);
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

    unsigned int next_prid() {
        std::unique_lock<std::mutex> lck(mtx_);
        return ++prid_;
    }

    unsigned int next_reqid() {
        std::unique_lock<std::mutex> lck(mtx_);
        return ++reqid_;
    }

    //rep
    s_hm outg_flytx_map_;

    //cli subscription repo
    s_hm outg_reqid_sbs_map_;
    s_hm outg_sbsid_sbs_map_;

    unsigned int prid_;
    unsigned int reqid_;
};

}
