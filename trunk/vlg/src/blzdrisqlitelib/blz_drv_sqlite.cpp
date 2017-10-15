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

#include "blz_drv_sqlite.h"
#include "blz_persistence_int.h"
#include "sqlite3.h"

#if defined(_MSC_VER)
#define snprintf sprintf_s
#endif

namespace blaze {

enum BLZ_SQLITE_DATATYPE {
    BLZ_SQLITE_DATATYPE_Undef,
    BLZ_SQLITE_DATATYPE_NUMERIC,
    BLZ_SQLITE_DATATYPE_INTEGER,
    BLZ_SQLITE_DATATYPE_REAL,
    BLZ_SQLITE_DATATYPE_TEXT,
    BLZ_SQLITE_DATATYPE_BLOB,
};

#define BLZ_SQLITE_DTYPE_NUMERIC    "NUMERIC"
#define BLZ_SQLITE_DTYPE_INTEGER    "INTEGER"
#define BLZ_SQLITE_DTYPE_REAL       "REAL"
#define BLZ_SQLITE_DTYPE_TEXT       "TEXT"
#define BLZ_SQLITE_DTYPE_BLOB       "BLOB"

#define SQLITE_VAL_BUFF 256
#define SQLITE_FIDX_BUFF 16

class pers_conn_sqlite;

//-----------------------------
// entity_desc_impl, partial
//-----------------------------
class entity_desc_impl {
    public:
        const blaze::hash_map &GetMap_NM_MMBRDSC() const;
        const blaze::hash_map &GetMap_KEYID_KDESC() const;
};

class key_desc_impl {
    public:
        const blaze::linked_list &GetKeyFieldSet() const;
};


//-----------------------------
// general utils
//-----------------------------
const char *SQLITE_TypeStr_From_BLZType(Type type)
{
    switch(type) {
        case Type_BOOL:
            return BLZ_SQLITE_DTYPE_NUMERIC;
        case Type_INT16:
            return BLZ_SQLITE_DTYPE_INTEGER;
        case Type_UINT16:
            return BLZ_SQLITE_DTYPE_INTEGER;
        case Type_INT32:
            return BLZ_SQLITE_DTYPE_INTEGER;
        case Type_UINT32:
            return BLZ_SQLITE_DTYPE_INTEGER;
        case Type_INT64:
            return BLZ_SQLITE_DTYPE_INTEGER;
        case Type_UINT64:
            return BLZ_SQLITE_DTYPE_INTEGER;
        case Type_FLOAT32:
            return BLZ_SQLITE_DTYPE_REAL;
        case Type_FLOAT64:
            return BLZ_SQLITE_DTYPE_REAL;
        case Type_ASCII:
            return BLZ_SQLITE_DTYPE_TEXT;
        default:
            return NULL;
    }
}

int fill_buff_fld_value_1(const char *fld_ptr, Type btype, char *out)
{
    switch(btype) {
        case Type_BOOL:
            return sprintf(out, "%d", *(bool *)fld_ptr);
        case Type_INT16:
            return sprintf(out, "%d", *(short *)fld_ptr);
        case Type_UINT16:
            return sprintf(out, "%u", *(unsigned short *)fld_ptr);
        case Type_INT32:
            return sprintf(out, "%d", *(int *)fld_ptr);
        case Type_UINT32:
            return sprintf(out, "%u", *(unsigned int *)fld_ptr);
        case Type_INT64:
#if defined(__GNUG__) && defined(__linux)
            return sprintf(out, "%ld", *(int64_t *)fld_ptr);
#else
            return sprintf(out, "%lld", *(int64_t *)fld_ptr);
#endif
        case Type_UINT64:
#if defined(__GNUG__) && defined(__linux)
            return sprintf(out, "%lu", *(uint64_t *)fld_ptr);
#else
            return sprintf(out, "%llu", *(uint64_t *)fld_ptr);
#endif
        case Type_FLOAT32:
            return sprintf(out, "%f", *(float *)fld_ptr);
        case Type_FLOAT64:
            return sprintf(out, "%f", *(double *)fld_ptr);
        case Type_ASCII:
            return sprintf(out, "'%c'", *fld_ptr);
        default:
            return -1;
    }
}

static int entity_fill_fld(const member_desc *mmbrd,
                           int colmn_idx,
                           sqlite3_stmt *stmt,
                           char *obj_f_ptr)
{
    const char *cptr;
    switch(mmbrd->get_field_blz_type()) {
        case Type_BOOL:
            *(bool *)obj_f_ptr = sqlite3_column_int(stmt, colmn_idx) ? true : false;
            return 0;
        case Type_INT16:
            *(short *)obj_f_ptr = (short)sqlite3_column_int(stmt, colmn_idx);
            return 0;
        case Type_UINT16:
            *(unsigned short *)obj_f_ptr = (unsigned short)sqlite3_column_int(stmt,
                                                                              colmn_idx);
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
            cptr = (const char *)sqlite3_column_text(stmt, colmn_idx);
            *(char *)obj_f_ptr = *cptr;
            return 0;
        default:
            return -1;
    }
}

//-----------------------------
// SQLTE_ENM_BUFF
//-----------------------------
struct SQLTE_ENM_BUFF {
    SQLTE_ENM_BUFF();
    ~SQLTE_ENM_BUFF();

    //a convenience buffer used to render entity fld values
    void Init();
    char *getValBuff();
    char *getValBuff_RSZ(size_t req_size);
    size_t val_buf_sz_;
    char *val_buf_;
};

SQLTE_ENM_BUFF::SQLTE_ENM_BUFF() :
    val_buf_sz_(0),
    val_buf_(NULL)
{
    Init();
}

SQLTE_ENM_BUFF::~SQLTE_ENM_BUFF()
{
    if(val_buf_) {
        free(val_buf_);
    }
}

void SQLTE_ENM_BUFF::Init()
{
    val_buf_sz_ = SQLITE_VAL_BUFF;
    val_buf_ = (char *)blaze::grow_buff_or_die(0, 0, SQLITE_VAL_BUFF);
}

char *SQLTE_ENM_BUFF::getValBuff()
{
    return val_buf_;
}

char *SQLTE_ENM_BUFF::getValBuff_RSZ(size_t req_size)
{
    if(req_size > val_buf_sz_) {
        val_buf_ = (char *)blaze::grow_buff_or_die(val_buf_, val_buf_sz_,
                                                   (req_size - val_buf_sz_));
        val_buf_sz_ = req_size;
    }
    return val_buf_;
}

//-----------------------------
// SQLTE_ENM_SELECT_REC_UD
//-----------------------------
struct SQLTE_ENM_SELECT_REC_UD {
    const entity_manager &bem;
    char *obj_ptr;
    blaze::ascii_string *prfx;
    blaze::ascii_string *columns;
    blaze::ascii_string *where_claus;

    bool *first_fld;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array

    bool *first_key;
    bool *first_key_mmbr;

    blaze::RetCode *last_error_code;
    blaze::ascii_string *last_error_msg;

    //used in enum_mmbrs_fill_entity
    int *colmn_idx;
    sqlite3_stmt *stmt;
    int *sqlite_rc;

    SQLTE_ENM_BUFF *enm_buff;
};

// enum_mmbrs_fill_entity
void enum_mmbrs_fill_entity(const blaze::hash_map &map, const void *key,
                            const void *ptr, void *ud)
{
    SQLTE_ENM_SELECT_REC_UD *rud = static_cast<SQLTE_ENM_SELECT_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    char *obj_f_ptr = NULL;
    if(mmbrd->get_field_blz_type() == Type_ENTITY) {
        if(mmbrd->get_field_entity_type() == EntityType_ENUM) {
            //treat enum as number
            if(mmbrd->get_field_nmemb() > 1) {
                for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                    //value
                    obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset() +
                                mmbrd->get_field_type_size()*i;
                    *(int *)obj_f_ptr = sqlite3_column_int(rud->stmt, (*(rud->colmn_idx))++);
                }
            } else {
                //value
                obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
                *(int *)obj_f_ptr = sqlite3_column_int(rud->stmt, (*(rud->colmn_idx))++);
            }
        } else {
            //class, struct is a recursive step.
            SQLTE_ENM_SELECT_REC_UD rrud = *rud;
            const entity_desc *edsc = NULL;
            if(!rud->bem.get_entity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const blaze::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
                if(mmbrd->get_field_nmemb() > 1) {
                    for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                        rrud.obj_ptr = rud->obj_ptr + mmbrd->get_field_offset() +
                                       mmbrd->get_field_type_size()*i;
                        nm_desc.enum_elements(enum_mmbrs_fill_entity, &rrud);
                    }
                } else {
                    rrud.obj_ptr = rud->obj_ptr + mmbrd->get_field_offset();
                    nm_desc.enum_elements(enum_mmbrs_fill_entity, &rrud);
                }
            } else {
                *rud->last_error_code = blaze::RetCode_GENERR;
                rud->last_error_msg->assign("enum_mmbrs_fill_entity: entity not found in bem [");
                rud->last_error_msg->append(mmbrd->get_field_user_type());
                rud->last_error_msg->append("]");
            }
        }
    } else {
        //primitive type
        if(mmbrd->get_field_blz_type() == Type_ASCII) {
            //value
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            strncpy(obj_f_ptr, (const char *)sqlite3_column_text(rud->stmt,
                                                                 (*(rud->colmn_idx))++),
                    mmbrd->get_field_type_size()*mmbrd->get_field_nmemb());
        } else if(mmbrd->get_field_nmemb() > 1) {
            for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                //value
                obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset() +
                            mmbrd->get_field_type_size()*i;
                entity_fill_fld(mmbrd, (*(rud->colmn_idx))++, rud->stmt, obj_f_ptr);
            }
        } else {
            //value
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            entity_fill_fld(mmbrd, (*(rud->colmn_idx))++, rud->stmt, obj_f_ptr);
        }
    }
}

