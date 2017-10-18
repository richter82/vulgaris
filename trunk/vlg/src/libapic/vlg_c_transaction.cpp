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

#include "blaze_c_transaction.h"
#include "blaze_transaction.h"

namespace vlg {
class c_transaction : public transaction {
    private:
        static void transaction_status_change_c_transaction(transaction &tx,
                                                            TransactionStatus status, void *ud) {
            c_transaction &self = static_cast<c_transaction &>(tx);
            self.tsc_wr_((transaction_wr)&tx, status, self.tsc_ud_);
        }

        static void transaction_closure_c_transaction(transaction &tx, void *ud) {
            c_transaction &self = static_cast<c_transaction &>(tx);
            self.tc_wr_((transaction_wr)&tx, self.tc_ud_);
        }

    public:
        c_transaction() : tc_wr_(NULL), tsc_wr_(NULL), tsc_ud_(NULL), tc_ud_(NULL) {}

        transaction_closure_wr Tc_wr() const {
            return tc_wr_;
        }
        void Tc_wr(transaction_closure_wr val) {
            tc_wr_ = val;
        }
        transaction_status_change_wr Tsc_wr() const {
            return tsc_wr_;
        }
        void Tsc_wr(transaction_status_change_wr val) {
            tsc_wr_ = val;
        }
        void *Tsc_ud() const {
            return tsc_ud_;
        }
        void Tsc_ud(void *val) {
            tsc_ud_ = val;
            set_transaction_status_change_handler(transaction_status_change_c_transaction,
                                                  tsc_ud_);
        }
        void *Tc_ud() const {
            return tc_ud_;
        }
        void Tc_ud(void *val) {
            tc_ud_ = val;
            set_transaction_closure_handler(transaction_closure_c_transaction, tc_ud_);
        }
    private:
        transaction_closure_wr tc_wr_;
        transaction_status_change_wr tsc_wr_;
        void *tsc_ud_;
        void *tc_ud_;
};

extern "C" {

    transaction_wr transaction_create()
    {
        return new c_transaction();
    }

    void transaction_destroy(transaction_wr tx)
    {
        delete static_cast<c_transaction *>(tx);
    }

    RetCode transaction_bind(transaction_wr tx, connection_wr conn)
    {
        return static_cast<transaction *>(tx)->bind(*(connection *)conn);
    }

    connection_wr transaction_get_connection(transaction_wr tx)
    {
        return (connection_wr)static_cast<transaction *>(tx)->get_connection();
    }

    TransactionResult transaction_get_transaction_result(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_result();
    }

    ProtocolCode transaction_get_transaction_result_code(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_result_code();
    }

    TransactionRequestType transaction_get_transaction_request_type(
        transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_request_type();
    }

    Action transaction_get_transaction_action(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_action();
    }

    unsigned int transaction_get_transaction_request_class_id(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_request_class_id();
    }

    Encode transaction_get_transaction_request_class_encode(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_request_class_encode();
    }

    unsigned int transaction_get_transaction_result_class_id(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_result_class_id();
    }

    Encode transaction_get_transaction_result_class_encode(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_result_class_encode();
    }

    int transaction_is_transaction_result_class_required(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->is_transaction_result_class_required();
    }

    int transaction_is_transaction_result_class_set(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->is_transaction_result_class_set();
    }

    net_class_wr transaction_get_request_obj(transaction_wr tx)
    {
        return (net_class_wr)static_cast<transaction *>(tx)->get_request_obj();
    }

    net_class_wr transaction_get_current_obj(transaction_wr tx)
    {
        return (net_class_wr)static_cast<transaction *>(tx)->get_current_obj();
    }

    net_class_wr transaction_get_result_obj(transaction_wr tx)
    {
        return (net_class_wr)static_cast<transaction *>(tx)->get_result_obj();
    }

    void transaction_set_transaction_result(transaction_wr tx,
                                            TransactionResult tx_res)
    {
        static_cast<transaction *>(tx)->set_transaction_result(tx_res);
    }

    void transaction_set_transaction_result_code(transaction_wr tx,
                                                 ProtocolCode tx_res_code)
    {
        static_cast<transaction *>(tx)->set_transaction_result_code(tx_res_code);
    }

    void transaction_set_transaction_request_type(transaction_wr tx,
                                                  TransactionRequestType tx_req_type)
    {
        static_cast<transaction *>(tx)->set_transaction_request_type(tx_req_type);
    }

    void transaction_set_transaction_action(transaction_wr tx, Action tx_act)
    {
        static_cast<transaction *>(tx)->set_transaction_action(tx_act);
    }

    void transaction_set_transaction_request_class_id(transaction_wr tx,
                                                      unsigned int nclass_id)
    {
        static_cast<transaction *>(tx)->set_transaction_request_class_id(nclass_id);
    }

    void transaction_set_transaction_request_class_encode(transaction_wr tx,
                                                          Encode class_encode)
    {
        static_cast<transaction *>(tx)->set_transaction_request_class_encode(
            class_encode);
    }

    void transaction_set_transaction_result_class_id(transaction_wr tx,
                                                     unsigned int nclass_id)
    {
        static_cast<transaction *>(tx)->set_transaction_result_class_id(nclass_id);
    }

    void transaction_set_transaction_result_class_encode(transaction_wr tx,
                                                         Encode class_encode)
    {
        static_cast<transaction *>(tx)->set_transaction_result_class_encode(
            class_encode);
    }

    void transaction_set_transaction_result_class_required(transaction_wr tx,
                                                           int res_class_req)
    {
        static_cast<transaction *>(tx)->set_transaction_result_class_required(
            res_class_req ? true : false);
    }

    void transaction_set_request_obj(transaction_wr tx, net_class_wr obj)
    {
        static_cast<transaction *>(tx)->set_request_obj((nclass *)obj);
    }

    void transaction_set_current_obj(transaction_wr tx, net_class_wr obj)
    {
        static_cast<transaction *>(tx)->set_current_obj((nclass *)obj);
    }

    void transaction_set_result_obj(transaction_wr tx, net_class_wr obj)
    {
        static_cast<transaction *>(tx)->set_result_obj((nclass *)obj);
    }

    TransactionStatus transaction_get_status(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_status();
    }

    RetCode transaction_await_for_status_reached_or_outdated(transaction_wr tx,
                                                             TransactionStatus test,
                                                             TransactionStatus *current,
                                                             time_t sec,
                                                             long nsec)
    {
        return static_cast<transaction *>(tx)->await_for_status_reached_or_outdated(
                   test, *current, sec, nsec);
    }

    RetCode transaction_await_for_closure(transaction_wr tx, time_t sec,
                                          long nsec)
    {
        return static_cast<transaction *>(tx)->await_for_closure(sec, nsec);
    }

    void transaction_set_transaction_status_change_handler(transaction_wr tx,
                                                           transaction_status_change_wr handler,
                                                           void *ud)
    {
        static_cast<c_transaction *>(tx)->Tsc_wr(handler);
        static_cast<c_transaction *>(tx)->Tsc_ud(ud);
    }

    void transaction_set_transaction_closure_handler(transaction_wr tx,
                                                     transaction_closure_wr handler, void *ud)
    {
        static_cast<c_transaction *>(tx)->Tc_wr(handler);
        static_cast<c_transaction *>(tx)->Tc_ud(ud);
    }

    tx_id_wr *transaction_get_transaction_id(transaction_wr tx)
    {
        tx_id &txid = static_cast<transaction *>(tx)->get_transaction_id();
        return (tx_id_wr *)&txid;
    }

    void transaction_set_transaction_id(transaction_wr tx, tx_id_wr *txid)
    {
        static_cast<transaction *>(tx)->set_transaction_id((tx_id &)*txid);
    }

    unsigned int transaction_get_transaction_id_PLID(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_id_PLID();
    }

    unsigned int transaction_get_transaction_id_SVID(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_id_SVID();
    }

    unsigned int transaction_get_transaction_id_CNID(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_id_CNID();
    }

    unsigned int transaction_get_transaction_id_PRID(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->get_transaction_id_PRID();
    }

    void transaction_set_transaction_id_PLID(transaction_wr tx, unsigned int plid)
    {
        static_cast<transaction *>(tx)->set_transaction_id_PLID(plid);
    }

    void transaction_set_transaction_id_SVID(transaction_wr tx, unsigned int svid)
    {
        static_cast<transaction *>(tx)->set_transaction_id_SVID(svid);
    }

    void transaction_set_transaction_id_CNID(transaction_wr tx, unsigned int cnid)
    {
        static_cast<transaction *>(tx)->set_transaction_id_CNID(cnid);
    }

    void transaction_set_transaction_id_PRID(transaction_wr tx, unsigned int prid)
    {
        static_cast<transaction *>(tx)->set_transaction_id_PRID(prid);
    }

    RetCode transaction_renew(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->renew();
    }

    RetCode transaction_prepare(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->prepare();
    }

    RetCode transaction_prepare_full(transaction_wr tx,
                                     TransactionRequestType tx_request_type,
                                     Action tx_action,
                                     net_class_wr sending_obj,
                                     net_class_wr current_obj)
    {
        return static_cast<transaction *>(tx)->prepare(tx_request_type,
                                                       tx_action,
                                                       (nclass *)sending_obj,
                                                       (nclass *)current_obj);
    }

    RetCode transaction_send(transaction_wr tx)
    {
        return static_cast<transaction *>(tx)->send();
    }

}
}
