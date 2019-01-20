/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#include "vlg_broker.h"
#include "vlg_connection.h"
#include "vlg/brk_impl.h"

namespace vlg {

struct default_broker_listener : public broker_listener {
    virtual RetCode on_load_config(broker &,
                                   int pnum,
                                   const char *param,
                                   const char *value) override {
        return RetCode_OK;
    }

    virtual RetCode on_init(broker &) override {
        return RetCode_OK;
    }
    virtual RetCode on_starting(broker &) override {
        return RetCode_OK;
    }
    virtual RetCode on_stopping(broker &) override {
        return RetCode_OK;
    }
    virtual RetCode on_move_running(broker &) override {
        return RetCode_OK;
    }
    virtual void on_error(broker &) override {}
    virtual void on_status_change(broker &, BrokerStatus) override {}
    virtual RetCode on_incoming_connection(broker &, std::shared_ptr<incoming_connection> &) override {
        return RetCode_OK;
    }
};

static default_broker_listener dpl;

broker_listener &broker_listener::default_listener()
{
    return dpl;
}

broker::broker(broker_listener &listener) : impl_(new broker_impl(*this, listener))
{}

broker::~broker()
{
    DTOR_TRC(impl_->log_)
}

RetCode broker::set_params_file_dir(const char *dir)
{
    return impl_->set_params_file_dir(dir);
}

RetCode broker::set_params_file_path_name(const char *file_path)
{
    return impl_->set_params_file_path_name(file_path);
}

unsigned int broker::get_version_major()
{
    return impl_->broker_ver_[0];
}

unsigned int broker::get_version_minor()
{
    return impl_->broker_ver_[1];
}

unsigned int broker::get_version_maintenance()
{
    return impl_->broker_ver_[2];
}

unsigned int broker::get_version_architecture()
{
    return impl_->broker_ver_[3];
}

bool broker::is_configured()
{
    return impl_->configured_;
}

const nentity_manager &broker::get_nentity_manager() const
{
    return impl_->nem_;
}

bool broker::is_persistent()
{
    return impl_->pers_enabled_;
}

bool broker::is_create_persistent_schema()
{
    return impl_->pers_schema_create_;
}

bool broker::is_drop_existing_persistent_schema()
{
    return impl_->drop_existing_schema_;
}

BrokerPersonality broker::get_personality()
{
    return impl_->personality_;
}

sockaddr_in broker::get_server_sockaddr()
{
    return impl_->selector_.srv_sockaddr_in_;
}

unsigned int broker::get_server_transaction_service_executor_size()
{
    return impl_->srv_exectrs_;
}

unsigned int broker::get_client_transaction_service_executor_size()
{
    return impl_->cli_exectrs_;
}

unsigned int broker::get_server_subscription_service_executor_size()
{
    return impl_->srv_sbs_exectrs_;
}

void broker::set_personality(BrokerPersonality personality)
{
    impl_->personality_ = personality;
}

void broker::add_load_model(const char *model)
{
    impl_->set_cfg_load_model(model);
}

void broker::set_server_address(const char *address)
{
    impl_->set_cfg_srv_sin_addr(address);
}

void broker::set_server_port(int port)
{
    impl_->set_cfg_srv_sin_port(port);
}

void broker::set_server_transaction_service_executor_size(unsigned int server_executors)
{
    impl_->srv_exectrs_ = server_executors;
}

void broker::set_client_transaction_service_executor_size(unsigned int client_executors)
{
    impl_->cli_exectrs_ = client_executors;
}

void broker::set_server_subscription_service_executor_size(unsigned int srv_sbs_executors)
{
    impl_->srv_sbs_exectrs_ = srv_sbs_executors;
}

void broker::set_persistent(bool persistent)
{
    impl_->pers_enabled_ = persistent;
}

void broker::set_create_persistent_schema(bool persistent_schema_create)
{
    impl_->pers_schema_create_ = persistent_schema_create;
}

void broker::set_drop_existing_persistent_schema(bool drop_existing_schema)
{
    impl_->drop_existing_schema_ = drop_existing_schema;
}

void broker::add_load_persistent_driver(const char *driver)
{
    impl_->set_cfg_load_pers_driv(driver);
}

RetCode broker::extend_model(nentity_manager &nem)
{
    return impl_->extend_model(nem);
}

RetCode broker::extend_model(const char *model_name)
{
    return impl_->extend_model(model_name);
}

void broker::set_configured(bool configured)
{
    impl_->configured_ = configured;
}

BrokerStatus broker::get_status()
{
    return impl_->broker_status_;
}

RetCode broker::await_for_status_reached(BrokerStatus test,
                                         BrokerStatus &current,
                                         time_t sec,
                                         long nsec)
{
    return impl_->await_for_status_reached(test,
                                           current,
                                           sec,
                                           nsec);
}

RetCode broker::await_for_status_change(BrokerStatus &broker_status,
                                        time_t sec,
                                        long nsec)
{
    return impl_->await_for_status_change(broker_status,
                                          sec,
                                          nsec);
}

incoming_connection_factory &broker::get_incoming_connection_factory()
{
    return impl_->get_incoming_connection_factory();
}

void broker::set_incoming_connection_factory(incoming_connection_factory &conn_factory)
{
    impl_->set_incoming_connection_factory(conn_factory);
}

RetCode broker::start(int argc,
                      char *argv[],
                      bool spawn_new_thread)
{
    return impl_->start(argc, argv, spawn_new_thread);
}

RetCode broker::stop(bool force_disconnect /*= false*/)
{
    return impl_->stop(force_disconnect);
}

RetCode broker::create_persistent_schema(PersistenceAlteringMode mode)
{
    return impl_->create_persistent_schema(mode);
}

RetCode broker::nclass_create_persistent_schema(PersistenceAlteringMode mode,
                                                unsigned int nclass_id)
{
    return impl_->nclass_create_persistent_schema(mode, nclass_id);
}

RetCode broker::obj_load(unsigned short nclass_key,
                         unsigned int &ts_0_out,
                         unsigned int &ts_1_out,
                         nclass &in_out)
{
    return impl_->obj_load(nclass_key,
                           ts_0_out,
                           ts_1_out,
                           in_out);
}

RetCode broker::obj_save(const nclass &in)
{
    return impl_->obj_save(in);
}

RetCode broker::obj_update(unsigned short nclass_key,
                           const nclass &in)
{
    return impl_->obj_update(nclass_key, in);
}

RetCode broker::obj_update_or_save(unsigned short nclass_key,
                                   const nclass &in)
{
    return impl_->obj_update_or_save(nclass_key, in);
}

RetCode broker::obj_remove(unsigned short nclass_key,
                           PersistenceDeletionMode mode,
                           const nclass &in)
{
    return impl_->obj_remove(nclass_key, mode, in);
}

RetCode broker::obj_distribute(SubscriptionEventType event_type,
                               Action action,
                               const nclass &in)
{
    return impl_->obj_distribute(event_type,
                                 ProtocolCode_SUCCESS,
                                 action,
                                 in);
}

RetCode broker::obj_save_and_distribute(const nclass &in)
{
    return impl_->obj_save_and_distribute(in);
}

RetCode broker::obj_update_and_distribute(unsigned short nclass_key,
                                          const nclass &in)
{
    return impl_->obj_update_and_distribute(nclass_key, in);
}

RetCode broker::obj_update_or_save_and_distribute(unsigned short nclass_key,
                                                  const nclass &in)
{
    return impl_->obj_update_or_save_and_distribute(nclass_key, in);
}

RetCode broker::obj_remove_and_distribute(unsigned short nclass_key,
                                          PersistenceDeletionMode mode,
                                          const nclass &in)
{
    return impl_->obj_remove_and_distribute(nclass_key, mode, in);
}

}