//-----------------------------
// SQLTE_ENM_UPDATE_REC_UD
//-----------------------------
struct SQLTE_ENM_UPDATE_REC_UD {
    const entity_manager &bem;
    const char *obj_ptr;
    blaze::ascii_string *prfx;
    blaze::ascii_string *set_section;
    blaze::ascii_string *where_claus;

    bool *first_fld;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array

    bool *first_key;
    bool *first_key_mmbr;

    blaze::RetCode *last_error_code;
    blaze::ascii_string *last_error_msg;

    SQLTE_ENM_BUFF *enm_buff;
};

// enum_mmbrs_update
void enum_mmbrs_update(const blaze::hash_map &map, const void *key,
                       const void *ptr,
                       void *ud)
{
    SQLTE_ENM_UPDATE_REC_UD *rud = static_cast<SQLTE_ENM_UPDATE_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    blaze::ascii_string idx_prfx;
    char idx_b[SQLITE_FIDX_BUFF] = { 0 };
    const char *obj_f_ptr = NULL;
    idx_prfx.assign(*(rud->prfx));
    if(rud->array_fld) {
        sprintf(idx_b, "%s%u", idx_prfx.length() ? "_" : "", rud->fld_idx);
        idx_prfx.append(idx_b);
    }
    if(mmbrd->get_field_blz_type() == Type_ENTITY) {
        if(mmbrd->get_field_entity_type() == EntityType_ENUM) {
            //treat enum as number
            //coma handling
            if(*(rud->first_fld)) {
                *(rud->first_fld) = false;
            } else {
                rud->set_section->append(", ");
            }
            if(mmbrd->get_field_nmemb() > 1) {
                bool frst_ar_idx = true;
                for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
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
                    rud->set_section->append(mmbrd->get_member_name());
                    rud->set_section->append(idx_b);
                    //value
                    obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset() +
                                mmbrd->get_field_type_size()*i;
                    sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
                    rud->set_section->append("=");
                    rud->set_section->append(rud->enm_buff->getValBuff());
                }
            } else {
                if(rud->prfx->length()) {
                    rud->set_section->append(idx_prfx);
                    rud->set_section->append("_");
                }
                rud->set_section->append(mmbrd->get_member_name());
                //value
                obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
                sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
                rud->set_section->append("=");
                rud->set_section->append(rud->enm_buff->getValBuff());
            }
        } else {
            //class, struct is a recursive step.
            SQLTE_ENM_UPDATE_REC_UD rrud = *rud;
            blaze::ascii_string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd->get_member_name());
            rrud.prfx = &rprfx;
            const entity_desc *edsc = NULL;
            if(!rud->bem.get_entity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const blaze::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
                if(mmbrd->get_field_nmemb() > 1) {
                    rrud.array_fld = true;
                    for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                        rrud.fld_idx = i;
                        rrud.obj_ptr = rud->obj_ptr + mmbrd->get_field_offset() +
                                       mmbrd->get_field_type_size()*i;
                        nm_desc.enum_elements(enum_mmbrs_update, &rrud);
                    }
                } else {
                    rrud.obj_ptr = rud->obj_ptr + mmbrd->get_field_offset();
                    nm_desc.enum_elements(enum_mmbrs_update, &rrud);
                }
            } else {
                *rud->last_error_code = blaze::RetCode_GENERR;
                rud->last_error_msg->assign("enum_mmbrs_insert: entity not found in bem [");
                rud->last_error_msg->append(mmbrd->get_field_user_type());
                rud->last_error_msg->append("]");
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
        if(mmbrd->get_field_blz_type() == Type_ASCII) {
            if(rud->prfx->length()) {
                rud->set_section->append(idx_prfx);
                rud->set_section->append("_");
            }
            rud->set_section->append(mmbrd->get_member_name());
            //value
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            if(mmbrd->get_field_nmemb() > 1) {
                snprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()),
                         mmbrd->get_field_nmemb(), "%s", obj_f_ptr);
            } else {
                sprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()), "%c",
                        *(char *)obj_f_ptr);
            }
            rud->set_section->append("='");
            rud->set_section->append(rud->enm_buff->getValBuff());
            rud->set_section->append("'");
        } else if(mmbrd->get_field_nmemb() > 1) {
            bool frst_ar_idx = true;
            for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
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
                rud->set_section->append(mmbrd->get_member_name());
                rud->set_section->append(idx_b);
                //value
                obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset() +
                            mmbrd->get_field_type_size()*i;
                fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_blz_type(),
                                      rud->enm_buff->getValBuff());
                rud->set_section->append("=");
                rud->set_section->append(rud->enm_buff->getValBuff());
            }
        } else {
            if(rud->prfx->length()) {
                rud->set_section->append(idx_prfx);
                rud->set_section->append("_");
            }
            rud->set_section->append(mmbrd->get_member_name());
            //value
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_blz_type(),
                                  rud->enm_buff->getValBuff());
            rud->set_section->append("=");
            rud->set_section->append(rud->enm_buff->getValBuff());
        }
    }
}

//-----------------------------
// SQLTE_ENM_DELETE_REC_UD
//-----------------------------
struct SQLTE_ENM_DELETE_REC_UD {
    const entity_manager &bem;
    const char *obj_ptr;
    blaze::ascii_string *where_claus;
    bool *first_key;
    bool *first_key_mmbr;
    blaze::RetCode *last_error_code;
    blaze::ascii_string *last_error_msg;
    SQLTE_ENM_BUFF *enm_buff;
};

//-----------------------------
// SQLTE_ENM_INSERT_REC_UD
//-----------------------------
struct SQLTE_ENM_INSERT_REC_UD {
    const entity_manager &bem;
    const char *obj_ptr;
    blaze::ascii_string *insert_stmt;
    blaze::ascii_string *prfx;
    blaze::ascii_string *values;

    bool *first_fld;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array

    blaze::RetCode *last_error_code;
    blaze::ascii_string *last_error_msg;

    SQLTE_ENM_BUFF *enm_buff;
};

//-----------------------------
// BLZ_PERS_QUERY_SQLITE
//-----------------------------
class pers_query_sqlite : public persistence_query_int {
    public:
        //---ctors
        pers_query_sqlite(unsigned int id,
                          persistence_connection_int &conn,
                          const entity_manager &bem,
                          sqlite3_stmt *stmt);

        ~pers_query_sqlite();
        sqlite3_stmt *get_sqlite_stmt();


    protected:
        sqlite3_stmt *stmt_;
};

pers_query_sqlite::pers_query_sqlite(unsigned int id,
                                     persistence_connection_int &conn,
                                     const entity_manager &bem,
                                     sqlite3_stmt *stmt) :
    persistence_query_int(id, conn, bem),
    stmt_(stmt)
{}

pers_query_sqlite::~pers_query_sqlite()
{}

sqlite3_stmt *pers_query_sqlite::get_sqlite_stmt()
{
    return stmt_;
}

//-----------------------------
// BLZ_PERS_CONN_SQLITE - CONNECTION
//-----------------------------
class pers_conn_sqlite : public persistence_connection_int {
    public:
        pers_conn_sqlite(unsigned int id,
                         persistence_connection_pool &conn_pool);

        blaze::RetCode sqlite_connect(const char *filename,
                                      int flags);

        blaze::RetCode sqlite_disconnect();

