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

#include "vlg_logger.h"
#include "vlg_globint.h"

#define MSEC_F 1000000L
#define USEC_F 1000L
#define NSEC_F 1L

namespace vlg {


// synch_mon_impl

class synch_mon_impl {
    public:
        synch_mon_impl(int pshared) {
            init(pshared);
        }

        ~synch_mon_impl() {
            destroy();
        }

        int destroy() {
            RET_PTH_ERR_1(pthread_cond_destroy, &cv_)
            RET_PTH_ERR_1(pthread_mutex_destroy, &mutex_)
            RET_PTH_ERR_1(pthread_mutexattr_destroy, &mattr_)
            RET_PTH_ERR_1(pthread_condattr_destroy, &cattr_)
            return RetCode_OK;
        }

        int init(int pshared) {
            RET_PTH_ERR_1(pthread_mutexattr_init, &mattr_)
            RET_PTH_ERR_2(pthread_mutexattr_setpshared, &mattr_, pshared)
            RET_PTH_ERR_2(pthread_mutex_init, &mutex_, &mattr_)
            RET_PTH_ERR_1(pthread_condattr_init, &cattr_)
            RET_PTH_ERR_2(pthread_condattr_setpshared, &cattr_, pshared)
            RET_PTH_ERR_2(pthread_cond_init, &cv_, &cattr_)
            return RetCode_OK;
        }

        int lock() {
            return pthread_mutex_lock(&mutex_);
        }

        int unlock() {
            return pthread_mutex_unlock(&mutex_);
        }

        int wait() {
            return pthread_cond_wait(&cv_, &mutex_);
        }

        int wait(time_t sec, long nsec) {
            if(sec<0) {
                return pthread_cond_wait(&cv_, &mutex_);
            }
            timespec abstime;
            long sc;
            get_timestamp_ts(&abstime);
            abstime.tv_sec  += sec;
            abstime.tv_nsec += nsec;
            if((sc = (abstime.tv_nsec / 1000000000L))) {
                abstime.tv_sec += sc;
                abstime.tv_nsec %= 1000000000L;
            }
            return pthread_cond_timedwait(&cv_, &mutex_, &abstime);
        }

        int Notify() {
            return pthread_cond_signal(&cv_);
        }

        int notify_all() {
            return pthread_cond_broadcast(&cv_);
        }


        pthread_mutexattr_t     mattr_;
        pthread_mutex_t         mutex_;
        pthread_condattr_t      cattr_;
        pthread_cond_t          cv_;
};


// synch_monitor

synch_monitor::synch_monitor(int pshared) : impl_(nullptr)
{
    impl_ = new synch_mon_impl(pshared);
}

synch_monitor::~synch_monitor()
{
    if(impl_) {
        delete impl_;
    }
}

int synch_monitor::lock()
{
    return impl_->lock();
}

int synch_monitor::unlock()
{
    return impl_->unlock();
}

int synch_monitor::wait()
{
    return impl_->wait();
}

int synch_monitor::wait(time_t sec, long nsec)
{
    return impl_->wait(sec, nsec);
}

int synch_monitor::notify()
{
    return impl_->Notify();
}

int synch_monitor::notify_all()
{
    return impl_->notify_all();
}


// runnable

runnable::~runnable() {}


// p_thread_impl

class p_thread_impl {
    public:
        p_thread_impl(p_thread *target) :
            th_id_(-1),
            attr_(nullptr),
            target_(target) {
        }

        p_thread_impl(runnable *target) :
            th_id_(-1),
            attr_(nullptr),
            target_(target) {
        }

        p_thread_impl(runnable *target, pthread_attr_t *attr) :
            th_id_(-1),
            attr_(attr),
            target_(target) {
        }

        int join(void **value_ptr) {
            return pthread_join(thread_, value_ptr);
        }

        int stop() {
            return pthread_cancel(thread_);
        }

