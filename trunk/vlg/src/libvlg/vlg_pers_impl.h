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

#ifndef BLZ_MDL_UTL_H_
#define BLZ_MDL_UTL_H_
#include "blaze_logger.h"
#include "blaze_model.h"
#include "blz_glob_int.h"

#define BLZ_DRV_NAME_LEN 64

namespace blaze {

class persistence_connection_int;
class persistence_driver_int;
class persistence_query_int;
class persistence_manager_int;
class persistence_worker;

//-----------------------------
// PERSISTENCE
//-----------------------------

#define P_F_TS0 "TS0"
#define P_F_TS1 "TS1"
#define P_F_DEL "DEL"
#define TMSTMP_BUFF_SZ 11

typedef enum  {
    BLZ_PERS_TASK_OP_UNDEFINED,
    BLZ_PERS_TASK_OP_CONNECT,
    BLZ_PERS_TASK_OP_CREATETABLE,
    BLZ_PERS_TASK_OP_SELECT,
    BLZ_PERS_TASK_OP_UPDATE,
    BLZ_PERS_TASK_OP_DELETE,
    BLZ_PERS_TASK_OP_INSERT,
    BLZ_PERS_TASK_OP_EXECUTEQUERY,
    BLZ_PERS_TASK_OP_RELEASEQUERY,
    BLZ_PERS_TASK_OP_NEXTENTITYFROMQUERY,
    BLZ_PERS_TASK_OP_EXECUTESTATEMENT,
} BLZ_PERS_TASK_OP;

//-----------------------------
// persistence_task
//-----------------------------
class persistence_task : public blaze::p_task {

    public:
        //---ctor
        persistence_task(BLZ_PERS_TASK_OP op_code);

    public:
        virtual blaze::RetCode execute();

    public:
        BLZ_PERS_TASK_OP                op_code() const;
        void                            op_code(BLZ_PERS_TASK_OP val);
        blaze::RetCode                  op_res() const;
        void                            op_res(blaze::RetCode val);
        void                            in_bem(const entity_manager &val);
        PersistenceDeletionMode         in_mode() const;
        void                            in_mode(PersistenceDeletionMode val);
        void                            in_obj(const nclass &val);
        void                            in_out_obj(nclass &val);
        void                            in_sql(const char *val);
        unsigned short                  in_key() const;
        void                            in_key(unsigned short val);
        void                            in_edesc(const entity_desc &val);
        bool                            in_drop_if_exist() const;
        void                            in_drop_if_exist(bool val);
        void                            in_out_ts0(unsigned int &val);
        void                            in_out_ts1(unsigned int &val);
        persistence_query_int           *in_out_query() const;
        void                            in_out_query(persistence_query_int *val);
        bool                            in_fail_is_error() const;
        void                            in_fail_is_error(bool val);
        void                            stmt_bf(blaze::ascii_string &stmt_bf);
        blaze::ascii_string             &stmt_bf();

    protected:
        virtual blaze::RetCode do_connect() = 0;
        virtual blaze::RetCode do_create_table() = 0;
        virtual blaze::RetCode do_select() = 0;
        virtual blaze::RetCode do_update() = 0;
        virtual blaze::RetCode do_delete() = 0;
        virtual blaze::RetCode do_insert() = 0;
        virtual blaze::RetCode do_execute_query() = 0;
        virtual blaze::RetCode do_release_query() = 0;
        virtual blaze::RetCode do_next_entity_from_query() = 0;
        virtual blaze::RetCode do_execute_statement() = 0;

        //--rep
    protected:
        //--rep
        BLZ_PERS_TASK_OP        op_code_;
        blaze::RetCode          op_res_;
        const entity_manager    *in_bem_;
        PersistenceDeletionMode in_mode_;
        const nclass         *in_obj_;
        nclass               *in_out_obj_;
        const char              *in_sql_;
        unsigned short          in_key_;
        const entity_desc       *in_edesc_;
        bool                    in_drop_if_exist_;
        unsigned int            *in_out_ts0_;
        unsigned int            *in_out_ts1_;
        persistence_query_int   *in_out_query_;
        bool                    in_fail_is_error_;
        blaze::ascii_string     *stmt_bf_;
};

//-----------------------------
// persistence_connection_pool
//-----------------------------
class persistence_connection_pool {
        friend class persistence_driver_int;

