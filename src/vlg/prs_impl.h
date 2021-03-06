/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#pragma once
#include "glob.h"

#define VLG_DRV_NAME_LEN 64

namespace vlg {

struct persistence_connection_impl;
struct persistence_driver;
struct persistence_query_impl;
struct persistence_manager_impl;

#define P_F_TS0 "TS0"
#define P_F_TS1 "TS1"
#define P_F_DEL "DEL"
#define TMSTMP_BUFF_SZ 11

typedef enum  {
    VLG_PERS_TASK_OP_UNDEFINED,
    VLG_PERS_TASK_OP_CONNECT,
    VLG_PERS_TASK_OP_CREATETABLE,
    VLG_PERS_TASK_OP_SELECT,
    VLG_PERS_TASK_OP_UPDATE,
    VLG_PERS_TASK_OP_DELETE,
    VLG_PERS_TASK_OP_INSERT,
    VLG_PERS_TASK_OP_EXECUTEQUERY,
    VLG_PERS_TASK_OP_RELEASEQUERY,
    VLG_PERS_TASK_OP_NEXTENTITYFROMQUERY,
    VLG_PERS_TASK_OP_EXECUTESTATEMENT,
} VLG_PERS_TASK_OP;

struct persistence_task : public task {
    persistence_task(VLG_PERS_TASK_OP op_code, std::shared_ptr<spdlog::logger> &log) :
        op_code_(op_code),
        op_res_(RetCode_OK),
        in_nem_(nullptr),
        in_mode_(PersistenceDeletionMode_UNDEFINED),
        in_out_obj_(nullptr),
        in_sql_(nullptr),
        in_key_(0),
        in_edesc_((nullptr)),
        in_drop_if_exist_(false),
        in_out_ts0_(nullptr),
        in_out_ts1_(nullptr),
        in_out_query_(nullptr),
        in_fail_is_error_(false),
        stmt_bf_(nullptr),
        log_(log)
    {}

    virtual RetCode execute() override;

    virtual RetCode do_connect() = 0;
    virtual RetCode do_create_table() = 0;
    virtual RetCode do_select() = 0;
    virtual RetCode do_update() = 0;
    virtual RetCode do_delete() = 0;
    virtual RetCode do_insert() = 0;
    virtual RetCode do_execute_query() = 0;
    virtual RetCode do_release_query() = 0;
    virtual RetCode do_next_entity_from_query() = 0;
    virtual RetCode do_execute_statement() = 0;

    VLG_PERS_TASK_OP op_code_;
    RetCode op_res_;
    const nentity_manager *in_nem_;
    PersistenceDeletionMode in_mode_;
    const nclass *in_obj_;
    nclass *in_out_obj_;
    const char *in_sql_;
    unsigned short in_key_;
    const nentity_desc *in_edesc_;
    bool in_drop_if_exist_;
    unsigned int *in_out_ts0_;
    unsigned int *in_out_ts1_;
    persistence_query_impl *in_out_query_;
    bool in_fail_is_error_;
    const char *stmt_bf_;
    std::shared_ptr<spdlog::logger> log_;
};

struct persistence_connection_pool {
    persistence_connection_pool(persistence_driver &driv,
                                const char *url,
                                const char *usr,
                                const char *psswd,
                                unsigned int conn_pool_sz,
                                unsigned int conn_pool_th_max_sz);

    ~persistence_connection_pool();

    RetCode start();
    persistence_connection_impl *request_connection();

    persistence_worker *get_worker_rr_can_create_start();
    persistence_worker *get_worker_rr();

    std::string url_;
    std::string usr_;
    std::string psswd_;

    persistence_driver &driv_;
    // connection pool size
    unsigned int conn_pool_sz_;
    // current connection id used for round-robin
    unsigned int conn_pool_curr_idx_;
    // connection idx --> connection
    std::unordered_map<unsigned int, persistence_connection_impl *>  conn_idx_conn_hm_;
    // connection pool thread max size
    unsigned int conn_pool_th_max_sz_;
    // connection pool thread current size
    unsigned int conn_pool_th_curr_sz_;
    // current thread id used for round-robin
    unsigned int conn_pool_th_curr_idx_;
    // allocated threads for this connection pool.
    persistence_worker  **conn_pool_th_pool_;

