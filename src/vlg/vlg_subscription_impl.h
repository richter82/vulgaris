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
#include "vlg_logger.h"
#include "vlg_globint.h"

namespace vlg {

class persistence_query_impl;

// subscription_event_impl
class subscription_event_impl : public vlg::collectable {
        friend class connection_impl;
        friend class subscription_impl;

    public:
        static RetCode new_instance(subscription_event_impl **sbs_evt);

    private:
        subscription_event_impl();
    public:
        virtual ~subscription_event_impl();

    public:
        virtual vlg::collector &get_collector();

        //--getters
    public:
        unsigned int            get_sbsid();
        unsigned int            get_evtid();
        SubscriptionEventType   get_evttype();
        ProtocolCode            get_protocode();
        unsigned int            get_tmstp0();   /*part. dwnl.*/
        unsigned int            get_tmstp1();   /*part. dwnl.*/
        Action                  get_act();
        nclass               *get_obj();

        //--setters
    public:
        void set_sbsid(unsigned int val);
        void set_evtid(unsigned int val);
        void set_evttype(SubscriptionEventType val);
        void set_protocode(ProtocolCode val);
        void set_tmstp0(unsigned int val);
        void set_tmstp1(unsigned int val);
        void set_act(Action val);
        void set_obj(const nclass *obj);

    private:
        void set_obj_on_event_receive(nclass *val);

        //--rep
    private:
        unsigned int            sbs_sbsid_;
        unsigned int            sbs_evtid_;
        SubscriptionEventType   sbs_evttype_;
        ProtocolCode            sbs_protocode_;
        unsigned int            sbs_tmstp0_;    /*part. dwnl.*/
        unsigned int            sbs_tmstp1_;    /*part. dwnl.*/
        Action                  sbs_act_;
        nclass               *sbs_data_;

    protected:
        static nclass_logger   *log_;
};

// VLG_SUBSCRIPTION_EVENT_WRAPPER
// server only.
class sbs_event_wrapper {
    public:
        //--ctors
        explicit sbs_event_wrapper(subscription_event_impl *evt);
        ~sbs_event_wrapper();

        void                        set_evt(subscription_event_impl *evt);
        subscription_event_impl     *get_evt();

        //--rep
    private:
        subscription_event_impl *evt_;
};

enum SBSEvt {
    SBSEvt_Undef,
    SBSEvt_Reset,
    SBSEvt_Ready,
    SBSEvt_ToAck,
};

// VLG_SUBSCRIPTION

class subscription_impl : public vlg::collectable {
        friend class peer_impl;
        friend class connection_impl;
        friend class peer_sbs_task;

        typedef void (*status_change)(subscription_impl &sbs,
                                      SubscriptionStatus status,
                                      void *ud);

        typedef void(*event_notify)(subscription_impl &sbs,
                                    subscription_event_impl &sbs_evt,
                                    void *ud);

        //---ctors
    protected:
        explicit subscription_impl(subscription &publ,
                                   connection_impl &conn);
        virtual ~subscription_impl();

    public:
        virtual vlg::collector &get_collector();

        // GETTERS
    public:
        peer_impl           &peer();
        connection_impl     &get_connection();
        unsigned int        sbsid();
        unsigned int        nclass_id();

        SubscriptionType            get_sbstyp() const;
        SubscriptionMode            get_sbsmod() const;
        SubscriptionFlowType        get_flotyp() const;
        SubscriptionDownloadType    get_dwltyp() const;
        Encode                      get_enctyp() const;
        unsigned int                get_open_tmstp0() const;
        unsigned int                get_open_tmstp1() const;

        persistence_query_impl      *get_initial_query();
        bool                        is_initial_query_ended();

        // SETTERS
    public:
        void    set_nclassid(unsigned int nclass_id);
        void    set_sbstyp(SubscriptionType val);
        void    set_sbsmod(SubscriptionMode val);
        void    set_flotyp(SubscriptionFlowType val);
        void    set_dwltyp(SubscriptionDownloadType val);
        void    set_enctyp(Encode val);
        void    set_open_tmstp0(unsigned int val);
        void    set_open_tmstp1(unsigned int val);
        void    set_initial_query(persistence_query_impl *initial_query);
        void    set_initial_query_ended(bool val);

        // INIT
    private:
        RetCode  init();

        // ACTIONS
    public:
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
        RetCode    await_for_start_result(SubscriptionResponse
                                          &sbs_start_result,
                                          ProtocolCode &sbs_start_protocode,
                                          time_t sec = -1,
                                          long nsec = 0);


        /***********************************
        Calling this function will ensure that you won't receive subscription events
        anymore until you call Start() again.
        Even if subscription is stopped, it is still attached to underlying
        [peer-connection] rep.
        This means that until you call connection.ReleaseSubscription()
        you cannot safedestroy this subscription.
        ***********************************/
        RetCode stop();

        /* this function must be called from same thread that called Stop()*/
        RetCode    await_for_stop_result(SubscriptionResponse &sbs_stop_result,
                                         ProtocolCode &sbs_stop_protocode,
                                         time_t sec = -1,
                                         long nsec = 0);

    private:
        RetCode    notify_for_start_stop_result();

        // STATUS SYNCHRO
    public:
        RetCode    await_for_status_reached_or_outdated(SubscriptionStatus test,
                                                        SubscriptionStatus &current,
                                                        time_t sec = -1,
                                                        long nsec = 0);

