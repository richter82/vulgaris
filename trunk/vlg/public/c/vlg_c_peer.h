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

#ifndef VLG_C_PEER_H_
#define VLG_C_PEER_H_
#include "vlg.h"

/************************************************************************
PEER HANDLERS
************************************************************************/

typedef void (*peer_status_change)(peer *p, PeerStatus status, void *ud);
typedef const char *(*peer_name_handler)(peer *p, void *ud);
typedef const unsigned int *(*peer_version_handler)(peer *p, void *ud);
typedef RetCode(*peer_load_config_handler)(peer *p, int pnum, const char *param, const char *value, void *ud);
typedef RetCode(*peer_init_handler)(peer *p, void *ud);
typedef RetCode(*peer_starting_handler)(peer *p, void *ud);
typedef RetCode(*peer_stopping_handler)(peer *p, void *ud);
typedef RetCode(*peer_on_move_running_handler)(peer *p, void *ud);
typedef RetCode(*peer_error_handler)(peer *p, void *ud);
typedef void(*peer_dying_breath_handler)(peer *p, void *ud);
typedef RetCode(*peer_on_incoming_connection_handler)(peer *p, shr_incoming_connection *ic, void *ud);

/************************************************************************
PEER
************************************************************************/

own_peer *peer_create(void);
void peer_destroy(own_peer *p);
peer *own_peer_get_ptr(own_peer *p);
RetCode peer_set_params_file_dir(peer *p, const char *dir);
RetCode peer_set_params_file_path_name(peer *p, const char *file_path);
const char *peer_get_name(peer *p);
const unsigned int *peer_get_version(peer *p);
unsigned int peer_get_version_major(peer *p);
unsigned int peer_get_version_minor(peer *p);
unsigned int peer_get_version_maintenance(peer *p);
unsigned int peer_get_version_architecture(peer *p);
int peer_is_configured(peer *p);
const nentity_manager *peer_get_nentity_manager(peer *p);
int peer_is_persistent(peer *p);
int peer_is_persistent_schema_creating(peer *p);
int peer_is_dropping_existing_schema(peer *p);
PeerPersonality peer_get_personality(peer *p);
struct sockaddr_in peer_get_server_sockaddr(peer *p);
unsigned int peer_server_executor_count(peer *p);
unsigned int peer_client_executor_count(peer *p);
unsigned int peer_server_sbs_executor_count(peer *p);
void peer_set_personality(peer *p, PeerPersonality personality);
void peer_add_load_model(peer *p, const char *model);
void peer_set_srv_sin_addr(peer *p, const char *address);
void peer_set_sin_port(peer *p, int port);
void peer_set_srv_executors(peer *p, unsigned int server_executors);
void peer_set_cli_executors(peer *p, unsigned int client_executors);
void peer_set_srv_sbs_executors(peer *p, unsigned int srv_sbs_executors);
void peer_set_persistent(peer *p, int persistent);
void peer_set_persistent_schema_creating(peer *p, int persistent_schema_create);
void peer_set_dropping_existing_schema(peer *p, int drop_existing_schema);
void peer_add_load_persistent_driver(peer *p, const char *driver);
RetCode peer_extend_model_with_nem(peer *p, nentity_manager *nem);
RetCode peer_extend_model_with_model_name(peer *p, const char *model_name);
void peer_set_name_handler(peer *p, peer_name_handler hndl, void *ud);
void peer_set_version_handler(peer *p, peer_version_handler hndl, void *ud);
void peer_set_load_config_handler(peer *p, peer_load_config_handler hndl, void *ud);
void peer_set_init_handler(peer *p, peer_init_handler hndl, void *ud);
void peer_set_starting_handler(peer *p, peer_starting_handler hndl, void *ud);
void peer_set_stopping_handler(peer *p, peer_stopping_handler hndl, void *ud);
void peer_set_on_move_running_handler(peer *p, peer_on_move_running_handler hndl, void *ud);
void peer_set_error_handler(peer *p, peer_error_handler hndl, void *ud);
void peer_set_dying_breath_handler(peer *p, peer_dying_breath_handler hndl, void *ud);
void peer_set_configured(peer *p, int configured);
void peer_set_status_change_handler(peer *p, peer_status_change handler, void *ud);
void peer_set_peer_on_incoming_connection_handler(peer *p, peer_on_incoming_connection_handler handler, void *ud);
PeerStatus peer_get_status(peer *p);
RetCode peer_await_for_status_reached_or_outdated(peer *p, PeerStatus test, PeerStatus *current, time_t sec, long nsec);
RetCode peer_await_for_status_change(peer *p, PeerStatus *peer_status, time_t sec, long nsec);
RetCode peer_start(peer *p, int argc, char *argv[], int spawn_new_thread);
RetCode peer_stop(peer *p, int force_disconnect);
RetCode peer_persistence_schema_create(peer *p, PersistenceAlteringMode mode);
RetCode peer_nclass_persistence_schema_create(peer *p, PersistenceAlteringMode mode, unsigned int nclass_id);
RetCode peer_nclass_persistent_load(peer *p, unsigned short nclass_key, unsigned int *ts_0_out, unsigned int *ts_1_out, nclass *in_out);
RetCode peer_nclass_persistent_save(peer *p, nclass *in);
RetCode peer_nclass_persistent_update(peer *p, unsigned short nclass_key, nclass *in);
RetCode peer_nclass_persistent_update_or_save(peer *p, unsigned short nclass_key, nclass *in);
RetCode peer_nclass_persistent_remove(peer *p, unsigned short nclass_key, PersistenceDeletionMode mode, nclass *in);
RetCode peer_nclass_distribute(peer *p, SubscriptionEventType event_type, Action action, nclass *in);
RetCode peer_nclass_persistent_save_and_distribute(peer *p, nclass *in);
RetCode peer_nclass_persistent_update_and_distribute(peer *p, unsigned short nclass_key, nclass *in);
RetCode peer_nclass_persistent_update_or_save_and_distribute(peer *p, unsigned short nclass_key, nclass *in);
RetCode peer_nclass_persistent_remove_and_distribute(peer *p, unsigned short nclass_key, PersistenceDeletionMode mode, nclass *in);

#endif
