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

#include "vlg_drv_sqlite.h"
#include "vlg/vlg_pers_impl.h"
#include "sqlite3.h"

#if defined(_MSC_VER)
#define snprintf sprintf_s
#endif

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

class pers_conn_sqlite;

//-----------------------------
// entity_desc_impl, partial
//-----------------------------
class nentity_desc_impl {
    public:
        const vlg::hash_map &GetMap_NM_MMBRDSC() const;
        const vlg::hash_map &GetMap_KEYID_KDESC() const;
};

class key_desc_impl {
    public:
        const vlg::linked_list &get_key_field_set() const;
};

//-----------------------------
// general utils
//-----------------------------
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
    switch(mmbrd->get_field_vlg_type()) {
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
    val_buf_ = (char *)vlg::grow_buff_or_die(0, 0, SQLITE_VAL_BUFF);
}

char *SQLTE_ENM_BUFF::getValBuff()
{
    return val_buf_;
}

char *SQLTE_ENM_BUFF::getValBuff_RSZ(size_t req_size)
{
    if(req_size > val_buf_sz_) {
        val_buf_ = (char *)vlg::grow_buff_or_die(val_buf_, val_buf_sz_,
                                                 (req_size - val_buf_sz_));
        val_buf_sz_ = req_size;
    }
    return val_buf_;
}

//-----------------------------
// SQLTE_ENM_SELECT_REC_UD
//-----------------------------
struct SQLTE_ENM_SELECT_REC_UD {
    const nentity_manager &nem;
    char *obj_ptr;
    vlg::ascii_string *prfx;
    vlg::ascii_string *columns;
    vlg::ascii_string *where_claus;

    bool *first_fld;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array

    bool *first_key;
    bool *first_key_mmbr;

    vlg::RetCode *last_error_code;
    vlg::ascii_string *last_error_msg;

    //used in enum_mmbrs_fill_entity
    int *colmn_idx;
    sqlite3_stmt *stmt;
    int *sqlite_rc;

    SQLTE_ENM_BUFF *enm_buff;
};

