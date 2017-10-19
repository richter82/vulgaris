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

#ifndef VLG_CPP_TRANSACTION_H_
#define VLG_CPP_TRANSACTION_H_
#include "vlg_memory.h"

namespace vlg {

/** @brief transaction_factory class.
*/
class transaction_factory {
    public:
        static transaction_int  *tx_factory_int_f(connection_int &conn,
                                                  void *ud);

    public:
        transaction_factory();
        virtual ~transaction_factory();

    public:
        virtual transaction     *new_transaction(connection &conn);

    public:
        static transaction_factory *default_transaction_factory();
};

/** @brief transaction class.
*/
class transaction_impl;
class transaction : public vlg::collectable {
    public:
        typedef void (*transaction_status_change)(transaction &tx,
                                                  TransactionStatus status,
                                                  void *ud);

        typedef void (*transaction_closure)(transaction &tx,
                                            void *ud);

        //---ctors
    public:
        explicit transaction();
        virtual ~transaction();

        virtual vlg::collector &get_collector();

        vlg::RetCode bind(connection &conn);

    public:
        connection                  *get_connection();
        TransactionResult           get_transaction_result();
        ProtocolCode                get_transaction_result_code();
        TransactionRequestType      get_transaction_request_type();
        Action                      get_transaction_action();
        unsigned int                get_transaction_request_class_id();
        Encode                      get_transaction_request_class_encode();
        unsigned int                get_transaction_result_class_id();
        Encode                      get_transaction_result_class_encode();
        bool                        is_transaction_result_class_required();
        bool                        is_transaction_result_class_set();
        const nclass             *get_request_obj();
        const nclass             *get_current_obj();
        const nclass             *get_result_obj();

    public:
        void    set_transaction_result(TransactionResult tx_res);
        void    set_transaction_result_code(ProtocolCode tx_res_code);

        void
        set_transaction_request_type(TransactionRequestType tx_req_type);

        void    set_transaction_action(Action tx_act);
        void    set_transaction_request_class_id(unsigned int nclass_id);
        void    set_transaction_request_class_encode(Encode class_encode);
        void    set_transaction_result_class_id(unsigned int nclass_id);
        void    set_transaction_result_class_encode(Encode class_encode);
        void    set_transaction_result_class_required(bool res_class_req);
        void    set_request_obj(const nclass *obj);
        void    set_current_obj(const nclass *obj);
        void    set_result_obj(const nclass *obj);

    public:
        TransactionStatus get_status();

    public:
        vlg::RetCode
        await_for_status_reached_or_outdated(TransactionStatus  test,
                                             TransactionStatus  &current,
                                             time_t             sec = -1,
                                             long               nsec = 0);

        vlg::RetCode await_for_closure(time_t sec = -1, long nsec = 0);

    public:
        void
        set_transaction_status_change_handler(transaction_status_change handler,
                                              void *ud);

    public:
        void
        set_transaction_closure_handler(transaction_closure handler, void *ud);

    public:
        tx_id           &get_transaction_id();
        void            set_transaction_id(tx_id &txid);
        unsigned int    get_transaction_id_PLID();
        unsigned int    get_transaction_id_SVID();
        unsigned int    get_transaction_id_CNID();
        unsigned int    get_transaction_id_PRID();
        void            set_transaction_id_PLID(unsigned int plid);
        void            set_transaction_id_SVID(unsigned int svid);
        void            set_transaction_id_CNID(unsigned int cnid);
        void            set_transaction_id_PRID(unsigned int prid);

    public:
        vlg::RetCode  renew();

        vlg::RetCode  prepare();

        vlg::RetCode  prepare(TransactionRequestType  tx_request_type,
                              Action                  tx_action,
                              const nclass         *sending_obj = NULL,
                              const nclass         *current_obj = NULL);

    public:
        vlg::RetCode  send();

    public:
        virtual void    on_request();
        virtual void    on_close();

    public:
        transaction_int *get_internal();
        void             set_internal(transaction_int *tx);

    private:
        transaction_impl *impl_;

    protected:
        static nclass_logger *log_;
};

}

#endif
