/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#pragma once
#include "vlg.h"

#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <vector>

#define FNV_32_PRIME ((uint32_t)0x01000193)

void MurmurHash3_x86_32(const void *key, int len, uint32_t seed, void *out);

namespace vlg {

//obj_mng func types

typedef void *(*alloc_func)(size_t type_size,
                            const void *copy);

typedef void(*dealloc_func)(void *ptr);

typedef void *(*cpy_func)(void *copy,
                          const void *ptr,
                          size_t type_size);

typedef int(*cmp_func)(const void *obj1,
                       const void *obj2,
                       size_t len);

typedef void(*hash_func)(const void *key,
                         int len,
                         uint32_t seed,
                         void *out);

typedef void *(*rplc_on_hit_func)(void *hit,
                                  const void *rplcr,
                                  size_t type_size);

//obj_mng default func

inline void *def_alloc_func(size_t type_size, const void *copy)
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

inline void *cstr_alloc_func(size_t type_size, const void *copy)
{
    void *new_ptr = strdup((char *)copy);
    if(!new_ptr) {
        return nullptr;
    }
    return new_ptr;
}

inline void *cstr_cpy_func(void *copy, const void *ptr, size_t type_size)
{
    return strcpy((char *)copy, (char *)ptr);
}

inline int cstr_cmp_func(const void *obj1, const void *obj2, size_t len)
{
    return strcmp((char *)obj1, (char *)obj2);
}

inline void cstr_hash_func(const void *key, int len, uint32_t seed, void *out)
{
    MurmurHash3_x86_32(key, (int)strlen((char *)key), seed, out);
}

/** @brief Object Manager.
*/
struct obj_mng {
    explicit obj_mng(size_t type_size) :
        seed_(FNV_32_PRIME),
        type_size_(type_size),
        alloc_func_(def_alloc_func),
        dealloc_func_(free),
        cmp_func_(memcmp),
        cpy_func_(memcpy),
        hash_func_(MurmurHash3_x86_32),
        rplc_func_(nullptr) {}

    explicit obj_mng(const obj_mng &other) :
        seed_(other.seed_),
        type_size_(other.type_size_),
        alloc_func_(other.alloc_func_),
        dealloc_func_(other.dealloc_func_),
        cmp_func_(other.cmp_func_),
        cpy_func_(other.cpy_func_),
        hash_func_(other.hash_func_),
        rplc_func_(other.rplc_func_) {}

    explicit obj_mng(size_t type_size,
                     alloc_func alloc_f,
                     dealloc_func dealloc_f,
                     cmp_func cmp_f,
                     cpy_func copy_f,
                     hash_func hash_f,
                     rplc_on_hit_func rplc_f) :
        seed_(FNV_32_PRIME),
        type_size_(type_size),
        alloc_func_(alloc_f),
        dealloc_func_(dealloc_f),
        cmp_func_(cmp_f),
        cpy_func_(copy_f),
        hash_func_(hash_f),
        rplc_func_(rplc_f) {}

    uint32_t seed_;
    size_t type_size_;
    alloc_func alloc_func_;
    dealloc_func dealloc_func_;
    cmp_func cmp_func_;
    cpy_func cpy_func_;
    hash_func hash_func_;
    rplc_on_hit_func rplc_func_;
};

/** @brief Pointer Object Manager.
*/
struct ptr_obj_mng : public obj_mng {
    ptr_obj_mng();
    static ptr_obj_mng *instance_;
};

/** @brief c-string Object Manager.
*/
struct cstr_obj_mng : public obj_mng {
    cstr_obj_mng();
    static cstr_obj_mng *instance_;
};

const ptr_obj_mng &sngl_ptr_obj_mng();
const cstr_obj_mng &sngl_cstr_obj_mng();

}

namespace vlg {

struct brep {
    brep(): elemcount_(0) {}
    void dcr_elems() {
        elemcount_--;
    }
    void incr_elems() {
        elemcount_++;
    }
    void rst_elems() {
        elemcount_ = 0;
    }
    uint32_t elemcount_;
};

struct s_hm;

typedef void(*s_hm_enm_func)(const s_hm &map,
                             const void *key,
                             void *ptr,
                             void *ud);

typedef void(*s_hm_enm_func_br)(const s_hm &map,
                                const void *key,
                                void *ptr,
                                void *ud,
                                bool &brk);

const unsigned int HMSz_23 = 23U;
const unsigned int HMSz_71 = 71U;
const unsigned int HMSz_101 = 101U;
const unsigned int HMSz_1031 = 1031U;
const unsigned int HMSz_10037 = 10037U;
const unsigned int HMSz_33013 = 33013U;
const unsigned int HMSz_65557 = 65557U;
const unsigned int HMSz_130343 = 130343U;
const unsigned int HMSz_257177 = 257177U;
const unsigned int HMSz_517411 = 517411U;
const unsigned int HMSz_1031117 = 1031117U;

/** @brief Basic thread safe Hash-Map.
*/
struct hm_node;
struct s_hm : public brep {
        explicit s_hm(uint32_t hash_size,
                      size_t elem_size,
                      size_t key_size) :
            elem_manager_(elem_size),
            key_manager_(key_size),
            hash_size_(hash_size),
            buckets_((hm_node * *)calloc(hash_size_, sizeof(hm_node *))),
            head_(nullptr),
            tail_(nullptr),
            it_(nullptr),
            prev_(nullptr)
        {}