// enum_mmbrs_fill_entity
void enum_mmbrs_fill_entity(const vlg::hash_map &map, const void *key,
                            const void *ptr, void *ud)
{
    SQLTE_ENM_SELECT_REC_UD *rud = static_cast<SQLTE_ENM_SELECT_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    char *obj_f_ptr = NULL;
    if(mmbrd->get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd->get_field_nentity_type() == NEntityType_NENUM) {
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
            const nentity_desc *edsc = NULL;
            if(!rud->nem.get_nentity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const vlg::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
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
                *rud->last_error_code = vlg::RetCode_GENERR;
                rud->last_error_msg->assign("enum_mmbrs_fill_entity: entity not found in nem [");
                rud->last_error_msg->append(mmbrd->get_field_user_type());
                rud->last_error_msg->append("]");
            }
        }
    } else {
        //primitive type
        if(mmbrd->get_field_vlg_type() == Type_ASCII) {
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
    const nentity_manager &nem;
    const char *obj_ptr;
    vlg::ascii_string *prfx;
    vlg::ascii_string *set_section;
    vlg::ascii_string *where_claus;

    bool *first_fld;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array

    bool *first_key;
    bool *first_key_mmbr;

    vlg::RetCode *last_error_code;
    vlg::ascii_string *last_error_msg;

    SQLTE_ENM_BUFF *enm_buff;
};

// enum_mmbrs_update
void enum_mmbrs_update(const vlg::hash_map &map, const void *key,
                       const void *ptr,
                       void *ud)
{
    SQLTE_ENM_UPDATE_REC_UD *rud = static_cast<SQLTE_ENM_UPDATE_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    vlg::ascii_string idx_prfx;
    char idx_b[SQLITE_FIDX_BUFF] = { 0 };
    const char *obj_f_ptr = NULL;
    idx_prfx.assign(*(rud->prfx));
    if(rud->array_fld) {
        sprintf(idx_b, "%s%u", idx_prfx.length() ? "_" : "", rud->fld_idx);
        idx_prfx.append(idx_b);
    }
    if(mmbrd->get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd->get_field_nentity_type() == NEntityType_NENUM) {
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
            vlg::ascii_string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd->get_member_name());
            rrud.prfx = &rprfx;
            const nentity_desc *edsc = NULL;
            if(!rud->nem.get_nentity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const vlg::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
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
                *rud->last_error_code = vlg::RetCode_GENERR;
                rud->last_error_msg->assign("enum_mmbrs_insert: entity not found in nem [");
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
        if(mmbrd->get_field_vlg_type() == Type_ASCII) {
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
                fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_vlg_type(),
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
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_vlg_type(),
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
    const nentity_manager &nem;
    const char *obj_ptr;
    vlg::ascii_string *where_claus;
    bool *first_key;
    bool *first_key_mmbr;
    vlg::RetCode *last_error_code;
    vlg::ascii_string *last_error_msg;
    SQLTE_ENM_BUFF *enm_buff;
};

//-----------------------------
// SQLTE_ENM_INSERT_REC_UD
//-----------------------------
struct SQLTE_ENM_INSERT_REC_UD {
    const nentity_manager &nem;
    const char *obj_ptr;
    vlg::ascii_string *insert_stmt;
    vlg::ascii_string *prfx;
    vlg::ascii_string *values;

    bool *first_fld;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array

    vlg::RetCode *last_error_code;
    vlg::ascii_string *last_error_msg;

    SQLTE_ENM_BUFF *enm_buff;
};

//-----------------------------
// VLG_PERS_QUERY_SQLITE
//-----------------------------
class pers_query_sqlite : public persistence_query_impl {
    public:
        //---ctors
        pers_query_sqlite(unsigned int id,
                          persistence_connection_impl &conn,
                          const nentity_manager &nem,
                          sqlite3_stmt *stmt);

        ~pers_query_sqlite();
        sqlite3_stmt *get_sqlite_stmt();


    protected:
        sqlite3_stmt *stmt_;
};

pers_query_sqlite::pers_query_sqlite(unsigned int id,
                                     persistence_connection_impl &conn,
                                     const nentity_manager &nem,
                                     sqlite3_stmt *stmt) :
    persistence_query_impl(id, conn, nem),
    stmt_(stmt)
{}

pers_query_sqlite::~pers_query_sqlite()
{}

sqlite3_stmt *pers_query_sqlite::get_sqlite_stmt()
{
    return stmt_;
}

//-----------------------------
// VLG_PERS_CONN_SQLITE - CONNECTION
//-----------------------------
class pers_conn_sqlite : public persistence_connection_impl {
    public:
        pers_conn_sqlite(unsigned int id,
                         persistence_connection_pool &conn_pool);

        vlg::RetCode sqlite_connect(const char *filename,
                                    int flags);

        vlg::RetCode sqlite_disconnect();

        vlg::RetCode sqlite_exec_stmt(const char *stmt,
                                      bool fail_is_error = true);

        vlg::RetCode sqlite_prepare_stmt(const char *sql_stmt,
                                         sqlite3_stmt **stmt);

        vlg::RetCode sqlite_step_stmt(sqlite3_stmt *stmt,
                                      int &sqlite_rc);

        vlg::RetCode sqlite_release_stmt(sqlite3_stmt *stmt);

        int  sqlite_last_err_code();

        virtual vlg::RetCode do_connect();

        virtual vlg::RetCode do_create_table(const nentity_manager &nem,
                                             const nentity_desc &edesc,
                                             bool drop_if_exist);

        virtual vlg::RetCode do_select(unsigned int key,
                                       const nentity_manager &nem,
                                       unsigned int &ts0_out,
                                       unsigned int &ts1_out,
                                       nclass &in_out_obj);

        virtual vlg::RetCode do_update(unsigned int key,
                                       const nentity_manager &nem,
                                       unsigned int ts0,
                                       unsigned int ts1,
                                       const nclass &in_obj);

        virtual vlg::RetCode do_delete(unsigned int key,
                                       const nentity_manager &nem,
                                       unsigned int ts0,
                                       unsigned int ts1,
                                       PersistenceDeletionMode mode,
                                       const nclass &in_obj);

        virtual vlg::RetCode do_insert(const nentity_manager &nem,
                                       unsigned int ts0,
                                       unsigned int ts1,
                                       const nclass &in_obj,
                                       bool fail_is_error = true);

        virtual vlg::RetCode do_execute_query(const nentity_manager &nem,
                                              const char *sql,
                                              persistence_query_impl **qry_out);

        virtual vlg::RetCode do_release_query(persistence_query_impl *qry);

        virtual vlg::RetCode do_next_entity_from_query(persistence_query_impl *qry,
                                                       unsigned int &ts0_out,
                                                       unsigned int &ts1_out,
                                                       nclass &out_obj);

        virtual vlg::RetCode do_execute_statement(const char *sql);


    private:
        static vlg::RetCode read_timestamp_and_del_from_record(sqlite3_stmt  *stmt,
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
                persistence_task_sqlite(pers_conn_sqlite &sql_conn, VLG_PERS_TASK_OP op_code) :
                    persistence_task(op_code),
                    sql_conn_(sql_conn),
                    sel_rud_(NULL) {
                }
            protected:

                virtual vlg::RetCode do_connect() {
                    persistence_connection_pool &pcp = sql_conn_.get_connection_pool();
                    IFLOG(trc(TH_ID, LS_OPN "%s(url:%s, usr:%s, psswd:%s)", __func__, pcp.url(),
                              pcp.user(), pcp.password()))
                    vlg::RetCode rcode = sql_conn_.sqlite_connect(pcp.url(),
                                                                  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
                    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
                    return rcode;
                }

                virtual vlg::RetCode do_create_table() {
                    vlg::RetCode rcode = vlg::RetCode_OK;
                    if((rcode = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff(), false))) {
                        if(in_drop_if_exist_) {
                            vlg::ascii_string drop_stmt;
                            RETURN_IF_NOT_OK(drop_stmt.assign("DROP TABLE "))
                            RETURN_IF_NOT_OK(drop_stmt.append(in_edesc_->get_nentity_name()))
                            RETURN_IF_NOT_OK(drop_stmt.append(";"))
                            IFLOG(dbg(TH_ID, LS_STM "%s() - drop_stmt:%s", __func__,
                                      drop_stmt.internal_buff()))
                            if((rcode = sql_conn_.sqlite_exec_stmt(drop_stmt.internal_buff()))) {
                                IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - drop failed.", __func__, rcode))
                                return rcode;
                            }
                            if((rcode = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff()))) {
                                IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - create after drop failed.", __func__,
                                          rcode))
                                return rcode;
                            }
                        } else {
                            IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - create failed.", __func__, rcode))
                            return rcode;
                        }
                    }
                    return rcode;
                }

                virtual vlg::RetCode do_select() {
                    vlg::RetCode rcode = vlg::RetCode_OK;
                    sqlite3_stmt *stmt = NULL;
                    if((rcode = sql_conn_.sqlite_prepare_stmt(stmt_bf_->internal_buff(),
                                                              &stmt))) {
                        IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - sqlite_prepare_stmt failed.", __func__,
                                  rcode))
                    } else {
                        if(!(rcode = sql_conn_.sqlite_step_stmt(stmt, *sel_rud_->sqlite_rc))) {
                            if(*sel_rud_->sqlite_rc == SQLITE_ROW) {
                                const vlg::hash_map &nm_desc =
                                    in_out_obj_->get_nentity_descriptor()->get_opaque()->GetMap_NM_MMBRDSC();
                                read_timestamp_and_del_from_record(stmt, *sel_rud_->sqlite_rc, in_out_ts0_,
                                                                   in_out_ts1_, NULL);
                                sel_rud_->stmt = stmt;
                                nm_desc.enum_elements(enum_mmbrs_fill_entity, sel_rud_);
                                rcode = vlg::RetCode_DBROW;
                            } else if(*sel_rud_->sqlite_rc == SQLITE_DONE) {
                                IFLOG(trc(TH_ID, LS_TRL "%s() - no data.", __func__))
                                rcode = vlg::RetCode_NODATA;
                            } else {
                                IFLOG(err(TH_ID, LS_TRL "%s(rc:%d) - sqlite_step_stmt - unhandled sqlite code.",
                                          __func__, *sel_rud_->sqlite_rc))
                                rcode = vlg::RetCode_DBERR;
                            }
                            vlg::RetCode rels_rcode = vlg::RetCode_OK;
                            if((rels_rcode = sql_conn_.sqlite_release_stmt(stmt))) {
                                IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_release_stmt failed.", __func__,
                                          rels_rcode))
                            }
                        } else {
                            IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_step_stmt failed.", __func__,
                                      rcode))
                        }
                    }
                    return rcode;
                }

                virtual vlg::RetCode do_update() {
                    vlg::RetCode rcode = vlg::RetCode_OK;
                    if((rcode = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff()))) {
                        IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_exec_stmt failed.", __func__,
                                  rcode))
                    }
                    return rcode;
                }

                virtual vlg::RetCode do_delete() {
                    vlg::RetCode rcode = vlg::RetCode_OK;
                    if((rcode = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff()))) {
                        IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_exec_stmt failed.", __func__,
                                  rcode))
                    }
                    return rcode;
                }

                virtual vlg::RetCode do_insert() {
                    vlg::RetCode rcode = vlg::RetCode_OK;
                    if((rcode = sql_conn_.sqlite_exec_stmt(stmt_bf_->internal_buff(),
                                                           in_fail_is_error_))) {
                        if(in_fail_is_error_) {
                            IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_exec_stmt failed.", __func__,
                                      rcode))
                        } else {
                            IFLOG(dbg(TH_ID, LS_TRL "%s(res:%d) - sqlite_exec_stmt failed.", __func__,
                                      rcode))
                        }
                    }
                    return rcode;
                }

                virtual vlg::RetCode do_execute_query() {
                    vlg::RetCode rcode = vlg::RetCode_OK;
                    sqlite3_stmt *stmt = NULL;
                    if((rcode = sql_conn_.sqlite_prepare_stmt(in_sql_, &stmt))) {
                        IFLOG(err(TH_ID, LS_CLO "%s(res:%d) - sqlite_prepare_stmt failed.", __func__,
                                  rcode))
                        return rcode;
                    }
                    in_out_query_ = new pers_query_sqlite(0, sql_conn_, *in_nem_,
                                                          stmt);  //@fixme sanity and id..
                    return rcode;
                }

                virtual vlg::RetCode do_release_query() {
                    pers_query_sqlite *qry_sqlite = static_cast<pers_query_sqlite *>(in_out_query_);
                    vlg::RetCode rcode = sql_conn_.sqlite_release_stmt(
                                             qry_sqlite->get_sqlite_stmt());
                    return rcode;
                }

                virtual vlg::RetCode do_next_entity_from_query() {
                    vlg::RetCode rcode = vlg::RetCode_OK;
                    pers_query_sqlite *qry_sqlite = static_cast<pers_query_sqlite *>(in_out_query_);
                    if(!(rcode = sql_conn_.sqlite_step_stmt(qry_sqlite->get_sqlite_stmt(),
                                                            *sel_rud_->sqlite_rc))) {
                        if(*sel_rud_->sqlite_rc == SQLITE_ROW) {
                            const vlg::hash_map &nm_desc =
                                in_out_obj_->get_nentity_descriptor()->get_opaque()->GetMap_NM_MMBRDSC();
                            read_timestamp_and_del_from_record(qry_sqlite->get_sqlite_stmt(),
                                                               *sel_rud_->sqlite_rc, in_out_ts0_, in_out_ts1_, NULL);
                            nm_desc.enum_elements(enum_mmbrs_fill_entity, sel_rud_);
                            rcode = vlg::RetCode_DBROW;
                        } else if(*sel_rud_->sqlite_rc == SQLITE_DONE) {
                            rcode = vlg::RetCode_QRYEND;
                        } else {
                            IFLOG(err(TH_ID, LS_TRL "%s(rc:%d) - sqlite_step_stmt unhandled sqlite code.",
                                      __func__, *sel_rud_->sqlite_rc))
                            rcode = vlg::RetCode_DBERR;
                        }
                    } else {
                        IFLOG(err(TH_ID, LS_TRL "%s(res:%d) - sqlite_step_stmt failed.", __func__,
                                  rcode))
                    }
                    return rcode;
                }

                virtual vlg::RetCode do_execute_statement() {
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
    persistence_connection_impl(id, conn_pool),
    db_(NULL),
    worker_(NULL)
{
    IFLOG(trc(TH_ID, LS_CTR "%s(id:%d)", __func__, id))
}

vlg::RetCode pers_conn_sqlite::sqlite_connect(const char *filename, int flags)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d, filename:%s, flags:%d)", __func__, id_,
              filename, flags))
    vlg::RetCode rcode = vlg::RetCode_OK;
    int last_rc = sqlite3_open_v2(filename, &db_, flags, 0);
    if(last_rc) {
        IFLOG(err(TH_ID, LS_TRL
                  "%s(id:%d, filename:%s) - sqlite3_open_v2(rc:%d) - errdesc[%s] - db error.",
                  __func__, id_,
                  filename, last_rc, sqlite3_errstr(last_rc)))
        status_ = PersistenceConnectionStatus_ERROR;
        rcode = vlg::RetCode_DBERR;
    } else {
        status_ = PersistenceConnectionStatus_CONNECTED;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return rcode;
}

