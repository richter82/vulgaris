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

const std_shared_ptr_obj_mng<task> tsk_std_shp_omng;

// p_task

RetCode task::set_status(TskStatus status)
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

RetCode task::await_for_status(TskStatus target_status,
                               time_t sec,
                               long nsec) const
{
    std::unique_lock<std::mutex> lck(mtx_);
    if(status_ < TskStatus_SUBMITTED) {
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

exectr::exectr(exec_srv &eserv)  :
    status_(ExecutorStatus_INIT),
    eserv_(eserv) {}

exectr::~exectr()
{}

RetCode exectr::set_status(ExecutorStatus status)
{
    std::unique_lock<std::mutex> lck(mtx_);
    status_ = status;
    return RetCode_OK;
}

void exectr::run()
{
    ExecSrvStatus eserv_status = ExecSrvStatus_TOINIT;
    ExecutorStatus exec_status = ExecutorStatus_INIT;
    uint32_t tq_size = 0;
    std::shared_ptr<task> task;
    RetCode pres = RetCode_KO;
    bool go_term = true;
    if(status_ != ExecutorStatus_INIT && status_ != ExecutorStatus_STOPPED) {
        set_status(ExecutorStatus_ERROR);
    }
    eserv_.await_for_status_reached(ExecSrvStatus_STARTED, eserv_status);
    if(eserv_status != ExecSrvStatus_STARTED) {
        IFLOG(eserv_.log_, error(LS_TRL "[aborting] [status:{}]", __func__, eserv_.get_status()))
    }
    set_status(ExecutorStatus_IDLE);
    while((eserv_status = eserv_.get_status()) == ExecSrvStatus_STARTED) {
        if(!(pres = eserv_.get_task_queue().take(0, 10*MSEC_F, &task))) {
            set_status(ExecutorStatus_EXECUTING);
            task->set_execution_result(task->execute());
            task->set_status(TskStatus_EXECUTED);
            set_status(ExecutorStatus_IDLE);
        } else if(pres == RetCode_PTHERR) {
            set_status(ExecutorStatus_ERROR);
        }
    }
    IFLOG(eserv_.log_, debug(LS_TRL "[stopping]", __func__))
    while((eserv_status = eserv_.get_status()) == ExecSrvStatus_STOPPING) {
        if(!(pres = eserv_.get_task_queue().take(0, 2*MSEC_F, &task))) {
            task->set_status(TskStatus_SUBMITTED);
            set_status(ExecutorStatus_EXECUTING);
            task->set_execution_result(task->execute());
            task->set_status(TskStatus_EXECUTED);
            set_status(ExecutorStatus_IDLE);
        } else if(pres == RetCode_PTHERR) {
            set_status(ExecutorStatus_ERROR);
        }
        if(!(tq_size = eserv_.get_task_queue().size())) {
            set_status(ExecutorStatus_DISPOSING);
            for(unsigned int i = 0; i<eserv_.get_executor_count(); i++) {
                exec_status = eserv_.get_executor_at_idx(i).get_status();
                go_term &= ((exec_status == ExecutorStatus_DISPOSING) ||
                            (exec_status == ExecutorStatus_STOPPED));
            }
            if(go_term) {
                IFLOG(eserv_.log_, debug(LS_TRL "[terminating executor service]", __func__))
                eserv_.terminated();
            }
            break;
        }
    }
    set_status(ExecutorStatus_STOPPED);
    stop();
}

// p_executor_service
static int p_exec_srv_id = 0;

exec_srv::exec_srv(std::shared_ptr<spdlog::logger> &log) :
    id_(++p_exec_srv_id),
    status_(ExecSrvStatus_TOINIT),
    task_queue_(tsk_std_shp_omng),
    log_(log)
{}

exec_srv::~exec_srv()
{}

RetCode exec_srv::init(unsigned int executor_num)
{
    IFLOG(log_, trace(LS_OPN "[executor_num:{}]", __func__, executor_num))
    if(!executor_num) {
        IFLOG(log_, info(LS_TRL "[id:{}][zero executors]", __func__, id_))
        set_status(ExecSrvStatus_INIT);
    } else {
        exec_pool_.resize(executor_num);
        for(unsigned int i = 0; i<executor_num; i++) {
            exec_pool_[i] = std::unique_ptr<exectr>(new exectr(*this));
        }
        set_status(ExecSrvStatus_INIT);
    }
    return RetCode_OK;
}

RetCode exec_srv::set_status(ExecSrvStatus status)
{
    std::unique_lock<std::mutex> lck(mtx_);
    status_ = status;
    cv_.notify_all();
    return RetCode_OK;
}

RetCode exec_srv::start()
{
    if(status_ != ExecSrvStatus_INIT && status_ != ExecSrvStatus_STOPPED) {
        return RetCode_BADSTTS;
    }
    set_status(ExecSrvStatus_STARTING);
    for(unsigned int i = 0; i<exec_pool_.size(); i++) {
        exec_pool_[i]->start();
    }
    set_status(ExecSrvStatus_STARTED);
    return RetCode_OK;
}

RetCode exec_srv::await_for_status_reached(ExecSrvStatus test,
                                           ExecSrvStatus &current,
                                           time_t sec,
                                           long nsec)
{
    RetCode rcode = RetCode_OK;
    std::unique_lock<std::mutex> lck(mtx_);
    if(status_ < ExecSrvStatus_INIT) {
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

RetCode exec_srv::await_termination()
{
    std::unique_lock<std::mutex> lck(mtx_);
    while(status_ < ExecSrvStatus_STOPPED) {
        cv_.wait(lck);
    }
    return RetCode_OK;
}

RetCode exec_srv::await_termination(time_t sec, long nsec)
{
    RetCode rcode = RetCode_OK;
    std::unique_lock<std::mutex> lck(mtx_);
    if(sec<0) {
        cv_.wait(lck,[&]() {
            return status_ >= ExecSrvStatus_STOPPED;
        });
    } else {
        rcode = cv_.wait_for(lck, std::chrono::seconds(sec) + std::chrono::nanoseconds(nsec), [&]() {
            return status_ >= ExecSrvStatus_STOPPED;
        }) ? RetCode_OK : RetCode_TIMEOUT;
    }
    IFLOG(log_, trace(LS_CLO "[{}]", __func__, !rcode ? "terminated" : "timeout"))
    return rcode;
}

RetCode exec_srv::shutdown()
{
    if(exec_pool_.empty()) {
        set_status(ExecSrvStatus_STOPPED);
    } else {
        set_status(ExecSrvStatus_STOPPING);
    }
    return RetCode_OK;
}

RetCode exec_srv::terminated()
{
    set_status(ExecSrvStatus_STOPPED);
    return RetCode_OK;
}

RetCode exec_srv::submit(std::shared_ptr<task> &task)
{
    if(status_ != ExecSrvStatus_STARTED) {
        task->set_status(TskStatus_REJECTED);
        return RetCode_BADSTTS;
    }
    if(exec_pool_.empty()) {
        task->set_execution_result(task->execute());
        task->set_status(TskStatus_EXECUTED);
        return RetCode_OK;
    }
    RetCode rcode = RetCode_OK;
    std::unique_lock<std::mutex> lck(mtx_);
    if((rcode = task_queue_.put(0, 5*MSEC_F, &task))) {
        task->set_status(TskStatus_REJECTED);
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
        task->set_status(TskStatus_SUBMITTED);
    }
    return rcode;
}

}
