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

/** @brief pepersistence_managerer class.
*/
class persistence_manager {
    public:

        /***************************
        driver loading
        ***************************/
        /*dyna*/
        static vlg::RetCode load_driver_dynamic(const char *drivers[],
                                                int drivers_num);

        /*static*/
        static vlg::RetCode load_driver(persistence_driver_impl *drivers[],
                                        int drivers_num);

        /***************************
        persistence config loading
        ***************************/
        static vlg::RetCode set_config_file_dir(const char *dir);
        static vlg::RetCode set_config_file_path_name(const char *file_path);
        static vlg::RetCode load_config(const char *file_name);

        /***************************
        driver usage
        ***************************/
        static vlg::RetCode start_all_drivers();

        //returns null if no driver is available.
        static persistence_driver_impl *available_driver(unsigned int nclass_id);
};

/** @brief persistence_connection class.
*/
class persistence_connection_impl_pub;
class persistence_connection {

        //---ctors
    public:
        explicit persistence_connection();
        ~persistence_connection();

        vlg::RetCode bind(unsigned int nclass_id,
                          persistence_driver_impl &driver);

        //getters
    public:
        persistence_connection_impl     *get_opaque();
        unsigned int                    get_id()        const;
        persistence_driver_impl         *get_driver();
        PersistenceConnectionStatus     get_status()    const;

        //business meths
    public:
        vlg::RetCode    create_entity_schema(PersistenceAlteringMode mode,
                                             const nentity_manager &nem,
                                             unsigned int nclass_id);

        vlg::RetCode    create_entity_schema(PersistenceAlteringMode mode,
                                             const nentity_manager &nem,
                                             const nentity_desc &desc);

        vlg::RetCode    save_obj(const nentity_manager &nem,
                                 unsigned int ts0,
                                 unsigned int ts1,
                                 const nclass &in_obj);

        vlg::RetCode    update_obj(unsigned short key,
                                   const nentity_manager &nem,
                                   unsigned int ts0,
                                   unsigned int ts1,
                                   const nclass &in_obj);

        vlg::RetCode    save_or_update_obj(unsigned short key,
                                           const nentity_manager &nem,
                                           unsigned int ts0,
                                           unsigned int ts1,
                                           const nclass &in_obj);

        vlg::RetCode    remove_obj(unsigned short key,
                                   const nentity_manager &nem,
                                   unsigned int ts0,
                                   unsigned int ts1,
                                   PersistenceDeletionMode mode,
                                   const nclass &in_obj);

        vlg::RetCode    load_obj(unsigned short key,
                                 const nentity_manager &nem,
                                 unsigned int &ts0_out,
                                 unsigned int &ts1_out,
                                 nclass &in_out_obj);

        vlg::RetCode    execute_statement(const char *stmt);

    private:
        persistence_connection_impl_pub *impl_;
};

/** @brief persistence_query class.
*/
class persistence_query_impl_pub;
class persistence_query {

    public:
        explicit persistence_query(const nentity_manager &nem);
        ~persistence_query();

        unsigned int    get_id()                        const;

        vlg::RetCode    bind(unsigned int nclass_id,
                             const char *sql);

    public:
        PersistenceQueryStatus  get_status()            const;
        const nentity_manager    &get_entity_manager()   const;

        vlg::RetCode    next_obj(unsigned int &ts0_out,
                                 unsigned int &ts1_out,
                                 nclass &out_obj);

        vlg::RetCode    release();

    private:
        persistence_query_impl_pub *impl_;
};

}

#endif
