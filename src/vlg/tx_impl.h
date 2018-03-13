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
#include "glob.h"

namespace vlg {

struct transaction_impl {
    explicit transaction_impl(transaction &);
    virtual ~transaction_impl();

    void set_connection_sh(std::shared_ptr<connection> &);
    void set_request_obj(const nclass &);
    void set_current_obj(const nclass &);
    void set_result_obj(const nclass &);

    RetCode re_new();

    RetCode await_for_status_reached_or_outdated(TransactionStatus test,
                                                 TransactionStatus &current,
                                                 time_t sec = -1,
                                                 long nsec = 0);

    RetCode await_for_closure(time_t sec = -1, long nsec = 0);

    RetCode send();             //called by client
    RetCode send_response();    //called by server

    void set_request_obj_on_request(nclass &);
    void set_result_obj_on_response(nclass &);

    RetCode set_status(TransactionStatus status);
    RetCode set_flying();
    RetCode set_closed();
    RetCode set_aborted();
    void trace_tx_closure(const char *tx_res_str);

    //associated connection.
    //cannot be ref, because bind is after construction.
    connection *conn_;
    std::shared_ptr<connection> conn_sh_;
    tx_id txid_;                            //(set by client)
    TransactionStatus status_;
    TransactionResult tx_res_;              //(set by server)
    ProtocolCode result_code_;              //(set by server)
    TransactionRequestType txtype_;         //(set by client)
    Action txactn_;                         //(set by client)
    unsigned int req_nclassid_;             //(set by client)
    Encode req_clsenc_;                     //(set by client)
    unsigned int res_nclassid_;             //(set by server)
    Encode res_clsenc_;                     //(set by server)
    bool rsclrq_;    //result class required (set by client)
    bool rescls_;    //result class          (set by server)

    std::unique_ptr<nclass> request_obj_;   //(set by client)
    std::unique_ptr<nclass> current_obj_;   //(set by client)
    std::unique_ptr<nclass> result_obj_;    //(set by server)

    //timing, for stat.
    rt_time_t start_mark_tim_;

    mutable vlg::mx mon_;

    transaction &publ_;
};

}

#endif
