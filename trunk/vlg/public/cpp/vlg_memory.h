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

#ifndef VLG_MEM_H_
#define VLG_MEM_H_
#include "vlg.h"

namespace vlg {

/** @brief collectable interface.
*/
class collector;
class collectable {
    public:
        virtual ~collectable();
        virtual collector &get_collector() = 0;
};

/** @brief collector class.
Collector class for raw pointers.
*/
class collector_impl;
class collector {
    public:
        typedef void(*enum_coll_inst)(const collector &coll,
                                      const void *ptr,
                                      const void *ref_count,
                                      void *ud);

    public:
        explicit collector(const char *class_name);
        virtual ~collector();

    public:
        const char  *get_class_name()    const;
        void        enum_collected_instances(enum_coll_inst clbk,
                                             void *ud = nullptr);

        bool        is_instance_collected(collectable *ptr);
        void        retain(collectable *ptr);
        RetCode     release(collectable *ptr);

    private:
        collector_impl *impl_;
};

/** @brief shared_pointer class.
*/
template <typename T>
class shared_pointer {
    private:
        class shared_pointer_reference_count {
            public:
                explicit shared_pointer_reference_count(unsigned int reference_count) :
                    reference_count_(reference_count) {
                    pthread_mutex_init(&mutex_, nullptr);
                }

                ~shared_pointer_reference_count() {
                    pthread_mutex_destroy(&mutex_);
                }

                void increment_ref_count() {
                    pthread_mutex_lock(&mutex_);
                    reference_count_++;
                    pthread_mutex_unlock(&mutex_);
                }

                void decrement_ref_count() {
                    pthread_mutex_lock(&mutex_);
                    if(reference_count_) {
                        reference_count_--;
                    }
                    pthread_mutex_unlock(&mutex_);
                }

                unsigned int get_ref_count() {
                    return reference_count_;
                }

            private:
                unsigned int reference_count_;
                pthread_mutex_t mutex_;
        };

    public:
        /**
        */
        explicit shared_pointer() :
            ptr_(nullptr),
            reference_counter_(nullptr) {
        }

        /**
        @param ptr
        */
        explicit shared_pointer(T *ptr) :
            ptr_(ptr),
            reference_counter_(nullptr) {
            if(ptr_) {
                reference_counter_ = new shared_pointer_reference_count(1);
            }
        }

        /**
        @param other
        */
        explicit shared_pointer(shared_pointer &other):
            ptr_(other.ptr_),
            reference_counter_(other.reference_counter_) {
            if(reference_counter_) {
                reference_counter_->increment_ref_count();
            }
        }

        /**
        */
        ~shared_pointer() {
            release();
        }

        /**
        @return
        */
        shared_pointer<T> *make_new_shared() {
            return new shared_pointer<T>(*this);
        }

        /**
        @param other
        @return
        */
        bool equals(const shared_pointer<T> &other) const {
            return reference_counter_ == other.reference_counter_;
        }

        /**
        @return
        */
        bool is_null_pointer() {
            return ptr_ == nullptr;
        }

        /**
        */
        void release() {
            if(reference_counter_) {
                reference_counter_->decrement_ref_count();
                if(!reference_counter_->get_ref_count()) {
                    delete reference_counter_;
                    if(ptr_) {
                        delete ptr_;
                        ptr_ = nullptr;
                    }
                }
                reference_counter_ = nullptr;
            }
        }

        /**
        @param ptr
        */
        void set_pointer(T *ptr) {
            release();
            ptr_ = ptr;
            if(ptr_) {
                reference_counter_ = new shared_pointer_reference_count(1);
            }
        }

        /**
        @return
        */
        const void *get_internal_reference_counter() const {
            return reference_counter_;
        }

        /**
        @return
        */
        T *ptr() {
            return ptr_;
        }

        /**
        @param other
        @return
        */
        shared_pointer &operator = (shared_pointer &other) {
            release();
            reference_counter_ = other.reference_counter_;
            ptr_ = other.ptr_;
            if(reference_counter_) {
                reference_counter_->increment_ref_count();
            }
            return *this;
        }

    private:
        T *ptr_;
        shared_pointer_reference_count *reference_counter_;
};

}

#endif