    private:
        persistence_connection_pool(persistence_driver_int &driv,
                                    const char *url,
                                    const char *usr,
                                    const char *psswd,
                                    unsigned int conn_pool_sz,
                                    unsigned int conn_pool_th_max_sz);

        ~persistence_connection_pool();

    public:
        persistence_driver_int &driver();
        const char *password() const;
        const char *user() const;
        const char *url() const;

    public:
        blaze::RetCode init();
        blaze::RetCode start();
        persistence_connection_int *request_connection();

    public:
        persistence_worker *get_worker_rr_can_create_start();
        persistence_worker *get_worker_rr();

        //uri
    private:
        blaze::ascii_string url_;
        blaze::ascii_string usr_;
        blaze::ascii_string psswd_;

    private:
        persistence_driver_int &driv_;
        unsigned int conn_pool_sz_; // connection pool size
        // current connection id used for round-robin
        unsigned int conn_pool_curr_idx_;
        blaze::hash_map conn_idx_conn_hm_;  // connection idx --> connection
        unsigned int conn_pool_th_max_sz_;  // connection pool thread max size
        unsigned int conn_pool_th_curr_sz_; // connection pool thread current size
        unsigned int conn_pool_th_curr_idx_; // current thread id used for round-robin
        // allocated threads for this connection pool.
        persistence_worker  **conn_pool_th_pool_;

        mutable blaze::synch_monitor mon_;
};

//------------------------------
// persistence_worker
// we cannot use a thread-pool because tipically we want 1 thread per connection.
//------------------------------
class persistence_worker : public blaze::p_thread {
    public:
        persistence_worker(persistence_connection_pool &conn_pool);
        virtual ~persistence_worker();

    public:
        blaze::RetCode submit_task(persistence_task *task);

        persistence_connection_pool &get_connection_pool();

        virtual void *run();

    private:
        persistence_connection_pool &conn_pool_;
        blaze::blocking_queue task_queue_;
    protected:
        static nclass_logger *log_;
};

//-----------------------------
// persistence_connection_int
//-----------------------------
class persistence_connection_int {
        friend class persistence_query_int;

    protected:
        //---ctors
        persistence_connection_int(unsigned int id,
                                   persistence_connection_pool &conn_pool);
        virtual ~persistence_connection_int();

        //getters
    public:
        PersistenceConnectionStatus status() const;
        unsigned int                      get_id() const;
        persistence_driver_int           &get_driver();
        persistence_connection_pool      &get_connection_pool();

        //business meths
    public:

        blaze::RetCode    connect();

        blaze::RetCode    create_entity_schema(PersistenceAlteringMode mode,
                                               const entity_manager &bem,
                                               unsigned int nclass_id);

        blaze::RetCode    create_entity_schema(PersistenceAlteringMode mode,
                                               const entity_manager &bem,
                                               const entity_desc &edesc);

        blaze::RetCode    save_entity(const entity_manager &bem,
                                      unsigned int ts0,
                                      unsigned int ts1,
                                      const nclass &in_obj);

        blaze::RetCode    update_entity(unsigned short key,
                                        const entity_manager &bem,
                                        unsigned int ts0,
                                        unsigned int ts1,
                                        const nclass &in_obj);

        blaze::RetCode    save_or_update_entity(unsigned short key,
                                                const entity_manager &bem,
                                                unsigned int ts0,
                                                unsigned int ts1,
                                                const nclass &in_obj);

        blaze::RetCode    remove_entity(unsigned short key,
                                        const entity_manager &bem,
                                        unsigned int ts0,
                                        unsigned int ts1,
                                        PersistenceDeletionMode mode,
                                        const nclass &in_obj);

