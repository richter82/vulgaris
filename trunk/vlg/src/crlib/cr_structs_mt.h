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

#ifndef CR_STRUCTS_MT_H_
#define CR_STRUCTS_MT_H_
#include "cr_structs.h"
#include "cr_concurrent.h"

namespace blaze {

class synch_linked_list;

/**
@param list
@param ptr
@param ud
@return
*/
typedef void(*listts_enum_func)(const synch_linked_list &list,
                                const void *ptr,
                                void *ud);

/**
@param list
@param ptr
@param ud
@param brk
@return
*/
typedef void(*listts_enum_func_breakable)(const synch_linked_list &list,
                                          const void *ptr,
                                          void *ud,
                                          bool &brk);


/** @brief Basic thread safe Linked-List class.

*/
class synch_linked_list {
    public:

        /**
        @param elem_size
        */
        explicit synch_linked_list(size_t elem_size);

        /**
        @param obj_mng
        */
        explicit synch_linked_list(obj_mng &obj_mng);

        /**
        */
        ~synch_linked_list();

    private:

        /**
        @return
        */
        RetCode destroy();

    public:

        /**
        @param attr
        @return
        */
        RetCode init(pthread_rwlockattr_t *attr = NULL);

        /**
        @param idx
        @param copy
        @return
        */
        RetCode at(uint32_t idx,
                   void *copy) const;

        /**
        @return
        */
        RetCode clear();

        /**
        @param empty
        @return
        */
        RetCode is_empty(bool &empty) const;

        /**
        @param ptr
        @return
        */
        RetCode push_back(const void *ptr);

        /**
        @param ptr
        @return
        */
        RetCode push_front(const void *ptr);

        /**
        @param copy
        @return
        */
        RetCode pop_front(void *copy);

        /**
        @param copy
        @return
        */
        RetCode pop_back(void *copy);

        /**
        @param copy
        @return
        */
        RetCode last(void *copy) const;

        /**
        @param copy
        @return
        */
        RetCode first(void *copy) const;

        /**
        @param copy
        @return
        */
        RetCode next(void *copy);

        /**
        */
        void start_iteration();

        /**
        @return
        */
        RetCode remove_in_iteration();

        /**
        @param idx
        @param copy
        @return
        */
        RetCode remove_at(uint32_t idx,
                          void *copy);

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode enum_elements_safe_read(listts_enum_func enum_f,
                                        void *ud) const;

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode enum_elements_safe_write(listts_enum_func enum_f,
                                         void *ud) const;

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode enum_elements_breakable_safe_read(listts_enum_func_breakable enum_f,
                                                  void *ud) const;

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode enum_elements_breakable_safe_write(listts_enum_func_breakable enum_f,
                                                   void *ud) const;

        /**
        @param idx
        @param ptr
        @return
        */
        RetCode insert_at(uint32_t idx,
                          const void *ptr);

        /**
        @param idx
        @param ptr
        @return
        */
        RetCode set_at(uint32_t idx,
                       const void *ptr);

        /**
        @param size
        @return
        */
        RetCode size(uint32_t &size) const;

        /**
        @return
        */
        uint32_t size() const;

        /**
        @return
        */
        RetCode lock_read() const;

        /**
        @return
        */
        RetCode lock_write() const;

        /**
        @return
        */
        RetCode unlock() const;

    private:
        obj_mng                     manager_;
        linked_list_rep            *impl_;
        mutable pthread_rwlock_t    lock_;
};


class synch_hash_map;

/**
@param map
@param key
@param ptr
@param ud
@return
*/
typedef void(*hmapts_enum_func)(const synch_hash_map &map,
                                const void *key,
                                const void *ptr,
                                void *ud);

/**
@param map
@param key
@param ptr
@param ud
@param brk
@return
*/
typedef void(*hmapts_enum_func_breakable)(const synch_hash_map &map,
                                          const void *key,
                                          const void *ptr,
                                          void *ud,
                                          bool &brk);

/** @brief Basic thread safe Hash-Map class.

*/
class synch_hash_map {
    public:

        /**
        @param elem_size
        @param key_size
        */
        explicit synch_hash_map(size_t elem_size,
                                size_t key_size);

        /**
        @param elem_manager
        @param key_manager
        */
        explicit synch_hash_map(obj_mng &elem_manager,
                                obj_mng &key_manager);

        /**
        @param elem_size
        @param key_manager
        */
        explicit synch_hash_map(size_t elem_size,
                                obj_mng &key_manager);

        /**
        @param elem_manager
        @param key_size
        */
        explicit synch_hash_map(obj_mng &elem_manager,
                                size_t key_size);

        /**
        */
        ~synch_hash_map();

    private:

        /**
        @return
        */
        RetCode destroy();

    public:

        /**
        @param hash_size
        @param attr
        @return
        */
        RetCode init(uint32_t hash_size,
                     pthread_rwlockattr_t *attr = NULL);

        /**
        @return
        */
        RetCode clear();