        // STATUS ASYNCHRO HNDLRS
    public:
        void set_status_change_handler(status_change hndlr,
                                       void *ud);

        // EVENT NOTIFY SYNCHRO
    public:
        RetCode await_for_next_event(subscription_event_impl **sbs_evt,
                                     time_t sec = -1,
                                     long nsec = 0);

        RetCode  ack_event();

        // EVENT NOTIFY ASYNCHRO HNDLRS
    public:
        void set_event_notify_handler(event_notify hndlr,
                                      void *ud);

        // APPLICATIVE HANDLERS
    public:
        virtual void on_start();
        virtual void on_stop();
        virtual void on_event(subscription_event_impl &sbs_evt);

        // AUTHORIZE EVENT (SERVER
        virtual RetCode accept_event(subscription_event_impl *sbs_evt);

        // SEND
    private:
        RetCode send_start_request();
        RetCode send_start_response();
        RetCode send_event(const subscription_event_impl *sbs_evt);
        RetCode send_event_ack();


        // submit_live_event  - SERVER ONLY

        /***********************************
        Note:
        In the scope of this function evt is live. This because for this evt
        this function is called for all subscriptions by the same sbs thread.
        When evt must be stored or send to the selector thread an adoption is needed.
        ***********************************/
    private:
        RetCode submit_live_event(subscription_event_impl *sbs_evt);

        // RECEIVE
    private:
        /*Client only*/
        RetCode receive_event(const vlg_hdr_rec *pkt_hdr,
                              vlg::grow_byte_buffer *pkt_body,
                              subscription_event_impl *sbs_evt);

        /*Server only*/
        RetCode receive_event_ack(const vlg_hdr_rec *pkt_hdr);


        // Sbs Initial Query  - SERVER ONLY

    private:
        RetCode execute_initial_query();
        RetCode safe_submit_dwnl_event();
        RetCode submit_dwnl_event();
        void release_initial_query();

        // STATUS
    public:
        SubscriptionStatus  status();
        RetCode             set_req_sent();
        RetCode             set_started();
        RetCode             set_stopped();
        RetCode             set_released();
        RetCode             set_error();

    private:
        RetCode             set_status(SubscriptionStatus status);

        // EVT INTER
        // All these func MUST be called inside a synch region.
    private:
        RetCode    consume_event(subscription_event_impl **sbs_evt);
        RetCode    ack_event_priv();
        RetCode    evt_reset();
        RetCode    evt_ready();
        RetCode    evt_to_ack();

        // REP
    private:
        peer_impl               &peer_; // associated peer.
        connection_impl         &conn_; // underlying connection.
        const nentity_manager   &nem_;
        unsigned int            sbsid_;
        unsigned int            reqid_; //set by client
        SubscriptionStatus      status_;

        //--asynch status
        status_change   ssc_hndl_;
        void            *ssc_hndl_ud_;
        bool            start_stop_evt_occur_;

    private:
        SubscriptionType            sbstyp_;         //set by client
        SubscriptionMode            sbsmod_;         //set by client
        SubscriptionFlowType        flotyp_;         //set by client
        SubscriptionDownloadType    dwltyp_;         //set by client
        Encode                      enctyp_;         //set by client
        unsigned int                nclassid_;         //set by client
        unsigned int                open_tmstp0_;    //set by client
        unsigned int                open_tmstp1_;    //set by client

    private:
        SubscriptionResponse        sbresl_;         //set by srv - start/stop
        ProtocolCode                last_vlgcod_;    //set by srv

        //--asynch evt notify
        event_notify    sen_hndl_;
        void            *sen_hndl_ud_;

    private:
        SBSEvt                  cli_evt_sts_;
        vlg::blocking_queue     cli_evt_q_;
        subscription_event_impl *cli_last_evt_;

        // SRV SBS MNG REP BG
    private:
        //last sent evt has been ack by client?
        bool srv_sbs_last_evt_ack_;
        //sent sbs evt id that must be ack by client.
        unsigned int srv_sbs_to_ack_evtid_;
        //last sbs evt id that has been ack by client.
        unsigned int srv_sbs_last_ack_evtid_;

        void    set_sbs_to_ack_evt_id(unsigned int sbs_evtid);
        void    set_sbs_last_ack_evt_id();

        /**Asyncro sbs rep - begin*/
    private:
        //srv sbs event global queue
        vlg::blocking_queue srv_sbs_evt_glob_q_;
        //[classkeyvalue] --> [classkeyvalue_instance_event_queue]
        vlg::hash_map srv_sbs_classkey_evt_q_hm_;

        RetCode store_sbs_evt_srv_asynch(subscription_event_impl *evt);
        /**returns OK if a new event is available. KO if not.*/
        RetCode consume_sbs_evt_srv_asynch(subscription_event_impl **evt_out);

        //dedicated lock to synchro *asyncro sbs rep*
        mutable pthread_rwlock_t lock_srv_sbs_rep_asynch_;
        /**Asyncro sbs rep - end*/

        //initial query rep
    private:
        persistence_query_impl  *initial_query_;
        bool                    initial_query_ended_;

    private:
        mutable vlg::synch_monitor mon_;

    private:
        subscription &publ_;

    protected:
        static nclass_logger *log_;
};

}

#endif
