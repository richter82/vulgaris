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

#include "vlg_memory.h"
#include "vlg_logger.h"
#include "vlg_globint.h"

namespace vlg {

// Collectable
collectable::~collectable() {}

// reference_counter
class reference_counter {
    public:
        reference_counter();
        ~reference_counter();
        unsigned int ref_counter_;
        bool delete_requested_;
};

reference_counter::reference_counter() : ref_counter_(0),
    delete_requested_(false)
{}

reference_counter::~reference_counter()
{}

// collector_impl
#define COLL_CLASS_NAME_LEN 256

struct coll_impl_enum_inst_supp {
    collector &coll_;
    collector::enum_coll_inst clbk_;
    void *ud;
};

void collector_impl_enum_collected_instances(const synch_hash_map &map,
                                             const void *key,
                                             const void *ptr,
                                             void *ud)
{
    coll_impl_enum_inst_supp *cieis = (coll_impl_enum_inst_supp *)ud;
    cieis->clbk_(cieis->coll_, *(void **)key, *(reference_counter **)ptr,
                 cieis->ud);
}

class collector_impl {
    public:
        explicit collector_impl(const char *class_name) :
            id_(get_next_id()),
            inst_collector_(sngl_ptr_obj_mng(), sngl_ptr_obj_mng()) {
            log_ = logger::get_logger("collector_impl");
            strncpy(class_name_, class_name, COLL_CLASS_NAME_LEN);
            if(inst_collector_.init(HM_SIZE_NORM)) {
                EXIT_ACTION
            }
            IFLOG(trc(TH_ID, LS_CTR "%s", __func__))
        }

        ~collector_impl() {
            IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
        }

        const char *get_class_name() {
            return class_name_;
        }

        void enum_collected_instances(collector &coll,
                                      collector::enum_coll_inst clbk,
                                      void *ud) {
            coll_impl_enum_inst_supp cieis = {coll, clbk, ud};
            inst_collector_.enum_elements_safe_read(collector_impl_enum_collected_instances,
                                                    &cieis);
        }

        synch_monitor get_collector_synch_monitor() {
            return mon_;
        }

        synch_hash_map instance_collector() {
            return inst_collector_;
        }

        void retain(collectable *ptr) {
            reference_counter *rfc = nullptr;
            mon_.lock();
            if(inst_collector_.get(&ptr, &rfc)) {
                rfc = new reference_counter();
                inst_collector_.put(&ptr, &rfc);
            }
            rfc->ref_counter_++;
            mon_.unlock();
        }

        bool is_instance_collected(collectable *ptr) {
            return (inst_collector_.contains_key(&ptr) == RetCode_OK);
        }

        RetCode release(collectable *ptr) {
            RetCode res = RetCode_MEMNOTR;
            mon_.lock();
            reference_counter *rfc = nullptr;
            if(!(inst_collector_.get(&ptr, &rfc))) {
                if(rfc->ref_counter_) {
                    if(!--rfc->ref_counter_) {
                        inst_collector_.remove(&ptr, nullptr);
                        IFLOG(trc(TH_ID,
                                  LS_TRL "%s(ptr:%p, class:%s) - [deleted] [ref-count:%d]",
                                  __func__,
                                  ptr,
                                  class_name_,
                                  rfc->ref_counter_))
                        delete rfc;
                        delete ptr;
                        res = RetCode_MEMRLSD;
                    }
                }
                IFLOG(trc(TH_ID, LS_TRL
                          "%s(ptr:%p, class:%s) - [released by this thread] [ref-count:%u]",
                          __func__,
                          ptr,
                          class_name_,
                          rfc->ref_counter_))
            } else {
                IFLOG(trc(TH_ID, LS_TRL
                          "%s(ptr:%p, class:%s) - [cannot release an uncollected instance]",
                          __func__,
                          ptr,
                          class_name_))
            }
            mon_.unlock();
            return res;
        }

    private:
        unsigned int        id_;
        char                class_name_[COLL_CLASS_NAME_LEN];
        synch_monitor       mon_;
        synch_hash_map      inst_collector_;
        static logger       *log_;
        static unsigned int nextid_;