    mutable std::mutex mtx_;
    mutable std::condition_variable cv_;
};

// we cannot use a thread-pool because we want 1 thread per connection.
struct persistence_worker : public th {
    persistence_worker(persistence_connection_pool &conn_pool,
                       bool surrogate_th = false);

    RetCode submit(persistence_task &task);
    virtual void run() override;

    persistence_connection_pool &conn_pool_;
    b_qu task_queue_;
    bool surrogate_th_;
};

struct persistence_connection_impl {
    persistence_connection_impl(persistence_connection_pool &conn_pool, std::shared_ptr<spdlog::logger> &log);

    RetCode connect();

    RetCode create_entity_schema(PersistenceAlteringMode mode,
                                 const nentity_manager &nem,
                                 unsigned int nclass_id);

    RetCode create_entity_schema(PersistenceAlteringMode mode,
                                 const nentity_manager &nem,
                                 const nentity_desc &edesc);

    RetCode save_entity(const nentity_manager &nem,
                        unsigned int ts_0,
                        unsigned int ts_1,
                        const nclass &in);

    RetCode update_entity(unsigned short key,
                          const nentity_manager &nem,
                          unsigned int ts_0,
                          unsigned int ts_1,
                          const nclass &in);

    RetCode save_or_update_entity(unsigned short key,
                                  const nentity_manager &nem,
                                  unsigned int ts_0,
                                  unsigned int ts_1,
                                  const nclass &in);

    RetCode remove_entity(unsigned short key,
                          const nentity_manager &nem,
                          unsigned int ts_0,
                          unsigned int ts_1,
                          PersistenceDeletionMode mode,
                          const nclass &in);

    RetCode load_entity(unsigned short key,
                        const nentity_manager &nem,
                        unsigned int &ts0_out,
                        unsigned int &ts1_out,
                        nclass &in_out);

    RetCode execute_query(const char *sql,
                          const nentity_manager &nem,
                          std::unique_ptr<persistence_query_impl> &query_out);

    RetCode destroy_query(persistence_query_impl *query,
                          bool release_before_destroy = false);

    RetCode execute_statement(const char *sql);


    virtual RetCode do_connect() = 0;

    virtual RetCode do_create_table(const nentity_manager &nem,
                                    const nentity_desc &edesc,
                                    bool drop_if_exist) = 0;

    virtual RetCode do_select(unsigned int key,
                              const nentity_manager &nem,
                              unsigned int &ts0_out,
                              unsigned int &ts1_out,
                              nclass &in_out) = 0;

    virtual RetCode do_update(unsigned int key,
                              const nentity_manager &nem,
                              unsigned int ts_0,
                              unsigned int ts_1,
                              const nclass &in) = 0;

    virtual RetCode do_delete(unsigned int key,
                              const nentity_manager &nem,
                              unsigned int ts_0,
                              unsigned int ts_1,
                              PersistenceDeletionMode mode,
                              const nclass &in) = 0;

    virtual RetCode do_insert(const nentity_manager &nem,
                              unsigned int ts_0,
                              unsigned int ts_1,
                              const nclass &in,
                              bool fail_is_error = true) = 0;

    virtual RetCode do_execute_query(const nentity_manager &nem,
                                     const char *sql,
                                     std::unique_ptr<persistence_query_impl> &qry_out) = 0;

    virtual RetCode do_release_query(persistence_query_impl &qry) = 0;

    virtual RetCode do_next_entity_from_query(persistence_query_impl &qry,
                                              unsigned int &ts0_out,
                                              unsigned int &ts1_out,
                                              nclass &out) = 0;

    virtual RetCode do_execute_statement(const char *sql) = 0;


    unsigned int id_;
    PersistenceConnectionStatus status_;
    persistence_connection_pool &conn_pool_;
    std::shared_ptr<spdlog::logger> log_;
};

struct persistence_driver {
    static RetCode load_driver_dyna(const char *drvname,
                                    persistence_driver **driver,
                                    std::shared_ptr<spdlog::logger> &log);

    persistence_driver(unsigned int id,
                       std::shared_ptr<spdlog::logger> &log);