        int start() {
            return pthread_create(&thread_, attr_, &p_thread_impl::pthread_run, this);
        }

        static void *pthread_run(void *arg) {
            p_thread_impl *thread = static_cast<p_thread_impl *>(arg);
            thread->th_id_ = TH_ID;
            return thread->target_->run();
        }


        //thread id has a valid value only when start() has executed.
        int             th_id_;
        pthread_attr_t  *attr_;
        runnable        *target_;
        pthread_t       thread_;
};


// p_thread

p_thread::p_thread()
{
    impl_ = new p_thread_impl(this);
}

p_thread::p_thread(runnable *target)
{
    impl_ = new p_thread_impl(target);
}

p_thread::p_thread(pthread_attr_t *attr)
{
    impl_ = new p_thread_impl(this, attr);
}

p_thread::p_thread(runnable *target, pthread_attr_t *attr)
{
    impl_ = new p_thread_impl(target, attr);
}

p_thread::~p_thread()
{
    if(impl_) {
        delete impl_;
    }
}

int p_thread::thread_id() const
{
    return impl_->th_id_;
}

int p_thread::join(void **value_ptr)
{
    return impl_->join(value_ptr);
}

int p_thread::stop()
{
    return impl_->stop();
}

int p_thread::start()
{
    return impl_->start();
}

void *p_thread::run()
{
    return 0;
}


// p_task_impl

class p_task_impl {
    public:
        p_task_impl() :
            id_(0),
            status_(PTASK_STATUS_INIT),
            exec_res_(RetCode_OK),
            wt_th_(0) {
        }

        p_task_impl(unsigned int id) :
            id_(id),
            status_(PTASK_STATUS_INIT),
            exec_res_(RetCode_OK),
            wt_th_(0) {
        }

        ~p_task_impl() {}

        RetCode re_new() {
            mon_.lock();
            status_ = PTASK_STATUS_INIT;
            mon_.unlock();
            return RetCode_OK;
        }

        unsigned int get_id() const {
            return id_;
        }

        void set_id(unsigned int id) {
            id_ = id;
        }

        RetCode status(PTASK_STATUS &status) const {
            mon_.lock();
            status = status_;
            mon_.unlock();
            return RetCode_OK;
        }

        RetCode set_status(PTASK_STATUS status) {
            mon_.lock();
            if(status <= status_) {
                mon_.unlock();
                return RetCode_BADARG;
            }
            status_ = status;
            if(wt_th_) {
                mon_.notify_all();
                wt_th_ = 0;
            }
            mon_.unlock();
            return RetCode_OK;
        }

        RetCode execution_result() const {
            return exec_res_;
        }

        void set_execution_result(RetCode val) {
            exec_res_ = val;
        }

        RetCode await_for_status(PTASK_STATUS target_status) const {
            mon_.lock();
            if(status_ < PTASK_STATUS_SUBMITTED) {
                mon_.unlock();
                return RetCode_BADSTTS;
            }
            while(status_ < target_status) {
                wt_th_++;
                mon_.wait();
            }
            mon_.unlock();
            return RetCode_OK;
        }

        RetCode await_for_status(PTASK_STATUS target_status,
                                 time_t sec,
                                 long nsec) const {
            mon_.lock();
            if(status_ != PTASK_STATUS_SUBMITTED) {
                mon_.unlock();
                return RetCode_BADSTTS;
            }
            int pthres;
            while(status_ < target_status) {
                wt_th_++;
                if((pthres = mon_.wait(sec, nsec))) {
                    if(pthres == ETIMEDOUT) {
                        wt_th_--;
                        mon_.unlock();
                        return RetCode_TIMEOUT;
                    } else {
                        return RetCode_PTHERR;
                    }
                }
            }
            mon_.unlock();
            return RetCode_OK;
        }


