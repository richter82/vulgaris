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

#include "vlg_globint.h"
#include "cr_structs_rep.h"

namespace vlg {

//-----------------------------
// synch_hash_map
//-----------------------------
synch_hash_map::synch_hash_map(size_t elem_size, size_t key_size)
    : elem_manager_(elem_size), key_manager_(key_size), impl_(NULL)
{}

synch_hash_map::synch_hash_map(obj_mng &elem_manager, obj_mng &key_manager)
    : elem_manager_(elem_manager), key_manager_(key_manager), impl_(NULL)
{}

synch_hash_map::synch_hash_map(size_t elem_size, obj_mng &key_manager)
    : elem_manager_(elem_size), key_manager_(key_manager), impl_(NULL)
{}

synch_hash_map::synch_hash_map(obj_mng &elem_manager, size_t key_size)
    : elem_manager_(elem_manager), key_manager_(key_size), impl_(NULL)
{}

synch_hash_map::~synch_hash_map()
{
    destroy();
}

RetCode synch_hash_map::init(uint32_t hash_size, pthread_rwlockattr_t *attr)
{
    if(hash_size > HM_SIZE_MEGA) {
        return RetCode_OVRSZ;
    }
    CHK_PTH_ERR_2(pthread_rwlock_init, &lock_, attr)
    if(!(impl_ = new hash_map_rep(hash_size,
                                  elem_manager_,
                                  key_manager_))) {
        return RetCode_MEMERR;
    }
    return impl_->r_init();
}

RetCode synch_hash_map::destroy()
{
    if(impl_) {
        delete impl_;
        CHK_PTH_ERR_1(pthread_rwlock_destroy, &lock_)
    }
    return RetCode_OK;
}

RetCode synch_hash_map::clear()
{
    CHK_PTH_ERR_1(pthread_rwlock_wrlock, &lock_)
    impl_->r_clear();
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return RetCode_OK;
}

RetCode synch_hash_map::is_empty(bool &empty) const
{
    CHK_PTH_ERR_1(pthread_rwlock_rdlock, &lock_)
    empty = !impl_->elems_cnt();
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return RetCode_OK;
}

RetCode synch_hash_map::size(uint32_t &size) const
{
    CHK_PTH_ERR_1(pthread_rwlock_rdlock, &lock_)
    size = impl_->elems_cnt();
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return RetCode_OK;
}

uint32_t synch_hash_map::size() const
{
    return impl_->elems_cnt();
}

RetCode synch_hash_map::next(void *key_copy, void *elem_copy)
{
    return impl_->r_next(key_copy, elem_copy) ? RetCode_OK : RetCode_KO;
}

void synch_hash_map::start_iteration()
{
    impl_->r_start_it();
}

RetCode synch_hash_map::remove_in_iteration()
{
    return impl_->r_rem_in_it() ? RetCode_OK : RetCode_KO;
}

RetCode synch_hash_map::enum_elements_safe_read(hmapts_enum_func enum_f,
                                                void *ud) const
{
    CHK_PTH_ERR_1(pthread_rwlock_rdlock, &lock_)
    impl_->r_enum_ts(*this, enum_f, ud);
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return RetCode_OK;
}

RetCode synch_hash_map::enum_elements_safe_write(hmapts_enum_func enum_f,
                                                 void *ud) const
{
    CHK_PTH_ERR_1(pthread_rwlock_wrlock, &lock_)
    impl_->r_enum_ts(*this, enum_f, ud);
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return RetCode_OK;
}

RetCode synch_hash_map::enum_elements_breakable_safe_read(
    hmapts_enum_func_breakable enum_f, void *ud) const
{
    CHK_PTH_ERR_1(pthread_rwlock_rdlock, &lock_)
    impl_->r_enum_ts_br(*this, enum_f, ud);
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return RetCode_OK;
}

RetCode synch_hash_map::enum_elements_breakable_safe_write(
    hmapts_enum_func_breakable enum_f, void *ud) const
{
    CHK_PTH_ERR_1(pthread_rwlock_wrlock, &lock_)
    impl_->r_enum_ts_br(*this, enum_f, ud);
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return RetCode_OK;
}

RetCode synch_hash_map::get(const void *key, void *copy) const
{
    if(!key) {
        return RetCode_BADARG;
    }
    CHK_PTH_ERR_1(pthread_rwlock_rdlock, &lock_)
    RetCode res = impl_->r_get(key, copy);
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return res;
}

RetCode synch_hash_map::put(const void *key, const void *ptr)
{
    if(!key || !ptr) {
        return RetCode_BADARG;
    }
    CHK_PTH_ERR_1(pthread_rwlock_wrlock, &lock_)
    RetCode res = impl_->r_put(key, ptr);
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return res;
}

RetCode synch_hash_map::contains_key(const void *key) const
{
    if(!key) {
        return RetCode_BADARG;
    }
    CHK_PTH_ERR_1(pthread_rwlock_rdlock, &lock_)
    RetCode res = impl_->r_cnt_key(key);
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return res;
}

RetCode synch_hash_map::remove(const void *key, void *copy)
{
    if(!key) {
        return RetCode_BADARG;
    }
    CHK_PTH_ERR_1(pthread_rwlock_wrlock, &lock_)
    RetCode res = impl_->r_rem(key, copy);
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return res;
}

RetCode synch_hash_map::lock_read() const
{
    CHK_PTH_ERR_1(pthread_rwlock_rdlock, &lock_)
    return RetCode_OK;
}

RetCode synch_hash_map::lock_write() const
{
    CHK_PTH_ERR_1(pthread_rwlock_wrlock, &lock_)
    return RetCode_OK;
}

RetCode synch_hash_map::unlock() const
{
    CHK_PTH_ERR_1(pthread_rwlock_unlock, &lock_)
    return RetCode_OK;
}

//-----------------------------
// blocking_queue
blocking_queue::blocking_queue(size_t elemsize) :
    manager_(elemsize),
    impl_(NULL),
    wt_prod_(0),
    wt_cons_(0),
    mon_()
{}

blocking_queue::blocking_queue(obj_mng &elem_manager) :
    manager_(elem_manager),
    impl_(NULL),
    wt_prod_(0),
    wt_cons_(0),
    mon_()
{}

blocking_queue::~blocking_queue()
{
    destroy();
}

RetCode blocking_queue::init(uint32_t capacity)
{
    return !(impl_ = new blocking_queue_rep(capacity, manager_))
           ? RetCode_MEMERR : RetCode_OK;
}

RetCode blocking_queue::destroy()
{
    if(impl_) {
        delete impl_;
    }
    return RetCode_OK;
}

RetCode blocking_queue::is_empty(bool &empty) const
{
    CHK_MON_ERR_0(lock)
    empty = !impl_->elems_cnt();
    CHK_MON_ERR_0(unlock)
    return RetCode_OK;
}

RetCode blocking_queue::size(uint32_t &size) const
{
    CHK_MON_ERR_0(lock)
    size = impl_->elems_cnt();
    CHK_MON_ERR_0(unlock)
    return RetCode_OK;
}

uint32_t blocking_queue::size() const
{
    return impl_->elems_cnt();
}

RetCode blocking_queue::remain_capacity(uint32_t &rcpty) const
{
    CHK_MON_ERR_0(lock)
    rcpty = impl_->r_rem_cpcty();
    CHK_MON_ERR_0(unlock)
    return RetCode_OK;
}

uint32_t blocking_queue::remain_capacity() const
{
    uint32_t rcpty = 0;
    mon_.lock();
    rcpty = impl_->r_rem_cpcty();
    mon_.unlock();
    return rcpty;
}

RetCode blocking_queue::clear()
{
    CHK_MON_ERR_0(lock)
    impl_->r_clear();
    if(wt_prod_) {
        wt_prod_ = 0;
        CHK_MON_ERR_0(notify_all)
    }
    CHK_MON_ERR_0(unlock)
    return RetCode_OK;
}

RetCode blocking_queue::get(void *copy)
{
    CHK_MON_ERR_0(lock)
    while(!impl_->elems_cnt()) {
        wt_cons_++;
        CHK_MON_ERR_0(wait)
    }
    impl_->r_deque(copy);
    if(wt_prod_) {
        wt_prod_ = 0;
        CHK_MON_ERR_0(notify_all)
    }
    CHK_MON_ERR_0(unlock)
    return RetCode_OK;
}

RetCode blocking_queue::get(time_t sec, long nsec, void *copy)
{
    RetCode res = RetCode_OK;
    CHK_MON_ERR_0(lock)
    if(!nsec && !sec) {
        if(impl_->elems_cnt()) {
            impl_->r_deque(copy);
        } else {
            res = RetCode_EMPTY;
        }
    } else {
        while(!impl_->elems_cnt()) {
            wt_cons_++;
            int pthres;
            if((pthres = mon_.wait(sec, nsec))) {
                if(pthres == ETIMEDOUT) {
                    wt_cons_--;
                    CHK_MON_ERR_0(unlock)
                    return RetCode_TIMEOUT;
                } else {
                    return RetCode_PTHERR;
                }
            }
        }
        impl_->r_deque(copy);
        if(wt_prod_) {
            wt_prod_ = 0;
            CHK_MON_ERR_0(notify_all)
        }
    }
    CHK_MON_ERR_0(unlock)
    return res;
}

RetCode blocking_queue::peek(time_t sec, long nsec, void *copy)
{
    RetCode res = RetCode_OK;
    CHK_MON_ERR_0(lock)
    if(!nsec && !sec) {
        if(impl_->elems_cnt()) {
            impl_->r_peek(copy);
        } else {
            res = RetCode_EMPTY;
        }
    } else {
        while(!impl_->elems_cnt()) {
            wt_cons_++;
            int pthres;
            if((pthres = mon_.wait(sec, nsec))) {
                if(pthres == ETIMEDOUT) {
                    wt_cons_--;
                    CHK_MON_ERR_0(unlock)
                    return RetCode_TIMEOUT;
                } else {
                    return RetCode_PTHERR;
                }
            }
        }
        impl_->r_peek(copy);
    }
    CHK_MON_ERR_0(unlock)
    return res;
}

RetCode blocking_queue::put(time_t sec, long nsec, const void *ptr)
{
    RetCode res = RetCode_OK;
    if(!ptr) {
        return RetCode_BADARG;
    }
    CHK_MON_ERR_0(lock)
    if(!nsec && !sec) {
        if(impl_->r_rem_cpcty()) {
            res = impl_->r_enque(ptr);
        } else {
            res = RetCode_QFULL;
        }
    } else {
        while(!impl_->r_rem_cpcty()) {
            wt_prod_++;
            int pthres;
            if((pthres = mon_.wait(sec, nsec))) {
                if(pthres == ETIMEDOUT) {
                    wt_prod_--;
                    CHK_MON_ERR_0(unlock)
                    return RetCode_TIMEOUT;
                } else {
                    return RetCode_PTHERR;
                }
            }
        }
        res = impl_->r_enque(ptr);
        if(wt_cons_) {
            wt_cons_ = 0;
            CHK_MON_ERR_0(notify_all)
        }
    }
    CHK_MON_ERR_0(unlock)
    return res;
}

RetCode blocking_queue::put(const void *ptr)
{
    RetCode res = RetCode_OK;
    if(!ptr) {
        return RetCode_BADARG;
    }
    CHK_MON_ERR_0(lock)
    while(!impl_->r_rem_cpcty()) {
        wt_prod_++;
        CHK_MON_ERR_0(wait)
    }
    res = impl_->r_enque(ptr);
    if(wt_cons_) {
        if(wt_cons_ > 1) {
            CHK_MON_ERR_0(notify_all)
        } else {
            CHK_MON_ERR_0(notify)
        }
        wt_cons_ = 0;
    }
    CHK_MON_ERR_0(unlock)
    return res;
}

RetCode blocking_queue::peek(void *copy)
{
    CHK_MON_ERR_0(lock)
    while(!impl_->elems_cnt()) {
        wt_cons_++;
        CHK_MON_ERR_0(wait)
    }
    impl_->r_peek(copy);
    CHK_MON_ERR_0(unlock)
    return RetCode_OK;
}

}
