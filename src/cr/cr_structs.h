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

#ifndef CR_STRUCTS_H_
#define CR_STRUCTS_H_
#include "vlg.h"

/**
MurmurHash3 was written by Austin Appleby, and is placed in the public
domain. The author hereby disclaims copyright to this source code.
*/
void MurmurHash3_x86_32(const void *key,
                        int len,
                        uint32_t seed,
                        void *out);

namespace vlg {

/**
@param type_size
@param copy
@return
*/
typedef void *(*alloc_func)(size_t type_size,
                            const void *copy);

/**
@param ptr
@return
*/
typedef void(*dealloc_func)(void *ptr);

/**
@param copy
@param ptr
@param type_size
@return
*/
typedef void *(*cpy_func)(void *copy,
                          const void *ptr,
                          size_t type_size);

/**
@param obj1
@param obj2
@param len
@return
*/
typedef int(*cmp_func)(const void *obj1,
                       const void *obj2,
                       size_t len);

/**
@param key
@param len
@param seed
@param out
@return
*/
typedef void(*hash_func)(const void *key,
                         int len,
                         uint32_t seed,
                         void *out);

/** @brief Object Manager class.

*/
class obj_mng_impl;
class obj_mng {
    public:

        /**
        @param type_size
        */
        explicit obj_mng(size_t type_size);

        /**
        @param other
        */
        explicit obj_mng(const obj_mng &other);

        /**
        @param type_size
        @param alloc_f
        @param dealloc_f
        @param cmp_f
        @param copy_f
        @param hash_f
        */
        explicit obj_mng(size_t type_size,
                         alloc_func alloc_f,
                         dealloc_func dealloc_f,
                         cmp_func cmp_f,
                         cpy_func copy_f,
                         hash_func hash_f);
        /**
        */
        ~obj_mng();

        /**
        @param copy
        @return
        */
        void *new_obj(const void *copy)             const;

        /**
        @param ptr
        */
        void del_obj(void *ptr)                     const;

        /**
        @param copy
        @param ptr
        @param can_realloc
        @return
        */
        void *copy_obj(void *copy,
                       const void *ptr,
                       bool can_realloc = false)    const;

        /**
        @param ptr1
        @param ptr2
        @return
        */
        int cmp_obj(const void *ptr1,
                    const void *ptr2)               const;

        /**
        @param ptr1
        @param out
        */
        void hash_obj(const void *ptr,
                      void *out)                    const;

    private:
        obj_mng_impl *impl_;
};

/** @brief Pointer Object Manager class.

*/
class ptr_obj_mng : public obj_mng {
        friend ptr_obj_mng &sngl_ptr_obj_mng();
    public:

        /**
        */
        ptr_obj_mng();

    private:
        static ptr_obj_mng *instance_;
};

/** @brief c-string Object Manager class.

*/
class cstr_obj_mng : public obj_mng {
        friend cstr_obj_mng &sngl_cstr_obj_mng();
    public:

        /**
        */
        cstr_obj_mng();

    private:
        static cstr_obj_mng *instance_;
};

/** @brief Integer Object Manager class.

*/
class int_obj_mng : public obj_mng {
        friend int_obj_mng &sngl_impl_obj_mng();
    public:

        /**
        */
        int_obj_mng();

    private:
        static int_obj_mng *instance_;
};

/**
@return
*/
ptr_obj_mng &sngl_ptr_obj_mng();

/**
@return
*/
cstr_obj_mng &sngl_cstr_obj_mng();

/**
@return
*/
int_obj_mng &sngl_impl_obj_mng();

class linked_list;

/**
@param list
@param ptr
@param ud
@return
*/
typedef void(*list_enum_func)(const linked_list &list,
                              const void *ptr,
                              void *ud);

/**
@param list
@param ptr
@param ud
@param brk
@return
*/
typedef void(*list_enum_func_breakable)(const linked_list &list,
                                        const void *ptr,
                                        void *ud,
                                        bool &brk);

/** @brief Basic not thread safe Linked-List class.
*/
class linked_list_rep;
class linked_list {
    public:

        /**
        @param elem_size
        */
        explicit linked_list(size_t elem_size);

        /**
        @param obj_mng
        */
        explicit linked_list(obj_mng &obj_mng);

        /**
        */
        ~linked_list();

    private:

        /**
        @return
        */
        RetCode        destroy();

    public:

        /**
        @return
        */
        RetCode        init();

        /**
        @param idx
        @param copy
        @return
        */
        RetCode        at(uint32_t idx,
                          void *copy)       const;

        /**
        @param idx
        @return
        */
        const void     *at(uint32_t idx)    const;

        /**
        @return
        */
        RetCode        clear();

        /**
        @param empty
        @return
        */
        RetCode        is_empty(bool &empty)    const;

        /**
        @param ptr
        @return
        */
        RetCode        push_back(const void *ptr);

        /**
        @param ptr
        @return
        */
        RetCode        push_front(const void *ptr);

