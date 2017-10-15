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

#ifndef BLZ_SUBSCRIPTION_H_
#define BLZ_SUBSCRIPTION_H_
#include "blaze_logger.h"
#include "blz_glob_int.h"

namespace blaze {

class persistence_query_int;

//-----------------------------
// subscription_event_int
//-----------------------------
class subscription_event_int : public blaze::collectable {
        friend class connection_int;
        friend class subscription_int;

    public:
        static blaze::RetCode new_instance(subscription_event_int **sbs_evt);

    private:
        subscription_event_int();
    public:
        virtual ~subscription_event_int();

    public:
        virtual blaze::collector &get_collector();

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

//-----------------------------
// BLZ_SUBSCRIPTION_EVENT_WRAPPER
// server only.
//-----------------------------
class sbs_event_wrapper {
    public:
        //--ctors
        explicit sbs_event_wrapper(subscription_event_int *evt);
        ~sbs_event_wrapper();

        void                        set_evt(subscription_event_int *evt);
        subscription_event_int     *get_evt();

        //--rep
    private:
        subscription_event_int *evt_;
};

enum BLZ_SBS_Evt {
    BLZ_SBS_Evt_Undef,
    BLZ_SBS_Evt_Rst,
    BLZ_SBS_Evt_Rdy,
    BLZ_SBS_Evt_ToAck,
};

//-----------------------------
// BLZ_SUBSCRIPTION
//-----------------------------

class subscription_int : public blaze::collectable {
        friend class peer_int;
        friend class connection_int;
        friend class peer_sbs_task;

        typedef void (*subscription_status_change_hndlr)(subscription_int &sbs,
                                                         SubscriptionStatus status,
                                                         void *ud);

        typedef void(*subscription_evt_notify_hndlr)(subscription_int &sbs,
                                                     subscription_event_int &sbs_evt,
                                                     void *ud);

        //---ctors
    protected:
        explicit subscription_int(connection_int &conn);
        virtual ~subscription_int();

    public:
        virtual blaze::collector &get_collector();

        //-----------------------------
        // GETTERS
        //-----------------------------
    public:
        peer_int           &peer();
        connection_int     &get_connection();
        unsigned int        sbsid();
        unsigned int        nclass_id();

        SubscriptionType            get_sbstyp() const;
        SubscriptionMode            get_sbsmod() const;
        SubscriptionFlowType        get_flotyp() const;
        SubscriptionDownloadType    get_dwltyp() const;
        Encode                      get_enctyp() const;
        unsigned int                get_open_tmstp0() const;
        unsigned int                get_open_tmstp1() const;

        persistence_query_int      *get_initial_query();
        bool                        is_initial_query_ended();

        //-----------------------------
        // SETTERS
        //-----------------------------
    public:
        void    set_nclassid(unsigned int nclass_id);
        void    set_sbstyp(SubscriptionType val);
        void    set_sbsmod(SubscriptionMode val);
        void    set_flotyp(SubscriptionFlowType val);
        void    set_dwltyp(SubscriptionDownloadType val);
        void    set_enctyp(Encode val);
        void    set_open_tmstp0(unsigned int val);
        void    set_open_tmstp1(unsigned int val);

        void    set_initial_query(persistence_query_int *initial_query);
        void    set_initial_query_ended(bool val);

        //-----------------------------
        // INIT
        //-----------------------------
    private:
        blaze::RetCode  init();

        //-----------------------------
        // ACTIONS
        //-----------------------------
    public:
        //client side
        blaze::RetCode start();

        //client side
        blaze::RetCode start(SubscriptionType sbscr_type,
                             SubscriptionMode sbscr_mode,
                             SubscriptionFlowType sbscr_flow_type,
                             SubscriptionDownloadType sbscr_dwnld_type,
                             Encode sbscr_class_encode,
                             unsigned int nclass_id,
                             unsigned int start_timestamp_0 = 0,
                             unsigned int start_timestamp_1 = 0);

