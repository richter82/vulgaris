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
#include "vlg_memory.h"

namespace vlg {

/** @brief subscription_event class.
*/
class subscription_event_impl_pub;
class subscription_event : public vlg::collectable {

    public:
        explicit subscription_event(subscription &sbs);
        ~subscription_event();

        virtual vlg::collector &get_collector();

    public:
        subscription            &get_subscription();
        unsigned int            get_id();
        SubscriptionEventType   get_event_type();
        ProtocolCode            get_proto_code();
        unsigned int            get_timestamp_0();
        unsigned int            get_timestamp_1();
        Action                  get_action();
        nclass                  *get_object();

    public:
        subscription_event_impl_pub *get_opaque();

    private:
        subscription_event_impl_pub *impl_;
};

/** @brief subscription_factory class.
*/
class subscription_factory {
    public:
        static subscription_impl    *subscription_impl_factory_f(connection_impl &conn,
                                                                 void *ud);

    public:
        subscription_factory();
        virtual ~subscription_factory();

    public:
        virtual subscription    &make_subscription(connection &conn);

    public:
        static subscription_factory &default_factory();
};

/** @brief subscription class.
*/
class subscription_impl_pub;
class subscription : public vlg::collectable {
    public:
        typedef void (*status_change)(subscription &sbs,
                                      SubscriptionStatus status,
                                      void *ud);

        typedef void (*event_notify)(subscription &sbs,
                                     subscription_event &sbs_evt,
                                     void *ud);

    public:
        explicit subscription();
        virtual ~subscription();

        virtual vlg::collector &get_collector();

    public:
        RetCode bind(connection &conn);

    public:
        connection                  &get_connection();
        unsigned int                get_id();
        unsigned int                get_nclass_id();
        SubscriptionType            get_type()              const;
        SubscriptionMode            get_mode()              const;
        SubscriptionFlowType        get_flow_type()         const;
        SubscriptionDownloadType    get_download_type()     const;
        Encode                      get_nclass_encode()     const;
        unsigned int                get_open_timestamp_0()  const;
        unsigned int                get_open_timestamp_1()  const;
        bool                        is_initial_query_ended();

    public:
        void    set_nclass_id(unsigned int nclass_id);
        void    set_type(SubscriptionType sbs_type);
        void    set_mode(SubscriptionMode sbs_mode);
        void    set_flow_type(SubscriptionFlowType sbs_flow_type);
        void    set_download_type(SubscriptionDownloadType sbs_dwnl_type);
        void    set_nclass_encode(Encode nclass_encode);
        void    set_open_timestamp_0(unsigned int ts0);
        void    set_open_timestamp_1(unsigned int ts1);

    public:
        SubscriptionStatus get_status() const;

    public:
        RetCode
        await_for_status_reached_or_outdated(SubscriptionStatus test,
                                             SubscriptionStatus &current,
                                             time_t sec = -1,
                                             long nsec = 0);

    public:
        void set_status_change_handler(status_change handler,
                                       void *ud);

    public:
        void set_event_notify_handler(event_notify handler,
                                      void *ud);

    public:
        RetCode start();

        RetCode start(SubscriptionType           sbs_type,
                      SubscriptionMode           sbs_mode,
                      SubscriptionFlowType       sbs_flow_type,
                      SubscriptionDownloadType   sbs_dwnl_type,
                      Encode                     class_encode,
                      unsigned int               nclass_id,
                      unsigned int               open_timestamp_0 = 0,
                      unsigned int               open_timestamp_1 = 0);

        RetCode await_for_start_result(SubscriptionResponse     &start_result,
                                       ProtocolCode             &start_protocode,
                                       time_t                   sec = -1,
                                       long                     nsec = 0);

        RetCode stop();

        /* this function must be called from same thread that called stop()*/
        RetCode await_for_stop_result(SubscriptionResponse      &stop_result,
                                      ProtocolCode              &stop_protocode,
                                      time_t                    sec = -1,
                                      long                      nsec = 0);

    public:
        virtual void on_start();
        virtual void on_stop();
        virtual void on_event(subscription_event &sbs_evt);

    public:
        virtual RetCode on_event_accept(const subscription_event &sbs_evt);

    public:
        subscription_impl   *get_opaque();
        void                set_opaque(subscription_impl *sbs);

    private:
        subscription_impl_pub *impl_;

    protected:
        static nclass_logger *log_;
};

}

#endif