        /**
        @param key
        @param copy
        @return RetCode_OK if the specified key is mapped,
        RetCode_KO if this map contains no mapping for the key or
        RetCode_BDARG if key is NULL.
        */
        RetCode contains_key(const void *key) const;

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode enum_elements_safe_read(hmapts_enum_func enum_f,
                                        void *ud) const;

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode enum_elements_safe_write(hmapts_enum_func enum_f,
                                         void *ud) const;

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode enum_elements_breakable_safe_read(hmapts_enum_func_breakable enum_f,
                                                  void *ud) const;

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode enum_elements_breakable_safe_write(hmapts_enum_func_breakable
                                                   enum_f,
                                                   void *ud) const;
        /**
        @param key
        @param copy
        @return RetCode_OK if the specified key is mapped,
        RetCode_KO if this map contains no mapping for the key, or
        RetCode_BDARG if key is NULL.
        */
        RetCode get(const void *key,
                    void *copy) const;

        /**
        @param empty
        @return
        */
        RetCode is_empty(bool &empty) const;

        /**
        @param key_copy
        @param elem_copy
        @return
        */
        RetCode next(void *key_copy,
                     void *elem_copy);

        /**
        @param key
        @param copy
        @return
        */
        RetCode remove(const void *key,
                       void *copy);

        /**
        @return
        */
        RetCode remove_in_iteration();

        /**
        @param key
        @param ptr
        @return
        */
        RetCode put(const void *key,
                    const void *ptr);

        /**
        */
        void start_iteration();

        /**
        @param size
        @return
        */
        RetCode size(uint32_t &size) const;

        /**
        @return
        */
        uint32_t size() const;

        /**
        @return
        */
        RetCode lock_read() const;

        /**
        @return
        */
        RetCode lock_write() const;

        /**
        @return
        */
        RetCode unlock() const;

    private:
        obj_mng                     elem_manager_,
                                    key_manager_;
        hash_map_rep               *impl_;
        mutable pthread_rwlock_t    lock_;
};

/** @brief Basic Blocking-Queue class.

*/
class blocking_queue_rep;
class blocking_queue {
    public:

        /**
        @param elemsize
        */
        explicit blocking_queue(size_t elemsize);

        /**
        @param elem_manager
        */
        explicit blocking_queue(obj_mng &elem_manager);

        /**
        */
        ~blocking_queue();

    private:

        /**
        @return
        */
        RetCode destroy();

    public:

        /**
        @param capacity
        @return
        */
        RetCode init(uint32_t capacity = 0);

        /**
        @param empty
        @return
        */
        RetCode is_empty(bool &empty) const;

        /**
        @param size
        @return
        */
        RetCode size(uint32_t &size) const;

        /**
        @return
        */
        uint32_t size() const;

        /**
        @param rcpty
        @return
        */
        RetCode remain_capacity(uint32_t &rcpty) const;

        /**
        @return
        */
        uint32_t remain_capacity() const;

        /**
        @return
        */
        RetCode clear();

        /**
        waits indefinitely until an element becomes available.
        @param copy
        @return RetCode_OK when an element becomes available.
        */
        RetCode get(void *copy);

        /**
        same as above, except that this method waits at most for sec/nsec seconds
        before return with RetCode_TMOUT code.
        Use sec=0 and nsec=0 if you want this method to return immediatly
        with rescode RetCode_EPTY if no elements are available.
        @param sec
        @param nsec
        @param copy
        @return RetCode_OK when an element becomes available. RetCode_TMOUT if no
        elements became available in sec/nsec after invocation. RetCode_EPTY if
        no elements are available and sec=0 and nsec=0.
        */
        RetCode get(time_t sec,
                    long nsec,
                    void *copy);

        /**
        @param ptr
        @return
        */
        RetCode put(const void *ptr);

        /**
        @param sec
        @param nsec
        @param ptr
        @return
        */
        RetCode put(time_t sec,
                    long nsec,
                    const void *ptr);

        /**
        @param copy
        @return
        */
        RetCode peek(void *copy);

        /**
        @param sec
        @param nsec
        @param ptr
        @return
        */
        RetCode peek(time_t sec,
                     long nsec,
                     void *copy);

    private:
        obj_mng                 manager_;
        blocking_queue_rep     *impl_;
        uint32_t                wt_prod_, wt_cons_;
        mutable synch_monitor   mon_;
};

template <typename T>
class shared_ptr_obj_mng_priv : public obj_mng {
    private:
        static void *shared_ptr_alloc_func(size_t type_size, const void *copy) {
            shared_pointer<T *> &sh_ptr_cpy = *(shared_pointer<T *> *)(copy);
            shared_pointer<T *> *new_sh_ptr = new shared_pointer<T *>(sh_ptr_cpy);
            return new_sh_ptr;
        }

        static void shared_ptr_dealloc_func(void *ptr) {
            shared_pointer<T *> *sh_ptr = (shared_pointer<T *> *)(ptr);
            delete sh_ptr;
        }

        static void *shared_ptr_cpy_func(void *copy, const void *ptr,
                                         size_t type_size) {
            shared_pointer<T *> &sh_ptr_cpy = *(shared_pointer<T *> *)(copy);
            shared_pointer<T *> &sh_ptr_ptr = *(shared_pointer<T *> *)(ptr);
            sh_ptr_cpy = sh_ptr_ptr;
            return copy;
        }

    protected:
        shared_ptr_obj_mng_priv() : obj_mng(0,
                                                shared_ptr_alloc_func,
                                                shared_ptr_dealloc_func,
                                                memcmp,
                                                shared_ptr_cpy_func,
                                                MurmurHash3_x86_32) {}
};

/** @brief shared pointer object manager class.
*/
template <typename T>
class shared_ptr_queue_obj_mng : public
    shared_ptr_obj_mng_priv<shared_pointer<T>> {};

}

#endif