        static unsigned int get_next_id() {
            return ++nextid_;
        }
};

logger *collector_impl::log_ = nullptr;
unsigned int collector_impl::nextid_ = 0;

// collector
collector::collector(const char *class_name)
{
    impl_ = new collector_impl(class_name);
    collector_stat::get_instance().add_collector(*this);
}

collector::~collector()
{
    if(impl_) {
        delete impl_;
    }
}

const char *collector::get_class_name() const
{
    return impl_->get_class_name();
}

void collector::enum_collected_instances(enum_coll_inst clbk, void *ud)
{
    impl_->enum_collected_instances(*this, clbk, ud);
}

bool collector::is_instance_collected(collectable *ptr)
{
    return impl_->is_instance_collected(ptr);
}

void collector::retain(collectable *ptr)
{
    impl_->retain(ptr);
}

RetCode collector::release(collectable *ptr)
{
    return impl_->release(ptr);
}

// collector_stat
void collector_stat_impl_enum(const collector &coll,
                              const void *ptr,
                              const void *ref_count,
                              void *ud);

class collector_stat_impl : public p_thread {
    public:
        collector_stat_impl(collector_stat &publ) : publ_(publ),
            freq_sec_(0),
            level_(TL_TRC),
            collector_map_(sngl_ptr_obj_mng(), sngl_cstr_obj_mng()) {
            log_ = logger::get_logger("collector_stat");
            collector_map_.init(HM_SIZE_TINY);
        }

        ~collector_stat_impl() {
            stop();
        }

        collector_stat &get_collector_stat() {
            return publ_;
        }

        unsigned int freq_sec() const {
            return freq_sec_;
        }

        void freq_sec(unsigned int val) {
            freq_sec_ = val;
        }

        TraceLVL level() const {
            return level_;
        }

        void level(TraceLVL val) {
            level_ = val;
        }

        RetCode add_collector(collector &coll) {
            void *coll_ptr = &coll;
            return collector_map_.put(coll.get_class_name(), &coll_ptr);
        }

        virtual void *run() {
            while(true) {
                mssleep(freq_sec_*1000);
                collector_map_.start_iteration();
                collector *curr_coll = nullptr;
                IFLOG(log(level_, TH_ID, LS_STA "*** memory-coll-stats-begin"))
                while(!collector_map_.next(nullptr, &curr_coll)) {
                    curr_coll->enum_collected_instances(collector_stat_impl_enum,
                                                        this);
                }
                IFLOG(log(level_, TH_ID, LS_STA "*** memory-coll-stats-end"))
            }
            return 0;
        }

    private:
        collector_stat &publ_;
        unsigned int freq_sec_;
        TraceLVL level_;
        hash_map collector_map_; //collector class --> collector

    public:
        static logger *log_;
};

void collector_stat_impl_enum(const collector &coll,
                              const void *ptr,
                              const void *ref_count,
                              void *ud)
{
    collector_stat_impl *csi = (collector_stat_impl *) ud;
    reference_counter *ref_c = (reference_counter *)ref_count;
    IFLOG2(collector_stat_impl::log_, log(csi->level(),
                                          TH_ID,
                                          LS_STA "memory-coll-class:%s [%p][%u]",
                                          coll.get_class_name(),
                                          ptr,
                                          ref_c->ref_counter_))
}

logger *collector_stat_impl::log_ = nullptr;

collector_stat *coll_stat_instance_ = nullptr;
collector_stat &coll_stat_instance()
{
    if(coll_stat_instance_  == nullptr) {
        coll_stat_instance_ = new collector_stat();
        if(!coll_stat_instance_) {
            EXIT_ACTION
        }
    }
    return *coll_stat_instance_;
}

collector_stat &collector_stat::get_instance()
{
    return coll_stat_instance();
}

collector_stat::collector_stat()
{
    impl_ = new collector_stat_impl(*this);
}

collector_stat::~collector_stat()
{
    if(impl_) {
        delete impl_;
    }
}

RetCode collector_stat::add_collector(collector &coll)
{
    return impl_->add_collector(coll);
}

RetCode collector_stat::start_monitoring(unsigned int freq_sec,
                                         TraceLVL at_level)
{
    impl_->freq_sec(freq_sec);
    impl_->level(at_level);
    return impl_->start() ? RetCode_GENERR : RetCode_OK;
}

RetCode collector_stat::stop_monitoring()
{
    return impl_->stop() ? RetCode_GENERR : RetCode_OK;
}

}


