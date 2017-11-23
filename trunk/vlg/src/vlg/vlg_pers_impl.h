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

#ifndef VLG_MDL_UTL_H_
#define VLG_MDL_UTL_H_
#include "vlg_logger.h"
#include "vlg_model.h"
#include "vlg_globint.h"

#define VLG_DRV_NAME_LEN 64

namespace vlg {

class persistence_connection_impl;
class persistence_driver_impl;
class persistence_query_impl;
class persistence_manager_impl;
class persistence_worker;

//-----------------------------
// PERSISTENCE
//-----------------------------

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

//-----------------------------
// persistence_task
//-----------------------------
class persistence_task : public vlg::p_task {

    public:
        //---ctor
        persistence_task(VLG_PERS_TASK_OP op_code);

    public:
        virtual vlg::RetCode execute();

    public:
        VLG_PERS_TASK_OP                op_code() const;
        void                            op_code(VLG_PERS_TASK_OP val);
        vlg::RetCode                    op_res() const;
        void                            op_res(vlg::RetCode val);
        void                            in_nem(const nentity_manager &val);
        PersistenceDeletionMode         in_mode() const;
        void                            in_mode(PersistenceDeletionMode val);
        void                            in_obj(const nclass &val);
        void                            in_out_obj(nclass &val);
        void                            in_sql(const char *val);
        unsigned short                  in_key() const;
        void                            in_key(unsigned short val);
        void                            in_edesc(const nentity_desc &val);
        bool                            in_drop_if_exist() const;
        void                            in_drop_if_exist(bool val);
        void                            in_out_ts0(unsigned int &val);
        void                            in_out_ts1(unsigned int &val);
        persistence_query_impl          *in_out_query() const;
        void                            in_out_query(persistence_query_impl *val);
        bool                            in_fail_is_error() const;
        void                            in_fail_is_error(bool val);
        void                            stmt_bf(vlg::ascii_string &stmt_bf);
        vlg::ascii_string               &stmt_bf();

    protected:
        virtual vlg::RetCode do_connect() = 0;
        virtual vlg::RetCode do_create_table() = 0;
        virtual vlg::RetCode do_select() = 0;
        virtual vlg::RetCode do_update() = 0;
        virtual vlg::RetCode do_delete() = 0;
        virtual vlg::RetCode do_insert() = 0;
        virtual vlg::RetCode do_execute_query() = 0;
        virtual vlg::RetCode do_release_query() = 0;
        virtual vlg::RetCode do_next_entity_from_query() = 0;
        virtual vlg::RetCode do_execute_statement() = 0;

        //--rep
    protected:
        //--rep
        VLG_PERS_TASK_OP        op_code_;
        vlg::RetCode            op_res_;
        const nentity_manager   *in_nem_;
        PersistenceDeletionMode in_mode_;
        const nclass            *in_obj_;
        nclass                  *in_out_obj_;
        const char              *in_sql_;
        unsigned short          in_key_;
        const nentity_desc      *in_edesc_;
        bool                    in_drop_if_exist_;
        unsigned int            *in_out_ts0_;
        unsigned int            *in_out_ts1_;
        persistence_query_impl  *in_out_query_;
        bool                    in_fail_is_error_;
        vlg::ascii_string       *stmt_bf_;
};

//-----------------------------
// persistence_connection_pool
//-----------------------------
class persistence_connection_pool {
        friend class persistence_driver_impl;

    private:
        persistence_connection_pool(persistence_driver_impl &driv,
                                    const char *url,
                                    const char *usr,
                                    const char *psswd,
                                    unsigned int conn_pool_sz,
                                    unsigned int conn_pool_th_max_sz);

        ~persistence_connection_pool();

    public:
        persistence_driver_impl &driver();
        const char *password() const;
        const char *user() const;
        const char *url() const;

    public:
        vlg::RetCode init();
        vlg::RetCode start();
        persistence_connection_impl *request_connection();

    public:
        persistence_worker *get_worker_rr_can_create_start();
        persistence_worker *get_worker_rr();

        //uri
    private:
        vlg::ascii_string url_;
        vlg::ascii_string usr_;
        vlg::ascii_string psswd_;

    private:
        persistence_driver_impl &driv_;
        unsigned int conn_pool_sz_; // connection pool size
        // current connection id used for round-robin
        unsigned int conn_pool_curr_idx_;
        vlg::hash_map conn_idx_conn_hm_;  // connection idx --> connection
        unsigned int conn_pool_th_max_sz_;  // connection pool thread max size
        unsigned int conn_pool_th_curr_sz_; // connection pool thread current size
        unsigned int conn_pool_th_curr_idx_; // current thread id used for round-robin
        // allocated threads for this connection pool.
        persistence_worker  **conn_pool_th_pool_;

        mutable vlg::synch_monitor mon_;
};

//------------------------------
// persistence_worker
// we cannot use a thread-pool because tipically we want 1 thread per connection.
//------------------------------
class persistence_worker : public vlg::p_thread {
    public:
        persistence_worker(persistence_connection_pool &conn_pool);
        virtual ~persistence_worker();

