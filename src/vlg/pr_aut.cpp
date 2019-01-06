/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "pr_aut.h"

namespace vlg {

// broker_automa_th
struct broker_automa_th : public th {
    broker_automa_th(unsigned int id, broker_automa &broker) :
        id_(id),
        broker_(broker) {
    }

    virtual void run() override {
        IFLOG(broker_.log_, info(LS_APL"#broker automa on this thread#"))
        broker_.running_cycle();
        stop();
    }

    unsigned int id_;
    broker_automa &broker_;
};

void broker_automa::broker_param_clbk_ud(int pnum,
                                         const char *param,
                                         const char *value,
                                         void *ud)
{
    broker_automa *broker = static_cast<broker_automa *>(ud);
    RetCode res = broker->on_automa_load_config(pnum, param, value);
    if(res) {
        broker->broker_last_error_ = res;
    }
    IFLOG(broker->log_, info(" -{} {}", param, value ? value : ""))
}

static unsigned int broker_id = 0;

broker_automa::broker_automa(broker &publ, broker_listener &listener) :
    broker_id_(++broker_id),
    broker_plid_(0),
    broker_svid_(0),
    broker_status_(PeerStatus_ZERO),
    broker_argc_(0),
    broker_argv_(nullptr),
    configured_(false),
    broker_last_error_(RetCode_OK),
    force_disconnect_on_stop_(false),
    publ_(publ),
    listener_(listener),
    log_(get_logger(VLGDEFLOG))
{
    memset(&broker_ver_[0], 0, sizeof(broker_ver_));
}

broker_automa::~broker_automa()
{
    if(!(broker_status_ <= PeerStatus_INITIALIZED) && !(broker_status_ >= PeerStatus_STOPPED)) {
        IFLOG(log_, critical(LS_DTR "[broker:{} is not in a safe state:{}] " LS_EXUNX, __func__, broker_id_, broker_status_))
    }
}

RetCode broker_automa::set_params_file_path_name(const char *file_path)
{
    broker_cfg_file_path_name_ = file_path;
    return RetCode_OK;
}

RetCode broker_automa::early_init()
{
    if((broker_last_error_ = on_automa_early_init())) {
        IFLOG(log_, error(LS_APL "#early init FAIL#"))
        return broker_last_error_;
    }
    broker_status_ = PeerStatus_EARLY;
    IFLOG(log_, info(LS_APL"#early init#"))
    return RetCode_OK;
}

RetCode broker_automa::set_status(PeerStatus broker_status)
{
    IFLOG(log_, trace(LS_OPN "[status:{}]", __func__, broker_status))
    std::unique_lock<std::mutex> lck(broker_mtx_);
    broker_status_ = broker_status;
    listener_.on_status_change(publ_, broker_status_);
    broker_cv_.notify_all();
    return RetCode_OK;
}

RetCode broker_automa::await_for_status_reached(PeerStatus test,
                                                PeerStatus &current,
                                                time_t sec,
                                                long nsec)
{
    RetCode rcode = RetCode_OK;
    std::unique_lock<std::mutex> lck(broker_mtx_);
    if(broker_status_ < PeerStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    if(sec<0) {
        broker_cv_.wait(lck,[&]() {
            return broker_status_ >= test;
        });
    } else {
        rcode = broker_cv_.wait_for(lck,std::chrono::seconds(sec) + std::chrono::nanoseconds(nsec),[&]() {
            return broker_status_ >= test;
        }) ? RetCode_OK : RetCode_TIMEOUT;
    }
    current = broker_status_;
    IFLOG(log_, trace(LS_CLO "test:{} [{}] current:{}", __func__, test, !rcode ? "reached" : "timeout", broker_status_))
    return rcode;
}

RetCode broker_automa::await_for_status_change(PeerStatus &broker_status,
                                               time_t sec,
                                               long nsec)
{
    RetCode rcode = RetCode_OK;
    std::unique_lock<std::mutex> lck(broker_mtx_);
    if(broker_status_ < PeerStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    if(sec<0) {
        broker_cv_.wait(lck,[&]() {
            return broker_status_ != broker_status;
        });
    } else {
        rcode = broker_cv_.wait_for(lck, std::chrono::seconds(sec) + std::chrono::nanoseconds(nsec), [&]() {
            return broker_status_ != broker_status;
        }) ? RetCode_OK : RetCode_TIMEOUT;
    }
    IFLOG(log_, trace(LS_CLO "test:{} [{}] current:{}", __func__, broker_status, !rcode ? "changed" : "timeout", broker_status_))
    broker_status = broker_status_;
    return rcode;
}

// MACHINE STATE

RetCode broker_automa::start(int argc,
                             char *argv[],
                             bool spawn_new_thread)
{
    if(broker_status_ != PeerStatus_EARLY &&
            broker_status_ != PeerStatus_STOPPED &&
            broker_status_ != PeerStatus_ERROR) {
        return RetCode_BADSTTS;
    }
    broker_argc_ = argc;
    broker_argv_ = argv;
    broker_last_error_ = RetCode_OK;
    if(broker_status_ == PeerStatus_EARLY) {
        const char *broker_name = get_automa_name();
        if(!broker_name) {
            IFLOG(log_, critical(LS_APL"#early FAIL: invalid broker name#"))
            return RetCode_EXIT;
        }
        broker_name_ = broker_name;
        const unsigned int *broker_ver = get_automa_version();
        if(!broker_ver) {
            IFLOG(log_, critical(LS_APL"#early FAIL: invalid broker version#"))
            return RetCode_EXIT;
        }
        memcpy(broker_ver_, broker_ver, sizeof(broker_ver_));
        if((broker_last_error_ = step_welcome())) {
            IFLOG(log_, critical(LS_APL"#welcome FAIL with res:{}#", broker_last_error_))
            return RetCode_EXIT;
        }
        if((broker_last_error_ = step_init())) {
            IFLOG(log_, error(LS_APL"#init FAIL with res:{}#", broker_last_error_))
            step_error();
        }
    } else {
        set_status(PeerStatus_RESTART_REQUESTED);
    }
    if((broker_last_error_ = step_start())) {
        IFLOG(log_, error(LS_APL"#start FAIL with res:{}#", broker_last_error_))
        step_error();
    }
    broker_automa_th *broker_thd = nullptr;
    if(spawn_new_thread) {
        IFLOG(log_, info(LS_APL"#spawning new thread#"))
        broker_thd = new broker_automa_th(broker_id_, *this);
        broker_thd->start();
    } else {
        IFLOG(log_, info(LS_APL"#broker automa on caller thread#"))
        broker_last_error_ = running_cycle();
    }
    return RetCode_OK;
}

RetCode broker_automa::stop(bool force_disconnect)
{
    if(broker_status_ != PeerStatus_RUNNING) {
        return RetCode_BADSTTS;
    }
    force_disconnect_on_stop_ = force_disconnect;
    RetCode rcode = set_stop_request();
    IFLOG(log_, trace(LS_CLO "[res:{}]", __func__, rcode))
    return rcode;
}

//LIFECYCL

RetCode broker_automa::running_cycle()
{
    RetCode handlers_res = RetCode_OK;
    PeerStatus p_status = PeerStatus_ZERO;
    bool move_running = true;
    do {
        if(move_running) {
            if((broker_last_error_ = handlers_res = step_move_running())) {
                IFLOG(log_, error(LS_APL"#move running FAIL with res:{}#", handlers_res))
                step_error();
            }
            //here we are RUNNING
            p_status = PeerStatus_RUNNING;
            await_for_status_change(p_status);
            move_running = false;
        }
        switch(p_status) {
            case PeerStatus_STOP_REQUESTED:
                IFLOG(log_, info(LS_APL"#broker requested to stop#"))
                if((broker_last_error_ = handlers_res = step_stop())) {
                    IFLOG(log_, error(LS_APL"#stop FAIL with res:{}#", handlers_res))
                    step_error();
                }
                break;
            default:
                IFLOG(log_, critical(LS_APL"#broker in unknown status#"))
                step_error();
        }
    } while(broker_status_ != PeerStatus_STOPPED && broker_status_ != PeerStatus_ERROR);
    return RetCode_OK;
}

// GET ARCH
static const char *get_arch()
{
#if defined WIN32 && defined _MSC_VER
    return "x86_64@WIN_MSVC";
#endif
#if defined __amd64__ && defined __linux
    return "x86_64@LNX_GCC";
#endif
#if defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
    return "x86_64@ios_sim";
#elif TARGET_OS_IPHONE
    return "x86_64@ios";
#elif TARGET_OS_MAC
    return "x86_64@osx";
#endif
#endif
}

const char *broker_welcome_fmt = LS_WEL "@{} {}" "@{} {}.{}.{}" "@{} {}";

RetCode broker_automa::step_welcome()
{
    if(broker_status_ > PeerStatus_EARLY) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(broker_welcome_fmt, "BROKER:",
                     broker_name_.c_str(),
                     "VER:",
                     broker_ver_[0],
                     broker_ver_[1],
                     broker_ver_[2],
                     "ARCH:",
                     get_arch()))
    set_status(PeerStatus_WELCOMED);
    IFLOG(log_, info(LS_APL"#welcomed#"))
    return RetCode_OK;
}

RetCode broker_automa::step_init()
{
    if(broker_status_ > PeerStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#initializing#"))
    set_status(PeerStatus_INITIALIZING);
    if(!configured_) {
        bool params_chance = false;
        if(broker_argc_ < 2) {
            IFLOG(log_, warn(LS_APL"#init# [invalid argc:{} - broker will try to load params file]", broker_argc_))
            params_chance = true;
        }
        if(params_chance || !strcmp("-file", broker_argv_[1])) {
            if((broker_last_error_ = broker_conf_ldr_.init(broker_cfg_file_path_name_.length() ? broker_cfg_file_path_name_.c_str() :
                                                           "params"))) {
                IFLOG(log_, error(LS_APL"#init FAIL# [loading file params] errcode:{}", broker_last_error_))
                return RetCode_BADCFG;
            }
        } else {
            if((broker_last_error_ = broker_conf_ldr_.init(broker_argc_, broker_argv_))) {
                IFLOG(log_, error(LS_APL"#init FAIL# [loading argv params] errcode:{}", broker_last_error_))
                return RetCode_BADCFG;
            }
        }
        if((broker_last_error_ = broker_conf_ldr_.load_config())) {
            IFLOG(log_, error(LS_APL"#init FAIL# [reading params] errcode:{}", broker_last_error_))
            return RetCode_BADCFG;
        }
        broker_conf_ldr_.enum_params(broker_param_clbk_ud, this);
    } else {
        IFLOG(log_, info(LS_APL"#init# [this broker has already been configured]"))
    }
    if(broker_last_error_) {
        IFLOG(log_, error(LS_APL"#init FAIL# [bad config]"))
        return RetCode_BADCFG;
    }
    if((broker_last_error_ = on_automa_init())) {
        IFLOG(log_, error(LS_APL"#init FAIL# [user hndl]"))
        return broker_last_error_;
    }
    IFLOG(log_, info(LS_APL"#init#"))
    set_status(PeerStatus_INITIALIZED);
    return RetCode_OK;
}

RetCode broker_automa::step_start()
{
    if(broker_status_ > PeerStatus_STARTED) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#starting#"))
    set_status(PeerStatus_STARTING);
    if((broker_last_error_ = on_automa_start())) {
        IFLOG(log_, error(LS_APL"#start FAIL#"))
        return broker_last_error_;
    }
    IFLOG(log_, info(LS_APL"#started#"))
    set_status(PeerStatus_STARTED);
    return RetCode_OK;
}

RetCode broker_automa::step_move_running()
{
    if(broker_status_ != PeerStatus_STARTED) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#move running#"))
    if((broker_last_error_ = on_automa_move_running())) {
        IFLOG(log_, error(LS_APL"#move running FAIL#"))
        return broker_last_error_;
    }
    return set_running();
}

RetCode broker_automa::set_running()
{
    if(broker_status_ > PeerStatus_RUNNING) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#running#"))
    set_status(PeerStatus_RUNNING);
    return RetCode_OK;
}

RetCode broker_automa::step_stop()
{
    if(broker_status_ > PeerStatus_STOPPED) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#stopping#"))
    set_status(PeerStatus_STOPPING);
    if((broker_last_error_ = on_automa_stop())) {
        IFLOG(log_, error(LS_APL"#stop FAIL#"))
        return broker_last_error_;
    }
    IFLOG(log_, info(LS_APL"#stopped#"))
    set_status(PeerStatus_STOPPED);
    return RetCode_OK;
}

RetCode broker_automa::set_stop_request()
{
    IFLOG(log_, info(LS_APL"#stop request#"))
    set_status(PeerStatus_STOP_REQUESTED);
    return RetCode_OK;
}

void broker_automa::step_error()
{
    IFLOG(log_, warn(LS_APL"#going error#"))
    on_automa_error();
    IFLOG(log_, error(LS_APL"#error#"))
    set_status(PeerStatus_ERROR);
}

}