    virtual ~persistence_driver() = default;

    virtual const char *get_driver_name() = 0;

    RetCode start_all_pools();

    //returns null if no connection is available.
    persistence_connection_impl *available_connection(unsigned int nclass_id);

    //used by manager
    RetCode add_pool(const char *conn_pool_name,
                     const char *url,
                     const char *usr,
                     const char *psswd,
                     unsigned int conn_pool_sz,
                     unsigned int conn_pool_th_max_sz);

    RetCode map_nclassid_to_pool(unsigned int nclass_id,
                                 const char *conn_pool_name);

    virtual RetCode new_connection(persistence_connection_pool &conn_pool,
                                   persistence_connection_impl **new_conn) = 0;

    virtual RetCode close_connection(persistence_connection_impl &conn) = 0;

    unsigned int id_;
    std::unordered_map<std::string, persistence_connection_pool *> conn_pool_hm_; // [conn_pool_name --> conn_pool]
    std::unordered_map<unsigned int, persistence_connection_pool *> nclassid_conn_pool_hm_; // [nclass_id --> conn_pool]
    std::shared_ptr<spdlog::logger> log_;
};

struct persistence_query_impl {
    persistence_query_impl(unsigned int id,
                           persistence_connection_impl &conn,
                           const nentity_manager &nem,
                           std::shared_ptr<spdlog::logger> &log);

    RetCode load_next_entity(unsigned int &ts0_out,
                             unsigned int &ts1_out,
                             nclass &out);
    RetCode release();

    unsigned int id_;
    PersistenceQueryStatus status_;
    persistence_connection_impl &conn_;
    const nentity_manager &nem_;
    std::shared_ptr<spdlog::logger> log_;
};

struct persistence_manager_impl {
        static persistence_manager_impl &get_instance();
        static RetCode set_cfg_file_dir(const char *dir);
        static RetCode set_cfg_file_path_name(const char *file_path);
        static RetCode set_db_data_dir(const char *dir);

        /*dyna*/
        static RetCode load_pers_driver_dyna(const char *drivers[],
                                             int drivers_num);

        static RetCode load_pers_driver_dyna(std::set<std::string> &drivmap);

        /*static*/
        static RetCode persistence_driver_load(persistence_driver *drivers[],
                                               int drivers_num);

        explicit persistence_manager_impl();

        RetCode load_cfg();
        RetCode load_cfg(const char *filename);
        RetCode start_all_drivers();

        //returns null if no driver is available.
        persistence_driver *available_driver(unsigned int nclass_id);

    private:
        RetCode parse_data(std::string &data);

        RetCode parse_conn_pool_cfg(unsigned long &lnum,
                                    str_tok &tknz,
                                    std::unordered_map<std::string, persistence_driver *> &conn_pool_name_to_driv);

        RetCode parse_single_conn_pool_cfg(unsigned long &lnum,
                                           str_tok &tknz,
                                           std::string &conn_pool_name,
                                           std::unordered_map<std::string, persistence_driver *> &conn_pool_name_to_driv);

        RetCode parse_URI(unsigned long &lnum,
                          str_tok &tknz,
                          std::string &url,
                          std::string &usr,
                          std::string &psswd);

        RetCode parse_impl_after_colon(unsigned long &lnum,
                                       str_tok &tknz,
                                       unsigned int &pool_size);

        RetCode parse_nclass_mapping_cfg(unsigned long &lnum,
                                         str_tok &tknz,
                                         std::unordered_map<std::string, persistence_driver *> &conn_pool_name_to_driv);

        RetCode parse_single_nclass_map_cfg(unsigned long &lnum,
                                            str_tok &tknz,
                                            unsigned int nclass_id,
                                            std::unordered_map<std::string, persistence_driver *> &conn_pool_name_to_driv);

        RetCode map_classid_driver(unsigned int nclass_id,
                                   persistence_driver *driver);

    private:
        std::unordered_map<std::string, persistence_driver *> drivname_driv_hm_;  // [driver-name --> driver]
        std::unordered_map<unsigned int, persistence_driver *> nclassid_driv_hm_;  // [nclass_id --> driver]
        std::shared_ptr<spdlog::logger> log_;
};


}