    public:
        vlg::RetCode submit_task(persistence_task *task);

        persistence_connection_pool &get_connection_pool();

        virtual void *run();

    private:
        persistence_connection_pool &conn_pool_;
        vlg::blocking_queue task_queue_;
    protected:
        static nclass_logger *log_;
};

//-----------------------------
// persistence_connection_impl
//-----------------------------
class persistence_connection_impl {
        friend class persistence_query_impl;

    protected:
        //---ctors
        persistence_connection_impl(unsigned int id,
                                    persistence_connection_pool &conn_pool);
        virtual ~persistence_connection_impl();

        //getters
    public:
        PersistenceConnectionStatus     status() const;
        unsigned int                    get_id() const;
        persistence_driver_impl         &get_driver();
        persistence_connection_pool     &get_connection_pool();

        //business meths
    public:

        vlg::RetCode    connect();

        vlg::RetCode    create_entity_schema(PersistenceAlteringMode mode,
                                             const nentity_manager &nem,
                                             unsigned int nclass_id);

        vlg::RetCode    create_entity_schema(PersistenceAlteringMode mode,
                                             const nentity_manager &nem,
                                             const nentity_desc &edesc);

        vlg::RetCode    save_entity(const nentity_manager &nem,
                                    unsigned int ts0,
                                    unsigned int ts1,
                                    const nclass &in_obj);

        vlg::RetCode    update_entity(unsigned short key,
                                      const nentity_manager &nem,
                                      unsigned int ts0,
                                      unsigned int ts1,
                                      const nclass &in_obj);

        vlg::RetCode    save_or_update_entity(unsigned short key,
                                              const nentity_manager &nem,
                                              unsigned int ts0,
                                              unsigned int ts1,
                                              const nclass &in_obj);

        vlg::RetCode    remove_entity(unsigned short key,
                                      const nentity_manager &nem,
                                      unsigned int ts0,
                                      unsigned int ts1,
                                      PersistenceDeletionMode mode,
                                      const nclass &in_obj);

        vlg::RetCode    load_entity(unsigned short key,
                                    const nentity_manager &nem,
                                    unsigned int &ts0_out,
                                    unsigned int &ts1_out,
                                    nclass &in_out_obj);

        vlg::RetCode    execute_query(const char *sql,
                                      const nentity_manager &nem,
                                      persistence_query_impl **query_out);

        vlg::RetCode    destroy_query(persistence_query_impl *query,
                                      bool release_before_destroy = false);

        vlg::RetCode    execute_statement(const char *sql);

        //internal meths
    private:

        virtual vlg::RetCode do_connect() = 0;

        virtual vlg::RetCode do_create_table(const nentity_manager &nem,
                                             const nentity_desc &edesc,
                                             bool drop_if_exist) = 0;

        virtual vlg::RetCode do_select(unsigned int key,
                                       const nentity_manager &nem,
                                       unsigned int &ts0_out,
                                       unsigned int &ts1_out,
                                       nclass &in_out_obj) = 0;

        virtual vlg::RetCode do_update(unsigned int key,
                                       const nentity_manager &nem,
                                       unsigned int ts0,
                                       unsigned int ts1,
                                       const nclass &in_obj) = 0;

        virtual vlg::RetCode do_delete(unsigned int key,
                                       const nentity_manager &nem,
                                       unsigned int ts0,
                                       unsigned int ts1,
                                       PersistenceDeletionMode mode,
                                       const nclass &in_obj) = 0;

        virtual vlg::RetCode do_insert(const nentity_manager &nem,
                                       unsigned int ts0,
                                       unsigned int ts1,
                                       const nclass &in_obj,
                                       bool fail_is_error = true) = 0;

        virtual vlg::RetCode do_execute_query(const nentity_manager &nem,
                                              const char *sql,
                                              persistence_query_impl **qry_out) = 0;

        virtual vlg::RetCode do_release_query(persistence_query_impl *qry) = 0;

        virtual vlg::RetCode do_next_entity_from_query(persistence_query_impl *qry,
                                                       unsigned int &ts0_out,
                                                       unsigned int &ts1_out,
                                                       nclass &out_obj) = 0;

        virtual vlg::RetCode do_execute_statement(const char *sql) = 0;



    protected:
        unsigned int id_;
        PersistenceConnectionStatus status_;
        persistence_connection_pool &conn_pool_;

    protected:
        static nclass_logger *log_;
};

//-----------------------------
// persistence_driver_impl
//-----------------------------
typedef const char *(*get_pers_driv_version)();
typedef persistence_driver_impl *(*load_pers_driver)();

class persistence_driver_impl {
        friend class persistence_connection_pool;
        friend class persistence_manager_impl;

    public:
        /*dyna*/
        static vlg::RetCode load_driver_dyna(const char *drvname,
                                             persistence_driver_impl **driver);

    protected:
        //---ctors
        persistence_driver_impl(unsigned int id);
        virtual ~persistence_driver_impl();

