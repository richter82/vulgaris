/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
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
