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

const std_shared_ptr_obj_mng<p_tsk> tsk_std_shp_omng;

// p_task

RetCode p_tsk::set_status(PTskStatus status)
{
    std::unique_lock<std::mutex> lck(mtx_);
    if(status <= status_) {
        return RetCode_BADARG;
    }
    status_ = status;
    if(wt_th_) {
        cv_.notify_all();
        wt_th_ = 0;
    }
    return RetCode_OK;
}

RetCode p_tsk::await_for_status(PTskStatus target_status,
                                time_t sec,
                                long nsec) const
{
    std::unique_lock<std::mutex> lck(mtx_);
    if(status_ < PTskStatus_SUBMITTED) {
        return RetCode_BADSTTS;
    }
    wt_th_++;
    if(sec<0) {
        cv_.wait(lck, [&]() {
            return status_ >= target_status;
        });
    } else {
        if(!cv_.wait_for(lck, std::chrono::seconds(sec) + std::chrono::nanoseconds(nsec), [&]() {
        return status_ >= target_status;
    })) {
            wt_th_--;
        }
    }
    return RetCode_OK;
}

// p_executor

p_exectr::p_exectr(p_exec_srv &eserv)  :
    status_(PExecutorStatus_INIT),
    eserv_(eserv) {}

p_exectr::~p_exectr()
{}

RetCode p_exectr::set_status(PExecutorStatus status)
{
    std::unique_lock<std::mutex> lck(mtx_);
    status_ = status;
    return RetCode_OK;
}

void *p_exectr::run()
{
    PExecSrvStatus eserv_status = PExecSrvStatus_TOINIT;
    PExecutorStatus exec_status = PExecutorStatus_INIT;
    uint32_t tq_size = 0;
    std::shared_ptr<p_tsk> task;
    RetCode pres = RetCode_KO;
    bool go_term = true;
    if(status_ != PExecutorStatus_INIT && status_ != PExecutorStatus_STOPPED) {
        set_status(PExecutorStatus_ERROR);
        return (void *)1;
    }
    eserv_.await_for_status_reached(PExecSrvStatus_STARTED, eserv_status);
    if(eserv_status != PExecSrvStatus_STARTED) {
        IFLOG(eserv_.log_, error(LS_TRL "[aborting] [status:{}]", __func__, eserv_.get_status()))
        return (void *)1;
    }
    set_status(PExecutorStatus_IDLE);
    while((eserv_status = eserv_.get_status()) == PExecSrvStatus_STARTED) {
        if(!(pres = eserv_.get_task_queue().take(0, 10*MSEC_F, &task))) {
            set_status(PExecutorStatus_EXECUTING);
            task->set_execution_result(task->execute());
            task->set_status(PTskStatus_EXECUTED);
            set_status(PExecutorStatus_IDLE);
        } else if(pres == RetCode_PTHERR) {
            set_status(PExecutorStatus_ERROR);
            return (void *)1;
        }
    }
    IFLOG(eserv_.log_, debug(LS_TRL "[stopping]", __func__))
    while((eserv_status = eserv_.get_status()) == PExecSrvStatus_STOPPING) {
        if(!(pres = eserv_.get_task_queue().take(0, 2*MSEC_F, &task))) {
            task->set_status(PTskStatus_SUBMITTED);
            set_status(PExecutorStatus_EXECUTING);
            task->set_execution_result(task->execute());
            task->set_status(PTskStatus_EXECUTED);
            set_status(PExecutorStatus_IDLE);
        } else if(pres == RetCode_PTHERR) {
            set_status(PExecutorStatus_ERROR);
            return (void *)1;
        }
        if(!(tq_size = eserv_.get_task_queue().size())) {
            set_status(PExecutorStatus_DISPOSING);
            for(unsigned int i = 0; i<eserv_.get_executor_count(); i++) {
                exec_status = eserv_.get_executor_at_idx(i).get_status();
                go_term &= ((exec_status == PExecutorStatus_DISPOSING) ||
                            (exec_status == PExecutorStatus_STOPPED));
            }
            if(go_term) {
                IFLOG(eserv_.log_, debug(LS_TRL "[terminating executor service]", __func__))
                eserv_.terminated();
            }
            break;
        }
    }
    set_status(PExecutorStatus_STOPPED);
    stop();
    return 0;
}

// p_executor_service
static int p_exec_srv_id = 0;

p_exec_srv::p_exec_srv(std::shared_ptr<spdlog::logger> &log) :
    id_(++p_exec_srv_id),
    status_(PExecSrvStatus_TOINIT),
    task_queue_(tsk_std_shp_omng),
    log_(log)
{}

p_exec_srv::~p_exec_srv()
{}