    protected:
        vlg::RetCode init();

    public:
        //---getters
        unsigned int get_id() const;
        virtual const char *get_driver_name() = 0;

    public:
        vlg::RetCode start_all_pools();

        //returns null if no connection is available.
        persistence_connection_impl  *available_connection(unsigned int nclass_id);

        /****USED BY MANAGER****/
    private:
        vlg::RetCode    add_pool(const char *conn_pool_name,
                                 const char *url,
                                 const char *usr,
                                 const char *psswd,
                                 unsigned int conn_pool_sz,
                                 unsigned int conn_pool_th_max_sz);

        vlg::RetCode   map_nclassid_to_pool(unsigned int nclass_id,
                                            const char *conn_pool_name);

        virtual vlg::RetCode new_connection(persistence_connection_pool &conn_pool,
                                            persistence_connection_impl **new_conn) = 0;

        virtual vlg::RetCode close_connection(persistence_connection_impl &conn) = 0;
        /*******/


    protected:
        unsigned int id_;

        /****CONN POOL REP****/
        vlg::hash_map conn_pool_hm_;  // [conn_pool_name --> conn_pool]
        vlg::hash_map nclassid_conn_pool_hm_; // [nclass_id --> conn_pool]
        /****CONN POOL REP****/

    protected:
        static nclass_logger *log_;
};

//-----------------------------
// persistence_query_impl
//-----------------------------
class persistence_query_impl {
        friend class persistence_connection_impl;

    protected:
        //---ctors
        persistence_query_impl(unsigned int id,
                               persistence_connection_impl &conn,
                               const nentity_manager &nem);

        virtual ~persistence_query_impl();

    public:
        unsigned int get_id() const;
        PersistenceQueryStatus status() const;
        const nentity_manager &get_em() const;
        persistence_connection_impl &get_connection();

        vlg::RetCode    load_next_entity(unsigned int &ts0_out,
                                         unsigned int &ts1_out,
                                         nclass &out_obj);
        vlg::RetCode    release();


    protected:
        unsigned int id_;
        PersistenceQueryStatus status_;
        persistence_connection_impl &conn_;
        const nentity_manager &nem_;

    protected:
        static nclass_logger *log_;
};

//-----------------------------
// persistence_manager_impl
//-----------------------------
class persistence_manager_impl {
    public:

        static persistence_manager_impl &get_instance();
        static vlg::RetCode set_cfg_file_dir(const char *dir);
        static vlg::RetCode set_cfg_file_path_name(const char *file_path);

        /*dyna*/
        static vlg::RetCode load_pers_driver_dyna(const char *drivers[],
                                                  int drivers_num);

        static vlg::RetCode load_pers_driver_dyna(vlg::hash_map &drivmap);

        /*static*/
        static vlg::RetCode load_pers_driver(persistence_driver_impl *drivers[],
                                             int drivers_num);

    private:
        //---ctors
        persistence_manager_impl();
        ~persistence_manager_impl();
        vlg::RetCode init();

    public:
        vlg::RetCode load_cfg();
        vlg::RetCode load_cfg(const char *filename);
        vlg::RetCode start_all_drivers();

        //returns null if no driver is available.
        persistence_driver_impl *available_driver(unsigned int nclass_id);

    private:
        vlg::RetCode    parse_data(vlg::ascii_string &data);

        vlg::RetCode    parse_conn_pool_cfg(unsigned long &lnum,
                                            vlg::ascii_string_tok &tknz,
                                            vlg::hash_map &conn_pool_name_to_driv);

        vlg::RetCode    parse_single_conn_pool_cfg(unsigned long &lnum,
                                                   vlg::ascii_string_tok &tknz,
                                                   vlg::ascii_string &conn_pool_name,
                                                   vlg::hash_map &conn_pool_name_to_driv);

        vlg::RetCode    parse_URI(unsigned long &lnum,
                                  vlg::ascii_string_tok &tknz,
                                  vlg::ascii_string &url,
                                  vlg::ascii_string &usr,
                                  vlg::ascii_string &psswd);

        vlg::RetCode    parse_impl_after_colon(unsigned long &lnum,
                                               vlg::ascii_string_tok &tknz,
                                               unsigned int &pool_size);

        vlg::RetCode    parse_class_mapping_cfg(unsigned long &lnum,
                                                vlg::ascii_string_tok &tknz,
                                                vlg::hash_map &conn_pool_name_to_driv);

        vlg::RetCode    parse_single_class_map_cfg(unsigned long &lnum,
                                                   vlg::ascii_string_tok &tknz,
                                                   unsigned int nclass_id,
                                                   vlg::hash_map &conn_pool_name_to_driv);

        vlg::RetCode    map_classid_driver(unsigned int nclass_id,
                                           persistence_driver_impl *driver);


    private:
        vlg::hash_map drivname_driv_hm_;  // [driver-name --> driver]
        vlg::hash_map nclassid_driv_hm_;   // [nclass_id --> driver]

    protected:
        static nclass_logger *log_;
};


}

#endif
