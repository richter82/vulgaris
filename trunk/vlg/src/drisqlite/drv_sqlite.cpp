/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "vlg_drv_sqlite.h"
#include "vlg/prs_impl.h"
#include "sqlite3.h"

namespace vlg {

enum VLG_SQLITE_DATATYPE {
    VLG_SQLITE_DATATYPE_Undef,
    VLG_SQLITE_DATATYPE_NUMERIC,
    VLG_SQLITE_DATATYPE_INTEGER,
    VLG_SQLITE_DATATYPE_REAL,
    VLG_SQLITE_DATATYPE_TEXT,
    VLG_SQLITE_DATATYPE_BLOB,
};

#define VLG_SQLITE_DTYPE_NUMERIC    "NUMERIC"
#define VLG_SQLITE_DTYPE_INTEGER    "INTEGER"
#define VLG_SQLITE_DTYPE_REAL       "REAL"
#define VLG_SQLITE_DTYPE_TEXT       "TEXT"
#define VLG_SQLITE_DTYPE_BLOB       "BLOB"

#define SQLITE_VAL_BUFF 256
#define SQLITE_FIDX_BUFF 16

//utils

const char *SQLITE_TypeStr_From_VLGType(Type type)
{
    switch(type) {
        case Type_BOOL:
            return VLG_SQLITE_DTYPE_NUMERIC;
        case Type_INT16:
            return VLG_SQLITE_DTYPE_INTEGER;
        case Type_UINT16:
            return VLG_SQLITE_DTYPE_INTEGER;
        case Type_INT32:
            return VLG_SQLITE_DTYPE_INTEGER;
        case Type_UINT32:
            return VLG_SQLITE_DTYPE_INTEGER;
        case Type_INT64:
            return VLG_SQLITE_DTYPE_INTEGER;
        case Type_UINT64:
            return VLG_SQLITE_DTYPE_INTEGER;
        case Type_FLOAT32:
            return VLG_SQLITE_DTYPE_REAL;
        case Type_FLOAT64:
            return VLG_SQLITE_DTYPE_REAL;
        case Type_ASCII:
            return VLG_SQLITE_DTYPE_TEXT;
        case Type_BYTE:
            return VLG_SQLITE_DTYPE_BLOB;
        default:
            return nullptr;
    }
}

static int bind_stmt_fld(const member_desc &mmbrd,
                         const char *obj_f_ptr,
                         int colmn_idx,
                         sqlite3_stmt *stmt)
{
    switch(mmbrd.get_field_vlg_type()) {
        case Type_BOOL:
            return sqlite3_bind_int(stmt, colmn_idx, *(bool *)obj_f_ptr);
        case Type_INT16:
            return sqlite3_bind_int(stmt, colmn_idx, *(short *)obj_f_ptr);
        case Type_UINT16:
            return sqlite3_bind_int(stmt, colmn_idx, *(unsigned short *)obj_f_ptr);
        case Type_INT32:
            return sqlite3_bind_int(stmt, colmn_idx, *(int *)obj_f_ptr);
        case Type_UINT32:
            return sqlite3_bind_int(stmt, colmn_idx, *(unsigned int *)obj_f_ptr);
        case Type_INT64:
            return sqlite3_bind_int64(stmt, colmn_idx, *(int64_t *)obj_f_ptr);
        case Type_UINT64:
            return sqlite3_bind_int64(stmt, colmn_idx, *(uint64_t *)obj_f_ptr);
        case Type_FLOAT32:
            return sqlite3_bind_double(stmt, colmn_idx, *(float *)obj_f_ptr);
        case Type_FLOAT64:
            return sqlite3_bind_double(stmt, colmn_idx, *(double *)obj_f_ptr);
        case Type_ASCII:
            return sqlite3_bind_text(stmt,
                                     colmn_idx,
                                     obj_f_ptr,
                                     mmbrd.get_field_nmemb() == 1 ? 1 : (int)min(strlen(obj_f_ptr), mmbrd.get_field_nmemb()),
                                     SQLITE_STATIC);
        case Type_BYTE:
            return sqlite3_bind_blob(stmt, colmn_idx, obj_f_ptr, (int)mmbrd.get_field_nmemb(), SQLITE_STATIC);
        default:
            return -1;
    }
}

static int entity_fill_fld(const member_desc &mmbrd,
                           int colmn_idx,
                           sqlite3_stmt *stmt,
                           char *obj_f_ptr)
{
    const unsigned char *cptr;
    switch(mmbrd.get_field_vlg_type()) {
        case Type_BOOL:
            *(bool *)obj_f_ptr = sqlite3_column_int(stmt, colmn_idx) ? true : false;
            return 0;
        case Type_INT16:
            *(short *)obj_f_ptr = (short)sqlite3_column_int(stmt, colmn_idx);
            return 0;
        case Type_UINT16:
            *(unsigned short *)obj_f_ptr = (unsigned short)sqlite3_column_int(stmt, colmn_idx);
            return 0;
        case Type_INT32:
            *(int *)obj_f_ptr = sqlite3_column_int(stmt, colmn_idx);
            return 0;
        case Type_UINT32:
            *(unsigned int *)obj_f_ptr = (unsigned int)sqlite3_column_int(stmt, colmn_idx);
            return 0;
        case Type_INT64:
            *(int64_t *)obj_f_ptr = (int64_t)sqlite3_column_int64(stmt, colmn_idx);
            return 0;
        case Type_UINT64:
            *(uint64_t *)obj_f_ptr = (uint64_t)sqlite3_column_int64(stmt, colmn_idx);
            return 0;
        case Type_FLOAT32:
            *(float *)obj_f_ptr = (float)sqlite3_column_double(stmt, colmn_idx);
            return 0;
        case Type_FLOAT64:
            *(double *)obj_f_ptr = sqlite3_column_double(stmt, colmn_idx);
            return 0;
        case Type_ASCII:
            cptr = (const unsigned char *)sqlite3_column_text(stmt, colmn_idx);
            *(char *)obj_f_ptr = *cptr;
            return 0;
        case Type_BYTE:
            cptr = (const unsigned char *)sqlite3_column_blob(stmt, colmn_idx);
            *(char *)obj_f_ptr = *cptr;
            return 0;
        default:
            return -1;
    }
}

//pers_query_sqlite

struct pers_query_sqlite : public persistence_query_impl {
    pers_query_sqlite(unsigned int id,
                      persistence_connection_impl &conn,
                      const nentity_manager &nem,
                      sqlite3_stmt *stmt);
    sqlite3_stmt *stmt_;
};

pers_query_sqlite::pers_query_sqlite(unsigned int id,
                                     persistence_connection_impl &conn,
                                     const nentity_manager &nem,
                                     sqlite3_stmt *stmt) :
    persistence_query_impl(id, conn, nem),
    stmt_(stmt)
{}

//SQLTE_ENM_SELECT_REC_UD

struct SQLTE_ENM_SELECT_REC_UD {
    const nentity_manager &nem;
    char *obj_ptr;
    //used in enum_mmbrs_fill_entity
    int *colmn_idx;
    sqlite3_stmt *stmt;
};

// enum_mmbrs_fill_entity
bool enum_mmbrs_fill_entity(const member_desc &mmbrd, void *usr_data)
{
    SQLTE_ENM_SELECT_REC_UD *rud = static_cast<SQLTE_ENM_SELECT_REC_UD *>(usr_data);
    char *obj_f_ptr = nullptr;
    if(mmbrd.get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd.get_field_nentity_type() == NEntityType_NENUM) {
            //treat enum as number
            if(mmbrd.get_field_nmemb() > 1) {
                for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                    //value
                    obj_f_ptr = rud->obj_ptr + mmbrd.get_field_offset() + mmbrd.get_field_type_size()*i;
                    *(int *)obj_f_ptr = sqlite3_column_int(rud->stmt, (*(rud->colmn_idx))++);
                }
            } else {
                //value
                obj_f_ptr = rud->obj_ptr + mmbrd.get_field_offset();
                *(int *)obj_f_ptr = sqlite3_column_int(rud->stmt, (*(rud->colmn_idx))++);
            }
        } else {
            //class, struct is a recursive step.
            SQLTE_ENM_SELECT_REC_UD rrud = *rud;
            const nentity_desc *edsc = rud->nem.get_nentity_descriptor(mmbrd.get_field_user_type());
            if(edsc) {
                if(mmbrd.get_field_nmemb() > 1) {
                    for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                        rrud.obj_ptr = rud->obj_ptr + mmbrd.get_field_offset() + mmbrd.get_field_type_size()*i;
                        edsc->enum_member_descriptors(enum_mmbrs_fill_entity, &rrud);
                    }
                } else {
                    rrud.obj_ptr = rud->obj_ptr + mmbrd.get_field_offset();
                    edsc->enum_member_descriptors(enum_mmbrs_fill_entity, &rrud);
                }
            }
        }
    } else {
        //primitive type
        if(mmbrd.get_field_vlg_type() == Type_ASCII) {
            //value
            obj_f_ptr = rud->obj_ptr + mmbrd.get_field_offset();
            strncpy(obj_f_ptr,
                    (const char *)sqlite3_column_text(rud->stmt, (*(rud->colmn_idx))++),
                    mmbrd.get_field_type_size()*mmbrd.get_field_nmemb());
        } else if(mmbrd.get_field_vlg_type() == Type_BYTE) {
            //value
            obj_f_ptr = rud->obj_ptr + mmbrd.get_field_offset();
            memcpy(obj_f_ptr,
                   (const char *)sqlite3_column_blob(rud->stmt, (*(rud->colmn_idx))++),
                   mmbrd.get_field_type_size()*mmbrd.get_field_nmemb());
        } else if(mmbrd.get_field_nmemb() > 1) {
            for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                //value
                obj_f_ptr = rud->obj_ptr + mmbrd.get_field_offset() + mmbrd.get_field_type_size()*i;
                entity_fill_fld(mmbrd, (*(rud->colmn_idx))++, rud->stmt, obj_f_ptr);
            }
        } else {
            //value
            obj_f_ptr = rud->obj_ptr + mmbrd.get_field_offset();
            entity_fill_fld(mmbrd, (*(rud->colmn_idx))++, rud->stmt, obj_f_ptr);
        }
    }
    return true;
}