        explicit s_hm(uint32_t hash_size,
                      const obj_mng &elem_manager,
                      const obj_mng &key_manager) :
            elem_manager_(elem_manager),
            key_manager_(key_manager),
            hash_size_(hash_size),
            buckets_((hm_node * *)calloc(hash_size_, sizeof(hm_node *))),
            head_(nullptr),
            tail_(nullptr),
            it_(nullptr),
            prev_(nullptr)
        {}

        explicit s_hm(uint32_t hash_size,
                      size_t elem_size,
                      const obj_mng &key_manager) :
            elem_manager_(elem_size),
            key_manager_(key_manager),
            hash_size_(hash_size),
            buckets_((hm_node * *)calloc(hash_size_, sizeof(hm_node *))),
            head_(nullptr),
            tail_(nullptr),
            it_(nullptr),
            prev_(nullptr)
        {}

        explicit s_hm(uint32_t hash_size,
                      const obj_mng &elem_manager,
                      size_t key_size) :
            elem_manager_(elem_manager),
            key_manager_(key_size),
            hash_size_(hash_size),
            buckets_((hm_node * *)calloc(hash_size_, sizeof(hm_node *))),
            head_(nullptr),
            tail_(nullptr),
            it_(nullptr),
            prev_(nullptr)
        {}

        ~s_hm();

        RetCode clear();

        /**
        @return RetCode_OK if the specified key is mapped,
        RetCode_KO if this map contains no mapping for the key or
        RetCode_BDARG if key is nullptr.
        */
        RetCode contains_key(const void *key) const;

        /**
        @return RetCode_OK if the specified key is mapped,
        RetCode_KO if this map contains no mapping for the key, or
        RetCode_BDARG if key is nullptr.
        */
        RetCode get(const void *key,
                    void *copy) const;

        bool is_empty() const {
            return elemcount_ == 0;
        }

        RetCode remove(const void *key,
                       void *copy);

        RetCode put(const void *key,
                    const void *ptr);

        uint32_t size() const {
            return elemcount_;
        }

        void enum_elements_safe_read(s_hm_enm_func enum_f,
                                     void *ud) const {
            std::shared_lock<std::shared_mutex> lock(mutex_);
            enm(*this, enum_f, ud);
        }

        void enum_elements_safe_write(s_hm_enm_func enum_f,
                                      void *ud) const {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            enm(*this, enum_f, ud);
        }

        void enum_elements_breakable_safe_read(s_hm_enm_func_br enum_f,
                                               void *ud) const {
            std::shared_lock<std::shared_mutex> lock(mutex_);
            enmbr(*this, enum_f, ud);
        }

        void enum_elements_breakable_safe_write(s_hm_enm_func_br enum_f,
                                                void *ud) const {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            enmbr(*this, enum_f, ud);
        }

    protected:
        uint32_t gidx(const void *key) const;
        void rm(hm_node *del_mn, uint32_t idx);
        void enm(const s_hm &map, s_hm_enm_func enum_f, void *ud) const;
        void enmbr(const s_hm &map, s_hm_enm_func_br enum_f, void *ud) const;

    public:
        const obj_mng elem_manager_, key_manager_;
        uint32_t hash_size_;
        hm_node **buckets_;
        hm_node *head_, *tail_, *it_, *prev_;
        mutable std::shared_mutex mutex_;
};

/** @brief Blocking-Queue.
*/
struct lnk_node;
struct b_qu : public brep {
        typedef void (*rm_idx_func)(b_qu &, const void *);

        explicit b_qu(size_t elemsize,
                      uint32_t capacity = 0) :
            manager_(elemsize),
            capacity_(capacity),
            head_(nullptr),
            tail_(nullptr),
            rif_(nullptr),
            wt_prod_(0),
            wt_cons_(0)
        {}

        explicit b_qu(const obj_mng &elem_manager,
                      uint32_t capacity = 0) :
            manager_(elem_manager),
            capacity_(capacity),
            head_(nullptr),
            tail_(nullptr),
            rif_(nullptr),
            wt_prod_(0),
            wt_cons_(0)
        {}

        ~b_qu();

        bool is_empty() const {
            return elemcount_ == 0;
        }

        uint32_t size() const {
            return elemcount_;
        }

        uint32_t remain_capacity() const {
            return !capacity_ ? 1 : (capacity_ - elemcount_);
        }

        RetCode clear();

        RetCode take(void *copy);

        RetCode take(time_t sec,
                     long nsec,
                     void *copy);

        RetCode put(const void *ptr);

        RetCode put(time_t sec,
                    long nsec,
                    const void *ptr);

        RetCode peek(void *copy);

        RetCode peek(time_t sec,
                     long nsec,
                     void *copy);

