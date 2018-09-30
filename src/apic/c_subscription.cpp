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

    incoming_connection *inco_subscription_get_connection(incoming_subscription *sbs)
    {
        return &sbs->get_connection();
    }

    unsigned int inco_subscription_get_id(incoming_subscription *sbs)
    {
        return sbs->get_id();
    }

    unsigned int inco_subscription_get_subscription_nclass_id(incoming_subscription *sbs)
    {
        return sbs->get_nclass_id();
    }

    SubscriptionType inco_subscription_get_subscription_type(incoming_subscription *sbs)
    {
        return sbs->get_type();
    }

    SubscriptionMode inco_subscription_get_subscription_mode(incoming_subscription *sbs)
    {
        return sbs->get_mode();
    }

    SubscriptionFlowType inco_subscription_get_subscription_flow_type(incoming_subscription *sbs)
    {
        return sbs->get_flow_type();
    }

    SubscriptionDownloadType inco_subscription_get_subscription_download_type(incoming_subscription *sbs)
    {
        return sbs->get_download_type();
    }

    Encode inco_subscription_get_subscription_nclass_encode(incoming_subscription *sbs)
    {
        return sbs->get_nclass_encode();
    }

    unsigned int inco_subscription_get_open_timestamp_0(incoming_subscription *sbs)
    {
        return sbs->get_open_timestamp_0();
    }

    unsigned int inco_subscription_get_open_timestamp_1(incoming_subscription *sbs)
    {
        return sbs->get_open_timestamp_1();
    }

    int inco_subscription_is_initial_query_ended(incoming_subscription *sbs)
    {
        return sbs->is_initial_query_ended() ? 1 : 0;
    }

    void inco_subscription_set_subscription_nclass_id(incoming_subscription *sbs,
                                                      unsigned int nclass_id)
    {
        sbs->set_nclass_id(nclass_id);
    }

    void inco_subscription_set_subscription_type(incoming_subscription *sbs,
                                                 SubscriptionType sbs_type)
    {
        sbs->set_type(sbs_type);
    }

    void inco_subscription_set_subscription_mode(incoming_subscription *sbs,
                                                 SubscriptionMode sbs_mode)
    {
        sbs->set_mode(sbs_mode);
    }

    void inco_subscription_set_subscription_flow_type(incoming_subscription *sbs,
                                                      SubscriptionFlowType sbs_flow_type)
    {
        sbs->set_flow_type(sbs_flow_type);
    }

    void inco_subscription_set_subscription_download_type(incoming_subscription *sbs,
                                                          SubscriptionDownloadType sbs_dwnl_type)
    {
        sbs->set_download_type(sbs_dwnl_type);
    }

    void inco_subscription_set_nclass_encode(incoming_subscription *sbs, Encode nclass_encode)
    {
        sbs->set_nclass_encode(nclass_encode);
    }

    void inco_subscription_set_open_timestamp_0(incoming_subscription *sbs, unsigned int ts_0)
    {
        sbs->set_open_timestamp_0(ts_0);
    }

    void inco_subscription_set_open_timestamp_1(incoming_subscription *sbs, unsigned int ts_1)
    {
        sbs->set_open_timestamp_0(ts_1);
    }

    RetCode inco_subscription_await_for_status_reached(incoming_subscription *sbs,
                                                       SubscriptionStatus test,
                                                       SubscriptionStatus *current,
                                                       time_t sec,
                                                       long nsec)
    {
        return sbs->await_for_status_reached(test,
                                             *current,
                                             sec,
                                             nsec);
    }

    RetCode inco_subscription_stop(incoming_subscription *sbs)
    {
        return sbs->stop();
    }

    RetCode inco_subscription_await_for_stop_result(incoming_subscription *sbs,
                                                    SubscriptionResponse *stop_result,
                                                    ProtocolCode *stop_protocode,
                                                    time_t sec,
                                                    long nsec)
    {
        return sbs->await_for_stop_result(*stop_result,
                                          *stop_protocode,
                                          sec,
                                          nsec);
    }
}

//c_outg_sbs

#define OS_NTF_UD_IDX 0
#define OS_STC_UD_IDX 1
#define OS_STA_UD_IDX 2
#define OS_STO_UD_IDX 3

struct c_outg_sbs : public outgoing_subscription {
    c_outg_sbs();

    outg_subscription_on_notify_event osne_;
    outg_subscription_on_status_change ossc_;
    outg_subscription_on_start ososrt_;
    outg_subscription_on_stop osostp_;

    void *ud_[4];
    void *ud2_[4];
};