vlg::RetCode pers_conn_sqlite::sqlite_disconnect()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
    vlg::RetCode rcode = vlg::RetCode_OK;
    int last_rc = sqlite3_close_v2(db_);
    if(last_rc) {
        IFLOG(err(TH_ID, LS_CLO
                  "%s(id:%d) - sqlite3_close_v2(rc:%d) - errdesc[%s] - db error.", __func__, id_,
                  last_rc,
                  sqlite3_errstr(last_rc)))
        status_ = PersistenceConnectionStatus_ERROR;
        rcode = vlg::RetCode_DBERR;
    } else {
        status_ = PersistenceConnectionStatus_DISCONNECTED;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return rcode;
}

vlg::RetCode pers_conn_sqlite::sqlite_exec_stmt(const char *stmt,
                                                bool fail_is_error)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d, stmt:%p)", __func__, id_, stmt))
    vlg::RetCode rcode = vlg::RetCode_OK;
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
        rcode = vlg::RetCode_DBOPFAIL;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return rcode;
}

vlg::RetCode pers_conn_sqlite::sqlite_prepare_stmt(const char *sql_stmt,
                                                   sqlite3_stmt **stmt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sql_stmt:p, stmt:%p)", __func__, sql_stmt, stmt))
    if(!stmt) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADARG;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    int last_rc = sqlite3_prepare_v2(db_, sql_stmt, (int)strlen(sql_stmt), stmt, 0);
    if(last_rc != SQLITE_OK) {
        IFLOG(err(TH_ID, LS_TRL
                  "%s(id:%d) - sqlite3_prepare_v2(rc:%d) - errdesc[%s] - db error.", __func__,
                  id_, last_rc,
                  sqlite3_errstr(last_rc)))
        rcode = vlg::RetCode_DBERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return rcode;
}

