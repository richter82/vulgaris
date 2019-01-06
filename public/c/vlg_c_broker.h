/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#ifndef VLG_C_BROKER_H_
#define VLG_C_BROKER_H_
#include "vlg.h"

/************************************************************************
BROKER HANDLERS
************************************************************************/

typedef void (*broker_on_status_change)(broker *p, PeerStatus status, void *ud, void *ud2);
typedef const char *(*broker_name)(broker *p, void *ud, void *ud2);
typedef const unsigned int *(*broker_version)(broker *p, void *ud, void *ud2);
typedef RetCode(*broker_on_load_config)(broker *p, int pnum, const char *param, const char *value, void *ud, void *ud2);
typedef RetCode(*broker_on_init)(broker *p, void *ud, void *ud2);
typedef RetCode(*broker_on_starting)(broker *p, void *ud, void *ud2);
typedef RetCode(*broker_on_stopping)(broker *p, void *ud, void *ud2);
typedef RetCode(*broker_on_move_running)(broker *p, void *ud, void *ud2);
typedef void(*broker_on_error)(broker *p, void *ud, void *ud2);
typedef RetCode(*broker_on_incoming_connection)(broker *p, shr_incoming_connection *ic, void *ud, void *ud2);

void broker_set_name(broker *p, broker_name hndl, void *ud, void *ud2);
void broker_set_version(broker *p, broker_version hndl, void *ud, void *ud2);
void broker_set_on_load_config(broker *p, broker_on_load_config hndl, void *ud, void *ud2);
void broker_set_on_init(broker *p, broker_on_init hndl, void *ud, void *ud2);
void broker_set_on_starting(broker *p, broker_on_starting hndl, void *ud, void *ud2);
void broker_set_on_stopping(broker *p, broker_on_stopping hndl, void *ud, void *ud2);
void broker_set_on_move_running(broker *p, broker_on_move_running hndl, void *ud, void *ud2);
void broker_set_on_error(broker *p, broker_on_error hndl, void *ud, void *ud2);
void broker_set_on_status_change(broker *p, broker_on_status_change hndl, void *ud, void *ud2);
void broker_set_on_incoming_connection(broker *p, broker_on_incoming_connection hndl, void *ud, void *ud2);

/************************************************************************
BROKER
************************************************************************/

own_broker *broker_create(void);
void broker_destroy(own_broker *p);
broker *own_broker_get_ptr(own_broker *p);
RetCode broker_set_params_file_dir(broker *p, const char *dir);
RetCode broker_set_params_file_path_name(broker *p, const char *file_path);
const char *broker_get_name(broker *p);
const unsigned int *broker_get_version(broker *p);
unsigned int broker_get_version_major(broker *p);
unsigned int broker_get_version_minor(broker *p);
unsigned int broker_get_version_maintenance(broker *p);
unsigned int broker_get_version_architecture(broker *p);
int broker_is_configured(broker *p);
const nentity_manager *broker_get_nentity_manager(broker *p);
int broker_is_persistent(broker *p);
int broker_is_persistent_schema_creating(broker *p);
int broker_is_dropping_existing_schema(broker *p);
PeerPersonality broker_get_personality(broker *p);
struct sockaddr_in broker_get_server_sockaddr(broker *p);
unsigned int broker_server_executor_count(broker *p);
unsigned int broker_client_executor_count(broker *p);
unsigned int broker_server_sbs_executor_count(broker *p);
void broker_set_personality(broker *p, PeerPersonality personality);
void broker_add_load_model(broker *p, const char *model);
void broker_set_srv_sin_addr(broker *p, const char *address);
void broker_set_sin_port(broker *p, int port);
void broker_set_srv_executors(broker *p, unsigned int server_executors);
void broker_set_cli_executors(broker *p, unsigned int client_executors);
void broker_set_srv_sbs_executors(broker *p, unsigned int srv_sbs_executors);
void broker_set_persistent(broker *p, int persistent);
void broker_set_persistent_schema_creating(broker *p, int persistent_schema_create);
void broker_set_dropping_existing_schema(broker *p, int drop_existing_schema);
void broker_add_load_persistent_driver(broker *p, const char *driver);
RetCode broker_extend_model_with_nem(broker *p, nentity_manager *nem);
RetCode broker_extend_model_with_model_name(broker *p, const char *model_name);
void broker_set_configured(broker *p, int configured);
PeerStatus broker_get_status(broker *p);
RetCode broker_await_for_status_reached(broker *p, PeerStatus test, PeerStatus *current, time_t sec, long nsec);
RetCode broker_await_for_status_change(broker *p, PeerStatus *broker_status, time_t sec, long nsec);
RetCode broker_start(broker *p, int argc, char *argv[], int spawn_new_thread);
RetCode broker_stop(broker *p, int force_disconnect);
RetCode broker_persistence_schema_create(broker *p, PersistenceAlteringMode mode);
RetCode broker_nclass_persistence_schema_create(broker *p, PersistenceAlteringMode mode, unsigned int nclass_id);
RetCode broker_nclass_persistent_load(broker *p, unsigned short nclass_key, unsigned int *ts_0_out, unsigned int *ts_1_out, nclass *in_out);
RetCode broker_nclass_persistent_save(broker *p, nclass *in);
RetCode broker_nclass_persistent_update(broker *p, unsigned short nclass_key, nclass *in);
RetCode broker_nclass_persistent_update_or_save(broker *p, unsigned short nclass_key, nclass *in);
RetCode broker_nclass_persistent_remove(broker *p, unsigned short nclass_key, PersistenceDeletionMode mode, nclass *in);
RetCode broker_nclass_distribute(broker *p, SubscriptionEventType event_type, Action action, nclass *in);
RetCode broker_nclass_persistent_save_and_distribute(broker *p, nclass *in);
RetCode broker_nclass_persistent_update_and_distribute(broker *p, unsigned short nclass_key, nclass *in);
RetCode broker_nclass_persistent_update_or_save_and_distribute(broker *p, unsigned short nclass_key, nclass *in);
RetCode broker_nclass_persistent_remove_and_distribute(broker *p, unsigned short nclass_key, PersistenceDeletionMode mode, nclass *in);

#endif
