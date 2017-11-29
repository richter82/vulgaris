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

#include "cr_structs_rep.h"

#if defined(_MSC_VER)
#define VLG_STRDUP _strdup
#else
#define VLG_STRDUP strdup
#endif

//-----------------------------
// HASHING - LOW LEVEL FUNCS

#define FNV_32_PRIME ((uint32_t)0x01000193)

// Platform-specific functions and macros
// Microsoft Visual Studio
#if defined(_MSC_VER)

#define ROTL32(x,y)     _rotl(x,y)
#define ROTL64(x,y)     _rotl64(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers
#else   // defined(_MSC_VER)

inline uint32_t rotl32(uint32_t x, int8_t r)
{
    return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64(uint64_t x, int8_t r)
{
    return (x << r) | (x >> (64 - r));
}

#define ROTL32(x,y)     rotl32(x,y)
#define ROTL64(x,y)     rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here
inline uint32_t getblock(const uint32_t *p, int i)
{
    return p[i];
}

inline uint64_t getblock(const uint64_t *p, int i)
{
    return p[i];
}

//-----------------------------
// Finalization mix - force all bits of a hash block to avalanche
inline uint32_t fmix(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

inline uint64_t fmix(uint64_t k)
{
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;
    return k;
}

//-----------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
void MurmurHash3_x86_32(const void *key, int len, uint32_t seed, void *out)
{
    const uint8_t *data = (const uint8_t *) key;
    const int nblocks = len / 4;
    uint32_t h1 = seed;
    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;
    //----------
    // body
    const uint32_t *blocks = (const uint32_t *)(data + nblocks * 4);
    for(int i = -nblocks; i; i++) {
        uint32_t k1 = getblock(blocks, i);
        k1 *= c1;
        k1 = ROTL32(k1,15);
        k1 *= c2;
        h1 ^= k1;
        h1 = ROTL32(h1,13);
        h1 = h1 * 5 + 0xe6546b64;
    }
    //----------
    // tail
    const uint8_t *tail = (const uint8_t *)(data + nblocks * 4);
    uint32_t k1 = 0;
    switch(len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = ROTL32(k1,16);
            k1 *= c2;
            h1 ^= k1;
    };
    //----------
    // finalization
    h1 ^= len;
    h1 = fmix(h1);
    *(uint32_t *) out = h1;
}

//-----------------------------
// MEMORY MANAGEMENT
namespace vlg {

typedef void *(*alloc_func)(size_t type_size, const void *copy);
typedef void(*dealloc_func)(void *ptr);
typedef void *(*cpy_func)(void *copy, const void *ptr, size_t type_size);
typedef int(*cmp_func)(const void *obj1, const void *obj2, size_t len);
typedef void(*hash_func)(const void *key, int len, uint32_t seed, void *out);

void *def_alloc_func(size_t type_size, const void *copy)
{
    void *new_ptr = malloc(type_size);
    if(!new_ptr) {
        return NULL;
    }
    if(copy) {
        return memcpy(new_ptr, copy, type_size);
    }
    return new_ptr;
}

void *cstr_alloc_func(size_t type_size, const void *copy)
{
    void *new_ptr = VLG_STRDUP((char *)copy);
    if(!new_ptr) {
        return NULL;
    }
    return new_ptr;
}

void *cstr_cpy_func(void *copy, const void *ptr, size_t type_size)
{
    return strcpy((char *)copy, (char *)ptr);
}

int cstr_cmp_func(const void *obj1, const void *obj2, size_t len)
{
    return strcmp((char *)obj1, (char *)obj2);
}

void cstr_hash_func(const void *key, int len, uint32_t seed, void *out)
{
    MurmurHash3_x86_32(key, (int)strlen((char *)key), seed, out);
}

void *int_alloc_func(size_t type_size, const void *copy)
{
    void *new_ptr = malloc(sizeof(int));
    if(!new_ptr) {
        return NULL;
    }
    if(copy) {
        *(int *)new_ptr = (int)(intptr_t)copy;
    }
    return new_ptr;
}

void *int_cpy_func(void *copy, const void *ptr, size_t type_size)
{
    *(int *)copy = *(int *)ptr;
    return copy;
}

int int_cmp_func(const void *obj1, const void *obj2, size_t len)
{
    int int1 = *(int *)obj1, int2 = *(int *)obj2;
    return int1 > int2 ? 1 : int1 < int2 ? -1 : 0;
}

void int_hash_func(const void *key, int len, uint32_t seed, void *out)
{
    MurmurHash3_x86_32(key, sizeof(int), seed, out);
}

//-----------------------------
// obj_mng_impl
class obj_mng_impl {
    public:
        obj_mng_impl(size_t type_size) :
            seed_(FNV_32_PRIME),
            type_size_(type_size),
            alloc_func_(def_alloc_func),
            dealloc_func_(free),
            cmp_func_(memcmp),
            cpy_func_(memcpy),
            hash_func_(MurmurHash3_x86_32) {
        }

        obj_mng_impl(size_t type_size,
                     alloc_func alloc_f,
                     dealloc_func dealloc_f,
                     cmp_func cmp_f,
                     cpy_func copy_f,
                     hash_func hash_f) :
            seed_(FNV_32_PRIME),
            type_size_(type_size),
            alloc_func_(alloc_f),
            dealloc_func_(dealloc_f),
            cmp_func_(cmp_f),
            cpy_func_(copy_f),
            hash_func_(hash_f) {
        }

        obj_mng_impl(const obj_mng_impl &other) :
            seed_(other.seed_),
            type_size_(other.type_size_),
            alloc_func_(other.alloc_func_),
            dealloc_func_(other.dealloc_func_),
            cmp_func_(other.cmp_func_),
            cpy_func_(other.cpy_func_),
            hash_func_(other.hash_func_) {
        }

        ~obj_mng_impl() {}

        void *new_obj(const void *copy) const {
            return alloc_func_(type_size_, copy);
        }

        void destroy_obj(void *ptr) const {
            dealloc_func_(ptr);
        }

        void *cpy_obj(void *copy,
                      const void *ptr,
                      bool can_realloc) const {
            if(can_realloc) {
                if(!type_size_) {
                    size_t strl = strlen((char *)ptr) + 1;
                    if(!(copy = realloc(copy, strl))) {
                        return NULL;
                    }
                }
            }
            return copy ? cpy_func_(copy, ptr, type_size_) : NULL;
        }

        int cmp_obj(const void *ptr1, const void *ptr2) const {
            return cmp_func_(ptr1, ptr2, type_size_);
        }

        void hash_obj(const void *ptr, void *out) const {
            hash_func_(ptr, (int)type_size_, seed_, out);
        }

    private:
        uint32_t        seed_;
        size_t          type_size_;
        alloc_func      alloc_func_;
        dealloc_func    dealloc_func_;
        cmp_func        cmp_func_;
        cpy_func        cpy_func_;
        hash_func       hash_func_;
};

//-----------------------------
// obj_mng
class obj_mng {
    public:
        explicit obj_mng(size_t type_size);
        explicit obj_mng(const obj_mng &other);
        explicit obj_mng(size_t type_size,
                         alloc_func alloc_f,
                         dealloc_func dealloc_f,
                         cmp_func cmp_f,
                         cpy_func copy_f,
                         hash_func hash_f);

        ~obj_mng();

        void *new_obj(const void *copy)     const;
        void del_obj(void *ptr)             const;

        void *copy_obj(void *copy,
                       const void *ptr,
                       bool can_realloc = false)    const;

        int cmp_obj(const void *ptr1,
                    const void *ptr2)               const;

        void hash_obj(const void *ptr,
                      void *out)                    const;

    private:
        obj_mng_impl *impl_;
};

obj_mng::obj_mng(size_t type_size) : impl_(NULL)
{
    impl_ = new obj_mng_impl(type_size);
}

obj_mng::obj_mng(size_t type_size,
                 alloc_func alloc_f,
                 dealloc_func dealloc_f,
                 cmp_func cmp_f,
                 cpy_func copy_f,
                 hash_func hash_f) : impl_(NULL)
{
    impl_ = new obj_mng_impl(type_size,
                             alloc_f,
                             dealloc_f,
                             cmp_f,
                             copy_f,
                             hash_f);
}

obj_mng::obj_mng(const obj_mng &other) : impl_(NULL)
{
    impl_ = new obj_mng_impl(*other.impl_);
}

obj_mng::~obj_mng()
{
    if(impl_) {
        delete impl_;
    }
}

void *obj_mng::new_obj(const void *copy) const
{
    return impl_->new_obj(copy);
}

void obj_mng::del_obj(void *ptr) const
{
    impl_->destroy_obj(ptr);
}

void *obj_mng::copy_obj(void *copy,
                        const void *ptr,
                        bool can_realloc) const
{
    return impl_->cpy_obj(copy, ptr, can_realloc);
}

int obj_mng::cmp_obj(const void *ptr1, const void *ptr2) const
{
    return impl_->cmp_obj(ptr1, ptr2);
}

void obj_mng::hash_obj(const void *ptr, void *out) const
{
    impl_->hash_obj(ptr, out);
}

}

namespace vlg {
//-----------------------------
// COMMON MANAGER SINGLETONS
class ptr_obj_mng : public obj_mng {
        friend ptr_obj_mng &sngl_ptr_obj_mng();
    public:
        ptr_obj_mng();

    private:
        static ptr_obj_mng *instance_;
};

class cstr_obj_mng : public obj_mng {
        friend cstr_obj_mng &sngl_cstr_obj_mng();
    public:
        cstr_obj_mng();

    private:
        static cstr_obj_mng *instance_;
};

class int_obj_mng : public obj_mng {
        friend int_obj_mng &sngl_impl_obj_mng();
    public:
        int_obj_mng();
    private:
        static int_obj_mng *instance_;
};

ptr_obj_mng::ptr_obj_mng() : obj_mng(sizeof(void *)) {}

ptr_obj_mng *ptr_obj_mng::instance_ = NULL;
ptr_obj_mng &sngl_ptr_obj_mng()
{
    if(ptr_obj_mng::instance_ == NULL) {
        ptr_obj_mng::instance_ = new ptr_obj_mng();
    }
    return *ptr_obj_mng::instance_;
}

cstr_obj_mng::cstr_obj_mng() : obj_mng(0,
                                           cstr_alloc_func,
                                           free,
                                           cstr_cmp_func,
                                           cstr_cpy_func,
                                           cstr_hash_func) {}

cstr_obj_mng *cstr_obj_mng::instance_ = NULL;
cstr_obj_mng &sngl_cstr_obj_mng()
{
    if(cstr_obj_mng::instance_ == NULL) {
        cstr_obj_mng::instance_ = new cstr_obj_mng();
    }
    return *cstr_obj_mng::instance_;
}

int_obj_mng::int_obj_mng() : obj_mng(0,
                                         int_alloc_func,
                                         free,
                                         int_cmp_func,
                                         int_cpy_func,
                                         int_hash_func) {}

int_obj_mng *int_obj_mng::instance_ = NULL;
int_obj_mng &sngl_impl_obj_mng()
{
    if(int_obj_mng::instance_ == NULL) {
        int_obj_mng::instance_ = new int_obj_mng();
    }
    return *int_obj_mng::instance_;
}
}

//-----------------------------
// Linked-List and Hash-Map REPS
namespace vlg {

//-----------------------------
// base_rep
base_rep::base_rep() : elemcount_(0) {};

void base_rep::dcr_elems()
{
    elemcount_--;
}

void base_rep::incr_elems()
{
    elemcount_++;
}

void base_rep::rst_elems()
{
    elemcount_ = 0;
}

uint32_t base_rep::elems_cnt()  const
{
    return elemcount_;
}

//-----------------------------
// lnk_node
class lnk_node {
    public:
        lnk_node(void *ptr) : ptr_(ptr),
            next_(NULL),
            previous_(NULL) {}
        void *ptr_;
        lnk_node *next_;
        lnk_node *previous_;
};

//-----------------------------
// linked_list_rep
linked_list_rep::linked_list_rep(obj_mng &manager) : manager_(manager),
    head_(NULL),
    tail_(NULL),
    it_(NULL),
    prev_(NULL)
{
}

linked_list_rep::~linked_list_rep()
{
    r_clear();
}

void linked_list_rep::r_at(uint32_t idx, void *copy) const
{
    lnk_node *ln = r_lnk_node_at(idx);
    manager_.copy_obj(copy, ln->ptr_);
}

const void *linked_list_rep::r_at(uint32_t idx) const
{
    return r_lnk_node_at(idx)->ptr_;
}

void linked_list_rep::r_last(void *copy) const
{
    manager_.copy_obj(copy, tail_->ptr_);
}

void linked_list_rep::r_first(void *copy) const
{
    manager_.copy_obj(copy, head_->ptr_);
}

RetCode linked_list_rep::r_set_at(uint32_t idx, const void *ptr)
{
    lnk_node *ln = r_lnk_node_at(idx);
    return (ln->ptr_ = manager_.copy_obj(ln->ptr_, ptr, true))
           ? RetCode_OK : RetCode_MEMERR ;
}

void linked_list_rep::r_start_it()
{
    it_ = head_;
    prev_ = NULL;
}

void linked_list_rep::r_clear()
{
    lnk_node *cur_ln = head_, *next = NULL;
    while(cur_ln != NULL) {
        next = cur_ln->next_;
        manager_.del_obj(cur_ln->ptr_);
        delete cur_ln;
        cur_ln = next;
    }
    head_ = tail_ = NULL;
    rst_elems();
}

RetCode linked_list_rep::r_ins_at(uint32_t idx, const void *ptr)
{
    void *new_obj = NULL;
    if(!(new_obj = manager_.new_obj(ptr))) {
        return RetCode_MEMERR;
    }
    lnk_node *new_ln = new lnk_node(new_obj);
    if(!new_ln) {
        return RetCode_MEMERR;
    }
    lnk_node *cur_ln = r_lnk_node_at(idx);
    if(cur_ln == head_) {
        head_ = new_ln;
    } else {
        new_ln->previous_ = cur_ln->previous_;
        cur_ln->previous_->next_ = new_ln;
    }
    new_ln->next_ = cur_ln;
    cur_ln->previous_ = new_ln;
    incr_elems();
    return RetCode_OK;
}

void linked_list_rep::r_pop_bk(void *copy)
{
    lnk_node *tail = tail_->previous_;
    manager_.copy_obj(copy, tail_->ptr_);
    manager_.del_obj(tail_->ptr_);
    delete tail_;
    if(tail) {
        tail_ = tail;
        tail_->next_ = NULL;
    } else {
        head_ = tail_ = NULL;
    }
    dcr_elems();
}

void linked_list_rep::r_pop_fr(void *copy)
{
    lnk_node *head = head_->next_;
    manager_.copy_obj(copy, head_->ptr_);
    manager_.del_obj(head_->ptr_);
    delete head_;
    if(head) {
        head_ = head;
        head_->previous_ = NULL;
    } else {
        head_ = tail_ = NULL;
    }
    dcr_elems();
}

void linked_list_rep::r_rem_at(uint32_t idx, void *copy)
{
    lnk_node *del_ln = r_lnk_node_at(idx);
    manager_.copy_obj(copy, del_ln->ptr_);
    r_rem(del_ln);
    dcr_elems();
}

RetCode linked_list_rep::r_push_bk(const void *ptr)
{
    void *new_obj = NULL;
    if(!(new_obj = manager_.new_obj(ptr))) {
        return RetCode_MEMERR;
    }
    lnk_node *new_ln = new lnk_node(new_obj);
    if(!new_ln) {
        return RetCode_MEMERR;
    }
    if(!head_) {
        head_ = new_ln;
    } else {
        new_ln->previous_ = tail_;
        tail_->next_ = new_ln;
    }
    tail_ = new_ln;
    incr_elems();
    return RetCode_OK;
}

RetCode linked_list_rep::r_push_fr(const void *ptr)
{
    void *new_obj = NULL;
    if(!(new_obj = manager_.new_obj(ptr))) {
        return RetCode_MEMERR;
    }
    lnk_node *new_ln = new lnk_node(new_obj);
    if(!new_ln) {
        return RetCode_MEMERR;
    }
    if(!tail_) {
        tail_ = new_ln;
    } else {
        new_ln->next_ = head_;
        head_->previous_ = new_ln;
    }
    head_ = new_ln;
    incr_elems();
    return RetCode_OK;
}

bool linked_list_rep::r_next(void *copy)
{
    if(!it_) {
        return false;
    }
    manager_.copy_obj(copy, it_->ptr_);
    prev_ = it_;
    it_ = it_->next_;
    return true;
}

bool linked_list_rep::r_rem_in_iter()
{
    if(!prev_) {
        return false;
    }
    r_rem(prev_);
    dcr_elems();
    prev_ = NULL;
    return true;
}

void linked_list_rep::r_enum(const linked_list &list,
                             list_enum_func enum_f, void *ud) const
{
    lnk_node *it = head_, *prev = NULL;
    while(it) {
        prev = it;
        it = it->next_;
        enum_f(list, prev->ptr_, ud);
    }
}

void linked_list_rep::r_enum_br(const linked_list &list,
                                list_enum_func_breakable enum_f,
                                void *ud) const
{
    lnk_node *it = head_, *prev = NULL;
    bool brk = false;
    while(!brk && it) {
        prev = it;
        it = it->next_;
        enum_f(list, prev->ptr_, ud, brk);
    }
}

void linked_list_rep::r_enum_ts(const synch_linked_list &list,
                                listts_enum_func enum_f,
                                void *ud) const
{
    lnk_node *it = head_, *prev = NULL;
    while(it) {
        prev = it;
        it = it->next_;
        enum_f(list, prev->ptr_, ud);
    }
}

void linked_list_rep::r_enum_ts_br(const synch_linked_list &list,
                                   listts_enum_func_breakable enum_f,
                                   void *ud) const
{
    lnk_node *it = head_, *prev = NULL;
    bool brk = false;
    while(!brk && it) {
        prev = it;
        it = it->next_;
        enum_f(list, prev->ptr_, ud, brk);
    }
}

void linked_list_rep::r_rem(lnk_node *del_ln)
{
    if(del_ln->previous_) {
        del_ln->previous_->next_ = del_ln->next_;
    } else {
        head_ = del_ln->next_;
    }
    if(del_ln->next_) {
        del_ln->next_->previous_ = del_ln->previous_;
    } else {
        tail_ = del_ln->previous_;
    }
    manager_.del_obj(del_ln->ptr_);
    delete del_ln;
}

lnk_node *linked_list_rep::r_lnk_node_at(uint32_t idx) const
{
    lnk_node *cur_ln = head_;
    uint32_t i = 0;
    while(i++ < idx) {
        cur_ln = cur_ln->next_;
    }
    return cur_ln;
}

//-----------------------------
// hm_node
class hm_node {
    public:
        hm_node(void *ptr,
                void *key_ptr) :
            next_(NULL),
            prev_(NULL),
            ptr_(ptr),
            key_ptr_(key_ptr),
            insrt_next_(NULL),
            insrt_prev_(NULL) {}
        hm_node *next_, *prev_;
        void *ptr_, *key_ptr_;
        hm_node *insrt_next_, *insrt_prev_;
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

//-----------------------------
// hash_map_rep
hash_map_rep::hash_map_rep(uint32_t hash_size,
                           obj_mng &elem_manager,
                           obj_mng &key_manager) :
    hash_size_(hash_size),
    buckets_(NULL),
    elem_manager_(elem_manager),
    key_manager_(key_manager),
    head_(NULL),
    tail_(NULL),
    it_(NULL),
    prev_(NULL)
{
}

hash_map_rep::~hash_map_rep()
{
    if(buckets_) {
        hm_node *cur_mn = head_, *next = NULL;
        while(cur_mn != NULL) {
            next = cur_mn->insrt_next_;
            elem_manager_.del_obj(cur_mn->ptr_);
            key_manager_.del_obj(cur_mn->key_ptr_);
            delete cur_mn;
            cur_mn = next;
        }
        free(buckets_);
    }
}

RetCode hash_map_rep::r_init()
{
    return !(buckets_ = (hm_node **)calloc(hash_size_, sizeof(hm_node *)))
           ? RetCode_MEMERR : RetCode_OK;
}

void hash_map_rep::r_start_it()
{
    it_ = head_;
    prev_ = NULL;
}

uint32_t hash_map_rep::r_get_idx(const void *key) const
{
    uint32_t hash_code;
    key_manager_.hash_obj(key, &hash_code);
    return hash_code % hash_size_;
}

void hash_map_rep::r_clear()
{
    hm_node *cur_mn = head_, *next = NULL;
    while(cur_mn != NULL) {
        next = cur_mn->insrt_next_;
        elem_manager_.del_obj(cur_mn->ptr_);
        key_manager_.del_obj(cur_mn->key_ptr_);
        delete cur_mn;
        cur_mn = next;
    }
    memset(buckets_, 0, (hash_size_*sizeof(hm_node *)));
    head_ = tail_ = NULL;
    rst_elems();
}

RetCode hash_map_rep::r_put(const void *key, const void *ptr)
{
    uint32_t idx = r_get_idx(key);
    hm_node *cur_node = buckets_[idx];
    bool is_new = true;
    if(cur_node) {
        do {
            if(!key_manager_.cmp_obj(cur_node->key_ptr_, key)) {
                is_new = false;
                break;
            }
            if(cur_node->next_) {
                cur_node = cur_node->next_;
            } else {
                break;
            }
        } while(true);
        if(is_new) {
            void *elem_ptr = elem_manager_.new_obj(ptr);
            if(!elem_ptr) {
                return RetCode_MEMERR;
            }
            void *key_ptr = key_manager_.new_obj(key);
            if(!key_ptr) {
                return RetCode_MEMERR;
            }
            hm_node *new_node = new hm_node(elem_ptr, key_ptr);
            if(!new_node) {
                return RetCode_MEMERR;
            }
            cur_node->next_ = new_node;
            new_node->prev_ = cur_node;
            tail_->insrt_next_ = new_node;
            new_node->insrt_prev_ = tail_;
            tail_ = new_node;
            incr_elems();
        } else {
            if(!(cur_node->ptr_ = elem_manager_.copy_obj(cur_node->ptr_,
                                                         ptr, true))) {
                return RetCode_MEMERR;
            }
        }
    } else {
        void *elem_ptr = elem_manager_.new_obj(ptr);
        if(!elem_ptr) {
            return RetCode_MEMERR;
        }
        void *key_ptr = key_manager_.new_obj(key);
        if(!key_ptr) {
            return RetCode_MEMERR;
        }
        hm_node *new_node = new hm_node(elem_ptr, key_ptr);
        if(!new_node) {
            return RetCode_MEMERR;
        }
        buckets_[idx] = new_node;
        if(!head_) {
            head_ = new_node;
        } else {
            tail_->insrt_next_ = new_node;
        }
        new_node->insrt_prev_ = tail_;
        tail_ = new_node;
        incr_elems();
    }
    return RetCode_OK;
}

RetCode hash_map_rep::r_get(const void *key, void *copy) const
{
    hm_node *m_node = buckets_[r_get_idx(key)];
    while(m_node) {
        if(!key_manager_.cmp_obj(m_node->key_ptr_, key)) {
            elem_manager_.copy_obj(copy, m_node->ptr_);
            return RetCode_OK;
        }
        m_node = m_node->next_;
    }
    return RetCode_KO;
}

void *hash_map_rep::r_get(const void *key) const
{
    hm_node *m_node = buckets_[r_get_idx(key)];
    while(m_node) {
        if(!key_manager_.cmp_obj(m_node->key_ptr_, key)) {
            return m_node->ptr_;
        }
        m_node = m_node->next_;
    }
    return NULL;
}

RetCode hash_map_rep::r_rem(const void *key, void *copy)
{
    uint32_t idx = r_get_idx(key);
    hm_node *m_node = buckets_[idx];
    while(m_node) {
        if(!key_manager_.cmp_obj(m_node->key_ptr_, key)) {
            elem_manager_.copy_obj(copy, m_node->ptr_);
            r_rem(m_node, idx);
            dcr_elems();
            return RetCode_OK;
        }
        m_node = m_node->next_;
    }
    return RetCode_KO;
}

RetCode hash_map_rep::r_cnt_key(const void *key) const
{
    hm_node *m_node = buckets_[r_get_idx(key)];
    while(m_node) {
        if(!key_manager_.cmp_obj(m_node->key_ptr_, key)) {
            return RetCode_OK;
        }
        m_node = m_node->next_;
    }
    return RetCode_KO;
}

bool hash_map_rep::r_next(void *key_copy, void *elem_copy)
{
    if(!it_) {
        return false;
    }
    if(elem_copy) {
        elem_manager_.copy_obj(elem_copy, it_->ptr_);
    }
    if(key_copy) {
        key_manager_.copy_obj(key_copy, it_->key_ptr_);
    }
    prev_ = it_;
    it_ = it_->insrt_next_;
    return true;
}

bool hash_map_rep::r_rem_in_it()
{
    if(!prev_) {
        return false;
    }
    r_rem(prev_, r_get_idx(prev_->key_ptr_));
    dcr_elems();
    prev_ = NULL;
    return true;
}

void hash_map_rep::r_enum(const hash_map &map, hmap_enum_func enum_f,
                          void *ud) const
{
    hm_node *it = head_, *prev = NULL;
    while(it) {
        prev = it;
        it = it->insrt_next_;
        enum_f(map, prev->key_ptr_, prev->ptr_, ud);
    }
}

void hash_map_rep::r_enum_ts(const synch_hash_map &map, hmapts_enum_func enum_f,
                             void *ud)
{
    hm_node *it = head_, *prev = NULL;
    while(it) {
        prev = it;
        it = it->insrt_next_;
        enum_f(map, prev->key_ptr_, prev->ptr_, ud);
    }
}

void hash_map_rep::r_enum_br(const hash_map &map,
                             hmap_enum_func_breakable enum_f, void *ud)
{
    hm_node *it = head_, *prev = NULL;
    bool brk = false;
    while(!brk && it) {
        prev = it;
        it = it->insrt_next_;
        enum_f(map, prev->key_ptr_, prev->ptr_, ud, brk);
    }
}

void hash_map_rep::r_enum_ts_br(const synch_hash_map &map,
                                hmapts_enum_func_breakable enum_f, void *ud)
{
    hm_node *it = head_, *prev = NULL;
    bool brk = false;
    while(!brk && it) {
        prev = it;
        it = it->insrt_next_;
        enum_f(map, prev->key_ptr_, prev->ptr_, ud, brk);
    }
}

void hash_map_rep::r_rem(hm_node *del_mn, uint32_t idx)
{
    if(del_mn->prev_) {
        del_mn->prev_->next_ = del_mn->next_;
    } else {
        buckets_[idx] = del_mn->next_;
    }
    if(del_mn->next_) {
        del_mn->next_->prev_ = del_mn->prev_;
    }
    if(del_mn->insrt_prev_) {
        del_mn->insrt_prev_->insrt_next_ = del_mn->insrt_next_;
    } else if(head_ == del_mn) {
        head_ = del_mn->insrt_next_;
    }
    if(del_mn->insrt_next_) {
        del_mn->insrt_next_->insrt_prev_ = del_mn->insrt_prev_;
    } else if(tail_ == del_mn) {
        tail_ = del_mn->insrt_prev_;
    }
    elem_manager_.del_obj(del_mn->ptr_);
    key_manager_.del_obj(del_mn->key_ptr_);
    delete del_mn;
}

//-----------------------------
// blocking_queue_rep
blocking_queue_rep::blocking_queue_rep(uint32_t capacity,
                                       obj_mng &manager) :
    inifinite_cpcty_(capacity ? false : true),
    capacity_(capacity),
    manager_(manager),
    head_(NULL),
    tail_(NULL)
{
}

blocking_queue_rep::~blocking_queue_rep()
{
    lnk_node *cur = tail_, *next = NULL;
    while(cur != NULL) {
        next = cur->next_;
        manager_.del_obj(cur->ptr_);
        delete cur;
        cur = next;
    }
}

void blocking_queue_rep::r_peek(void *copy)
{
    if(copy) {
        manager_.copy_obj(copy, head_->ptr_);
    }
}

uint32_t blocking_queue_rep::r_rem_cpcty()
{
    return inifinite_cpcty_ ? 1 : (capacity_ - elems_cnt());
}

void blocking_queue_rep::r_clear()
{
    lnk_node *cur = tail_, *next = NULL;
    while(cur != NULL) {
        next = cur->next_;
        manager_.del_obj(cur->ptr_);
        delete cur;
        cur = next;
    }
    tail_ = head_ = NULL;
    rst_elems();
}

void blocking_queue_rep::r_deque(void *copy)
{
    lnk_node *new_head = head_->previous_;
    if(copy) {
        manager_.copy_obj(copy, head_->ptr_);
    }
    manager_.del_obj(head_->ptr_);
    delete head_;
    if(new_head) {
        head_ = new_head;
        head_->next_ = NULL;
    } else {
        tail_ = head_ = NULL;
    }
    dcr_elems();
}

RetCode blocking_queue_rep::r_enque(const void *ptr)
{
    void *new_obj = manager_.new_obj(ptr);
    lnk_node *new_tail = new lnk_node(new_obj);
    if(!new_tail) {
        return RetCode_MEMERR;
    }
    lnk_node *old_tail = tail_;
    tail_ = new_tail;
    if(!old_tail) {
        head_ = new_tail;
    } else {
        new_tail->next_ = old_tail;
        old_tail->previous_ = new_tail;
    }
    incr_elems();
    return RetCode_OK;
}

}