        /**
        @param copy
        @return
        */
        RetCode        pop_front(void *copy);

        /**
        @param copy
        @return
        */
        RetCode        pop_back(void *copy);

        /**
        @param copy
        @return
        */
        RetCode        last(void *copy)     const;

        /**
        @param copy
        @return
        */
        RetCode        first(void *copy)    const;

        /**
        @param copy
        @return
        */
        RetCode        next(void *copy);

        /**
        */
        void           start_iteration();

        /**
        @return
        */
        RetCode        remove_in_iteration();

        /**
        @param idx
        @param copy
        @return
        */
        RetCode        remove_at(uint32_t idx,
                                 void *copy);

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode        enum_elements(list_enum_func enum_f,
                                     void *ud)  const;

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode        enum_elements_breakable(list_enum_func_breakable enum_f,
                                               void *ud)    const;

        /**
        @param idx
        @param ptr
        @return
        */
        RetCode        insert_at(uint32_t idx,
                                 const void *ptr);

        /**
        @param idx
        @param ptr
        @return
        */
        RetCode        set_at(uint32_t idx,
                              const void *ptr);

        /**
        @param size
        @return
        */
        RetCode        size(uint32_t &size)     const;

        /**
        @return
        */
        uint32_t    size()                      const;

    public:
        const void     *operator[](uint32_t idx);

    private:
        obj_mng             manager_;
        linked_list_rep    *impl_;
};

#define HM_SIZE_TINY    23U
#define HM_SIZE_NANO    71U
#define HM_SIZE_MICRO   101U
#define HM_SIZE_MINI    1031U
#define HM_SIZE_SMALL   10037U
#define HM_SIZE_MIDI    33013U
#define HM_SIZE_NORM    65557U
#define HM_SIZE_LARGE   130343U
#define HM_SIZE_XLARGE  257177U
#define HM_SIZE_BIG     517411U
#define HM_SIZE_HUGE    1031117U
#define HM_SIZE_MEGA    9999991U

class hash_map;

/**
@param map
@param key
@param ptr
@param ud
@return
*/
typedef void(*hmap_enum_func)(const hash_map &map,
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
typedef void(*hmap_enum_func_breakable)(const hash_map &map,
                                        const void *key,
                                        const void *ptr,
                                        void *ud,
                                        bool &brk);

/** @brief Basic not thread safe Hash-Map class.
*/
class hash_map_rep;
class hash_map {
    public:

        /**
        @param elem_size
        @param key_size
        */
        explicit hash_map(size_t elem_size,
                          size_t key_size);

        /**
        @param elem_manager
        @param key_manager
        */
        explicit hash_map(obj_mng &elem_manager,
                          obj_mng &key_manager);

        /**
        @param elem_size
        @param key_manager
        */
        explicit hash_map(size_t elem_size,
                          obj_mng &key_manager);

        /**
        @param elem_manager
        @param key_size
        */
        explicit hash_map(obj_mng &elem_manager,
                          size_t key_size);

        /**
        */
        ~hash_map();

    private:

        /**
        @return
        */
        RetCode     destroy();

    public:

        /**
        @param hash_size
        @return
        */
        RetCode     init(uint32_t hash_size);

        /**
        @return
        */
        RetCode     clear();

        /**
        @param key
        @param copy
        @return RetCode_OK if the specified key is mapped,
        RetCode_KO if this map contains no mapping for the key, or
        RetCode_BDARG if key is NULL.
        */
        RetCode     contains_key(const void *key)        const;

        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode     enum_elements(hmap_enum_func enum_f,
                                  void *ud)              const;
        /**
        @param enum_f
        @param ud
        @return
        */
        RetCode     enum_elements_breakable(hmap_enum_func_breakable enum_f,
                                            void *ud)    const;
        /**
        @param key
        @param copy
        @return RetCode_OK if the specified key is mapped,
        RetCode_KO if this map contains no mapping for the key, or
        RetCode_BDARG if key is NULL.
        */
        RetCode     get(const void *key,
                        void *copy)          const;

        /**
        @param key
        @return a pointer to the element or
        NULL if this map contains no mapping for the key
        */
        const void  *get(const void *key)       const;

        /**
        @param empty
        @return
        */
        RetCode     is_empty(bool &empty)       const;

        /**
        @param key_copy
        @param elem_copy
        @return
        */
        RetCode     next(void *key_copy,
                         void *elem_copy);

        /**
        @param key
        @param copy
        @return
        */
        RetCode     remove(const void *key,
                           void *copy);

        /**
        @return
        */
        RetCode     remove_in_iteration();

        /**
        @param key
        @param ptr
        @return
        */
        RetCode     put(const void *key,
                        const void *ptr);

        /**
        */
        void        start_iteration();

        /**
        @param size
        @return
        */
        RetCode     size(uint32_t &size)    const;

        /**
        @return
        */
        uint32_t    size()                  const;

    private:
        obj_mng         elem_manager_, key_manager_;
        hash_map_rep    *impl_;
};

}

#endif
