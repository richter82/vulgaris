/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
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

incoming_subscription::incoming_subscription(std::shared_ptr<incoming_connection> &conn) :
    impl_(new incoming_subscription_impl(*this, conn))
{
    CTOR_TRC
}

incoming_subscription::~incoming_subscription()
{
    DTOR_TRC
}

incoming_connection &incoming_subscription::get_connection()
{
    return *impl_->conn_sh_;
}

unsigned int incoming_subscription::get_id()
{
    return impl_->sbsid_;
}

unsigned int incoming_subscription::get_nclass_id()
{
    return impl_->nclassid_;
}

SubscriptionType incoming_subscription::get_type() const
{
    return impl_->sbstyp_;
}

SubscriptionMode incoming_subscription::get_mode() const
{
    return impl_->sbsmod_;
}

SubscriptionFlowType incoming_subscription::get_flow_type() const
{
    return impl_->flotyp_;
}

SubscriptionDownloadType incoming_subscription::get_download_type() const
{
    return impl_->dwltyp_;
}

Encode incoming_subscription::get_nclass_encode() const
{
    return impl_->enctyp_;
}

unsigned int incoming_subscription::get_open_timestamp_0() const
{
    return impl_->open_tmstp0_;
}

unsigned int incoming_subscription::get_open_timestamp_1() const
{
    return impl_->open_tmstp1_;
}

bool incoming_subscription::is_initial_query_ended()
{
    return impl_->initial_query_ended_;
}

void incoming_subscription::set_nclass_id(unsigned int nclass_id)
{
    impl_->nclassid_ = nclass_id;
}

void incoming_subscription::set_type(SubscriptionType sbs_type)
{
    impl_->sbstyp_ = sbs_type;
}

void incoming_subscription::set_mode(SubscriptionMode sbs_mode)
{
    impl_->sbsmod_ = sbs_mode;
}

void incoming_subscription::set_flow_type(SubscriptionFlowType
                                          sbs_flow_type)
{
    impl_->flotyp_ = sbs_flow_type;
}

void incoming_subscription::set_download_type(SubscriptionDownloadType
                                              sbs_dwnl_type)
{
    impl_->dwltyp_ = sbs_dwnl_type;
}

void incoming_subscription::set_nclass_encode(Encode nclass_encode)
{
    impl_->enctyp_ = nclass_encode;
}

void incoming_subscription::set_open_timestamp_0(unsigned int ts0)
{
    impl_->open_tmstp0_ = ts0;
}

void incoming_subscription::set_open_timestamp_1(unsigned int ts1)
{
    impl_->open_tmstp1_ = ts1;
}

SubscriptionStatus incoming_subscription::get_status() const
{
    return impl_->status_;
}

RetCode incoming_subscription::await_for_status_reached(SubscriptionStatus test,
                                                        SubscriptionStatus &current,
                                                        time_t sec,
                                                        long nsec)
{
    return impl_->await_for_status_reached(test,
                                           current,
                                           sec,
                                           nsec);
}

RetCode incoming_subscription::stop()
{
    return impl_->stop();
}

RetCode incoming_subscription::await_for_stop_result(SubscriptionResponse
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

void incoming_subscription::on_status_change(SubscriptionStatus current)
{}

void incoming_subscription::on_stop()
{}

RetCode incoming_subscription::accept_distribution(const subscription_event &sbs_evt)
{
    return RetCode_OK;
}

}

namespace vlg {

outgoing_subscription::outgoing_subscription() : impl_(new outgoing_subscription_impl(*this))
{
    CTOR_TRC
}

outgoing_subscription::~outgoing_subscription()
{
    DTOR_TRC
}

RetCode outgoing_subscription::bind(outgoing_connection &conn)
{
    impl_->conn_ = conn.impl_.get();
    return RetCode_OK;
}

outgoing_connection &outgoing_subscription::get_connection()
{
    return *impl_->conn_->opubl_;
}

unsigned int outgoing_subscription::get_id()
{
    return impl_->sbsid_;
}

unsigned int outgoing_subscription::get_nclass_id()
{
    return impl_->nclassid_;
}

SubscriptionType outgoing_subscription::get_type() const
{
    return impl_->sbstyp_;
}

SubscriptionMode outgoing_subscription::get_mode() const
{
    return impl_->sbsmod_;
}

SubscriptionFlowType outgoing_subscription::get_flow_type() const
{
    return impl_->flotyp_;
}

SubscriptionDownloadType outgoing_subscription::get_download_type()
const
{
    return impl_->dwltyp_;
}

Encode outgoing_subscription::get_nclass_encode() const
{
    return impl_->enctyp_;
}

unsigned int outgoing_subscription::get_open_timestamp_0() const
{
    return impl_->open_tmstp0_;
}

unsigned int outgoing_subscription::get_open_timestamp_1() const
{
    return impl_->open_tmstp1_;
}

void outgoing_subscription::set_nclass_id(unsigned int nclass_id)
{
    impl_->nclassid_ = nclass_id;
}

void outgoing_subscription::set_type(SubscriptionType sbs_type)
{
    impl_->sbstyp_ = sbs_type;
}

void outgoing_subscription::set_mode(SubscriptionMode sbs_mode)
{
    impl_->sbsmod_ = sbs_mode;
}

void outgoing_subscription::set_flow_type(SubscriptionFlowType
                                          sbs_flow_type)
{
    impl_->flotyp_ = sbs_flow_type;
}

void outgoing_subscription::set_download_type(SubscriptionDownloadType
                                              sbs_dwnl_type)
{
    impl_->dwltyp_ = sbs_dwnl_type;
}

void outgoing_subscription::set_nclass_encode(Encode nclass_encode)
{
    impl_->enctyp_ = nclass_encode;
}

void outgoing_subscription::set_open_timestamp_0(unsigned int ts0)
{
    impl_->open_tmstp0_ = ts0;
}

void outgoing_subscription::set_open_timestamp_1(unsigned int ts1)
{
    impl_->open_tmstp1_ = ts1;
}

SubscriptionStatus outgoing_subscription::get_status() const
{
    return impl_->status_;
}

RetCode outgoing_subscription::await_for_status_reached(SubscriptionStatus test,
                                                        SubscriptionStatus &current,
                                                        time_t sec,
                                                        long nsec)
{
    return impl_->await_for_status_reached(test,
                                           current,
                                           sec,
                                           nsec);
}

RetCode outgoing_subscription::start()
{
    return impl_->start();
}

RetCode outgoing_subscription::start(SubscriptionType sbs_type,
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

RetCode outgoing_subscription::await_for_start_result(SubscriptionResponse
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

RetCode outgoing_subscription::stop()
{
    return impl_->stop();
}

RetCode outgoing_subscription::await_for_stop_result(SubscriptionResponse
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

void outgoing_subscription::on_status_change(SubscriptionStatus current)
{}

void outgoing_subscription::on_start()
{}

void outgoing_subscription::on_stop()
{}

void outgoing_subscription::on_incoming_event(std::unique_ptr<subscription_event> &sbs_evt)
{}

}
