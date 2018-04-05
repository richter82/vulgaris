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

//dyna
RetCode persistence_manager_load_persistence_driver_dynamic(const char *drivers[], int drivers_num);

//static
RetCode persistence_manager_load_persistence_driver(persistence_driver *drivers[], int drivers_num);

persistence_driver *persistence_manager_available_driver(unsigned int nclass_id);
RetCode persistence_manager_set_cfg_file_dir(const char *dir);
RetCode persistence_manager_set_cfg_file_path_name(const char *file_path);
RetCode persistence_manager_load_config(const char *file_name);
RetCode persistence_manager_start_all_drivers(void);
RetCode persistence_connection_create_nclass_schema_by_nclass_id(persistence_connection *pconn, PersistenceAlteringMode mode, const nentity_manager *nem, unsigned int nclass_id);
RetCode persistence_connection_create_nclass_schema_by_edesc(persistence_connection *pconn, PersistenceAlteringMode mode, const nentity_manager *nem, const nentity_desc *desc);
RetCode persistence_connection_save_nclass(persistence_connection *pconn, const nentity_manager *nem, unsigned int ts0, unsigned int ts1, const nclass *in);
RetCode persistence_connection_update_nclass(persistence_connection *pconn, unsigned short key, const nentity_manager *nem, unsigned int ts0, unsigned int ts1, const nclass *in);
RetCode persistence_connection_save_or_update_nclass(persistence_connection *pconn, unsigned short key, const nentity_manager *nem, unsigned int ts0, unsigned int ts1, const nclass *in);
RetCode persistence_connection_remove_nclass(persistence_connection *pconn, unsigned short key, const nentity_manager *nem, unsigned int ts0, unsigned int ts1, PersistenceDeletionMode mode, const nclass *in);
RetCode persistence_connection_load_nclass(persistence_connection *pconn, unsigned short key, const nentity_manager *nem, unsigned int *ts0_out, unsigned int *ts1_out, nclass *in_out);
RetCode persistence_connection_execute_statement(persistence_connection *pconn, const char *stmt, unsigned int nclass_id);
RetCode persistence_query_bind(persistence_query *pqry, unsigned int nclass_id, const char *sql);
const nentity_manager *persistence_query_get_nentity_manager(persistence_query *pqry);
RetCode persistence_query_next_nclass(persistence_query *pqry, unsigned int *ts0_out, unsigned int *ts1_out, nclass *out);

#endif