//pers_conn_sqlite
struct pers_conn_sqlite : public persistence_connection_impl {
        pers_conn_sqlite(persistence_connection_pool &conn_pool);

        inline RetCode sqlite_connect(const char *filename,
                                      int flags);

        inline RetCode sqlite_disconnect();

        inline RetCode sqlite_exec_stmt(const char *stmt,
                                        bool fail_is_error = true);

        inline RetCode sqlite_prepare_stmt(const char *sql_stmt,
                                           sqlite3_stmt **stmt);

        inline RetCode sqlite_bind_where_clause(unsigned int key,
                                                unsigned int strt_col_idx,
                                                const nclass &in,
                                                sqlite3_stmt *stmt);

        inline RetCode sqlite_bind_obj_fields(unsigned int ts_0,
                                              unsigned int ts_1,
                                              const nclass &in,
                                              const nentity_manager &nem,
                                              sqlite3_stmt *stmt,
                                              unsigned int *bnd_col_idx = nullptr);

        inline RetCode sqlite_step_stmt(sqlite3_stmt *stmt,
                                        int &sqlite_rc);

        inline RetCode sqlite_release_stmt(sqlite3_stmt *stmt);

        virtual RetCode do_connect() override;

        virtual RetCode do_create_table(const nentity_manager &nem,
                                        const nentity_desc &edesc,
                                        bool drop_if_exist) override;

        virtual RetCode do_select(unsigned int key,
                                  const nentity_manager &nem,
                                  unsigned int &ts0_out,
                                  unsigned int &ts1_out,
                                  nclass &in_out) override;

        virtual RetCode do_update(unsigned int key,
                                  const nentity_manager &nem,
                                  unsigned int ts0,
                                  unsigned int ts1,
                                  const nclass &in) override;

        virtual RetCode do_delete(unsigned int key,
                                  const nentity_manager &nem,
                                  unsigned int ts0,
                                  unsigned int ts1,
                                  PersistenceDeletionMode mode,
                                  const nclass &in) override;

        virtual RetCode do_insert(const nentity_manager &nem,
                                  unsigned int ts0,
                                  unsigned int ts1,
                                  const nclass &in,
                                  bool fail_is_error = true) override;

        virtual RetCode do_execute_query(const nentity_manager &nem,
                                         const char *sql,
                                         std::unique_ptr<persistence_query_impl> &qry_out) override;

        virtual RetCode do_release_query(persistence_query_impl &qry) override;

        virtual RetCode do_next_entity_from_query(persistence_query_impl &qry,
                                                  unsigned int &ts0_out,
                                                  unsigned int &ts1_out,
                                                  nclass &out) override;

        virtual RetCode do_execute_statement(const char *sql) override;


    private:
        static inline RetCode read_timestamp_and_del_from_record(sqlite3_stmt *stmt,
                                                                 int &sqlite_rc,
                                                                 unsigned int *ts0,
                                                                 unsigned int *ts1,
                                                                 bool *del);
    protected:
        sqlite3 *db_;
        persistence_worker *worker_;

    private:

        // persistence_task_sqlite
        struct persistence_task_sqlite : public persistence_task {
                persistence_task_sqlite(pers_conn_sqlite &sql_conn, VLG_PERS_TASK_OP op_code) :
                    persistence_task(op_code),
                    sql_conn_(sql_conn),
                    sel_stmt_(nullptr) {
                }