vlg::RetCode pers_conn_sqlite::sqlite_step_stmt(sqlite3_stmt *stmt,
                                                int &sqlite_rc)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(stmt:%p)", __func__, stmt))
    vlg::RetCode rcode = vlg::RetCode_OK;
    if(!stmt) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADARG;
    }
    sqlite_rc = sqlite3_step(stmt);
    if(sqlite_rc != SQLITE_ROW && sqlite_rc != SQLITE_DONE) {
        IFLOG(inf(TH_ID, LS_TRL
                  "%s(id:%d) - sqlite3_step(rc:%d) - errdesc[%s] - db operation fail.", __func__,
                  id_, sqlite_rc,
                  sqlite3_errstr(sqlite_rc)))
        rcode = vlg::RetCode_DBERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d, rc:%d)", __func__, id_, sqlite_rc))
    return rcode;
}

vlg::RetCode pers_conn_sqlite::sqlite_release_stmt(sqlite3_stmt *stmt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
    vlg::RetCode rcode = vlg::RetCode_OK;
    if(!stmt) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADARG;
    }
    int last_rc = sqlite3_finalize(stmt);
    if(last_rc != SQLITE_OK) {
        IFLOG(err(TH_ID, LS_TRL "%s(id:%d) - sqlite3_finalize(rc:%d) - db error.",
                  __func__, id_, last_rc))
        rcode = vlg::RetCode_DBERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(id:%d)", __func__, id_))
    return rcode;
}

vlg::RetCode pers_conn_sqlite::read_timestamp_and_del_from_record(
    sqlite3_stmt  *stmt,
    int           &sqlite_rc,
    unsigned int  *ts0,
    unsigned int  *ts1,
    bool          *del)
{
    vlg::RetCode rcode = vlg::RetCode_OK;
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

vlg::RetCode pers_conn_sqlite::do_connect()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
    vlg::RetCode rcode = vlg::RetCode_OK;
    worker_ = conn_pool_.get_worker_rr_can_create_start();
    if(worker_ == NULL) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - unavailable thread.", __func__,
                  vlg::RetCode_UNVRSC))
        return vlg::RetCode_UNVRSC;
    }
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                VLG_PERS_TASK_OP_CONNECT);
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//--------------------- CREATE -------------------------------------------------

struct SQLTE_ENM_CREATE_REC_UD {
    const nentity_manager &nem;
    vlg::ascii_string *create_stmt;
    vlg::ascii_string *prfx;
    bool array_fld;
    unsigned int fld_idx;   //used to render column name when the field is an array
    bool *first_key;
    bool *first_key_mmbr;
    vlg::RetCode *last_error_code;
    vlg::ascii_string *last_error_msg;
};

void enum_mmbrs_create_table(const vlg::hash_map &map, const void *key,
                             const void *ptr, void *ud)
{
    SQLTE_ENM_CREATE_REC_UD *rud = static_cast<SQLTE_ENM_CREATE_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    vlg::ascii_string idx_prfx;
    char idx_b[SQLITE_FIDX_BUFF] = {0};
    idx_prfx.assign(*(rud->prfx));
    if(rud->array_fld) {
        sprintf(idx_b, "%s%u", idx_prfx.length() ? "_" : "", rud->fld_idx);
        idx_prfx.append(idx_b);
    }
    if(mmbrd->get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd->get_field_nentity_type() == NEntityType_NENUM) {
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
                    rud->create_stmt->append(" " VLG_SQLITE_DTYPE_NUMERIC", ");
                }
            } else {
                if(rud->prfx->length()) {
                    rud->create_stmt->append(idx_prfx);
                    rud->create_stmt->append("_");
                }
                rud->create_stmt->append(mmbrd->get_member_name());
                rud->create_stmt->append(" " VLG_SQLITE_DTYPE_NUMERIC", ");
            }
        } else {
            //class, struct is a recursive step.
            SQLTE_ENM_CREATE_REC_UD rrud = *rud;
            vlg::ascii_string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd->get_member_name());
            rrud.prfx = &rprfx;
            const nentity_desc *edsc = NULL;
            if(!rud->nem.get_nentity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const vlg::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
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
                *rud->last_error_code = vlg::RetCode_GENERR;
                rud->last_error_msg->assign("enum_mmbrs_create_table: entity not found in nem [");
                rud->last_error_msg->append(mmbrd->get_field_user_type());
                rud->last_error_msg->append("]");
            }
        }
    } else {
        //primitive type
        if(mmbrd->get_field_vlg_type() == Type_ASCII) {
            if(rud->prfx->length()) {
                rud->create_stmt->append(idx_prfx);
                rud->create_stmt->append("_");
            }
            rud->create_stmt->append(mmbrd->get_member_name());
            rud->create_stmt->append(" " VLG_SQLITE_DTYPE_TEXT", ");
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
                rud->create_stmt->append(SQLITE_TypeStr_From_VLGType(
                                             mmbrd->get_field_vlg_type()));
                rud->create_stmt->append(", ");
            }
        } else {
            if(rud->prfx->length()) {
                rud->create_stmt->append(idx_prfx);
                rud->create_stmt->append("_");
            }
            rud->create_stmt->append(mmbrd->get_member_name());
            rud->create_stmt->append(" ");
            rud->create_stmt->append(SQLITE_TypeStr_From_VLGType(
                                         mmbrd->get_field_vlg_type()));
            rud->create_stmt->append(", ");
        }
    }
}

