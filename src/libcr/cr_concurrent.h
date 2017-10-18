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

#ifndef CR_CONCURRENT_H_
#define CR_CONCURRENT_H_
#include "vlg.h"

namespace vlg {

class blocking_queue;
class p_executor_service;

/** @brief Synch-Monitor class.
*/
class synch_mon_impl;
class synch_monitor {
    public:

        /**
        @param pshared
        */
        explicit synch_monitor(int pshared = PTHREAD_PROCESS_PRIVATE);

        /**
        */
        ~synch_monitor();

    private:

        /**
        @param pshared
        @return
        */
        int init(int pshared);

        /**
        @return
        */
        int destroy();

    public:

        /**
        @return
        */
        int lock();

        /**
        @return
        */
        int unlock();

        /**
        @return
        */
        int wait();

        /**
        @param sec
        @param nsec
        @return
        */
        int wait(time_t sec,
                 long nsec);

        /**
        @return
        */
        int notify();

        /**
        @return
        */
        int notify_all();

    private:
        synch_mon_impl  *impl_;
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
                    pthread_mutex_init(&mutex_, NULL);
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
            ptr_(NULL),
            reference_counter_(NULL) {
        }

        /**
        @param ptr
        */
        explicit shared_pointer(T *ptr) :
            ptr_(ptr),
            reference_counter_(NULL) {
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
            return ptr_ == NULL;
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
                        ptr_ = NULL;
                    }
                }
                reference_counter_ = NULL;
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

/** @brief Runnable interface.
*/
class runnable {
    public:

        /**
        */
        virtual ~runnable();
    public:

        /**
        @return
        */
        virtual void *run() = 0;
};

/** @brief A convenient wrapper for pthread.
*/
class p_thread_impl;
class p_thread : public runnable {
    public:

        /**
        */
        explicit p_thread();

        /**
        @param target
        @return
        */
        explicit p_thread(runnable *target);

        /**
        @param target
        @param attr
        @return
        */
        explicit p_thread(runnable *target,
                          pthread_attr_t *attr);

        /**
        @param attr
        @return
        */
        explicit p_thread(pthread_attr_t *attr);

        /**
        */
        virtual ~p_thread();

        /**
        @return
        */
        int thread_id() const;

        /**
        @param value_ptr
        @return
        */
        int join(void **value_ptr);

        /**
        @return
        */
        int stop();

        /**
        @return
        */
        int start();

        /**
        @return
        */
        virtual void *run();

    private:
        p_thread_impl *impl_;
};

/** @brief An enum representing the status a p_task can assume.

*/
enum PTASK_STATUS {
    PTASK_STATUS_ZERO,
    PTASK_STATUS_TOINIT,
    PTASK_STATUS_INIT,
    PTASK_STATUS_SUBMITTED,
    PTASK_STATUS_RUNNING,
    PTASK_STATUS_EXECUTED,
    PTASK_STATUS_INTERRUPTED,
    PTASK_STATUS_REJECTED,
    PTASK_STATUS_EVICTED,
    PTASK_STATUS_ERROR = 500,
};

/** @brief  This class logically represent a task that can be executed
            by a p_executor.
*/
class p_task_impl;
class p_task {
    public:

        /**
        */
        explicit p_task();

        /**
        @param id
        */
        explicit p_task(unsigned int id);

        /**
        */
        virtual ~p_task();

        /**
        @return
        */
        RetCode re_new();

        /**
        @return
        */
        unsigned int get_id() const;

        /**
        @param id
        */
        void set_id(unsigned int id);

        /**
        this method will be called when this task will be run by an executor

        @return RetCode_OK when this task has been successfully completed.
        */
        virtual RetCode execute() = 0;

        /**
        @param status
        @return
        */
        RetCode status(PTASK_STATUS &status) const;

        /**
        @param status
        @return
        */
        RetCode set_status(PTASK_STATUS status);

        /**
        @return
        */
        RetCode execution_result() const;

        /**
        @param result
        */
        void set_execution_result(RetCode result);

        /**
        await at most for sec (seconds) and nsec (nanoseconds) for
        a certain status to occur; this task must be submitted,
        otherwise RetCode_BDSTTS will be returned;
        if sec < 0 this method await indefinitely for a certain status to occur;
        NOTE: it is safe to wait for PTASK_STATUS_RUNNING or PTASK_STATUS_EXECUTED
        states but not to other (error) final states such as
        PTASK_STATUS_INTERRUPTED or PTASK_STATUS_REJECTED in which case this
        method could not even return at all (if this task will execute normally);
        if this instance reaches a state > to the one passed, this method
        is guaranteed to return.

        @param target_status
        @param sec
        @param nsec

        @return RetCode_OK when target_status has been reached or outdated.
                RetCode_BDSTTS if this task has not been submitted.
                RetCode_TMOUT if sec/nsec have been elapsed after invocation.
        */
        RetCode await_for_status(PTASK_STATUS target_status,
                                 time_t sec = -1,
                                 long nsec = 0) const;

