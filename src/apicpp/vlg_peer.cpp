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

#include "vlg_peer.h"
#include "vlg_connection.h"
#include "vlg/vlg_peer_impl.h"

namespace vlg {

vlg::synch_hash_map *int_publ_peer_map_ = NULL;  //peer_impl --> peer
vlg::synch_hash_map &int_publ_peer_map()
{
    if(int_publ_peer_map_) {
        return *int_publ_peer_map_;
    }
    if(!(int_publ_peer_map_ = new vlg::synch_hash_map(
        vlg::sngl_ptr_obj_mng(),
        vlg::sngl_ptr_obj_mng()))) {
        EXIT_ACTION("failed creating int_publ_peer_map_\n")
    }
    int_publ_peer_map_->init(HM_SIZE_TINY);
    return *int_publ_peer_map_;
}

//-----------------------------
// peer_impl
//-----------------------------

class peer_impl {
    private:
        class pimpl_peer_impl : public peer_impl {
            public:
                pimpl_peer_impl(peer &publ, unsigned int id) : peer_impl(id), publ_(publ) {
                    early_init();
                }

                virtual const char *peer_name_usr() {
                    return publ_.name_handler();
                }

                virtual const unsigned int *peer_ver_usr() {
                    return publ_.version_handler();
                }

                virtual vlg::RetCode peer_load_cfg_usr(int pnum, const char *param,
                                                       const char *value) {
                    vlg::RetCode cdrs_res = peer_impl::peer_load_cfg_usr(pnum, param, value);
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_load_config(pnum, param, value);
                }

                virtual vlg::RetCode peer_init_usr() {
                    vlg::RetCode cdrs_res = peer_impl::peer_init_usr();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_init();
                }

                virtual vlg::RetCode peer_start_usr() {
                    vlg::RetCode cdrs_res = peer_impl::peer_start_usr();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_starting();
                }

                virtual vlg::RetCode peer_stop_usr() {
                    vlg::RetCode cdrs_res = peer_impl::peer_stop_usr();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_stopping();
                }

                virtual vlg::RetCode peer_move_running() {
                    vlg::RetCode cdrs_res = peer_impl::peer_move_running_usr();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_transit_on_air();
                }

                virtual vlg::RetCode peer_error_handler() {
                    vlg::RetCode cdrs_res = peer_impl::peer_error_handler();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    publ_.on_error();
                    return cdrs_res;
                }

                virtual vlg::RetCode peer_dying_breath_handler() {
                    publ_.on_dying_breath();
                    return peer_impl::peer_dying_breath_handler();
                }

                virtual vlg::RetCode new_incoming_connection_accept(connection_impl
                                                                    &incoming_connection) {
                    connection *conn_publ = new connection();
                    vlg::collector &c = conn_publ->get_collector();
                    c.retain(conn_publ);
                    conn_publ->set_implernal(&incoming_connection);
                    vlg::RetCode accept_res = publ_.on_new_incoming_connection(*conn_publ);
                    c.release(conn_publ);
                    return accept_res;
                }

            private:
                peer &publ_;
        };

        static void peer_lfcyc_status_change_hndlr_pimpl(peer_automa &peer,
                                                         PeerStatus status,
                                                         void *ud) {
            peer_impl *pimpl = static_cast<peer_impl *>(ud);
            if(pimpl->psh_) {
                pimpl->psh_(pimpl->publ_, status, pimpl->psh_ud_);
            }
        }

    public:
        peer_impl(peer &publ) : publ_(publ), int_(NULL), psh_(NULL), psh_ud_(NULL),
            conn_factory_(connection_factory::default_connection_factory()) {
            int_ = new pimpl_peer_impl(publ_, 0);
            peer *publ_ptr = &publ;
            int_publ_peer_map().put(&int_, &publ_ptr);
            int_->set_peer_status_change_hndlr(peer_lfcyc_status_change_hndlr_pimpl, this);
        }

        ~peer_impl() {
            if(int_) {
                if(int_->peer_status() > PeerStatus_INITIALIZED) {
                    int_->stop_peer(true);
                    PeerStatus current = PeerStatus_ZERO;
                    int_->await_for_peer_status_reached_or_outdated(PeerStatus_STOPPED,
                                                                    current);
                }
                int_publ_peer_map().remove(&int_, NULL);
                delete int_;
            }
        }

        peer_impl *get_peer_impl() const {
            return int_;
        }
        void set_peer_impl(peer_impl *val) {
            int_ = val;
        }

        peer::peer_status_change get_psh() const {
            return psh_;
        }
        void set_psh(peer::peer_status_change val) {
            psh_ = val;
        }

        void *get_psh_ud() const {
            return psh_ud_;
        }
        void set_psh_ud(void *val) {
            psh_ud_ = val;
        }

        connection_factory *get_conn_factory() const {
            return conn_factory_;
        }
        void set_conn_factory(connection_factory *val) {
            conn_factory_ = val;
            int_->set_conn_factory(connection_factory::conn_factory_impl_f);
            int_->set_conn_factory_ud(conn_factory_);
        }