        blaze::RetCode sqlite_exec_stmt(const char *stmt,
                                        bool fail_is_error = true);

        blaze::RetCode sqlite_prepare_stmt(const char *sql_stmt,
                                           sqlite3_stmt **stmt);

        blaze::RetCode sqlite_step_stmt(sqlite3_stmt *stmt,
                                        int &sqlite_rc);

        blaze::RetCode sqlite_release_stmt(sqlite3_stmt *stmt);

        int  sqlite_last_err_code();

        virtual blaze::RetCode do_connect();

        virtual blaze::RetCode do_create_table(const entity_manager &bem,
                                               const entity_desc &edesc,
                                               bool drop_if_exist);

        virtual blaze::RetCode do_select(unsigned int key,
                                         const entity_manager &bem,
                                         unsigned int &ts0_out,
                                         unsigned int &ts1_out,
                                         nclass &in_out_obj);

        virtual blaze::RetCode do_update(unsigned int key,
                                         const entity_manager &bem,
                                         unsigned int ts0,
                                         unsigned int ts1,
                                         const nclass &in_obj);

        virtual blaze::RetCode do_delete(unsigned int key,
                                         const entity_manager &bem,
                                         unsigned int ts0,
                                         unsigned int ts1,
                                         PersistenceDeletionMode mode,
                                         const nclass &in_obj);

        virtual blaze::RetCode do_insert(const entity_manager &bem,
                                         unsigned int ts0,
                                         unsigned int ts1,
                                         const nclass &in_obj,
                                         bool fail_is_error = true);

        virtual blaze::RetCode do_execute_query(const entity_manager &bem,
                                                const char *sql,
                                                persistence_query_int **qry_out);

        virtual blaze::RetCode do_release_query(persistence_query_int *qry);

        virtual blaze::RetCode do_next_entity_from_query(persistence_query_int *qry,
                                                         unsigned int &ts0_out,
                                                         unsigned int &ts1_out,
                                                         nclass &out_obj);

        virtual blaze::RetCode do_execute_statement(const char *sql);


    private:
        static blaze::RetCode read_timestamp_and_del_from_record(sqlite3_stmt  *stmt,
                                                                 int           &sqlite_rc,
                                                                 unsigned int  *ts0,
                                                                 unsigned int  *ts1,
                                                                 bool          *del);


    protected:
        sqlite3             *db_;
        persistence_worker  *worker_;

    private:
        //-----------------------------
        // persistence_task_sqlite
        //-----------------------------
        class persistence_task_sqlite : public persistence_task {
            public:
                persistence_task_sqlite(pers_conn_sqlite &sql_conn, BLZ_PERS_TASK_OP op_code) :
                    persistence_task(op_code),
                    sql_conn_(sql_conn),
                    sel_rud_(NULL) {
                }
            protected:

                virtual blaze::RetCode do_connect() {
                    persistence_connection_pool &pcp = sql_conn_.get_connection_pool();
                    IFLOG(trc(TH_ID, LS_OPN "%s(url:%s, usr:%s, psswd:%s)", __func__, pcp.url(),
                              pcp.user(), pcp.password()))
                    blaze::RetCode cdrs_res = sql_conn_.sqlite_connect(pcp.url(),
                                                                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
                    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
                    return cdrs_res;
                }

                virtual blaze::RetCode do_create_table() {
                    blaze::RetCode cdrs_res = blaze::RetCode_OK;
                    if((cdrs_res = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff(), false))) {
                        if(in_drop_if_exist_) {
                            blaze::ascii_string drop_stmt;
                            RETURN_IF_NOT_OK(drop_stmt.assign("DROP TABLE "))
                            RETURN_IF_NOT_OK(drop_stmt.append(in_edesc_->get_entity_name()))
                            RETURN_IF_NOT_OK(drop_stmt.append(";"))
                            IFLOG(dbg(TH_ID, LS_STM "%s() - drop_stmt:%s", __func__,
                                      drop_stmt.internal_buff()))
                            if((cdrs_res = sql_conn_.sqlite_exec_stmt(drop_stmt.internal_buff()))) {
                                IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - drop failed.", __func__, cdrs_res))
                                return cdrs_res;
                            }
                            if((cdrs_res = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff()))) {
                                IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - create after drop failed.", __func__,
                                          cdrs_res))
                                return cdrs_res;
                            }
                        } else {
                            IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - create failed.", __func__, cdrs_res))
                            return cdrs_res;
                        }
                    }
                    return cdrs_res;
                }

                virtual blaze::RetCode do_select() {
                    blaze::RetCode cdrs_res = blaze::RetCode_OK;
                    sqlite3_stmt *stmt = NULL;
                    if((cdrs_res = sql_conn_.sqlite_prepare_stmt(stmt_bf_->internal_buff(),
                                                                 &stmt))) {
                        IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - sqlite_prepare_stmt failed.", __func__,
                                  cdrs_res))
                    } else {
                        if(!(cdrs_res = sql_conn_.sqlite_step_stmt(stmt, *sel_rud_->sqlite_rc))) {
                            if(*sel_rud_->sqlite_rc == SQLITE_ROW) {
                                const blaze::hash_map &nm_desc =
                                    in_out_obj_->get_entity_descriptor()->get_opaque()->GetMap_NM_MMBRDSC();
                                read_timestamp_and_del_from_record(stmt, *sel_rud_->sqlite_rc, in_out_ts0_,
                                                                   in_out_ts1_, NULL);
                                sel_rud_->stmt = stmt;
                                nm_desc.enum_elements(enum_mmbrs_fill_entity, sel_rud_);
                                cdrs_res = blaze::RetCode_DBROW;
                            } else if(*sel_rud_->sqlite_rc == SQLITE_DONE) {
                                IFLOG(trc(TH_ID, LS_TRL "%s() - no data.", __func__))
                                cdrs_res = blaze::RetCode_NODATA;
                            } else {
                                IFLOG(err(TH_ID, LS_TRL "%s(rc:%d) - sqlite_step_stmt - unhandled sqlite code.",
                                          __func__, *sel_rud_->sqlite_rc))
                                cdrs_res = blaze::RetCode_DBERR;
                            }
                            blaze::RetCode rels_cdrs_res = blaze::RetCode_OK;
                            if((rels_cdrs_res = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_release_stmt failed.", __func__,
                                          rels_cdrs_res))
                            }
                        } else {
                            IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_step_stmt failed.", __func__,
                                      cdrs_res))
                        }
                    }
                    return cdrs_res;
                }

                virtual blaze::RetCode do_update() {
                    blaze::RetCode cdrs_res = blaze::RetCode_OK;
                    if((cdrs_res = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff()))) {
                        IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_exec_stmt failed.", __func__,
                                  cdrs_res))
                    }
                    return cdrs_res;
                }

                virtual blaze::RetCode do_delete() {
                    blaze::RetCode cdrs_res = blaze::RetCode_OK;
                    if((cdrs_res = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff()))) {
                        IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_exec_stmt failed.", __func__,
                                  cdrs_res))
                    }
                    return cdrs_res;
                }

                virtual blaze::RetCode do_insert() {
                    blaze::RetCode cdrs_res = blaze::RetCode_OK;
                    if((cdrs_res = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff(),
                                                              in_fail_is_error_))) {
                        if(in_fail_is_error_) {
                            IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_exec_stmt failed.", __func__,
                                      cdrs_res))
                        } else {
                            IFLOG(dbg(TH_ID, LS_TRL "%s(res:%d) - sqlite_exec_stmt failed.", __func__,
                                      cdrs_res))
                        }
                    }
                    return cdrs_res;
                }

                virtual blaze::RetCode do_execute_query() {
                    blaze::RetCode cdrs_res = blaze::RetCode_OK;
                    sqlite3_stmt *stmt = NULL;
                    if((cdrs_res = sql_conn_.sqlite_prepare_stmt(in_sql_, &stmt))) {
                        IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - sqlite_prepare_stmt failed.", __func__,
                                  cdrs_res))
                        return cdrs_res;
                    }
                    in_out_query_ = new pers_query_sqlite(0, sql_conn_, *in_bem_,
                                                          stmt);  //@fixme sanity and id..
                    return cdrs_res;
                }

                virtual blaze::RetCode do_release_query() {
                    pers_query_sqlite *qry_sqlite = static_cast<pers_query_sqlite *>(in_out_query_);
                    blaze::RetCode cdrs_res = sql_conn_.sqlite_release_stmt(
                                                  qry_sqlite->get_sqlite_stmt());
                    return cdrs_res;
                }

                virtual blaze::RetCode do_next_entity_from_query() {
                    blaze::RetCode cdrs_res = blaze::RetCode_OK;
                    pers_query_sqlite *qry_sqlite = static_cast<pers_query_sqlite *>(in_out_query_);
                    if(!(cdrs_res = sql_conn_.sqlite_step_stmt(qry_sqlite->get_sqlite_stmt(),
                                                               *sel_rud_->sqlite_rc))) {
                        if(*sel_rud_->sqlite_rc == SQLITE_ROW) {
                            const blaze::hash_map &nm_desc =
                                in_out_obj_->get_entity_descriptor()->get_opaque()->GetMap_NM_MMBRDSC();
                            read_timestamp_and_del_from_record(qry_sqlite->get_sqlite_stmt(),
                                                               *sel_rud_->sqlite_rc, in_out_ts0_, in_out_ts1_, NULL);
                            nm_desc.enum_elements(enum_mmbrs_fill_entity, sel_rud_);
                            cdrs_res = blaze::RetCode_DBROW;
                        } else if(*sel_rud_->sqlite_rc == SQLITE_DONE) {
                            cdrs_res = blaze::RetCode_QRYEND;
                        } else {
                            IFLOG(err(TH_ID, LS_TRL "%s(rc:%d) - sqlite_step_stmt unhandled sqlite code.",
                                      __func__, *sel_rud_->sqlite_rc))
                            cdrs_res = blaze::RetCode_DBERR;
                        }
                    } else {
                        IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_step_stmt failed.", __func__,
                                  cdrs_res))
                    }
                    return cdrs_res;
                }

                virtual blaze::RetCode do_execute_statement() {
                    return sql_conn_.sqlite_exec_stmt(in_sql_);
                }

            private:
                pers_conn_sqlite &sql_conn_;
            public:
                SQLTE_ENM_SELECT_REC_UD *sel_rud_;
        };
};

