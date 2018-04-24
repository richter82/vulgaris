/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "glob.h"

#define MSEC_F 1000000L
#define USEC_F 1000L
#define NSEC_F 1L

namespace vlg {

// synch_mon_impl

mx::mx(int pshared)
{
    pthread_mutexattr_init(&mattr_);
    pthread_mutexattr_setpshared(&mattr_, pshared);
    pthread_mutex_init(&mutex_, &mattr_);
    pthread_condattr_init(&cattr_);
    pthread_condattr_setpshared(&cattr_, pshared);
    pthread_cond_init(&cv_, &cattr_);
}

mx::~mx()
{
    pthread_cond_destroy(&cv_);
    pthread_mutex_destroy(&mutex_);
    pthread_mutexattr_destroy(&mattr_);
    pthread_condattr_destroy(&cattr_);
}

int mx::wait(time_t sec, long nsec)
{
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

// p_thread

p_th::p_th() :
    th_id_(-1),
    attr_(nullptr),
    target_(this)
{
}

p_th::p_th(runnable *target) :
    th_id_(-1),
    attr_(nullptr),
    target_(target)
{
}

p_th::p_th(pthread_attr_t *attr) :
    th_id_(-1),
    attr_(attr),
    target_(this)
{
}

p_th::p_th(runnable *target, pthread_attr_t *attr) :
    th_id_(-1),
    attr_(attr),
    target_(target)
{
}

p_th::~p_th()
{}

void *p_th::pthread_run(void *arg)
{
    p_th *thread = static_cast<p_th *>(arg);
    thread->th_id_ = TH_ID;
    return thread->target_->run();
}

// p_task

p_tsk::p_tsk() :
    id_(0),
    status_(PTASK_STATUS_INIT),
    exec_res_(RetCode_OK),
    wt_th_(0)
{
}

p_tsk::p_tsk(unsigned int id) :
    id_(id),
    status_(PTASK_STATUS_INIT),
    exec_res_(RetCode_OK),
    wt_th_(0)
{
}

p_tsk::~p_tsk()
{}

RetCode p_tsk::re_new()
{
    scoped_mx smx(mon_);
    status_ = PTASK_STATUS_INIT;
    return RetCode_OK;
}

RetCode p_tsk::set_status(PTASK_STATUS status)
{
    scoped_mx smx(mon_);
    if(status <= status_) {
        return RetCode_BADARG;
    }
    status_ = status;
    if(wt_th_) {
        mon_.notify_all();
        wt_th_ = 0;
    }
    return RetCode_OK;
}

RetCode p_tsk::await_for_status(PTASK_STATUS target_status,
                                time_t sec,
                                long nsec) const
{
    scoped_mx smx(mon_);
    if(status_ < PTASK_STATUS_SUBMITTED) {
        return RetCode_BADSTTS;
    }
    while(status_ < target_status) {
        wt_th_++;
        mon_.wait();
    }
    return RetCode_OK;
}

// p_executor

p_exectr::p_exectr(p_exec_srv &eserv)  :
    status_(PEXECUTOR_STATUS_INIT),
    eserv_(eserv) {}

p_exectr::~p_exectr()
{}

RetCode p_exectr::set_status(PEXECUTOR_STATUS status)
{
    scoped_mx smx(mon_);
    status_ = status;
    return RetCode_OK;
}

void *p_exectr::run()
{
    PEXEC_SERVICE_STATUS eserv_status = PEXEC_SERVICE_STATUS_ZERO;
    PEXECUTOR_STATUS exec_status = PEXECUTOR_STATUS_ZERO;
    uint32_t tq_size = 0;
    p_tsk *task = nullptr;
    RetCode pres = RetCode_KO;
    bool go_term = true;
    if(status_ != PEXECUTOR_STATUS_INIT && status_ != PEXECUTOR_STATUS_STOPPED) {
        set_status(PEXECUTOR_STATUS_ERROR);
        return (void *)1;
    }
    eserv_.await_for_status_reached(PEXEC_SERVICE_STATUS_STARTED, eserv_status);
    if(eserv_status != PEXEC_SERVICE_STATUS_STARTED) {
        IFLOG(err(TH_ID, LS_TRL "[aborting] [status:%d]", __func__, eserv_.get_status()))
        return (void *)1;
    }
    set_status(PEXECUTOR_STATUS_IDLE);
    while((eserv_status = eserv_.get_status()) == PEXEC_SERVICE_STATUS_STARTED) {
        if(!(pres = eserv_.get_task_queue().get(0, 50*MSEC_F, &task))) {
            set_status(PEXECUTOR_STATUS_EXECUTING);
            task->set_execution_result(task->execute());
            task->set_status(PTASK_STATUS_EXECUTED);
            if(eserv_.is_task_disposer()) {
                delete task;
            }
            set_status(PEXECUTOR_STATUS_IDLE);
        } else if(pres == RetCode_PTHERR) {
            set_status(PEXECUTOR_STATUS_ERROR);
            return (void *)1;
        }
    }
    IFLOG(dbg(TH_ID, LS_TRL "[stopping]", __func__))
    while((eserv_status = eserv_.get_status()) == PEXEC_SERVICE_STATUS_STOPPING) {
        if(!(pres = eserv_.get_task_queue().get(0, 10*MSEC_F, &task))) {
            task->set_status(PTASK_STATUS_SUBMITTED);
            set_status(PEXECUTOR_STATUS_EXECUTING);
            task->set_execution_result(task->execute());
            task->set_status(PTASK_STATUS_EXECUTED);
            if(eserv_.is_task_disposer()) {
                delete task;
            }
            set_status(PEXECUTOR_STATUS_IDLE);
        } else if(pres == RetCode_PTHERR) {
            set_status(PEXECUTOR_STATUS_ERROR);
            return (void *)1;
        }
        if(!(tq_size = eserv_.get_task_queue().size())) {
            set_status(PEXECUTOR_STATUS_DISPOSING);
            for(unsigned int i = 0; i<eserv_.get_executor_count(); i++) {
                exec_status = eserv_.get_executor_at_idx(i).get_status();
                go_term &= ((exec_status == PEXECUTOR_STATUS_DISPOSING) ||
                            (exec_status == PEXECUTOR_STATUS_STOPPED));
            }
            if(go_term) {
                IFLOG(dbg(TH_ID, LS_TRL "[terminating executor service]", __func__))
                eserv_.terminated();
            }
            break;
        }
    }
    set_status(PEXECUTOR_STATUS_STOPPED);
    stop();
    return 0;
}

// p_executor_service
static int p_exec_srv_id = 0;

p_exec_srv::p_exec_srv(bool dispose_task) :
    id_(++p_exec_srv_id),
    status_(PEXEC_SERVICE_STATUS_TOINIT),
    dispose_task_(dispose_task),
    task_queue_(sngl_ptr_obj_mng())
{
}

p_exec_srv::~p_exec_srv()
{}

RetCode p_exec_srv::init(unsigned int executor_num)
{
    IFLOG(trc(TH_ID, LS_OPN "[executor_num:%d]", __func__, executor_num))
    if(!executor_num) {
        IFLOG(inf(TH_ID, LS_TRL "[id:%d][zero executors]", __func__, id_))
        RET_ON_KO(set_status(PEXEC_SERVICE_STATUS_INIT))
    } else {
        exec_pool_.resize(executor_num);
        for(unsigned int i = 0; i<executor_num; i++) {
            exec_pool_[i] = std::move(std::unique_ptr<p_exectr>(new p_exectr(*this)));
        }
        RET_ON_KO(set_status(PEXEC_SERVICE_STATUS_INIT))
    }
    return RetCode_OK;
}

RetCode p_exec_srv::set_status(PEXEC_SERVICE_STATUS status)
{
    IFLOG(trc(TH_ID, LS_OPN "[status:%d]", __func__, status))
    scoped_mx smx(mon_);
    status_ = status;
    mon_.notify_all();
    return RetCode_OK;
}

RetCode p_exec_srv::start()
{
    if(status_ != PEXEC_SERVICE_STATUS_INIT && status_ != PEXEC_SERVICE_STATUS_STOPPED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    RET_ON_KO(set_status(PEXEC_SERVICE_STATUS_STARTING))
    for(unsigned int i = 0; i<exec_pool_.size(); i++) {
        exec_pool_[i]->start();
    }
    RET_ON_KO(set_status(PEXEC_SERVICE_STATUS_STARTED))
    return RetCode_OK;
}

RetCode p_exec_srv::await_for_status_reached(PEXEC_SERVICE_STATUS test,
                                             PEXEC_SERVICE_STATUS &current,
                                             time_t sec,
                                             long nsec)
{
    RetCode rcode = RetCode_OK;
    scoped_mx smx(mon_);
    if(status_ < PEXEC_SERVICE_STATUS_INIT) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
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
    IFLOG(log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO "test:%d [reached] current:%d",
              __func__,
              test,
              status_))
    return rcode;
}

RetCode p_exec_srv::await_termination()
{
    scoped_mx smx(mon_);
    while(status_ < PEXEC_SERVICE_STATUS_STOPPED) {
        mon_.wait();
    }
    return RetCode_OK;
}

RetCode p_exec_srv::await_termination(time_t sec, long nsec)
{
    int pthres;
    scoped_mx smx(mon_);
    while(status_ < PEXEC_SERVICE_STATUS_STOPPED) {
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                IFLOG(inf(TH_ID, LS_CLO "[sec%d, nsec:%d] - [timeout]", __func__, sec, nsec))
                return RetCode_TIMEOUT;
            } else {
                IFLOG(cri(TH_ID, LS_CLO "[sec%d, nsec:%d, pthres:%d] - [pthread error]", __func__, sec, nsec, pthres))
                return RetCode_PTHERR;
            }
        }
    }
    return RetCode_OK;
}