RetCode p_exec_srv::init(unsigned int executor_num)
{
    IFLOG(log_, trace(LS_OPN "[executor_num:{}]", __func__, executor_num))
    if(!executor_num) {
        IFLOG(log_, info(LS_TRL "[id:{}][zero executors]", __func__, id_))
        set_status(PExecSrvStatus_INIT);
    } else {
        exec_pool_.resize(executor_num);
        for(unsigned int i = 0; i<executor_num; i++) {
            exec_pool_[i] = std::unique_ptr<p_exectr>(new p_exectr(*this));
        }
        set_status(PExecSrvStatus_INIT);
    }
    return RetCode_OK;
}

RetCode p_exec_srv::set_status(PExecSrvStatus status)
{
    std::unique_lock<std::mutex> lck(mtx_);
    status_ = status;
    cv_.notify_all();
    return RetCode_OK;
}

RetCode p_exec_srv::start()
{
    if(status_ != PExecSrvStatus_INIT && status_ != PExecSrvStatus_STOPPED) {
        return RetCode_BADSTTS;
    }
    set_status(PExecSrvStatus_STARTING);
    for(unsigned int i = 0; i<exec_pool_.size(); i++) {
        exec_pool_[i]->start();
    }
    set_status(PExecSrvStatus_STARTED);
    return RetCode_OK;
}

RetCode p_exec_srv::await_for_status_reached(PExecSrvStatus test,
                                             PExecSrvStatus &current,
                                             time_t sec,
                                             long nsec)
{
    RetCode rcode = RetCode_OK;
    std::unique_lock<std::mutex> lck(mtx_);
    if(status_ < PExecSrvStatus_INIT) {
        return RetCode_BADSTTS;
    }
    if(sec<0) {
        cv_.wait(lck, [&]() {
            return status_ >= test;
        });
    } else {
        rcode = cv_.wait_for(lck, std::chrono::seconds(sec) + std::chrono::nanoseconds(nsec), [&]() {
            return status_ >= test;
        }) ? RetCode_OK : RetCode_TIMEOUT;
    }
    current = status_;
    IFLOG(log_, trace(LS_CLO "test:{} [{}] current:{}", __func__, test, !rcode ? "reached" : "timeout", status_))
    return rcode;
}

RetCode p_exec_srv::await_termination()
{
    std::unique_lock<std::mutex> lck(mtx_);
    while(status_ < PExecSrvStatus_STOPPED) {
        cv_.wait(lck);
    }
    return RetCode_OK;
}

RetCode p_exec_srv::await_termination(time_t sec, long nsec)
{
    RetCode rcode = RetCode_OK;
    std::unique_lock<std::mutex> lck(mtx_);
    rcode = cv_.wait_for(lck, std::chrono::seconds(sec) + std::chrono::nanoseconds(nsec), [&]() {
        return status_ >= PExecSrvStatus_STOPPED;
    }) ? RetCode_OK : RetCode_TIMEOUT;
    IFLOG(log_, trace(LS_CLO "[sec{}, nsec:{}] - [{}]", __func__, sec, nsec, !rcode ? "terminated" : "timeout"))
    return rcode;
}

RetCode p_exec_srv::shutdown()
{
    if(exec_pool_.empty()) {
        set_status(PExecSrvStatus_STOPPED);
    } else {
        set_status(PExecSrvStatus_STOPPING);
    }
    return RetCode_OK;
}

RetCode p_exec_srv::terminated()
{
    set_status(PExecSrvStatus_STOPPED);
    return RetCode_OK;
}

RetCode p_exec_srv::submit(std::shared_ptr<p_tsk> &task)
{
    if(status_ != PExecSrvStatus_STARTED) {
        task->set_status(PTskStatus_REJECTED);
        return RetCode_BADSTTS;
    }
    if(exec_pool_.empty()) {
        task->set_execution_result(task->execute());
        task->set_status(PTskStatus_EXECUTED);
        return RetCode_OK;
    }
    RetCode rcode = RetCode_OK;
    std::unique_lock<std::mutex> lck(mtx_);
    if((rcode = task_queue_.put(0, 5*MSEC_F, &task))) {
        task->set_status(PTskStatus_REJECTED);
        switch(rcode) {
            case RetCode_QFULL:
            case RetCode_TIMEOUT:
                IFLOG(log_, warn(LS_TRL "[id:{}][queue full]", __func__, id_))
                break;
            case RetCode_PTHERR:
                IFLOG(log_, critical(LS_TRL "[id:{}][pthread error]", __func__, id_))
                break;
            default:
                IFLOG(log_, error(LS_TRL "[id:{}][res:{}]", __func__, id_, rcode))
                break;
        }
    } else {
        task->set_status(PTskStatus_SUBMITTED);
    }
    return rcode;
}

}
