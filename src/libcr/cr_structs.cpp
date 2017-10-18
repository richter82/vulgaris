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

#include "blz_glob_int.h"
#include "cr_structs_rep.h"

namespace vlg {

//-----------------------------
// linked_list
linked_list::linked_list(size_t elem_size) : manager_(elem_size), impl_(NULL)
{}

linked_list::linked_list(obj_mng &obj_mng) : manager_(obj_mng), impl_(NULL)
{}

linked_list::~linked_list()
{
    destroy();
}

RetCode linked_list::init()
{
    return !(impl_ = new linked_list_rep(manager_)) ? RetCode_MEMERR :
           RetCode_OK;
}

RetCode linked_list::destroy()
{
    if(impl_) {
        delete impl_;
    }
    return RetCode_OK;
}

RetCode linked_list::at(uint32_t idx, void *copy) const
{
    if(idx >= impl_->elems_cnt()) {
        return RetCode_BADIDX;
    }
    impl_->r_at(idx, copy);
    return RetCode_OK;
}

const void *linked_list::at(uint32_t idx) const
{
    if(idx >= impl_->elems_cnt()) {
        return NULL;
    }
    return impl_->r_at(idx);
}

RetCode linked_list::clear()
{
    impl_->r_clear();
    return RetCode_OK;
}

RetCode linked_list::is_empty(bool &empty) const
{
    empty = !impl_->elems_cnt();
    return RetCode_OK;
}

RetCode linked_list::size(uint32_t &size) const
{
    size = impl_->elems_cnt();
    return RetCode_OK;
}

uint32_t linked_list::size() const
{
    return impl_->elems_cnt();;
}

RetCode linked_list::push_back(const void *ptr)
{
    return impl_->r_push_bk(ptr);
}

RetCode linked_list::push_front(const void *ptr)
{
    return impl_->r_push_fr(ptr);
}

RetCode linked_list::pop_front(void *copy)
{
    if(!impl_->elems_cnt()) {
        return RetCode_KO;
    }
    impl_->r_pop_fr(copy);
    return RetCode_OK;
}

RetCode linked_list::last(void *copy) const
{
    if(!impl_->elems_cnt()) {
        return RetCode_KO;
    }
    impl_->r_last(copy);
    return RetCode_OK;
}

RetCode linked_list::first(void *copy) const
{
    if(!impl_->elems_cnt()) {
        return RetCode_KO;
    }
    impl_->r_first(copy);
    return RetCode_OK;
}

RetCode linked_list::next(void *copy)
{
    return impl_->r_next(copy) ? RetCode_OK : RetCode_KO;
}

void linked_list::start_iteration()
{
    impl_->r_start_it();
}

RetCode linked_list::remove_in_iteration()
{
    return impl_->r_rem_in_iter() ? RetCode_OK : RetCode_KO;
}

RetCode linked_list::enum_elements(list_enum_func enum_f, void *ud) const
{
    impl_->r_enum(*this, enum_f, ud);
    return RetCode_OK;
}

RetCode linked_list::enum_elements_breakable(list_enum_func_breakable enum_f,
                                             void *ud) const
{
    impl_->r_enum_br(*this, enum_f, ud);
    return RetCode_OK;
}

RetCode linked_list::insert_at(uint32_t idx, const void *ptr)
{
    if(!impl_->elems_cnt()) {
        return RetCode_BADSTTS;
    } else if(idx >= impl_->elems_cnt()) {
        return RetCode_BADIDX;
    }
    return impl_->r_ins_at(idx, ptr);
}

RetCode linked_list::pop_back(void *copy)
{
    if(!impl_->elems_cnt()) {
        return RetCode_KO;
    }
    impl_->r_pop_bk(copy);
    return RetCode_OK;
}

RetCode linked_list::remove_at(uint32_t idx, void *copy)
{
    if(idx >= impl_->elems_cnt()) {
        return RetCode_BADIDX;
    }
    impl_->r_rem_at(idx, copy);
    return RetCode_OK;
}

RetCode linked_list::set_at(uint32_t idx, const void *ptr)
{
    if(idx >= impl_->elems_cnt()) {
        return RetCode_BADIDX;
    }
    return impl_->r_set_at(idx, ptr);
}


const void *linked_list::operator [](uint32_t idx)
{
    return at(idx);
}

//-----------------------------
// hash_map
hash_map::hash_map(size_t elem_size,
                   size_t key_size) :
    elem_manager_(elem_size),
    key_manager_(key_size),
    impl_(NULL)
{}

hash_map::hash_map(obj_mng &elem_manager,
                   obj_mng &key_manager) :
    elem_manager_(elem_manager),
    key_manager_(key_manager),
    impl_(NULL)
{}

hash_map::hash_map(size_t elem_size,
                   obj_mng &key_manager) :
    elem_manager_(elem_size),
    key_manager_(key_manager),
    impl_(NULL)
{}

hash_map::hash_map(obj_mng &elem_manager,
                   size_t key_size) :
    elem_manager_(elem_manager),
    key_manager_(key_size),
    impl_(NULL)
{}

hash_map::~hash_map()
{
    destroy();
}

RetCode hash_map::init(uint32_t hash_size)
{
    if(hash_size > HM_SIZE_MEGA) {
        return RetCode_OVRSZ;
    }
    if(!(impl_ = new hash_map_rep(hash_size,
                                  elem_manager_,
                                  key_manager_))) {
        return RetCode_MEMERR;
    }
    return impl_->r_init();
}

RetCode hash_map::destroy()
{
    if(impl_) {
        delete impl_;
    }
    return RetCode_OK;
}

RetCode hash_map::clear()
{
    impl_->r_clear();
    return RetCode_OK;
}

RetCode hash_map::is_empty(bool &empty) const
{
    empty = !impl_->elems_cnt();
    return RetCode_OK;
}

RetCode hash_map::size(uint32_t &size) const
{
    size = impl_->elems_cnt();
    return RetCode_OK;
}

uint32_t hash_map::size() const
{
    return impl_->elems_cnt();
}

RetCode hash_map::get(const void *key, void *copy) const
{
    if(!key) {
        return RetCode_BADARG;
    }
    return impl_->r_get(key, copy);
}

const void *hash_map::get(const void *key) const
{
    if(!key) {
        return NULL;
    }
    return impl_->r_get(key);
}

RetCode hash_map::put(const void *key, const void *ptr)
{
    if(!key || !ptr) {
        return RetCode_BADARG;
    }
    return impl_->r_put(key, ptr);
}

RetCode hash_map::contains_key(const void *key) const
{
    if(!key) {
        return RetCode_BADARG;
    }
    return impl_->r_cnt_key(key);
}

RetCode hash_map::remove(const void *key, void *copy)
{
    if(!key) {
        return RetCode_BADARG;
    }
    return impl_->r_rem(key, copy);
}

RetCode hash_map::next(void *key_copy, void *elem_copy)
{
    return impl_->r_next(key_copy, elem_copy) ? RetCode_OK : RetCode_KO;
}

void hash_map::start_iteration()
{
    impl_->r_start_it();
}

RetCode hash_map::remove_in_iteration()
{
    return impl_->r_rem_in_it() ? RetCode_OK : RetCode_KO;
}

RetCode hash_map::enum_elements(hmap_enum_func enum_f, void *ud) const
{
    impl_->r_enum(*this, enum_f, ud);
    return RetCode_OK;
}

RetCode hash_map::enum_elements_breakable(hmap_enum_func_breakable enum_f,
                                          void *ud) const
{
    impl_->r_enum_br(*this, enum_f, ud);
    return RetCode_OK;
}

}