            protected:
                virtual RetCode do_connect() override {
                    IFLOG(trc(TH_ID, LS_OPN "[url:%s, user:%s, password:%s]", __func__,
                              sql_conn_.conn_pool_.url_.c_str(),
                              sql_conn_.conn_pool_.usr_.c_str(),
                              sql_conn_.conn_pool_.psswd_.c_str()))
                    RetCode rcode = sql_conn_.sqlite_connect(sql_conn_.conn_pool_.url_.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
                    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
                    return rcode;
                }

                virtual RetCode do_create_table() override {
                    RetCode rcode = RetCode_OK;
                    if((rcode = sql_conn_.sqlite_exec_stmt(stmt_bf_, false))) {
                        if(in_drop_if_exist_) {
                            std::string drop_stmt;
                            drop_stmt.assign("DROP TABLE ");
                            drop_stmt.append(in_edesc_->get_nentity_name());
                            drop_stmt.append(";");
                            IFLOG(dbg(TH_ID, LS_STM "[drop_stmt:%s]", __func__, drop_stmt.c_str()))
                            if((rcode = sql_conn_.sqlite_exec_stmt(drop_stmt.c_str()))) {
                                IFLOG(err(TH_ID, LS_CLO "[drop failed][res:%d].", __func__, rcode))
                                return rcode;
                            }
                            if((rcode = sql_conn_.sqlite_exec_stmt(stmt_bf_))) {
                                IFLOG(err(TH_ID, LS_CLO "[create after drop failed][res:%d]", __func__, rcode))
                                return rcode;
                            }
                        } else {
                            IFLOG(err(TH_ID, LS_CLO "[create failed][res:%d]", __func__, rcode))
                            return rcode;
                        }
                    }
                    return rcode;
                }

                virtual RetCode do_select() override {
                    RetCode rcode = RetCode_OK;
                    sqlite3_stmt *stmt = nullptr;
                    int sqlite_rc = 0;

                    //prepare
                    if((rcode = sql_conn_.sqlite_prepare_stmt(stmt_bf_, &stmt))) {
                        IFLOG(err(TH_ID, LS_CLO "[sqlite_prepare_stmt failed][res:%d]", __func__, rcode))
                    } else {
                        //bind
                        if((rcode = sql_conn_.sqlite_bind_where_clause(in_key_, 1, *in_out_obj_, stmt))) {
                            IFLOG(err(TH_ID, LS_CLO "[sqlite_bind_where_clause failed][res:%d]", __func__, rcode))
                            RetCode rels_rcode = RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_release_stmt failed]", __func__, rels_rcode))
                            }
                        } else if(!(rcode = sql_conn_.sqlite_step_stmt(stmt, sqlite_rc))) {
                            if(sqlite_rc == SQLITE_ROW) {
                                read_timestamp_and_del_from_record(stmt,
                                                                   sqlite_rc,
                                                                   in_out_ts0_,
                                                                   in_out_ts1_,
                                                                   nullptr);
                                int column_idx = 3; //column idx, [ts0, ts1, del] we start from 3.
                                SQLTE_ENM_SELECT_REC_UD rud = { *in_nem_,
                                                                reinterpret_cast<char *>(&in_out_obj_),
                                                                &column_idx,
                                                                stmt
                                                              };
                                in_out_obj_->get_nentity_descriptor().enum_member_descriptors(enum_mmbrs_fill_entity, &rud);
                                rcode = RetCode_DBROW;
                            } else if(sqlite_rc == SQLITE_DONE) {
                                rcode = RetCode_NODATA;
                            } else {
                                IFLOG(err(TH_ID, LS_TRL "[rc:%d][sqlite_step_stmt: unhandled sqlite code]", __func__, sqlite_rc))
                                rcode = RetCode_DBERR;
                            }
                            RetCode rels_rcode = RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_release_stmt failed]", __func__, rels_rcode))
                            }
                        } else {
                            IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_step_stmt failed]", __func__, rcode))
                        }
                    }
                    return rcode;
                }

                virtual RetCode do_insert() override {
                    RetCode rcode = RetCode_OK;
                    sqlite3_stmt *stmt = nullptr;
                    int sqlite_rc = 0;

                    //prepare
                    if((rcode = sql_conn_.sqlite_prepare_stmt(stmt_bf_, &stmt))) {
                        IFLOG(err(TH_ID, LS_CLO "[sqlite_prepare_stmt failed][res:%d]", __func__, rcode))
                    } else {
                        //bind
                        if((rcode = sql_conn_.sqlite_bind_obj_fields(*in_out_ts0_, *in_out_ts1_, *in_obj_, *in_nem_, stmt))) {
                            IFLOG(err(TH_ID, LS_CLO "[sqlite_bind_obj_fields failed][res:%d]", __func__, rcode))
                            RetCode rels_rcode = RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_release_stmt failed]", __func__, rels_rcode))
                            }
                        } else if(!(rcode = sql_conn_.sqlite_step_stmt(stmt, sqlite_rc))) {
                            if(sqlite_rc == SQLITE_DONE) {
                                rcode = RetCode_OK;
                            } else {
                                IFLOG(err(TH_ID, LS_TRL "[rc:%d][sqlite_step_stmt: unhandled sqlite code]", __func__, sqlite_rc))
                                rcode = RetCode_DBERR;
                            }
                            RetCode rels_rcode = RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_release_stmt failed]", __func__, rels_rcode))
                            }
                        } else {
                            IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_step_stmt failed]", __func__, rcode))
                        }
                    }
                    return rcode;
                }

                virtual RetCode do_update() override {
                    RetCode rcode = RetCode_OK;
                    sqlite3_stmt *stmt = nullptr;
                    int sqlite_rc = 0;

                    //prepare
                    if((rcode = sql_conn_.sqlite_prepare_stmt(stmt_bf_, &stmt))) {
                        IFLOG(err(TH_ID, LS_CLO "[sqlite_prepare_stmt failed][res:%d]", __func__, rcode))
                    } else {
                        //bind
                        unsigned int bnd_col_idx = 0;
                        if((rcode = sql_conn_.sqlite_bind_obj_fields(*in_out_ts0_, *in_out_ts1_, *in_obj_, *in_nem_, stmt, &bnd_col_idx))) {
                            IFLOG(err(TH_ID, LS_CLO "[sqlite_bind_obj_fields failed][res:%d]", __func__, rcode))
                            RetCode rels_rcode = RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_release_stmt failed]", __func__, rels_rcode))
                            }
                        } else if((rcode = sql_conn_.sqlite_bind_where_clause(in_key_, bnd_col_idx, *in_obj_, stmt))) {
                            IFLOG(err(TH_ID, LS_CLO "[sqlite_bind_where_clause failed][res:%d]", __func__, rcode))
                            RetCode rels_rcode = RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_release_stmt failed]", __func__, rels_rcode))
                            }
                        } else if(!(rcode = sql_conn_.sqlite_step_stmt(stmt, sqlite_rc))) {
                            if(sqlite_rc == SQLITE_DONE) {
                                rcode = RetCode_OK;
                            } else {
                                IFLOG(err(TH_ID, LS_TRL "[rc:%d][sqlite_step_stmt: unhandled sqlite code]", __func__, sqlite_rc))
                                rcode = RetCode_DBERR;
                            }
                            RetCode rels_rcode = RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_release_stmt failed]", __func__, rels_rcode))
                            }
                        } else {
                            IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_step_stmt failed]", __func__, rcode))
                        }
                    }
                    return rcode;
                }

                virtual RetCode do_delete() override {
                    RetCode rcode = RetCode_OK;
                    sqlite3_stmt *stmt = nullptr;
                    int sqlite_rc = 0;

                    //prepare
                    if((rcode = sql_conn_.sqlite_prepare_stmt(stmt_bf_, &stmt))) {
                        IFLOG(err(TH_ID, LS_CLO "[sqlite_prepare_stmt failed][res:%d]", __func__, rcode))
                    } else {
                        //bind
                        if((rcode = sql_conn_.sqlite_bind_where_clause(in_key_, 1, *in_out_obj_, stmt))) {
                            IFLOG(err(TH_ID, LS_CLO "[sqlite_bind_where_clause failed][res:%d]", __func__, rcode))
                            RetCode rels_rcode = RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_release_stmt failed]", __func__, rels_rcode))
                            }
                        } else if(!(rcode = sql_conn_.sqlite_step_stmt(stmt, sqlite_rc))) {
                            if(sqlite_rc == SQLITE_DONE) {
                                rcode = RetCode_OK;
                            } else {
                                IFLOG(err(TH_ID, LS_TRL "[rc:%d][sqlite_step_stmt: unhandled sqlite code]", __func__, sqlite_rc))
                                rcode = RetCode_DBERR;
                            }
                            RetCode rels_rcode = RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_release_stmt failed]", __func__, rels_rcode))
                            }
                        } else {
                            IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_step_stmt failed]", __func__, rcode))
                        }
                    }
                    return rcode;
                }

                virtual RetCode do_execute_query() override {
                    RetCode rcode = RetCode_OK;
                    sqlite3_stmt *stmt = nullptr;
                    if((rcode = sql_conn_.sqlite_prepare_stmt(in_sql_, &stmt))) {
                        IFLOG(err(TH_ID, LS_CLO "[sqlite_prepare_stmt failed][res:%d]", __func__, rcode))
                        return rcode;
                    }
                    in_out_query_ = new pers_query_sqlite(0, sql_conn_, *in_nem_, stmt);  //@fixme sanity and id..
                    return rcode;
                }

                virtual RetCode do_release_query() override {
                    pers_query_sqlite *qry_sqlite = static_cast<pers_query_sqlite *>(in_out_query_);
                    RetCode rcode = sql_conn_.sqlite_release_stmt(qry_sqlite->stmt_);
                    return rcode;
                }

                virtual RetCode do_next_entity_from_query() override {
                    RetCode rcode = RetCode_OK;
                    int sqlite_rc = 0;
                    if(!(rcode = sql_conn_.sqlite_step_stmt(sel_stmt_, sqlite_rc))) {
                        if(sqlite_rc == SQLITE_ROW) {
                            read_timestamp_and_del_from_record(sel_stmt_,
                                                               sqlite_rc,
                                                               in_out_ts0_,
                                                               in_out_ts1_,
                                                               nullptr);
                            int column_idx = 3; //column idx, [ts0, ts1, del] we start from 3.
                            SQLTE_ENM_SELECT_REC_UD rud = { *in_nem_,
                                                            reinterpret_cast<char *>(in_out_obj_),
                                                            &column_idx,
                                                            sel_stmt_
                                                          };
                            in_out_obj_->get_nentity_descriptor().enum_member_descriptors(enum_mmbrs_fill_entity, &rud);
                            rcode = RetCode_DBROW;
                        } else if(sqlite_rc == SQLITE_DONE) {
                            rcode = RetCode_QRYEND;
                        } else {
                            IFLOG(err(TH_ID, LS_TRL "[rc:%d][sqlite_step_stmt unhandled sqlite code]", __func__, sqlite_rc))
                            rcode = RetCode_DBERR;
                        }
                    } else {
                        IFLOG(err(TH_ID, LS_TRL "[res:%d][sqlite_step_stmt failed]", __func__, rcode))
                    }
                    return rcode;
                }

                virtual RetCode do_execute_statement() override {
                    return sql_conn_.sqlite_exec_stmt(in_sql_);
                }

            private:
                pers_conn_sqlite &sql_conn_;

            public:
                sqlite3_stmt *sel_stmt_;
        };
};

pers_conn_sqlite::pers_conn_sqlite(persistence_connection_pool &conn_pool) :
    persistence_connection_impl(conn_pool),
    db_(nullptr),
    worker_(nullptr)
{}

inline RetCode pers_conn_sqlite::sqlite_connect(const char *filename, int flags)
{
    IFLOG(trc(TH_ID, LS_OPN "[filename:%s, flags:%d]", __func__, filename, flags))
    RetCode rcode = RetCode_OK;
    int last_rc = sqlite3_open_v2(filename, &db_, flags, 0);
    if(last_rc) {
        IFLOG(err(TH_ID, LS_TRL"[filename:%s][sqlite3_open_v2 - rc:%d - errdesc:%s]",
                  __func__, filename, last_rc, sqlite3_errstr(last_rc)))
        status_ = PersistenceConnectionStatus_ERROR;
        rcode = RetCode_DBERR;
    } else {
        status_ = PersistenceConnectionStatus_CONNECTED;
    }
    return rcode;
}

