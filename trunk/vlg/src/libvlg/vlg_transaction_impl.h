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

#ifndef BLZ_TRANSACTION_H_
#define BLZ_TRANSACTION_H_
#include "blz_glob_int.h"

namespace vlg {

#define TX_RES_COMMT    "COMMITTED"
#define TX_RES_FAIL     "FAILED"
#define TX_RES_ABORTED  "ABORTED"
#define TX_NO_OBJ       "NO-OBJ"

//-----------------------------
// BLZ_TRANSACTION
//-----------------------------

class transaction_int : public vlg::collectable {
        friend class peer_int;
        friend class connection_int;

        typedef void (*transaction_status_change_hndlr)(transaction_int &trans,
                                                        TransactionStatus status,
                                                        void *ud);

        typedef void (*transaction_closure_hndlr)(transaction_int &trans, void *ud);

        //---ctors
    protected:
        transaction_int(connection_int &conn);
        virtual ~transaction_int();

    public:
        virtual vlg::collector &get_collector();

        //-----------------------------
        // GETTERS
        //-----------------------------
    public:
        peer_int                    &peer();
        connection_int              &get_connection();
        TransactionResult           tx_res();
        ProtocolCode                tx_result_code();
        TransactionRequestType      tx_req_type();
        Action                      tx_act();
        unsigned int                tx_req_class_id();
        Encode                      tx_req_class_encode();
        unsigned int                tx_res_class_id();
        Encode                      tx_res_class_encode();
        bool                        is_result_class_req();
        bool                        is_result_class_set();
        const nclass             *request_obj();
        const nclass             *current_obj();
        const nclass             *result_obj();

        //-----------------------------
        // SETTERS
        //-----------------------------
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

        //-----------------------------
        // INIT
        //-----------------------------
    private:
        vlg::RetCode  init();

    public:
        vlg::RetCode  re_new();

        //-----------------------------
        // STATUS SYNCHRO
        //-----------------------------
    public:
        vlg::RetCode await_for_status_reached_or_outdated(TransactionStatus test,
                                                            TransactionStatus &current,
                                                            time_t sec = -1,
                                                            long nsec = 0);

        vlg::RetCode await_for_closure(time_t sec = -1, long nsec = 0);

        //-----------------------------
        // STATUS ASYNCHRO HNDLRS
        //-----------------------------
    public:
        void
        set_transaction_status_change_handler(transaction_status_change_hndlr hndlr,
                                              void *ud);

        //-----------------------------
        // TX RES ASYNCHRO HNDLRS
        //-----------------------------
    public:
        void set_transaction_closure_handler(transaction_closure_hndlr hndlr, void *ud);

        //-----------------------------
        // TRANSACTION ID
        //-----------------------------
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

        //-----------------------------
        // TX PREPARE
        //-----------------------------
    public:
        vlg::RetCode        prepare();

        vlg::RetCode        prepare(TransactionRequestType    txtype,
                                      Action  txactn,
                                      Encode  clsenc,
                                      bool    rsclrq,
                                      const nclass *request_obj = NULL,
                                      const nclass *current_obj = NULL);

        //-----------------------------
        // TX SEND
        //-----------------------------
    public:
        vlg::RetCode        send();             //called by client peer
        vlg::RetCode        send_response();    //called by server peer


        //-----------------------------
        // STATUS
        //-----------------------------
    public:
        TransactionStatus  status();


    private:
        vlg::RetCode  set_status(TransactionStatus status);
        vlg::RetCode  set_flying();
        vlg::RetCode  set_closed();
        vlg::RetCode  set_aborted();
        void            trace_tx_closure(const char *tx_res_str);

        //-----------------------------
        // APPLICATIVE HANDLERS
        //-----------------------------
    public:
        virtual void on_request();
        virtual void on_close();

    private:
        vlg::RetCode  objs_release();

        //-----------------------------
        // REP
        //-----------------------------
    private:

        peer_int                &peer_; // associated peer.
        connection_int          &conn_; // underlying connection.
        const entity_manager    &bem_;

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
        transaction_status_change_hndlr tsc_hndl_;
        void                            *tsc_hndl_ud_;

        //resp hndl.
        transaction_closure_hndlr   tres_hndl_;
        void                        *tres_hndl_ud_;

        //timing, for stat.
    private:
        rt_time_t start_mark_tim_;

    private:
        mutable vlg::synch_monitor mon_;

    protected:
        static nclass_logger *log_;
};

}

#endif