        unsigned int            id_;
        PTASK_STATUS            status_;
        RetCode                 exec_res_;
        mutable uint32_t        wt_th_;
        mutable synch_monitor   mon_;
};



// p_task

p_task::p_task()
{
    impl_ = new p_task_impl();
}

p_task::p_task(unsigned int id)
{
    impl_ = new p_task_impl(id);
}

p_task::~p_task()
{
    if(impl_) {
        delete impl_;
    }
}

RetCode p_task::re_new()
{
    return impl_->re_new();
}

unsigned int p_task::get_id() const
{
    return impl_->get_id();
}

void p_task::set_id(unsigned int id)
{
    impl_->set_id(id);
}

RetCode p_task::status(PTASK_STATUS &status) const
{
    return impl_->status(status);
}

RetCode p_task::set_status(PTASK_STATUS status)
{
    return impl_->set_status(status);
}

RetCode p_task::execution_result() const
{
    return impl_->execution_result();
}

void p_task::set_execution_result(RetCode val)
{
    impl_->set_execution_result(val);
}

RetCode p_task::await_for_status(PTASK_STATUS target_status,
                                 time_t sec,
                                 long nsec) const
{
    return impl_->await_for_status(target_status, sec, nsec);
}


// p_executor_impl

class p_executor_impl {
    public:
        p_executor_impl() :
            status_(PEXECUTOR_STATUS_TOINIT),
            eserv_(nullptr) {
            log_ = logger::get_logger("p_executor_impl");
            IFLOG(trc(TH_ID, LS_CTR "%s(ptr:%p)", __func__, this))
        }

        ~p_executor_impl() {
            IFLOG(trc(TH_ID, LS_DTR "%s(ptr:%p)", __func__, this))
        }

