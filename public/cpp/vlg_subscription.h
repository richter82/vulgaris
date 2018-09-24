/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
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
    virtual incoming_subscription &make_incoming_subscription(std::shared_ptr<incoming_connection> &);

    static incoming_subscription_factory &default_factory();
};

/** @brief incoming subscription listener.
*/
struct incoming_subscription_listener {
    virtual void on_status_change(incoming_subscription &, SubscriptionStatus) = 0;
    virtual void on_stop(incoming_subscription &) = 0;
    virtual RetCode on_accept_event(incoming_subscription &, const subscription_event &) = 0;
    virtual void on_releaseable(incoming_subscription &) = 0;

    static incoming_subscription_listener &default_listener();
};

/** @brief incoming_subscription.
*/
struct incoming_subscription {
    explicit incoming_subscription(std::shared_ptr<incoming_connection> &,
                                   incoming_subscription_listener &listener =
                                       incoming_subscription_listener::default_listener());

    virtual ~incoming_subscription();

    incoming_connection &get_connection();
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

    RetCode await_for_status_reached(SubscriptionStatus test,
                                     SubscriptionStatus &current,
                                     time_t sec = -1,
                                     long nsec = 0);

    RetCode stop();

    RetCode await_for_stop_result(SubscriptionResponse &stop_result,
                                  ProtocolCode &stop_protocode,
                                  time_t sec = -1,
                                  long nsec = 0);

    std::unique_ptr<incoming_subscription_impl> impl_;
};

}

namespace vlg {

/** @brief outgoing subscription listener.
*/
struct outgoing_subscription_listener {
    virtual void on_status_change(outgoing_subscription &, SubscriptionStatus) = 0;
    virtual void on_start(outgoing_subscription &) = 0;
    virtual void on_stop(outgoing_subscription &) = 0;
    virtual void on_incoming_event(outgoing_subscription &, std::unique_ptr<subscription_event> &) = 0;

    static outgoing_subscription_listener &default_listener();
};

/** @brief outgoing_subscription.
*/
struct outgoing_subscription {
    explicit outgoing_subscription(outgoing_subscription_listener &listener =
                                       outgoing_subscription_listener::default_listener());

    virtual ~outgoing_subscription();

    RetCode bind(outgoing_connection &);

    outgoing_connection &get_connection();
    unsigned int get_id();
    unsigned int get_nclass_id();
    SubscriptionType get_type() const;
    SubscriptionMode get_mode() const;
    SubscriptionFlowType get_flow_type() const;
    SubscriptionDownloadType get_download_type() const;
    Encode get_nclass_encode() const;
    unsigned int get_open_timestamp_0() const;
    unsigned int get_open_timestamp_1() const;

    void set_nclass_id(unsigned int);
    void set_type(SubscriptionType);
    void set_mode(SubscriptionMode);
    void set_flow_type(SubscriptionFlowType);
    void set_download_type(SubscriptionDownloadType);
    void set_nclass_encode(Encode);
    void set_open_timestamp_0(unsigned int);
    void set_open_timestamp_1(unsigned int);

    SubscriptionStatus get_status() const;

    RetCode await_for_status_reached(SubscriptionStatus test,
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
                  unsigned int ts_0 = 0,
                  unsigned int ts_1 = 0);

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

    std::unique_ptr<outgoing_subscription_impl> impl_;
};

}
