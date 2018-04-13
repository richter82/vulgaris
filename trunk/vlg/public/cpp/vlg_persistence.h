/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
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

    RetCode execute(unsigned int nclass_id,
                    const char *sql);

    const nentity_manager &get_nentity_manager() const;

    RetCode next_obj(unsigned int &ts0_out,
                     unsigned int &ts1_out,
                     nclass &out);

    RetCode release();

    const nentity_manager &nem_;
    std::unique_ptr<persistence_query_impl> impl_;
};

}
