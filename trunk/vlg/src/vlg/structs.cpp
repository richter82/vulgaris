/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "glob.h"

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

//obj_mng

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
                                           cstr_hash_func,
                                           nullptr) {}

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
            void *key_ptr = nullptr) :
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
            elem_manager_.dealloc_func_(cur_mn->ptr_);
            key_manager_.dealloc_func_(cur_mn->key_ptr_);
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

inline uint32_t s_hm::gidx(const void *key) const
{
    uint32_t hash_code;
    key_manager_.hash_func_(key, (int)key_manager_.type_size_, key_manager_.seed_, &hash_code);
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
    elem_manager_.dealloc_func_(del_mn->ptr_);
    key_manager_.dealloc_func_(del_mn->key_ptr_);
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
        elem_manager_.dealloc_func_(cur_mn->ptr_);
        key_manager_.dealloc_func_(cur_mn->key_ptr_);
        delete cur_mn;
        cur_mn = next;
    }
    memset(buckets_, 0, (hash_size_*sizeof(hm_node *)));
    head_ = tail_ = nullptr;
    rst_elems();
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
        if(!key_manager_.cmp_func_(m_node->key_ptr_, key, key_manager_.type_size_)) {
            if(copy) {
                elem_manager_.cpy_func_(copy, m_node->ptr_, elem_manager_.type_size_);
            }
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
            if(!key_manager_.cmp_func_(cur_node->key_ptr_, key, key_manager_.type_size_)) {
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
            void *elem_ptr = elem_manager_.alloc_func_(elem_manager_.type_size_, ptr);
            void *key_ptr = key_manager_.alloc_func_(key_manager_.type_size_, key);
            hm_node *new_node = new hm_node(elem_ptr, key_ptr);
            cur_node->next_ = new_node;
            new_node->prev_ = cur_node;
            tail_->insrt_next_ = new_node;
            new_node->insrt_prev_ = tail_;
            tail_ = new_node;
            incr_elems();
        } else {
            cur_node->ptr_ = elem_manager_.cpy_func_(cur_node->ptr_, ptr, elem_manager_.type_size_);
        }
    } else {
        void *elem_ptr = elem_manager_.alloc_func_(elem_manager_.type_size_, ptr);
        void *key_ptr = key_manager_.alloc_func_(key_manager_.type_size_, key);
        hm_node *new_node = new hm_node(elem_ptr, key_ptr);
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
        if(!key_manager_.cmp_func_(m_node->key_ptr_, key, key_manager_.type_size_)) {
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
        if(!key_manager_.cmp_func_(m_node->key_ptr_, key, key_manager_.type_size_)) {
            if(copy) {
                elem_manager_.cpy_func_(copy, m_node->ptr_, elem_manager_.type_size_);
            }
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
    lnk_node(void *ptr, bool indexed = false) :
        ptr_(ptr),
        next_(nullptr),
        previous_(nullptr),
        indexed_(indexed) {}
    void *ptr_;
    lnk_node *next_;
    lnk_node *previous_;
    bool indexed_;
};

// blocking_queue

b_qu::b_qu(size_t elemsize,
           uint32_t capacity) :
    manager_(elemsize),
    capacity_(capacity),
    head_(nullptr),
    tail_(nullptr),
    rif_(nullptr),
    wt_prod_(0),
    wt_cons_(0)
{}

b_qu::b_qu(const obj_mng &elem_manager,
           uint32_t capacity) :
    manager_(elem_manager),
    capacity_(capacity),
    head_(nullptr),
    tail_(nullptr),
    rif_(nullptr),
    wt_prod_(0),
    wt_cons_(0)
{}

b_qu::~b_qu()
{
    lnk_node *cur = tail_, *next = nullptr;
    while(cur != nullptr) {
        next = cur->next_;
        manager_.dealloc_func_(cur->ptr_);
        delete cur;
        cur = next;
    }
}

void b_qu::dq(void *copy)
{
    lnk_node *new_head = head_->previous_;
    if(head_->indexed_) {
        rif_(*this, head_->ptr_);
    }
    if(copy) {
        manager_.cpy_func_(copy, head_->ptr_, manager_.type_size_);
    }
    manager_.dealloc_func_(head_->ptr_);
    delete head_;
    if(new_head) {
        head_ = new_head;
        head_->next_ = nullptr;
    } else {
        tail_ = head_ = nullptr;
    }
    dcr_elems();
}

RetCode b_qu::enq(const void *ptr, bool idxed)
{
    void *new_obj = (!idxed ? manager_.alloc_func_(manager_.type_size_, ptr) : const_cast<void *>(ptr));
    lnk_node *new_tail = new lnk_node(new_obj, idxed);
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
        manager_.dealloc_func_(cur->ptr_);
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

RetCode b_qu::take(void *copy)
{
    scoped_mx smx(mon_);
    while(!elemcount_) {
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

RetCode b_qu::take(time_t sec, long nsec, void *copy)
{
    RetCode res = RetCode_OK;
    scoped_mx smx(mon_);
    if(!nsec && !sec) {
        if(elemcount_) {
            dq(copy);
        } else {
            res = RetCode_EMPTY;
        }
    } else {
        while(!elemcount_) {
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
        if(elemcount_) {
            if(copy) {
                manager_.cpy_func_(copy, head_->ptr_, manager_.type_size_);
            }
        } else {
            res = RetCode_EMPTY;
        }
    } else {
        while(!elemcount_) {
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
            manager_.cpy_func_(copy, head_->ptr_, manager_.type_size_);
        }
    }
    return res;
}

RetCode b_qu::put(time_t sec, long nsec, const void *ptr)
{
    if(!ptr) {
        return RetCode_BADARG;
    }
    RetCode res = RetCode_OK;
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
    if(!ptr) {
        return RetCode_BADARG;
    }
    RetCode res = RetCode_OK;
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
    while(!elemcount_) {
        wt_cons_++;
        mon_.wait();
    }
    if(copy) {
        manager_.cpy_func_(copy, head_->ptr_, manager_.type_size_);
    }
    return RetCode_OK;
}

// hm blocking_queue

#define B_QU_HM_SZ HMSz_1031

b_qu_hm::b_qu_hm(size_t elemsize,
                 uint32_t capacity) :
    b_qu(elemsize, capacity),
    buckets_((hm_node * *)calloc(B_QU_HM_SZ, sizeof(hm_node *))),
    hhead_(nullptr),
    htail_(nullptr)
{
    rif_ = hmr;
}

b_qu_hm::b_qu_hm(const obj_mng &elem_manager,
                 uint32_t capacity) :
    b_qu(elem_manager, capacity),
    buckets_((hm_node * *)calloc(B_QU_HM_SZ, sizeof(hm_node *))),
    hhead_(nullptr),
    htail_(nullptr)
{
    rif_ = hmr;
}

b_qu_hm::~b_qu_hm()
{
    if(buckets_) {
        hm_node *cur_mn = hhead_, *next = nullptr;
        while(cur_mn != nullptr) {
            next = cur_mn->insrt_next_;
            delete cur_mn;
            cur_mn = next;
        }
        free(buckets_);
    }
}

inline uint32_t b_qu_hm::gidx(const void *ptr) const
{
    uint32_t hash_code;
    manager_.hash_func_(ptr, (int)manager_.type_size_, manager_.seed_, &hash_code);
    return hash_code % B_QU_HM_SZ;
}

void *b_qu_hm::hmp(const void *ptr)
{
    uint32_t idx = gidx(ptr);
    hm_node *cur_node = buckets_[idx];
    bool is_new = true;
    void *elem_ptr = nullptr;
    if(cur_node) {
        do {
            if(!manager_.cmp_func_(cur_node->ptr_, ptr, manager_.type_size_)) {
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
            elem_ptr = manager_.alloc_func_(manager_.type_size_, ptr);
            hm_node *new_node = new hm_node(elem_ptr);
            cur_node->next_ = new_node;
            new_node->prev_ = cur_node;
            htail_->insrt_next_ = new_node;
            new_node->insrt_prev_ = htail_;
            htail_ = new_node;
        } else {
            cur_node->ptr_ = manager_.rplc_func_(cur_node->ptr_, ptr, manager_.type_size_);
        }
    } else {
        elem_ptr = manager_.alloc_func_(manager_.type_size_, ptr);
        hm_node *new_node = new hm_node(elem_ptr);
        buckets_[idx] = new_node;
        if(!hhead_) {
            hhead_ = new_node;
        } else {
            htail_->insrt_next_ = new_node;
        }
        new_node->insrt_prev_ = htail_;
        htail_ = new_node;
    }
    return elem_ptr;
}

void b_qu_hm::hmr(b_qu &self, const void *ptr)
{
    b_qu_hm &hself = static_cast<b_qu_hm &>(self);
    uint32_t idx = hself.gidx(ptr);
    hm_node *del_mn = hself.buckets_[idx];
    while(del_mn) {
        if(!hself.manager_.cmp_func_(del_mn->ptr_, ptr, hself.manager_.type_size_)) {
            if(del_mn->prev_) {
                del_mn->prev_->next_ = del_mn->next_;
            } else {
                hself.buckets_[idx] = del_mn->next_;
            }
            if(del_mn->next_) {
                del_mn->next_->prev_ = del_mn->prev_;
            }
            if(del_mn->insrt_prev_) {
                del_mn->insrt_prev_->insrt_next_ = del_mn->insrt_next_;
            } else if(hself.hhead_ == del_mn) {
                hself.hhead_ = del_mn->insrt_next_;
            }
            if(del_mn->insrt_next_) {
                del_mn->insrt_next_->insrt_prev_ = del_mn->insrt_prev_;
            } else if(hself.htail_ == del_mn) {
                hself.htail_ = del_mn->insrt_prev_;
            }
            delete del_mn;
            return;
        }
        del_mn = del_mn->next_;
    }
}

RetCode b_qu_hm::clear()
{
    scoped_mx smx(mon_);
    hm_node *cur_mn = hhead_, *hnext = nullptr;
    while(cur_mn != nullptr) {
        hnext = cur_mn->insrt_next_;
        delete cur_mn;
        cur_mn = hnext;
    }
    memset(buckets_, 0, (B_QU_HM_SZ * sizeof(hm_node *)));
    hhead_ = htail_ = nullptr;
    lnk_node *cur = tail_, *next = nullptr;
    while(cur != nullptr) {
        next = cur->next_;
        manager_.dealloc_func_(cur->ptr_);
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

RetCode b_qu_hm::put_or_update(const void *ptr)
{
    if(!ptr) {
        return RetCode_BADARG;
    }
    RetCode res = RetCode_OK;
    scoped_mx smx(mon_);
    void *elem_ptr = hmp(ptr);
    if(elem_ptr) {
        while(!remain_capacity()) {
            wt_prod_++;
            mon_.wait();
        }
        res = enq(elem_ptr, true);
        if(wt_cons_) {
            if(wt_cons_ > 1) {
                mon_.notify_all();
            } else {
                mon_.notify();
            }
            wt_cons_ = 0;
        }
    }
    return res;
}

RetCode b_qu_hm::put_or_update(time_t sec,
                               long nsec,
                               const void *ptr)
{
    if(!ptr) {
        return RetCode_BADARG;
    }
    RetCode res = RetCode_OK;
    scoped_mx smx(mon_);
    void *elem_ptr = hmp(ptr);
    if(elem_ptr) {
        if(!nsec && !sec) {
            if(remain_capacity()) {
                res = enq(elem_ptr, true);
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
            res = enq(elem_ptr, true);
            if(wt_cons_) {
                wt_cons_ = 0;
                mon_.notify_all();
            }
        }
    }
    return res;
}

}