inline RetCode pers_conn_sqlite::sqlite_disconnect()
{
    RetCode rcode = RetCode_OK;
    int last_rc = sqlite3_close_v2(db_);
    if(last_rc) {
        IFLOG(err(TH_ID, LS_CLO "[sqlite3_close_v2(rc:%d) - errdesc[%s] - db error]", __func__,
                  last_rc,
                  sqlite3_errstr(last_rc)))
        status_ = PersistenceConnectionStatus_ERROR;
        rcode = RetCode_DBERR;
    } else {
        status_ = PersistenceConnectionStatus_DISCONNECTED;
    }
    return rcode;
}

inline RetCode pers_conn_sqlite::sqlite_exec_stmt(const char *stmt, bool fail_is_error)
{
    RetCode rcode = RetCode_OK;
    char *zErrMsg = 0;
    int last_rc = sqlite3_exec(db_, stmt, 0, 0, &zErrMsg);
    if(last_rc != SQLITE_OK) {
        if(fail_is_error) {
            IFLOG(err(TH_ID, LS_TRL"[sqlite3_exec - rc:%d - errdesc:%s]", __func__, last_rc, zErrMsg))
        } else {
            IFLOG(dbg(TH_ID, LS_TRL"[sqlite3_exec - rc:%d - errdesc:%s]", __func__, last_rc, zErrMsg))
        }
        sqlite3_free(zErrMsg);
        rcode = RetCode_DBOPFAIL;
    }
    return rcode;
}

inline RetCode pers_conn_sqlite::sqlite_prepare_stmt(const char *sql_stmt, sqlite3_stmt **stmt)
{
    if(!stmt) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADARG;
    }
    RetCode rcode = RetCode_OK;
    int last_rc = sqlite3_prepare_v2(db_, sql_stmt, (int)strlen(sql_stmt), stmt, 0);
    if(last_rc != SQLITE_OK) {
        IFLOG(err(TH_ID, LS_TRL "[sqlite3_prepare_v2 rc:%d - errdesc:%s]", __func__, last_rc, sqlite3_errstr(last_rc)))
        rcode = RetCode_DBERR;
    }
    return rcode;
}

struct k_bind_w_c {
    unsigned int col_idx;
    sqlite3_stmt *stmt;
    const char *obj_ptr;
    const nentity_manager *nem;
};

bool enum_keyset_bind_where_clause(const member_desc &mmbrd, void *usr_data)
{
    k_bind_w_c &kbwc = *(k_bind_w_c *)usr_data;
    const char *obj_f_ptr = kbwc.obj_ptr + mmbrd.get_field_offset();
    if(mmbrd.get_field_vlg_type() == Type_ENTITY) {
        //treat enum as int
        sqlite3_bind_int(kbwc.stmt, kbwc.col_idx++, *(int *)obj_f_ptr);
    } else {
        //can be only a primitive type
        int br = bind_stmt_fld(mmbrd, obj_f_ptr, kbwc.col_idx++, kbwc.stmt);
        if(br) {
            IFLOG(err(TH_ID, LS_CLO "[bind failed:%d]", __func__, br))
        }
    }
    return true;
}

inline RetCode pers_conn_sqlite::sqlite_bind_where_clause(unsigned int key,
                                                          unsigned int strt_col_idx,
                                                          const nclass &in,
                                                          sqlite3_stmt *stmt)
{
    const key_desc *kdsc = in.get_nentity_descriptor().get_key_desc_by_id(key);
    if(!kdsc) {
        IFLOG(err(TH_ID, LS_CLO "[key:%d not found]", __func__, key))
        return RetCode_BADARG;
    }
    k_bind_w_c kbwc = { strt_col_idx, stmt, (const char *) &in };
    kdsc->enum_member_descriptors(enum_keyset_bind_where_clause, &kbwc);
    return RetCode_OK;
}

bool enum_bind_obj_fields(const member_desc &mmbrd, void *usr_data)
{
    k_bind_w_c &kbwc = *(k_bind_w_c *)usr_data;
    if(mmbrd.get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd.get_field_nentity_type() == NEntityType_NENUM) {
            size_t i = 0;
            do {
                int br = sqlite3_bind_int(kbwc.stmt,
                                          kbwc.col_idx++,
                                          *(int *)(kbwc.obj_ptr + mmbrd.get_field_offset() + mmbrd.get_field_type_size()*i++));
                if(br) {
                    IFLOG(err(TH_ID, LS_CLO "[bind failed:%d, %s]", __func__, br, mmbrd.get_member_name()))
                }
            } while(i < mmbrd.get_field_nmemb());
        } else {
            //class, struct is a recursive step.
            k_bind_w_c rkbwc = kbwc;
            const nentity_desc *edsc = kbwc.nem->get_nentity_descriptor(mmbrd.get_field_user_type());
            if(edsc) {
                size_t i = 0;
                do {
                    rkbwc.obj_ptr = kbwc.obj_ptr + mmbrd.get_field_offset() + mmbrd.get_field_type_size()*i++;
                    edsc->enum_member_descriptors(enum_bind_obj_fields, &rkbwc);
                } while(i < mmbrd.get_field_nmemb());
                kbwc.col_idx = rkbwc.col_idx;
            }
        }
    } else {
        //primitive type
        if(mmbrd.get_field_vlg_type() == Type_ASCII || mmbrd.get_field_vlg_type() == Type_BYTE) {
            int br = bind_stmt_fld(mmbrd,
                                   kbwc.obj_ptr + mmbrd.get_field_offset(),
                                   kbwc.col_idx++,
                                   kbwc.stmt);
            if(br) {
                IFLOG(err(TH_ID, LS_CLO "[bind failed:%d, %s]", __func__, br, mmbrd.get_member_name()))
            }
        } else {
            size_t i = 0;
            do {
                int br = bind_stmt_fld(mmbrd,
                                       kbwc.obj_ptr + mmbrd.get_field_offset() + mmbrd.get_field_type_size()*i++,
                                       kbwc.col_idx++,
                                       kbwc.stmt);
                if(br) {
                    IFLOG(err(TH_ID, LS_CLO "[bind failed:%d, %s]", __func__, br, mmbrd.get_member_name()))
                }
            } while(i < mmbrd.get_field_nmemb());
        }
    }
    return true;
}

RetCode pers_conn_sqlite::sqlite_bind_obj_fields(unsigned int ts_0,
                                                 unsigned int ts_1,
                                                 const nclass &in,
                                                 const nentity_manager &nem,
                                                 sqlite3_stmt *stmt,
                                                 unsigned int *bnd_col_idx)
{
    k_bind_w_c kbwc = { 3, stmt, (const char *) &in, &nem };
    sqlite3_bind_int(stmt, 1, ts_0);
    sqlite3_bind_int(stmt, 2, ts_1);
    in.get_nentity_descriptor().enum_member_descriptors(enum_bind_obj_fields, &kbwc);
    if(bnd_col_idx) {
        *bnd_col_idx = kbwc.col_idx;
    }
    return RetCode_OK;
}

inline RetCode pers_conn_sqlite::sqlite_step_stmt(sqlite3_stmt *stmt, int &sqlite_rc)
{
    RetCode rcode = RetCode_OK;
    if(!stmt) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADARG;
    }
    sqlite_rc = sqlite3_step(stmt);
    if(sqlite_rc != SQLITE_ROW && sqlite_rc != SQLITE_DONE) {
        IFLOG(inf(TH_ID, LS_TRL "[sqlite3_step rc:%d - errdesc:%s]", __func__, sqlite_rc, sqlite3_errstr(sqlite_rc)))
        rcode = RetCode_DBERR;
    }
    return rcode;
}

inline RetCode pers_conn_sqlite::sqlite_release_stmt(sqlite3_stmt *stmt)
{
    RetCode rcode = RetCode_OK;
    if(!stmt) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADARG;
    }
    int last_rc = sqlite3_finalize(stmt);
    if(last_rc != SQLITE_OK) {
        IFLOG(err(TH_ID, LS_TRL "[sqlite3_finalize rc:%d]", __func__, last_rc))
        rcode = RetCode_DBERR;
    }
    return rcode;
}