        RetCode init(p_executor_service *eserv) {
            eserv_ = eserv;
            IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, eserv_id:%u)", __func__, this,
                      eserv_->get_id()))
            status_ = PEXECUTOR_STATUS_INIT;
            IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
            return RetCode_OK;
        }

        RetCode status(PEXECUTOR_STATUS &status) const {
            mon_.lock();
            status = status_;
            mon_.unlock();
            return RetCode_OK;
        }

        RetCode set_status(PEXECUTOR_STATUS status) {
            IFLOG(low(TH_ID, LS_OPN "%s(ptr:%p, status:%d)", __func__, this, status))
            mon_.lock();
            status_ = status;
            mon_.unlock();
            IFLOG(low(TH_ID, LS_CLO "%s", __func__))
            return RetCode_OK;
        }

        void *run() {
            IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
            PEXEC_SERVICE_STATUS eserv_status = PEXEC_SERVICE_STATUS_ZERO;
            PEXECUTOR_STATUS exec_status = PEXECUTOR_STATUS_ZERO;
            uint32_t tq_size = 0;
            p_task *task = nullptr;
            RetCode pres = RetCode_KO;
            bool go_term = true;
            if(status_ != PEXECUTOR_STATUS_INIT && status_ != PEXECUTOR_STATUS_STOPPED) {
                set_status(PEXECUTOR_STATUS_ERROR);
                IFLOG(err(TH_ID, LS_CLO "%s() [PEXECUTOR_STATUS_ERROR] -1-", __func__))
                return (void *)1;
            }
            eserv_->await_for_status_reached_or_outdated(PEXEC_SERVICE_STATUS_STARTED,
                                                         eserv_status);
            if(eserv_status != PEXEC_SERVICE_STATUS_STARTED) {
                IFLOG(err(TH_ID, LS_TRL "%s() [aborting] [executor service(%p) status:%d]",
                          __func__, eserv_, eserv_->status()))
                return (void *)1;
            }
            set_status(PEXECUTOR_STATUS_IDLE);
            while(!eserv_->status(eserv_status) &&
                    eserv_status == PEXEC_SERVICE_STATUS_STARTED) {
                if(!(pres = eserv_->get_task_queue().get(0, 100*MSEC_F, &task))) {
                    IFLOG(low(TH_ID, LS_TRL "%s() GET[task:%p, qsz:%u, qrm:%u]", __func__, task,
                              eserv_->get_task_queue().size(),
                              eserv_->get_task_queue().remain_capacity()))
                    set_status(PEXECUTOR_STATUS_EXECUTING);
                    IFLOG(low(TH_ID, LS_EXE "%s()[executing taskid:%d]", __func__, task->get_id()))
                    task->set_execution_result(task->execute());
                    IFLOG(low(TH_ID, LS_EXE "%s()[executed taskid:%d]", __func__, task->get_id()))
                    task->set_status(PTASK_STATUS_EXECUTED);
                    if(eserv_->is_task_disposer()) {
                        delete task;
                    }
                    set_status(PEXECUTOR_STATUS_IDLE);
                } else if(pres == RetCode_PTHERR) {
                    set_status(PEXECUTOR_STATUS_ERROR);
                    IFLOG(err(TH_ID, LS_CLO "%s() [PEXECUTOR_STATUS_ERROR] -2-", __func__))
                    return (void *)1;
                }
            }
            IFLOG(dbg(TH_ID, LS_TRL "%s() [stopping] [executor service(%p) status:%d]",
                      __func__, eserv_,
                      eserv_->status()))
            while(!eserv_->status(eserv_status) &&
                    eserv_status == PEXEC_SERVICE_STATUS_STOPPING) {
                if(!(pres = eserv_->get_task_queue().get(0, 10*MSEC_F, &task))) {
                    task->set_status(PTASK_STATUS_SUBMITTED);
                    set_status(PEXECUTOR_STATUS_EXECUTING);
                    IFLOG(dbg(TH_ID, LS_EXE "%s()[stopping phase][executing taskid:%d]", __func__,
                              task->get_id()))
                    task->set_execution_result(task->execute());
                    IFLOG(dbg(TH_ID, LS_EXE "%s()[stopping phase][executed taskid:%d]", __func__,
                              task->get_id()))
                    task->set_status(PTASK_STATUS_EXECUTED);
                    if(eserv_->is_task_disposer()) {
                        delete task;
                    }
                    set_status(PEXECUTOR_STATUS_IDLE);
                } else if(pres == RetCode_PTHERR) {
                    set_status(PEXECUTOR_STATUS_ERROR);
                    IFLOG(err(TH_ID, LS_CLO "%s() [PEXECUTOR_STATUS_ERROR] -3-", __func__))
                    return (void *)1;
                }
                if(!(pres = eserv_->get_task_queue().size(tq_size)) && !tq_size) {
                    set_status(PEXECUTOR_STATUS_DISPOSING);
                    for(unsigned int i = 0; i<eserv_->get_executor_count(); i++) {
                        go_term &= (!eserv_->get_executor_at_idx(i).status(exec_status)) &&
                                   ((exec_status == PEXECUTOR_STATUS_DISPOSING) ||
                                    (exec_status == PEXECUTOR_STATUS_STOPPED));
                    }
                    if(go_term) {
                        IFLOG(dbg(TH_ID, LS_TRL "%s() [terminating executor service]", __func__))
                        eserv_->terminated();
                    }
                    break;
                }
            }
            set_status(PEXECUTOR_STATUS_STOPPED);
            IFLOG(trc(TH_ID, LS_CLO "%s(0)", __func__))
            return 0;
        }

    private:
        PEXECUTOR_STATUS        status_;
        p_executor_service      *eserv_;
        mutable synch_monitor   mon_;
        static logger           *log_;
};

logger *p_executor_impl::log_ = nullptr;


// p_executor

p_executor::p_executor()
{
    impl_ = new p_executor_impl();
}

p_executor::~p_executor()
{
    if(impl_) {
        delete impl_;
    }
}

RetCode p_executor::init(p_executor_service *eserv)
{
    return impl_->init(eserv);
}

