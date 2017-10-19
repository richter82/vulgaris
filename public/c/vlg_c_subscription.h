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

#ifndef VLG_C_SUBSCRIPTION_H_
#define VLG_C_SUBSCRIPTION_H_
#include "vlg.h"

#if defined(__cplusplus)
using namespace vlg;
extern "C" {
#endif

subscription_wr subscription_event_get_subscription(subscription_event_wr sev);

unsigned int subscription_event_get_id(subscription_event_wr sev);

SubscriptionEventType
subscription_event_get_event_type(subscription_event_wr sev);

ProtocolCode subscription_event_get_proto_code(subscription_event_wr
                                               sev);

unsigned int subscription_event_get_timestamp_0(subscription_event_wr sev);

unsigned int subscription_event_get_timestamp_1(subscription_event_wr sev);

Action  subscription_event_get_action(subscription_event_wr sev);

net_class_wr  subscription_event_get_object(subscription_event_wr sev);

typedef void(*subscription_status_change_wr)(subscription_wr sbs,
                                             SubscriptionStatus status,
                                             void *ud);

typedef void(*subscription_event_notify_wr)(subscription_wr sbs,
                                            subscription_event_wr sbs_evt,
                                            void *ud);

subscription_wr subscription_create(void);
void            subscription_destroy(subscription_wr sbs);

RetCode subscription_bind(subscription_wr sbs,
                          connection_wr conn);

connection_wr subscription_get_connection(subscription_wr sbs);

unsigned int subscription_get_id(subscription_wr sbs);

unsigned int subscription_get_subscription_class_id(subscription_wr sbs);

SubscriptionType   subscription_get_subscription_type(subscription_wr sbs);

SubscriptionMode   subscription_get_subscription_mode(subscription_wr sbs);

SubscriptionFlowType
subscription_get_subscription_flow_type(subscription_wr sbs);

SubscriptionDownloadType
subscription_get_subscription_download_type(subscription_wr sbs);

Encode subscription_get_subscription_class_encode(subscription_wr sbs);

unsigned int subscription_get_open_timestamp_0(subscription_wr sbs);

unsigned int subscription_get_open_timestamp_1(subscription_wr sbs);

int subscription_is_initial_query_ended(subscription_wr sbs);

void subscription_set_subscription_class_id(subscription_wr sbs,
                                            unsigned int nclass_id);

void subscription_set_subscription_type(subscription_wr sbs,
                                        SubscriptionType sbs_type);

void subscription_set_subscription_mode(subscription_wr sbs,
                                        SubscriptionMode sbs_mode);

void subscription_set_subscription_flow_type(subscription_wr sbs,
                                             SubscriptionFlowType sbs_flow_type);

void subscription_set_subscription_download_type(subscription_wr sbs,
                                                 SubscriptionDownloadType sbs_dwnl_type);

void subscription_set_class_encode(subscription_wr sbs,
                                   Encode class_encode);

void subscription_set_open_timestamp_0(subscription_wr sbs,
                                       unsigned int ts0);

void subscription_set_open_timestamp_1(subscription_wr sbs,
                                       unsigned int ts1);

RetCode
subscription_await_for_status_reached_or_outdated(subscription_wr sbs,
                                                  SubscriptionStatus test,
                                                  SubscriptionStatus *current,
                                                  time_t sec,
                                                  long nsec);

void
subscription_set_status_change_handler(subscription_wr sbs,
                                       subscription_status_change_wr handler,
                                       void *ud);

void
subscription_set_event_notify_handler(subscription_wr sbs,
                                      subscription_event_notify_wr handler,
                                      void *ud);

RetCode subscription_start(subscription_wr sbs);

RetCode subscription_start_full(subscription_wr             sbs,
                                SubscriptionType            sbs_type,
                                SubscriptionMode            sbs_mode,
                                SubscriptionFlowType        sbs_flow_type,
                                SubscriptionDownloadType    sbs_dwnl_type,
                                Encode                      class_encode,
                                unsigned int                nclass_id,
                                unsigned int                open_timestamp_0,
                                unsigned int                open_timestamp_1);

/* this function must be called from same thread that called start()*/
RetCode subscription_await_for_start_result(subscription_wr sbs,
                                            SubscriptionResponse *start_result,
                                            ProtocolCode *start_protocode,
                                            time_t sec,
                                            long nsec);

RetCode subscription_stop(subscription_wr sbs);

/* this function must be called from same thread that called stop()*/
RetCode subscription_await_for_stop_result(subscription_wr sbs,
                                           SubscriptionResponse *stop_result,
                                           ProtocolCode *stop_protocode,
                                           time_t sec,
                                           long nsec);

#if defined(__cplusplus)
}
#endif

#endif
