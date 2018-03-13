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

#ifndef VLG_CPP_SUBSCRIPTION_H_
#define VLG_CPP_SUBSCRIPTION_H_
#include "vlg.h"

namespace vlg {

/** @brief subscription_event.
*/
struct subscription_event {
    explicit subscription_event(subscription_event_impl &);
    ~subscription_event();

    unsigned int get_id();
    SubscriptionEventType get_event_type();
    ProtocolCode get_proto_code();
    unsigned int get_timestamp_0();
    unsigned int get_timestamp_1();
    Action get_action();
    std::unique_ptr<nclass> &get_data();

    std::unique_ptr<subscription_event_impl> impl_;
};

/** @brief subscription_factory.
*/
struct incoming_subscription_factory {
    explicit incoming_subscription_factory();
    virtual ~incoming_subscription_factory();
    virtual subscription &make_incoming_subscription(connection &);

    static incoming_subscription_factory &default_factory();
};

/** @brief subscription.
*/
struct subscription {
    explicit subscription();
    virtual ~subscription();

    RetCode bind(connection &);

    connection &get_connection();
    unsigned int get_id();
    unsigned int get_nclass_id();
    SubscriptionType get_type() const;
    SubscriptionMode get_mode() const;
    SubscriptionFlowType get_flow_type() const;
    SubscriptionDownloadType get_download_type() const;
    Encode get_nclass_encode() const;
    unsigned int get_open_timestamp_0() const;
    unsigned int get_open_timestamp_1() const;
    bool is_initial_query_ended();

    void set_nclass_id(unsigned int);
    void set_type(SubscriptionType);
    void set_mode(SubscriptionMode);
    void set_flow_type(SubscriptionFlowType);
    void set_download_type(SubscriptionDownloadType);
    void set_nclass_encode(Encode);
    void set_open_timestamp_0(unsigned int);
    void set_open_timestamp_1(unsigned int);

    SubscriptionStatus get_status() const;

    RetCode await_for_status_reached_or_outdated(SubscriptionStatus test,
                                                 SubscriptionStatus &current,
                                                 time_t sec = -1,
                                                 long nsec = 0);
    RetCode start();

    RetCode start(SubscriptionType sbs_type,
                  SubscriptionMode sbs_mode,
                  SubscriptionFlowType sbs_flow_type,
                  SubscriptionDownloadType sbs_dwnl_type,
                  Encode nclass_encode,
                  unsigned int nclass_id,
                  unsigned int open_timestamp_0 = 0,
                  unsigned int open_timestamp_1 = 0);

    RetCode await_for_start_result(SubscriptionResponse &start_result,
                                   ProtocolCode &start_protocode,
                                   time_t sec = -1,
                                   long nsec = 0);

    RetCode stop();

    /* this function must be called from same thread that called stop()*/
    RetCode await_for_stop_result(SubscriptionResponse &stop_result,
                                  ProtocolCode &stop_protocode,
                                  time_t sec = -1,
                                  long nsec = 0);

    virtual void on_status_change(SubscriptionStatus);
    virtual void on_start();
    virtual void on_stop();
    virtual void on_incoming_event(std::unique_ptr<subscription_event> &);

    virtual RetCode accept_distribution(const subscription_event &);

    std::unique_ptr<vlg::subscription_impl> impl_;
};

}

#endif