RetCode p_executor::status(PEXECUTOR_STATUS &status) const
{
    return impl_->status(status);
}

RetCode p_executor::set_status(PEXECUTOR_STATUS status)
{
    return impl_->set_status(status);
}

void *p_executor::run()
{
    void *res = impl_->run();
    int stop_res = stop();
    if(stop_res) {
        FILE *ferr = fopen("log.err", "wa+");
        fprintf(ferr ? ferr : stderr, "p_executor::run() - pthread error:%d", stop_res);
        if(ferr) {
            fclose(ferr);
        }
    }
    return res;
}


// p_executor_service_impl

class p_executor_service_impl {
        friend class p_executor_service;

    public:
        p_executor_service_impl(p_executor_service &publ, unsigned int id,
                                bool dispose_task) :
            publ_(publ),
            id_(id),
            status_(PEXEC_SERVICE_STATUS_TOINIT),
            executor_num_(0),
            dispose_task_(dispose_task),
            exec_pool_(nullptr),
            task_queue_(sngl_ptr_obj_mng()),
            idle_exctrs_(0) {
            log_ = logger::get_logger("p_executor_service_impl");
            IFLOG(trc(TH_ID, LS_CTR "%s(id:%d, disposetask:%d)", __func__,
                      id,
                      dispose_task))
        }

        ~p_executor_service_impl() {
            IFLOG(trc(TH_ID, LS_DTR"%s(ptr:%p)", __func__, this))
    }

    unsigned int get_id() {
        return id_;
    }

