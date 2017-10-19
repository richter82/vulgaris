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
#include "vlg/vlg_connection_impl.h"
#include "vlg/vlg_subscription_impl.h"

namespace vlg {

extern vlg::synch_hash_map &int_publ_peer_map();
extern vlg::synch_hash_map &int_publ_srv_conn_map();

vlg::synch_hash_map *int_publ_srv_sbs_map_ = NULL;  //int --> publ
vlg::synch_hash_map &int_publ_srv_sbs_map()
{
    if(int_publ_srv_sbs_map_) {
        return *int_publ_srv_sbs_map_;
    }
    if(!(int_publ_srv_sbs_map_ = new vlg::synch_hash_map(
        vlg::sngl_ptr_obj_mng(),
        vlg::sngl_ptr_obj_mng()))) {
        EXIT_ACTION("failed creating int_publ_srv_sbs_map_\n")
    }
    int_publ_srv_sbs_map_->init(HM_SIZE_MICRO);
    return *int_publ_srv_sbs_map_;
}

//-----------------------------
// CLASS subscription_event_impl
//-----------------------------
class subscription_event_impl {
    public:
        subscription_event_impl() : sbs_(NULL), int_(NULL) {}
        ~subscription_event_impl() {
            if(int_) {
                vlg::collector &c = int_->get_collector();
                c.release(int_);
            }
        }

        subscription_event_impl *get_sbs_evt() {
            return int_;
        }

        void set_sbs_evt(subscription_event_impl *val) {
            if(int_) {
                vlg::collector &c = int_->get_collector();
                c.release(int_);
                int_ = NULL;
            }
            if((int_ = val)) {
                vlg::collector &c = int_->get_collector();
                c.retain(int_);
            }
        }

        subscription *get_sbs() const {
            return sbs_;
        }

        void set_sbs(subscription *val) {
            sbs_ = val;
        }

    private:
        subscription *sbs_;
        subscription_event_impl *int_;
};

class subscription_event_collector : public vlg::collector {
    public:
        subscription_event_collector() : vlg::collector("subscription_event") {}
};

vlg::collector *inst_subscription_event_collector = NULL;
vlg::collector &get_inst_subscription_event_collector()
{
    if(inst_subscription_event_collector) {
        return *inst_subscription_event_collector;
    }
    if(!(inst_subscription_event_collector = new
    subscription_event_collector())) {
        EXIT_ACTION("failed creating inst_subscription_event_collector\n")
    }
    return *inst_subscription_event_collector;
}

vlg::collector &subscription_event::get_collector()
{
    return get_inst_subscription_event_collector();
}

//-----------------------------
// CLASS subscription_event
//-----------------------------

subscription_event::subscription_event()
{
    impl_ = new subscription_event_impl();
}

subscription_event::~subscription_event()
{
    if(impl_) {
        delete impl_;
    }
}

subscription *subscription_event::get_subscription()
{
    return impl_->get_sbs();
}

unsigned int subscription_event::get_id()
{
    return impl_->get_sbs_evt()->get_evtid();
}

SubscriptionEventType subscription_event::get_event_type()
{
    return impl_->get_sbs_evt()->get_evttype();
}

ProtocolCode subscription_event::get_proto_code()
{
    return impl_->get_sbs_evt()->get_protocode();
}

unsigned int subscription_event::get_timestamp_0()
{
    return impl_->get_sbs_evt()->get_tmstp0();
}

unsigned int subscription_event::get_timestamp_1()
{
    return impl_->get_sbs_evt()->get_tmstp1();
}

Action subscription_event::get_action()
{
    return impl_->get_sbs_evt()->get_act();
}

nclass *subscription_event::get_object()
{
    return impl_->get_sbs_evt()->get_obj();
}

subscription_event_impl *subscription_event::get_opaque()
{
    return impl_;
}

//-----------------------------
// CLASS subscription_impl
//-----------------------------

class subscription_impl {
    private:
        class simpl_subscription_impl : public subscription_impl {
            public:
                simpl_subscription_impl(connection_impl &conn,
                                        subscription &publ)
                    : subscription_impl(conn), publ_(publ) {}

            public:
                virtual void on_start() {
                    publ_.on_start();
                }

                virtual void on_stop() {
                    publ_.on_stop();
                }

                virtual void on_event(subscription_event_impl &sbs_evt_impl) {
                    subscription_event *sbs_evt = new subscription_event();
                    sbs_evt->get_opaque()->set_sbs_evt(&sbs_evt_impl);
                    publ_.on_event(*sbs_evt);
                    delete sbs_evt;
                }

