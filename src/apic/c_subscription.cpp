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

#include "vlg_c_subscription.h"
#include "vlg_subscription.h"

namespace vlg {

class c_subscription : public subscription {
    private:
        static void subscription_status_change_c_subscription(subscription &subscription,
                                                              SubscriptionStatus status, void *ud) {
            c_subscription &self = static_cast<c_subscription &>(subscription);
            self.ssc_wr_((subscription_wr)&subscription, status, self.ssc_ud_);
        }

        static void subscription_event_notify_c_subscription(subscription &subscription,
                                                             subscription_event &sbs_evt, void *ud) {
            c_subscription &self = static_cast<c_subscription &>(subscription);
            self.sen_wr_((subscription_wr)&subscription, (subscription_event_wr)&sbs_evt,
                         self.sen_ud_);
        }

    public:
        c_subscription() : sen_wr_(nullptr), ssc_wr_(nullptr), ssc_ud_(nullptr), sen_ud_(nullptr) {}

        subscription_event_notify_wr Sen_wr() const {
            return sen_wr_;
        }
        void Sen_wr(subscription_event_notify_wr val) {
            sen_wr_ = val;
        }
        subscription_status_change_wr Ssc_wr() const {
            return ssc_wr_;
        }
        void Ssc_wr(subscription_status_change_wr val) {
            ssc_wr_ = val;
        }
        void *Ssc_ud() const {
            return ssc_ud_;
        }
        void Ssc_ud(void *val) {
            ssc_ud_ = val;
            //set_status_change_handler(subscription_status_change_c_subscription, ssc_ud_);
        }
        void *Sen_ud() const {
            return sen_ud_;
        }
        void Sen_ud(void *val) {
            sen_ud_ = val;
            //set_event_notify_handler(subscription_event_notify_c_subscription, sen_ud_);
        }