inline RetCode pers_conn_sqlite::read_timestamp_and_del_from_record(sqlite3_stmt *stmt,
                                                                    int &sqlite_rc,
                                                                    unsigned int *ts0,
                                                                    unsigned int *ts1,
                                                                    bool *del)
{
    RetCode rcode = RetCode_OK;
    //TS0
    *ts0 = (unsigned int)sqlite3_column_int(stmt, 0);
    //TS1
    *ts1 = (unsigned int)sqlite3_column_int(stmt, 1);
    //DEL
    if(del) {
        *del = sqlite3_column_int(stmt, 2) ? true : false;
    }
    return rcode;
}

//--------------------- CONNECT -------------------------------------------------

inline RetCode pers_conn_sqlite::do_connect()
{
    RetCode rcode = RetCode_OK;
    worker_ = conn_pool_.get_worker_rr_can_create_start();
    if(worker_ == nullptr) {
        IFLOG(cri(TH_ID, LS_CLO "[thread unavailable]", __func__, RetCode_UNVRSC))
        return RetCode_UNVRSC;
    }
    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_CONNECT));
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    return task->op_res_;
}

//--------------------- CREATE -------------------------------------------------

struct SQLTE_ENM_CREATE_REC_UD {
    const nentity_manager &nem;
    std::string *create_stmt;
    std::string *prfx;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array
    bool *first_key;
    bool *first_key_mmbr;
    RetCode *last_error_code;
    std::string *last_error_msg;
};

bool enum_mmbrs_create_table(const member_desc &mmbrd, void *usr_data)
{
    SQLTE_ENM_CREATE_REC_UD *rud = static_cast<SQLTE_ENM_CREATE_REC_UD *>(usr_data);
    std::string idx_prfx;
    char idx_b[SQLITE_FIDX_BUFF] = {0};
    idx_prfx.assign(*(rud->prfx));
    if(rud->array_fld) {
        sprintf(idx_b, "%s%u", idx_prfx.length() ? "_" : "", rud->fld_idx);
        idx_prfx.append(idx_b);
    }
    if(mmbrd.get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd.get_field_nentity_type() == NEntityType_NENUM) {
            //treat enum as number
            if(mmbrd.get_field_nmemb() > 1) {
                for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                    if(rud->prfx->length()) {
                        rud->create_stmt->append(idx_prfx);
                        rud->create_stmt->append("_");
                    }
                    sprintf(idx_b, "_%u", i);
                    rud->create_stmt->append(mmbrd.get_member_name());
                    rud->create_stmt->append(idx_b);
                    rud->create_stmt->append(" " VLG_SQLITE_DTYPE_NUMERIC", ");
                }
            } else {
                if(rud->prfx->length()) {
                    rud->create_stmt->append(idx_prfx);
                    rud->create_stmt->append("_");
                }
                rud->create_stmt->append(mmbrd.get_member_name());
                rud->create_stmt->append(" " VLG_SQLITE_DTYPE_NUMERIC", ");
            }
        } else {
            //class, struct is a recursive step.
            SQLTE_ENM_CREATE_REC_UD rrud = *rud;
            std::string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd.get_member_name());
            rrud.prfx = &rprfx;
            const nentity_desc *edsc = rud->nem.get_nentity_descriptor(mmbrd.get_field_user_type());
            if(edsc) {
                if(mmbrd.get_field_nmemb() > 1) {
                    rrud.array_fld = true;
                    for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                        rrud.fld_idx = i;
                        edsc->enum_member_descriptors(enum_mmbrs_create_table, &rrud);
                    }
                } else {
                    edsc->enum_member_descriptors(enum_mmbrs_create_table, &rrud);
                }
            }
        }
    } else {
        //primitive type
        if(mmbrd.get_field_vlg_type() == Type_ASCII) {
            if(rud->prfx->length()) {
                rud->create_stmt->append(idx_prfx);
                rud->create_stmt->append("_");
            }
            rud->create_stmt->append(mmbrd.get_member_name());
            rud->create_stmt->append(" " VLG_SQLITE_DTYPE_TEXT", ");
        } else if(mmbrd.get_field_vlg_type() == Type_BYTE) {
            if(rud->prfx->length()) {
                rud->create_stmt->append(idx_prfx);
                rud->create_stmt->append("_");
            }
            rud->create_stmt->append(mmbrd.get_member_name());
            rud->create_stmt->append(" " VLG_SQLITE_DTYPE_BLOB", ");
        } else if(mmbrd.get_field_nmemb() > 1) {
            for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                if(rud->prfx->length()) {
                    rud->create_stmt->append(idx_prfx);
                    rud->create_stmt->append("_");
                }
                sprintf(idx_b, "_%u", i);
                rud->create_stmt->append(mmbrd.get_member_name());
                rud->create_stmt->append(idx_b);
                rud->create_stmt->append(" ");
                rud->create_stmt->append(SQLITE_TypeStr_From_VLGType(mmbrd.get_field_vlg_type()));
                rud->create_stmt->append(", ");
            }
        } else {
            if(rud->prfx->length()) {
                rud->create_stmt->append(idx_prfx);
                rud->create_stmt->append("_");
            }
            rud->create_stmt->append(mmbrd.get_member_name());
            rud->create_stmt->append(" ");
            rud->create_stmt->append(SQLITE_TypeStr_From_VLGType(mmbrd.get_field_vlg_type()));
            rud->create_stmt->append(", ");
        }
    }
    return true;
}

bool enum_keyset_create_table(const member_desc &mmbrd, void *usr_data)
{
    SQLTE_ENM_CREATE_REC_UD *rud = static_cast<SQLTE_ENM_CREATE_REC_UD *>(usr_data);
    //coma handling
    if(*(rud->first_key_mmbr)) {
        *(rud->first_key_mmbr) = false;
    } else {
        rud->create_stmt->append(", ");
    }
    rud->create_stmt->append(mmbrd.get_member_name());
    return true;
}

bool enum_keys_create_table(const key_desc &kdsc, void *usr_data)
{
    SQLTE_ENM_CREATE_REC_UD *rud = static_cast<SQLTE_ENM_CREATE_REC_UD *>(usr_data);
    //coma handling
    if(*(rud->first_key)) {
        *(rud->first_key) = false;
    } else {
        //suppressed because we can have only 1 key
        //rud->stmt.Apnd(",\n");
    }
    if(kdsc.is_primary()) {
        rud->create_stmt->append("PRIMARY KEY(");
    } else {
        //only 1 primary key can be defined with sqlite
        return false;
        //rud->stmt.Apnd("KEY(");
    }
    *(rud->first_key_mmbr) = true;

    kdsc.enum_member_descriptors(enum_keyset_create_table, rud);
    rud->create_stmt->append(")");
    return true;
}

RetCode pers_conn_sqlite::do_create_table(const nentity_manager &nem,
                                          const nentity_desc &edesc,
                                          bool drop_if_exist)
{
    RetCode rcode = RetCode_OK;
    RetCode last_error_code = RetCode_OK;
    std::string last_error_str;
    std::string create_stmt;
    create_stmt.assign("CREATE TABLE ");
    create_stmt.append(edesc.get_nentity_name());
    create_stmt.append("(");
    create_stmt.append(P_F_TS0" " VLG_SQLITE_DTYPE_NUMERIC", ");
    create_stmt.append(P_F_TS1" " VLG_SQLITE_DTYPE_NUMERIC", ");
    create_stmt.append(P_F_DEL" " VLG_SQLITE_DTYPE_NUMERIC", ");
    std::string prfx;
    prfx.assign("");
    bool frst_key = true, frst_key_mmbr = true;
    SQLTE_ENM_CREATE_REC_UD rud = { nem,
                                    &create_stmt,
                                    &prfx,
                                    false,
                                    0,
                                    &frst_key,
                                    &frst_key_mmbr,
                                    &last_error_code,
                                    &last_error_str
                                  };

    edesc.enum_member_descriptors(enum_mmbrs_create_table, &rud);
    edesc.enum_key_descriptors(enum_keys_create_table, &rud);

    create_stmt.append(");");
    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_CREATETABLE));
    task->in_edesc_ = &edesc;
    task->in_drop_if_exist_ = drop_if_exist;
    task->stmt_bf_ = create_stmt.c_str();
    IFLOG(dbg(TH_ID, LS_STM "[create_stmt:%s]", __func__, create_stmt.c_str()))
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    return task->op_res_;
}

//--------------------- SELECT -------------------------------------------------

struct SQLTE_ENM_KSET_SELECT_REC_UD {
    std::string *where_claus;
    bool *first_key;
};

