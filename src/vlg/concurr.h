/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "structs.h"

#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>

#if defined WIN32 && defined _MSC_VER
#define TH_ID GetCurrentThreadId()
#else
#define TH_ID ((unsigned int)((unsigned long)pthread_self()))
#endif

namespace spdlog {
class logger;
}

namespace vlg {

/** @brief runnable interface.
*/
struct runnable {
    virtual ~runnable() {}
    virtual void *run() = 0;
};

/** @brief A convenient wrapper for pthread.
*/
struct p_th : public runnable {
        explicit p_th() :
            th_id_(-1),
            attr_(nullptr),
            target_(this) {
        }

        explicit p_th(runnable *target):
            th_id_(-1),
            attr_(nullptr),
            target_(target) {
        }

        explicit p_th(pthread_attr_t *attr) :
            th_id_(-1),
            attr_(attr),
            target_(this) {
        }

        explicit p_th(runnable *target,
                      pthread_attr_t *attr) :
            th_id_(-1),
            attr_(attr),
            target_(target) {
        }

        virtual ~p_th() = default;

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

        virtual void *run() override {
            return 0;
        }

    private:
        static void *pthread_run(void *arg) {
            p_th *thread = static_cast<p_th *>(arg);
            thread->th_id_ = TH_ID;
            return thread->target_->run();
        }

    private:
        //thread id has a valid value only when start() has executed.
        int th_id_;
        pthread_attr_t *attr_;
        runnable *target_;
        pthread_t thread_;
};

/** @brief An enum representing the status a p_task can assume.

*/
enum PTskStatus {
    PTskStatus_INIT,
    PTskStatus_SUBMITTED,
    PTskStatus_RUNNING,
    PTskStatus_EXECUTED,
    PTskStatus_INTERRUPTED,
    PTskStatus_REJECTED,
    PTskStatus_EVICTED,
    PTskStatus_ERROR = 500,
};

/** @brief  This class logically represent a task that can be executed
            by a p_executor.
*/
struct p_tsk {
        explicit p_tsk() :
            id_(0),
            status_(PTskStatus_INIT),
            exec_res_(RetCode_OK),
            wt_th_(0) {
        }

        explicit p_tsk(unsigned int id) :
            id_(id),
            status_(PTskStatus_INIT),
            exec_res_(RetCode_OK),
            wt_th_(0) {
        }

        virtual ~p_tsk() = default;

        unsigned int get_id() const {
            return id_;
        }

        /**
        this method will be called when this task will be run by an executor
        @return RetCode_OK when this task has been successfully completed.
        */
        virtual RetCode execute() = 0;

        PTskStatus get_status() const {
            return status_;
        }

        RetCode set_status(PTskStatus status);

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

        @return RetCode_OK when target_status has been reached.
                RetCode_BDSTTS if this task has not been submitted.
                RetCode_TMOUT if sec/nsec have been elapsed after invocation.
        */
        RetCode await_for_status(PTskStatus target_status,
                                 time_t sec = -1,
                                 long nsec = 0) const;

    private:
        unsigned int id_;
        PTskStatus status_;
        RetCode exec_res_;
        mutable uint32_t wt_th_;

        mutable std::mutex mtx_;
        mutable std::condition_variable cv_;
};

/** @brief An enum representing the status a p_executor can assume.
*/
enum PExecutorStatus {
    PExecutorStatus_INIT,
    PExecutorStatus_IDLE,
    PExecutorStatus_EXECUTING,
    PExecutorStatus_DISPOSING,
    PExecutorStatus_STOPPED,
    PExecutorStatus_ERROR = 500,
};

/** @brief This class represent an thread-executor used by a p_executor_service.
*/
struct p_exec_srv;
struct p_exectr : public p_th {
        explicit p_exectr(p_exec_srv &eserv);
        ~p_exectr();

        PExecutorStatus get_status() const {
            return status_;
        }

        RetCode set_status(PExecutorStatus status);

        virtual void *run() override;

    private:
        PExecutorStatus status_;
        p_exec_srv &eserv_;

        mutable std::mutex mtx_;
        mutable std::condition_variable cv_;
};

/** @brief An enum representing the status a p_executor_service can assume.
*/
enum PExecSrvStatus {
    PExecSrvStatus_TOINIT,
    PExecSrvStatus_INACTIVE,
    PExecSrvStatus_INIT,
    PExecSrvStatus_STARTING,
    PExecSrvStatus_STARTED,
    PExecSrvStatus_STOPPING,
    PExecSrvStatus_STOPPED,
    PExecSrvStatus_ERROR = 500,
};

/** @brief This class represent an thread-executor-service that can be used
           to execute asynchronously a set of p_task(s).
*/
struct p_exec_srv {
        explicit p_exec_srv(std::shared_ptr<spdlog::logger> &log);
        ~p_exec_srv();

        RetCode init(unsigned int executor_num);

        unsigned int get_id() {
            return id_;
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

        PExecSrvStatus get_status() const {
            return status_;
        }

        RetCode set_status(PExecSrvStatus status);

        /**
        @return true if this executor has been shut down, false otherwise.
        */
        bool is_shutdown() {
            return (status_ == PExecSrvStatus_STOPPING);
        }

        /**
        @return true if all tasks have completed following shut down.
        */
        bool is_terminated() {
            return (status_ == PExecSrvStatus_STOPPED);
        }

        /**
        initiates an orderly shutdown in which previously submitted tasks
        are executed, but no new tasks will be accepted.
        */
        RetCode shutdown();

        /**
        set this executor service as terminated.
        */
        RetCode terminated();

        /**
        starts this Executor service.
        */
        RetCode start();

        /**
        submits a p_task for execution, returns RetCode_OK if task has been
        accepted for execution, RetCode_QFULL if queue_task has reached its max
        capacity.
        This executor service must be in PEXEC_SERVICE_STATUS_STARTED state,
        otherwise this method will return RetCode_BDSTTS.
        */
        RetCode submit(std::shared_ptr<p_tsk> &task);

        /**
        blocks until all tasks have completed execution after a shutdown.
        */
        RetCode await_termination();

        /**
        same as above, except that this method waits at most for sec/nsec seconds
        before return with RetCode_TMOUT code.
        */
        RetCode await_termination(time_t sec,
                                  long nsec);

        /**
        await indefinitely until this service reaches or reaches status provided.
        */
        RetCode await_for_status_reached(PExecSrvStatus test,
                                         PExecSrvStatus &current,
                                         time_t sec = -1,
                                         long nsec = 0);
    private:
        unsigned int id_;
        PExecSrvStatus status_;
        std::vector<std::unique_ptr<p_exectr>> exec_pool_;
        b_qu task_queue_;

        mutable std::mutex mtx_;
        mutable std::condition_variable cv_;

    public:
        std::shared_ptr<spdlog::logger> log_;
};

}
