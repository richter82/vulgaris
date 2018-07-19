/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "glob.h"

namespace vlg {

struct tx_impl {
    explicit tx_impl(incoming_transaction &publ,
                     incoming_connection &conn,
                     incoming_transaction_listener &listener);
    explicit tx_impl(outgoing_transaction &publ,
                     outgoing_transaction_listener &listener);

    virtual ~tx_impl() = default;

    void set_request_obj(const nclass &);
    void set_current_obj(const nclass &);
    void set_result_obj(const nclass &);

    RetCode await_for_status_reached(TransactionStatus test,
                                     TransactionStatus &current,
                                     time_t sec = -1,
                                     long nsec = 0);

    RetCode await_for_closure(time_t sec = -1, long nsec = 0);

    RetCode set_status(TransactionStatus status);
    RetCode set_flying();
    RetCode set_closed();
    RetCode set_aborted();
    void trace_tx_closure(const char *tx_res_str);

    //rep
    conn_impl *conn_;
    tx_id txid_;                            //set by client
    TransactionStatus status_;
    TransactionResult tx_res_;              //set by server
    ProtocolCode result_code_;              //set by server
    TransactionRequestType txtype_;         //set by client
    Action txactn_;                         //set by client
    unsigned int req_nclassid_;             //set by client
    Encode req_clsenc_;                     //set by client
    unsigned int res_nclassid_;             //set by server
    Encode res_clsenc_;                     //set by server
    bool rsclrq_;    //result class required set by client
    bool rescls_;    //result class set by server

    std::unique_ptr<nclass> request_obj_;   //set by client
    std::unique_ptr<nclass> current_obj_;   //set by client
    std::unique_ptr<nclass> result_obj_;    //set by server

    //timing, for stat.
    rt_time_t start_mark_tim_;
    mutable mx mon_;

    incoming_transaction *ipubl_;
    outgoing_transaction *opubl_;
    incoming_transaction_listener *ilistener_;
    outgoing_transaction_listener *olistener_;
};

struct incoming_transaction_impl : public tx_impl {
    explicit incoming_transaction_impl(incoming_transaction &,
                                       std::shared_ptr<incoming_connection> &conn,
                                       incoming_transaction_listener &listener);
    virtual ~incoming_transaction_impl();

    RetCode send_response();

    void set_request_obj_on_request(std::unique_ptr<nclass> &);

    //associated connection.
    std::shared_ptr<incoming_connection> conn_sh_;
};

}

namespace vlg {

struct outgoing_transaction_impl : public tx_impl {
    explicit outgoing_transaction_impl(outgoing_transaction &,
                                       outgoing_transaction_listener &listener);
    virtual ~outgoing_transaction_impl();

    RetCode re_new();
    RetCode send();

    void set_result_obj_on_response(std::unique_ptr<nclass> &);
};

}