void enum_keyset_create(const vlg::linked_list &list, const void *ptr,
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

void enum_keys_create_table(const vlg::hash_map &map, const void *key,
                            const void *ptr, void *ud)
{
    SQLTE_ENM_CREATE_REC_UD *rud = static_cast<SQLTE_ENM_CREATE_REC_UD *>(ud);
    const key_desc *kdsc = *(const key_desc **)ptr;
    const vlg::linked_list &kset = kdsc->get_opaque()->get_key_field_set();
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

vlg::RetCode pers_conn_sqlite::do_create_table(const nentity_manager &nem,
                                               const nentity_desc &edesc,
                                               bool drop_if_exist)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(drop_if_exist:%d)", __func__, drop_if_exist))
    vlg::RetCode rcode = vlg::RetCode_OK;
    vlg::RetCode last_error_code = vlg::RetCode_OK;
    vlg::ascii_string last_error_str;
    vlg::ascii_string create_stmt;
    RETURN_IF_NOT_OK(create_stmt.assign("CREATE TABLE "))
    RETURN_IF_NOT_OK(create_stmt.append(edesc.get_nentity_name()))
    RETURN_IF_NOT_OK(create_stmt.append("("))
    RETURN_IF_NOT_OK(create_stmt.append(P_F_TS0" " VLG_SQLITE_DTYPE_NUMERIC", "))
    RETURN_IF_NOT_OK(create_stmt.append(P_F_TS1" " VLG_SQLITE_DTYPE_NUMERIC", "))
    RETURN_IF_NOT_OK(create_stmt.append(P_F_DEL" " VLG_SQLITE_DTYPE_NUMERIC", "))
    const vlg::hash_map &nm_desc = edesc.get_opaque()->GetMap_NM_MMBRDSC();
    vlg::ascii_string prfx;
    RETURN_IF_NOT_OK(prfx.assign(""))
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

    nm_desc.enum_elements(enum_mmbrs_create_table, &rud);
    const vlg::hash_map &idk_desc = edesc.get_opaque()->GetMap_KEYID_KDESC();
    idk_desc.enum_elements(enum_keys_create_table, &rud);
    RETURN_IF_NOT_OK(create_stmt.append(");"))
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                VLG_PERS_TASK_OP_CREATETABLE);
    task->in_edesc(edesc);
    task->in_drop_if_exist(drop_if_exist);
    task->stmt_bf(create_stmt);
    IFLOG(dbg(TH_ID, LS_STM "%s() - create_stmt:%s", __func__,
              create_stmt.internal_buff()))
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return rcode;
}

//--------------------- SELECT -------------------------------------------------

void enum_keyset_select(const vlg::linked_list &list, const void *ptr,
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
    if(mmbrd->get_field_vlg_type() == Type_ENTITY) {
        //treat enum as number
        obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
        sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
        rud->where_claus->append("=");
        rud->where_claus->append(rud->enm_buff->getValBuff());
    } else {
        //can be only a primitive type
        if(mmbrd->get_field_vlg_type() == Type_ASCII) {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            sprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()), "%s",
                    obj_f_ptr);
            rud->where_claus->append("='");
            rud->where_claus->append(rud->enm_buff->getValBuff());
            rud->where_claus->append("'");
        } else {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_vlg_type(),
                                  rud->enm_buff->getValBuff());
            rud->where_claus->append("=");
            rud->where_claus->append(rud->enm_buff->getValBuff());
        }
    }
}