            private:
                subscription &publ_;
        };

        static subscription_impl *vlg_sbs_factory_simpl(connection_impl &connection,
                                                        void *ud) {
            subscription *publ = static_cast<subscription *>(ud);
            return new simpl_subscription_impl(connection, *publ);
        }

        static void subscription_status_change_hndlr_simpl(subscription_impl &sbs,
                                                           SubscriptionStatus status,
                                                           void *ud) {
            subscription_impl *simpl = static_cast<subscription_impl *>(ud);
            if(simpl->ssh_) {
                simpl->ssh_(simpl->publ_, status, simpl->ssh_ud_);
            }
        }

        static void subscription_evt_notify_hndlr_simpl(subscription_impl &sbs,
                                                        subscription_event_impl &sbs_evt_impl,
                                                        void *ud) {
            subscription_impl *simpl = static_cast<subscription_impl *>(ud);
            if(simpl->senh_) {
                subscription_event *sbs_evt = new subscription_event();
                sbs_evt->get_opaque()->set_sbs_evt(&sbs_evt_impl);
                simpl->senh_(simpl->publ_, *sbs_evt, simpl->senh_ud_);
                delete sbs_evt;
            }
        }

    public:
        subscription_impl(subscription &publ)
            : publ_(publ),
              conn_(NULL),
              int_(NULL),
              ssh_(NULL),
              ssh_ud_(NULL),
              senh_(NULL),
              senh_ud_(NULL)  {}

        ~subscription_impl() {
            if(int_ && int_->get_connection().conn_type() == ConnectionType_OUTGOING) {
                if(int_->status() == SubscriptionStatus_REQUEST_SENT ||
                        int_->status() == SubscriptionStatus_STARTED) {
                    ProtocolCode pcode = ProtocolCode_SUCCESS;
                    SubscriptionResponse resp = SubscriptionResponse_UNDEFINED;
                    int_->stop();
                    int_->await_for_stop_result(resp, pcode);
                }
                vlg::collector &c = int_->get_collector();
                c.release(int_);
            }
        }

        subscription_impl *get_sbs() const {
            return int_;
        }
        void set_sbs(subscription_impl *val) {
            int_ = val;
        }

        connection *get_conn() const {
            return conn_;
        }
        void set_conn(connection &val) {
            conn_ = &val;
        }

        subscription::subscription_status_change get_ssh() const {
            return ssh_;
        }
        void set_ssh(subscription::subscription_status_change val) {
            ssh_ = val;
        }

        void *get_ssh_ud() const {
            return ssh_ud_;
        }
        void set_ssh_ud(void *val) {
            ssh_ud_ = val;
        }

        subscription::subscription_event_notify get_senh() const {
            return senh_;
        }
        void set_senh(subscription::subscription_event_notify val) {
            senh_ = val;
        }

        void *get_senh_ud() const {
            return senh_ud_;
        }
        void set_senh_ud(void *val) {
            senh_ud_ = val;
        }

        vlg::RetCode bind_implernal(connection &conn) {
            vlg::RetCode cdrs_res = vlg::RetCode_OK;
            if(conn.get_connection_type() == ConnectionType_OUTGOING) {
                subscription_impl *s_impl = NULL;
                if((cdrs_res = conn.get_implernal()->new_subscription(&s_impl,
                                                                      vlg_sbs_factory_simpl, &publ_)) == vlg::RetCode_OK) {
                    int_ = s_impl;
                    vlg::collector &c = int_->get_collector();
                    c.retain(int_);
                }
            }
            int_->set_subscription_status_change_handler(
                subscription_status_change_hndlr_simpl, this);
            int_->set_subscription_event_notify_handler(subscription_evt_notify_hndlr_simpl,
                                                        this);
            return cdrs_res;
        }