    RetCode init(unsigned int executor_num, unsigned int queue_task_capacity) {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d, queuetaskcap:%d, executornum:%d)", __func__, id_,
                  queue_task_capacity, executor_num))
        if(!executor_num) {
            IFLOG(inf(TH_ID, LS_TRL "%s(id:%d) [EXEC.SERV. SET INACTIVE]", __func__, id_))
            RETURN_IF_NOT_OK(set_status(PEXEC_SERVICE_STATUS_INACTIVE))
        } else {
            executor_num_ = executor_num;
            RETURN_IF_NOT_OK(task_queue_.init(queue_task_capacity))
            COMMAND_IF_NULL(exec_pool_ = new p_executor[executor_num_], EXIT_ACTION)
            for(unsigned int i = 0; i<executor_num_; i++) {
                RETURN_IF_NOT_OK(exec_pool_[i].init(&publ_))
            }
            RETURN_IF_NOT_OK(set_status(PEXEC_SERVICE_STATUS_INIT))
        }
        IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        return RetCode_OK;
    }

    PEXEC_SERVICE_STATUS status() const {
        return status_;
    }

    RetCode status(PEXEC_SERVICE_STATUS &status) const {
        mon_.lock();
        status = status_;
        mon_.unlock();
        return RetCode_OK;
    }

    RetCode set_status(PEXEC_SERVICE_STATUS status) {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d, status:%d)", __func__,id_, status))
        mon_.lock();
        status_ = status;
        mon_.notify_all();
        mon_.unlock();
        IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        return RetCode_OK;
    }

    RetCode start() {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
        if(status_ != PEXEC_SERVICE_STATUS_INIT && status_ != PEXEC_SERVICE_STATUS_STOPPED) {
            IFLOG(err(TH_ID, LS_CLO "%s", __func__))
            return RetCode_BADSTTS;
        }
        RETURN_IF_NOT_OK(set_status(PEXEC_SERVICE_STATUS_STARTING))
        for(unsigned int i = 0; i<executor_num_; i++) {
            exec_pool_[i].start();
        }
        RETURN_IF_NOT_OK(set_status(PEXEC_SERVICE_STATUS_STARTED))
        IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        return RetCode_OK;
    }

    RetCode await_for_status_reached_or_outdated(PEXEC_SERVICE_STATUS test,
                                              PEXEC_SERVICE_STATUS &current,
                                              time_t sec,
                                              long nsec) {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d, test:%d)", __func__, id_, test))
        RetCode rcode = RetCode_OK;
        mon_.lock();
        if(status_ < PEXEC_SERVICE_STATUS_INIT) {
            mon_.unlock();
            IFLOG(err(TH_ID, LS_CLO "%s", __func__))
            return RetCode_BADSTTS;
        }
        while(status_ < test) {
            int pthres;
            if((pthres = mon_.wait(sec, nsec))) {
                if(pthres == ETIMEDOUT) {
                    rcode =  RetCode_TIMEOUT;
                    break;
                }
            }
        }
        current = status_;
        IFLOG(log(rcode ? TL_WRN : TL_DBG ,TH_ID, LS_CLO "%s(id:%d, res:%d) - test:%d [reached or outdated] current:%d",
                  __func__, id_, rcode, test,
                  status_))
        mon_.unlock();
        return rcode;
    }

    RetCode await_termination() {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
        mon_.lock();
        if(status_ != PEXEC_SERVICE_STATUS_STOPPING) {
            mon_.unlock();
            IFLOG(err(TH_ID, LS_CLO "%s", __func__))
            return RetCode_BADSTTS;
        }
        do {
            mon_.wait();
        } while(status_ == PEXEC_SERVICE_STATUS_STOPPING);
        mon_.unlock();
        IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        return RetCode_OK;
    }

    RetCode await_termination(time_t sec, long nsec) {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d, sec%d, nsec:%d)", __func__, id_, sec, nsec))
        int pthres;
        mon_.lock();
        if(status_ != PEXEC_SERVICE_STATUS_STOPPING) {
            mon_.unlock();
            IFLOG(err(TH_ID, LS_CLO "%s", __func__))
            return RetCode_BADSTTS;
        }
        do {
            if((pthres = mon_.wait(sec, nsec))) {
                if(pthres == ETIMEDOUT) {
                    mon_.unlock();
                    IFLOG(inf(TH_ID, LS_CLO "%s(id:%d, sec%d, nsec:%d) - [timeout]", __func__, id_, sec, nsec))
                    return RetCode_TIMEOUT;
                } else {
                    IFLOG(cri(TH_ID, LS_CLO "%s(id:%d, sec%d, nsec:%d, pthres:%d) - [pthread error]", __func__, id_, sec, nsec, pthres))
                    return RetCode_PTHERR;
                }
            }
        }
        while(status_ == PEXEC_SERVICE_STATUS_STOPPING);
        mon_.unlock();
        IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        return RetCode_OK;
    }

    RetCode is_shutdown(bool &res) {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
        mon_.lock();
        res = (status_ == PEXEC_SERVICE_STATUS_STOPPING);
        mon_.unlock();
        IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        return RetCode_OK;
    }

    RetCode is_terminated(bool &res) {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
        mon_.lock();
        res = (status_ == PEXEC_SERVICE_STATUS_STOPPED);
        mon_.unlock();
        IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        return RetCode_OK;
    }

    RetCode shutdown() {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
        RETURN_IF_NOT_OK(set_status(PEXEC_SERVICE_STATUS_STOPPING))
        IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        return RetCode_OK;
    }

    RetCode terminated() {
        IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
        RETURN_IF_NOT_OK(set_status(PEXEC_SERVICE_STATUS_STOPPED))
        IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        return RetCode_OK;
    }

    RetCode submit(p_task *task) {
        IFLOG(low(TH_ID, LS_OPN "%s(id:%d, task:%p)", __func__, id_, task))
        RetCode rcode = RetCode_OK;
        mon_.lock();
        if(status_ != PEXEC_SERVICE_STATUS_STARTED) {
            task->set_status(PTASK_STATUS_REJECTED);
            mon_.unlock();
            IFLOG(err(TH_ID, LS_CLO "%s", __func__))
            return RetCode_BADSTTS;
        }
        if((rcode = task_queue_.put(0, 20*MSEC_F, &task))) {
            task->set_status(PTASK_STATUS_REJECTED);
            switch(rcode) {
                case RetCode_QFULL:
                case RetCode_TIMEOUT:
                    IFLOG(wrn(TH_ID, LS_TRL "%s(id:%d) - [queue full]", __func__, id_))
                case RetCode_PTHERR:
                    IFLOG(cri(TH_ID, LS_TRL "%s(id:%d) - [pthread error]", __func__, id_))
                default:
                    IFLOG(err(TH_ID, LS_TRL "%s(id:%d) - RetCode_CODE:%d", __func__, id_, rcode))
            }
        }
        else{
            task->set_status(PTASK_STATUS_SUBMITTED);
        }
        IFLOG(low(TH_ID, LS_TRL "%s() - PUT[res:%d, id:%d, task:%p, qsz:%u, qrm:%u]", __func__,
                  rcode,
                  id_,
                  task,
                  task_queue_.size(),
                  task_queue_.remain_capacity()))
        mon_.unlock();
        IFLOG(low(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
        return rcode;
    }

    private:
    p_executor_service      &publ_;
    unsigned int            id_;
    PEXEC_SERVICE_STATUS    status_;
    unsigned int            executor_num_;
    bool                    dispose_task_;
    p_executor             *exec_pool_;
    blocking_queue          task_queue_;
    mutable synch_monitor   mon_;
    //info
    unsigned int            idle_exctrs_;

    //statistic
    static logger           *log_;
};

logger *p_executor_service_impl::log_ = nullptr;


// p_executor_service

p_executor_service::p_executor_service(unsigned int id,
                                       bool dispose_task)
{
    impl_ = new p_executor_service_impl(*this, id, dispose_task);
}

p_executor_service::~p_executor_service()
{
    if(impl_) {
        delete impl_;
    }
}

unsigned int p_executor_service::get_id()
{
    return impl_->get_id();
}

bool p_executor_service::is_task_disposer()
{
    return impl_->dispose_task_;
}

blocking_queue &p_executor_service::get_task_queue()
{
    return impl_->task_queue_;
}

unsigned int p_executor_service::get_executor_count()
{
    return impl_->executor_num_;
}

p_executor &p_executor_service::get_executor_at_idx(unsigned int idx)
{
    return impl_->exec_pool_[idx];
}

RetCode p_executor_service::init(unsigned int executor_num, unsigned int queue_task_capacity)
{
    return impl_->init(executor_num, queue_task_capacity);
}

PEXEC_SERVICE_STATUS p_executor_service::status() const
{
    return impl_->status();
}

RetCode p_executor_service::status(PEXEC_SERVICE_STATUS &status) const
{
    return impl_->status(status);
}

RetCode p_executor_service::set_status(PEXEC_SERVICE_STATUS status)
{
    return impl_->set_status(status);
}

RetCode p_executor_service::start()
{
    return impl_->start();
}

RetCode p_executor_service::await_for_status_reached_or_outdated(PEXEC_SERVICE_STATUS test,
                                                              PEXEC_SERVICE_STATUS &current,
                                                              time_t sec,
                                                              long nsec)
{
    return impl_->await_for_status_reached_or_outdated(test, current, sec, nsec);
}

RetCode p_executor_service::await_termination()
{
    return impl_->await_termination();
}

RetCode p_executor_service::await_termination(time_t sec, long nsec)
{
    return impl_->await_termination(sec, nsec);
}

RetCode p_executor_service::is_shutdown(bool &res)
{
    return impl_->is_shutdown(res);
}

RetCode p_executor_service::is_terminated(bool &res)
{
    return impl_->is_terminated(res);
}

RetCode p_executor_service::shutdown()
{
    return impl_->shutdown();
}

RetCode p_executor_service::terminated()
{
    return impl_->terminated();
}

RetCode p_executor_service::submit(p_task *task)
{
    return impl_->submit(task);
}

}