        blaze::RetCode    load_entity(unsigned short key,
                                      const entity_manager &bem,
                                      unsigned int &ts0_out,
                                      unsigned int &ts1_out,
                                      nclass &in_out_obj);

        blaze::RetCode    execute_query(const char *sql,
                                        const entity_manager &bem,
                                        persistence_query_int **query_out);

        blaze::RetCode    destroy_query(persistence_query_int *query,
                                        bool release_before_destroy = false);

        blaze::RetCode    execute_statement(const char *sql);

        //internal meths
    private:

        virtual blaze::RetCode do_connect() = 0;

        virtual blaze::RetCode do_create_table(const entity_manager &bem,
                                               const entity_desc &edesc,
                                               bool drop_if_exist) = 0;

        virtual blaze::RetCode do_select(unsigned int key,
                                         const entity_manager &bem,
                                         unsigned int &ts0_out,
                                         unsigned int &ts1_out,
                                         nclass &in_out_obj) = 0;

        virtual blaze::RetCode do_update(unsigned int key,
                                         const entity_manager &bem,
                                         unsigned int ts0,
                                         unsigned int ts1,
                                         const nclass &in_obj) = 0;

        virtual blaze::RetCode do_delete(unsigned int key,
                                         const entity_manager &bem,
                                         unsigned int ts0,
                                         unsigned int ts1,
                                         PersistenceDeletionMode mode,
                                         const nclass &in_obj) = 0;

        virtual blaze::RetCode do_insert(const entity_manager &bem,
                                         unsigned int ts0,
                                         unsigned int ts1,
                                         const nclass &in_obj,
                                         bool fail_is_error = true) = 0;

        virtual blaze::RetCode do_execute_query(const entity_manager &bem,
                                                const char *sql,
                                                persistence_query_int **qry_out) = 0;

        virtual blaze::RetCode do_release_query(persistence_query_int *qry) = 0;

        virtual blaze::RetCode do_next_entity_from_query(persistence_query_int *qry,
                                                         unsigned int &ts0_out,
                                                         unsigned int &ts1_out,
                                                         nclass &out_obj) = 0;

        virtual blaze::RetCode do_execute_statement(const char *sql) = 0;



    protected:
        unsigned int id_;
        PersistenceConnectionStatus status_;
        persistence_connection_pool &conn_pool_;

    protected:
        static nclass_logger *log_;
};

//-----------------------------
// persistence_driver_int
//-----------------------------
typedef const char *(*get_pers_driv_version)();
typedef persistence_driver_int *(*load_pers_driver)();

class persistence_driver_int {
        friend class persistence_connection_pool;
        friend class persistence_manager_int;

    public:
        /*dyna*/
        static blaze::RetCode load_driver_dyna(const char *drvname,
                                               persistence_driver_int **driver);

    protected:
        //---ctors
        persistence_driver_int(unsigned int id);
        virtual ~persistence_driver_int();

    protected:
        blaze::RetCode init();

    public:
        //---getters
        unsigned int get_id() const;
        virtual const char *get_driver_name() = 0;

    public:
        blaze::RetCode start_all_pools();

        //returns null if no connection is available.
        persistence_connection_int  *available_connection(unsigned int nclass_id);

        /****USED BY MANAGER****/
    private:
        blaze::RetCode    add_pool(const char *conn_pool_name,
                                   const char *url,
                                   const char *usr,
                                   const char *psswd,
                                   unsigned int conn_pool_sz,
                                   unsigned int conn_pool_th_max_sz);

        blaze::RetCode   map_nclassid_to_pool(unsigned int nclass_id,
                                              const char *conn_pool_name);

        virtual blaze::RetCode new_connection(persistence_connection_pool &conn_pool,
                                              persistence_connection_int **new_conn) = 0;

        virtual blaze::RetCode close_connection(persistence_connection_int &conn) = 0;
        /*******/


    protected:
        unsigned int id_;

