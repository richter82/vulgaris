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

#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include "vlg.h"

#if defined(__cplusplus)
using namespace vlg;
extern "C" {
#endif

typedef void (*peer_status_change_wr)(peer_wr p,
                                      PeerStatus status,
                                      void *ud);

typedef const char  *(*peer_name_handler_wr)(peer_wr p,
                                             void *ud);

typedef const unsigned int  *(*peer_version_handler_wr)(peer_wr p,
                                                        void *ud);

typedef RetCode(*peer_load_config_handler_wr)(peer_wr p,
                                              int pnum,
                                              const char *param,
                                              const char *value,
                                              void *ud);

typedef RetCode(*peer_init_handler_wr)(peer_wr p,
                                       void *ud);

typedef RetCode(*peer_starting_handler_wr)(peer_wr p,
                                           void *ud);

typedef RetCode(*peer_stopping_handler_wr)(peer_wr p,
                                           void *ud);

typedef RetCode(*peer_transit_on_air_handler_wr)(peer_wr p,
                                                 void *ud);

typedef void(*peer_error_handler_wr)(peer_wr p,
                                     void *ud);

typedef void(*peer_dying_breath_handler_wr)(peer_wr p,
                                            void *ud);

peer_wr peer_create(void);
void    peer_destroy(peer_wr p);

RetCode    peer_set_params_file_dir(peer_wr p,
                                    const char *dir);

RetCode    peer_set_params_file_path_name(peer_wr p,
                                          const char *file_path);

const char         *peer_get_name(peer_wr p);
const unsigned int *peer_get_version(peer_wr p);
unsigned int        peer_get_version_major(peer_wr p);
unsigned int        peer_get_version_minor(peer_wr p);
unsigned int        peer_get_version_maintenance(peer_wr p);
unsigned int        peer_get_version_architecture(peer_wr p);
int                 peer_is_configured(peer_wr p);

const nentity_manager_wr   peer_get_entity_manager(peer_wr p);
nentity_manager_wr         peer_get_entity_manager_m(peer_wr p);
int                       peer_is_persistent(peer_wr p);
int                       peer_is_persistent_schema_creating(peer_wr p);
int                       peer_is_dropping_existing_schema(peer_wr p);

PeerPersonality         peer_get_personality(peer_wr p);
struct sockaddr_in      peer_get_server_sockaddr(peer_wr p);
unsigned int            peer_server_executor_count(peer_wr p);
unsigned int            peer_client_executor_count(peer_wr p);
unsigned int            peer_server_sbs_executor_count(peer_wr p);

void peer_set_personality(peer_wr p, PeerPersonality personality);

void peer_add_load_model(peer_wr p,
                         const char *model);

void peer_set_srv_sin_addr(peer_wr p,
                           const char *address);

void peer_set_sin_port(peer_wr p,
                       int port);

void peer_set_srv_executors(peer_wr p,
                            unsigned int server_executors);

void peer_set_cli_executors(peer_wr p,
                            unsigned int client_executors);

void peer_set_srv_sbs_executors(peer_wr p,
                                unsigned int srv_sbs_executors);

void peer_set_persistent(peer_wr p,
                         int persistent);

void peer_set_persistent_schema_creating(peer_wr p,
                                         int persistent_schema_create);

void peer_set_dropping_existing_schema(peer_wr p,
                                       int drop_existing_schema);

void peer_add_load_persistent_driver(peer_wr p,
                                     const char *driver);

RetCode peer_extend_model_with_em(peer_wr p,
                                  nentity_manager_wr nem);

RetCode peer_extend_model_with_model_name(peer_wr p,
                                          const char *model_name);

void peer_set_name_handler(peer_wr p,
                           peer_name_handler_wr hndl,
                           void *ud);

void peer_set_version_handler(peer_wr p,
                              peer_version_handler_wr hndl,
                              void *ud);

void peer_set_load_config_handler(peer_wr p,
                                  peer_load_config_handler_wr hndl,
                                  void *ud);

void peer_set_init_handler(peer_wr p,
                           peer_init_handler_wr hndl,
                           void *ud);

void peer_set_starting_handler(peer_wr p,
                               peer_starting_handler_wr hndl,
                               void *ud);

void peer_set_stopping_handler(peer_wr p,
                               peer_stopping_handler_wr hndl,
                               void *ud);

void peer_set_transit_on_air_handler(peer_wr p,
                                     peer_transit_on_air_handler_wr hndl,
                                     void *ud);

void peer_set_error_handler(peer_wr p,
                            peer_error_handler_wr hndl,
                            void *ud);

void peer_set_dying_breath_handler(peer_wr p,
                                   peer_dying_breath_handler_wr hndl,
                                   void *ud);

void peer_set_configured(peer_wr p,
                         int configured);

PeerStatus peer_get_status(peer_wr p);

void peer_set_status_change_handler(peer_wr p,
                                    peer_status_change_wr handler,
                                    void *ud);

RetCode peer_await_for_status_reached_or_outdated(peer_wr p,
                                                  PeerStatus test,
                                                  PeerStatus *current,
                                                  time_t sec,
                                                  long nsec);

RetCode peer_await_for_status_change(peer_wr p,
                                     PeerStatus *peer_status,
                                     time_t sec,
                                     long nsec);

RetCode peer_start(peer_wr p,
                   int argc,
                   char *argv[],
                   int spawn_new_thread);

RetCode peer_stop(peer_wr p,
                  int force_disconnect);

RetCode peer_persistence_schema_create(peer_wr p,
                                       PersistenceAlteringMode mode);

RetCode peer_class_persistence_schema_create(peer_wr p,
                                             PersistenceAlteringMode mode,
                                             unsigned int nclass_id);

RetCode peer_class_persistent_load(peer_wr p,
                                   unsigned short class_key,
                                   unsigned int *ts_0_out,
                                   unsigned int *ts_1_out,
                                   nclass_wr in_out_obj);

RetCode peer_class_persistent_save(peer_wr p,
                                   nclass_wr in_obj);

RetCode peer_class_persistent_update(peer_wr p,
                                     unsigned short class_key,
                                     nclass_wr in_obj);

RetCode peer_class_persistent_update_or_save(peer_wr p,
                                             unsigned short class_key,
                                             nclass_wr in_obj);

RetCode peer_class_persistent_remove(peer_wr p,
                                     unsigned short class_key,
                                     PersistenceDeletionMode mode,
                                     nclass_wr in_obj);

RetCode peer_class_distribute(peer_wr p,
                              SubscriptionEventType event_type,
                              Action action,
                              nclass_wr in_obj);

RetCode peer_class_persistent_save_and_distribute(peer_wr p,
                                                  nclass_wr in_obj);

RetCode peer_class_persistent_update_and_distribute(peer_wr p,
                                                    unsigned short class_key,
                                                    nclass_wr in_obj);

RetCode peer_class_persistent_update_or_save_and_distribute(peer_wr p,
                                                            unsigned short class_key,
                                                            nclass_wr in_obj);

RetCode peer_class_persistent_remove_and_distribute(peer_wr p,
                                                    unsigned short class_key,
                                                    PersistenceDeletionMode mode,
                                                    nclass_wr in_obj);

#if defined(__cplusplus)
}
#endif

#endif
