/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#ifndef VLG_C_PEER_H_
#define VLG_C_PEER_H_
#include "vlg.h"

/************************************************************************
LOGGER
************************************************************************/

RetCode set_logger_cfg_file_dir(const char *dir);
RetCode set_logger_cfg_file_path_name(const char *file_path);
RetCode load_logger_cfg(void);

/************************************************************************
PEER HANDLERS
************************************************************************/

typedef void (*peer_on_status_change)(peer *p, PeerStatus status, void *ud, void *ud2);
typedef const char *(*peer_name)(peer *p, void *ud, void *ud2);
typedef const unsigned int *(*peer_version)(peer *p, void *ud, void *ud2);
typedef RetCode(*peer_on_load_config)(peer *p, int pnum, const char *param, const char *value, void *ud, void *ud2);
typedef RetCode(*peer_on_init)(peer *p, void *ud, void *ud2);
typedef RetCode(*peer_on_starting)(peer *p, void *ud, void *ud2);
typedef RetCode(*peer_on_stopping)(peer *p, void *ud, void *ud2);
typedef RetCode(*peer_on_move_running)(peer *p, void *ud, void *ud2);
typedef void(*peer_on_dying_breath)(peer *p, void *ud, void *ud2);
typedef RetCode(*peer_on_incoming_connection)(peer *p, shr_incoming_connection *ic, void *ud, void *ud2);

/************************************************************************
PEER
************************************************************************/

own_peer *peer_create(void);
void peer_destroy(own_peer *p);
peer *own_peer_get_ptr(own_peer *p);
logger *peer_get_logger(peer *p);
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
void peer_set_name(peer *p, peer_name hndl, void *ud, void *ud2);
void peer_set_version(peer *p, peer_version hndl, void *ud, void *ud2);
void peer_set_on_load_config(peer *p, peer_on_load_config hndl, void *ud, void *ud2);
void peer_set_on_init(peer *p, peer_on_init hndl, void *ud, void *ud2);
void peer_set_on_starting(peer *p, peer_on_starting hndl, void *ud, void *ud2);
void peer_set_on_stopping(peer *p, peer_on_stopping hndl, void *ud, void *ud2);
void peer_set_on_move_running(peer *p, peer_on_move_running hndl, void *ud, void *ud2);
void peer_set_on_dying_breath(peer *p, peer_on_dying_breath hndl, void *ud, void *ud2);
void peer_set_configured(peer *p, int configured);
void peer_set_on_status_change(peer *p, peer_on_status_change hndl, void *ud, void *ud2);
void peer_set_peer_on_incoming_connection(peer *p, peer_on_incoming_connection hndl, void *ud, void *ud2);
PeerStatus peer_get_status(peer *p);
RetCode peer_await_for_status_reached(peer *p, PeerStatus test, PeerStatus *current, time_t sec, long nsec);
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
