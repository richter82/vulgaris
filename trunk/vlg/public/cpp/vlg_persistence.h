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

#ifndef VLG_CPP_PERSISTENCE_H_
#define VLG_CPP_PERSISTENCE_H_
#include "vlg.h"

namespace vlg {

/** @brief persistence_manager.
*/
struct persistence_manager {
    static RetCode set_config_file_dir(const char *dir);
    static RetCode set_config_file_path_name(const char *file_path);
    static RetCode load_config(const char *file_name);
    static RetCode start_all_drivers();

    //returns null if no driver is available.
    static persistence_driver *available_driver(unsigned int nclass_id);

    /*dyna*/
    static RetCode load_driver_dynamic(const char *drivers[],
                                       int drivers_num);
    /*static*/
    static RetCode load_driver(persistence_driver *drivers[],
                               int drivers_num);
};

/** @brief persistence_connection.
*/
struct persistence_connection {
    explicit persistence_connection(persistence_driver &driver);
    ~persistence_connection();

    persistence_driver &get_driver();

    RetCode create_entity_schema(PersistenceAlteringMode mode,
                                 const nentity_manager &nem,
                                 unsigned int nclass_id);

    RetCode create_entity_schema(PersistenceAlteringMode mode,
                                 const nentity_manager &nem,
                                 const nentity_desc &desc);

    RetCode save_obj(const nentity_manager &nem,
                     unsigned int ts0,
                     unsigned int ts1,
                     const nclass &in);

    RetCode update_obj(unsigned short key,
                       const nentity_manager &nem,
                       unsigned int ts0,
                       unsigned int ts1,
                       const nclass &in);

    RetCode save_or_update_obj(unsigned short key,
                               const nentity_manager &nem,
                               unsigned int ts0,
                               unsigned int ts1,
                               const nclass &in);

    RetCode remove_obj(unsigned short key,
                       const nentity_manager &nem,
                       unsigned int ts0,
                       unsigned int ts1,
                       PersistenceDeletionMode mode,
                       const nclass &in);

    RetCode load_obj(unsigned short key,
                     const nentity_manager &nem,
                     unsigned int &ts0_out,
                     unsigned int &ts1_out,
                     nclass &in_out);

    RetCode execute_statement(const char *stmt,
                              unsigned int nclass_id);

    persistence_driver &driver_;
};

/** @brief persistence_query.
*/
struct persistence_query {
    explicit persistence_query(const nentity_manager &);
    ~persistence_query();

    unsigned int get_id() const;

    RetCode execute(unsigned int nclass_id,
                    const char *sql);

    PersistenceQueryStatus get_status() const;
    const nentity_manager &get_nentity_manager() const;

    RetCode next_obj(unsigned int &ts0_out,
                     unsigned int &ts1_out,
                     nclass &out);

    RetCode release();

    const nentity_manager &nem_;
    std::unique_ptr<persistence_query_impl> impl_;
};

}

#endif
