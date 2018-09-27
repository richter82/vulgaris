/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "vlg_subscription.h"
using namespace vlg;

extern "C" {
    typedef struct own_outgoing_subscription own_outgoing_subscription;
    typedef struct own_nclass own_nclass;

    typedef void(*outg_subscription_on_status_change)(outgoing_subscription *osbs,
                                                      SubscriptionStatus status,
                                                      void *ud,
                                                      void *ud2);

    typedef void(*outg_subscription_on_notify_event)(outgoing_subscription *osbs,
                                                     const subscription_event *sbs_evt,
                                                     void *ud,
                                                     void *ud2);

    typedef void(*outg_subscription_on_start)(outgoing_subscription *osbs,
                                              void *ud,
                                              void *ud2);

    typedef void(*outg_subscription_on_stop)(outgoing_subscription *osbs,
                                             void *ud,
                                             void *ud2);
}

extern "C" {

    //subscription_event

    unsigned int subscription_event_get_id(subscription_event *sev)
    {
        return sev->get_id();
    }

    SubscriptionEventType subscription_event_get_event_type(subscription_event *sev)
    {
        return sev->get_event_type();
    }

    ProtocolCode subscription_event_get_proto_code(subscription_event *sev)
    {
        return sev->get_proto_code();
    }

    unsigned int subscription_event_get_timestamp_0(subscription_event *sev)
    {
        return sev->get_timestamp_0();
    }

    unsigned int subscription_event_get_timestamp_1(subscription_event *sev)
    {
        return sev->get_timestamp_1();
    }

    Action subscription_event_get_action(subscription_event *sev)
    {
        return sev->get_action();
    }

    own_nclass *subscription_event_get_object(subscription_event *sev)
    {
        return (own_nclass *) new std::unique_ptr<nclass>(std::move(sev->get_data()));
    }

    //incoming_subscription

    incoming_connection *inco_subscription_get_connection(incoming_subscription *subscription)
    {
        return &subscription->get_connection();
    }

    unsigned int inco_subscription_get_id(incoming_subscription *subscription)
    {
        return subscription->get_id();
    }

    unsigned int inco_subscription_get_subscription_nclass_id(incoming_subscription *subscription)
    {
        return subscription->get_nclass_id();
    }

    SubscriptionType inco_subscription_get_subscription_type(incoming_subscription *subscription)
    {
        return subscription->get_type();
    }

    SubscriptionMode inco_subscription_get_subscription_mode(incoming_subscription *subscription)
    {
        return subscription->get_mode();
    }

    SubscriptionFlowType inco_subscription_get_subscription_flow_type(incoming_subscription *subscription)
    {
        return subscription->get_flow_type();
    }

    SubscriptionDownloadType inco_subscription_get_subscription_download_type(incoming_subscription *subscription)
    {
        return subscription->get_download_type();
    }

    Encode inco_subscription_get_subscription_nclass_encode(incoming_subscription *subscription)
    {
        return subscription->get_nclass_encode();
    }

    unsigned int inco_subscription_get_open_timestamp_0(incoming_subscription *subscription)
    {
        return subscription->get_open_timestamp_0();
    }

    unsigned int inco_subscription_get_open_timestamp_1(incoming_subscription *subscription)
    {
        return subscription->get_open_timestamp_1();
    }

    int inco_subscription_is_initial_query_ended(incoming_subscription *subscription)
    {
        return subscription->is_initial_query_ended() ? 1 : 0;
    }

    void inco_subscription_set_subscription_nclass_id(incoming_subscription *subscription,
                                                      unsigned int nclass_id)
    {
        subscription->set_nclass_id(nclass_id);
    }

    void inco_subscription_set_subscription_type(incoming_subscription *subscription,
                                                 SubscriptionType sbs_type)
    {
        subscription->set_type(sbs_type);
    }

    void inco_subscription_set_subscription_mode(incoming_subscription *subscription,
                                                 SubscriptionMode sbs_mode)
    {
        subscription->set_mode(sbs_mode);
    }

    void inco_subscription_set_subscription_flow_type(incoming_subscription *subscription,
                                                      SubscriptionFlowType sbs_flow_type)
    {
        subscription->set_flow_type(sbs_flow_type);
    }

    void inco_subscription_set_subscription_download_type(incoming_subscription *subscription,
                                                          SubscriptionDownloadType sbs_dwnl_type)
    {
        subscription->set_download_type(sbs_dwnl_type);
    }

    void inco_subscription_set_nclass_encode(incoming_subscription *subscription, Encode nclass_encode)
    {
        subscription->set_nclass_encode(nclass_encode);
    }

    void inco_subscription_set_open_timestamp_0(incoming_subscription *subscription, unsigned int ts_0)
    {
        subscription->set_open_timestamp_0(ts_0);
    }

    void inco_subscription_set_open_timestamp_1(incoming_subscription *subscription, unsigned int ts_1)
    {
        subscription->set_open_timestamp_0(ts_1);
    }

    RetCode inco_subscription_await_for_status_reached(incoming_subscription *subscription,
                                                       SubscriptionStatus test,
                                                       SubscriptionStatus *current,
                                                       time_t sec,
                                                       long nsec)
    {
        return subscription->await_for_status_reached(test,
                                                      *current,
                                                      sec,
                                                      nsec);
    }

    RetCode inco_subscription_stop(incoming_subscription *subscription)
    {
        return subscription->stop();
    }

    RetCode inco_subscription_await_for_stop_result(incoming_subscription *subscription,
                                                    SubscriptionResponse *stop_result,
                                                    ProtocolCode *stop_protocode,
                                                    time_t sec,
                                                    long nsec)
    {
        return subscription->await_for_stop_result(*stop_result,
                                                   *stop_protocode,
                                                   sec,
                                                   nsec);
    }
}