vlg::RetCode pers_conn_sqlite::do_select(unsigned int key,
                                         const nentity_manager &nem,
                                         unsigned int &ts0_out,
                                         unsigned int &ts1_out,
                                         nclass &in_out_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%d)", __func__, key))
    vlg::RetCode rcode = vlg::RetCode_OK;
    vlg::RetCode last_error_code = vlg::RetCode_OK;
    vlg::ascii_string last_error_str;
    vlg::ascii_string select_stmt;
    vlg::ascii_string columns;
    vlg::ascii_string where_claus;
    int sqlite_rc = 0;
    RETURN_IF_NOT_OK(columns.assign(""))
    RETURN_IF_NOT_OK(where_claus.assign(P_F_DEL"=0 AND "));
    vlg::ascii_string prfx;
    RETURN_IF_NOT_OK(prfx.assign(""))
    bool frst_fld = true, frst_key = true, frst_key_mmbr = true;
    int column_idx = 3; //column idx, [ts0, ts1, del] we start from 3.
    SQLTE_ENM_BUFF *enm_buff = new SQLTE_ENM_BUFF();
    SQLTE_ENM_SELECT_REC_UD rud = {nem,
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

    const vlg::hash_map &idk_desc =
        in_out_obj.get_nentity_descriptor()->get_opaque()->GetMap_KEYID_KDESC();
    const key_desc *kdsc = NULL;
    if(idk_desc.get(&key, &kdsc)) {
        IFLOG(err(TH_ID, LS_CLO "%s() - key not found [key:%d]", __func__, key))
        return vlg::RetCode_BADARG;
    }
    const vlg::linked_list &kset = kdsc->get_opaque()->get_key_field_set();
    kset.enum_elements(enum_keyset_select, &rud);
    /*not necessary because we can use select (*), column order is preserved*/
    //nm_desc.Enum(enum_mmbrs_select, &rud);
    RETURN_IF_NOT_OK(select_stmt.assign("SELECT "))
    //CHK_MTH(select_stmt.Apnd(columns))
    RETURN_IF_NOT_OK(select_stmt.append("*"))
    RETURN_IF_NOT_OK(select_stmt.append(" FROM "))
    RETURN_IF_NOT_OK(select_stmt.append(
                         in_out_obj.get_nentity_descriptor()->get_nentity_name()))
    RETURN_IF_NOT_OK(select_stmt.append(" WHERE "))
    RETURN_IF_NOT_OK(select_stmt.append(where_claus));
    RETURN_IF_NOT_OK(select_stmt.append(";"));
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                VLG_PERS_TASK_OP_SELECT);
    task->in_nem(nem);
    task->in_key(key);
    task->in_out_ts0(ts0_out);
    task->in_out_ts1(ts1_out);
    task->in_out_obj(in_out_obj);
    task->stmt_bf(select_stmt);
    task->sel_rud_ = &rud;
    IFLOG(dbg(TH_ID, LS_QRY "%s() - select_stmt:%s", __func__,
              select_stmt.internal_buff()))
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//--------------------- UPDATE -------------------------------------------------

void enum_keyset_update(const vlg::linked_list &list, const void *ptr,
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
    if(mmbrd->get_field_vlg_type() == Type_ENTITY) {
        //treat enum as number
        obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
        sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
        rud->where_claus->append("=");
        rud->where_claus->append(rud->enm_buff->getValBuff());
    } else {
        //can be only a primitive type
        if(mmbrd->get_field_vlg_type() == Type_ASCII) {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            sprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()), "%s",
                    obj_f_ptr);
            rud->where_claus->append("='");
            rud->where_claus->append(rud->enm_buff->getValBuff());
            rud->where_claus->append("'");
        } else {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_vlg_type(),
                                  rud->enm_buff->getValBuff());
            rud->where_claus->append("=");
            rud->where_claus->append(rud->enm_buff->getValBuff());
        }
    }
}

vlg::RetCode pers_conn_sqlite::do_update(unsigned int key,
                                         const nentity_manager &nem,
                                         unsigned int ts0,
                                         unsigned int ts1,
                                         const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%d)", __func__, key))
    vlg::RetCode rcode = vlg::RetCode_OK;
    vlg::RetCode last_error_code = vlg::RetCode_OK;
    vlg::ascii_string last_error_str;
    vlg::ascii_string update_stmt;
    vlg::ascii_string set_section;
    vlg::ascii_string where_claus;
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
    const vlg::hash_map &nm_desc =
        in_obj.get_nentity_descriptor()->get_opaque()->GetMap_NM_MMBRDSC();
    vlg::ascii_string prfx;
    RETURN_IF_NOT_OK(prfx.assign(""))
    bool frst_fld = true, frst_key = true, frst_key_mmbr = true;
    SQLTE_ENM_BUFF *enm_buff = new SQLTE_ENM_BUFF();
    SQLTE_ENM_UPDATE_REC_UD rud = { nem,
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

    const vlg::hash_map &idk_desc =
        in_obj.get_nentity_descriptor()->get_opaque()->GetMap_KEYID_KDESC();
    const key_desc *kdsc = NULL;
    if(idk_desc.get(&key, &kdsc)) {
        IFLOG(err(TH_ID, LS_CLO "%s() - key not found [key:%d]", __func__, key))
        return vlg::RetCode_BADARG;
    }
    const vlg::linked_list &kset = kdsc->get_opaque()->get_key_field_set();
    kset.enum_elements(enum_keyset_update, &rud);
    nm_desc.enum_elements(enum_mmbrs_update, &rud);
    RETURN_IF_NOT_OK(update_stmt.assign("UPDATE "))
    RETURN_IF_NOT_OK(update_stmt.append(
                         in_obj.get_nentity_descriptor()->get_nentity_name()))
    RETURN_IF_NOT_OK(update_stmt.append(" SET "))
    RETURN_IF_NOT_OK(update_stmt.append(set_section))
    RETURN_IF_NOT_OK(update_stmt.append(" WHERE "))
    RETURN_IF_NOT_OK(update_stmt.append(where_claus));
    RETURN_IF_NOT_OK(update_stmt.append(";"));
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                VLG_PERS_TASK_OP_UPDATE);
    task->in_nem(nem);
    task->in_key(key);
    task->in_out_ts0(ts0);
    task->in_out_ts1(ts1);
    task->stmt_bf(update_stmt);
    IFLOG(dbg(TH_ID, LS_STM "%s() - update_stmt:%s", __func__,
              update_stmt.internal_buff()))
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//--------------------- DELETE -------------------------------------------------

