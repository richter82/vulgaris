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

#include "vlg_c_peer.h"
#include "vlg_peer.h"

namespace vlg {

class c_peer : public peer {
    private:
        static void peer_status_change_c_peer(peer &p, PeerStatus status,
                                              void *ud) {
            c_peer &self = static_cast<c_peer &>(p);
            self.psc_wr_((peer_wr)&p, status, self.psc_ud_);
        }

    public:
        c_peer() :
            pnh_wr_(NULL),
            pvh_wr_(NULL),
            plch_wr_(NULL),
            pih_wr_(NULL),
            pstarth_wr_(NULL),
            pstoph_wr_(NULL),
            ptoah_wr_(NULL),
            peh_wr_(NULL),
            pdbh_wr_(NULL),
            psc_wr_(NULL),
            psc_ud_(NULL),
            pnh_wr_ud_(NULL),
            pvh_wr_ud_(NULL),
            plch_wr_ud_(NULL),
            pih_wr_ud_(NULL),
            pstarth_wr_ud_(NULL),
            pstoph_wr_ud_(NULL),
            ptoah_wr_ud_(NULL),
            peh_wr_ud_(NULL),
            pdbh_wr_ud_(NULL) {}

    public:
        // LIFECYCLE - User mandatory entrypoints
        virtual const char *name_handler() {
            if(pnh_wr_) {
                return pnh_wr_((peer_wr)this, pnh_wr_ud_);
            }
            return NULL;
        }

        virtual const unsigned int *version_handler() {
            if(pvh_wr_) {
                return pvh_wr_((peer_wr)this, pvh_wr_ud_);
            }
            return NULL;
        }

        // LIFECYCLE - User opt. entrypoints
    public:
        virtual RetCode on_load_config(int pnum, const char *param, const char *value) {
            if(plch_wr_) {
                return plch_wr_((peer_wr)this, pnum, param, value, plch_wr_ud_);
            } else {
                return RetCode_OK;
            }
        }

        virtual RetCode on_init() {
            if(pih_wr_) {
                return pih_wr_((peer_wr)this, pih_wr_ud_);
            } else {
                return RetCode_OK;
            }
        }

        virtual RetCode on_starting() {
            if(pstarth_wr_) {
                return pstarth_wr_((peer_wr)this, pstarth_wr_ud_);
            } else {
                return RetCode_OK;
            }
        }

        virtual RetCode on_stopping() {
            if(pstoph_wr_) {
                return pstoph_wr_((peer_wr)this, pstoph_wr_ud_);
            } else {
                return RetCode_OK;
            }
        }

        virtual RetCode on_transit_on_air() {
            if(ptoah_wr_) {
                return ptoah_wr_((peer_wr)this, ptoah_wr_ud_);
            } else {
                return RetCode_OK;
            }
        }

        virtual void on_error() {
            if(peh_wr_) {
                peh_wr_((peer_wr)this, peh_wr_ud_);
            }
        }

        virtual void on_dying_breath() {
            if(pdbh_wr_) {
                pdbh_wr_((peer_wr)this, pdbh_wr_ud_);
            }
        }

        peer_name_handler_wr Pnh_wr() const {
            return pnh_wr_;
        }

        void Pnh_wr(peer_name_handler_wr val) {
            pnh_wr_ = val;
        }

        peer_version_handler_wr Pvh_wr() const {
            return pvh_wr_;
        }

        void Pvh_wr(peer_version_handler_wr val) {
            pvh_wr_ = val;
        }

        peer_load_config_handler_wr Plch_wr() const {
            return plch_wr_;
        }

        void Plch_wr(peer_load_config_handler_wr val) {
            plch_wr_ = val;
        }

        peer_init_handler_wr Pih_wr() const {
            return pih_wr_;
        }

        void Pih_wr(peer_init_handler_wr val) {
            pih_wr_ = val;
        }

        peer_starting_handler_wr Pstarth_wr() const {
            return pstarth_wr_;
        }

        void Pstarth_wr(peer_starting_handler_wr val) {
            pstarth_wr_ = val;
        }