        /****CONN POOL REP****/
        blaze::hash_map conn_pool_hm_;  // [conn_pool_name --> conn_pool]
        blaze::hash_map nclassid_conn_pool_hm_; // [nclass_id --> conn_pool]
        /****CONN POOL REP****/

    protected:
        static nclass_logger *log_;
};

//-----------------------------
// persistence_query_int
//-----------------------------
class persistence_query_int {
        friend class persistence_connection_int;

    protected:
        //---ctors
        persistence_query_int(unsigned int id,
                              persistence_connection_int &conn,
                              const entity_manager &bem);

        virtual ~persistence_query_int();

    public:
        unsigned int get_id() const;
        PersistenceQueryStatus status() const;
        const entity_manager &get_em() const;
        persistence_connection_int &get_connection();

        blaze::RetCode    load_next_entity(unsigned int &ts0_out,
                                           unsigned int &ts1_out,
                                           nclass &out_obj);
        blaze::RetCode    release();


    protected:
        unsigned int id_;
        PersistenceQueryStatus status_;
        persistence_connection_int &conn_;
        const entity_manager &bem_;

    protected:
        static nclass_logger *log_;
};

//-----------------------------
// persistence_manager_int
//-----------------------------
class persistence_manager_int {
    public:

        static persistence_manager_int &get_instance();
        static blaze::RetCode set_cfg_file_dir(const char *dir);
        static blaze::RetCode set_cfg_file_path_name(const char *file_path);

        /*dyna*/
        static blaze::RetCode load_pers_driver_dyna(const char *drivers[],
                                                    int drivers_num);

        static blaze::RetCode load_pers_driver_dyna(blaze::hash_map &drivmap);

        /*static*/
        static blaze::RetCode load_pers_driver(persistence_driver_int *drivers[],
                                               int drivers_num);

    private:
        //---ctors
        persistence_manager_int();
        ~persistence_manager_int();
        blaze::RetCode init();

    public:
        blaze::RetCode load_cfg();
        blaze::RetCode load_cfg(const char *filename);
        blaze::RetCode start_all_drivers();

        //returns null if no driver is available.
        persistence_driver_int *available_driver(unsigned int nclass_id);

    private:
        blaze::RetCode    parse_data(blaze::ascii_string &data);

        blaze::RetCode    parse_conn_pool_cfg(unsigned long &lnum,
                                              blaze::ascii_string_tok &tknz,
                                              blaze::hash_map &conn_pool_name_to_driv);

        blaze::RetCode    parse_single_conn_pool_cfg(unsigned long &lnum,
                                                     blaze::ascii_string_tok &tknz,
                                                     blaze::ascii_string &conn_pool_name,
                                                     blaze::hash_map &conn_pool_name_to_driv);

        blaze::RetCode    parse_URI(unsigned long &lnum,
                                    blaze::ascii_string_tok &tknz,
                                    blaze::ascii_string &url,
                                    blaze::ascii_string &usr,
                                    blaze::ascii_string &psswd);

        blaze::RetCode    parse_int_after_colon(unsigned long &lnum,
                                                blaze::ascii_string_tok &tknz,
                                                unsigned int &pool_size);

        blaze::RetCode    parse_class_mapping_cfg(unsigned long &lnum,
                                                  blaze::ascii_string_tok &tknz,
                                                  blaze::hash_map &conn_pool_name_to_driv);

        blaze::RetCode    parse_single_class_map_cfg(unsigned long &lnum,
                                                     blaze::ascii_string_tok &tknz,
                                                     unsigned int nclass_id,
                                                     blaze::hash_map &conn_pool_name_to_driv);

        blaze::RetCode    map_classid_driver(unsigned int nclass_id,
                                             persistence_driver_int *driver);


    private:
        blaze::hash_map drivname_driv_hm_;  // [driver-name --> driver]
        blaze::hash_map nclassid_driv_hm_;   // [nclass_id --> driver]

    protected:
        static nclass_logger *log_;
};


}

#endif
