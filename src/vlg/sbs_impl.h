/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#pragma once
#include "glob.h"

namespace vlg {

struct persistence_query_impl;
struct subscription_event_impl {
    explicit subscription_event_impl(unsigned int sbsid,
                                     unsigned int evtid,
                                     SubscriptionEventType set,
                                     ProtocolCode pc,
                                     unsigned int ts_0,
                                     unsigned int ts_1,
                                     Action act,
                                     const nclass &data);

    explicit subscription_event_impl(unsigned int sbsid,
                                     unsigned int evtid,
                                     SubscriptionEventType set,
                                     ProtocolCode pc,
                                     unsigned int ts_0,
                                     unsigned int ts_1,
                                     Action act,
                                     std::unique_ptr<nclass> &data);

    unsigned int sbs_sbsid_;
    unsigned int sbs_evtid_;
    SubscriptionEventType sbs_evttype_;
    ProtocolCode sbs_protocode_;
    unsigned int sbs_tmstp0_;
    unsigned int sbs_tmstp1_;
    Action sbs_act_;
    std::unique_ptr<nclass> sbs_data_;
};

struct sbs_impl {
    explicit sbs_impl(incoming_subscription &publ,
                      incoming_connection &conn,
                      incoming_subscription_listener &listener);

    explicit sbs_impl(outgoing_subscription &publ,
                      outgoing_subscription_listener &listener);

    virtual ~sbs_impl() = default;

    RetCode set_status(SubscriptionStatus status);
    RetCode set_started();
    RetCode set_stopped();
    RetCode set_released();

    RetCode stop();

    RetCode await_for_start_result(SubscriptionResponse &sbs_start_result,
                                   ProtocolCode &sbs_start_protocode,
                                   time_t sec = -1,
                                   long nsec = 0);

    RetCode await_for_stop_result(SubscriptionResponse &sbs_stop_result,
                                  ProtocolCode &sbs_stop_protocode,
                                  time_t sec = -1,
                                  long nsec = 0);

    RetCode await_for_status_reached(SubscriptionStatus test,
                                     SubscriptionStatus &current,
                                     time_t sec = -1,
                                     long nsec = 0);

    RetCode notify_for_start_stop_result();
    void ntfy_sel_snd_pkt();

    conn_impl *conn_;

    unsigned int sbsid_;
    unsigned int reqid_;
    SubscriptionStatus status_;
    bool start_stop_evt_occur_;

    SubscriptionType sbstyp_;           //set by client
    SubscriptionMode sbsmod_;           //set by client
    SubscriptionFlowType flotyp_;       //set by client
    SubscriptionDownloadType dwltyp_;   //set by client
    Encode enctyp_;                     //set by client
    unsigned int nclassid_;             //set by client
    unsigned int open_tmstp0_;          //set by client
    unsigned int open_tmstp1_;          //set by client

    SubscriptionResponse sbresl_;       //set by srv - start/stop
    ProtocolCode last_vlgcod_;          //set by srv

    mutable std::mutex mtx_;
    mutable std::condition_variable cv_;

    incoming_subscription *ipubl_;
    outgoing_subscription *opubl_;
    incoming_subscription_listener *ilistener_;
    outgoing_subscription_listener *olistener_;
};

struct incoming_subscription_impl : public sbs_impl {
    explicit incoming_subscription_impl(incoming_subscription &publ,
                                        std::shared_ptr<incoming_connection> &,
                                        incoming_subscription_listener &);
    virtual ~incoming_subscription_impl();

    RetCode execute_initial_query();
    void release_initial_query();
    RetCode send_start_response();

    void submit_evt(std::shared_ptr<subscription_event> &sbs_evt) {
        if(!(ilistener_->on_accept_event(*ipubl_, *sbs_evt))) {
            enq_event(sbs_evt);
        }
    }

    void ntfy_sel_snd_pkt();

    RetCode send_initial_query();
    void enq_event(std::shared_ptr<subscription_event> &sbs_evt);

    std::shared_ptr<incoming_connection> conn_sh_;
    std::unique_ptr<persistence_query_impl> initial_query_;
    bool initial_query_ended_;
};

}

namespace vlg {

struct outgoing_subscription_impl : public sbs_impl {
    explicit outgoing_subscription_impl(outgoing_subscription &publ,
                                        outgoing_subscription_listener &);
    virtual ~outgoing_subscription_impl();

    RetCode set_req_sent();

    RetCode start();

    RetCode start(SubscriptionType sbscr_type,
                  SubscriptionMode sbscr_mode,
                  SubscriptionFlowType sbscr_flow_type,
                  SubscriptionDownloadType sbscr_dwnld_type,
                  Encode sbscr_nclass_encode,
                  unsigned int nclass_id,
                  unsigned int start_timestamp_0 = 0,
                  unsigned int start_timestamp_1 = 0);

    RetCode send_start_request();

    RetCode receive_event(const vlg_hdr_rec *pkt_hdr,
                          g_bbuf *pkt_body);
};

}