//-----------------------------

pers_conn_sqlite::pers_conn_sqlite(unsigned int id,
                                   persistence_connection_pool &conn_pool) :
    persistence_connection_int(id, conn_pool),
    db_(NULL),
    worker_(NULL)
{
    IFLOG(trc(TH_ID, LS_CTR "%s(id:%d)", __func__, id))
}

blaze::RetCode pers_conn_sqlite::sqlite_connect(const char *filename, int flags)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d, filename:%s, flags:%d)", __func__, id_,
              filename, flags))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    int last_rc = sqlite3_open_v2(filename, &db_, flags, 0);
    if(last_rc) {
        IFLOG(err(TH_ID, LS_TRL
                  "%s(id:%d, filename:%s) - sqlite3_open_v2(rc:%d) - errdesc[%s] - db error.",
                  __func__, id_,
                  filename, last_rc, sqlite3_errstr(last_rc)))
        status_ = PersistenceConnectionStatus_ERROR;
        cdrs_res = blaze::RetCode_DBERR;
    } else {
        status_ = PersistenceConnectionStatus_CONNECTED;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return cdrs_res;
}

blaze::RetCode pers_conn_sqlite::sqlite_disconnect()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    int last_rc = sqlite3_close_v2(db_);
    if(last_rc) {
        IFLOG(err(TH_ID, LS_CLO
                  "%s(id:%d) - sqlite3_close_v2(rc:%d) - errdesc[%s] - db error.", __func__, id_,
                  last_rc,
                  sqlite3_errstr(last_rc)))
        status_ = PersistenceConnectionStatus_ERROR;
        cdrs_res = blaze::RetCode_DBERR;
    } else {
        status_ = PersistenceConnectionStatus_DISCONNECTED;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return cdrs_res;
}

blaze::RetCode pers_conn_sqlite::sqlite_exec_stmt(const char *stmt,
                                                  bool fail_is_error)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d, stmt:%p)", __func__, id_, stmt))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    char *zErrMsg = 0;
    int last_rc = sqlite3_exec(db_, stmt, 0, 0, &zErrMsg);
    if(last_rc != SQLITE_OK) {
        if(fail_is_error) {
            IFLOG(err(TH_ID, LS_TRL
                      "%s(id:%d) - sqlite3_exec(rc:%d) - errdesc[%s] - db operation fail.", __func__,
                      id_, last_rc,
                      zErrMsg))
        } else {
            IFLOG(dbg(TH_ID, LS_TRL
                      "%s(id:%d) - sqlite3_exec(rc:%d) - errdesc[%s] - db operation fail.", __func__,
                      id_, last_rc,
                      zErrMsg))
        }
        sqlite3_free(zErrMsg);
        cdrs_res = blaze::RetCode_DBOPFAIL;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return cdrs_res;
}

blaze::RetCode pers_conn_sqlite::sqlite_prepare_stmt(const char *sql_stmt,
                                                     sqlite3_stmt **stmt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sql_stmt:p, stmt:%p)", __func__, sql_stmt, stmt))
    if(!stmt) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADARG;
    }
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    int last_rc = sqlite3_prepare_v2(db_, sql_stmt, (int)strlen(sql_stmt), stmt, 0);
    if(last_rc != SQLITE_OK) {
        IFLOG(err(TH_ID, LS_TRL
                  "%s(id:%d) - sqlite3_prepare_v2(rc:%d) - errdesc[%s] - db error.", __func__,
                  id_, last_rc,
                  sqlite3_errstr(last_rc)))
        cdrs_res = blaze::RetCode_DBERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return cdrs_res;
}

blaze::RetCode pers_conn_sqlite::sqlite_step_stmt(sqlite3_stmt *stmt,
                                                  int &sqlite_rc)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(stmt:%p)", __func__, stmt))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(!stmt) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADARG;
    }
    sqlite_rc = sqlite3_step(stmt);
    if(sqlite_rc != SQLITE_ROW && sqlite_rc != SQLITE_DONE) {
        IFLOG(inf(TH_ID, LS_TRL
                  "%s(id:%d) - sqlite3_step(rc:%d) - errdesc[%s] - db operation fail.", __func__,
                  id_, sqlite_rc,
                  sqlite3_errstr(sqlite_rc)))
        cdrs_res = blaze::RetCode_DBERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d, rc:%d)", __func__, id_, sqlite_rc))
    return cdrs_res;
}

blaze::RetCode pers_conn_sqlite::sqlite_release_stmt(sqlite3_stmt *stmt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(!stmt) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADARG;
    }
    int last_rc = sqlite3_finalize(stmt);
    if(last_rc != SQLITE_OK) {
        IFLOG(err(TH_ID, LS_TRL "%s(id:%d) - sqlite3_finalize(rc:%d) - db error.",
                  __func__, id_, last_rc))
        cdrs_res = blaze::RetCode_DBERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return cdrs_res;
}

blaze::RetCode pers_conn_sqlite::read_timestamp_and_del_from_record(
    sqlite3_stmt  *stmt,
    int           &sqlite_rc,
    unsigned int  *ts0,
    unsigned int  *ts1,
    bool          *del)
{
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    //TS0
    *ts0 = (unsigned int)sqlite3_column_int(stmt, 0);
    //TS1
    *ts1 = (unsigned int)sqlite3_column_int(stmt, 1);
    //DEL
    if(del) {
        *del = sqlite3_column_int(stmt, 2) ? true : false;
    }
    return cdrs_res;
}

//--------------------- CONNECT -------------------------------------------------

blaze::RetCode pers_conn_sqlite::do_connect()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    worker_ = conn_pool_.get_worker_rr_can_create_start();
    if(worker_ == NULL) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - unavailable thread.", __func__,
                  blaze::RetCode_UNVRSC))
        return blaze::RetCode_UNVRSC;
    }
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_CONNECT);
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

//--------------------- CREATE -------------------------------------------------

struct SQLTE_ENM_CREATE_REC_UD {
    const entity_manager &bem;
    blaze::ascii_string *create_stmt;
    blaze::ascii_string *prfx;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array
    bool *first_key;
    bool *first_key_mmbr;
    blaze::RetCode *last_error_code;
    blaze::ascii_string *last_error_msg;
};

