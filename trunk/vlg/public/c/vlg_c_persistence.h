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

#ifndef VLG_C_PERSISTENCE_H_
#define VLG_C_PERSISTENCE_H_
#include "vlg.h"

#if defined(__cplusplus)
using namespace vlg;
extern "C" {
#endif

/***************************
driver loading
***************************/
/*dyna*/
RetCode persistence_manager_load_persistence_driver_dynamic(const char *drivers[],
                                                            int drivers_num);

/*static*/
RetCode persistence_manager_load_persistence_driver(persistence_driver_impl_wr
                                                    drivers[],
                                                    int drivers_num);
//returns null if no driver is available.
persistence_driver_impl_wr persistence_manager_available_driver(unsigned int nclass_id);

RetCode persistence_manager_set_cfg_file_dir(const char *dir);

RetCode persistence_manager_set_cfg_file_path_name(const char *file_path);

RetCode persistence_manager_load_config(const char *file_name);

RetCode persistence_manager_start_all_drivers(void);

RetCode persistence_connection_bind(persistence_connection_wr pconn,
                                    unsigned int nclass_id,
                                    persistence_driver_impl_wr driver);

PersistenceConnectionStatus persistence_connection_get_status(
    persistence_connection_wr
    pconn);

unsigned int persistence_connection_get_id(persistence_connection_wr pconn);

RetCode persistence_connection_create_entity_schema_by_class_id(persistence_connection_wr pconn,
                                                                PersistenceAlteringMode mode,
                                                                const entity_manager_wr nem,
                                                                unsigned int nclass_id);

RetCode persistence_connection_create_entity_schema_by_edesc(persistence_connection_wr pconn,
                                                             PersistenceAlteringMode mode,
                                                             const entity_manager_wr nem,
                                                             const entity_desc_wr desc);

RetCode persistence_connection_save_entity(persistence_connection_wr pconn,
                                           const entity_manager_wr nem,
                                           unsigned int ts0,
                                           unsigned int ts1,
                                           const net_class_wr in_obj);

RetCode persistence_connection_update_entity(persistence_connection_wr pconn,
                                             unsigned short key,
                                             const entity_manager_wr nem,
                                             unsigned int ts0,
                                             unsigned int ts1,
                                             const net_class_wr in_obj);

RetCode persistence_connection_save_or_update_entity(persistence_connection_wr
                                                     pconn,
                                                     unsigned short key,
                                                     const entity_manager_wr nem,
                                                     unsigned int ts0,
                                                     unsigned int ts1,
                                                     const net_class_wr in_obj);

RetCode persistence_connection_remove_entity(persistence_connection_wr pconn,
                                             unsigned short key,
                                             const entity_manager_wr nem,
                                             unsigned int ts0,
                                             unsigned int ts1,
                                             PersistenceDeletionMode mode,
                                             const net_class_wr in_obj);

RetCode persistence_connection_load_entity(persistence_connection_wr pconn,
                                           unsigned short key,
                                           const entity_manager_wr nem,
                                           unsigned int *ts0_out,
                                           unsigned int *ts1_out,
                                           net_class_wr in_out_obj);

RetCode persistence_connection_execute_statement(persistence_connection_wr
                                                 pconn,
                                                 const char *stmt);

RetCode persistence_query_bind(persistence_query_wr pqry,
                               unsigned int nclass_id,
                               const char *sql);

unsigned int persistence_query_get_id(persistence_query_wr pqry);

PersistenceQueryStatus persistence_query_get_status(persistence_query_wr pqry);

const entity_manager_wr persistence_query_get_entity_manager(persistence_query_wr pqry);

persistence_connection_wr persistence_query_get_connection(persistence_query_wr pqry);

RetCode persistence_query_next_entity(persistence_query_wr pqry,
                                      unsigned int *ts0_out,
                                      unsigned int *ts1_out,
                                      net_class_wr out_obj);

#if defined(__cplusplus)
}
#endif

#endif