bool enum_keyset_select_table(const member_desc &mmbrd, void *usr_data)
{
    SQLTE_ENM_KSET_SELECT_REC_UD &rud = *static_cast<SQLTE_ENM_KSET_SELECT_REC_UD *>(usr_data);
    if(*(rud.first_key)) {
        *(rud.first_key) = false;
    } else {
        rud.where_claus->append(" AND ");
    }
    rud.where_claus->append(mmbrd.get_member_name());
    rud.where_claus->append("=?");
    return true;
}

RetCode pers_conn_sqlite::do_select(unsigned int key,
                                    const nentity_manager &nem,
                                    unsigned int &ts0_out,
                                    unsigned int &ts1_out,
                                    nclass &in_out)
{
    static pthread_rwlock_t sel_stmt_m_l = PTHREAD_RWLOCK_INITIALIZER;
    static std::unordered_map<std::string, std::string> sel_stmt_m;
    RetCode rcode = RetCode_OK;
    const char *sel_stmt = nullptr;
    std::stringstream ss;
    ss << in_out.get_id() << '_' << key;

    {
        scoped_rd_lock rl(sel_stmt_m_l);
        auto it = sel_stmt_m.find(ss.str());
        if(it != sel_stmt_m.end()) {
            sel_stmt = it->second.c_str();
        }
    }

    if(!sel_stmt) {
        std::string select_stmt;
        std::string where_claus(P_F_DEL"=0 AND ");
        bool frst_key = true;
        SQLTE_ENM_KSET_SELECT_REC_UD rud = { &where_claus, &frst_key };

        const key_desc *kdsc = in_out.get_nentity_descriptor().get_key_desc_by_id(key);
        if(!kdsc) {
            IFLOG(err(TH_ID, LS_CLO "[key:%d not found]", __func__, key))
            return RetCode_BADARG;
        }

        kdsc->enum_member_descriptors(enum_keyset_select_table, &rud);

        select_stmt.assign("SELECT * FROM ");
        select_stmt.append(in_out.get_nentity_descriptor().get_nentity_name());
        select_stmt.append(" WHERE ");
        select_stmt.append(where_claus);
        select_stmt.append(";");
        {
            scoped_wr_lock wl(sel_stmt_m_l);
            auto nit = sel_stmt_m.insert(std::pair<std::string, std::string>(ss.str(), select_stmt));
            sel_stmt = nit.first->second.c_str();
        }
    }

    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_SELECT));
    task->in_nem_ = &nem;
    task->in_key_ = key;
    task->in_out_ts0_ = &ts0_out;
    task->in_out_ts1_ = &ts1_out;
    task->in_out_obj_ = &in_out;
    task->stmt_bf_ = sel_stmt;

    IFLOG(trc(TH_ID, LS_QRY "[select_stmt:%s]", __func__, sel_stmt))
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    return task->op_res_;
}

//--------------------- UPDATE -------------------------------------------------

struct SQLTE_ENM_UPDATE_REC_UD {
    const nentity_manager &nem;
    const char *obj_ptr;
    std::string *prfx;
    std::string *set_section;
    std::string *where_claus;

    bool *first_fld;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array

    bool *first_key;
    bool *first_key_mmbr;

    RetCode *last_error_code;
    std::string *last_error_msg;
};

bool enum_mmbrs_update(const member_desc &mmbrd, void *usr_data)
{
    SQLTE_ENM_UPDATE_REC_UD *rud = static_cast<SQLTE_ENM_UPDATE_REC_UD *>(usr_data);
    std::string idx_prfx;
    char idx_b[SQLITE_FIDX_BUFF] = { 0 };
    idx_prfx.assign(*(rud->prfx));
    if(rud->array_fld) {
        sprintf(idx_b, "%s%u", idx_prfx.length() ? "_" : "", rud->fld_idx);
        idx_prfx.append(idx_b);
    }
    if(mmbrd.get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd.get_field_nentity_type() == NEntityType_NENUM) {
            //treat enum as number
            //coma handling
            if(*(rud->first_fld)) {
                *(rud->first_fld) = false;
            } else {
                rud->set_section->append(", ");
            }
            if(mmbrd.get_field_nmemb() > 1) {
                bool frst_ar_idx = true;
                for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                    if(frst_ar_idx) {
                        frst_ar_idx = false;
                    } else {
                        rud->set_section->append(", ");
                    }
                    if(rud->prfx->length()) {
                        rud->set_section->append(idx_prfx);
                        rud->set_section->append("_");
                    }
                    sprintf(idx_b, "_%u", i);
                    rud->set_section->append(mmbrd.get_member_name());
                    rud->set_section->append(idx_b);
                    rud->set_section->append("=?");
                }
            } else {
                if(rud->prfx->length()) {
                    rud->set_section->append(idx_prfx);
                    rud->set_section->append("_");
                }
                rud->set_section->append(mmbrd.get_member_name());
                rud->set_section->append("=?");
            }
        } else {
            //class, struct is a recursive step.
            SQLTE_ENM_UPDATE_REC_UD rrud = *rud;
            std::string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd.get_member_name());
            rrud.prfx = &rprfx;
            const nentity_desc *edsc = rud->nem.get_nentity_descriptor(mmbrd.get_field_user_type());
            if(edsc) {
                if(mmbrd.get_field_nmemb() > 1) {
                    rrud.array_fld = true;
                    for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                        rrud.fld_idx = i;
                        rrud.obj_ptr = rud->obj_ptr + mmbrd.get_field_offset() + mmbrd.get_field_type_size()*i;
                        edsc->enum_member_descriptors(enum_mmbrs_update, &rrud);
                    }
                } else {
                    rrud.obj_ptr = rud->obj_ptr + mmbrd.get_field_offset();
                    edsc->enum_member_descriptors(enum_mmbrs_update, &rrud);
                }
            }
        }
    } else {
        //primitive type
        //coma handling
        if(*(rud->first_fld)) {
            *(rud->first_fld) = false;
        } else {
            rud->set_section->append(", ");
        }
        if(mmbrd.get_field_vlg_type() == Type_ASCII || mmbrd.get_field_vlg_type() == Type_BYTE) {
            if(rud->prfx->length()) {
                rud->set_section->append(idx_prfx);
                rud->set_section->append("_");
            }
            rud->set_section->append(mmbrd.get_member_name());
            rud->set_section->append("=?");
        } else if(mmbrd.get_field_nmemb() > 1) {
            bool frst_ar_idx = true;
            for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                if(frst_ar_idx) {
                    frst_ar_idx = false;
                } else {
                    rud->set_section->append(", ");
                }
                if(rud->prfx->length()) {
                    rud->set_section->append(idx_prfx);
                    rud->set_section->append("_");
                }
                sprintf(idx_b, "_%u", i);
                rud->set_section->append(mmbrd.get_member_name());
                rud->set_section->append(idx_b);
                rud->set_section->append("=?");
            }
        } else {
            if(rud->prfx->length()) {
                rud->set_section->append(idx_prfx);
                rud->set_section->append("_");
            }
            rud->set_section->append(mmbrd.get_member_name());
            rud->set_section->append("=?");
        }
    }
    return true;
}

bool enum_keyset_update_table(const member_desc &mmbrd, void *usr_data)
{
    SQLTE_ENM_UPDATE_REC_UD &rud = *static_cast<SQLTE_ENM_UPDATE_REC_UD *>(usr_data);
    if(*(rud.first_key)) {
        *(rud.first_key) = false;
    } else {
        rud.where_claus->append(" AND ");
    }
    rud.where_claus->append(mmbrd.get_member_name());
    rud.where_claus->append("=?");
    return true;
}