    protected:
        void wtCons(time_t sec, long nsec, std::unique_lock<std::mutex> &lck, RetCode &res);
        void wtProd(time_t sec, long nsec, std::unique_lock<std::mutex> &lck, RetCode &res);

        void dq(void *copy);
        RetCode enq(const void *ptr, bool idxed = false);

        const obj_mng manager_;
        uint32_t capacity_;
        lnk_node *head_, *tail_;
        rm_idx_func rif_;
        uint32_t wt_prod_, wt_cons_;

        mutable std::mutex mtx_;
        mutable std::condition_variable cv_;
};

/** @brief Blocking-Queue with hash-map capability.
*/
struct b_qu_hm : public b_qu {
        explicit b_qu_hm(size_t elemsize,
                         uint32_t capacity = 0);

        explicit b_qu_hm(const obj_mng &elem_manager,
                         uint32_t capacity = 0);

        ~b_qu_hm();

        RetCode clear();

        RetCode put_or_update(const void *ptr);

        RetCode put_or_update(time_t sec,
                              long nsec,
                              const void *ptr);

    protected:
        uint32_t gidx(const void *ptr) const;
        void *hmp(const void *ptr);
        static void hmr(b_qu &self, const void *ptr);

        hm_node **buckets_;
        hm_node *hhead_, *htail_;
};

/** @brief object manager for std shared pointers.
*/
template <typename T>
struct std_shared_ptr_obj_mng : public obj_mng {
    static void *shared_ptr_alloc_func(size_t type_size, const void *copy) {
        std::shared_ptr<T> &sh_ptr_cpy = *(std::shared_ptr<T> *)(copy);
        std::shared_ptr<T> *new_sh_ptr = new std::shared_ptr<T>(sh_ptr_cpy);
        return new_sh_ptr;
    }

    static void shared_ptr_dealloc_func(void *ptr) {
        std::shared_ptr<T> *sh_ptr = (std::shared_ptr<T> *)(ptr);
        delete sh_ptr;
    }

    static void *shared_ptr_cpy_func(void *copy, const void *ptr, size_t type_size) {
        std::shared_ptr<T> &sh_ptr_cpy = *(std::shared_ptr<T> *)(copy);
        std::shared_ptr<T> &sh_ptr_ptr = *(std::shared_ptr<T> *)(ptr);
        sh_ptr_cpy = sh_ptr_ptr;
        return copy;
    }

    explicit std_shared_ptr_obj_mng() : obj_mng(0,
                                                    shared_ptr_alloc_func,
                                                    shared_ptr_dealloc_func,
                                                    0,
                                                    shared_ptr_cpy_func,
                                                    0,
                                                    0) {}

    explicit std_shared_ptr_obj_mng(size_t type_size,
                                    alloc_func alloc_f,
                                    dealloc_func dealloc_f,
                                    cmp_func cmp_f,
                                    cpy_func copy_f,
                                    hash_func hash_f,
                                    rplc_on_hit_func rplc_f) : obj_mng(type_size,
                                                                           alloc_f,
                                                                           dealloc_f,
                                                                           cmp_f,
                                                                           copy_f,
                                                                           hash_f,
                                                                           rplc_f) {}
};

/** @brief object manager for std unique pointers.
*/
template <typename T>
struct std_unique_ptr_obj_mng : public obj_mng {
    static void *unique_ptr_alloc_func(size_t type_size, const void *copy) {
        std::unique_ptr<T> &uq_ptr_cpy = *(std::unique_ptr<T> *)(copy);
        std::unique_ptr<T> *new_uq_ptr = new std::unique_ptr<T>(std::move(uq_ptr_cpy));
        return new_uq_ptr;
    }

    static void unique_ptr_dealloc_func(void *ptr) {
        std::unique_ptr<T> *uq_ptr = (std::unique_ptr<T> *)(ptr);
        delete uq_ptr;
    }

    static void *unique_ptr_cpy_func(void *copy, const void *ptr, size_t type_size) {
        std::unique_ptr<T> &uq_ptr_cpy = *(std::unique_ptr<T> *)(copy);
        std::unique_ptr<T> &uq_ptr_ptr = *(std::unique_ptr<T> *)(ptr);
        uq_ptr_cpy = std::move(uq_ptr_ptr);
        return copy;
    }

    explicit std_unique_ptr_obj_mng() : obj_mng(0,
                                                    unique_ptr_alloc_func,
                                                    unique_ptr_dealloc_func,
                                                    0,
                                                    unique_ptr_cpy_func,
                                                    0,
                                                    0) {}

    explicit std_unique_ptr_obj_mng(size_t type_size,
                                    alloc_func alloc_f,
                                    dealloc_func dealloc_f,
                                    cmp_func cmp_f,
                                    cpy_func copy_f,
                                    hash_func hash_f,
                                    rplc_on_hit_func rplc_f) : obj_mng(type_size,
                                                                           alloc_f,
                                                                           dealloc_f,
                                                                           cmp_f,
                                                                           copy_f,
                                                                           hash_f,
                                                                           rplc_f) {}
};

}