void enum_mmbrs_create_table(const blaze::hash_map &map, const void *key,
                             const void *ptr, void *ud)
{
    SQLTE_ENM_CREATE_REC_UD *rud = static_cast<SQLTE_ENM_CREATE_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    blaze::ascii_string idx_prfx;
    char idx_b[SQLITE_FIDX_BUFF] = {0};
    idx_prfx.assign(*(rud->prfx));
    if(rud->array_fld) {
        sprintf(idx_b, "%s%u", idx_prfx.length() ? "_" : "", rud->fld_idx);
        idx_prfx.append(idx_b);
    }
    if(mmbrd->get_field_blz_type() == Type_ENTITY) {
        if(mmbrd->get_field_entity_type() == EntityType_ENUM) {
            //treat enum as number
            if(mmbrd->get_field_nmemb() > 1) {
                for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                    if(rud->prfx->length()) {
                        rud->create_stmt->append(idx_prfx);
                        rud->create_stmt->append("_");
                    }
                    sprintf(idx_b, "_%u", i);
                    rud->create_stmt->append(mmbrd->get_member_name());
                    rud->create_stmt->append(idx_b);
                    rud->create_stmt->append(" " BLZ_SQLITE_DTYPE_NUMERIC", ");
                }
            } else {
                if(rud->prfx->length()) {
                    rud->create_stmt->append(idx_prfx);
                    rud->create_stmt->append("_");
                }
                rud->create_stmt->append(mmbrd->get_member_name());
                rud->create_stmt->append(" " BLZ_SQLITE_DTYPE_NUMERIC", ");
            }
        } else {
            //class, struct is a recursive step.
            SQLTE_ENM_CREATE_REC_UD rrud = *rud;
            blaze::ascii_string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd->get_member_name());
            rrud.prfx = &rprfx;
            const entity_desc *edsc = NULL;
            if(!rud->bem.get_entity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const blaze::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
                if(mmbrd->get_field_nmemb() > 1) {
                    rrud.array_fld = true;
                    for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                        rrud.fld_idx = i;
                        nm_desc.enum_elements(enum_mmbrs_create_table, &rrud);
                    }
                } else {
                    nm_desc.enum_elements(enum_mmbrs_create_table, &rrud);
                }
            } else {
                *rud->last_error_code = blaze::RetCode_GENERR;
                rud->last_error_msg->assign("enum_mmbrs_create_table: entity not found in bem [");
                rud->last_error_msg->append(mmbrd->get_field_user_type());
                rud->last_error_msg->append("]");
            }
        }
    } else {
        //primitive type
        if(mmbrd->get_field_blz_type() == Type_ASCII) {
            if(rud->prfx->length()) {
                rud->create_stmt->append(idx_prfx);
                rud->create_stmt->append("_");
            }
            rud->create_stmt->append(mmbrd->get_member_name());
            rud->create_stmt->append(" " BLZ_SQLITE_DTYPE_TEXT", ");
        } else if(mmbrd->get_field_nmemb() > 1) {
            for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                if(rud->prfx->length()) {
                    rud->create_stmt->append(idx_prfx);
                    rud->create_stmt->append("_");
                }
                sprintf(idx_b, "_%u", i);
                rud->create_stmt->append(mmbrd->get_member_name());
                rud->create_stmt->append(idx_b);
                rud->create_stmt->append(" ");
                rud->create_stmt->append(SQLITE_TypeStr_From_BLZType(
                                             mmbrd->get_field_blz_type()));
                rud->create_stmt->append(", ");
            }
        } else {
            if(rud->prfx->length()) {
                rud->create_stmt->append(idx_prfx);
                rud->create_stmt->append("_");
            }
            rud->create_stmt->append(mmbrd->get_member_name());
            rud->create_stmt->append(" ");
            rud->create_stmt->append(SQLITE_TypeStr_From_BLZType(
                                         mmbrd->get_field_blz_type()));
            rud->create_stmt->append(", ");
        }
    }
}

void enum_keyset_create(const blaze::linked_list &list, const void *ptr,
                        void *ud)
{
    SQLTE_ENM_CREATE_REC_UD *rud = static_cast<SQLTE_ENM_CREATE_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    //coma handling
    if(*(rud->first_key_mmbr)) {
        *(rud->first_key_mmbr) = false;
    } else {
        rud->create_stmt->append(", ");
    }
    rud->create_stmt->append(mmbrd->get_member_name());
}

void enum_keys_create_table(const blaze::hash_map &map, const void *key,
                            const void *ptr, void *ud)
{
    SQLTE_ENM_CREATE_REC_UD *rud = static_cast<SQLTE_ENM_CREATE_REC_UD *>(ud);
    const key_desc *kdsc = *(const key_desc **)ptr;
    const blaze::linked_list &kset = kdsc->get_opaque()->GetKeyFieldSet();
    //coma handling
    if(*(rud->first_key)) {
        *(rud->first_key) = false;
    } else {
        //suppressed because we can have only 1 key
        //rud->stmt.Apnd(",\n");
    }
    if(kdsc->is_primary()) {
        rud->create_stmt->append("PRIMARY KEY(");
    } else {
        //only 1 primary key can be defined with sqlite
        return;
        //rud->stmt.Apnd("KEY(");
    }
    *(rud->first_key_mmbr) = true;
    kset.enum_elements(enum_keyset_create, rud);
    rud->create_stmt->append(")");
}

blaze::RetCode pers_conn_sqlite::do_create_table(const entity_manager &bem,
                                                 const entity_desc &edesc,
                                                 bool drop_if_exist)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(drop_if_exist:%d)", __func__, drop_if_exist))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    blaze::RetCode last_error_code = blaze::RetCode_OK;
    blaze::ascii_string last_error_str;
    blaze::ascii_string create_stmt;
    RETURN_IF_NOT_OK(create_stmt.assign("CREATE TABLE "))
    RETURN_IF_NOT_OK(create_stmt.append(edesc.get_entity_name()))
    RETURN_IF_NOT_OK(create_stmt.append("("))
    RETURN_IF_NOT_OK(create_stmt.append(P_F_TS0" " BLZ_SQLITE_DTYPE_NUMERIC", "))
    RETURN_IF_NOT_OK(create_stmt.append(P_F_TS1" " BLZ_SQLITE_DTYPE_NUMERIC", "))
    RETURN_IF_NOT_OK(create_stmt.append(P_F_DEL" " BLZ_SQLITE_DTYPE_NUMERIC", "))
    const blaze::hash_map &nm_desc = edesc.get_opaque()->GetMap_NM_MMBRDSC();
    blaze::ascii_string prfx;
    RETURN_IF_NOT_OK(prfx.assign(""))
    bool frst_key = true, frst_key_mmbr = true;
    SQLTE_ENM_CREATE_REC_UD rud = { bem,
                                    &create_stmt,
                                    &prfx,
                                    false,
                                    0,
                                    &frst_key,
                                    &frst_key_mmbr,
                                    &last_error_code,
                                    &last_error_str
                                  };
    nm_desc.enum_elements(enum_mmbrs_create_table, &rud);
    const blaze::hash_map &idk_desc = edesc.get_opaque()->GetMap_KEYID_KDESC();
    idk_desc.enum_elements(enum_keys_create_table, &rud);
    RETURN_IF_NOT_OK(create_stmt.append(");"))
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_CREATETABLE);
    task->in_edesc(edesc);
    task->in_drop_if_exist(drop_if_exist);
    task->stmt_bf(create_stmt);
    IFLOG(dbg(TH_ID, LS_STM "%s() - create_stmt:%s", __func__,
              create_stmt.internal_buff()))
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return cdrs_res;
}

//--------------------- SELECT -------------------------------------------------

void enum_keyset_select(const blaze::linked_list &list, const void *ptr,
                        void *ud)
{
    SQLTE_ENM_SELECT_REC_UD *rud = static_cast<SQLTE_ENM_SELECT_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    const char *obj_f_ptr = NULL;
    //coma handling
    if(*(rud->first_key)) {
        *(rud->first_key) = false;
    } else {
        rud->where_claus->append(" AND ");
    }
    rud->where_claus->append(mmbrd->get_member_name());
    if(mmbrd->get_field_blz_type() == Type_ENTITY) {
        //treat enum as number
        obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
        sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
        rud->where_claus->append("=");
        rud->where_claus->append(rud->enm_buff->getValBuff());
    } else {
        //can be only a primitive type
        if(mmbrd->get_field_blz_type() == Type_ASCII) {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            sprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()), "%s",
                    obj_f_ptr);
            rud->where_claus->append("='");
            rud->where_claus->append(rud->enm_buff->getValBuff());
            rud->where_claus->append("'");
        } else {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_blz_type(),
                                  rud->enm_buff->getValBuff());
            rud->where_claus->append("=");
            rud->where_claus->append(rud->enm_buff->getValBuff());
        }
    }
}