    private:
        p_task_impl *impl_;
};

/** @brief An enum representing the status a p_executor can assume.
*/
enum PEXECUTOR_STATUS {
    PEXECUTOR_STATUS_ZERO,
    PEXECUTOR_STATUS_TOINIT,
    PEXECUTOR_STATUS_INIT,
    PEXECUTOR_STATUS_IDLE,
    PEXECUTOR_STATUS_EXECUTING,
    PEXECUTOR_STATUS_DISPOSING,
    PEXECUTOR_STATUS_STOPPED,
    PEXECUTOR_STATUS_ERROR = 500,
};

/** @brief This class represent an thread-executor used by a p_executor_service.
*/
class p_executor_impl;
class p_executor : public p_thread {
    public:

        /**
        */
        explicit p_executor();

        /**
        */
        ~p_executor();

        /**
        @param eserv
        @return
        */
        RetCode init(p_executor_service *eserv);

        /**
        @param status
        @return
        */
        RetCode status(PEXECUTOR_STATUS &status) const;

        /**
        @param status
        @return
        */
        RetCode set_status(PEXECUTOR_STATUS status);

        /**
        @return
        */
        virtual void *run();

    private:
        p_executor_impl *impl_;
};

/** @brief An enum representing the status a p_executor_service can assume.

*/
enum PEXEC_SERVICE_STATUS {
    PEXEC_SERVICE_STATUS_ZERO,
    PEXEC_SERVICE_STATUS_TOINIT,
    PEXEC_SERVICE_STATUS_INACTIVE,
    PEXEC_SERVICE_STATUS_INIT,
    PEXEC_SERVICE_STATUS_STARTING,
    PEXEC_SERVICE_STATUS_STARTED,
    PEXEC_SERVICE_STATUS_STOPPING,
    PEXEC_SERVICE_STATUS_STOPPED,
    PEXEC_SERVICE_STATUS_ERROR = 500,
};

/** @brief This class represent an thread-executor-service that can be used
           to execute asynchronously a set of p_task(s).
*/
class  p_executor_service_impl;
class p_executor_service {
    public:

        /**
        @param id
        @param dispose_task
        */
        explicit p_executor_service(unsigned int id,
                                    bool dispose_task = false);

        /**
        */
        ~p_executor_service();

        /**
        @return
        */
        unsigned int get_id();

        /**
        @return
        */
        bool is_task_disposer();

        /**
        @return
        */
        blocking_queue &get_task_queue();

        /**
        @return
        */
        unsigned int get_executor_count();

        /**
        @param idx
        @return
        */
        p_executor &get_executor_at_idx(unsigned int idx);

        /**
        @param executor_num
        @param queue_task_capacity
        @return
        */
        RetCode init(unsigned int executor_num,
                     unsigned int queue_task_capacity = 0);

        /**
        @return
        */
        PEXEC_SERVICE_STATUS status() const;

        /**
        @param status
        @return
        */
        RetCode status(PEXEC_SERVICE_STATUS &status) const;

        /**
        @param status
        @return
        */
        RetCode set_status(PEXEC_SERVICE_STATUS status);

        /**
        res will be set to true if this executor has been shut down, false otherwise.
        @param res
        @return RetCode_OK when this method successfully complete.
        */
        RetCode is_shutdown(bool &res);

        /**
        @param res
        @return RetCode_OK if all tasks have completed following shut down.
        */
        RetCode is_terminated(bool &res);

        /**
        initiates an orderly shutdown in which previously submitted tasks
        are executed, but no new tasks will be accepted.

        @return
        */
        RetCode shutdown();

        /**
        set this executor service as terminated.

        @return
        */
        RetCode terminated();

        /**
        starts this Executor service

        @return
        */
        RetCode start();

        /**
        submits a p_task for execution, returns RetCode_OK if task has been
        accepted for execution, RetCode_QFULL if queue_task has reached its max
        capacity.
        This executor service must be in PEXEC_SERVICE_STATUS_STARTED state,
        otherwise this method will return RetCode_BDSTTS.

        @param task
        @return
        */
        RetCode submit(p_task *task);

        /**
        blocks until all tasks have completed execution after a shutdown request

        @return
        */
        RetCode await_termination();

        /**
        same as above, except that this method waits at most for sec/nsec seconds
        before return with RetCode_TMOUT code.

        @param sec
        @param nsec
        @return
        */
        RetCode await_termination(time_t sec,
                                  long nsec);

        /**
        await indefinitely until this service reaches or outdoes status provided.

        @param test
        @param current
        @param sec
        @param nsec
        @return
        */
        RetCode await_for_status_reached_or_outdated(PEXEC_SERVICE_STATUS test,
                                                     PEXEC_SERVICE_STATUS &current,
                                                     time_t sec = -1,
                                                     long nsec = 0);
    private:
        p_executor_service_impl *impl_;
};

}

#endif
