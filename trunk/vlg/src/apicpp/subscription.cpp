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

#include "vlg_connection.h"
#include "vlg_subscription.h"
#include "vlg/conn_impl.h"
#include "vlg/sbs_impl.h"

namespace vlg {

subscription_event::subscription_event(subscription_event_impl &impl) : impl_(&impl)
{}

subscription_event::~subscription_event()
{}

unsigned int subscription_event::get_id()
{
    return impl_->sbs_evtid_;
}

SubscriptionEventType subscription_event::get_event_type()
{
    return impl_->sbs_evttype_;
}

ProtocolCode subscription_event::get_proto_code()
{
    return impl_->sbs_protocode_;
}

unsigned int subscription_event::get_timestamp_0()
{
    return impl_->sbs_tmstp0_;
}

unsigned int subscription_event::get_timestamp_1()
{
    return impl_->sbs_tmstp1_;
}

Action subscription_event::get_action()
{
    return impl_->sbs_act_;
}

std::unique_ptr<nclass> &subscription_event::get_data()
{
    return impl_->sbs_data_;
}

subscription::subscription() : impl_(new subscription_impl(*this))
{
    CTOR_TRC
}

subscription::~subscription()
{
    DTOR_TRC
}

RetCode subscription::bind(connection &conn)
{
    impl_->set_connection(conn);
    return RetCode_OK;
}

connection &subscription::get_connection()
{
    return *impl_->conn_;
}

unsigned int subscription::get_id()
{
    return impl_->sbsid_;
}

unsigned int subscription::get_nclass_id()
{
    return impl_->nclassid_;
}

SubscriptionType subscription::get_type() const
{
    return impl_->sbstyp_;
}

SubscriptionMode subscription::get_mode() const
{
    return impl_->sbsmod_;
}

SubscriptionFlowType subscription::get_flow_type() const
{
    return impl_->flotyp_;
}

SubscriptionDownloadType subscription::get_download_type()
const
{
    return impl_->dwltyp_;
}

Encode subscription::get_nclass_encode() const
{
    return impl_->enctyp_;
}

unsigned int subscription::get_open_timestamp_0() const
{
    return impl_->open_tmstp0_;
}

unsigned int subscription::get_open_timestamp_1() const
{
    return impl_->open_tmstp1_;
}

bool subscription::is_initial_query_ended()
{
    return impl_->initial_query_ended_;
}

void subscription::set_nclass_id(unsigned int nclass_id)
{
    impl_->nclassid_ = nclass_id;
}

void subscription::set_type(SubscriptionType sbs_type)
{
    impl_->sbstyp_ = sbs_type;
}

void subscription::set_mode(SubscriptionMode sbs_mode)
{
    impl_->sbsmod_ = sbs_mode;
}

void subscription::set_flow_type(SubscriptionFlowType
                                 sbs_flow_type)
{
    impl_->flotyp_ = sbs_flow_type;
}

void subscription::set_download_type(SubscriptionDownloadType
                                     sbs_dwnl_type)
{
    impl_->dwltyp_ = sbs_dwnl_type;
}

void subscription::set_nclass_encode(Encode nclass_encode)
{
    impl_->enctyp_ = nclass_encode;
}

void subscription::set_open_timestamp_0(unsigned int ts0)
{
    impl_->open_tmstp0_ = ts0;
}

void subscription::set_open_timestamp_1(unsigned int ts1)
{
    impl_->open_tmstp1_ = ts1;
}

SubscriptionStatus subscription::get_status() const
{
    return impl_->status_;
}

RetCode subscription::await_for_status_reached_or_outdated(SubscriptionStatus test,
                                                           SubscriptionStatus &current,
                                                           time_t sec,
                                                           long nsec)
{
    return impl_->await_for_status_reached_or_outdated(test,
                                                       current,
                                                       sec,
                                                       nsec);
}

RetCode subscription::start()
{
    return impl_->start();
}

RetCode subscription::start(SubscriptionType sbs_type,
                            SubscriptionMode sbs_mode,
                            SubscriptionFlowType sbs_flow_type,
                            SubscriptionDownloadType sbs_dwnl_type,
                            Encode nclass_encode,
                            unsigned int nclass_id,
                            unsigned int open_timestamp_0,
                            unsigned int open_timestamp_1)
{
    return impl_->start(sbs_type,
                        sbs_mode,
                        sbs_flow_type,
                        sbs_dwnl_type,
                        nclass_encode, nclass_id,
                        open_timestamp_0,
                        open_timestamp_1);
}

RetCode subscription::await_for_start_result(SubscriptionResponse
                                             &sbs_start_result,
                                             ProtocolCode &sbs_start_protocode,
                                             time_t sec,
                                             long nsec)
{
    return impl_->await_for_start_result(sbs_start_result,
                                         sbs_start_protocode,
                                         sec,
                                         nsec);
}

RetCode subscription::stop()
{
    return impl_->stop();
}

RetCode subscription::await_for_stop_result(SubscriptionResponse
                                            &sbs_stop_result,
                                            ProtocolCode &sbs_stop_protocode,
                                            time_t sec,
                                            long nsec)
{
    return impl_->await_for_stop_result(sbs_stop_result,
                                        sbs_stop_protocode,
                                        sec,
                                        nsec);
}

void subscription::on_status_change(SubscriptionStatus current)
{}

void subscription::on_start()
{}

void subscription::on_stop()
{}

void subscription::on_incoming_event(std::unique_ptr<subscription_event> &sbs_evt)
{}

RetCode subscription::accept_distribution(const subscription_event &sbs_evt)
{
    return vlg::RetCode_OK;
}

}