void enum_keyset_delete(const vlg::linked_list &list, const void *ptr,
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
    if(mmbrd->get_field_vlg_type() == Type_ENTITY) {
        //treat enum as number
        obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
        sprintf(rud->enm_buff->getValBuff(), "%d", *(int *)obj_f_ptr);
        rud->where_claus->append("=");
        rud->where_claus->append(rud->enm_buff->getValBuff());
    } else {
        //can be only a primitive type
        if(mmbrd->get_field_vlg_type() == Type_ASCII) {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            sprintf(rud->enm_buff->getValBuff_RSZ(mmbrd->get_field_nmemb()), "%s",
                    obj_f_ptr);
            rud->where_claus->append("='");
            rud->where_claus->append(rud->enm_buff->getValBuff());
            rud->where_claus->append("'");
        } else {
            obj_f_ptr = rud->obj_ptr + mmbrd->get_field_offset();
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_vlg_type(),
                                  rud->enm_buff->getValBuff());
            rud->where_claus->append("=");
            rud->where_claus->append(rud->enm_buff->getValBuff());
        }
    }
}

vlg::RetCode pers_conn_sqlite::do_delete(unsigned int key,
                                         const nentity_manager &nem,
                                         unsigned int ts0,
                                         unsigned int ts1,
                                         PersistenceDeletionMode  mode,
                                         const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%d)", __func__, key))
    vlg::RetCode rcode = vlg::RetCode_OK;
    vlg::RetCode last_error_code = vlg::RetCode_OK;
    vlg::ascii_string last_error_str;
    vlg::ascii_string delete_stmt;
    vlg::ascii_string where_claus;
    RETURN_IF_NOT_OK(where_claus.assign(""))
    bool frst_key = true, frst_key_mmbr = true;
    SQLTE_ENM_BUFF *enm_buff = new SQLTE_ENM_BUFF();
    SQLTE_ENM_DELETE_REC_UD rud = { nem,
                                    reinterpret_cast<const char *>(&in_obj),
                                    &where_claus,
                                    &frst_key,
                                    &frst_key_mmbr,
                                    &last_error_code,
                                    &last_error_str,
                                    enm_buff
                                  };

    const vlg::hash_map &idk_desc =
        in_obj.get_nentity_descriptor()->get_opaque()->GetMap_KEYID_KDESC();
    const key_desc *kdsc = NULL;
    if(idk_desc.get(&key, &kdsc)) {
        IFLOG(err(TH_ID, LS_CLO "%s() - key not found [key:%d]", __func__, key))
        return vlg::RetCode_BADARG;
    }
    const vlg::linked_list &kset = kdsc->get_opaque()->get_key_field_set();
    kset.enum_elements(enum_keyset_delete, &rud);
    if(mode == PersistenceDeletionMode_PHYSICAL) {
        RETURN_IF_NOT_OK(delete_stmt.assign("DELETE FROM "))
        RETURN_IF_NOT_OK(delete_stmt.append(
                             in_obj.get_nentity_descriptor()->get_nentity_name()))
        RETURN_IF_NOT_OK(delete_stmt.append(" WHERE "))
        RETURN_IF_NOT_OK(delete_stmt.append(where_claus));
        RETURN_IF_NOT_OK(delete_stmt.append(";"));
    } else {
        RETURN_IF_NOT_OK(delete_stmt.assign("UPDATE "))
        RETURN_IF_NOT_OK(delete_stmt.append(
                             in_obj.get_nentity_descriptor()->get_nentity_name()))
        RETURN_IF_NOT_OK(delete_stmt.append(" SET DEL=1 WHERE "))
        RETURN_IF_NOT_OK(delete_stmt.append(where_claus));
        RETURN_IF_NOT_OK(delete_stmt.append(";"));
    }
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                VLG_PERS_TASK_OP_DELETE);
    task->in_nem(nem);
    task->in_key(key);
    task->in_out_ts0(ts0);
    task->in_out_ts1(ts1);
    task->in_mode(mode);
    task->in_obj(in_obj);
    task->stmt_bf(delete_stmt);
    IFLOG(dbg(TH_ID, LS_STM "%s() - delete_stmt:%s", __func__,
              delete_stmt.internal_buff()))
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//--------------------- INSERT -------------------------------------------------

void enum_mmbrs_insert(const vlg::hash_map &map, const void *key,
                       const void *ptr,
                       void *ud)
{
    SQLTE_ENM_INSERT_REC_UD *rud = static_cast<SQLTE_ENM_INSERT_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    vlg::ascii_string idx_prfx;
    char idx_b[SQLITE_FIDX_BUFF] = {0};
    const char *obj_f_ptr = NULL;
    idx_prfx.assign(*(rud->prfx));
    if(rud->array_fld) {
        sprintf(idx_b, "%s%u", idx_prfx.length() ? "_" : "", rud->fld_idx);
        idx_prfx.append(idx_b);
    }
    if(mmbrd->get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd->get_field_nentity_type() == NEntityType_NENUM) {
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
            vlg::ascii_string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd->get_member_name());
            rrud.prfx = &rprfx;
            const nentity_desc *edsc = NULL;
            if(!rud->nem.get_nentity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const vlg::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
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
                *rud->last_error_code = vlg::RetCode_GENERR;
                rud->last_error_msg->assign("enum_mmbrs_insert: entity not found in nem [");
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
        if(mmbrd->get_field_vlg_type() == Type_ASCII) {
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
                fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_vlg_type(),
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
            fill_buff_fld_value_1(obj_f_ptr, mmbrd->get_field_vlg_type(),
                                  rud->enm_buff->getValBuff());
            rud->values->append(rud->enm_buff->getValBuff());
        }
    }
}

vlg::RetCode pers_conn_sqlite::do_insert(const nentity_manager &nem,
                                         unsigned int ts0,
                                         unsigned int ts1,
                                         const nclass &in_obj,
                                         bool fail_is_error)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode rcode = vlg::RetCode_OK;
    vlg::RetCode last_error_code = vlg::RetCode_OK;
    vlg::ascii_string last_error_str;
    vlg::ascii_string insert_stmt;
    vlg::ascii_string values;
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
                         in_obj.get_nentity_descriptor()->get_nentity_name()))
    RETURN_IF_NOT_OK(insert_stmt.append("(" P_F_TS0", " P_F_TS1", " P_F_DEL", "))
    const vlg::hash_map &nm_desc =
        in_obj.get_nentity_descriptor()->get_opaque()->GetMap_NM_MMBRDSC();
    vlg::ascii_string prfx;
    RETURN_IF_NOT_OK(prfx.assign(""))
    bool frst_fld = true;
    SQLTE_ENM_BUFF *enm_buff = new SQLTE_ENM_BUFF();
    SQLTE_ENM_INSERT_REC_UD rud = {nem,
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
                                                                VLG_PERS_TASK_OP_INSERT);
    task->in_fail_is_error(fail_is_error);
    task->stmt_bf(insert_stmt);
    IFLOG(dbg(TH_ID, LS_STM "%s() - insert_stmt:%s", __func__,
              insert_stmt.internal_buff()))
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//--------------------- QUERY -------------------------------------------------