RetCode p_exec_srv::shutdown()
{
    if(exec_pool_.empty()) {
        RET_ON_KO(set_status(PEXEC_SERVICE_STATUS_STOPPED))
    } else {
        RET_ON_KO(set_status(PEXEC_SERVICE_STATUS_STOPPING))
    }
    return RetCode_OK;
}

RetCode p_exec_srv::terminated()
{
    RET_ON_KO(set_status(PEXEC_SERVICE_STATUS_STOPPED))
    return RetCode_OK;
}

RetCode p_exec_srv::submit(p_tsk &task)
{
    if(status_ != PEXEC_SERVICE_STATUS_STARTED) {
        task.set_status(PTASK_STATUS_REJECTED);
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return RetCode_BADSTTS;
    }
    if(exec_pool_.empty()) {
        task.set_execution_result(task.execute());
        task.set_status(PTASK_STATUS_EXECUTED);
        if(is_task_disposer()) {
            delete &task;
        }
        return RetCode_OK;
    }
    RetCode rcode = RetCode_OK;
    scoped_mx smx(mon_);
    if((rcode = task_queue_.put(0, 10*MSEC_F, &task))) {
        task.set_status(PTASK_STATUS_REJECTED);
        switch(rcode) {
            case RetCode_QFULL:
            case RetCode_TIMEOUT:
                IFLOG(wrn(TH_ID, LS_TRL "[id:%d][queue full]", __func__, id_))
            case RetCode_PTHERR:
                IFLOG(cri(TH_ID, LS_TRL "[id:%d][pthread error]", __func__, id_))
            default:
                IFLOG(err(TH_ID, LS_TRL "[id:%d][res:%d]", __func__, id_, rcode))
        }
    } else {
        task.set_status(PTASK_STATUS_SUBMITTED);
    }
    return rcode;
}

}