struct c_outg_sbs_listener : public outgoing_subscription_listener {
    virtual void on_status_change(outgoing_subscription &os, SubscriptionStatus status) override {
        if(((c_outg_sbs &)os).ossc_) {
            ((c_outg_sbs &)os).ossc_(&os, status,
                                     ((c_outg_sbs &)os).ud_[OS_STC_UD_IDX],
                                     ((c_outg_sbs &)os).ud2_[OS_STC_UD_IDX]);
        }
    }
    virtual void on_start(outgoing_subscription &os) override {
        if(((c_outg_sbs &)os).ososrt_) {
            ((c_outg_sbs &)os).ososrt_(&os,
                                       ((c_outg_sbs &)os).ud_[OS_STA_UD_IDX],
                                       ((c_outg_sbs &)os).ud2_[OS_STA_UD_IDX]);
        }
    }
    virtual void on_stop(outgoing_subscription &os) override {
        if(((c_outg_sbs &)os).osostp_) {
            ((c_outg_sbs &)os).osostp_(&os,
                                       ((c_outg_sbs &)os).ud_[OS_STO_UD_IDX],
                                       ((c_outg_sbs &)os).ud2_[OS_STO_UD_IDX]);
        }
    }
    virtual void on_incoming_event(outgoing_subscription &os, std::unique_ptr<subscription_event> &sev) override {
        if(((c_outg_sbs &)os).osne_) {
            ((c_outg_sbs &)os).osne_(&os, sev.get(),
                                     ((c_outg_sbs &)os).ud_[OS_NTF_UD_IDX],
                                     ((c_outg_sbs &)os).ud2_[OS_NTF_UD_IDX]);
        }
    }
};

static c_outg_sbs_listener coslst;

c_outg_sbs::c_outg_sbs() :
    outgoing_subscription(coslst),
    osne_(nullptr),
    ossc_(nullptr),
    ososrt_(nullptr),
    osostp_(nullptr)
{
    memset(ud_, 0, sizeof(ud_));
    memset(ud2_, 0, sizeof(ud2_));
}

