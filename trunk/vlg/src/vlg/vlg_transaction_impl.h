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

#ifndef VLG_TRANSACTION_H_
#define VLG_TRANSACTION_H_
#include "vlg_globint.h"

namespace vlg {

// VLG_TRANSACTION
class transaction_impl : public vlg::collectable {
        friend class peer_impl;
        friend class connection_impl;

        typedef void (*status_change)(transaction_impl &trans,
                                      TransactionStatus status,
                                      void *ud);

        typedef void (*close)(transaction_impl &trans, void *ud);

        //---ctors
    protected:
        transaction_impl(transaction &publ,
                         connection_impl &conn);
        virtual ~transaction_impl();

    public:
        virtual vlg::collector &get_collector();

        // GETTERS
    public:
        peer_impl               &peer();
        connection_impl         &get_connection();
        TransactionResult       tx_res();
        ProtocolCode            tx_result_code();
        TransactionRequestType  tx_req_type();
        Action                  tx_act();
        unsigned int            tx_req_class_id();
        Encode                  tx_req_class_encode();
        unsigned int            tx_res_class_id();
        Encode                  tx_res_class_encode();
        bool                    is_result_class_req();
        bool                    is_result_class_set();
        const nclass            *request_obj();
        const nclass            *current_obj();
        const nclass            *result_obj();

        // SETTERS
    public:
        void    set_tx_res(TransactionResult val);
        void    set_tx_result_code(ProtocolCode val);
        void    set_tx_req_type(TransactionRequestType val);
        void    set_tx_act(Action val);
        void    set_tx_req_class_id(unsigned int val);
        void    set_tx_req_class_encode(Encode val);
        void    set_tx_res_class_id(unsigned int val);
        void    set_tx_res_class_encode(Encode val);
        void    set_result_class_req(bool val);
        void    set_result_class_set(bool val);
        void    set_request_obj(const nclass *val);
        void    set_current_obj(const nclass *val);
        void    set_result_obj(const nclass *val);

    private:
        void    set_request_obj_on_request(nclass *val);
        void    set_result_obj_on_response(nclass *val);

        // INIT
    private:
        RetCode  init();

    public:
        RetCode  re_new();

        // STATUS SYNCHRO
    public:
        RetCode await_for_status_reached_or_outdated(TransactionStatus test,
                                                     TransactionStatus &current,
                                                     time_t sec = -1,
                                                     long nsec = 0);

        RetCode await_for_closure(time_t sec = -1, long nsec = 0);

        // STATUS ASYNCHRO HNDLRS
    public:
        void set_status_change_handler(status_change hndlr,
                                       void *ud);

        // TX RES ASYNCHRO HNDLRS
    public:
        void set_close_handler(close hndlr, void *ud);

        // TRANSACTION ID
    public:
        tx_id           &txid();
        tx_id           *tx_id_ptr();
        void            set_tx_id(tx_id &txid);
        unsigned int    tx_id_PLID();
        unsigned int    tx_id_SVID();
        unsigned int    tx_id_CNID();
        unsigned int    tx_id_PRID();
        void            set_tx_id_PLID(unsigned int val);
        void            set_tx_id_SVID(unsigned int val);
        void            set_tx_id_CNID(unsigned int val);
        void            set_tx_id_PRID(unsigned int val);

        // TX PREPARE
    public:
        RetCode        prepare();
        RetCode        prepare(TransactionRequestType txtype,
                               Action txactn,
                               Encode clsenc,
                               bool rsclrq,
                               const nclass *request_obj = nullptr,
                               const nclass *current_obj = nullptr);

        // TX SEND
    public:
        RetCode        send();             //called by client peer
        RetCode        send_response();    //called by server peer

        // STATUS
    public:
        TransactionStatus  status();

    private:
        RetCode set_status(TransactionStatus status);
        RetCode set_flying();
        RetCode set_closed();
        RetCode set_aborted();
        void    trace_tx_closure(const char *tx_res_str);

        // APPLICATIVE HANDLERS
    public:
        virtual void on_request();
        virtual void on_close();

    private:
        RetCode  objs_release();

        // REP
    private:
        peer_impl                &peer_; // associated peer.
        connection_impl          &conn_; // underlying connection.
        const nentity_manager    &nem_;

        tx_id                   txid_;  //(set by client)
        TransactionStatus       status_;
        TransactionResult       tx_res_;    //(set by server)
        ProtocolCode            result_code_;   //(set by server)

        TransactionRequestType  txtype_;    //(set by client)
        Action                  txactn_;    //(set by client)
        unsigned int            req_classid_;   //(set by client)
        Encode                  req_clsenc_;    //(set by client)
        unsigned int            res_classid_;   //(set by server)
        Encode                  res_clsenc_;    //(set by server)
        bool                    rsclrq_;    //result class required (set by client)
        bool                    rescls_;    //result class          (set by server)

        nclass     *request_obj_;  //(set by client)
        nclass     *current_obj_;  //(set by client)
        nclass     *result_obj_;   //(set by server)

        //--synch status
        status_change   tsc_hndl_;
        void            *tsc_hndl_ud_;

        //resp hndl.
        close   tres_hndl_;
        void    *tres_hndl_ud_;

        //timing, for stat.
    private:
        rt_time_t start_mark_tim_;

    private:
        mutable vlg::synch_monitor mon_;

    private:
        transaction &publ_;

    protected:
        static nclass_logger *log_;
};

}

#endif
