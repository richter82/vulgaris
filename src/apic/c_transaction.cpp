/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#include "vlg_transaction.h"
using namespace vlg;

extern "C" {

    incoming_connection *inco_transaction_get_connection(incoming_transaction *tx)
    {
        return &tx->get_connection();
    }

    TransactionResult inco_transaction_get_transaction_result(incoming_transaction *tx)
    {
        return tx->get_close_result();
    }

    ProtocolCode inco_transaction_get_transaction_result_code(incoming_transaction *tx)
    {
        return tx->get_close_result_code();
    }

    TransactionRequestType inco_transaction_get_transaction_request_type(incoming_transaction *tx)
    {
        return tx->get_request_type();
    }

    Action inco_transaction_get_transaction_action(incoming_transaction *tx)
    {
        return tx->get_request_action();
    }

    unsigned int inco_transaction_get_transaction_request_nclass_id(incoming_transaction *tx)
    {
        return tx->get_request_nclass_id();
    }

    Encode inco_transaction_get_transaction_request_nclass_encode(incoming_transaction *tx)
    {
        return tx->get_request_nclass_encode();
    }

    unsigned int inco_transaction_get_transaction_result_nclass_id(incoming_transaction *tx)
    {
        return tx->get_result_nclass_id();
    }

    Encode inco_transaction_get_transaction_result_nclass_encode(incoming_transaction *tx)
    {
        return tx->get_result_nclass_encode();
    }

    int inco_transaction_is_transaction_result_nclass_required(incoming_transaction *tx)
    {
        return tx->is_result_obj_required();
    }

    int inco_transaction_is_transaction_result_nclass_set(incoming_transaction *tx)
    {
        return tx->is_result_obj_set();
    }

    const nclass *inco_transaction_get_request_obj(incoming_transaction *tx)
    {
        return tx->get_request_obj();
    }

    const nclass *inco_transaction_get_current_obj(incoming_transaction *tx)
    {
        return tx->get_current_obj();
    }

    const nclass *inco_transaction_get_result_obj(incoming_transaction *tx)
    {
        return tx->get_result_obj();
    }

    void inco_transaction_set_transaction_result(incoming_transaction *tx,
                                                 TransactionResult tx_res)
    {
        tx->set_result(tx_res);
    }

    void inco_transaction_set_transaction_result_code(incoming_transaction *tx,
                                                      ProtocolCode tx_res_code)
    {
        tx->set_result_code(tx_res_code);
    }

    void inco_transaction_set_transaction_result_nclass_id(incoming_transaction *tx,
                                                           unsigned int nclass_id)
    {
        tx->set_result_nclass_id(nclass_id);
    }

    void inco_transaction_set_transaction_result_nclass_encode(incoming_transaction *tx,
                                                               Encode nclass_encode)
    {
        tx->set_result_nclass_encode(nclass_encode);
    }

    void inco_transaction_set_current_obj(incoming_transaction *tx, nclass *obj)
    {
        tx->set_current_obj(*obj);
    }

    void inco_transaction_set_result_obj(incoming_transaction *tx, nclass *obj)
    {
        tx->set_result_obj(*obj);
    }

    TransactionStatus inco_transaction_get_status(incoming_transaction *tx)
    {
        return tx->get_status();
    }

    RetCode inco_transaction_await_for_status_reached(incoming_transaction *tx,
                                                      TransactionStatus test,
                                                      TransactionStatus *current,
                                                      time_t sec,
                                                      long nsec)
    {
        return tx->await_for_status_reached(test,
                                            *current,
                                            sec,
                                            nsec);
    }

    RetCode inco_transaction_await_for_closure(incoming_transaction *tx,
                                               time_t sec,
                                               long nsec)
    {
        return tx->await_for_close(sec, nsec);
    }

    tx_id *inco_transaction_get_transaction_id(incoming_transaction *tx)
    {
        return &tx->get_id();
    }

    void inco_transaction_set_transaction_id(incoming_transaction *tx, tx_id *txid)
    {
        tx->set_id(*txid);
    }
}

