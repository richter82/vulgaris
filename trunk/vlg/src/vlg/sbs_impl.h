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

#ifndef VLG_SUBSCRIPTION_H_
#define VLG_SUBSCRIPTION_H_
#include "glob.h"

namespace vlg {

enum SBSEvt {
    SBSEvt_Undef,
    SBSEvt_Reset,
    SBSEvt_Ready,
    SBSEvt_ToAck,
};

struct persistence_query_impl;

struct subscription_event_impl {
    explicit subscription_event_impl(unsigned int sbsid,
                                     unsigned int evtid,
                                     SubscriptionEventType set,
                                     ProtocolCode pc,
                                     unsigned int ts0,
                                     unsigned int ts1,
                                     Action act,
                                     const nclass &data);

    explicit subscription_event_impl(unsigned int sbsid,
                                     unsigned int evtid,
                                     SubscriptionEventType set,
                                     ProtocolCode pc,
                                     unsigned int ts0,
                                     unsigned int ts1,
                                     Action act,
                                     nclass *data);

    unsigned int                    sbs_sbsid_;
    unsigned int                    sbs_evtid_;
    SubscriptionEventType           sbs_evttype_;
    ProtocolCode                    sbs_protocode_;
    unsigned int                    sbs_tmstp0_;    /*part. dwnl.*/
    unsigned int                    sbs_tmstp1_;    /*part. dwnl.*/
    Action                          sbs_act_;
    std::unique_ptr<nclass>         sbs_data_;
};

// sbs_event_wrapper
// server only.
//@fixme @warning
//shared ptr should be enougth?
struct sbs_event_wrapper {
    explicit sbs_event_wrapper(std::shared_ptr<subscription_event> &sbs_evt) : sbs_evt_(sbs_evt) {};
    std::shared_ptr<subscription_event> sbs_evt_;
};

struct subscription_impl {
        explicit subscription_impl(subscription &publ);
        virtual ~subscription_impl();

        void set_connection(connection &);
        void set_connection_sh(std::shared_ptr<connection> &);

        // STATUS
        RetCode set_status(SubscriptionStatus status);
        RetCode set_req_sent();
        RetCode set_started();
        RetCode set_stopped();
        RetCode set_released();
        RetCode set_error();

        //client side
        RetCode start();

        //client side
        RetCode start(SubscriptionType sbscr_type,
                      SubscriptionMode sbscr_mode,
                      SubscriptionFlowType sbscr_flow_type,
                      SubscriptionDownloadType sbscr_dwnld_type,
                      Encode sbscr_class_encode,
                      unsigned int nclass_id,
                      unsigned int start_timestamp_0 = 0,
                      unsigned int start_timestamp_1 = 0);

        /* this function must be called from same thread that called Start()*/
        RetCode await_for_start_result(SubscriptionResponse &sbs_start_result,
                                       ProtocolCode &sbs_start_protocode,
                                       time_t sec = -1,
                                       long nsec = 0);

        RetCode stop();

        /* this function must be called from same thread that called Stop()*/
        RetCode await_for_stop_result(SubscriptionResponse &sbs_stop_result,
                                      ProtocolCode &sbs_stop_protocode,
                                      time_t sec = -1,
                                      long nsec = 0);

        RetCode await_for_status_reached_or_outdated(SubscriptionStatus test,
                                                     SubscriptionStatus &current,
                                                     time_t sec = -1,
                                                     long nsec = 0);

        RetCode execute_initial_query();
        void release_initial_query();
        RetCode safe_submit_dwnl_event();

        RetCode send_start_request();
        RetCode send_start_response();
        RetCode notify_for_start_stop_result();

        /*Client only*/
        RetCode receive_event(const vlg_hdr_rec *pkt_hdr,
                              vlg::g_bbuf *pkt_body);

        /*Server only*/
        RetCode receive_event_ack(const vlg_hdr_rec *pkt_hdr);
        RetCode submit_live_event(std::shared_ptr<subscription_event> &sbs_evt);
        RetCode submit_dwnl_event();

    private:
        RetCode send_event(std::shared_ptr<subscription_event> &sbs_evt);
        RetCode send_event_ack(unsigned int evtid);

        // client event handling
        RetCode evt_reset();
        RetCode evt_ready();
        RetCode evt_to_ack();
        RetCode evt_ack(unsigned int evtid);

        RetCode store_sbs_evt(std::shared_ptr<subscription_event> &sbs_evt);
        RetCode consume_sbs_evt(std::shared_ptr<subscription_event> &sbs_evt);

        void set_sbs_to_ack_evt_id(unsigned int sbs_evtid);
        void set_sbs_last_ack_evt_id();

    public:
        connection *conn_;
        std::shared_ptr<connection> conn_sh_;
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

        SBSEvt cli_evt_sts_;

        //last sent evt has been ack by client?
        bool srv_sbs_last_evt_ack_;
        //sent subscription evt id that must be ack by client.
        unsigned int srv_sbs_to_ack_evtid_;
        //last subscription evt id that has been ack by client.
        unsigned int srv_sbs_last_ack_evtid_;

        //srv subscription event global queue
        vlg::b_qu srv_sbs_evt_glob_q_;
        //[nclasskeyvalue] --> [nclasskeyvalue_instance_event_queue]
        std::unordered_map<std::string, std::unique_ptr<b_qu>> srv_sbs_nclasskey_evt_q_hm_;
        mutable pthread_rwlock_t lock_srv_sbs_rep_deferred_;

        std::unique_ptr<persistence_query_impl> initial_query_;
        bool initial_query_ended_;

        mutable vlg::mx mon_;

        subscription &publ_;
};

}

#endif