        peer_stopping_handler_wr Pstoph_wr() const {
            return pstoph_wr_;
        }

        void Pstoph_wr(peer_stopping_handler_wr val) {
            pstoph_wr_ = val;
        }

        peer_transit_on_air_handler_wr Ptoah_wr() const {
            return ptoah_wr_;
        }

        void Ptoah_wr(peer_transit_on_air_handler_wr val) {
            ptoah_wr_ = val;
        }

        peer_error_handler_wr Peh_wr() const {
            return peh_wr_;
        }

        void Peh_wr(peer_error_handler_wr val) {
            peh_wr_ = val;
        }

        peer_dying_breath_handler_wr Pdbh_wr() const {
            return pdbh_wr_;
        }

        void Pdbh_wr(peer_dying_breath_handler_wr val) {
            pdbh_wr_ = val;
        }

        peer_status_change_wr Psc_wr() const {
            return psc_wr_;
        }

        void Psc_wr(peer_status_change_wr val) {
            psc_wr_ = val;
        }

        void *Psc_ud() const {
            return psc_ud_;
        }

        void Psc_ud(void *val) {
            psc_ud_ = val;
            set_status_change_handler(peer_status_change_c_peer, psc_ud_);
        }

        void *Pnh_wr_ud() const {
            return pnh_wr_ud_;
        }
        void Pnh_wr_ud(void *val) {
            pnh_wr_ud_ = val;
        }
        void *Pvh_wr_ud() const {
            return pvh_wr_ud_;
        }
        void Pvh_wr_ud(void *val) {
            pvh_wr_ud_ = val;
        }
        void *Plch_wr_ud() const {
            return plch_wr_ud_;
        }
        void Plch_wr_ud(void *val) {
            plch_wr_ud_ = val;
        }
        void *Pih_wr_ud() const {
            return pih_wr_ud_;
        }
        void Pih_wr_ud(void *val) {
            pih_wr_ud_ = val;
        }
        void *Pstarth_wr_ud() const {
            return pstarth_wr_ud_;
        }
        void Pstarth_wr_ud(void *val) {
            pstarth_wr_ud_ = val;
        }
        void *Pstoph_wr_ud() const {
            return pstoph_wr_ud_;
        }
        void Pstoph_wr_ud(void *val) {
            pstoph_wr_ud_ = val;
        }
        void *Ptoah_wr_ud() const {
            return ptoah_wr_ud_;
        }
        void Ptoah_wr_ud(void *val) {
            ptoah_wr_ud_ = val;
        }
        void *Peh_wr_ud() const {
            return peh_wr_ud_;
        }
        void Peh_wr_ud(void *val) {
            peh_wr_ud_ = val;
        }
        void *Pdbh_wr_ud() const {
            return pdbh_wr_ud_;
        }
        void Pdbh_wr_ud(void *val) {
            pdbh_wr_ud_ = val;
        }