extern "C" {
    typedef struct own_outgoing_transaction own_outgoing_transaction;

    typedef void(*outg_transaction_on_status_change)(outgoing_transaction *tx,
                                                     TransactionStatus status,
                                                     void *ud,
                                                     void *ud2);

    typedef void(*outg_transaction_on_closure)(outgoing_transaction *tx,
                                               void *ud,
                                               void *ud2);
}

//c_outg_tx

#define OT_STC_UD_IDX 0
#define OT_CLO_UD_IDX 1

struct c_outg_tx : public outgoing_transaction {
    c_outg_tx();

    outg_transaction_on_closure tc_;
    outg_transaction_on_status_change tsc_;

    void *ud_[2];
    void *ud2_[2];
};

struct c_outg_tx_listener : public outgoing_transaction_listener {
    virtual void on_status_change(outgoing_transaction &ot, TransactionStatus status) override {
        if(((c_outg_tx &)ot).tsc_) {
            ((c_outg_tx &)ot).tsc_(&ot, status,
                                   ((c_outg_tx &)ot).ud_[OT_STC_UD_IDX],
                                   ((c_outg_tx &)ot).ud2_[OT_STC_UD_IDX]);
        }
    }
    virtual void on_close(outgoing_transaction &ot) override {
        if(((c_outg_tx &)ot).tc_) {
            ((c_outg_tx &)ot).tc_(&ot,
                                  ((c_outg_tx &)ot).ud_[OT_CLO_UD_IDX],
                                  ((c_outg_tx &)ot).ud2_[OT_CLO_UD_IDX]);
        }
    }
};

static c_outg_tx_listener cotl;

c_outg_tx::c_outg_tx() :
    outgoing_transaction(cotl),
    tc_(nullptr),
    tsc_(nullptr)
{
    memset(ud_, 0, sizeof(ud_));
    memset(ud2_, 0, sizeof(ud2_));
}

