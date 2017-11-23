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
        static transaction_impl  *tx_factory_impl_f(connection_impl &conn,
                                                    void *ud);

    public:
        transaction_factory();
        virtual ~transaction_factory();

    public:
        virtual transaction     *new_transaction(connection &conn);

    public:
        static transaction_factory *default_factory();
};

/** @brief transaction class.
*/
class transaction_impl_pub;
class transaction : public vlg::collectable {
    public:
        typedef void (*status_change)(transaction &tx,
                                      TransactionStatus status,
                                      void *ud);

        typedef void (*close)(transaction &tx,
                              void *ud);

        //---ctors
    public:
        explicit transaction();
        virtual ~transaction();

        virtual vlg::collector &get_collector();

        vlg::RetCode bind(connection &conn);

    public:
        connection                  *get_connection();
        TransactionResult           get_close_result();
        ProtocolCode                get_close_result_code();
        TransactionRequestType      get_request_type();
        Action                      get_request_action();
        unsigned int                get_request_nclass_id();
        Encode                      get_request_nclass_encode();
        unsigned int                get_result_nclass_id();
        Encode                      get_result_nclass_encode();
        bool                        is_result_obj_required();
        bool                        is_result_obj_set();
        const nclass                *get_request_obj();
        const nclass                *get_current_obj();
        const nclass                *get_result_obj();

    public:
        void    set_result(TransactionResult tx_res);
        void    set_result_code(ProtocolCode tx_res_code);
        void    set_request_type(TransactionRequestType tx_req_type);
        void    set_request_action(Action tx_act);
        void    set_request_nclass_id(unsigned int nclass_id);
        void    set_request_nclass_encode(Encode class_encode);
        void    set_result_nclass_id(unsigned int nclass_id);
        void    set_result_nclass_encode(Encode class_encode);
        void    set_result_obj_required(bool res_class_req);
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

        vlg::RetCode await_for_close(time_t sec = -1,
                                     long nsec = 0);

    public:
        void set_status_change_handler(status_change handler,
                                       void *ud);

    public:
        void set_close_handler(close handler,
                               void *ud);

    public:
        tx_id           &get_tx_id();
        void            set_tx_id(tx_id &txid);
        unsigned int    get_tx_id_PLID();
        unsigned int    get_tx_id_SVID();
        unsigned int    get_tx_id_CNID();
        unsigned int    get_tx_id_PRID();
        void            set_tx_id_PLID(unsigned int plid);
        void            set_tx_id_SVID(unsigned int svid);
        void            set_tx_id_CNID(unsigned int cnid);
        void            set_tx_id_PRID(unsigned int prid);

    public:
        vlg::RetCode  renew();

        vlg::RetCode  prepare();

        vlg::RetCode  prepare(TransactionRequestType    tx_request_type,
                              Action                    tx_action,
                              const nclass              *sending_obj = NULL,
                              const nclass              *current_obj = NULL);

    public:
        vlg::RetCode  send();

    public:
        virtual void    on_request();
        virtual void    on_close();

    public:
        transaction_impl *get_opaque();
        void             set_opaque(transaction_impl *tx);

    private:
        transaction_impl_pub *impl_;

    protected:
        static nclass_logger *log_;
};

}

#endif