blaze::RetCode pers_conn_sqlite::do_select(unsigned int key,
                                           const entity_manager &bem,
                                           unsigned int &ts0_out,
                                           unsigned int &ts1_out,
                                           nclass &in_out_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%d)", __func__, key))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    blaze::RetCode last_error_code = blaze::RetCode_OK;
    blaze::ascii_string last_error_str;
    blaze::ascii_string select_stmt;
    blaze::ascii_string columns;
    blaze::ascii_string where_claus;
    int sqlite_rc = 0;
    RETURN_IF_NOT_OK(columns.assign(""))
    RETURN_IF_NOT_OK(where_claus.assign(P_F_DEL"=0 AND "));
    blaze::ascii_string prfx;
    RETURN_IF_NOT_OK(prfx.assign(""))
    bool frst_fld = true, frst_key = true, frst_key_mmbr = true;
    int column_idx = 3; //column idx, [ts0, ts1, del] we start from 3.
    SQLTE_ENM_BUFF *enm_buff = new SQLTE_ENM_BUFF();
    SQLTE_ENM_SELECT_REC_UD rud = {bem,
                                   reinterpret_cast<char *>(&in_out_obj),
                                   &prfx,
                                   &columns,
                                   &where_claus,
                                   &frst_fld,
                                   false,
                                   0,
                                   &frst_key,
                                   &frst_key_mmbr,
                                   &last_error_code,
                                   &last_error_str,
                                   &column_idx,
                                   0,
                                   &sqlite_rc,
                                   enm_buff
                                  };
    const blaze::hash_map &idk_desc =
        in_out_obj.get_entity_descriptor()->get_opaque()->GetMap_KEYID_KDESC();
    const key_desc *kdsc = NULL;
    if(idk_desc.get(&key, &kdsc)) {
        IFLOG(err(TH_ID, LS_CLO "%s() - key not found [key:%d]", __func__, key))
        return blaze::RetCode_BADARG;
    }
    const blaze::linked_list &kset = kdsc->get_opaque()->GetKeyFieldSet();
    kset.enum_elements(enum_keyset_select, &rud);
    /*not necessary because we can use select (*), column order is preserved*/
    //nm_desc.Enum(enum_mmbrs_select, &rud);
    RETURN_IF_NOT_OK(select_stmt.assign("SELECT "))
    //CHK_MTH(select_stmt.Apnd(columns))
    RETURN_IF_NOT_OK(select_stmt.append("*"))
    RETURN_IF_NOT_OK(select_stmt.append(" FROM "))
    RETURN_IF_NOT_OK(select_stmt.append(
                         in_out_obj.get_entity_descriptor()->get_entity_name()))
    RETURN_IF_NOT_OK(select_stmt.append(" WHERE "))
    RETURN_IF_NOT_OK(select_stmt.append(where_claus));
    RETURN_IF_NOT_OK(select_stmt.append(";"));
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_SELECT);
    task->in_bem(bem);
    task->in_key(key);
    task->in_out_ts0(ts0_out);
    task->in_out_ts1(ts1_out);
    task->in_out_obj(in_out_obj);
    task->stmt_bf(select_stmt);
    task->sel_rud_ = &rud;
    IFLOG(dbg(TH_ID, LS_QRY "%s() - select_stmt:%s", __func__,
              select_stmt.internal_buff()))
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

//--------------------- UPDATE -------------------------------------------------

void enum_keyset_update(const blaze::linked_list &list, const void *ptr,
                        void *ud)
{
    SQLTE_ENM_UPDATE_REC_UD *rud = static_cast<SQLTE_ENM_UPDATE_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    const char *obj_f_ptr = NULL;
    //coma handling
    if(*(rud->first_key)) {
        *(rud->first_key) = false;
    } else {
        rud->where_claus->append(" AND ");
    }
    rud->where_claus->append(mmbrd->get_member_name());
    if(mmbrd->get_field_blz_type() == Type_ENTITY) {
        //treat enum as number
        obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
        sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
        rud->where_claus->append("=");
        rud->where_claus->append(rud->enm_buff->getValBuff());
    } else {
        //can be only a primitive type
        if(mmbrd->get_field_blz_type() == Type_ASCII) {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            sprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()), "%s",
                    obj_f_ptr);
            rud->where_claus->append("='");
            rud->where_claus->append(rud->enm_buff->getValBuff());
            rud->where_claus->append("'");
        } else {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_blz_type(),
                                  rud->enm_buff->getValBuff());
            rud->where_claus->append("=");
            rud->where_claus->append(rud->enm_buff->getValBuff());
        }
    }
}

blaze::RetCode pers_conn_sqlite::do_update(unsigned int key,
                                           const entity_manager &bem,
                                           unsigned int ts0,
                                           unsigned int ts1,
                                           const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%d)", __func__, key))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    blaze::RetCode last_error_code = blaze::RetCode_OK;
    blaze::ascii_string last_error_str;
    blaze::ascii_string update_stmt;
    blaze::ascii_string set_section;
    blaze::ascii_string where_claus;
    char ts_buff[TMSTMP_BUFF_SZ];
    RETURN_IF_NOT_OK(set_section.assign(P_F_TS0"="))
    snprintf(ts_buff, TMSTMP_BUFF_SZ, "%u", ts0);
    RETURN_IF_NOT_OK(set_section.append(ts_buff))
    RETURN_IF_NOT_OK(set_section.append(", "))
    RETURN_IF_NOT_OK(set_section.append(P_F_TS1"="))
    snprintf(ts_buff, TMSTMP_BUFF_SZ, "%u", ts1);
    RETURN_IF_NOT_OK(set_section.append(ts_buff))
    RETURN_IF_NOT_OK(set_section.append(", "))
    RETURN_IF_NOT_OK(where_claus.assign(P_F_DEL"=0 AND "));
    const blaze::hash_map &nm_desc =
        in_obj.get_entity_descriptor()->get_opaque()->GetMap_NM_MMBRDSC();
    blaze::ascii_string prfx;
    RETURN_IF_NOT_OK(prfx.assign(""))
    bool frst_fld = true, frst_key = true, frst_key_mmbr = true;
    SQLTE_ENM_BUFF *enm_buff = new SQLTE_ENM_BUFF();
    SQLTE_ENM_UPDATE_REC_UD rud = { bem,
                                    reinterpret_cast<const char *>(&in_obj),
                                    &prfx,
                                    &set_section,
                                    &where_claus,
                                    &frst_fld,
                                    false,
                                    0,
                                    &frst_key,
                                    &frst_key_mmbr,
                                    &last_error_code,
                                    &last_error_str,
                                    enm_buff
                                  };
    const blaze::hash_map &idk_desc =
        in_obj.get_entity_descriptor()->get_opaque()->GetMap_KEYID_KDESC();
    const key_desc *kdsc = NULL;
    if(idk_desc.get(&key, &kdsc)) {
        IFLOG(err(TH_ID, LS_CLO "%s() - key not found [key:%d]", __func__, key))
        return blaze::RetCode_BADARG;
    }
    const blaze::linked_list &kset = kdsc->get_opaque()->GetKeyFieldSet();
    kset.enum_elements(enum_keyset_update, &rud);
    nm_desc.enum_elements(enum_mmbrs_update, &rud);
    RETURN_IF_NOT_OK(update_stmt.assign("UPDATE "))
    RETURN_IF_NOT_OK(update_stmt.append(
                         in_obj.get_entity_descriptor()->get_entity_name()))
    RETURN_IF_NOT_OK(update_stmt.append(" SET "))
    RETURN_IF_NOT_OK(update_stmt.append(set_section))
    RETURN_IF_NOT_OK(update_stmt.append(" WHERE "))
    RETURN_IF_NOT_OK(update_stmt.append(where_claus));
    RETURN_IF_NOT_OK(update_stmt.append(";"));
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_UPDATE);
    task->in_bem(bem);
    task->in_key(key);
    task->in_out_ts0(ts0);
    task->in_out_ts1(ts1);
    task->stmt_bf(update_stmt);
    IFLOG(dbg(TH_ID, LS_STM "%s() - update_stmt:%s", __func__,
              update_stmt.internal_buff()))
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

//--------------------- DELETE -------------------------------------------------

void enum_keyset_delete(const blaze::linked_list &list, const void *ptr,
                        void *ud)
{
    SQLTE_ENM_DELETE_REC_UD *rud = static_cast<SQLTE_ENM_DELETE_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    const char *obj_f_ptr = NULL;
    //coma handling
    if(*(rud->first_key)) {
        *(rud->first_key) = false;
    } else {
        rud->where_claus->append(" AND ");
    }
    rud->where_claus->append(mmbrd->get_member_name());
    if(mmbrd->get_field_blz_type() == Type_ENTITY) {
        //treat enum as number
        obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
        sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
        rud->where_claus->append("=");
        rud->where_claus->append(rud->enm_buff->getValBuff());
    } else {
        //can be only a primitive type
        if(mmbrd->get_field_blz_type() == Type_ASCII) {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            sprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()), "%s",
                    obj_f_ptr);
            rud->where_claus->append("='");
            rud->where_claus->append(rud->enm_buff->getValBuff());
            rud->where_claus->append("'");
        } else {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_blz_type(),
                                  rud->enm_buff->getValBuff());
            rud->where_claus->append("=");
            rud->where_claus->append(rud->enm_buff->getValBuff());
        }
    }
}