    private:
        subscription &publ_;
        connection *conn_;
        subscription_impl *int_;
        subscription::subscription_status_change ssh_;
        void *ssh_ud_;
        subscription::subscription_event_notify senh_;
        void *senh_ud_;
};

//*************************************
//subscription MEMORY MNGMENT BEGIN
//*************************************

class subscription_collector : public vlg::collector {
    public:
        subscription_collector() : vlg::collector("subscription") {}
};

vlg::collector *inst_subscription_collector = NULL;
vlg::collector &get_inst_subscription_collector()
{
    if(inst_subscription_collector) {
        return *inst_subscription_collector;
    }
    if(!(inst_subscription_collector = new
    subscription_collector())) {
        EXIT_ACTION("failed creating inst_subscription_collector\n")
    }
    return *inst_subscription_collector;
}

vlg::collector &subscription::get_collector()
{
    return get_inst_subscription_collector();
}

//-----------------------------
// CLASS subscription
//-----------------------------

nclass_logger *subscription::log_ = NULL;

subscription::subscription()
{
    log_ = get_nclass_logger("subscription");
    impl_ = new subscription_impl(*this);
    IFLOG(trc(TH_ID, LS_CTR "%s(ptr:%p)", __func__, this))
}

subscription::~subscription()
{
    vlg::collector &c = get_collector();
    if((c.is_instance_collected(this))) {
        IFLOG(cri(TH_ID, LS_DTR "%s(ptr:%p)" D_W_R_COLL LS_EXUNX, __func__, this))
    }
    if(impl_) {
        delete impl_;
    }
    IFLOG(trc(TH_ID, LS_DTR "%s(ptr:%p)", __func__, this))
}

vlg::RetCode subscription::bind(connection &conn)
{
    impl_->set_conn(conn);
    return impl_->bind_implernal(conn);
}

connection *subscription::get_connection()
{
    return impl_->get_conn();
}

unsigned int subscription::get_id()
{
    return impl_->get_sbs()->sbsid();
}

unsigned int subscription::get_subscription_class_id()
{
    return impl_->get_sbs()->nclass_id();
}

SubscriptionType subscription::get_subscription_type() const
{
    return impl_->get_sbs()->get_sbstyp();
}

SubscriptionMode subscription::get_subscription_mode() const
{
    return impl_->get_sbs()->get_sbsmod();
}

SubscriptionFlowType subscription::get_subscription_flow_type() const
{
    return impl_->get_sbs()->get_flotyp();
}

SubscriptionDownloadType subscription::get_subscription_download_type()
const
{
    return impl_->get_sbs()->get_dwltyp();
}

Encode subscription::get_subscription_class_encode() const
{
    return impl_->get_sbs()->get_enctyp();
}

unsigned int subscription::get_open_timestamp_0() const
{
    return impl_->get_sbs()->get_open_tmstp0();
}

unsigned int subscription::get_open_timestamp_1() const
{
    return impl_->get_sbs()->get_open_tmstp1();
}

bool subscription::is_initial_query_ended()
{
    return impl_->get_sbs()->is_initial_query_ended();
}

void subscription::set_subscription_class_id(unsigned int nclass_id)
{
    impl_->get_sbs()->set_nclassid(nclass_id);
}

void subscription::set_subscription_type(SubscriptionType sbs_type)
{
    impl_->get_sbs()->set_sbstyp(sbs_type);
}

void subscription::set_subscription_mode(SubscriptionMode sbs_mode)
{
    impl_->get_sbs()->set_sbsmod(sbs_mode);
}

void subscription::set_subscription_flow_type(SubscriptionFlowType
                                              sbs_flow_type)
{
    impl_->get_sbs()->set_flotyp(sbs_flow_type);
}

void subscription::set_subscription_download_type(SubscriptionDownloadType
                                                  sbs_dwnl_type)
{
    impl_->get_sbs()->set_dwltyp(sbs_dwnl_type);
}

void subscription::set_class_encode(Encode class_encode)
{
    impl_->get_sbs()->set_enctyp(class_encode);
}

void subscription::set_open_timestamp_0(unsigned int ts0)
{
    impl_->get_sbs()->set_open_tmstp0(ts0);
}

void subscription::set_open_timestamp_1(unsigned int ts1)
{
    impl_->get_sbs()->set_open_tmstp1(ts1);
}

vlg::RetCode subscription::await_for_status_reached_or_outdated(
    SubscriptionStatus test,
    SubscriptionStatus &current,
    time_t sec,
    long nsec)
{
    return impl_->get_sbs()->await_for_status_reached_or_outdated(test, current,
                                                                  sec, nsec);
}

void subscription::set_status_change_handler(subscription_status_change handler,
                                             void *ud)
{
    impl_->set_ssh(handler);
    impl_->set_ssh_ud(ud);
}

vlg::RetCode subscription::start()
{
    return impl_->get_sbs()->start();
}

vlg::RetCode subscription::start(SubscriptionType sbs_type,
                                 SubscriptionMode sbs_mode,
                                 SubscriptionFlowType sbs_flow_type,
                                 SubscriptionDownloadType sbs_dwnl_type,
                                 Encode class_encode,
                                 unsigned int nclass_id,
                                 unsigned int open_timestamp_0,
                                 unsigned int open_timestamp_1)
{
    return impl_->get_sbs()->start(sbs_type,
                                   sbs_mode,
                                   sbs_flow_type,
                                   sbs_dwnl_type,
                                   class_encode, nclass_id,
                                   open_timestamp_0,
                                   open_timestamp_1);
}

vlg::RetCode subscription::await_for_start_result(SubscriptionResponse
                                                  &sbs_start_result,
                                                  ProtocolCode &sbs_start_protocode,
                                                  time_t sec,
                                                  long nsec)
{
    return impl_->get_sbs()->await_for_start_result(sbs_start_result,
                                                    sbs_start_protocode,
                                                    sec,
                                                    nsec);
}

vlg::RetCode subscription::stop()
{
    return impl_->get_sbs()->stop();
}

vlg::RetCode subscription::await_for_stop_result(SubscriptionResponse
                                                 &sbs_stop_result,
                                                 ProtocolCode &sbs_stop_protocode,
                                                 time_t sec,
                                                 long nsec)
{
    return impl_->get_sbs()->await_for_stop_result(sbs_stop_result,
                                                   sbs_stop_protocode, sec, nsec);
}

void subscription::set_event_notify_handler(subscription_event_notify handler,
                                            void *ud)
{
    impl_->set_senh(handler);
    impl_->set_senh_ud(ud);
}

void subscription::on_start()
{
}

void subscription::on_stop()
{
}

void subscription::on_event(subscription_event &sbs_evt)
{}

vlg::RetCode subscription::on_event_accept(const subscription_event &sbs_evt)
{
    return vlg::RetCode_OK;
}

subscription_impl *subscription::get_implernal()
{
    return impl_->get_sbs();
}

void subscription::set_implernal(subscription_impl *sbs)
{
    impl_->set_sbs(sbs);
}

//-----------------------------
// CLASS timpl_subscription_impl_server
//-----------------------------
class timpl_subscription_impl_server : public subscription_impl {
    public:
        timpl_subscription_impl_server(connection_impl &conn,
                                       subscription &publ) :
            subscription_impl(conn), publ_(publ) {}
    public:
        virtual ~timpl_subscription_impl_server() {
            void *self = this;
            int_publ_srv_sbs_map().remove(&self, NULL);
            /************************
            RELEASE_ID: SPB_SRV_01
            ************************/
            vlg::collector &c = publ_.get_collector();
            c.release(&publ_);
        }

