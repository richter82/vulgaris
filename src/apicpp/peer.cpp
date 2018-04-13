/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "vlg_peer.h"
#include "vlg_connection.h"
#include "vlg/pr_impl.h"

namespace vlg {

peer::peer() : impl_(new peer_impl(*this))
{
    CTOR_TRC
}

peer::~peer()
{
    DTOR_TRC
}

RetCode peer::set_params_file_dir(const char *dir)
{
    return impl_->set_params_file_dir(dir);
}

RetCode peer::set_params_file_path_name(const char *file_path)
{
    return impl_->set_params_file_path_name(file_path);
}

unsigned int peer::get_version_major()
{
    return impl_->peer_ver_[0];
}

unsigned int peer::get_version_minor()
{
    return impl_->peer_ver_[1];
}

unsigned int peer::get_version_maintenance()
{
    return impl_->peer_ver_[2];
}

unsigned int peer::get_version_architecture()
{
    return impl_->peer_ver_[3];
}

bool peer::is_configured()
{
    return impl_->configured_;
}

const nentity_manager &peer::get_entity_manager() const
{
    return impl_->nem_;
}

nentity_manager &peer::get_entity_manager_m()
{
    return impl_->nem_;
}

bool peer::is_persistent()
{
    return impl_->pers_enabled_;
}

bool peer::is_create_persistent_schema()
{
    return impl_->pers_schema_create_;
}

bool peer::is_drop_existing_persistent_schema()
{
    return impl_->drop_existing_schema_;
}

PeerPersonality peer::get_personality()
{
    return impl_->personality_;
}

sockaddr_in peer::get_server_sockaddr()
{
    return impl_->selector_.srv_sockaddr_in_;
}

unsigned int peer::get_server_transaction_service_executor_size()
{
    return impl_->srv_exectrs_;
}

unsigned int peer::get_client_transaction_service_executor_size()
{
    return impl_->cli_exectrs_;
}

unsigned int peer::get_server_subscription_service_executor_size()
{
    return impl_->srv_sbs_exectrs_;
}

void peer::set_personality(PeerPersonality personality)
{
    impl_->personality_ = personality;
}

void peer::add_load_model(const char *model)
{
    impl_->set_cfg_load_model(model);
}

void peer::set_server_address(const char *address)
{
    impl_->set_cfg_srv_sin_addr(address);
}

void peer::set_server_port(int port)
{
    impl_->set_cfg_srv_sin_port(port);
}

void peer::set_server_transaction_service_executor_size(unsigned int server_executors)
{
    impl_->srv_exectrs_ = server_executors;
}

void peer::set_client_transaction_service_executor_size(unsigned int client_executors)
{
    impl_->cli_exectrs_ = client_executors;
}

void peer::set_server_subscription_service_executor_size(unsigned int srv_sbs_executors)
{
    impl_->srv_sbs_exectrs_ = srv_sbs_executors;
}

void peer::set_persistent(bool persistent)
{
    impl_->pers_enabled_ = persistent;
}

void peer::set_create_persistent_schema(bool persistent_schema_create)
{
    impl_->pers_schema_create_ = persistent_schema_create;
}

void peer::set_drop_existing_persistent_schema(bool drop_existing_schema)
{
    impl_->drop_existing_schema_ = drop_existing_schema;
}

void peer::add_load_persistent_driver(const char *driver)
{
    impl_->set_cfg_load_pers_driv(driver);
}

RetCode peer::extend_model(nentity_manager &nem)
{
    return impl_->extend_model(nem);
}

RetCode peer::extend_model(const char *model_name)
{
    return impl_->extend_model(model_name);
}

RetCode peer::on_load_config(int pnum, const char *param,
                             const char *value)
{
    return RetCode_OK;
}

void peer::on_status_change(PeerStatus current)
{}

RetCode peer::on_init()
{
    return RetCode_OK;
}

RetCode peer::on_starting()
{
    return RetCode_OK;
}

RetCode peer::on_stopping()
{
    return RetCode_OK;
}

RetCode peer::on_move_running()
{
    return RetCode_OK;
}

RetCode peer::on_error()
{
    return RetCode_OK;
}

void peer::on_dying_breath()
{
}

void peer::set_configured(bool configured)
{
    impl_->configured_ = configured;
}

PeerStatus peer::get_status()
{
    return impl_->peer_status_;
}

RetCode peer::await_for_status_reached(PeerStatus test,
                                       PeerStatus &current,
                                       time_t sec,
                                       long nsec)
{
    return impl_->await_for_status_reached(test,
                                           current,
                                           sec,
                                           nsec);
}

RetCode peer::await_for_status_change(PeerStatus &peer_status,
                                      time_t sec,
                                      long nsec)
{
    return impl_->await_for_status_change(peer_status,
                                          sec,
                                          nsec);
}

incoming_connection_factory &peer::get_incoming_connection_factory()
{
    return impl_->get_incoming_connection_factory();
}

void peer::set_incoming_connection_factory(incoming_connection_factory &conn_factory)
{
    impl_->set_incoming_connection_factory(conn_factory);
}

RetCode peer::start(int argc,
                    char *argv[],
                    bool spawn_new_thread)
{
    return impl_->start(argc, argv, spawn_new_thread);
}

RetCode peer::stop(bool force_disconnect /*= false*/)
{
    return impl_->stop(force_disconnect);
}

RetCode peer::create_persistent_schema(PersistenceAlteringMode mode)
{
    return impl_->create_persistent_schema(mode);
}

RetCode peer::nclass_create_persistent_schema(PersistenceAlteringMode mode,
                                              unsigned int nclass_id)
{
    return impl_->nclass_create_persistent_schema(mode, nclass_id);
}

RetCode peer::obj_load(unsigned short nclass_key,
                       unsigned int &ts_0_out,
                       unsigned int &ts_1_out,
                       nclass &in_out)
{
    return impl_->obj_load(nclass_key,
                           ts_0_out,
                           ts_1_out,
                           in_out);
}

RetCode peer::obj_save(const nclass &in)
{
    return impl_->obj_save(in);
}

RetCode peer::obj_update(unsigned short nclass_key,
                         const nclass &in)
{
    return impl_->obj_update(nclass_key, in);
}

RetCode peer::obj_update_or_save(unsigned short nclass_key,
                                 const nclass &in)
{
    return impl_->obj_update_or_save(nclass_key, in);
}

RetCode peer::obj_remove(unsigned short nclass_key,
                         PersistenceDeletionMode mode,
                         const nclass &in)
{
    return impl_->obj_remove(nclass_key, mode, in);
}

RetCode peer::obj_distribute(SubscriptionEventType event_type,
                             Action action,
                             const nclass &in)
{
    return impl_->obj_distribute(event_type,
                                 ProtocolCode_SUCCESS,
                                 action,
                                 in);
}

RetCode peer::obj_save_and_distribute(const nclass &in)
{
    return impl_->obj_save_and_distribute(in);
}

RetCode peer::obj_update_and_distribute(unsigned short nclass_key,
                                        const nclass &in)
{
    return impl_->obj_update_and_distribute(nclass_key, in);
}

RetCode peer::obj_update_or_save_and_distribute(unsigned short nclass_key,
                                                const nclass &in)
{
    return impl_->obj_update_or_save_and_distribute(nclass_key, in);
}

RetCode peer::obj_remove_and_distribute(unsigned short nclass_key,
                                        PersistenceDeletionMode mode,
                                        const nclass &in)
{
    return impl_->obj_remove_and_distribute(nclass_key, mode, in);
}

RetCode peer::on_incoming_connection(std::shared_ptr<incoming_connection> &)
{
    return RetCode_OK;
}

}