blaze::RetCode pers_conn_sqlite::do_delete(unsigned int key,
                                           const entity_manager &bem,
                                           unsigned int ts0,
                                           unsigned int ts1,
                                           PersistenceDeletionMode  mode,
                                           const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%d)", __func__, key))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    blaze::RetCode last_error_code = blaze::RetCode_OK;
    blaze::ascii_string last_error_str;
    blaze::ascii_string delete_stmt;
    blaze::ascii_string where_claus;
    RETURN_IF_NOT_OK(where_claus.assign(""))
    bool frst_key = true, frst_key_mmbr = true;
    SQLTE_ENM_BUFF *enm_buff = new SQLTE_ENM_BUFF();
    SQLTE_ENM_DELETE_REC_UD rud = { bem,
                                    reinterpret_cast<const char *>(&in_obj),
                                    &where_claus,
                                    &frst_key,
                                    &frst_key_mmbr,
                                    &last_error_code,
                                    &last_error_str,
                                    enm_buff
                                  };
    const blaze::hash_map &idk_desc =
        in_obj.get_entity_descriptor()->get_opaque()->GetMap_KEYID_KDESC();
    const key_desc *kdsc = NULL;
    if(idk_desc.get(&key, &kdsc)) {
        IFLOG(err(TH_ID, LS_CLO "%s() - key not found [key:%d]", __func__, key))
        return blaze::RetCode_BADARG;
    }
    const blaze::linked_list &kset = kdsc->get_opaque()->GetKeyFieldSet();
    kset.enum_elements(enum_keyset_delete, &rud);
    if(mode == PersistenceDeletionMode_PHYSICAL) {
        RETURN_IF_NOT_OK(delete_stmt.assign("DELETE FROM "))
        RETURN_IF_NOT_OK(delete_stmt.append(
                             in_obj.get_entity_descriptor()->get_entity_name()))
        RETURN_IF_NOT_OK(delete_stmt.append(" WHERE "))
        RETURN_IF_NOT_OK(delete_stmt.append(where_claus));
        RETURN_IF_NOT_OK(delete_stmt.append(";"));
    } else {
        RETURN_IF_NOT_OK(delete_stmt.assign("UPDATE "))
        RETURN_IF_NOT_OK(delete_stmt.append(
                             in_obj.get_entity_descriptor()->get_entity_name()))
        RETURN_IF_NOT_OK(delete_stmt.append(" SET DEL=1 WHERE "))
        RETURN_IF_NOT_OK(delete_stmt.append(where_claus));
        RETURN_IF_NOT_OK(delete_stmt.append(";"));
    }
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_DELETE);
    task->in_bem(bem);
    task->in_key(key);
    task->in_out_ts0(ts0);
    task->in_out_ts1(ts1);
    task->in_mode(mode);
    task->in_obj(in_obj);
    task->stmt_bf(delete_stmt);
    IFLOG(dbg(TH_ID, LS_STM "%s() - delete_stmt:%s", __func__,
              delete_stmt.internal_buff()))
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

//--------------------- INSERT -------------------------------------------------

void enum_mmbrs_insert(const blaze::hash_map &map, const void *key,
                       const void *ptr,
                       void *ud)
{
    SQLTE_ENM_INSERT_REC_UD *rud = static_cast<SQLTE_ENM_INSERT_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    blaze::ascii_string idx_prfx;
    char idx_b[SQLITE_FIDX_BUFF] = {0};
    const char *obj_f_ptr = NULL;
    idx_prfx.assign(*(rud->prfx));
    if(rud->array_fld) {
        sprintf(idx_b, "%s%u", idx_prfx.length() ? "_" : "", rud->fld_idx);
        idx_prfx.append(idx_b);
    }
    if(mmbrd->get_field_blz_type() == Type_ENTITY) {
        if(mmbrd->get_field_entity_type() == EntityType_ENUM) {
            //treat enum as number
            //coma handling
            if(*(rud->first_fld)) {
                *(rud->first_fld) = false;
            } else {
                rud->insert_stmt->append(", ");
                rud->values->append(", ");
            }
            if(mmbrd->get_field_nmemb() > 1) {
                bool frst_ar_idx = true;
                for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
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
                    rud->insert_stmt->append(mmbrd->get_member_name());
                    rud->insert_stmt->append(idx_b);
                    obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset() +
                                mmbrd->get_field_type_size()*i;
                    sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
                    rud->values->append(rud->enm_buff->getValBuff());
                }
            } else {
                if(rud->prfx->length()) {
                    rud->insert_stmt->append(idx_prfx);
                    rud->insert_stmt->append("_");
                }
                rud->insert_stmt->append(mmbrd->get_member_name());
                //value
                obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
                sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
                rud->values->append(rud->enm_buff->getValBuff());
            }
        } else {
            //class, struct is a recursive step.
            SQLTE_ENM_INSERT_REC_UD rrud = *rud;
            blaze::ascii_string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd->get_member_name());
            rrud.prfx = &rprfx;
            const entity_desc *edsc = NULL;
            if(!rud->bem.get_entity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const blaze::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
                if(mmbrd->get_field_nmemb() > 1) {
                    rrud.array_fld = true;
                    for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                        rrud.fld_idx = i;
                        rrud.obj_ptr = rud->obj_ptr + mmbrd->get_field_offset() +
                                       mmbrd->get_field_type_size()*i;
                        nm_desc.enum_elements(enum_mmbrs_insert, &rrud);
                    }
                } else {
                    rrud.obj_ptr = rud->obj_ptr + mmbrd->get_field_offset();
                    nm_desc.enum_elements(enum_mmbrs_insert, &rrud);
                }
            } else {
                *rud->last_error_code = blaze::RetCode_GENERR;
                rud->last_error_msg->assign("enum_mmbrs_insert: entity not found in bem [");
                rud->last_error_msg->append(mmbrd->get_field_user_type());
                rud->last_error_msg->append("]");
            }
        }
    } else {
        //primitive type
        //coma handling
        if(*(rud->first_fld)) {
            *(rud->first_fld) = false;
        } else {
            rud->insert_stmt->append(", ");
            rud->values->append(", ");
        }
        if(mmbrd->get_field_blz_type() == Type_ASCII) {
            if(rud->prfx->length()) {
                rud->insert_stmt->append(idx_prfx);
                rud->insert_stmt->append("_");
            }
            rud->insert_stmt->append(mmbrd->get_member_name());
            //value
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            if(mmbrd->get_field_nmemb() > 1) {
                snprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()),
                         mmbrd->get_field_nmemb(), "%s", obj_f_ptr);
            } else {
                sprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()), "%c",
                        *(char *)obj_f_ptr);
            }
            rud->values->append("'");
            rud->values->append(rud->enm_buff->getValBuff());
            rud->values->append("'");
        } else if(mmbrd->get_field_nmemb() > 1) {
            bool frst_ar_idx = true;
            for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
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
                rud->insert_stmt->append(mmbrd->get_member_name());
                rud->insert_stmt->append(idx_b);
                obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset() +
                            mmbrd->get_field_type_size()*i;
                fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_blz_type(),
                                      rud->enm_buff->getValBuff());
                rud->values->append(rud->enm_buff->getValBuff());
            }
        } else {
            if(rud->prfx->length()) {
                rud->insert_stmt->append(idx_prfx);
                rud->insert_stmt->append("_");
            }
            rud->insert_stmt->append(mmbrd->get_member_name());
            //value
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_blz_type(),
                                  rud->enm_buff->getValBuff());
            rud->values->append(rud->enm_buff->getValBuff());
        }
    }
}

