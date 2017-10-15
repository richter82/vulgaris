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

#include "blaze_peer.h"
#include "blaze_connection.h"
#include "blz_peer_int.h"

namespace blaze {

blaze::synch_hash_map *int_publ_peer_map_ = NULL;  //peer_int --> peer
blaze::synch_hash_map &int_publ_peer_map()
{
    if(int_publ_peer_map_) {
        return *int_publ_peer_map_;
    }
    if(!(int_publ_peer_map_ = new blaze::synch_hash_map(
        blaze::sngl_ptr_obj_mng(),
        blaze::sngl_ptr_obj_mng()))) {
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
        class pimpl_peer_int : public peer_int {
            public:
                pimpl_peer_int(peer &publ, unsigned int id) : peer_int(id), publ_(publ) {
                    early_init();
                }

                virtual const char *peer_name_usr() {
                    return publ_.name_handler();
                }

                virtual const unsigned int *peer_ver_usr() {
                    return publ_.version_handler();
                }

                virtual blaze::RetCode peer_load_cfg_usr(int pnum, const char *param,
                                                         const char *value) {
                    blaze::RetCode cdrs_res = peer_int::peer_load_cfg_usr(pnum, param, value);
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_load_config(pnum, param, value);
                }

                virtual blaze::RetCode peer_init_usr() {
                    blaze::RetCode cdrs_res = peer_int::peer_init_usr();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_init();
                }

                virtual blaze::RetCode peer_start_usr() {
                    blaze::RetCode cdrs_res = peer_int::peer_start_usr();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_starting();
                }

                virtual blaze::RetCode peer_stop_usr() {
                    blaze::RetCode cdrs_res = peer_int::peer_stop_usr();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_stopping();
                }

                virtual blaze::RetCode peer_move_running() {
                    blaze::RetCode cdrs_res = peer_int::peer_move_running_usr();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    return publ_.on_transit_on_air();
                }

                virtual blaze::RetCode peer_error_handler() {
                    blaze::RetCode cdrs_res = peer_int::peer_error_handler();
                    if(cdrs_res) {
                        return cdrs_res;
                    }
                    publ_.on_error();
                    return cdrs_res;
                }

                virtual blaze::RetCode peer_dying_breath_handler() {
                    publ_.on_dying_breath();
                    return peer_int::peer_dying_breath_handler();
                }

                virtual blaze::RetCode new_incoming_connection_accept(connection_int
                                                                      &incoming_connection) {
                    connection *conn_publ = new connection();
                    blaze::collector &c = conn_publ->get_collector();
                    c.retain(conn_publ);
                    conn_publ->set_internal(&incoming_connection);
                    blaze::RetCode accept_res = publ_.on_new_incoming_connection(*conn_publ);
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
            int_ = new pimpl_peer_int(publ_, 0);
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

        peer_int *get_peer_int() const {
            return int_;
        }
        void set_peer_int(peer_int *val) {
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
            int_->set_conn_factory(connection_factory::conn_factory_int_f);
            int_->set_conn_factory_ud(conn_factory_);
        }

    private:
        peer     &publ_;
        peer_int *int_;
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

blaze::RetCode peer::set_params_file_dir(const char *dir)
{
    return impl_->get_peer_int()->set_params_file_dir(dir);
}

blaze::RetCode peer::set_params_file_path_name(const char *file_path)
{
    return impl_->get_peer_int()->set_params_file_path_name(file_path);
}

const char *peer::get_name()
{
    return impl_->get_peer_int()->peer_name();
}

const unsigned int *peer::get_version()
{
    return impl_->get_peer_int()->peer_ver();
}

unsigned int peer::get_version_major()
{
    return impl_->get_peer_int()->peer_ver_major();
}

unsigned int peer::get_version_minor()
{
    return impl_->get_peer_int()->peer_ver_minor();
}

unsigned int peer::get_version_maintenance()
{
    return impl_->get_peer_int()->peer_ver_mant();
}

unsigned int peer::get_version_architecture()
{
    return impl_->get_peer_int()->peer_ver_arch();
}

bool peer::is_configured()
{
    return impl_->get_peer_int()->is_configured();
}

const entity_manager &peer::get_entity_manager() const
{
    return impl_->get_peer_int()->get_em();
}

entity_manager &peer::get_entity_manager_m()
{
    return impl_->get_peer_int()->get_em_m();
}

bool peer::is_persistent()
{
    return impl_->get_peer_int()->persistent();
}

bool peer::is_persistent_schema_creating()
{
    return impl_->get_peer_int()->pers_schema_create();
}

bool peer::is_dropping_existing_schema()
{
    return impl_->get_peer_int()->get_cfg_drop_existing_schema();
}

PeerPersonality peer::get_personality()
{
    return impl_->get_peer_int()->get_cfg_personality();
}

sockaddr_in peer::get_server_sockaddr()
{
    return impl_->get_peer_int()->get_cfg_srv_sock_addr();
}

unsigned int peer::server_executor_count()
{
    return impl_->get_peer_int()->get_cfg_srv_exectrs();
}

unsigned int peer::client_executor_count()
{
    return impl_->get_peer_int()->get_cfg_cli_exectrs();
}

unsigned int peer::server_sbs_executor_count()
{
    return impl_->get_peer_int()->get_cfg_srv_sbs_exectrs();
}

void peer::set_personality(PeerPersonality personality)
{
    impl_->get_peer_int()->set_cfg_personality(personality);
}

void peer::add_load_model(const char *model)
{
    impl_->get_peer_int()->set_cfg_load_model(model);
}

void peer::set_srv_sin_addr(const char *address)
{
    impl_->get_peer_int()->set_cfg_srv_sin_addr(address);
}

void peer::set_sin_port(int port)
{
    impl_->get_peer_int()->set_cfg_srv_sin_port(port);
}

void peer::set_srv_executors(unsigned int server_executors)
{
    impl_->get_peer_int()->set_cfg_srv_exectrs(server_executors);
}

void peer::set_cli_executors(unsigned int client_executors)
{
    impl_->get_peer_int()->set_cfg_cli_exectrs(client_executors);
}

void peer::set_srv_sbs_executors(unsigned int srv_sbs_executors)
{
    impl_->get_peer_int()->set_cfg_srv_sbs_exectrs(srv_sbs_executors);
}

void peer::set_persistent(bool persistent)
{
    impl_->get_peer_int()->set_cfg_pers_enabled(persistent);
}

void peer::set_persistent_schema_creating(bool persistent_schema_create)
{
    impl_->get_peer_int()->set_cfg_pers_schema_create(persistent_schema_create);
}

void peer::set_dropping_existing_schema(bool drop_existing_schema)
{
    impl_->get_peer_int()->set_cfg_drop_existing_schema(drop_existing_schema);
}

void peer::add_load_persistent_driver(const char *driver)
{
    impl_->get_peer_int()->set_cfg_load_pers_driv(driver);
}

blaze::RetCode peer::extend_model(entity_manager *em)
{
    return impl_->get_peer_int()->extend_model(em);
}

blaze::RetCode peer::extend_model(const char *model_name)
{
    return impl_->get_peer_int()->extend_model(model_name);
}

blaze::RetCode peer::on_load_config(int pnum, const char *param,
                                    const char *value)
{
    return blaze::RetCode_OK;
}

blaze::RetCode peer::on_init()
{
    return blaze::RetCode_OK;
}

blaze::RetCode peer::on_starting()
{
    return blaze::RetCode_OK;
}

blaze::RetCode peer::on_stopping()
{
    return blaze::RetCode_OK;
}

blaze::RetCode peer::on_transit_on_air()
{
    return blaze::RetCode_OK;
}

void peer::on_error()
{
}

void peer::on_dying_breath()
{
}

void peer::set_configured(bool configured)
{
    return impl_->get_peer_int()->set_configured(configured);
}

PeerStatus peer::get_status()
{
    return impl_->get_peer_int()->peer_status();
}

void peer::set_status_change_handler(peer_status_change handler, void *ud)
{
    impl_->set_psh(handler);
    impl_->set_psh_ud(ud);
}

blaze::RetCode peer::await_for_status_reached_or_outdated(PeerStatus test,
                                                          PeerStatus &current,
                                                          time_t sec,
                                                          long nsec)
{
    return impl_->get_peer_int()->await_for_peer_status_reached_or_outdated(test,
                                                                            current,
                                                                            sec,
                                                                            nsec);
}

blaze::RetCode peer::await_for_status_change(PeerStatus &peer_status,
                                             time_t sec,
                                             long nsec)
{
    return impl_->get_peer_int()->await_for_peer_status_change(peer_status,
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

blaze::RetCode peer::start(int argc,
                           char *argv[],
                           bool spawn_new_thread)
{
    return impl_->get_peer_int()->start_peer(argc, argv, spawn_new_thread);
}

blaze::RetCode peer::stop(bool force_disconnect /*= false*/)
{
    return impl_->get_peer_int()->stop_peer(force_disconnect);
}

blaze::RetCode peer::persistence_schema_create(PersistenceAlteringMode
                                               mode)
{
    return impl_->get_peer_int()->pers_schema_create(mode);
}

blaze::RetCode peer::class_persistence_schema_create(
    PersistenceAlteringMode
    mode,
    unsigned int nclass_id)
{
    return impl_->get_peer_int()->class_pers_schema_create(mode, nclass_id);
}

blaze::RetCode peer::class_persistent_load(unsigned short class_key,
                                           unsigned int &ts_0_out,
                                           unsigned int &ts_1_out,
                                           nclass &in_out_obj)
{
    return impl_->get_peer_int()->class_pers_load(class_key,
                                                  ts_0_out,
                                                  ts_1_out,
                                                  in_out_obj);
}

blaze::RetCode peer::class_persistent_save(const nclass &in_obj)
{
    return impl_->get_peer_int()->class_pers_save(in_obj);
}

blaze::RetCode peer::class_persistent_update(unsigned short class_key,
                                             const nclass &in_obj)
{
    return impl_->get_peer_int()->class_pers_update(class_key, in_obj);
}

blaze::RetCode peer::class_persistent_update_or_save(unsigned short class_key,
                                                     const nclass &in_obj)
{
    return impl_->get_peer_int()->class_pers_update_or_save(class_key, in_obj);
}

blaze::RetCode peer::class_persistent_remove(unsigned short class_key,
                                             PersistenceDeletionMode mode,
                                             const nclass &in_obj)
{
    return impl_->get_peer_int()->class_pers_remove(class_key, mode, in_obj);
}

blaze::RetCode peer::class_distribute(SubscriptionEventType event_type,
                                      Action action,
                                      const nclass &in_obj)
{
    return impl_->get_peer_int()->class_distribute(event_type,
                                                   ProtocolCode_SUCCESS,
                                                   action,
                                                   in_obj);
}

blaze::RetCode peer::class_persistent_save_and_distribute(const nclass
                                                          &in_obj)
{
    return impl_->get_peer_int()->class_pers_save_and_distribute(in_obj);
}

blaze::RetCode peer::class_persistent_update_and_distribute(
    unsigned short class_key,
    const nclass &in_obj)
{
    return impl_->get_peer_int()->class_pers_update_and_distribute(class_key,
                                                                   in_obj);
}

blaze::RetCode peer::class_persistent_update_or_save_and_distribute(
    unsigned short class_key,
    const nclass &in_obj)
{
    return impl_->get_peer_int()->class_pers_update_or_save_and_distribute(
               class_key, in_obj);
}

blaze::RetCode peer::class_persistent_remove_and_distribute(
    unsigned short class_key,
    PersistenceDeletionMode mode,
    const nclass &in_obj)
{
    return impl_->get_peer_int()->class_pers_remove_and_distribute(class_key,
                                                                   mode,
                                                                   in_obj);
}

blaze::RetCode peer::on_new_incoming_connection(connection &incoming_connection)
{
    return blaze::RetCode_OK;
}

peer_int *peer::get_internal()
{
    return impl_->get_peer_int();
}

}
