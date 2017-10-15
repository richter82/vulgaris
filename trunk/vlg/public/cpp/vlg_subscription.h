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

#ifndef BLZ_CPP_SUBSCRIPTION_H_
#define BLZ_CPP_SUBSCRIPTION_H_
#include "blaze_memory.h"

namespace blaze {

/** @brief subscription_event class.
*/
class subscription_event_impl;
class subscription_event : public blaze::collectable {

    public:
        explicit subscription_event();
        ~subscription_event();

        virtual blaze::collector &get_collector();

    public:
        subscription            *get_subscription();
        unsigned int            get_id();
        SubscriptionEventType   get_event_type();
        ProtocolCode            get_proto_code();
        unsigned int            get_timestamp_0(); //part. dwnl.
        unsigned int            get_timestamp_1(); //part. dwnl.
        Action                  get_action();
        nclass               *get_object();

    public:
        subscription_event_impl *get_opaque();

    private:
        subscription_event_impl *impl_;
};

/** @brief subscription_factory class.
*/
class subscription_factory {
    public:
        static subscription_int     *sbs_factory_int_f(connection_int &conn,
                                                       void *ud);

    public:
        subscription_factory();
        virtual ~subscription_factory();

    public:
        virtual subscription        *new_subscription(connection &conn);

    public:
        static subscription_factory *default_subscription_factory();
};

/** @brief subscription class.
*/
class subscription_impl;
class subscription : public blaze::collectable {
    public:
        typedef void (*subscription_status_change)(subscription &sbs,
                                                   SubscriptionStatus status,
                                                   void *ud);

        typedef void (*subscription_event_notify)(subscription &sbs,
                                                  subscription_event &sbs_evt,
                                                  void *ud);

    public:
        explicit subscription();
        virtual ~subscription();

        virtual blaze::collector &get_collector();

    public:
        blaze::RetCode bind(connection &conn);

    public:
        connection                  *get_connection();
        unsigned int                get_id();
        unsigned int                get_subscription_class_id();
        SubscriptionType            get_subscription_type()             const;
        SubscriptionMode            get_subscription_mode()             const;
        SubscriptionFlowType        get_subscription_flow_type()        const;
        SubscriptionDownloadType    get_subscription_download_type()    const;
        Encode                      get_subscription_class_encode()     const;
        unsigned int                get_open_timestamp_0()              const;
        unsigned int                get_open_timestamp_1()              const;
        bool                        is_initial_query_ended();

    public:
        void    set_subscription_class_id(unsigned int nclass_id);
        void    set_subscription_type(SubscriptionType sbs_type);
        void    set_subscription_mode(SubscriptionMode sbs_mode);

        void
        set_subscription_flow_type(SubscriptionFlowType sbs_flow_type);

        void
        set_subscription_download_type(SubscriptionDownloadType
                                       sbs_dwnl_type);

        void    set_class_encode(Encode class_encode);
        void    set_open_timestamp_0(unsigned int ts0);
        void    set_open_timestamp_1(unsigned int ts1);

    public:
        blaze::RetCode
        await_for_status_reached_or_outdated(SubscriptionStatus test,
                                             SubscriptionStatus &current,
                                             time_t sec = -1,
                                             long nsec = 0);

    public:
        void set_status_change_handler(subscription_status_change handler,
                                       void *ud);

    public:
        void set_event_notify_handler(subscription_event_notify handler,
                                      void *ud);

    public:
        blaze::RetCode start();

        blaze::RetCode start(SubscriptionType           sbs_type,
                             SubscriptionMode           sbs_mode,
                             SubscriptionFlowType       sbs_flow_type,
                             SubscriptionDownloadType   sbs_dwnl_type,
                             Encode                     class_encode,
                             unsigned int               nclass_id,
                             unsigned int               open_timestamp_0 = 0,
                             unsigned int               open_timestamp_1 = 0);

        /* this function must be called from same thread that called start()*/
        blaze::RetCode
        await_for_start_result(SubscriptionResponse     &start_result,
                               ProtocolCode             &start_protocode,
                               time_t                   sec = -1,
                               long                     nsec = 0);

        blaze::RetCode stop();

        /* this function must be called from same thread that called stop()*/
        blaze::RetCode
        await_for_stop_result(SubscriptionResponse      &stop_result,
                              ProtocolCode              &stop_protocode,
                              time_t                    sec = -1,
                              long                      nsec = 0);

    public:
        virtual void on_start();
        virtual void on_stop();
        virtual void on_event(subscription_event &sbs_evt);

    public:
        virtual blaze::RetCode
        on_event_accept(const subscription_event &sbs_evt);

    public:
        subscription_int *get_internal();
        void              set_internal(subscription_int *sbs);

    private:
        subscription_impl *impl_;

    protected:
        static nclass_logger *log_;
};

}

#endif