extern "C" {
    own_outgoing_subscription *outg_subscription_create()
    {
        return (own_outgoing_subscription *)new c_outg_sbs();
    }

    outgoing_subscription *outg_subscription_get_ptr(own_outgoing_subscription *sbs)
    {
        return (outgoing_subscription *)sbs;
    }

    void outg_subscription_destroy(own_outgoing_subscription *sbs)
    {
        delete(c_outg_sbs *)sbs;
    }

    RetCode outg_subscription_bind(outgoing_subscription *sbs, outgoing_connection *conn)
    {
        return sbs->bind(*conn);
    }

    outgoing_connection *outg_subscription_get_connection(outgoing_subscription *sbs)
    {
        return &sbs->get_connection();
    }

    unsigned int outg_subscription_get_id(outgoing_subscription *sbs)
    {
        return sbs->get_id();
    }

    unsigned int outg_subscription_get_subscription_nclass_id(outgoing_subscription *sbs)
    {
        return sbs->get_nclass_id();
    }

    SubscriptionType outg_subscription_get_subscription_type(outgoing_subscription *sbs)
    {
        return sbs->get_type();
    }

    SubscriptionMode outg_subscription_get_subscription_mode(outgoing_subscription *sbs)
    {
        return sbs->get_mode();
    }

    SubscriptionFlowType outg_subscription_get_subscription_flow_type(outgoing_subscription *sbs)
    {
        return sbs->get_flow_type();
    }

    SubscriptionDownloadType outg_subscription_get_subscription_download_type(outgoing_subscription *sbs)
    {
        return sbs->get_download_type();
    }

    Encode outg_subscription_get_subscription_nclass_encode(outgoing_subscription *sbs)
    {
        return sbs->get_nclass_encode();
    }

    unsigned int outg_subscription_get_open_timestamp_0(outgoing_subscription *sbs)
    {
        return sbs->get_open_timestamp_0();
    }

    unsigned int outg_subscription_get_open_timestamp_1(outgoing_subscription *sbs)
    {
        return sbs->get_open_timestamp_1();
    }

    void outg_subscription_set_subscription_nclass_id(outgoing_subscription *sbs,
                                                      unsigned int nclass_id)
    {
        sbs->set_nclass_id(nclass_id);
    }

    void outg_subscription_set_subscription_type(outgoing_subscription *sbs,
                                                 SubscriptionType sbs_type)
    {
        sbs->set_type(sbs_type);
    }

    void outg_subscription_set_subscription_mode(outgoing_subscription *sbs,
                                                 SubscriptionMode sbs_mode)
    {
        sbs->set_mode(sbs_mode);
    }

    void outg_subscription_set_subscription_flow_type(outgoing_subscription *sbs,
                                                      SubscriptionFlowType sbs_flow_type)
    {
        sbs->set_flow_type(sbs_flow_type);
    }

    void outg_subscription_set_subscription_download_type(outgoing_subscription *sbs,
                                                          SubscriptionDownloadType sbs_dwnl_type)
    {
        sbs->set_download_type(sbs_dwnl_type);
    }

    void outg_subscription_set_nclass_encode(outgoing_subscription *sbs, Encode nclass_encode)
    {
        sbs->set_nclass_encode(nclass_encode);
    }

    void outg_subscription_set_open_timestamp_0(outgoing_subscription *sbs, unsigned int ts_0)
    {
        sbs->set_open_timestamp_0(ts_0);
    }

    void outg_subscription_set_open_timestamp_1(outgoing_subscription *sbs, unsigned int ts_1)
    {
        sbs->set_open_timestamp_0(ts_1);
    }

    RetCode outg_subscription_await_for_status_reached(outgoing_subscription *sbs,
                                                       SubscriptionStatus test,
                                                       SubscriptionStatus *current,
                                                       time_t sec,
                                                       long nsec)
    {
        return sbs->await_for_status_reached(test,
                                             *current,
                                             sec,
                                             nsec);
    }

    void outg_subscription_set_on_status_change(outgoing_subscription *sbs,
                                                outg_subscription_on_status_change hndl,
                                                void *ud, void *ud2)
    {
        static_cast<c_outg_sbs *>(sbs)->ossc_ = hndl;
        static_cast<c_outg_sbs *>(sbs)->ud_[OS_STC_UD_IDX] = ud;
        static_cast<c_outg_sbs *>(sbs)->ud2_[OS_STC_UD_IDX] = ud2;
    }

    void outg_subscription_set_on_event_notify(outgoing_subscription *sbs,
                                               outg_subscription_on_notify_event hndl,
                                               void *ud, void *ud2)
    {
        static_cast<c_outg_sbs *>(sbs)->osne_ = hndl;
        static_cast<c_outg_sbs *>(sbs)->ud_[OS_NTF_UD_IDX] = ud;
        static_cast<c_outg_sbs *>(sbs)->ud2_[OS_NTF_UD_IDX] = ud2;
    }

    void outg_subscription_set_on_start(outgoing_subscription *sbs,
                                        outg_subscription_on_start hndl,
                                        void *ud, void *ud2)
    {
        static_cast<c_outg_sbs *>(sbs)->ososrt_ = hndl;
        static_cast<c_outg_sbs *>(sbs)->ud_[OS_STA_UD_IDX] = ud;
        static_cast<c_outg_sbs *>(sbs)->ud2_[OS_STA_UD_IDX] = ud2;
    }

    void outg_subscription_set_on_stop(outgoing_subscription *sbs,
                                       outg_subscription_on_stop hndl,
                                       void *ud, void *ud2)
    {
        static_cast<c_outg_sbs *>(sbs)->osostp_ = hndl;
        static_cast<c_outg_sbs *>(sbs)->ud_[OS_STO_UD_IDX] = ud;
        static_cast<c_outg_sbs *>(sbs)->ud2_[OS_STO_UD_IDX] = ud2;
    }

    RetCode outg_subscription_start(outgoing_subscription *sbs)
    {
        return sbs->start();
    }

    RetCode outg_subscription_start_full(outgoing_subscription *sbs,
                                         SubscriptionType sbs_type,
                                         SubscriptionMode sbs_mode,
                                         SubscriptionFlowType sbs_flow_type,
                                         SubscriptionDownloadType sbs_dwnl_type,
                                         Encode nclass_encode,
                                         unsigned int nclass_id,
                                         unsigned int ts_0,
                                         unsigned int ts_1)
    {
        return sbs->start(sbs_type,
                          sbs_mode,
                          sbs_flow_type,
                          sbs_dwnl_type,
                          nclass_encode,
                          nclass_id,
                          ts_0,
                          ts_1);
    }

    RetCode outg_subscription_await_for_start_result(outgoing_subscription *sbs,
                                                     SubscriptionResponse *start_result,
                                                     ProtocolCode *start_protocode,
                                                     time_t sec,
                                                     long nsec)
    {
        return sbs->await_for_start_result(*start_result,
                                           *start_protocode,
                                           sec,
                                           nsec);
    }

    RetCode outg_subscription_stop(outgoing_subscription *sbs)
    {
        return sbs->stop();
    }

    RetCode outg_subscription_await_for_stop_result(outgoing_subscription *sbs,
                                                    SubscriptionResponse *stop_result,
                                                    ProtocolCode *stop_protocode,
                                                    time_t sec,
                                                    long nsec)
    {
        return sbs->await_for_stop_result(*stop_result,
                                          *stop_protocode,
                                          sec,
                                          nsec);
    }

}