//c_outg_sbs

struct c_outg_sbs : public outgoing_subscription {
    c_outg_sbs();

    outg_subscription_on_notify_event osad_;
    outg_subscription_on_status_change ossc_;
    outg_subscription_on_start ososrt_;
    outg_subscription_on_stop osostp_;
    void *ud_;
    void *ud2_;
};

struct c_outg_sbs_listener : public outgoing_subscription_listener {
    virtual void on_status_change(outgoing_subscription &os, SubscriptionStatus status) override {
        if(((c_outg_sbs &)os).ossc_) {
            ((c_outg_sbs &)os).ossc_(&os, status, ((c_outg_sbs &)os).ud_, ((c_outg_sbs &)os).ud2_);
        }
    }
    virtual void on_start(outgoing_subscription &os) override {
        if(((c_outg_sbs &)os).ososrt_) {
            ((c_outg_sbs &)os).ososrt_(&os, ((c_outg_sbs &)os).ud_, ((c_outg_sbs &)os).ud2_);
        }
    }
    virtual void on_stop(outgoing_subscription &os) override {
        if(((c_outg_sbs &)os).osostp_) {
            ((c_outg_sbs &)os).osostp_(&os, ((c_outg_sbs &)os).ud_, ((c_outg_sbs &)os).ud2_);
        }
    }
    virtual void on_incoming_event(outgoing_subscription &os, std::unique_ptr<subscription_event> &sev) override {
        if(((c_outg_sbs &)os).osad_) {
            ((c_outg_sbs &)os).osad_(&os, sev.get(), ((c_outg_sbs &)os).ud_, ((c_outg_sbs &)os).ud2_);
        }
    }
};

static c_outg_sbs_listener coslst;

c_outg_sbs::c_outg_sbs() :
    outgoing_subscription(coslst),
    osad_(nullptr),
    ossc_(nullptr),
    ososrt_(nullptr),
    osostp_(nullptr),
    ud_(nullptr),
    ud2_(nullptr) {}