    public:
        virtual void on_start() {
            publ_.on_start();
        }

        virtual void on_stop() {
            publ_.on_stop();
        }

        virtual vlg::RetCode accept_event(subscription_event_impl *sbs_evt) {
            subscription_event *publ_evt = new subscription_event();
            publ_evt->get_opaque()->set_sbs_evt(sbs_evt);
            vlg::RetCode cdrs_res = publ_.on_event_accept(*publ_evt);
            delete publ_evt;
            return cdrs_res;
        }

    private:
        subscription &publ_;
};

//-----------------------------
// CLASS subscription_factory
//-----------------------------
subscription_factory *default_sbs_factory = NULL;
subscription_factory *subscription_factory::default_subscription_factory()
{
    if(default_sbs_factory  == NULL) {
        default_sbs_factory  = new subscription_factory();
        if(!default_sbs_factory) {
            EXIT_ACTION("failed creating default_sbs_factory\n")
        }
    }
    return default_sbs_factory;
}

subscription_impl *subscription_factory::sbs_factory_impl_f(
    connection_impl &conn,
    void *ud)
{
    subscription_factory *ssf = static_cast<subscription_factory *>(ud);
    connection_impl *connection_impl_ptr = &conn;
    connection *c_publ = NULL;
    if(int_publ_srv_conn_map().get(&connection_impl_ptr, &c_publ)) {
        EXIT_ACTION("failed get instance from int_publ_srv_conn_map\n")
    }
    subscription *publ = ssf->new_subscription(*c_publ);
    vlg::collector &c = publ->get_collector();
    /************************
    RETAIN_ID: SPB_SRV_01
    ************************/
    c.retain(publ);

    subscription_impl *int_sbs =
        new timpl_subscription_impl_server(conn, *publ);

    publ->set_implernal(int_sbs);
    publ->bind(*c_publ);
    int_publ_srv_sbs_map().put(&int_sbs, &publ);
    return int_sbs;
}

subscription_factory::subscription_factory()
{}

subscription_factory::~subscription_factory()
{}

subscription *subscription_factory::new_subscription(connection &conn)
{
    return new subscription();
}

}
