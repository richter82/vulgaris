/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "pr_aut.h"

namespace vlg {

// peer_automa_th
struct peer_automa_th : public p_th {
    peer_automa_th(unsigned int id, peer_automa &peer) :
        id_(id),
        peer_(peer) {
    }

    virtual void *run() override {
        IFLOG(peer_.log_, info(LS_APL"#peer automa on this thread#"))
        peer_.running_cycle();
        stop();
        return 0;
    }

    unsigned int id_;
    peer_automa &peer_;
};

void peer_automa::peer_param_clbk_ud(int pnum,
                                     const char *param,
                                     const char *value,
                                     void *ud)
{
    peer_automa *peer = static_cast<peer_automa *>(ud);
    RetCode res = peer->on_automa_load_config(pnum, param, value);
    if(res) {
        peer->peer_last_error_ = res;
    }
    IFLOG(peer->log_, info(" -{} {}", param, value ? value : ""))
}

static unsigned int peer_id = 0;

peer_automa::peer_automa(peer &publ, peer_listener &listener) :
    peer_id_(++peer_id),
    peer_plid_(0),
    peer_svid_(0),
    peer_status_(PeerStatus_ZERO),
    peer_argc_(0),
    peer_argv_(nullptr),
    configured_(false),
    peer_last_error_(RetCode_OK),
    force_disconnect_on_stop_(false),
    publ_(publ),
    listener_(listener),
    log_(get_logger(VLGDEFLOG))
{
    memset(&peer_ver_[0], 0, sizeof(peer_ver_));
}

peer_automa::~peer_automa()
{
    if(!(peer_status_ <= PeerStatus_INITIALIZED) && !(peer_status_ >= PeerStatus_STOPPED)) {
        IFLOG(log_, critical(LS_DTR "[peer:{} is not in a safe state:{}] " LS_EXUNX, __func__, peer_id_, peer_status_))
    }
}

RetCode peer_automa::set_params_file_path_name(const char *file_path)
{
    peer_cfg_file_path_name_ = file_path;
    return RetCode_OK;
}

RetCode peer_automa::early_init()
{
    if((peer_last_error_ = on_automa_early_init())) {
        IFLOG(log_, error(LS_APL "#early init FAIL#"))
        return peer_last_error_;
    }
    peer_status_ = PeerStatus_EARLY;
    IFLOG(log_, info(LS_APL"#early init#"))
    return RetCode_OK;
}

RetCode peer_automa::set_status(PeerStatus peer_status)
{
    IFLOG(log_, trace(LS_OPN "[status:{}]", __func__, peer_status))
    scoped_mx smx(peer_mon_);
    peer_status_ = peer_status;
    listener_.on_status_change(publ_, peer_status_);
    peer_mon_.notify_all();
    return RetCode_OK;
}

RetCode peer_automa::await_for_status_reached(PeerStatus test,
                                              PeerStatus &current,
                                              time_t sec,
                                              long nsec)
{
    scoped_mx smx(peer_mon_);
    if(peer_status_ < PeerStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(peer_status_ < test) {
        int pthres;
        if((pthres = peer_mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  RetCode_TIMEOUT;
                break;
            }
        }
    }
    current = peer_status_;
    IFLOG(log_, debug(LS_CLO "test:{} [reached] current:{}",
                      __func__,
                      test,
                      peer_status_))
    return rcode;
}

RetCode peer_automa::await_for_status_change(PeerStatus &peer_status,
                                             time_t sec,
                                             long nsec)
{
    scoped_mx smx(peer_mon_);
    if(peer_status_ < PeerStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(peer_status == peer_status_) {
        int pthres;
        if((pthres = peer_mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  RetCode_TIMEOUT;
                break;
            }
        }
    }
    IFLOG(log_, trace(LS_CLO "status:{} [changed] current:{}", __func__,
                      peer_status,
                      peer_status_))
    peer_status = peer_status_;
    return rcode;
}

// MACHINE STATE

RetCode peer_automa::start(int argc,
                           char *argv[],
                           bool spawn_new_thread)
{
    if(peer_status_ != PeerStatus_EARLY && peer_status_ != PeerStatus_STOPPED) {
        return RetCode_BADSTTS;
    }
    peer_argc_ = argc;
    peer_argv_ = argv;
    peer_last_error_ = RetCode_OK;
    if(peer_status_ == PeerStatus_EARLY) {
        const char *peer_name = get_automa_name();
        if(!peer_name) {
            IFLOG(log_, critical(LS_APL"#early FAIL: invalid peer name#"))
            return RetCode_EXIT;
        }
        peer_name_ = peer_name;
        const unsigned int *peer_ver = get_automa_version();
        if(!peer_ver) {
            IFLOG(log_, critical(LS_APL"#early FAIL: invalid peer version#"))
            return RetCode_EXIT;
        }
        memcpy(peer_ver_, peer_ver, sizeof(peer_ver_));
        if((peer_last_error_ = step_welcome())) {
            IFLOG(log_, critical(LS_APL"#welcome FAIL with res:{}#", peer_last_error_))
            step_dying_breath();
            return RetCode_EXIT;
        }
        if((peer_last_error_ = step_init())) {
            IFLOG(log_, critical(LS_APL"#init FAIL with res:{}#", peer_last_error_))
            step_dying_breath();
            return RetCode_EXIT;
        }
    } else {
        set_status(PeerStatus_RESTART_REQUESTED);
    }
    if((peer_last_error_ = step_start())) {
        IFLOG(log_, critical(LS_APL"#start FAIL with res:{}#", peer_last_error_))
        step_dying_breath();
        return RetCode_EXIT;
    }
    peer_automa_th *peer_thd = nullptr;
    if(spawn_new_thread) {
        IFLOG(log_, info(LS_APL"#spawning new thread#"))
        peer_thd = new peer_automa_th(peer_id_, *this);
        peer_thd->start();
    } else {
        IFLOG(log_, info(LS_APL"#peer automa on caller thread#"))
        peer_last_error_ = running_cycle();
    }
    return RetCode_OK;
}

RetCode peer_automa::stop(bool force_disconnect)
{
    if(peer_status_ != PeerStatus_RUNNING) {
        return RetCode_BADSTTS;
    }
    force_disconnect_on_stop_ = force_disconnect;
    RetCode rcode = set_stop_request();
    IFLOG(log_, trace(LS_CLO "[res:{}]", __func__, rcode))
    return rcode;
}

//LIFECYCL

RetCode peer_automa::running_cycle()
{
    RetCode handlers_res = RetCode_OK;
    PeerStatus p_status = PeerStatus_ZERO;
    bool move_running = true;
    do {
        if(move_running) {
            if((peer_last_error_ = handlers_res = step_move_running())) {
                IFLOG(log_, critical(LS_APL"#move running FAIL with res:{}#", handlers_res))
                step_dying_breath();
                return RetCode_EXIT;
            }
            //here we are RUNNING
            p_status = PeerStatus_RUNNING;
            await_for_status_change(p_status);
            move_running = false;
        }
        switch(p_status) {
            case PeerStatus_STOP_REQUESTED:
                IFLOG(log_, info(LS_APL"#peer requested to stop#"))
                if((peer_last_error_ = handlers_res = step_stop())) {
                    IFLOG(log_, critical(LS_APL"#stop FAIL with res:{}#", handlers_res))
                    step_dying_breath();
                    return RetCode_EXIT;
                }
                break;
            default:
                IFLOG(log_, critical(LS_APL"#peer in unknown status#"))
                step_dying_breath();
                return RetCode_EXIT;
        }
    } while(peer_status_ != PeerStatus_STOPPED);
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

const char *peer_welcome_fmt = LS_WEL "@{} {}" "@{} {}.{}.{}" "@{} {}";

RetCode peer_automa::step_welcome()
{
    if(peer_status_ > PeerStatus_EARLY) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(peer_welcome_fmt, "PEER:",
                     peer_name_.c_str(),
                     "VER:",
                     peer_ver_[0],
                     peer_ver_[1],
                     peer_ver_[2],
                     "ARCH:",
                     get_arch()))
    set_status(PeerStatus_WELCOMED);
    IFLOG(log_, info(LS_APL"#welcomed#"))
    return RetCode_OK;
}

RetCode peer_automa::step_init()
{
    if(peer_status_ > PeerStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#initializing#"))
    set_status(PeerStatus_INITIALIZING);
    if(!configured_) {
        bool params_chance = false;
        if(peer_argc_ < 2) {
            IFLOG(log_, warn(LS_APL"#init# [invalid argc:{} - peer will try to load params file]", peer_argc_))
            params_chance = true;
        }
        if(params_chance || !strcmp("-file", peer_argv_[1])) {
            if((peer_last_error_ = peer_conf_ldr_.init(peer_cfg_file_path_name_.length() ? peer_cfg_file_path_name_.c_str() :
                                                       "params"))) {
                IFLOG(log_, error(LS_APL"#init FAIL# [loading file params] errcode:{}", peer_last_error_))
                return RetCode_BADCFG;
            }
        } else {
            if((peer_last_error_ = peer_conf_ldr_.init(peer_argc_, peer_argv_))) {
                IFLOG(log_, error(LS_APL"#init FAIL# [loading argv params] errcode:{}", peer_last_error_))
                return RetCode_BADCFG;
            }
        }
        if((peer_last_error_ = peer_conf_ldr_.load_config())) {
            IFLOG(log_, error(LS_APL"#init FAIL# [reading params] errcode:{}", peer_last_error_))
            return RetCode_BADCFG;
        }
        peer_conf_ldr_.enum_params(peer_param_clbk_ud, this);
    } else {
        IFLOG(log_, info(LS_APL"#init# [this peer has already been configured]"))
    }
    if(peer_last_error_) {
        IFLOG(log_, error(LS_APL"#init FAIL# [bad config]"))
        return RetCode_BADCFG;
    }
    if((peer_last_error_ = on_automa_init())) {
        IFLOG(log_, error(LS_APL"#init FAIL# [user hndl]"))
        return peer_last_error_;
    }
    IFLOG(log_, info(LS_APL"#init#"))
    set_status(PeerStatus_INITIALIZED);
    return RetCode_OK;
}

RetCode peer_automa::step_start()
{
    if(peer_status_ > PeerStatus_STARTED) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#starting#"))
    set_status(PeerStatus_STARTING);
    if((peer_last_error_ = on_automa_start())) {
        IFLOG(log_, error(LS_APL"#start FAIL#"))
        return peer_last_error_;
    }
    IFLOG(log_, info(LS_APL"#started#"))
    set_status(PeerStatus_STARTED);
    return RetCode_OK;
}

RetCode peer_automa::step_move_running()
{
    if(peer_status_ != PeerStatus_STARTED) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#move running#"))
    if((peer_last_error_ = on_automa_move_running())) {
        IFLOG(log_, error(LS_APL"#move running FAIL#"))
        return peer_last_error_;
    }
    return set_running();
}

RetCode peer_automa::set_running()
{
    if(peer_status_ > PeerStatus_RUNNING) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#running#"))
    set_status(PeerStatus_RUNNING);
    return RetCode_OK;
}

RetCode peer_automa::step_stop()
{
    if(peer_status_ > PeerStatus_STOPPED) {
        return RetCode_BADSTTS;
    }
    IFLOG(log_, info(LS_APL"#stopping#"))
    set_status(PeerStatus_STOPPING);
    if((peer_last_error_ = on_automa_stop())) {
        IFLOG(log_, error(LS_APL"#stop FAIL#"))
        return peer_last_error_;
    }
    IFLOG(log_, info(LS_APL"#stopped#"))
    set_status(PeerStatus_STOPPED);
    return RetCode_OK;
}

RetCode peer_automa::set_stop_request()
{
    IFLOG(log_, info(LS_APL"#stop request#"))
    set_status(PeerStatus_STOP_REQUESTED);
    return RetCode_OK;
}

void peer_automa::step_dying_breath()
{
    IFLOG(log_, info(LS_APL"#dying breath#"))
    on_automa_dying_breath();
    IFLOG(log_, info(LS_APL"#dying breath emitted#"))
    set_status(PeerStatus_DIED);
}

}
