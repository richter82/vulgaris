/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "vlg.h"

namespace vlg {

/** @brief transaction_factory.
*/
struct incoming_transaction_factory {
    explicit incoming_transaction_factory();
    virtual ~incoming_transaction_factory();
    virtual incoming_transaction &make_incoming_transaction(std::shared_ptr<incoming_connection> &);

    static incoming_transaction_factory &default_factory();
};

/** @brief incoming transaction listener.
*/
struct incoming_transaction_listener {
    virtual void on_status_change(incoming_transaction &, TransactionStatus) = 0;
    virtual void on_request(incoming_transaction &) = 0;
    virtual void on_close(incoming_transaction &) = 0;
    virtual void on_releaseable(incoming_transaction &) = 0;

    static incoming_transaction_listener &default_listener();
};

/** @brief incoming_transaction.
*/
struct incoming_transaction {
    explicit incoming_transaction(std::shared_ptr<incoming_connection> &,
                                  incoming_transaction_listener &listener =
                                      incoming_transaction_listener::default_listener());

    virtual ~incoming_transaction();

    incoming_connection &get_connection();

    TransactionResult get_close_result();
    ProtocolCode get_close_result_code();

    void set_result(TransactionResult);
    void set_result_code(ProtocolCode);

    TransactionRequestType get_request_type();
    Action get_request_action();
    unsigned int get_request_nclass_id();
    Encode get_request_nclass_encode();

    unsigned int get_result_nclass_id();
    Encode get_result_nclass_encode();

    bool is_result_obj_required();
    bool is_result_obj_set();

    const nclass *get_request_obj();
    const nclass *get_current_obj();
    const nclass *get_result_obj();

    void set_result_nclass_id(unsigned int);
    void set_result_nclass_encode(Encode);

    void set_current_obj(const nclass &);
    void set_result_obj(const nclass &);

    TransactionStatus get_status();

    RetCode await_for_status_reached(TransactionStatus test,
                                     TransactionStatus &current,
                                     time_t sec = -1,
                                     long nsec = 0);

    RetCode await_for_close(time_t sec = -1,
                            long nsec = 0);
    tx_id &get_id();
    void set_id(tx_id &);

    std::unique_ptr<incoming_transaction_impl> impl_;
};

/** @brief outgoing transaction listener.
*/
struct outgoing_transaction_listener {
    virtual void on_status_change(outgoing_transaction &, TransactionStatus) = 0;
    virtual void on_close(outgoing_transaction &) = 0;

    static outgoing_transaction_listener &default_listener();
};

/** @brief outgoing_transaction.
*/
struct outgoing_transaction {
    explicit outgoing_transaction(outgoing_transaction_listener &listener =
                                      outgoing_transaction_listener::default_listener());

    virtual ~outgoing_transaction();

    RetCode bind(outgoing_connection &);

    outgoing_connection &get_connection();

    TransactionResult get_close_result();
    ProtocolCode get_close_result_code();

    void set_result(TransactionResult);
    void set_result_code(ProtocolCode);

    TransactionRequestType get_request_type();
    Action get_request_action();
    unsigned int get_request_nclass_id();
    Encode get_request_nclass_encode();

    unsigned int get_result_nclass_id();
    Encode get_result_nclass_encode();

    bool is_result_obj_required();
    bool is_result_obj_set();

    const nclass *get_request_obj();
    const nclass *get_current_obj();
    const nclass *get_result_obj();

    void set_request_type(TransactionRequestType);
    void set_request_action(Action);
    void set_request_nclass_id(unsigned int);
    void set_request_nclass_encode(Encode);
    void set_result_obj_required(bool);

    void set_request_obj(const nclass &);
    void set_current_obj(const nclass &);

    TransactionStatus get_status();

    RetCode await_for_status_reached(TransactionStatus test,
                                     TransactionStatus &current,
                                     time_t sec = -1,
                                     long nsec = 0);

    RetCode await_for_close(time_t sec = -1,
                            long nsec = 0);

    tx_id &get_id();
    void set_id(tx_id &);

    RetCode send();
    RetCode renew();

    std::unique_ptr<outgoing_transaction_impl> impl_;
};

}
