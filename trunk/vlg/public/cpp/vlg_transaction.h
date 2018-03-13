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
#include "vlg.h"

namespace vlg {

/** @brief transaction_factory.
*/
struct incoming_transaction_factory {
    explicit incoming_transaction_factory();
    virtual ~incoming_transaction_factory();
    virtual transaction &make_incoming_transaction(connection &);

    static incoming_transaction_factory &default_factory();
};

/** @brief transaction.
*/
struct transaction {
    explicit transaction();
    virtual ~transaction();

    RetCode bind(connection &);

    connection &get_connection();

    TransactionResult get_close_result();
    ProtocolCode get_close_result_code();

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

    void set_result(TransactionResult);
    void set_result_code(ProtocolCode);

    void set_request_type(TransactionRequestType);
    void set_request_action(Action);
    void set_request_nclass_id(unsigned int);
    void set_request_nclass_encode(Encode);

    void set_result_nclass_id(unsigned int);
    void set_result_nclass_encode(Encode);
    void set_result_obj_required(bool);

    void set_request_obj(const nclass &);
    void set_current_obj(const nclass &);
    void set_result_obj(const nclass &);

    TransactionStatus get_status();

    RetCode await_for_status_reached_or_outdated(TransactionStatus test,
                                                 TransactionStatus &current,
                                                 time_t sec = -1,
                                                 long nsec = 0);

    RetCode await_for_close(time_t sec = -1,
                            long nsec = 0);

    tx_id &get_id();
    void set_id(tx_id &);

    RetCode send();
    RetCode renew();

    virtual void on_status_change(TransactionStatus);
    virtual void on_request();
    virtual void on_close();

    std::unique_ptr<vlg::transaction_impl> impl_;
};

}

#endif