    private:
        subscription_event_notify_wr sen_wr_;
        subscription_status_change_wr ssc_wr_;
        void *ssc_ud_;
        void *sen_ud_;
};

extern "C" {

    unsigned int subscription_event_get_id(subscription_event_wr sev)
    {
        return static_cast<subscription_event *>(sev)->get_id();
    }

    SubscriptionEventType subscription_event_get_event_type(
        subscription_event_wr sev)
    {
        return static_cast<subscription_event *>(sev)->get_event_type();
    }

    ProtocolCode subscription_event_get_proto_code(subscription_event_wr sev)
    {
        return static_cast<subscription_event *>(sev)->get_proto_code();
    }

    unsigned int subscription_event_get_timestamp_0(subscription_event_wr sev)
    {
        return static_cast<subscription_event *>(sev)->get_timestamp_0();
    }

    unsigned int subscription_event_get_timestamp_1(subscription_event_wr sev)
    {
        return static_cast<subscription_event *>(sev)->get_timestamp_1();
    }

    Action subscription_event_get_action(subscription_event_wr sev)
    {
        return static_cast<subscription_event *>(sev)->get_action();
    }

    nclass_wr subscription_event_get_object(subscription_event_wr sev)
    {
        //@FIXME
        //return static_cast<subscription_event *>(sev)->get_data();
        return nullptr;
    }

    subscription_wr subscription_create()
    {
        return new c_subscription();
    }

    void subscription_destroy(subscription_wr subscription)
    {
        delete static_cast<vlg::subscription *>(subscription);
    }

    RetCode subscription_bind(subscription_wr subscription, connection_wr conn)
    {
        return static_cast<vlg::subscription *>(subscription)->bind(*(connection *)conn);
    }

    connection_wr subscription_get_connection(subscription_wr subscription)
    {
        return &static_cast<vlg::subscription *>(subscription)->get_connection();
    }

    unsigned int subscription_get_id(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->get_id();
    }

    unsigned int subscription_get_subscription_class_id(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->get_nclass_id();
    }

    SubscriptionType subscription_get_subscription_type(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->get_type();
    }

    SubscriptionMode subscription_get_subscription_mode(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->get_mode();
    }

    SubscriptionFlowType subscription_get_subscription_flow_type(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->get_flow_type();
    }

    SubscriptionDownloadType subscription_get_subscription_download_type(
        subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->get_download_type();
    }

    Encode subscription_get_subscription_class_encode(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->get_nclass_encode();
    }

    unsigned int subscription_get_open_timestamp_0(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->get_open_timestamp_0();
    }

    unsigned int subscription_get_open_timestamp_1(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->get_open_timestamp_1();
    }

    int subscription_is_initial_query_ended(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->is_initial_query_ended() ? 1 : 0;
    }

    void subscription_set_subscription_class_id(subscription_wr subscription,
                                                unsigned int nclass_id)
    {
        static_cast<vlg::subscription *>(subscription)->set_nclass_id(nclass_id);
    }

    void subscription_set_subscription_type(subscription_wr subscription,
                                            SubscriptionType sbs_type)
    {
        static_cast<vlg::subscription *>(subscription)->set_type(sbs_type);
    }

    void subscription_set_subscription_mode(subscription_wr subscription,
                                            SubscriptionMode sbs_mode)
    {
        static_cast<vlg::subscription *>(subscription)->set_mode(sbs_mode);
    }

    void subscription_set_subscription_flow_type(subscription_wr subscription,
                                                 SubscriptionFlowType sbs_flow_type)
    {
        static_cast<vlg::subscription *>(subscription)->set_flow_type(sbs_flow_type);
    }

    void subscription_set_subscription_download_type(subscription_wr subscription,
                                                     SubscriptionDownloadType sbs_dwnl_type)
    {
        static_cast<vlg::subscription *>(subscription)->set_download_type(sbs_dwnl_type);
    }

    void subscription_set_class_encode(subscription_wr subscription, Encode nclass_encode)
    {
        static_cast<vlg::subscription *>(subscription)->set_nclass_encode(nclass_encode);
    }

    void subscription_set_open_timestamp_0(subscription_wr subscription, unsigned int ts0)
    {
        static_cast<vlg::subscription *>(subscription)->set_open_timestamp_0(ts0);
    }

    void subscription_set_open_timestamp_1(subscription_wr subscription, unsigned int ts1)
    {
        static_cast<vlg::subscription *>(subscription)->set_open_timestamp_0(ts1);
    }

    RetCode subscription_await_for_status_reached_or_outdated(subscription_wr subscription,
                                                              SubscriptionStatus test,
                                                              SubscriptionStatus *current,
                                                              time_t sec,
                                                              long nsec)
    {
        return static_cast<vlg::subscription *>(subscription)->await_for_status_reached_or_outdated(test,
                                                                                                    *current,
                                                                                                    sec,
                                                                                                    nsec);
    }

    void subscription_set_status_change_handler(subscription_wr subscription,
                                                subscription_status_change_wr handler,
                                                void *ud)
    {
        static_cast<c_subscription *>(subscription)->Ssc_wr(handler);
        static_cast<c_subscription *>(subscription)->Ssc_ud(ud);
    }

    void subscription_set_event_notify_handler(subscription_wr subscription,
                                               subscription_event_notify_wr handler,
                                               void *ud)
    {
        static_cast<c_subscription *>(subscription)->Sen_wr(handler);
        static_cast<c_subscription *>(subscription)->Sen_ud(ud);
    }

    RetCode subscription_start(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->start();
    }

    RetCode subscription_start_full(subscription_wr subscription,
                                    SubscriptionType sbs_type,
                                    SubscriptionMode sbs_mode,
                                    SubscriptionFlowType sbs_flow_type,
                                    SubscriptionDownloadType sbs_dwnl_type,
                                    Encode nclass_encode,
                                    unsigned int nclass_id,
                                    unsigned int open_timestamp_0,
                                    unsigned int open_timestamp_1)
    {
        return static_cast<vlg::subscription *>(subscription)->start(sbs_type,
                                                                     sbs_mode,
                                                                     sbs_flow_type,
                                                                     sbs_dwnl_type,
                                                                     nclass_encode,
                                                                     nclass_id,
                                                                     open_timestamp_0,
                                                                     open_timestamp_1);
    }

    RetCode subscription_await_for_start_result(subscription_wr subscription,
                                                SubscriptionResponse *start_result,
                                                ProtocolCode *start_protocode,
                                                time_t sec,
                                                long nsec)
    {
        return static_cast<vlg::subscription *>(subscription)->await_for_start_result(*start_result,
                                                                                      *start_protocode,
                                                                                      sec,
                                                                                      nsec);
    }

    RetCode subscription_stop(subscription_wr subscription)
    {
        return static_cast<vlg::subscription *>(subscription)->stop();
    }

    RetCode subscription_await_for_stop_result(subscription_wr subscription,
                                               SubscriptionResponse *stop_result,
                                               ProtocolCode *stop_protocode,
                                               time_t sec,
                                               long nsec)
    {
        return static_cast<vlg::subscription *>(subscription)->await_for_stop_result(*stop_result,
                                                                                     *stop_protocode,
                                                                                     sec,
                                                                                     nsec);
    }

}
}
