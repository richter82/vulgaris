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

#ifndef VLG_CPP_PEER_H_
#define VLG_CPP_PEER_H_
#include "vlg.h"
#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#ifdef __GNUG__
#define SOCKET int
#define INVALID_SOCKET (~0)
#define SOCKET_ERROR   (-1)
#endif

namespace vlg {

/** @brief peer class.
*/
class peer_impl_pub;
class peer {
    public:
        typedef void (*peer_status_change)(peer &p,
                                           PeerStatus status,
                                           void *ud);

    public:
        explicit peer();
        virtual ~peer();

        vlg::RetCode set_params_file_dir(const char *dir);
        vlg::RetCode set_params_file_path_name(const char *file_path);

    public:
        const char              *get_name();
        const unsigned int      *get_version();
        unsigned int            get_version_major();
        unsigned int            get_version_minor();
        unsigned int            get_version_maintenance();
        unsigned int            get_version_architecture();
        bool                    is_configured();
        const entity_manager    &get_entity_manager()               const;
        entity_manager          &get_entity_manager_m();
        bool                    is_persistent();
        bool                    is_persistent_schema_creating();
        bool                    is_dropping_existing_schema();

        // CONFIG GETTERS
    public:
        PeerPersonality         get_personality();
        sockaddr_in             get_server_sockaddr();
        unsigned int            server_executor_count();
        unsigned int            client_executor_count();
        unsigned int            server_sbs_executor_count();

        // CONFIG SETTERS
    public:
        void set_personality(PeerPersonality personality);
        void add_load_model(const char *model);
        void set_srv_sin_addr(const char *address);
        void set_sin_port(int port);
        void set_srv_executors(unsigned int server_executors);
        void set_cli_executors(unsigned int client_executors);
        void set_srv_sbs_executors(unsigned int srv_sbs_executors);
        void set_persistent(bool persistent);
        void set_persistent_schema_creating(bool persistent_schema_create);
        void set_dropping_existing_schema(bool drop_existing_schema);
        void add_load_persistent_driver(const char *driver);
        void set_configured(bool configured);

    public:
        vlg::RetCode    extend_model(entity_manager *em);
        vlg::RetCode    extend_model(const char *model_name);

        // AUTOMA - User mandatory entrypoints
    public:
        virtual const char             *name_handler() = 0;
        virtual const unsigned int     *version_handler() = 0;

        // AUTOMA - User opt. entrypoints
    public:
        virtual vlg::RetCode  on_load_config(int pnum,
                                             const char *param,
                                             const char *value);

        virtual vlg::RetCode  on_init();
        virtual vlg::RetCode  on_starting();
        virtual vlg::RetCode  on_stopping();
        virtual vlg::RetCode  on_transit_on_air();
        virtual void            on_error();
        virtual void            on_dying_breath();

    public:
        PeerStatus              get_status();

    public:
        void set_status_change_handler(peer_status_change handler,
                                       void *ud);

    public:
        vlg::RetCode
        await_for_status_reached_or_outdated(PeerStatus test,
                                             PeerStatus &current,
                                             time_t sec = -1,
                                             long nsec = 0);

        vlg::RetCode await_for_status_change(PeerStatus &peer_status,
                                             time_t sec = -1,
                                             long nsec = 0);
        // SERVER-CONNECTIVITY
    public:
        connection_factory *get_connection_factory();
        void set_connection_factory(connection_factory &conn_factory);

        // ACTIONS
    public:
        vlg::RetCode start(int argc,
                           char *argv[],
                           bool spawn_new_thread);

        vlg::RetCode stop(bool force_disconnect = false);

        // PERSISTENCE
    public:
        vlg::RetCode
        persistence_schema_create(PersistenceAlteringMode mode);

        vlg::RetCode
        class_persistence_schema_create(PersistenceAlteringMode mode,
                                        unsigned int nclass_id);

        vlg::RetCode class_persistent_load(unsigned short class_key,
                                           unsigned int &ts_0_out,
                                           unsigned int &ts_1_out,
                                           nclass &in_out_obj);

        vlg::RetCode class_persistent_save(const nclass &in_obj);

        vlg::RetCode class_persistent_update(unsigned short class_key,
                                             const nclass &in_obj);

        vlg::RetCode class_persistent_update_or_save(unsigned short class_key,
                                                     const nclass &in_obj);

        vlg::RetCode class_persistent_remove(unsigned short class_key,
                                             PersistenceDeletionMode mode,
                                             const nclass &in_obj);

        // DISTRIBUTION
    public:
        vlg::RetCode class_distribute(SubscriptionEventType event_type,
                                      Action action,
                                      const nclass &in_obj);

        // PERSISTENCE + DISTRIBUTION
    public:
        vlg::RetCode
        class_persistent_save_and_distribute(const nclass &in_obj);

        vlg::RetCode
        class_persistent_update_and_distribute(unsigned short class_key,
                                               const nclass &in_obj);

        vlg::RetCode class_persistent_update_or_save_and_distribute(
            unsigned short class_key,
            const nclass &in_obj);

        vlg::RetCode
        class_persistent_remove_and_distribute(unsigned short class_key,
                                               PersistenceDeletionMode mode,
                                               const nclass &in_obj);

    public:
        /**
        You will override this method when you are implementing your own
        server-peer and you want to control the incoming connections.
        This method will be called when a new incoming connection is being
        establishing between the client and this server peer. You can use this
        handler to accept or reject the incoming connection by returning
        respectively RetCode_OK to accept connection, or another value
        to reject it.

        @param incoming_connection the brand new incoming connection.
        @return default implementation always returns RetCode_OK,
                so all incoming will be accepted.
        */
        virtual
        vlg::RetCode
        on_new_incoming_connection(connection &incoming_connection);

    public:
        peer_impl *get_implernal();

    private:
        peer_impl_pub *impl_;
};


}

#endif