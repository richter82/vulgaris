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

#pragma once
#include "structs.h"
#include <vector>

namespace vlg {

/** @brief runnable interface.
*/
class runnable {
    public:
        virtual ~runnable() {}
        virtual void *run() = 0;
};

/** @brief A convenient wrapper for pthread.
*/
class p_th : public runnable {
    public:
        explicit p_th();
        explicit p_th(runnable *target);
        explicit p_th(pthread_attr_t *attr);
        explicit p_th(runnable *target,
                      pthread_attr_t *attr);

        virtual ~p_th();

        int thread_id() const {
            return th_id_;
        }

        int join(void **value_ptr) {
            return pthread_join(thread_, value_ptr);
        }

        int stop() {
            return pthread_cancel(thread_);
        }

        int start() {
            return pthread_create(&thread_, attr_, &pthread_run, this);
        }

        virtual void *run() {
            return 0;
        }

    private:
        static void *pthread_run(void *arg);

    private:
        //thread id has a valid value only when start() has executed.
        int th_id_;
        pthread_attr_t *attr_;
        runnable *target_;
        pthread_t thread_;
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
class p_tsk {
    public:

        explicit p_tsk();
        explicit p_tsk(unsigned int id);
        virtual ~p_tsk();

        RetCode re_new();

        unsigned int get_id() const {
            return id_;
        }

        /**
        this method will be called when this task will be run by an executor
        @return RetCode_OK when this task has been successfully completed.
        */
        virtual RetCode execute() = 0;

        PTASK_STATUS get_status() const {
            return status_;
        }

        RetCode set_status(PTASK_STATUS status);

        RetCode execution_result() const {
            return exec_res_;
        }

        void set_execution_result(RetCode result) {
            exec_res_ = result;
        }

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
        unsigned int id_;
        PTASK_STATUS status_;
        RetCode exec_res_;
        mutable uint32_t wt_th_;
        mutable mx mon_;
};

/** @brief An enum representing the status a p_executor can assume.
*/
enum PEXECUTOR_STATUS {
    PEXECUTOR_STATUS_ZERO,
    PEXECUTOR_STATUS_INIT,
    PEXECUTOR_STATUS_IDLE,
    PEXECUTOR_STATUS_EXECUTING,
    PEXECUTOR_STATUS_DISPOSING,
    PEXECUTOR_STATUS_STOPPED,
    PEXECUTOR_STATUS_ERROR = 500,
};

/** @brief This class represent an thread-executor used by a p_executor_service.
*/
class p_exec_srv;
class p_exectr : public p_th {
    public:

        explicit p_exectr(p_exec_srv &eserv);
        ~p_exectr();

        PEXECUTOR_STATUS get_status() const {
            return status_;
        }

        RetCode set_status(PEXECUTOR_STATUS status);

        virtual void *run();

    private:
        PEXECUTOR_STATUS status_;
        p_exec_srv &eserv_;
        mutable mx mon_;
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
class p_exec_srv {
    public:

        explicit p_exec_srv(unsigned int id,
                            bool dispose_task = false);

        ~p_exec_srv();

        RetCode init(unsigned int executor_num);

        unsigned int get_id() {
            return id_;
        }

        bool is_task_disposer() {
            return dispose_task_;
        }

        b_qu &get_task_queue() {
            return task_queue_;
        }

        unsigned int get_executor_count() {
            return (unsigned int)exec_pool_.size();
        }

        p_exectr &get_executor_at_idx(unsigned int idx) {
            return *exec_pool_[idx];
        }

        PEXEC_SERVICE_STATUS get_status() const {
            return status_;
        }

        RetCode set_status(PEXEC_SERVICE_STATUS status);

        /**
        res will be set to true if this executor has been shut down, false otherwise.
        @param res
        @return RetCode_OK when this method successfully complete.
        */
        bool is_shutdown() {
            return (status_ == PEXEC_SERVICE_STATUS_STOPPING);
        }

        /**
        @param res
        @return RetCode_OK if all tasks have completed following shut down.
        */
        bool is_terminated() {
            return (status_ == PEXEC_SERVICE_STATUS_STOPPED);
        }

        /**
        initiates an orderly shutdown in which previously submitted tasks
        are executed, but no new tasks will be accepted
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
        RetCode submit(p_tsk *task);

        /**
        blocks until all tasks have completed execution after a shutdown
        @return
        */
        RetCode await_termination();

        /**
        same as above, except that this method waits at most for sec/nsec seconds
        before return with RetCode_TMOUT code
        @param sec
        @param nsec
        @return
        */
        RetCode await_termination(time_t sec,
                                  long nsec);

        /**
        await indefinitely until this service reaches or outdoes status provided
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
        unsigned int id_;
        PEXEC_SERVICE_STATUS status_;
        bool dispose_task_;
        std::vector<std::unique_ptr<p_exectr>> exec_pool_;
        b_qu task_queue_;
        mutable mx mon_;
};

}