    private:
        peer     &publ_;
        peer_impl *int_;
        peer::peer_status_change psh_;
        void *psh_ud_;
        connection_factory *conn_factory_;
};


//-----------------------------
// peer
//-----------------------------

peer::peer()
{
    impl_ = new peer_impl(*this);
}

peer::~peer()
{
    if(impl_) {
        delete impl_;
    }
}

vlg::RetCode peer::set_params_file_dir(const char *dir)
{
    return impl_->get_peer_impl()->set_params_file_dir(dir);
}

vlg::RetCode peer::set_params_file_path_name(const char *file_path)
{
    return impl_->get_peer_impl()->set_params_file_path_name(file_path);
}

const char *peer::get_name()
{
    return impl_->get_peer_impl()->peer_name();
}

const unsigned int *peer::get_version()
{
    return impl_->get_peer_impl()->peer_ver();
}

unsigned int peer::get_version_major()
{
    return impl_->get_peer_impl()->peer_ver_major();
}

unsigned int peer::get_version_minor()
{
    return impl_->get_peer_impl()->peer_ver_minor();
}

unsigned int peer::get_version_maintenance()
{
    return impl_->get_peer_impl()->peer_ver_mant();
}

unsigned int peer::get_version_architecture()
{
    return impl_->get_peer_impl()->peer_ver_arch();
}

bool peer::is_configured()
{
    return impl_->get_peer_impl()->is_configured();
}

const entity_manager &peer::get_entity_manager() const
{
    return impl_->get_peer_impl()->get_em();
}

entity_manager &peer::get_entity_manager_m()
{
    return impl_->get_peer_impl()->get_em_m();
}

bool peer::is_persistent()
{
    return impl_->get_peer_impl()->persistent();
}

bool peer::is_persistent_schema_creating()
{
    return impl_->get_peer_impl()->pers_schema_create();
}

bool peer::is_dropping_existing_schema()
{
    return impl_->get_peer_impl()->get_cfg_drop_existing_schema();
}

PeerPersonality peer::get_personality()
{
    return impl_->get_peer_impl()->get_cfg_personality();
}

sockaddr_in peer::get_server_sockaddr()
{
    return impl_->get_peer_impl()->get_cfg_srv_sock_addr();
}

unsigned int peer::server_executor_count()
{
    return impl_->get_peer_impl()->get_cfg_srv_exectrs();
}

unsigned int peer::client_executor_count()
{
    return impl_->get_peer_impl()->get_cfg_cli_exectrs();
}

unsigned int peer::server_sbs_executor_count()
{
    return impl_->get_peer_impl()->get_cfg_srv_sbs_exectrs();
}

void peer::set_personality(PeerPersonality personality)
{
    impl_->get_peer_impl()->set_cfg_personality(personality);
}

void peer::add_load_model(const char *model)
{
    impl_->get_peer_impl()->set_cfg_load_model(model);
}

void peer::set_srv_sin_addr(const char *address)
{
    impl_->get_peer_impl()->set_cfg_srv_sin_addr(address);
}

void peer::set_sin_port(int port)
{
    impl_->get_peer_impl()->set_cfg_srv_sin_port(port);
}

void peer::set_srv_executors(unsigned int server_executors)
{
    impl_->get_peer_impl()->set_cfg_srv_exectrs(server_executors);
}

void peer::set_cli_executors(unsigned int client_executors)
{
    impl_->get_peer_impl()->set_cfg_cli_exectrs(client_executors);
}

void peer::set_srv_sbs_executors(unsigned int srv_sbs_executors)
{
    impl_->get_peer_impl()->set_cfg_srv_sbs_exectrs(srv_sbs_executors);
}

void peer::set_persistent(bool persistent)
{
    impl_->get_peer_impl()->set_cfg_pers_enabled(persistent);
}

void peer::set_persistent_schema_creating(bool persistent_schema_create)
{
    impl_->get_peer_impl()->set_cfg_pers_schema_create(persistent_schema_create);
}

void peer::set_dropping_existing_schema(bool drop_existing_schema)
{
    impl_->get_peer_impl()->set_cfg_drop_existing_schema(drop_existing_schema);
}

void peer::add_load_persistent_driver(const char *driver)
{
    impl_->get_peer_impl()->set_cfg_load_pers_driv(driver);
}

vlg::RetCode peer::extend_model(entity_manager *em)
{
    return impl_->get_peer_impl()->extend_model(em);
}

vlg::RetCode peer::extend_model(const char *model_name)
{
    return impl_->get_peer_impl()->extend_model(model_name);
}

vlg::RetCode peer::on_load_config(int pnum, const char *param,
                                  const char *value)
{
    return vlg::RetCode_OK;
}

vlg::RetCode peer::on_init()
{
    return vlg::RetCode_OK;
}

vlg::RetCode peer::on_starting()
{
    return vlg::RetCode_OK;
}

vlg::RetCode peer::on_stopping()
{
    return vlg::RetCode_OK;
}

vlg::RetCode peer::on_transit_on_air()
{
    return vlg::RetCode_OK;
}

void peer::on_error()
{
}

void peer::on_dying_breath()
{
}

void peer::set_configured(bool configured)
{
    return impl_->get_peer_impl()->set_configured(configured);
}

PeerStatus peer::get_status()
{
    return impl_->get_peer_impl()->peer_status();
}

void peer::set_status_change_handler(peer_status_change handler, void *ud)
{
    impl_->set_psh(handler);
    impl_->set_psh_ud(ud);
}

vlg::RetCode peer::await_for_status_reached_or_outdated(PeerStatus test,
                                                        PeerStatus &current,
                                                        time_t sec,
                                                        long nsec)
{
    return impl_->get_peer_impl()->await_for_peer_status_reached_or_outdated(test,
                                                                             current,
                                                                             sec,
                                                                             nsec);
}

vlg::RetCode peer::await_for_status_change(PeerStatus &peer_status,
                                           time_t sec,
                                           long nsec)
{
    return impl_->get_peer_impl()->await_for_peer_status_change(peer_status,
                                                                sec,
                                                                nsec);
}

connection_factory *peer::get_connection_factory()
{
    return impl_->get_conn_factory();
}

void peer::set_connection_factory(connection_factory &conn_factory)
{
    impl_->set_conn_factory(&conn_factory);
}

vlg::RetCode peer::start(int argc,
                         char *argv[],
                         bool spawn_new_thread)
{
    return impl_->get_peer_impl()->start_peer(argc, argv, spawn_new_thread);
}

vlg::RetCode peer::stop(bool force_disconnect /*= false*/)
{
    return impl_->get_peer_impl()->stop_peer(force_disconnect);
}

vlg::RetCode peer::persistence_schema_create(PersistenceAlteringMode
                                             mode)
{
    return impl_->get_peer_impl()->pers_schema_create(mode);
}

vlg::RetCode peer::class_persistence_schema_create(
    PersistenceAlteringMode
    mode,
    unsigned int nclass_id)
{
    return impl_->get_peer_impl()->class_pers_schema_create(mode, nclass_id);
}

vlg::RetCode peer::class_persistent_load(unsigned short class_key,
                                         unsigned int &ts_0_out,
                                         unsigned int &ts_1_out,
                                         nclass &in_out_obj)
{
    return impl_->get_peer_impl()->class_pers_load(class_key,
                                                   ts_0_out,
                                                   ts_1_out,
                                                   in_out_obj);
}

vlg::RetCode peer::class_persistent_save(const nclass &in_obj)
{
    return impl_->get_peer_impl()->class_pers_save(in_obj);
}

vlg::RetCode peer::class_persistent_update(unsigned short class_key,
                                           const nclass &in_obj)
{
    return impl_->get_peer_impl()->class_pers_update(class_key, in_obj);
}

vlg::RetCode peer::class_persistent_update_or_save(unsigned short class_key,
                                                   const nclass &in_obj)
{
    return impl_->get_peer_impl()->class_pers_update_or_save(class_key, in_obj);
}

vlg::RetCode peer::class_persistent_remove(unsigned short class_key,
                                           PersistenceDeletionMode mode,
                                           const nclass &in_obj)
{
    return impl_->get_peer_impl()->class_pers_remove(class_key, mode, in_obj);
}

vlg::RetCode peer::class_distribute(SubscriptionEventType event_type,
                                    Action action,
                                    const nclass &in_obj)
{
    return impl_->get_peer_impl()->class_distribute(event_type,
                                                    ProtocolCode_SUCCESS,
                                                    action,
                                                    in_obj);
}

vlg::RetCode peer::class_persistent_save_and_distribute(const nclass
                                                        &in_obj)
{
    return impl_->get_peer_impl()->class_pers_save_and_distribute(in_obj);
}

vlg::RetCode peer::class_persistent_update_and_distribute(
    unsigned short class_key,
    const nclass &in_obj)
{
    return impl_->get_peer_impl()->class_pers_update_and_distribute(class_key,
                                                                    in_obj);
}

vlg::RetCode peer::class_persistent_update_or_save_and_distribute(
    unsigned short class_key,
    const nclass &in_obj)
{
    return impl_->get_peer_impl()->class_pers_update_or_save_and_distribute(
               class_key, in_obj);
}

vlg::RetCode peer::class_persistent_remove_and_distribute(
    unsigned short class_key,
    PersistenceDeletionMode mode,
    const nclass &in_obj)
{
    return impl_->get_peer_impl()->class_pers_remove_and_distribute(class_key,
                                                                    mode,
                                                                    in_obj);
}

vlg::RetCode peer::on_new_incoming_connection(connection &incoming_connection)
{
    return vlg::RetCode_OK;
}

peer_impl *peer::get_implernal()
{
    return impl_->get_peer_impl();
}

}
