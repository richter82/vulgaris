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

#ifndef CR_STRUCTS_REP_H_
#define CR_STRUCTS_REP_H_
#include "blaze.h"

namespace blaze {

class obj_mng;

class base_rep {
    public:
        base_rep();
        void dcr_elems();
        void incr_elems();
        void rst_elems();
        uint32_t elems_cnt()  const;
    private:
        uint32_t elemcount_;
};

class linked_list;
class synch_linked_list;

typedef void(*list_enum_func)(const linked_list &list,
                              const void *ptr,
                              void *ud);

typedef void(*list_enum_func_breakable)(const linked_list &list,
                                        const void *ptr,
                                        void *ud,
                                        bool &brk);

typedef void(*listts_enum_func)(const synch_linked_list &list,
                                const void *ptr,
                                void *ud);

typedef void(*listts_enum_func_breakable)(const synch_linked_list &list,
                                          const void *ptr,
                                          void *ud,
                                          bool &brk);

class lnk_node;

class linked_list_rep : public base_rep {
    public:
        linked_list_rep(obj_mng &manager);
        ~linked_list_rep();
        void r_at(uint32_t idx, void *copy) const;
        const void *r_at(uint32_t idx) const;
        void r_last(void *copy) const;
        void r_first(void *copy) const;
        RetCode r_set_at(uint32_t idx, const void *ptr);
        void r_start_it();
        void r_clear();
        RetCode r_ins_at(uint32_t idx, const void *ptr);
        void r_pop_bk(void *copy);
        void r_pop_fr(void *copy);
        void r_rem_at(uint32_t idx, void *copy);
        RetCode r_push_bk(const void *ptr);
        RetCode r_push_fr(const void *ptr);
        bool r_next(void *copy);
        bool r_rem_in_iter();
        void r_enum(const linked_list &list, list_enum_func enum_f, void *ud) const;
        void r_enum_br(const linked_list &list, list_enum_func_breakable enum_f,
                       void *ud) const;
        void r_enum_ts(const synch_linked_list &list, listts_enum_func enum_f,
                       void *ud) const;
        void r_enum_ts_br(const synch_linked_list &list,
                          listts_enum_func_breakable enum_f,void *ud) const;

    private:
        void r_rem(lnk_node *del_ln);
        lnk_node *r_lnk_node_at(uint32_t idx) const;

    private:
        obj_mng &manager_;
        lnk_node *head_, *tail_, *it_, *prev_;
};

class hash_map;
class synch_hash_map;

typedef void(*hmap_enum_func)(const hash_map &map,
                              const void *key,
                              const void *ptr,
                              void *ud);

typedef void(*hmap_enum_func_breakable)(const hash_map &map,
                                        const void *key,
                                        const void *ptr,
                                        void *ud,
                                        bool &brk);

typedef void(*hmapts_enum_func)(const synch_hash_map &map,
                                const void *key,
                                const void *ptr,
                                void *ud);

typedef void(*hmapts_enum_func_breakable)(const synch_hash_map &map,
                                          const void *key,
                                          const void *ptr,
                                          void *ud,
                                          bool &brk);

class hm_node;

class hash_map_rep : public base_rep {
    public:
        hash_map_rep(uint32_t hash_size,
                     obj_mng &elem_manager,
                     obj_mng &key_manager);

        ~hash_map_rep();

        RetCode r_init();
        void r_start_it();
        uint32_t r_get_idx(const void *key) const;
        void r_clear();
        RetCode r_put(const void *key, const void *ptr);
        RetCode r_get(const void *key, void *copy) const;
        void *r_get(const void *key) const;
        RetCode r_rem(const void *key, void *copy);
        RetCode r_cnt_key(const void *key) const;
        bool r_next(void *key_copy, void *elem_copy);
        bool r_rem_in_it();
        void r_enum(const hash_map &map, hmap_enum_func enum_f, void *ud) const;
        void r_enum_ts(const synch_hash_map &map, hmapts_enum_func enum_f, void *ud);
        void r_enum_br(const hash_map &map, hmap_enum_func_breakable enum_f, void *ud);
        void r_enum_ts_br(const synch_hash_map &map, hmapts_enum_func_breakable enum_f,
                          void *ud);

    private:
        void r_rem(hm_node *del_mn, uint32_t idx);

    private:
        //the length of buckets array
        uint32_t hash_size_;
        //buckets
        hm_node **buckets_;
        obj_mng  &elem_manager_, &key_manager_;
        hm_node  *head_, *tail_, *it_, *prev_;
};

class blocking_queue_rep : public base_rep {
    public:
        blocking_queue_rep(uint32_t capacity,
                           obj_mng &manager);

        ~blocking_queue_rep();
        void r_peek(void *copy);
        uint32_t r_rem_cpcty();
        void r_clear();
        void r_deque(void *copy);
        RetCode r_enque(const void *ptr);

    private:
        bool inifinite_cpcty_;
        uint32_t capacity_;
        obj_mng &manager_;
        lnk_node *head_;
        lnk_node *tail_;
};

}

#endif