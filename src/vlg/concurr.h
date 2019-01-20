/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#pragma once
#include "structs.h"
#include <thread>

namespace spdlog {
class logger;
}

namespace vlg {

/** @brief runnable interface.
*/
struct runnable {
    virtual ~runnable() {}
    virtual void run() = 0;
};

/** @brief a wrapper to std::thread
*/
struct th : public runnable {
        explicit th() :
            target_(this) {
        }

        explicit th(runnable *target):
            target_(target) {
        }

        virtual ~th() = default;

        std::thread::id thread_id() const {
            return id_;
        }

        void join() {
            if(th_) {
                th_->join();
            }
        }

        void stop() {
            if(th_) {
                th_->detach();
                th_.release();
            }
        }

        void start() {
            th_.reset(new std::thread(th_run, this));
        }

        virtual void run() override {
        }

    private:
        static void th_run(th *thd) {
            std::this_thread::get_id();
            thd->target_->run();
        }

    private:
        runnable *target_;
        std::thread::id id_;
        std::unique_ptr<std::thread> th_;
};

/** @brief An enum representing the status a task can assume.

*/
enum TskStatus {
    TskStatus_INIT,
    TskStatus_SUBMITTED,
    TskStatus_RUNNING,
    TskStatus_EXECUTED,
    TskStatus_INTERRUPTED,
    TskStatus_REJECTED,
    TskStatus_EVICTED,
    TskStatus_ERROR = 500,
};

/** @brief  This class logically represent a task that can be executed
            by an executor.
*/
struct task {
        explicit task() :
            id_(0),
            status_(TskStatus_INIT),
            exec_res_(RetCode_OK),
            wt_th_(0) {
        }

        explicit task(unsigned int id) :
            id_(id),
            status_(TskStatus_INIT),
            exec_res_(RetCode_OK),
            wt_th_(0) {
        }

        virtual ~task() = default;

        unsigned int get_id() const {
            return id_;
        }

        /**
        this method will be called when this task will be run by an executor
        @return RetCode_OK when this task has been successfully completed.
        */
        virtual RetCode execute() = 0;

        TskStatus get_status() const {
            return status_;
        }

        RetCode set_status(TskStatus status);

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
        RetCode await_for_status(TskStatus target_status,
                                 time_t sec = -1,
                                 long nsec = 0) const;

    private:
        unsigned int id_;
        TskStatus status_;
        RetCode exec_res_;
        mutable uint32_t wt_th_;

        mutable std::mutex mtx_;
        mutable std::condition_variable cv_;
};

/** @brief An enum representing the status an executor can assume.
*/
enum ExecutorStatus {
    ExecutorStatus_INIT,
    ExecutorStatus_IDLE,
    ExecutorStatus_EXECUTING,
    ExecutorStatus_DISPOSING,
    ExecutorStatus_STOPPED,
    ExecutorStatus_ERROR = 500,
};

/** @brief This class represent an executor used by a exec_srv.
*/
struct exec_srv;
struct exectr : public th {
        explicit exectr(exec_srv &eserv);
        ~exectr();

        ExecutorStatus get_status() const {
            return status_;
        }

        RetCode set_status(ExecutorStatus status);

        virtual void run() override;

    private:
        ExecutorStatus status_;
        exec_srv &eserv_;

        mutable std::mutex mtx_;
        mutable std::condition_variable cv_;
};

/** @brief An enum representing the status a exec_srv can assume.
*/
enum ExecSrvStatus {
    ExecSrvStatus_TOINIT,
    ExecSrvStatus_INACTIVE,
    ExecSrvStatus_INIT,
    ExecSrvStatus_STARTING,
    ExecSrvStatus_STARTED,
    ExecSrvStatus_STOPPING,
    ExecSrvStatus_STOPPED,
    ExecSrvStatus_ERROR = 500,
};

/** @brief This class represent an thread-executor-service that can be used
           to execute asynchronously a set of task(s).
*/
struct exec_srv {
        explicit exec_srv(std::shared_ptr<spdlog::logger> &log);
        ~exec_srv();

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

        exectr &get_executor_at_idx(unsigned int idx) {
            return *exec_pool_[idx];
        }

        ExecSrvStatus get_status() const {
            return status_;
        }

        RetCode set_status(ExecSrvStatus status);

        /**
        @return true if this executor has been shut down, false otherwise.
        */
        bool is_shutdown() {
            return (status_ == ExecSrvStatus_STOPPING);
        }

        /**
        @return true if all tasks have completed following shut down.
        */
        bool is_terminated() {
            return (status_ == ExecSrvStatus_STOPPED);
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
        submits a task for execution, returns RetCode_OK if task has been
        accepted for execution, RetCode_QFULL if queue_task has reached its max
        capacity.
        */
        RetCode submit(std::shared_ptr<task> &task);

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
        RetCode await_for_status_reached(ExecSrvStatus test,
                                         ExecSrvStatus &current,
                                         time_t sec = -1,
                                         long nsec = 0);
    private:
        unsigned int id_;
        ExecSrvStatus status_;
        std::vector<std::unique_ptr<exectr>> exec_pool_;
        b_qu task_queue_;

        mutable std::mutex mtx_;
        mutable std::condition_variable cv_;

    public:
        std::shared_ptr<spdlog::logger> log_;
};

}