blaze::RetCode pers_conn_sqlite::do_insert(const entity_manager &bem,
                                           unsigned int ts0,
                                           unsigned int ts1,
                                           const nclass &in_obj,
                                           bool fail_is_error)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    blaze::RetCode last_error_code = blaze::RetCode_OK;
    blaze::ascii_string last_error_str;
    blaze::ascii_string insert_stmt;
    blaze::ascii_string values;
    char ts_buff[TMSTMP_BUFF_SZ];
    snprintf(ts_buff, TMSTMP_BUFF_SZ, "%u", ts0);
    RETURN_IF_NOT_OK(values.assign(ts_buff))
    RETURN_IF_NOT_OK(values.append(", "))
    snprintf(ts_buff, TMSTMP_BUFF_SZ, "%u", ts1);
    RETURN_IF_NOT_OK(values.append(ts_buff))
    RETURN_IF_NOT_OK(values.append(", "))
    RETURN_IF_NOT_OK(values.append("0, ")) //del
    RETURN_IF_NOT_OK(insert_stmt.assign("INSERT INTO "))
    RETURN_IF_NOT_OK(insert_stmt.append(
                         in_obj.get_entity_descriptor()->get_entity_name()))
    RETURN_IF_NOT_OK(insert_stmt.append("(" P_F_TS0", " P_F_TS1", " P_F_DEL", "))
    const blaze::hash_map &nm_desc =
        in_obj.get_entity_descriptor()->get_opaque()->GetMap_NM_MMBRDSC();
    blaze::ascii_string prfx;
    RETURN_IF_NOT_OK(prfx.assign(""))
    bool frst_fld = true;
    SQLTE_ENM_BUFF *enm_buff = new SQLTE_ENM_BUFF();
    SQLTE_ENM_INSERT_REC_UD rud = {bem,
                                   reinterpret_cast<const char *>(&in_obj),
                                   &insert_stmt,
                                   &prfx,
                                   &values,
                                   &frst_fld,
                                   false,
                                   0,
                                   &last_error_code,
                                   &last_error_str,
                                   enm_buff
                                  };
    nm_desc.enum_elements(enum_mmbrs_insert, &rud);
    RETURN_IF_NOT_OK(insert_stmt.append(") VALUES ("));
    RETURN_IF_NOT_OK(insert_stmt.append(values));
    RETURN_IF_NOT_OK(insert_stmt.append(");"));
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_INSERT);
    task->in_fail_is_error(fail_is_error);
    task->stmt_bf(insert_stmt);
    IFLOG(dbg(TH_ID, LS_STM "%s() - insert_stmt:%s", __func__,
              insert_stmt.internal_buff()))
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

//--------------------- QUERY -------------------------------------------------

blaze::RetCode pers_conn_sqlite::do_execute_query(const entity_manager &bem,
                                                  const char *sql, persistence_query_int **qry_out)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sql:%p, qry_out:%p)", __func__, sql, qry_out))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    IFLOG(trc(TH_ID, LS_QRY "%s() - query-sql:%s", __func__, sql))
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_EXECUTEQUERY);
    task->in_bem(bem);
    task->in_sql(sql);
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    *qry_out = task->in_out_query();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d, qry:%p)", __func__, cdrs_res, *qry_out))
    return cdrs_res;
}

blaze::RetCode pers_conn_sqlite::do_release_query(persistence_query_int *qry)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(qry:%p)", __func__, qry))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_RELEASEQUERY);
    task->in_out_query(qry);
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

blaze::RetCode pers_conn_sqlite::do_next_entity_from_query(persistence_query_int
                                                           *qry,
                                                           unsigned int &ts0_out,
                                                           unsigned int &ts1_out,
                                                           nclass &out_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(qry:%p)", __func__, qry))
    pers_query_sqlite *qry_sqlite = static_cast<pers_query_sqlite *>(qry);
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    blaze::RetCode last_error_code = blaze::RetCode_OK;
    blaze::ascii_string last_error_str;
    blaze::ascii_string select_stmt;
    blaze::ascii_string columns;
    blaze::ascii_string where_claus;
    int sqlite_rc = 0;
    bool frst_fld = true, frst_key = true, frst_key_mmbr = true;
    int column_idx = 3; //column idx, [ts0, ts1, del] we start from 3.
    SQLTE_ENM_BUFF *enm_buff = new SQLTE_ENM_BUFF();
    SQLTE_ENM_SELECT_REC_UD rud = { qry->get_em(),
                                    reinterpret_cast<char *>(&out_obj),
                                    NULL,
                                    &columns,
                                    &where_claus,
                                    &frst_fld,
                                    false,
                                    0,
                                    &frst_key,
                                    &frst_key_mmbr,
                                    &last_error_code,
                                    &last_error_str,
                                    &column_idx,
                                    qry_sqlite->get_sqlite_stmt(),
                                    &sqlite_rc,
                                    enm_buff
                                  };
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_NEXTENTITYFROMQUERY);
    task->in_out_query(qry_sqlite);
    task->in_out_ts0(ts0_out);
    task->in_out_ts1(ts1_out);
    task->in_out_obj(out_obj);
    task->sel_rud_ = &rud;
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

//--------------------- EXEC STMT ----------------------------------------------

blaze::RetCode pers_conn_sqlite::do_execute_statement(const char *sql)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sql:%p)", __func__, sql))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    IFLOG(trc(TH_ID, LS_STM "%s() - sql:%s", __func__, sql))
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                BLZ_PERS_TASK_OP_EXECUTESTATEMENT);
    task->in_sql(sql);
    if((cdrs_res = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, cdrs_res))
    } else {
        task->await_for_status(blaze::PTASK_STATUS_EXECUTED);
    }
    cdrs_res = task->op_res();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

//-----------------------------
// BLZ_PERS_DRIV_SQLITE - DRIVER
//-----------------------------
static unsigned int sqlite_connid = 0;
unsigned int GetSQLITE_NEXT_CONNID()
{
    return ++sqlite_connid;
}

//-----------------------------
// pers_driv_sqlite
//-----------------------------
class pers_driv_sqlite : public persistence_driver_int {
    public:
        static pers_driv_sqlite *get_instance();

    private:
        //---ctors
        pers_driv_sqlite();
        ~pers_driv_sqlite();

    public:
        virtual blaze::RetCode new_connection(persistence_connection_pool &conn_pool,
                                              persistence_connection_int **new_conn);

        virtual blaze::RetCode close_connection(persistence_connection_int &conn);

        virtual const char *get_driver_name();
};

pers_driv_sqlite *drv_sqlite_instance = NULL;

pers_driv_sqlite *pers_driv_sqlite::get_instance()
{
    if(!drv_sqlite_instance) {
        IFLOG(trc(TH_ID, LS_TRL "%s() - creating instance.", __func__))
        if((drv_sqlite_instance = new pers_driv_sqlite())) {
            IFLOG(dbg(TH_ID, LS_TRL "%s(drv_sqlite_instance:%p, id:%d) - instance created.",
                      __func__,
                      drv_sqlite_instance,
                      drv_sqlite_instance->get_id()))
            drv_sqlite_instance->init();
        } else {
            IFLOG(cri(TH_ID, LS_TRL "%s() - instance creation failed.", __func__))
            return NULL;
        }
    }
    return drv_sqlite_instance;
}

pers_driv_sqlite::pers_driv_sqlite() :
    persistence_driver_int(BLZ_PERS_DRIV_SQLITE_ID)
{
    IFLOG(trc(TH_ID, LS_CTR "%s(id:%d)", __func__, id_))
}

pers_driv_sqlite::~pers_driv_sqlite()
{
    IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
}

blaze::RetCode pers_driv_sqlite::new_connection(persistence_connection_pool
                                                &conn_pool,
                                                persistence_connection_int **new_conn)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(url:%s, usr:%s, psswd:%s, new_conn:%p)", __func__,
              conn_pool.url(), conn_pool.user(),
              conn_pool.password(), new_conn))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    pers_conn_sqlite *new_conn_instance = new pers_conn_sqlite(
        GetSQLITE_NEXT_CONNID(), conn_pool);
    if(!new_conn_instance) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - new failed.", __func__))
        return blaze::RetCode_MEMERR;
    }
    if(!cdrs_res) {
        *new_conn = new_conn_instance;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d, new_conn_instance:%p, id:%d)", __func__,
              cdrs_res, new_conn_instance,
              new_conn_instance->get_id()))
    return cdrs_res;
}

blaze::RetCode pers_driv_sqlite::close_connection(persistence_connection_int
                                                  &conn)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    blaze::RetCode res = static_cast<pers_conn_sqlite &>(conn).sqlite_disconnect();
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d, id:%d)", __func__, res, conn.get_id()))
    return res;
}

const char *pers_driv_sqlite::get_driver_name()
{
    return BLZ_PERS_DRIV_SQLITE_NAME;
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
BLZ_PERS_DRIV_SQLITE ENTRY POINT
*******************************/
extern "C" {
    EXP_SYM persistence_driver_int *get_pers_driv_sqlite()
    {
        return pers_driv_sqlite::get_instance();
    }
}

}
