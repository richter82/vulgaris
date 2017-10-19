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

#include "vlg_pers_impl.h"

namespace vlg {

//-----------------------------
// PERSISTENCE
//-----------------------------

//-----------------------------
// persistence_task
//-----------------------------
persistence_task::persistence_task(VLG_PERS_TASK_OP op_code) :
    op_code_(op_code),
    op_res_(vlg::RetCode_OK),
    in_bem_(NULL),
    in_mode_(PersistenceDeletionMode_UNDEFINED),
    in_out_obj_(NULL),
    in_sql_(NULL),
    in_key_(0),
    in_edesc_((NULL)),
    in_drop_if_exist_(false),
    in_out_ts0_(NULL),
    in_out_ts1_(NULL),
    in_out_query_(NULL),
    in_fail_is_error_(false),
    stmt_bf_(NULL)
{}

VLG_PERS_TASK_OP persistence_task::op_code() const
{
    return op_code_;
}

void persistence_task::op_code(VLG_PERS_TASK_OP val)
{
    op_code_ = val;
}

vlg::RetCode persistence_task::op_res() const
{
    return op_res_;
}

void persistence_task::op_res(vlg::RetCode val)
{
    op_res_ = val;
}

void persistence_task::in_bem(const entity_manager &val)
{
    in_bem_ = &val;
}

PersistenceDeletionMode persistence_task::in_mode() const
{
    return in_mode_;
}

void persistence_task::in_mode(PersistenceDeletionMode val)
{
    in_mode_ = val;
}

void persistence_task::in_obj(const nclass &val)
{
    in_obj_ = &val;
}

void persistence_task::in_out_obj(nclass &val)
{
    in_out_obj_ = &val;
}

void persistence_task::in_sql(const char *val)
{
    in_sql_ = val;
}

unsigned short persistence_task::in_key() const
{
    return in_key_;
}

void persistence_task::in_key(unsigned short val)
{
    in_key_ = val;
}

void persistence_task::in_edesc(const entity_desc &val)
{
    in_edesc_ = &val;
}

bool persistence_task::in_drop_if_exist() const
{
    return in_drop_if_exist_;
}

void persistence_task::in_drop_if_exist(bool val)
{
    in_drop_if_exist_ = val;
}

void persistence_task::in_out_ts0(unsigned int &val)
{
    in_out_ts0_ = &val;
}

void persistence_task::in_out_ts1(unsigned int &val)
{
    in_out_ts1_ = &val;
}

persistence_query_impl *persistence_task::in_out_query() const
{
    return in_out_query_;
}

void persistence_task::in_out_query(persistence_query_impl *val)
{
    in_out_query_ = val;
}

bool persistence_task::in_fail_is_error() const
{
    return in_fail_is_error_;
}

void  persistence_task::in_fail_is_error(bool val)
{
    in_fail_is_error_ = val;
}

vlg::ascii_string &persistence_task::stmt_bf()
{
    return *stmt_bf_;
}

void persistence_task::stmt_bf(vlg::ascii_string &stmt_bf)
{
    stmt_bf_ = &stmt_bf;
}

vlg::RetCode persistence_task::execute()
{
    switch(op_code_) {
        case VLG_PERS_TASK_OP_CONNECT:
            op_res_ = do_connect();
            break;
        case VLG_PERS_TASK_OP_CREATETABLE:
            op_res_ = do_create_table();
            break;
        case VLG_PERS_TASK_OP_SELECT:
            op_res_ = do_select();
            break;
        case VLG_PERS_TASK_OP_UPDATE:
            op_res_ = do_update();
            break;
        case VLG_PERS_TASK_OP_DELETE:
            op_res_ = do_delete();
            break;
        case VLG_PERS_TASK_OP_INSERT:
            op_res_ = do_insert();
            break;
        case VLG_PERS_TASK_OP_EXECUTEQUERY:
            op_res_ = do_execute_query();
            break;
        case VLG_PERS_TASK_OP_RELEASEQUERY:
            op_res_ = do_release_query();
            break;
        case VLG_PERS_TASK_OP_NEXTENTITYFROMQUERY:
            op_res_ = do_next_entity_from_query();
            break;
        case VLG_PERS_TASK_OP_EXECUTESTATEMENT:
            op_res_ = do_execute_statement();
            break;
        default:
            op_res_ = vlg::RetCode_UNSP;
            break;
    }
    return op_res_;
}

//-----------------------------
// persistence_connection_pool
// internal only
//-----------------------------
persistence_connection_pool::persistence_connection_pool(
    persistence_driver_impl &driv,
    const char *url,
    const char *usr,
    const char *psswd,
    unsigned int conn_pool_sz,
    unsigned int conn_pool_th_max_sz) :
    driv_(driv),
    conn_pool_sz_(conn_pool_sz),
    conn_pool_curr_idx_(0),
    conn_idx_conn_hm_(vlg::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    conn_pool_th_max_sz_(conn_pool_th_max_sz),
    conn_pool_th_curr_sz_(0),
    conn_pool_th_curr_idx_(0),
    conn_pool_th_pool_(NULL)
{
    url_.assign(url ? url : "");
    usr_.assign(usr ? usr : "");
    psswd_.assign(psswd ? psswd : "");
    if(conn_pool_th_max_sz > 0) {
        conn_pool_th_pool_ = new persistence_worker*[conn_pool_th_max_sz];
    }
}

persistence_connection_pool::~persistence_connection_pool()
{
    if(conn_pool_th_max_sz_) {
        for(unsigned int i = 0; i < conn_pool_th_curr_sz_; i++) {
            delete conn_pool_th_pool_[i];
        }
        delete[] conn_pool_th_pool_;
    }
}

persistence_driver_impl &persistence_connection_pool::driver()
{
    return driv_;
}

const char *persistence_connection_pool::password() const
{
    return psswd_.internal_buff();
}

const char *persistence_connection_pool::user() const
{
    return usr_.internal_buff();
}

const char *persistence_connection_pool::url() const
{
    return url_.internal_buff();
}

vlg::RetCode persistence_connection_pool::init()
{
    conn_idx_conn_hm_.init(HM_SIZE_TINY);
    return vlg::RetCode_OK;
}

vlg::RetCode persistence_connection_pool::start()
{
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    persistence_connection_impl *conn = NULL;
    for(unsigned int i=0; i<conn_pool_sz_; i++) {
        if((cdrs_res = driv_.new_connection(*this, &conn))) {
            break;
        }
        conn_idx_conn_hm_.put(&i, &conn);
        conn->connect();
    }
    return cdrs_res;
}

persistence_connection_impl *persistence_connection_pool::request_connection()
{
    persistence_connection_impl *conn = NULL;
    mon_.lock();
    conn_idx_conn_hm_.get(&conn_pool_curr_idx_, &conn);
    conn_pool_curr_idx_ = (conn_pool_curr_idx_+1) % conn_pool_sz_;
    mon_.unlock();
    return conn;
}

persistence_worker
*persistence_connection_pool::get_worker_rr_can_create_start()
{
    if(conn_pool_th_max_sz_ == 0) {
        return NULL;
    }
    persistence_worker *wrkr = NULL;
    if(conn_pool_th_curr_sz_ < conn_pool_th_max_sz_) {
        wrkr = conn_pool_th_pool_[conn_pool_th_curr_sz_] = new
        persistence_worker(
            *this);
        wrkr->start();
        conn_pool_th_curr_sz_++;
    } else {
        wrkr = conn_pool_th_pool_[conn_pool_th_curr_idx_];
        conn_pool_th_curr_idx_ = (conn_pool_th_curr_idx_ + 1) % conn_pool_th_curr_sz_;
    }
    return wrkr;
}

persistence_worker *persistence_connection_pool::get_worker_rr()
{
    if(conn_pool_th_max_sz_ == 0 || conn_pool_th_curr_sz_ == 0) {
        return NULL;
    }
    persistence_worker *wrkr = NULL;
    wrkr = conn_pool_th_pool_[conn_pool_th_curr_idx_];
    conn_pool_th_curr_idx_ = (conn_pool_th_curr_idx_ + 1) % conn_pool_th_curr_sz_;
    return wrkr;
}

//-----------------------------
// persistence_worker
//-----------------------------
nclass_logger *persistence_worker::log_ = NULL;

persistence_worker::persistence_worker(persistence_connection_pool &conn_pool) :
    conn_pool_(conn_pool),
    task_queue_(vlg::sngl_ptr_obj_mng())
{
    log_ = get_nclass_logger("persistence_worker");
    IFLOG(trc(TH_ID, LS_CTR "%s", __func__))
    task_queue_.init();
}

persistence_worker::~persistence_worker()
{}

vlg::RetCode persistence_worker::submit_task(persistence_task *task)
{
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if((cdrs_res = task_queue_.put(&task))) {
        task->set_status(vlg::PTASK_STATUS_REJECTED);
        IFLOG(cri(TH_ID, LS_TRL "%s() - [RetCode_CODE:%d]", __func__, cdrs_res))
    } else {
        task->set_status(vlg::PTASK_STATUS_SUBMITTED);
    }
    return cdrs_res;
}

void *persistence_worker::run()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::p_task *task = NULL;
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    do {
        if(!(cdrs_res = task_queue_.get(&task))) {
            task->set_execution_result(task->execute());
            task->set_status(vlg::PTASK_STATUS_EXECUTED);
        } else {
            IFLOG(cri(TH_ID, LS_CLO "%s(%d) - end", __func__, cdrs_res))
            return (void *)1;
        }
    } while(true);
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return 0;
}

persistence_connection_pool &persistence_worker::get_connection_pool()
{
    return conn_pool_;
}

//-----------------------------
// persistence_connection_impl - CONNECTION
//-----------------------------

nclass_logger *persistence_connection_impl::log_ = NULL;

persistence_connection_impl::persistence_connection_impl(unsigned int id,
                                                         persistence_connection_pool &conn_pool) :
    id_(id),
    status_(PersistenceConnectionStatus_DISCONNECTED),
    conn_pool_(conn_pool)
{
    log_ = get_nclass_logger("persistence_connection_impl");
    IFLOG(trc(TH_ID, LS_CTR "%s(id:%d)", __func__, id))
}

persistence_connection_impl::~persistence_connection_impl()
{
    IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
}

PersistenceConnectionStatus persistence_connection_impl::status() const
{
    return status_;
}

unsigned int persistence_connection_impl::get_id() const
{
    return id_;
}

persistence_driver_impl &persistence_connection_impl::get_driver()
{
    return conn_pool_.driver();
}

persistence_connection_pool &persistence_connection_impl::get_connection_pool()
{
    return conn_pool_;
}

vlg::RetCode persistence_connection_impl::connect()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    if(status_ != PersistenceConnectionStatus_DISCONNECTED) {
        IFLOG(err(TH_ID, LS_CLO "%s() -  pers-conn bad status:%d", __func__, status_))
        return vlg::RetCode_BADSTTS;
    }
    vlg::RetCode cdrs_res = do_connect();
    if(cdrs_res == vlg::RetCode_OK) {
        status_ = PersistenceConnectionStatus_CONNECTED;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::create_entity_schema(
    PersistenceAlteringMode mode, const entity_manager &bem,
    unsigned int nclass_id)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(mode:%d, nclass_id:%d)", __func__, mode, nclass_id))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    const entity_desc *edesc = NULL;
    if(bem.get_entity_descriptor(nclass_id, &edesc)) {
        IFLOG(err(TH_ID, LS_CLO "%s() - cannot find entity(nclass_id:%d)", __func__,
                  nclass_id))
        return vlg::RetCode_BADARG;
    }
    if(!edesc->is_persistent()) {
        IFLOG(err(TH_ID, LS_CLO "%s(nclass_id:%d) - not persistent", __func__,
                  edesc->get_nclass_id()))
        return vlg::RetCode_BADARG;
    }
    switch(mode) {
        case PersistenceAlteringMode_CREATE_ONLY:
            cdrs_res = do_create_table(bem, *edesc, false);
            break;
        case PersistenceAlteringMode_DROP_IF_EXIST:
            cdrs_res = do_create_table(bem, *edesc, true);
            break;
        case PersistenceAlteringMode_CREATE_OR_UPDATE:
            cdrs_res = vlg::RetCode_UNSP;
            break;
        default:
            cdrs_res = vlg::RetCode_BADARG;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::create_entity_schema(
    PersistenceAlteringMode mode, const entity_manager &bem,
    const entity_desc &edesc)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(mode:%d)", __func__, mode))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if(!edesc.is_persistent()) {
        IFLOG(err(TH_ID, LS_CLO "%s(nclass_id:%d) - not persistent", __func__,
                  edesc.get_nclass_id()))
        return vlg::RetCode_BADARG;
    }
    switch(mode) {
        case PersistenceAlteringMode_CREATE_ONLY:
            cdrs_res = do_create_table(bem, edesc, false);
            break;
        case PersistenceAlteringMode_DROP_IF_EXIST:
            cdrs_res = do_create_table(bem, edesc, true);
            break;
        case PersistenceAlteringMode_CREATE_OR_UPDATE:
            cdrs_res = vlg::RetCode_UNSP;
            break;
        default:
            cdrs_res = vlg::RetCode_BADARG;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::load_entity(unsigned short key,
                                                      const entity_manager &bem,
                                                      unsigned int &ts0_out,
                                                      unsigned int &ts1_out,
                                                      nclass &in_out_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%d)", __func__, key))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    const entity_desc *edesc = in_out_obj.get_entity_descriptor();
    if(!edesc->is_persistent()) {
        IFLOG(err(TH_ID, LS_CLO "(nclass_id:%d) - not persistent", __func__,
                  edesc->get_nclass_id()))
        return vlg::RetCode_BADARG;
    }
    cdrs_res = do_select(key, bem, ts0_out, ts1_out, in_out_obj);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::save_entity(const entity_manager
                                                      &bem,
                                                      unsigned int ts0,
                                                      unsigned int ts1,
                                                      const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    const entity_desc *edesc = in_obj.get_entity_descriptor();
    if(!edesc->is_persistent()) {
        IFLOG(err(TH_ID, LS_CLO "%s(nclass_id:%d) - not persistent", __func__,
                  edesc->get_nclass_id()))
        return vlg::RetCode_BADARG;
    }
    cdrs_res = do_insert(bem, ts0, ts1, in_obj);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::update_entity(unsigned short key,
                                                        const entity_manager &bem,
                                                        unsigned int ts0,
                                                        unsigned int ts1,
                                                        const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    const entity_desc *edesc = in_obj.get_entity_descriptor();
    if(!edesc->is_persistent()) {
        IFLOG(err(TH_ID, LS_CLO "%s(nclass_id:%d) - not persistent", __func__,
                  edesc->get_nclass_id()))
        return vlg::RetCode_BADARG;
    }
    cdrs_res = do_update(key, bem, ts0, ts1, in_obj);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::save_or_update_entity(
    unsigned short key,
    const entity_manager &bem,
    unsigned int ts0,
    unsigned int ts1,
    const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    const entity_desc *edesc = in_obj.get_entity_descriptor();
    if(!edesc->is_persistent()) {
        IFLOG(err(TH_ID, LS_CLO "%s(nclass_id:%d) - not persistent", __func__,
                  edesc->get_nclass_id()))
        return vlg::RetCode_BADARG;
    }
    cdrs_res = do_insert(bem, ts0, ts1, in_obj, false);
    if(cdrs_res) {
        cdrs_res = do_update(key, bem, ts0, ts1, in_obj);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::remove_entity(unsigned short key,
                                                        const entity_manager &bem,
                                                        unsigned int ts0,
                                                        unsigned int ts1,
                                                        PersistenceDeletionMode mode,
                                                        const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    const entity_desc *edesc = in_obj.get_entity_descriptor();
    if(!edesc->is_persistent()) {
        IFLOG(err(TH_ID, LS_CLO "%s(nclass_id:%d) - not persistent", __func__,
                  edesc->get_nclass_id()))
        return vlg::RetCode_BADARG;
    }
    cdrs_res = do_delete(key, bem, ts0, ts1, mode, in_obj);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::execute_query(const char *sql,
                                                        const entity_manager &bem,
                                                        persistence_query_impl **query_out)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = do_execute_query(bem, sql, query_out);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::destroy_query(persistence_query_impl
                                                        *query,
                                                        bool release_before_destroy)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    if(query) {
        persistence_connection_impl &conn_ref =  query->get_connection();
        persistence_connection_impl *conn_ptr = &conn_ref;
        if(conn_ptr == this) {
            if(release_before_destroy) {
                query->release();
            }
            delete query;
        } else {
            IFLOG(err(TH_ID, LS_TRL
                      "%s(res:%d) - invalid query:%p, should be relased-destroyed using connection:%p",
                      __func__,
                      cdrs_res, query, conn_ptr))
            cdrs_res = vlg::RetCode_BADARG;
        }
    } else {
        cdrs_res = vlg::RetCode_BADARG;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_connection_impl::execute_statement(const char *sql)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = do_execute_statement(sql);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

//-----------------------------
// persistence_query_impl
//-----------------------------

nclass_logger *persistence_query_impl::log_ = NULL;

persistence_query_impl::persistence_query_impl(unsigned int id,
                                               persistence_connection_impl &conn,
                                               const entity_manager &bem) :
    id_(id),
    status_(PersistenceQueryStatus_PREPARED),
    conn_(conn),
    bem_(bem)
{
    log_ = get_nclass_logger("persistence_query_impl");
    IFLOG(trc(TH_ID, LS_CTR "%s(id:%d)", __func__, id))
}
persistence_query_impl::~persistence_query_impl()
{
    IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
}

PersistenceQueryStatus persistence_query_impl::status() const
{
    return status_;
}

const entity_manager &persistence_query_impl::get_em() const
{
    return bem_;
}

unsigned int persistence_query_impl::get_id() const
{
    return id_;
}

persistence_connection_impl &persistence_query_impl::get_connection()
{
    return conn_;
}

vlg::RetCode persistence_query_impl::load_next_entity(unsigned int &ts0_out,
                                                      unsigned int &ts1_out,
                                                      nclass &out_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    const entity_desc *edesc = out_obj.get_entity_descriptor();
    if(!edesc->is_persistent()) {
        IFLOG(err(TH_ID, LS_CLO "%s(nclass_id:%d) - not persistent", __func__,
                  edesc->get_nclass_id()))
        return vlg::RetCode_BADARG;
    }
    cdrs_res = conn_.do_next_entity_from_query(this, ts0_out, ts1_out, out_obj);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_query_impl::release()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    cdrs_res = conn_.do_release_query(this);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

//-----------------------------
// persistence_driver_impl - DRIVER
//-----------------------------

nclass_logger *persistence_driver_impl::log_ = NULL;

vlg::RetCode persistence_driver_impl::load_driver_dyna(const char *drvname,
                                                       persistence_driver_impl **driver)
{
    log_ = get_nclass_logger("persistence_driver_impl");
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    if(!drvname || !strlen(drvname)) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return vlg::RetCode_BADARG;
    }
#ifdef WIN32
    wchar_t w_drvname[VLG_DRV_NAME_LEN] = {0};
    swprintf(w_drvname, VLG_DRV_NAME_LEN, L"blzdri%hs", drvname);
    void *dynalib = vlg::dynamic_lib_open(w_drvname);
#endif
#ifdef __linux
    char slib_name[VLG_DRV_NAME_LEN] = {0};
    sprintf(slib_name, "libblzdri%s.so", drvname);
    void *dynalib = vlg::dynamic_lib_open(slib_name);
#endif
#if defined (__MACH__) || defined (__APPLE__)
    char slib_name[VLG_DRV_NAME_LEN] = {0};
    sprintf(slib_name, "libblzdri%s.dylib", drvname);
    void *dynalib = vlg::dynamic_lib_open(slib_name);
#endif
    if(!dynalib) {
        IFLOG(err(TH_ID, LS_CLO "%s() - failed loading dynamic-lib for driver:%s",
                  __func__, drvname))
        return vlg::RetCode_KO;
    }
    char dri_ep_f[VLG_DRV_NAME_LEN] = {0};
    sprintf(dri_ep_f, "get_pers_driv_%s", drvname);
    load_pers_driver dri_f = (load_pers_driver)vlg::dynamic_lib_load_symbol(
                                 dynalib,
                                 dri_ep_f);
    if(!dri_f) {
        IFLOG(err(TH_ID, LS_CLO
                  "%s() - failed to locate entrypoint in dynamic-lib for driver:%s", __func__,
                  drvname))
        return vlg::RetCode_KO;
    }
    if(!(*driver = dri_f())) {
        IFLOG(err(TH_ID, LS_CLO "%s() - failed to get driver instance for driver:%s",
                  __func__, drvname))
        return vlg::RetCode_KO;
    } else {
        char driv_f_n[VLG_MDL_NAME_LEN] = {0};
        sprintf(driv_f_n, "get_pers_driv_ver_%s", drvname);
        get_pers_driv_version driv_f = (get_pers_driv_version)
                                       vlg::dynamic_lib_load_symbol(
                                           dynalib, driv_f_n);
        IFLOG(inf(TH_ID, LS_DRV"[DYNALOADED]%s", driv_f()))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return vlg::RetCode_OK;
}

persistence_driver_impl::persistence_driver_impl(unsigned int id) :
    id_(id),
    conn_pool_hm_(vlg::sngl_ptr_obj_mng(), vlg::sngl_cstr_obj_mng()),
    nclassid_conn_pool_hm_(vlg::sngl_ptr_obj_mng(), sizeof(unsigned int))
{
    IFLOG(trc(TH_ID, LS_CTR "%s(id:%d)", __func__, id))
}

persistence_driver_impl::~persistence_driver_impl()
{
    IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
}

vlg::RetCode persistence_driver_impl::init()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    RETURN_IF_NOT_OK(conn_pool_hm_.init(HM_SIZE_TINY))
    RETURN_IF_NOT_OK(nclassid_conn_pool_hm_.init(HM_SIZE_MINI))
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return vlg::RetCode_OK;
}

unsigned int persistence_driver_impl::get_id() const
{
    return id_;
}

vlg::RetCode persistence_driver_impl::start_all_pools()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    char conn_pool_name[64];
    persistence_connection_pool *conn_pool = NULL;
    conn_pool_hm_.start_iteration();
    while(!conn_pool_hm_.next(conn_pool_name, &conn_pool)) {
        if((cdrs_res = conn_pool->start())) {
            IFLOG(cri(TH_ID, LS_TRL "%s() - failed to start conn_pool_name:%s.", __func__,
                      conn_pool_name))
            break;
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_driver_impl::add_pool(const char *conn_pool_name,
                                               const char *url,
                                               const char *usr,
                                               const char *psswd,
                                               unsigned int conn_pool_sz,
                                               unsigned int conn_pool_th_max_sz)
{
    IFLOG(trc(TH_ID, LS_OPN
              "%s(conn_pool_name:%s, url:%s, usr:%s, conn_pool_sz:%d, thread_size:%d)",
              __func__,
              conn_pool_name,
              url,
              usr,
              conn_pool_sz,
              conn_pool_th_max_sz))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    persistence_connection_pool *conn_pool = new
    persistence_connection_pool(*this,
                                url,
                                usr,
                                psswd,
                                conn_pool_sz,
                                conn_pool_th_max_sz);
    conn_pool->init();
    conn_pool_hm_.put(conn_pool_name, &conn_pool);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

vlg::RetCode persistence_driver_impl::map_nclassid_to_pool(
    unsigned int nclass_id,
    const char *conn_pool_name)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(nclass_id:%d, conn_pool_name:%s)", __func__,
              nclass_id, conn_pool_name))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    persistence_connection_pool *conn_pool = NULL;
    if(!(cdrs_res = conn_pool_hm_.get(conn_pool_name, &conn_pool))) {
        nclassid_conn_pool_hm_.put(&nclass_id, &conn_pool);
    } else {
        IFLOG(cri(TH_ID, LS_TRL "%s() - conn_pool_name:%s - not defined.", __func__,
                  conn_pool_name))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

persistence_connection_impl *persistence_driver_impl::available_connection(
    unsigned int nclass_id)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(nclass_id:%d)", __func__, nclass_id))
    vlg::RetCode cdrs_res = vlg::RetCode_OK;
    persistence_connection_pool *conn_pool = NULL;
    persistence_connection_impl *conn_out = NULL;
    if(!(cdrs_res = nclassid_conn_pool_hm_.get(&nclass_id, &conn_pool))) {
        conn_out = conn_pool->request_connection();
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - nclass_id:%d - no connection-pool available.",
                  __func__, nclass_id))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d, conn_out:%p)", __func__, cdrs_res,
              conn_out))
    return conn_out;
}

//-----------------------------
// #VER#
//-----------------------------
const char *pers_lib_ver(void)
{
    static char str[] = "lib.blzpers.ver.0.0.0.date:" __DATE__;
    return str;
}

}