    private:
        peer_name_handler_wr pnh_wr_;
        peer_version_handler_wr pvh_wr_;
        peer_load_config_handler_wr plch_wr_;
        peer_init_handler_wr pih_wr_;
        peer_starting_handler_wr pstarth_wr_;
        peer_stopping_handler_wr pstoph_wr_;
        peer_transit_on_air_handler_wr ptoah_wr_;
        peer_error_handler_wr peh_wr_;
        peer_dying_breath_handler_wr pdbh_wr_;
        peer_status_change_wr psc_wr_;
        void *psc_ud_;
        void *pnh_wr_ud_;
        void *pvh_wr_ud_;
        void *plch_wr_ud_;
        void *pih_wr_ud_;
        void *pstarth_wr_ud_;
        void *pstoph_wr_ud_;
        void *ptoah_wr_ud_;
        void *peh_wr_ud_;
        void *pdbh_wr_ud_;
};

extern "C" {

    peer_wr peer_create()
    {
        return new c_peer();
    }

    void peer_destroy(peer_wr p)
    {
        delete static_cast<c_peer *>(p);
    }

    RetCode peer_set_params_file_dir(peer_wr p, const char *dir)
    {
        return static_cast<peer *>(p)->set_params_file_dir(dir);
    }

    RetCode peer_set_params_file_path_name(peer_wr p, const char *file_path)
    {
        return static_cast<peer *>(p)->set_params_file_path_name(file_path);
    }

    const char *peer_get_name(peer_wr p)
    {
        return static_cast<peer *>(p)->get_name();
    }

    const unsigned int *peer_get_version(peer_wr p)
    {
        return static_cast<peer *>(p)->get_version();
    }

    unsigned int peer_get_version_major(peer_wr p)
    {
        return static_cast<peer *>(p)->get_version_major();
    }

    unsigned int peer_get_version_minor(peer_wr p)
    {
        return static_cast<peer *>(p)->get_version_minor();
    }

    unsigned int peer_get_version_maintenance(peer_wr p)
    {
        return static_cast<peer *>(p)->get_version_maintenance();
    }

    unsigned int peer_get_version_architecture(peer_wr p)
    {
        return static_cast<peer *>(p)->get_version_architecture();
    }

    int peer_is_configured(peer_wr p)
    {
        return static_cast<peer *>(p)->is_configured();
    }

    const entity_manager_wr peer_get_entity_manager(peer_wr p)
    {
        const entity_manager &em = static_cast<peer *>(p)->get_entity_manager();
        return (const entity_manager_wr)&em;
    }

    entity_manager_wr peer_get_entity_manager_m(peer_wr p)
    {
        entity_manager &em = static_cast<peer *>(p)->get_entity_manager_m();
        return (entity_manager_wr)&em;
    }

    int peer_is_persistent(peer_wr p)
    {
        return static_cast<peer *>(p)->is_persistent() ? 1 : 0;
    }

    int peer_is_persistent_schema_creating(peer_wr p)
    {
        return static_cast<peer *>(p)->is_persistent_schema_creating() ? 1 : 0;
    }

    int peer_is_dropping_existing_schema(peer_wr p)
    {
        return static_cast<peer *>(p)->is_dropping_existing_schema() ? 1 : 0;
    }

    PeerPersonality peer_get_personality(peer_wr p)
    {
        return static_cast<peer *>(p)->get_personality();
    }

    sockaddr_in peer_get_server_sockaddr(peer_wr p)
    {
        return static_cast<peer *>(p)->get_server_sockaddr();
    }

    unsigned int peer_server_executor_count(peer_wr p)
    {
        return static_cast<peer *>(p)->server_executor_count();
    }

    unsigned int peer_client_executor_count(peer_wr p)
    {
        return static_cast<peer *>(p)->client_executor_count();
    }

    unsigned int peer_server_sbs_executor_count(peer_wr p)
    {
        return static_cast<peer *>(p)->server_sbs_executor_count();
    }

    void peer_set_personality(peer_wr p, PeerPersonality personality)
    {
        static_cast<peer *>(p)->set_personality(personality);
    }

    void peer_add_load_model(peer_wr p, const char *model)
    {
        static_cast<peer *>(p)->add_load_model(model);
    }

    void peer_set_srv_sin_addr(peer_wr p, const char *address)
    {
        static_cast<peer *>(p)->set_srv_sin_addr(address);
    }

    void peer_set_sin_port(peer_wr p, int port)
    {
        static_cast<peer *>(p)->set_sin_port(port);
    }

    void peer_set_srv_executors(peer_wr p, unsigned int server_executors)
    {
        static_cast<peer *>(p)->set_srv_executors(server_executors);
    }

    void peer_set_cli_executors(peer_wr p, unsigned int client_executors)
    {
        static_cast<peer *>(p)->set_cli_executors(client_executors);
    }

    void peer_set_srv_sbs_executors(peer_wr p, unsigned int srv_sbs_executors)
    {
        static_cast<peer *>(p)->set_srv_sbs_executors(srv_sbs_executors);
    }

    void peer_set_persistent(peer_wr p, int persistent)
    {
        static_cast<peer *>(p)->set_persistent(persistent ? true : false);
    }

    void peer_set_persistent_schema_creating(peer_wr p,
                                             int persistent_schema_create)
    {
        static_cast<peer *>(p)->set_persistent_schema_creating(persistent_schema_create
                                                               ? true : false);
    }

    void peer_set_dropping_existing_schema(peer_wr p, int drop_existing_schema)
    {
        static_cast<peer *>(p)->set_dropping_existing_schema(drop_existing_schema ?
                                                             true : false);
    }

    void peer_add_load_persistent_driver(peer_wr p, const char *driver)
    {
        static_cast<peer *>(p)->add_load_persistent_driver(driver);
    }

    RetCode peer_extend_model_with_em(peer_wr p, entity_manager_wr em)
    {
        return static_cast<peer *>(p)->extend_model(static_cast<entity_manager *>(em));
    }

    RetCode peer_extend_model_with_model_name(peer_wr p, const char *model_name)
    {
        return static_cast<peer *>(p)->extend_model(model_name);
    }

    void peer_set_name_handler(peer_wr p, peer_name_handler_wr hndl, void *ud)
    {
        static_cast<c_peer *>(p)->Pnh_wr(hndl);
        static_cast<c_peer *>(p)->Pnh_wr_ud(ud);
    }

    void peer_set_version_handler(peer_wr p, peer_version_handler_wr hndl, void *ud)
    {
        static_cast<c_peer *>(p)->Pvh_wr(hndl);
        static_cast<c_peer *>(p)->Pvh_wr_ud(ud);
    }

    void peer_set_load_config_handler(peer_wr p, peer_load_config_handler_wr hndl,
                                      void *ud)
    {
        static_cast<c_peer *>(p)->Plch_wr(hndl);
        static_cast<c_peer *>(p)->Plch_wr_ud(ud);
    }

    void peer_set_init_handler(peer_wr p, peer_init_handler_wr hndl, void *ud)
    {
        static_cast<c_peer *>(p)->Pih_wr(hndl);
        static_cast<c_peer *>(p)->Pih_wr_ud(ud);
    }

    void peer_set_starting_handler(peer_wr p, peer_starting_handler_wr hndl,
                                   void *ud)
    {
        static_cast<c_peer *>(p)->Pstarth_wr(hndl);
        static_cast<c_peer *>(p)->Pstarth_wr_ud(ud);
    }

    void peer_set_stopping_handler(peer_wr p, peer_stopping_handler_wr hndl,
                                   void *ud)
    {
        static_cast<c_peer *>(p)->Pstoph_wr(hndl);
        static_cast<c_peer *>(p)->Pstoph_wr_ud(ud);
    }

    void peer_set_transit_on_air_handler(peer_wr p,
                                         peer_transit_on_air_handler_wr hndl, void *ud)
    {
        static_cast<c_peer *>(p)->Ptoah_wr(hndl);
        static_cast<c_peer *>(p)->Ptoah_wr_ud(ud);
    }

    void peer_set_error_handler(peer_wr p, peer_error_handler_wr hndl, void *ud)
    {
        static_cast<c_peer *>(p)->Peh_wr(hndl);
        static_cast<c_peer *>(p)->Peh_wr_ud(ud);
    }

    void peer_set_dying_breath_handler(peer_wr p, peer_dying_breath_handler_wr hndl,
                                       void *ud)
    {
        static_cast<c_peer *>(p)->Pdbh_wr(hndl);
        static_cast<c_peer *>(p)->Pdbh_wr_ud(ud);
    }

    void peer_set_configured(peer_wr p, int configured)
    {
        static_cast<peer *>(p)->set_configured(configured ? true : false);
    }

    PeerStatus peer_get_status(peer_wr p)
    {
        return static_cast<peer *>(p)->get_status();
    }

    void peer_set_status_change_handler(peer_wr p, peer_status_change_wr handler,
                                        void *ud)
    {
        static_cast<c_peer *>(p)->Psc_wr(handler);
        static_cast<c_peer *>(p)->Psc_ud(ud);
    }

    RetCode peer_await_for_status_reached_or_outdated(peer_wr p,
                                                      PeerStatus test,
                                                      PeerStatus *current,
                                                      time_t sec,
                                                      long nsec)
    {
        return static_cast<peer *>(p)->await_for_status_reached_or_outdated(test,
                                                                            *current, sec, nsec);
    }

    RetCode peer_await_for_status_change(peer_wr p,
                                         PeerStatus *peer_status,
                                         time_t sec,
                                         long nsec)
    {
        return static_cast<peer *>(p)->await_for_status_change(*peer_status, sec, nsec);
    }

    RetCode peer_start(peer_wr p, int argc, char *argv[], int spawn_new_thread)
    {
        return static_cast<peer *>(p)->start(argc, argv,
                                             spawn_new_thread ? true : false);
    }

    RetCode peer_stop(peer_wr p, int force_disconnect /*= false*/)
    {
        return static_cast<peer *>(p)->stop(force_disconnect ? true : false);
    }

    RetCode peer_persistence_schema_create(peer_wr p,
                                           PersistenceAlteringMode mode)
    {
        return static_cast<peer *>(p)->persistence_schema_create(mode);
    }

    RetCode peer_class_persistence_schema_create(peer_wr p,
                                                 PersistenceAlteringMode mode, unsigned int nclass_id)
    {
        return static_cast<peer *>(p)->class_persistence_schema_create(mode, nclass_id);
    }

    RetCode peer_class_persistent_load(peer_wr p, unsigned short class_key,
                                       unsigned int *ts_0_out, unsigned int *ts_1_out,
                                       net_class_wr in_out_obj)
    {
        return static_cast<peer *>(p)->class_persistent_load(class_key, *ts_0_out,
                                                             *ts_1_out, *(nclass *)in_out_obj);
    }

    RetCode peer_class_persistent_save(peer_wr p, net_class_wr in_obj)
    {
        return static_cast<peer *>(p)->class_persistent_save(*(nclass *)in_obj);
    }

    RetCode peer_class_persistent_update(peer_wr p, unsigned short class_key,
                                         net_class_wr in_obj)
    {
        return static_cast<peer *>(p)->class_persistent_update(class_key,
                                                               *(nclass *)in_obj);
    }

    RetCode peer_class_persistent_update_or_save(peer_wr p,
                                                 unsigned short class_key,
                                                 net_class_wr in_obj)
    {
        return static_cast<peer *>(p)->class_persistent_update_or_save(class_key,
                                                                       *(nclass *)in_obj);
    }

    RetCode peer_class_persistent_remove(peer_wr p, unsigned short class_key,
                                         PersistenceDeletionMode mode,
                                         net_class_wr in_obj)
    {
        return static_cast<peer *>(p)->class_persistent_remove(class_key, mode,
                                                               *(nclass *)in_obj);
    }

    RetCode peer_class_distribute(peer_wr p,
                                  SubscriptionEventType event_type,
                                  Action action, net_class_wr in_obj)
    {
        return static_cast<peer *>(p)->class_distribute(event_type, action,
                                                        *(nclass *)in_obj);
    }

    RetCode peer_class_persistent_save_and_distribute(peer_wr p,
                                                      net_class_wr in_obj)
    {
        return static_cast<peer *>(p)->class_persistent_save_and_distribute(*
                                                                            (nclass *)in_obj);
    }

    RetCode peer_class_persistent_update_and_distribute(peer_wr p,
                                                        unsigned short class_key, net_class_wr in_obj)
    {
        return static_cast<peer *>(p)->class_persistent_update_and_distribute(class_key,
                                                                              *(nclass *)in_obj);
    }

    RetCode peer_class_persistent_update_or_save_and_distribute(peer_wr p,
                                                                unsigned short class_key, net_class_wr in_obj)
    {
        return static_cast<peer *>(p)->class_persistent_update_or_save_and_distribute(
                   class_key, *(nclass *)in_obj);
    }

    RetCode peer_class_persistent_remove_and_distribute(peer_wr p,
                                                        unsigned short class_key,
                                                        PersistenceDeletionMode mode,
                                                        net_class_wr in_obj)
    {
        return static_cast<peer *>(p)->class_persistent_remove_and_distribute(class_key,
                                                                              mode, *(nclass *)in_obj);
    }

}
}
