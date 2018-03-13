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

#include "glob.h"

#define FNV_32_PRIME ((uint32_t)0x01000193)

// Platform-specific functions and macros
// Microsoft Visual Studio
#if defined WIN32 && defined _MSC_VER

#define ROTL32(x,y)     _rotl(x,y)
#define ROTL64(x,y)     _rotl64(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers
#else

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

#endif

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

namespace vlg {

void *def_alloc_func(size_t type_size, const void *copy)
{
    void *new_ptr = malloc(type_size);
    if(!new_ptr) {
        return nullptr;
    }
    if(copy) {
        return memcpy(new_ptr, copy, type_size);
    }
    return new_ptr;
}

void *cstr_alloc_func(size_t type_size, const void *copy)
{
    void *new_ptr = strdup((char *)copy);
    if(!new_ptr) {
        return nullptr;
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

//obj_mng

obj_mng::obj_mng(size_t type_size) :
    seed_(FNV_32_PRIME),
    type_size_(type_size),
    alloc_func_(def_alloc_func),
    dealloc_func_(free),
    cmp_func_(memcmp),
    cpy_func_(memcpy),
    hash_func_(MurmurHash3_x86_32)
{
}

obj_mng::obj_mng(const obj_mng &other) :
    seed_(other.seed_),
    type_size_(other.type_size_),
    alloc_func_(other.alloc_func_),
    dealloc_func_(other.dealloc_func_),
    cmp_func_(other.cmp_func_),
    cpy_func_(other.cpy_func_),
    hash_func_(other.hash_func_)
{
}

obj_mng::obj_mng(size_t type_size,
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
    hash_func_(hash_f)
{
}

ptr_obj_mng::ptr_obj_mng() : obj_mng(sizeof(void *)) {}

ptr_obj_mng *ptr_obj_mng::instance_ = nullptr;
const ptr_obj_mng &sngl_ptr_obj_mng()
{
    if(ptr_obj_mng::instance_ == nullptr) {
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

cstr_obj_mng *cstr_obj_mng::instance_ = nullptr;
const cstr_obj_mng &sngl_cstr_obj_mng()
{
    if(cstr_obj_mng::instance_ == nullptr) {
        cstr_obj_mng::instance_ = new cstr_obj_mng();
    }
    return *cstr_obj_mng::instance_;
}

struct hm_node {
    hm_node(void *ptr,
            void *key_ptr) :
        next_(nullptr),
        prev_(nullptr),
        ptr_(ptr),
        key_ptr_(key_ptr),
        insrt_next_(nullptr),
        insrt_prev_(nullptr) {}
    hm_node *next_, *prev_;
    void *ptr_, *key_ptr_;
    hm_node *insrt_next_, *insrt_prev_;
};

s_hm::s_hm(uint32_t hash_size,
           size_t elem_size,
           size_t key_size,
           pthread_rwlockattr_t *attr) :
    elem_manager_(elem_size),
    key_manager_(key_size),
    hash_size_(hash_size),
    buckets_(nullptr),
    head_(nullptr),
    tail_(nullptr),
    it_(nullptr),
    prev_(nullptr)
{
    init(attr);
}

s_hm::s_hm(uint32_t hash_size,
           const obj_mng &elem_manager,
           const obj_mng &key_manager,
           pthread_rwlockattr_t *attr) :
    elem_manager_(elem_manager),
    key_manager_(key_manager),
    hash_size_(hash_size),
    buckets_(nullptr),
    head_(nullptr),
    tail_(nullptr),
    it_(nullptr),
    prev_(nullptr)
{
    init(attr);
}

s_hm::s_hm(uint32_t hash_size,
           size_t elem_size,
           const obj_mng &key_manager,
           pthread_rwlockattr_t *attr) :
    elem_manager_(elem_size),
    key_manager_(key_manager),
    hash_size_(hash_size),
    buckets_(nullptr),
    head_(nullptr),
    tail_(nullptr),
    it_(nullptr),
    prev_(nullptr)
{
    init(attr);
}

s_hm::s_hm(uint32_t hash_size,
           const obj_mng &elem_manager,
           size_t key_size,
           pthread_rwlockattr_t *attr) :
    elem_manager_(elem_manager),
    key_manager_(key_size),
    hash_size_(hash_size),
    buckets_(nullptr),
    head_(nullptr),
    tail_(nullptr),
    it_(nullptr),
    prev_(nullptr)
{
    init(attr);
}

s_hm::~s_hm()
{
    if(buckets_) {
        hm_node *cur_mn = head_, *next = nullptr;
        while(cur_mn != nullptr) {
            next = cur_mn->insrt_next_;
            elem_manager_.del_obj(cur_mn->ptr_);
            key_manager_.del_obj(cur_mn->key_ptr_);
            delete cur_mn;
            cur_mn = next;
        }
        free(buckets_);
    }
    pthread_rwlock_destroy(&lock_);
}


void s_hm::init(pthread_rwlockattr_t *attr)
{
    pthread_rwlock_init(&lock_, attr);
    buckets_ = (hm_node **)calloc(hash_size_, sizeof(hm_node *));
}

uint32_t s_hm::gidx(const void *key) const
{
    uint32_t hash_code;
    key_manager_.hash_obj(key, &hash_code);
    return hash_code % hash_size_;
}

void s_hm::rm(hm_node *del_mn, uint32_t idx)
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

void s_hm::enm(const s_hm &map,
               s_hm_enm_func enum_f,
               void *ud) const
{
    hm_node *it = head_, *prev = nullptr;
    while(it) {
        prev = it;
        it = it->insrt_next_;
        enum_f(map, prev->key_ptr_, prev->ptr_, ud);
    }
}

void s_hm::enmbr(const s_hm &map,
                 s_hm_enm_func_br enum_f,
                 void *ud) const
{
    hm_node *it = head_, *prev = nullptr;
    bool brk = false;
    while(!brk && it) {
        prev = it;
        it = it->insrt_next_;
        enum_f(map, prev->key_ptr_, prev->ptr_, ud, brk);
    }
}

RetCode s_hm::clear()
{
    scoped_wr_lock wl(lock_);
    hm_node *cur_mn = head_, *next = nullptr;
    while(cur_mn != nullptr) {
        next = cur_mn->insrt_next_;
        elem_manager_.del_obj(cur_mn->ptr_);
        key_manager_.del_obj(cur_mn->key_ptr_);
        delete cur_mn;
        cur_mn = next;
    }
    memset(buckets_, 0, (hash_size_*sizeof(hm_node *)));
    head_ = tail_ = nullptr;
    rst_elems();
    return RetCode_OK;
}

RetCode s_hm::enum_elements_safe_read(s_hm_enm_func enum_f,
                                      void *ud) const
{
    scoped_rd_lock rl(lock_);
    enm(*this, enum_f, ud);
    return RetCode_OK;
}

RetCode s_hm::enum_elements_safe_write(s_hm_enm_func enum_f,
                                       void *ud) const
{
    scoped_wr_lock wl(lock_);
    enm(*this, enum_f, ud);
    return RetCode_OK;
}

RetCode s_hm::enum_elements_breakable_safe_read(s_hm_enm_func_br enum_f,
                                                void *ud) const
{
    scoped_rd_lock rl(lock_);
    enmbr(*this, enum_f, ud);
    return RetCode_OK;
}

RetCode s_hm::enum_elements_breakable_safe_write(
    s_hm_enm_func_br enum_f, void *ud) const
{
    scoped_wr_lock wl(lock_);
    enmbr(*this, enum_f, ud);
    return RetCode_OK;
}

RetCode s_hm::get(const void *key, void *copy) const
{
    if(!key) {
        return RetCode_BADARG;
    }
    scoped_rd_lock rl(lock_);
    hm_node *m_node = buckets_[gidx(key)];
    while(m_node) {
        if(!key_manager_.cmp_obj(m_node->key_ptr_, key)) {
            elem_manager_.copy_obj(copy, m_node->ptr_);
            return RetCode_OK;
        }
        m_node = m_node->next_;
    }
    return RetCode_KO;
}

RetCode s_hm::put(const void *key, const void *ptr)
{
    if(!key || !ptr) {
        return RetCode_BADARG;
    }
    scoped_wr_lock wl(lock_);
    uint32_t idx = gidx(key);
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
            if(!(cur_node->ptr_ = elem_manager_.copy_obj(cur_node->ptr_, ptr))) {
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

RetCode s_hm::contains_key(const void *key) const
{
    if(!key) {
        return RetCode_BADARG;
    }
    scoped_rd_lock rl(lock_);
    hm_node *m_node = buckets_[gidx(key)];
    while(m_node) {
        if(!key_manager_.cmp_obj(m_node->key_ptr_, key)) {
            return RetCode_OK;
        }
        m_node = m_node->next_;
    }
    return RetCode_KO;
}

RetCode s_hm::remove(const void *key, void *copy)
{
    if(!key) {
        return RetCode_BADARG;
    }
    scoped_wr_lock wl(lock_);
    uint32_t idx = gidx(key);
    hm_node *m_node = buckets_[idx];
    while(m_node) {
        if(!key_manager_.cmp_obj(m_node->key_ptr_, key)) {
            elem_manager_.copy_obj(copy, m_node->ptr_);
            rm(m_node, idx);
            dcr_elems();
            return RetCode_OK;
        }
        m_node = m_node->next_;
    }
    return RetCode_KO;
}

// lnk_node

struct lnk_node {
    lnk_node(void *ptr) : ptr_(ptr),
        next_(nullptr),
        previous_(nullptr) {}
    void *ptr_;
    lnk_node *next_;
    lnk_node *previous_;
};

// blocking_queue

b_qu::b_qu(size_t elemsize,
           uint32_t capacity) :
    manager_(elemsize),
    inifinite_cpcty_(capacity ? false : true),
    capacity_(capacity),
    head_(nullptr),
    tail_(nullptr),
    wt_prod_(0),
    wt_cons_(0)
{}

b_qu::b_qu(const obj_mng &elem_manager,
           uint32_t capacity) :
    manager_(elem_manager),
    inifinite_cpcty_(capacity ? false : true),
    capacity_(capacity),
    head_(nullptr),
    tail_(nullptr),
    wt_prod_(0),
    wt_cons_(0)
{}

b_qu::~b_qu()
{
    lnk_node *cur = tail_, *next = nullptr;
    while(cur != nullptr) {
        next = cur->next_;
        manager_.del_obj(cur->ptr_);
        delete cur;
        cur = next;
    }
}

void b_qu::dq(void *copy)
{
    lnk_node *new_head = head_->previous_;
    if(copy) {
        manager_.copy_obj(copy, head_->ptr_);
    }
    manager_.del_obj(head_->ptr_);
    delete head_;
    if(new_head) {
        head_ = new_head;
        head_->next_ = nullptr;
    } else {
        tail_ = head_ = nullptr;
    }
    dcr_elems();
}

RetCode b_qu::enq(const void *ptr)
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

RetCode b_qu::clear()
{
    scoped_mx smx(mon_);
    lnk_node *cur = tail_, *next = nullptr;
    while(cur != nullptr) {
        next = cur->next_;
        manager_.del_obj(cur->ptr_);
        delete cur;
        cur = next;
    }
    tail_ = head_ = nullptr;
    rst_elems();
    if(wt_prod_) {
        wt_prod_ = 0;
        mon_.notify_all();
    }
    return RetCode_OK;
}

RetCode b_qu::get(void *copy)
{
    scoped_mx smx(mon_);
    while(!elems_cnt()) {
        wt_cons_++;
        mon_.wait();
    }
    dq(copy);
    if(wt_prod_) {
        wt_prod_ = 0;
        mon_.notify_all();
    }
    return RetCode_OK;
}

RetCode b_qu::get(time_t sec, long nsec, void *copy)
{
    RetCode res = RetCode_OK;
    scoped_mx smx(mon_);
    if(!nsec && !sec) {
        if(elems_cnt()) {
            dq(copy);
        } else {
            res = RetCode_EMPTY;
        }
    } else {
        while(!elems_cnt()) {
            wt_cons_++;
            int pthres;
            if((pthres = mon_.wait(sec, nsec))) {
                if(pthres == ETIMEDOUT) {
                    wt_cons_--;
                    return RetCode_TIMEOUT;
                } else {
                    return RetCode_PTHERR;
                }
            }
        }
        dq(copy);
        if(wt_prod_) {
            wt_prod_ = 0;
            mon_.notify_all();
        }
    }
    return res;
}

RetCode b_qu::peek(time_t sec, long nsec, void *copy)
{
    RetCode res = RetCode_OK;
    scoped_mx smx(mon_);
    if(!nsec && !sec) {
        if(elems_cnt()) {
            if(copy) {
                manager_.copy_obj(copy, head_->ptr_);
            }
        } else {
            res = RetCode_EMPTY;
        }
    } else {
        while(!elems_cnt()) {
            wt_cons_++;
            int pthres;
            if((pthres = mon_.wait(sec, nsec))) {
                if(pthres == ETIMEDOUT) {
                    wt_cons_--;
                    return RetCode_TIMEOUT;
                } else {
                    return RetCode_PTHERR;
                }
            }
        }
        if(copy) {
            manager_.copy_obj(copy, head_->ptr_);
        }
    }
    return res;
}

RetCode b_qu::put(time_t sec, long nsec, const void *ptr)
{
    RetCode res = RetCode_OK;
    if(!ptr) {
        return RetCode_BADARG;
    }
    scoped_mx smx(mon_);
    if(!nsec && !sec) {
        if(remain_capacity()) {
            res = enq(ptr);
        } else {
            res = RetCode_QFULL;
        }
    } else {
        while(!remain_capacity()) {
            wt_prod_++;
            int pthres;
            if((pthres = mon_.wait(sec, nsec))) {
                if(pthres == ETIMEDOUT) {
                    wt_prod_--;
                    return RetCode_TIMEOUT;
                } else {
                    return RetCode_PTHERR;
                }
            }
        }
        res = enq(ptr);
        if(wt_cons_) {
            wt_cons_ = 0;
            mon_.notify_all();
        }
    }
    return res;
}

RetCode b_qu::put(const void *ptr)
{
    RetCode res = RetCode_OK;
    if(!ptr) {
        return RetCode_BADARG;
    }
    scoped_mx smx(mon_);
    while(!remain_capacity()) {
        wt_prod_++;
        mon_.wait();
    }
    res = enq(ptr);
    if(wt_cons_) {
        if(wt_cons_ > 1) {
            mon_.notify_all();
        } else {
            mon_.notify();
        }
        wt_cons_ = 0;
    }
    return res;
}

RetCode b_qu::peek(void *copy)
{
    scoped_mx smx(mon_);
    while(!elems_cnt()) {
        wt_cons_++;
        mon_.wait();
    }
    if(copy) {
        manager_.copy_obj(copy, head_->ptr_);
    }
    return RetCode_OK;
}

}