RetCode pers_conn_sqlite::do_update(unsigned int key,
                                    const nentity_manager &nem,
                                    unsigned int ts0,
                                    unsigned int ts1,
                                    const nclass &in)
{
    static pthread_rwlock_t upd_stmt_m_l = PTHREAD_RWLOCK_INITIALIZER;
    static std::unordered_map<std::string, std::string> upd_stmt_m;
    RetCode rcode = RetCode_OK;
    const char *upd_stmt = nullptr;
    std::stringstream ss;
    ss << in.get_id() << '_' << key;

    {
        scoped_rd_lock rl(upd_stmt_m_l);
        auto it = upd_stmt_m.find(ss.str());
        if(it != upd_stmt_m.end()) {
            upd_stmt = it->second.c_str();
        }
    }

    if(!upd_stmt) {
        RetCode last_error_code = RetCode_OK;
        std::string update_stmt;
        std::string last_error_str;
        std::string set_section(P_F_TS0"=?," P_F_TS1"=?,");
        std::string where_claus(P_F_DEL"=0 AND ");
        std::string prfx;
        bool frst_fld = true, frst_key = true, frst_key_mmbr = true;
        SQLTE_ENM_UPDATE_REC_UD rud = { nem,
                                        reinterpret_cast<const char *>(&in),
                                        &prfx,
                                        &set_section,
                                        &where_claus,
                                        &frst_fld,
                                        false,
                                        0,
                                        &frst_key,
                                        &frst_key_mmbr,
                                        &last_error_code,
                                        &last_error_str
                                      };

        const key_desc *kdsc = in.get_nentity_descriptor().get_key_desc_by_id(key);
        if(!kdsc) {
            IFLOG(err(TH_ID, LS_CLO "[key:%d not found]", __func__, key))
            return RetCode_BADARG;
        }

        kdsc->enum_member_descriptors(enum_keyset_update_table, &rud);
        in.get_nentity_descriptor().enum_member_descriptors(enum_mmbrs_update, &rud);

        update_stmt.assign("UPDATE ");
        update_stmt.append(in.get_nentity_descriptor().get_nentity_name());
        update_stmt.append(" SET ");
        update_stmt.append(set_section);
        update_stmt.append(" WHERE ");
        update_stmt.append(where_claus);
        update_stmt.append(";");

        {
            scoped_wr_lock wl(upd_stmt_m_l);
            auto nit = upd_stmt_m.insert(std::pair<std::string, std::string>(ss.str(), update_stmt));
            upd_stmt = nit.first->second.c_str();
        }
    }

    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_UPDATE));
    task->in_nem_ = &nem;
    task->in_key_ = key;
    task->in_obj_ = &in;
    task->in_out_ts0_ = &ts0;
    task->in_out_ts1_ = &ts1;
    task->stmt_bf_ = upd_stmt;
    IFLOG(dbg(TH_ID, LS_STM "[update_stmt:%s]", __func__, upd_stmt))
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    return task->op_res_;
}

//--------------------- DELETE -------------------------------------------------

struct SQLTE_ENM_DELETE_REC_UD {
    const nentity_manager &nem;
    const char *obj_ptr;
    std::string *where_claus;
    bool *first_key;
    bool *first_key_mmbr;
    RetCode *last_error_code;
    std::string *last_error_msg;
};

bool enum_keyset_delete_table(const member_desc &mmbrd, void *usr_data)
{
    SQLTE_ENM_DELETE_REC_UD &rud = *static_cast<SQLTE_ENM_DELETE_REC_UD *>(usr_data);
    if(*(rud.first_key)) {
        *(rud.first_key) = false;
    } else {
        rud.where_claus->append(" AND ");
    }
    rud.where_claus->append(mmbrd.get_member_name());
    rud.where_claus->append("=?");
    return true;
}

RetCode pers_conn_sqlite::do_delete(unsigned int key,
                                    const nentity_manager &nem,
                                    unsigned int ts0,
                                    unsigned int ts1,
                                    PersistenceDeletionMode mode,
                                    const nclass &in)
{
    static pthread_rwlock_t del_stmt_m_l = PTHREAD_RWLOCK_INITIALIZER;
    static std::unordered_map<std::string, std::string> del_stmt_m;
    RetCode rcode = RetCode_OK;
    const char *del_stmt = nullptr;
    std::stringstream ss;
    ss << in.get_id() << '_' << key << '_' << mode;

    {
        scoped_rd_lock rl(del_stmt_m_l);
        auto it = del_stmt_m.find(ss.str());
        if(it != del_stmt_m.end()) {
            del_stmt = it->second.c_str();
        }
    }

    if(!del_stmt) {
        RetCode last_error_code = RetCode_OK;
        std::string last_error_str;
        std::string delete_stmt;
        std::string where_claus;
        where_claus.assign("");
        bool frst_key = true, frst_key_mmbr = true;
        SQLTE_ENM_DELETE_REC_UD rud = { nem,
                                        reinterpret_cast<const char *>(&in),
                                        &where_claus,
                                        &frst_key,
                                        &frst_key_mmbr,
                                        &last_error_code,
                                        &last_error_str
                                      };

        const key_desc *kdsc = in.get_nentity_descriptor().get_key_desc_by_id(key);
        if(!kdsc) {
            IFLOG(err(TH_ID, LS_CLO "[key:%d not found]", __func__, key))
            return RetCode_BADARG;
        }

        kdsc->enum_member_descriptors(enum_keyset_delete_table, &rud);

        if(mode == PersistenceDeletionMode_PHYSICAL) {
            delete_stmt.assign("DELETE FROM ");
            delete_stmt.append(in.get_nentity_descriptor().get_nentity_name());
            delete_stmt.append(" WHERE ");
            delete_stmt.append(where_claus);
            delete_stmt.append(";");
        } else {
            delete_stmt.assign("UPDATE ");
            delete_stmt.append(in.get_nentity_descriptor().get_nentity_name());
            delete_stmt.append(" SET DEL=1 WHERE ");
            delete_stmt.append(where_claus);
            delete_stmt.append(";");
        }

        {
            scoped_wr_lock wl(del_stmt_m_l);
            auto nit = del_stmt_m.insert(std::pair<std::string, std::string>(ss.str(), delete_stmt));
            del_stmt = nit.first->second.c_str();
        }
    }

    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_DELETE));
    task->in_nem_ = &nem;
    task->in_key_ = key;
    task->in_out_ts0_ = &ts0;
    task->in_out_ts1_ = &ts1;
    task->in_mode_ = mode;
    task->in_obj_ = &in;
    task->stmt_bf_ = del_stmt;
    IFLOG(dbg(TH_ID, LS_STM "[delete_stmt:%s]", __func__, del_stmt))
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    return task->op_res_;
}

//--------------------- INSERT -------------------------------------------------

struct SQLTE_ENM_INSERT_REC_UD {
    const nentity_manager &nem;
    const char *obj_ptr;
    std::string *insert_stmt;
    std::string *prfx;
    std::string *values;

    bool *first_fld;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array

    RetCode *last_error_code;
    std::string *last_error_msg;
};

bool enum_mmbrs_insert(const member_desc &mmbrd, void *usr_data)
{
    SQLTE_ENM_INSERT_REC_UD *rud = static_cast<SQLTE_ENM_INSERT_REC_UD *>(usr_data);
    std::string idx_prfx;
    char idx_b[SQLITE_FIDX_BUFF] = {0};
    idx_prfx.assign(*(rud->prfx));
    if(rud->array_fld) {
        sprintf(idx_b, "%s%u", idx_prfx.length() ? "_" : "", rud->fld_idx);
        idx_prfx.append(idx_b);
    }
    if(mmbrd.get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd.get_field_nentity_type() == NEntityType_NENUM) {
            //treat enum as number
            //coma handling
            if(*(rud->first_fld)) {
                *(rud->first_fld) = false;
            } else {
                rud->insert_stmt->append(", ");
                rud->values->append(",");
            }
            if(mmbrd.get_field_nmemb() > 1) {
                bool frst_ar_idx = true;
                for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                    if(frst_ar_idx) {
                        frst_ar_idx = false;
                    } else {
                        rud->insert_stmt->append(", ");
                        rud->values->append(",");
                    }
                    if(rud->prfx->length()) {
                        rud->insert_stmt->append(idx_prfx);
                        rud->insert_stmt->append("_");
                    }
                    sprintf(idx_b, "_%u", i);
                    rud->insert_stmt->append(mmbrd.get_member_name());
                    rud->insert_stmt->append(idx_b);
                    rud->values->append("?");
                }
            } else {
                if(rud->prfx->length()) {
                    rud->insert_stmt->append(idx_prfx);
                    rud->insert_stmt->append("_");
                }
                rud->insert_stmt->append(mmbrd.get_member_name());
                rud->values->append("?");
            }
        } else {
            //class, struct is a recursive step.
            SQLTE_ENM_INSERT_REC_UD rrud = *rud;
            std::string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd.get_member_name());
            rrud.prfx = &rprfx;
            const nentity_desc *edsc = rud->nem.get_nentity_descriptor(mmbrd.get_field_user_type());
            if(edsc) {
                if(mmbrd.get_field_nmemb() > 1) {
                    rrud.array_fld = true;
                    for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                        rrud.fld_idx = i;
                        rrud.obj_ptr = rud->obj_ptr + mmbrd.get_field_offset() + mmbrd.get_field_type_size()*i;
                        edsc->enum_member_descriptors(enum_mmbrs_insert, &rrud);
                    }
                } else {
                    rrud.obj_ptr = rud->obj_ptr + mmbrd.get_field_offset();
                    edsc->enum_member_descriptors(enum_mmbrs_insert, &rrud);
                }
            }
        }
    } else {
        //primitive type
        //coma handling
        if(*(rud->first_fld)) {
            *(rud->first_fld) = false;
        } else {
            rud->insert_stmt->append(", ");
            rud->values->append(",");
        }
        if(mmbrd.get_field_vlg_type() == Type_ASCII || mmbrd.get_field_vlg_type() == Type_BYTE) {
            if(rud->prfx->length()) {
                rud->insert_stmt->append(idx_prfx);
                rud->insert_stmt->append("_");
            }
            rud->insert_stmt->append(mmbrd.get_member_name());
            rud->values->append("?");
        } else if(mmbrd.get_field_nmemb() > 1) {
            bool frst_ar_idx = true;
            for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                if(frst_ar_idx) {
                    frst_ar_idx = false;
                } else {
                    rud->insert_stmt->append(", ");
                    rud->values->append(", ");
                }
                if(rud->prfx->length()) {
                    rud->insert_stmt->append(idx_prfx);
                    rud->insert_stmt->append("_");
                }
                sprintf(idx_b, "_%u", i);
                rud->insert_stmt->append(mmbrd.get_member_name());
                rud->insert_stmt->append(idx_b);
                rud->values->append("?");
            }
        } else {
            if(rud->prfx->length()) {
                rud->insert_stmt->append(idx_prfx);
                rud->insert_stmt->append("_");
            }
            rud->insert_stmt->append(mmbrd.get_member_name());
            rud->values->append("?");
        }
    }
    return true;
}

