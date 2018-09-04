/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "vlg_persistence.h"
using namespace vlg;

extern "C" {

    RetCode persistence_manager_load_persistence_driver_dynamic(const char *drivers[],
                                                                int drivers_num)
    {
        return persistence_manager::load_driver_dynamic(drivers, drivers_num);
    }

    RetCode persistence_manager_load_persistence_drivers(persistence_driver *drivers[],
                                                         int drivers_num)
    {
        return persistence_manager::load_driver(drivers, drivers_num);
    }

    RetCode persistence_manager_load_persistence_driver(persistence_driver *driver)
    {
        return persistence_manager::load_driver(&driver, 1);
    }

    RetCode persistence_manager_set_cfg_file_dir(const char *dir)
    {
        return persistence_manager::set_config_file_dir(dir);
    }

    RetCode persistence_manager_set_cfg_file_path_name(const char *file_path)
    {
        return persistence_manager::set_config_file_path_name(file_path);
    }

    RetCode persistence_manager_load_config(const char *file_name)
    {
        return persistence_manager::load_config(file_name);
    }

    RetCode persistence_manager_start_all_drivers()
    {
        return persistence_manager::start_all_drivers();
    }

    persistence_driver *persistence_manager_available_driver(unsigned int nclass_id)
    {
        return persistence_manager::available_driver(nclass_id);
    }

    RetCode persistence_connection_create_nclass_schema_by_nclass_id(persistence_connection *pconn,
                                                                     PersistenceAlteringMode mode,
                                                                     const nentity_manager *nem,
                                                                     unsigned int nclass_id)
    {
        return pconn->create_entity_schema(mode, *nem, nclass_id);
    }

    RetCode persistence_connection_create_nclass_schema_by_edesc(persistence_connection *pconn,
                                                                 PersistenceAlteringMode mode,
                                                                 const nentity_manager *nem,
                                                                 const nentity_desc *desc)
    {
        return pconn->create_entity_schema(mode, *nem, *desc);
    }

    RetCode persistence_connection_save_nclass(persistence_connection *pconn,
                                               const nentity_manager *nem,
                                               unsigned int ts_0,
                                               unsigned int ts_1,
                                               const nclass *in)
    {
        return pconn->save_obj(*(const nentity_manager *)nem, ts_0, ts_1, *in);
    }

    RetCode persistence_connection_update_nclass(persistence_connection *pconn,
                                                 unsigned short key,
                                                 const nentity_manager *nem,
                                                 unsigned int ts_0,
                                                 unsigned int ts_1,
                                                 const nclass *in)
    {
        return pconn->update_obj(key, *nem, ts_0, ts_1, *in);
    }

    RetCode persistence_connection_save_or_update_nclass(persistence_connection *pconn,
                                                         unsigned short key,
                                                         const nentity_manager *nem,
                                                         unsigned int ts_0,
                                                         unsigned int ts_1,
                                                         const nclass *in)
    {
        return pconn->save_or_update_obj(key, *nem, ts_0, ts_1, *in);
    }

    RetCode persistence_connection_remove_nclass(persistence_connection *pconn,
                                                 unsigned short key,
                                                 const nentity_manager *nem,
                                                 unsigned int ts_0,
                                                 unsigned int ts_1,
                                                 PersistenceDeletionMode mode,
                                                 const nclass *in)
    {
        return pconn->remove_obj(key, *nem, ts_0, ts_1, mode, *in);
    }

    RetCode persistence_connection_load_nclass(persistence_connection *pconn,
                                               unsigned short key,
                                               const nentity_manager *nem,
                                               unsigned int *ts0_out,
                                               unsigned int *ts1_out,
                                               nclass *in_out)
    {
        return pconn->load_obj(key, *nem, *ts0_out, *ts1_out, *in_out);
    }

    RetCode persistence_connection_execute_statement(persistence_connection *pconn,
                                                     const char *stmt,
                                                     unsigned int nclass_id)
    {
        return pconn->execute_statement(stmt, nclass_id);
    }

    RetCode persistence_query_bind(persistence_query *pqry,
                                   unsigned int nclass_id,
                                   const char *sql)
    {
        return pqry->execute(nclass_id, sql);
    }

    const nentity_manager *persistence_query_get_nentity_manager(persistence_query *pqry)
    {
        return &pqry->get_nentity_manager();
    }

    RetCode persistence_query_next_nclass(persistence_query *pqry,
                                          unsigned int *ts0_out,
                                          unsigned int *ts1_out,
                                          nclass *out)
    {
        return pqry->next_obj(*ts0_out, *ts1_out, *out);
    }
}