extern "C" {
    own_outgoing_transaction *outg_transaction_create()
    {
        return (own_outgoing_transaction *)new c_outg_tx();
    }

    outgoing_transaction *outg_transaction_get_ptr(own_outgoing_transaction *tx)
    {
        return (outgoing_transaction *)tx;
    }

    void outg_transaction_destroy(own_outgoing_transaction *tx)
    {
        delete(c_outg_tx *)(tx);
    }

    RetCode outg_transaction_bind(outgoing_transaction *tx, outgoing_connection *conn)
    {
        return tx->bind(*conn);
    }

    outgoing_connection *outg_transaction_get_connection(outgoing_transaction *tx)
    {
        return &tx->get_connection();
    }

    TransactionResult outg_transaction_get_transaction_result(outgoing_transaction *tx)
    {
        return tx->get_close_result();
    }

    ProtocolCode outg_transaction_get_transaction_result_code(outgoing_transaction *tx)
    {
        return tx->get_close_result_code();
    }

    TransactionRequestType outg_transaction_get_transaction_request_type(outgoing_transaction *tx)
    {
        return tx->get_request_type();
    }

    Action outg_transaction_get_transaction_action(outgoing_transaction *tx)
    {
        return tx->get_request_action();
    }

    unsigned int outg_transaction_get_transaction_request_nclass_id(outgoing_transaction *tx)
    {
        return tx->get_request_nclass_id();
    }

    Encode outg_transaction_get_transaction_request_nclass_encode(outgoing_transaction *tx)
    {
        return tx->get_request_nclass_encode();
    }

    unsigned int outg_transaction_get_transaction_result_nclass_id(outgoing_transaction *tx)
    {
        return tx->get_result_nclass_id();
    }

    Encode outg_transaction_get_transaction_result_nclass_encode(outgoing_transaction *tx)
    {
        return tx->get_result_nclass_encode();
    }

    int outg_transaction_is_transaction_result_nclass_required(outgoing_transaction *tx)
    {
        return tx->is_result_obj_required();
    }

    int outg_transaction_is_transaction_result_nclass_set(outgoing_transaction *tx)
    {
        return tx->is_result_obj_set();
    }

    const nclass *outg_transaction_get_request_obj(outgoing_transaction *tx)
    {
        return tx->get_request_obj();
    }

    const nclass *outg_transaction_get_current_obj(outgoing_transaction *tx)
    {
        return tx->get_current_obj();
    }

    const nclass *outg_transaction_get_result_obj(outgoing_transaction *tx)
    {
        return tx->get_result_obj();
    }

    void outg_transaction_set_transaction_result(outgoing_transaction *tx,
                                                 TransactionResult tx_res)
    {
        tx->set_result(tx_res);
    }

    void outg_transaction_set_transaction_result_code(outgoing_transaction *tx,
                                                      ProtocolCode tx_res_code)
    {
        tx->set_result_code(tx_res_code);
    }

    void outg_transaction_set_transaction_request_type(outgoing_transaction *tx,
                                                       TransactionRequestType tx_req_type)
    {
        tx->set_request_type(tx_req_type);
    }

    void outg_transaction_set_transaction_action(outgoing_transaction *tx, Action tx_act)
    {
        tx->set_request_action(tx_act);
    }

    void outg_transaction_set_transaction_request_nclass_id(outgoing_transaction *tx,
                                                            unsigned int nclass_id)
    {
        tx->set_request_nclass_id(nclass_id);
    }

    void outg_transaction_set_transaction_request_nclass_encode(outgoing_transaction *tx,
                                                                Encode nclass_encode)
    {
        tx->set_request_nclass_encode(nclass_encode);
    }

    void outg_transaction_set_transaction_result_nclass_required(outgoing_transaction *tx,
                                                                 int res_nclass_req)
    {
        tx->set_result_obj_required(res_nclass_req ? true : false);
    }

    void outg_transaction_set_request_obj(outgoing_transaction *tx, nclass *obj)
    {
        tx->set_request_obj(*obj);
    }

    void outg_transaction_set_current_obj(outgoing_transaction *tx, nclass *obj)
    {
        tx->set_current_obj(*obj);
    }

    TransactionStatus outg_transaction_get_status(outgoing_transaction *tx)
    {
        return tx->get_status();
    }

    RetCode outg_transaction_await_for_status_reached(outgoing_transaction *tx,
                                                      TransactionStatus test,
                                                      TransactionStatus *current,
                                                      time_t sec,
                                                      long nsec)
    {
        return tx->await_for_status_reached(test,
                                            *current,
                                            sec,
                                            nsec);
    }

    RetCode outg_transaction_await_for_closure(outgoing_transaction *tx, time_t sec, long nsec)
    {
        return tx->await_for_close(sec, nsec);
    }

    void outg_transaction_set_on_status_change(outgoing_transaction *tx,
                                               outg_transaction_on_status_change hndl,
                                               void *ud, void *ud2)
    {
        static_cast<c_outg_tx *>(tx)->tsc_ = hndl;
        static_cast<c_outg_tx *>(tx)->ud_[OT_STC_UD_IDX] = ud;
        static_cast<c_outg_tx *>(tx)->ud2_[OT_STC_UD_IDX] = ud2;
    }

    void outg_transaction_set_on_closure(outgoing_transaction *tx,
                                         outg_transaction_on_closure hndl,
                                         void *ud, void *ud2)
    {
        static_cast<c_outg_tx *>(tx)->tc_ = hndl;
        static_cast<c_outg_tx *>(tx)->ud_[OT_CLO_UD_IDX] = ud;
        static_cast<c_outg_tx *>(tx)->ud2_[OT_CLO_UD_IDX] = ud2;
    }

    tx_id *outg_transaction_get_transaction_id(outgoing_transaction *tx)
    {
        return &tx->get_id();
    }

    void outg_transaction_set_transaction_id(outgoing_transaction *tx, tx_id *txid)
    {
        tx->set_id(*txid);
    }

    RetCode outg_transaction_renew(outgoing_transaction *tx)
    {
        return tx->renew();
    }

    RetCode outg_transaction_send(outgoing_transaction *tx)
    {
        return tx->send();
    }
}