        /* this function must be called from same thread that called Start()*/
        blaze::RetCode    await_for_start_result(SubscriptionResponse
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
        blaze::RetCode stop();

        /* this function must be called from same thread that called Stop()*/
        blaze::RetCode    await_for_stop_result(SubscriptionResponse
                                                &sbs_stop_result,
                                                ProtocolCode &sbs_stop_protocode,
                                                time_t sec = -1,
                                                long nsec = 0);


    private:
        blaze::RetCode    notify_for_start_stop_result();

        //-----------------------------
        // STATUS SYNCHRO
        //-----------------------------
    public:
        blaze::RetCode    await_for_status_reached_or_outdated(SubscriptionStatus
                                                               test,
                                                               SubscriptionStatus &current,
                                                               time_t sec = -1,
                                                               long nsec = 0);

        //-----------------------------
        // STATUS ASYNCHRO HNDLRS
        //-----------------------------
    public:
        void set_subscription_status_change_handler(subscription_status_change_hndlr
                                                    hndlr, void *ud);

        //-----------------------------
        // EVENT NOTIFY SYNCHRO
        //-----------------------------
    public:
        blaze::RetCode await_for_next_event(subscription_event_int **sbs_evt,
                                            time_t sec = -1,
                                            long nsec = 0);

        blaze::RetCode  ack_event();

        //-----------------------------
        // EVENT NOTIFY ASYNCHRO HNDLRS
        //-----------------------------
    public:
        void set_subscription_event_notify_handler(subscription_evt_notify_hndlr hndlr,
                                                   void *ud);

        //-----------------------------
        // APPLICATIVE HANDLERS
        //-----------------------------
    public:
        virtual void on_start();
        virtual void on_stop();
        virtual void on_event(subscription_event_int &sbs_evt);

        //-----------------------------
        // AUTHORIZE EVENT (SERVER)
        //-----------------------------
        virtual blaze::RetCode accept_event(subscription_event_int *sbs_evt);

        //-----------------------------
        // SEND
        //-----------------------------
    private:
        blaze::RetCode send_start_request();
        blaze::RetCode send_start_response();
        blaze::RetCode send_event(const subscription_event_int *sbs_evt);
        blaze::RetCode send_event_ack();

        //-----------------------------
        // submit_live_event  - SERVER ONLY
        //-----------------------------
        /***********************************
        Note:
        In the scope of this function evt is live. This because for this evt
        this function is called for all subscriptions by the same sbs thread.
        When evt must be stored or send to the selector thread an adoption is needed.
        ***********************************/
    private:
        blaze::RetCode submit_live_event(subscription_event_int *sbs_evt);

        //-----------------------------
        // RECEIVE
        //-----------------------------
    private:
        /*Client only*/
        blaze::RetCode receive_event(const blz_hdr_rec *pkt_hdr,
                                     blaze::grow_byte_buffer *pkt_body,
                                     subscription_event_int *sbs_evt);

        /*Server only*/
        blaze::RetCode receive_event_ack(const blz_hdr_rec *pkt_hdr);

        //-----------------------------
        // Sbs Initial Query  - SERVER ONLY
        //-----------------------------
    private:
        blaze::RetCode execute_initial_query();
        blaze::RetCode safe_submit_dwnl_event();
        blaze::RetCode submit_dwnl_event();
        void release_initial_query();

        //-----------------------------
        // STATUS
        //-----------------------------
    public:
        SubscriptionStatus  status();

        blaze::RetCode    set_req_sent();
        blaze::RetCode    set_started();
        blaze::RetCode    set_stopped();
        blaze::RetCode    set_released();
        blaze::RetCode    set_error();

    private:
        blaze::RetCode    set_status(SubscriptionStatus status);

        //-----------------------------
        // EVT INTER
        // All these func MUST be called inside a synch region.
        //-----------------------------
    private:
        blaze::RetCode    consume_event(subscription_event_int **sbs_evt);
        blaze::RetCode    ack_event_priv();
        blaze::RetCode    evt_reset();
        blaze::RetCode    evt_ready();
        blaze::RetCode    evt_to_ack();

        //-----------------------------
        // REP
        //-----------------------------
    private:

        peer_int                &peer_; // associated peer.
        connection_int          &conn_; // underlying connection.
        const entity_manager    &bem_;
        unsigned int            sbsid_;
        unsigned int            reqid_; //set by client
        SubscriptionStatus      status_;

        //--asynch status
        subscription_status_change_hndlr ssc_hndl_;
        void    *ssc_hndl_ud_;

        bool    start_stop_evt_occur_;

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
        ProtocolCode                last_blzcod_;    //set by srv

        //--asynch evt notify
        subscription_evt_notify_hndlr   sen_hndl_;
        void                            *sen_hndl_ud_;

    private:
        BLZ_SBS_Evt                 cli_evt_sts_;
        blaze::blocking_queue       cli_evt_q_;
        subscription_event_int      *cli_last_evt_;

        //****-----***********
        // SRV SBS MNG REP BG
        //****-----***********
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
        blaze::blocking_queue srv_sbs_evt_glob_q_;
        //[classkeyvalue] --> [classkeyvalue_instance_event_queue]
        blaze::hash_map srv_sbs_classkey_evt_q_hm_;

        blaze::RetCode store_sbs_evt_srv_asynch(subscription_event_int *evt);
        /**returns OK if a new event is available. KO if not.*/
        blaze::RetCode consume_sbs_evt_srv_asynch(subscription_event_int **evt_out);

        //dedicated lock to synchro *asyncro sbs rep*
        mutable pthread_rwlock_t lock_srv_sbs_rep_asynch_;
        /**Asyncro sbs rep - end*/

        //initial query rep
    private:
        persistence_query_int  *initial_query_;
        bool                    initial_query_ended_;
        //****-----***********
        // SRV SBS MNG REP END
        //****-----***********

    private:
        mutable blaze::synch_monitor mon_;

    protected:
        static nclass_logger *log_;
};

}

#endif