vlg::RetCode pers_conn_sqlite::do_execute_query(const nentity_manager &nem,
                                                const char *sql, persistence_query_impl **qry_out)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sql:%p, qry_out:%p)", __func__, sql, qry_out))
    vlg::RetCode rcode = vlg::RetCode_OK;
    IFLOG(trc(TH_ID, LS_QRY "%s() - query-sql:%s", __func__, sql))
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                VLG_PERS_TASK_OP_EXECUTEQUERY);
    task->in_nem(nem);
    task->in_sql(sql);
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    *qry_out = task->in_out_query();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d, qry:%p)", __func__, rcode, *qry_out))
    return rcode;
}

vlg::RetCode pers_conn_sqlite::do_release_query(persistence_query_impl *qry)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(qry:%p)", __func__, qry))
    vlg::RetCode rcode = vlg::RetCode_OK;
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                VLG_PERS_TASK_OP_RELEASEQUERY);
    task->in_out_query(qry);
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode pers_conn_sqlite::do_next_entity_from_query(persistence_query_impl
                                                         *qry,
                                                         unsigned int &ts0_out,
                                                         unsigned int &ts1_out,
                                                         nclass &out_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(qry:%p)", __func__, qry))
    pers_query_sqlite *qry_sqlite = static_cast<pers_query_sqlite *>(qry);
    vlg::RetCode rcode = vlg::RetCode_OK;
    vlg::RetCode last_error_code = vlg::RetCode_OK;
    vlg::ascii_string last_error_str;
    vlg::ascii_string select_stmt;
    vlg::ascii_string columns;
    vlg::ascii_string where_claus;
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
                                                                VLG_PERS_TASK_OP_NEXTENTITYFROMQUERY);
    task->in_out_query(qry_sqlite);
    task->in_out_ts0(ts0_out);
    task->in_out_ts1(ts1_out);
    task->in_out_obj(out_obj);
    task->sel_rud_ = &rud;
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    delete task;
    if(enm_buff) {
        delete enm_buff;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//--------------------- EXEC STMT ----------------------------------------------

vlg::RetCode pers_conn_sqlite::do_execute_statement(const char *sql)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sql:%p)", __func__, sql))
    vlg::RetCode rcode = vlg::RetCode_OK;
    IFLOG(trc(TH_ID, LS_STM "%s() - sql:%s", __func__, sql))
    persistence_task_sqlite *task = new persistence_task_sqlite(*this,
                                                                VLG_PERS_TASK_OP_EXECUTESTATEMENT);
    task->in_sql(sql);
    if((rcode = worker_->submit_task(task))) {
        IFLOG(cri(TH_ID, LS_CLO "%s(res:%d) - submit failed.", __func__, rcode))
    } else {
        task->await_for_status(vlg::PTASK_STATUS_EXECUTED);
    }
    rcode = task->op_res();
    delete task;
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//-----------------------------
// VLG_PERS_DRIV_SQLITE - DRIVER
//-----------------------------
static unsigned int sqlite_connid = 0;
unsigned int GetSQLITE_NEXT_CONNID()
{
    return ++sqlite_connid;
}

//-----------------------------
// pers_driv_sqlite
//-----------------------------
class pers_driv_sqlite : public persistence_driver_impl {
    public:
        static pers_driv_sqlite *get_instance();

    private:
        //---ctors
        pers_driv_sqlite();
        ~pers_driv_sqlite();

    public:
        virtual vlg::RetCode new_connection(persistence_connection_pool &conn_pool,
                                            persistence_connection_impl **new_conn);

        virtual vlg::RetCode close_connection(persistence_connection_impl &conn);

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
    persistence_driver_impl(VLG_PERS_DRIV_SQLITE_ID)
{
    IFLOG(trc(TH_ID, LS_CTR "%s(id:%d)", __func__, id_))
}

pers_driv_sqlite::~pers_driv_sqlite()
{
    IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
}

vlg::RetCode pers_driv_sqlite::new_connection(persistence_connection_pool
                                              &conn_pool,
                                              persistence_connection_impl **new_conn)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(url:%s, usr:%s, psswd:%s, new_conn:%p)", __func__,
              conn_pool.url(), conn_pool.user(),
              conn_pool.password(), new_conn))
    vlg::RetCode rcode = vlg::RetCode_OK;
    pers_conn_sqlite *new_conn_instance = new pers_conn_sqlite(
        GetSQLITE_NEXT_CONNID(), conn_pool);
    if(!new_conn_instance) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - new failed.", __func__))
        return vlg::RetCode_MEMERR;
    }
    if(!rcode) {
        *new_conn = new_conn_instance;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d, new_conn_instance:%p, id:%d)", __func__,
              rcode, new_conn_instance,
              new_conn_instance->get_id()))
    return rcode;
}

vlg::RetCode pers_driv_sqlite::close_connection(persistence_connection_impl
                                                &conn)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode res = static_cast<pers_conn_sqlite &>(conn).sqlite_disconnect();
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d, id:%d)", __func__, res, conn.get_id()))
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
    EXP_SYM persistence_driver_impl *get_pers_driv_sqlite()
    {
        return pers_driv_sqlite::get_instance();
    }
}

}
