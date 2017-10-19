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

#include "vlg_c_persistence.h"
#include "vlg_persistence.h"

namespace vlg {
extern "C" {

    RetCode persistence_manager_load_persistence_driver_dynamic(const char *drivers[],
    int drivers_num)
    {
        return persistence_manager::load_persistence_driver_dynamic(drivers,
        drivers_num);
    }

    RetCode persistence_manager_load_persistence_driver(persistence_driver_int_wr
    drivers[],
    int drivers_num)
    {
        return persistence_manager::load_persistence_driver((persistence_driver_int **)
        drivers, drivers_num);
    }

    RetCode persistence_manager_set_cfg_file_dir(const char *dir)
    {
        return persistence_manager::set_persistence_config_file_dir(dir);
    }

    RetCode persistence_manager_set_cfg_file_path_name(const char *file_path)
    {
        return persistence_manager::set_persistence_config_file_path_name(file_path);
    }

    RetCode persistence_manager_load_config(const char *file_name)
    {
        return persistence_manager::load_persistence_config(file_name);
    }

    RetCode persistence_manager_start_all_drivers()
    {
        return persistence_manager::start_all_persistence_drivers();
    }

    persistence_driver_int_wr persistence_manager_available_driver(
        unsigned int nclass_id)
    {
        return (persistence_driver_int *)persistence_manager::available_driver(
            nclass_id);
    }

    RetCode persistence_connection_bind(persistence_connection_wr pconn,
    unsigned int nclass_id,
    persistence_driver_int_wr driver)
    {
        return static_cast<persistence_connection *>(pconn)->bind(nclass_id,
        *(persistence_driver_int *)driver);
    }

    PersistenceConnectionStatus persistence_connection_get_status(
        persistence_connection_wr pconn)
    {
        return static_cast<persistence_connection *>(pconn)->get_status();
    }

    unsigned int persistence_connection_get_id(persistence_connection_wr pconn)
    {
        return static_cast<persistence_connection *>(pconn)->get_id();
    }

    RetCode persistence_connection_create_entity_schema_by_class_id(
        persistence_connection_wr pconn,
        PersistenceAlteringMode mode,
        const entity_manager_wr em,
        unsigned int nclass_id)
    {
        return static_cast<persistence_connection *>(pconn)->create_entity_schema(mode,
        *(const entity_manager *)em, nclass_id);
    }

    RetCode persistence_connection_create_entity_schema_by_edesc(
        persistence_connection_wr pconn,
        PersistenceAlteringMode mode,
        const entity_manager_wr em,
        const entity_desc_wr desc)
    {
        return static_cast<persistence_connection *>(pconn)->create_entity_schema(mode,
        *(const entity_manager *)em,
        *(const entity_desc *)desc);
    }

    RetCode persistence_connection_save_entity(persistence_connection_wr pconn,
    const entity_manager_wr em,
    unsigned int ts0,
    unsigned int ts1,
    const net_class_wr in_obj)
    {
        return static_cast<persistence_connection *>(pconn)->save_entity(*
        (const entity_manager *)em,
        ts0,
        ts1,
        *(const nclass *)in_obj);
    }

    RetCode persistence_connection_update_entity(persistence_connection_wr pconn,
    unsigned short key,
    const entity_manager_wr em,
    unsigned int ts0,
    unsigned int ts1,
    const net_class_wr in_obj)
    {
        return static_cast<persistence_connection *>(pconn)->update_entity(key,
        *(const entity_manager *)em,
        ts0,
        ts1,
        *(const nclass *)in_obj);
    }

    RetCode persistence_connection_save_or_update_entity(persistence_connection_wr
    pconn,
    unsigned short key,
    const entity_manager_wr em,
    unsigned int ts0,
    unsigned int ts1,
    const net_class_wr in_obj)
    {
        return static_cast<persistence_connection *>(pconn)->save_or_update_entity(key,
        *(const entity_manager *)em, ts0, ts1,
        *(const nclass *)in_obj);
    }

    RetCode persistence_connection_remove_entity(persistence_connection_wr pconn,
    unsigned short key,
    const entity_manager_wr em,
    unsigned int ts0,
    unsigned int ts1,
    PersistenceDeletionMode mode,
    const net_class_wr in_obj)
    {
        return static_cast<persistence_connection *>(pconn)->remove_entity(key,
        *(const entity_manager *)em,
        ts0,
        ts1,
        mode,
        *(const nclass *)in_obj);
    }

    RetCode persistence_connection_load_entity(persistence_connection_wr pconn,
    unsigned short key,
    const entity_manager_wr em,
    unsigned int *ts0_out,
    unsigned int *ts1_out,
    net_class_wr in_out_obj)
    {
        return static_cast<persistence_connection *>(pconn)->load_entity(key,
        *(const entity_manager *)em,
        *ts0_out,
        *ts1_out,
        *(nclass *)in_out_obj);
    }

    RetCode persistence_connection_execute_statement(persistence_connection_wr pconn,
    const char *stmt)
    {
        return static_cast<persistence_connection *>(pconn)->execute_statement(stmt);
    }

    RetCode persistence_query_bind(persistence_query_wr pqry,
    unsigned int nclass_id,
    const char *sql)
    {
        return static_cast<persistence_query *>(pqry)->bind(nclass_id, sql);
    }

    unsigned int persistence_query_get_id(persistence_query_wr pqry)
    {
        return static_cast<persistence_query *>(pqry)->get_id();
    }

    PersistenceQueryStatus persistence_query_get_status(
        persistence_query_wr pqry)
    {
        return static_cast<persistence_query *>(pqry)->get_status();
    }

    const entity_manager_wr persistence_query_get_entity_manager(
        persistence_query_wr pqry)
    {
        const entity_manager &em = static_cast<persistence_query *>
        (pqry)->get_entity_manager();
        return (const entity_manager_wr)&em;
    }

    RetCode persistence_query_next_entity(persistence_query_wr pqry,
    unsigned int *ts0_out,
    unsigned int *ts1_out,
    net_class_wr out_obj)
    {
        return static_cast<persistence_query *>(pqry)->next_entity(*ts0_out,
        *ts1_out,
        *(nclass *)out_obj);
    }
}
}
