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

#pragma once
#include "vlg.h"

namespace vlg {

/** @brief peer.
*/
struct peer {
    explicit peer();
    virtual ~peer();

    RetCode set_params_file_dir(const char *);
    RetCode set_params_file_path_name(const char *);

    unsigned int get_version_major();
    unsigned int get_version_minor();
    unsigned int get_version_maintenance();
    unsigned int get_version_architecture();
    bool is_configured();
    const nentity_manager &get_entity_manager() const;
    nentity_manager &get_entity_manager_m();
    bool is_persistent();
    bool is_create_persistent_schema();
    bool is_drop_existing_persistent_schema();

    PeerPersonality get_personality();
    sockaddr_in get_server_sockaddr();
    unsigned int get_server_transaction_service_executor_size();
    unsigned int get_server_subscription_service_executor_size();
    unsigned int get_client_transaction_service_executor_size();

    void set_personality(PeerPersonality);
    void set_server_address(const char *);
    void set_server_port(int);
    void set_server_transaction_service_executor_size(unsigned int);
    void set_server_subscription_service_executor_size(unsigned int);
    void set_client_transaction_service_executor_size(unsigned int);
    void set_persistent(bool);
    void set_create_persistent_schema(bool);
    void set_drop_existing_persistent_schema(bool);
    void set_configured(bool);
    void add_load_model(const char *);
    void add_load_persistent_driver(const char *);

    RetCode extend_model(nentity_manager &nem);
    RetCode extend_model(const char *model_name);

    //User mandatory entrypoints
    virtual const char *get_name() = 0;
    virtual const unsigned int *get_version() = 0;

    //User opt. entrypoints
    virtual RetCode on_load_config(int pnum,
                                   const char *param,
                                   const char *value);

    virtual RetCode on_init();
    virtual RetCode on_starting();
    virtual RetCode on_stopping();
    virtual RetCode on_move_running();
    virtual RetCode on_error();
    virtual void on_dying_breath();
    virtual void on_status_change(PeerStatus);

    PeerStatus get_status();

    RetCode await_for_status_reached_or_outdated(PeerStatus test,
                                                 PeerStatus &current,
                                                 time_t sec = -1,
                                                 long nsec = 0);

    RetCode await_for_status_change(PeerStatus &peer_status,
                                    time_t sec = -1,
                                    long nsec = 0);

    incoming_connection_factory &get_incoming_connection_factory();
    void set_incoming_connection_factory(incoming_connection_factory &);

    RetCode start(int argc,
                  char *argv[],
                  bool peer_automa_on_new_thread);

    RetCode stop(bool force_disconnect = false);

    RetCode create_persistent_schema(PersistenceAlteringMode);

    RetCode nclass_create_persistent_schema(PersistenceAlteringMode mode,
                                            unsigned int nclass_id);

    RetCode obj_load(unsigned short nclass_key,
                     unsigned int &ts_0_out,
                     unsigned int &ts_1_out,
                     nclass &in_out);

    RetCode obj_save(const nclass &in);

    RetCode obj_update(unsigned short nclass_key,
                       const nclass &in);

    RetCode obj_update_or_save(unsigned short nclass_key,
                               const nclass &in);

    RetCode obj_remove(unsigned short nclass_key,
                       PersistenceDeletionMode mode,
                       const nclass &in);

    RetCode obj_distribute(SubscriptionEventType event_type,
                           Action action,
                           const nclass &in);

    RetCode obj_save_and_distribute(const nclass &in);

    RetCode obj_update_and_distribute(unsigned short nclass_key,
                                      const nclass &in);

    RetCode obj_update_or_save_and_distribute(unsigned short nclass_key,
                                              const nclass &in);

    RetCode obj_remove_and_distribute(unsigned short nclass_key,
                                      PersistenceDeletionMode mode,
                                      const nclass &in);

    /**
    @param incoming_connection the brand new incoming connection.
    @return default implementation always returns RetCode_OK,
    so all incoming will be accepted.
    */
    virtual RetCode on_incoming_connection(std::shared_ptr<incoming_connection> &);

    std::unique_ptr<peer_impl> impl_;
};


}