RetCode pers_conn_sqlite::do_insert(const nentity_manager &nem,
                                    unsigned int ts0,
                                    unsigned int ts1,
                                    const nclass &in,
                                    bool fail_is_error)
{
    static pthread_rwlock_t ins_stmt_m_l = PTHREAD_RWLOCK_INITIALIZER;
    static std::unordered_map<std::string, std::string> ins_stmt_m;
    RetCode rcode = RetCode_OK;
    const char *ins_stmt = nullptr;
    std::stringstream ss;
    ss << in.get_id();

    {
        scoped_rd_lock rl(ins_stmt_m_l);
        auto it = ins_stmt_m.find(ss.str());
        if(it != ins_stmt_m.end()) {
            ins_stmt = it->second.c_str();
        }
    }

    if(!ins_stmt) {
        RetCode last_error_code = RetCode_OK;
        std::string last_error_str;
        std::string insert_stmt("INSERT INTO ");
        std::string values("?,?,0,");
        insert_stmt.append(in.get_nentity_descriptor().get_nentity_name());
        insert_stmt.append("(" P_F_TS0", " P_F_TS1", " P_F_DEL", ");
        std::string prfx;
        bool frst_fld = true;
        SQLTE_ENM_INSERT_REC_UD rud = {nem,
                                       reinterpret_cast<const char *>(&in),
                                       &insert_stmt,
                                       &prfx,
                                       &values,
                                       &frst_fld,
                                       false,
                                       0,
                                       &last_error_code,
                                       &last_error_str
                                      };

        in.get_nentity_descriptor().enum_member_descriptors(enum_mmbrs_insert, &rud);
        insert_stmt.append(") VALUES (");
        insert_stmt.append(values);
        insert_stmt.append(");");

        {
            scoped_wr_lock wl(ins_stmt_m_l);
            auto nit = ins_stmt_m.insert(std::pair<std::string, std::string>(ss.str(), insert_stmt));
            ins_stmt = nit.first->second.c_str();
        }
    }

    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_INSERT));
    task->in_nem_ = &nem;
    task->in_obj_ = &in;
    task->in_out_ts0_ = &ts0;
    task->in_out_ts1_ = &ts1;
    task->in_fail_is_error_ = fail_is_error;
    task->stmt_bf_ = ins_stmt;
    IFLOG(dbg(TH_ID, LS_STM "[insert_stmt:%s]", __func__, ins_stmt))
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    return task->op_res_;
}

//--------------------- QUERY -------------------------------------------------

RetCode pers_conn_sqlite::do_execute_query(const nentity_manager &nem,
                                           const char *sql,
                                           std::unique_ptr<persistence_query_impl> &qry_out)
{
    RetCode rcode = RetCode_OK;
    IFLOG(trc(TH_ID, LS_QRY "[query-sql:%s]", __func__, sql))
    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_EXECUTEQUERY));
    task->in_nem_ = &nem;
    task->in_sql_ = sql;
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    qry_out.reset(task->in_out_query_);
    return task->op_res_;
}

RetCode pers_conn_sqlite::do_release_query(persistence_query_impl &qry)
{
    RetCode rcode = RetCode_OK;
    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_RELEASEQUERY));
    task->in_out_query_ = &qry;
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    return task->op_res_;
}

RetCode pers_conn_sqlite::do_next_entity_from_query(persistence_query_impl &qry,
                                                    unsigned int &ts0_out,
                                                    unsigned int &ts1_out,
                                                    nclass &out)
{
    pers_query_sqlite *qry_sqlite = static_cast<pers_query_sqlite *>(&qry);
    RetCode rcode = RetCode_OK;
    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_NEXTENTITYFROMQUERY));
    task->in_out_ts0_ = &ts0_out;
    task->in_out_ts1_ = &ts1_out;
    task->in_out_obj_ = &out;
    task->sel_stmt_ = qry_sqlite->stmt_;
    task->in_nem_ = &qry_sqlite->nem_;
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    return task->op_res_;;
}

//--------------------- EXEC STMT ----------------------------------------------

RetCode pers_conn_sqlite::do_execute_statement(const char *sql)
{
    RetCode rcode = RetCode_OK;
    IFLOG(trc(TH_ID, LS_STM "[sql:%s]", __func__, sql))
    std::unique_ptr<persistence_task_sqlite> task(new persistence_task_sqlite(*this, VLG_PERS_TASK_OP_EXECUTESTATEMENT));
    task->in_sql_= sql;
    if((rcode = worker_->submit(*task))) {
        IFLOG(cri(TH_ID, LS_CLO "[submit failed][res:%d]", __func__, rcode))
        return rcode;
    } else {
        task->await_for_status(PTASK_STATUS_EXECUTED);
    }
    return task->op_res_;
}

// VLG_PERS_DRIV_SQLITE - DRIVER

// pers_driv_sqlite

struct pers_driv_sqlite : public persistence_driver {
    static pers_driv_sqlite &get_instance();
    explicit pers_driv_sqlite();

    virtual RetCode new_connection(persistence_connection_pool &conn_pool,
                                   persistence_connection_impl **new_conn) override;

    virtual RetCode close_connection(persistence_connection_impl &conn) override;
    virtual const char *get_driver_name() override;
};

std::unique_ptr<pers_driv_sqlite> drv_sqlite_instance;

pers_driv_sqlite &pers_driv_sqlite::get_instance()
{
    if(!drv_sqlite_instance) {
        drv_sqlite_instance.reset(new pers_driv_sqlite());
    }
    return *drv_sqlite_instance;
}

pers_driv_sqlite::pers_driv_sqlite() : persistence_driver(VLG_PERS_DRIV_SQLITE_ID)
{}

RetCode pers_driv_sqlite::new_connection(persistence_connection_pool &conn_pool,
                                         persistence_connection_impl **new_conn)
{
    IFLOG(trc(TH_ID, LS_OPN "[url:%s, user:%s, password:%s]", __func__,
              conn_pool.url_.c_str(),
              conn_pool.usr_.c_str(),
              conn_pool.psswd_.c_str()))
    RetCode rcode = RetCode_OK;
    pers_conn_sqlite *new_conn_instance = new pers_conn_sqlite(conn_pool);
    *new_conn = new_conn_instance;
    IFLOG(trc(TH_ID, LS_CLO "[new_conn_instance:%p, id:%d]", __func__, new_conn_instance, new_conn_instance->id_))
    return rcode;
}

RetCode pers_driv_sqlite::close_connection(persistence_connection_impl &conn)
{
    RetCode res = static_cast<pers_conn_sqlite &>(conn).sqlite_disconnect();
    IFLOG(trc(TH_ID, LS_CLO "[id:%d]", __func__, conn.id_))
    return res;
}

const char *pers_driv_sqlite::get_driver_name()
{
    return VLG_PERS_DRIV_SQLITE_NAME;
}

/********************************
#VER#
*******************************/
extern "C" {
    EXP_SYM const char *get_pers_driv_ver_sqlite()
    {
        return "driv.sqlite.ver.0.0.0.date:" __DATE__;
    }
}

/********************************
VLG_PERS_DRIV_SQLITE ENTRY POINT
*******************************/
extern "C" {
    EXP_SYM persistence_driver *get_pers_driv_sqlite()
    {
        return &pers_driv_sqlite::get_instance();
    }
}

}