extern "C" {
    own_outgoing_subscription *outg_subscription_create()
    {
        return (own_outgoing_subscription *)new c_outg_sbs();
    }

    outgoing_subscription *outg_subscription_get_ptr(own_outgoing_subscription *sbs)
    {
        return (outgoing_subscription *)sbs;
    }

    void outg_subscription_destroy(own_outgoing_subscription *subscription)
    {
        delete(c_outg_sbs *)subscription;
    }

    RetCode outg_subscription_bind(outgoing_subscription *subscription, outgoing_connection *conn)
    {
        return subscription->bind(*conn);
    }

    outgoing_connection *outg_subscription_get_connection(outgoing_subscription *subscription)
    {
        return &subscription->get_connection();
    }

    unsigned int outg_subscription_get_id(outgoing_subscription *subscription)
    {
        return subscription->get_id();
    }

    unsigned int outg_subscription_get_subscription_nclass_id(outgoing_subscription *subscription)
    {
        return subscription->get_nclass_id();
    }

    SubscriptionType outg_subscription_get_subscription_type(outgoing_subscription *subscription)
    {
        return subscription->get_type();
    }

    SubscriptionMode outg_subscription_get_subscription_mode(outgoing_subscription *subscription)
    {
        return subscription->get_mode();
    }

    SubscriptionFlowType outg_subscription_get_subscription_flow_type(outgoing_subscription *subscription)
    {
        return subscription->get_flow_type();
    }

    SubscriptionDownloadType outg_subscription_get_subscription_download_type(outgoing_subscription *subscription)
    {
        return subscription->get_download_type();
    }

    Encode outg_subscription_get_subscription_nclass_encode(outgoing_subscription *subscription)
    {
        return subscription->get_nclass_encode();
    }

    unsigned int outg_subscription_get_open_timestamp_0(outgoing_subscription *subscription)
    {
        return subscription->get_open_timestamp_0();
    }

    unsigned int outg_subscription_get_open_timestamp_1(outgoing_subscription *subscription)
    {
        return subscription->get_open_timestamp_1();
    }

    void outg_subscription_set_subscription_nclass_id(outgoing_subscription *subscription,
                                                      unsigned int nclass_id)
    {
        subscription->set_nclass_id(nclass_id);
    }

    void outg_subscription_set_subscription_type(outgoing_subscription *subscription,
                                                 SubscriptionType sbs_type)
    {
        subscription->set_type(sbs_type);
    }

    void outg_subscription_set_subscription_mode(outgoing_subscription *subscription,
                                                 SubscriptionMode sbs_mode)
    {
        subscription->set_mode(sbs_mode);
    }

    void outg_subscription_set_subscription_flow_type(outgoing_subscription *subscription,
                                                      SubscriptionFlowType sbs_flow_type)
    {
        subscription->set_flow_type(sbs_flow_type);
    }

    void outg_subscription_set_subscription_download_type(outgoing_subscription *subscription,
                                                          SubscriptionDownloadType sbs_dwnl_type)
    {
        subscription->set_download_type(sbs_dwnl_type);
    }

    void outg_subscription_set_nclass_encode(outgoing_subscription *subscription, Encode nclass_encode)
    {
        subscription->set_nclass_encode(nclass_encode);
    }

    void outg_subscription_set_open_timestamp_0(outgoing_subscription *subscription, unsigned int ts_0)
    {
        subscription->set_open_timestamp_0(ts_0);
    }

    void outg_subscription_set_open_timestamp_1(outgoing_subscription *subscription, unsigned int ts_1)
    {
        subscription->set_open_timestamp_0(ts_1);
    }

    RetCode outg_subscription_await_for_status_reached(outgoing_subscription *subscription,
                                                       SubscriptionStatus test,
                                                       SubscriptionStatus *current,
                                                       time_t sec,
                                                       long nsec)
    {
        return subscription->await_for_status_reached(test,
                                                      *current,
                                                      sec,
                                                      nsec);
    }

    void outg_subscription_set_on_status_change(outgoing_subscription *subscription,
                                                outg_subscription_on_status_change hndl)
    {
        static_cast<c_outg_sbs *>(subscription)->ossc_ = hndl;
    }

    void outg_subscription_set_on_event_notify(outgoing_subscription *subscription,
                                               outg_subscription_on_notify_event hndl)
    {
        static_cast<c_outg_sbs *>(subscription)->osad_ = hndl;
    }

    void outg_subscription_set_on_start(outgoing_subscription *sbs,
                                        outg_subscription_on_start hndl)
    {
        static_cast<c_outg_sbs *>(sbs)->ososrt_ = hndl;
    }

    void outg_subscription_set_on_stop(outgoing_subscription *sbs,
                                       outg_subscription_on_stop hndl)
    {
        static_cast<c_outg_sbs *>(sbs)->osostp_ = hndl;
    }

    void outg_subscription_set_user_data(outgoing_subscription *sbs, void *ud, void *ud2)
    {
        static_cast<c_outg_sbs *>(sbs)->ud_ = ud;
        static_cast<c_outg_sbs *>(sbs)->ud2_ = ud2;
    }

    RetCode outg_subscription_start(outgoing_subscription *subscription)
    {
        return subscription->start();
    }

    RetCode outg_subscription_start_full(outgoing_subscription *subscription,
                                         SubscriptionType sbs_type,
                                         SubscriptionMode sbs_mode,
                                         SubscriptionFlowType sbs_flow_type,
                                         SubscriptionDownloadType sbs_dwnl_type,
                                         Encode nclass_encode,
                                         unsigned int nclass_id,
                                         unsigned int ts_0,
                                         unsigned int ts_1)
    {
        return subscription->start(sbs_type,
                                   sbs_mode,
                                   sbs_flow_type,
                                   sbs_dwnl_type,
                                   nclass_encode,
                                   nclass_id,
                                   ts_0,
                                   ts_1);
    }

    RetCode outg_subscription_await_for_start_result(outgoing_subscription *subscription,
                                                     SubscriptionResponse *start_result,
                                                     ProtocolCode *start_protocode,
                                                     time_t sec,
                                                     long nsec)
    {
        return subscription->await_for_start_result(*start_result,
                                                    *start_protocode,
                                                    sec,
                                                    nsec);
    }

    RetCode outg_subscription_stop(outgoing_subscription *subscription)
    {
        return subscription->stop();
    }

    RetCode outg_subscription_await_for_stop_result(outgoing_subscription *subscription,
                                                    SubscriptionResponse *stop_result,
                                                    ProtocolCode *stop_protocode,
                                                    time_t sec,
                                                    long nsec)
    {
        return subscription->await_for_stop_result(*stop_result,
                                                   *stop_protocode,
                                                   sec,
                                                   nsec);
    }

}

