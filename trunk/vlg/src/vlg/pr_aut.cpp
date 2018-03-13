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

#include "pr_aut.h"

namespace vlg {

// peer_automa_th
class peer_automa_th : public vlg::p_th {
    public:
        peer_automa_th(unsigned int id, peer_automa &peer) :
            id_(id),
            peer_(peer) {
        }

        virtual void *run() {
            IFLOG(trc(TH_ID, LS_OPN, __func__))
            IFLOG(inf(TH_ID, LS_APL"#peer automa on this thread#"))
            peer_.running_cycle();
            int stop_res = stop();
            IFLOG(trc(TH_ID, LS_CLO"[res:%d]", __func__, stop_res))
            return 0;
        }

    private:
        unsigned int id_;
        peer_automa &peer_;
};

// VLG_PEER_LFCYC

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
    IFLOG(pln(" -%-20s %s", param, value ? value : ""))
}

// VLG_PEER_LFCYC CTORS - INIT - DESTROY

static unsigned int peer_id = 0;

peer_automa::peer_automa(peer &publ) :
    peer_id_(++peer_id),
    peer_plid_(0),
    peer_svid_(0),
    peer_status_(PeerStatus_ZERO),
    peer_argc_(0),
    peer_argv_(nullptr),
    configured_(false),
    peer_last_error_(vlg::RetCode_OK),
    peer_exit_required_(false),
    force_disconnect_on_stop_(false),
    publ_(publ)
{
    memset(&peer_ver_[0], 0, sizeof(peer_ver_));
}

peer_automa::~peer_automa()
{
    if(!(peer_status_ <= PeerStatus_INITIALIZED) && !(peer_status_ >= PeerStatus_STOPPED)) {
        IFLOG(cri(TH_ID, LS_DTR "[peer:%d is not in a safe state:%d] " LS_EXUNX, __func__, peer_id_, peer_status_))
    }
}

RetCode peer_automa::set_params_file_path_name(const char *file_path)
{
    peer_cfg_file_path_name_ = file_path;
    return vlg::RetCode_OK;
}

RetCode peer_automa::early_init()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if((peer_last_error_ = on_automa_early_init())) {
        IFLOG(err(TH_ID, LS_APL "#early init FAIL#"))
        return peer_last_error_;
    }
    peer_status_ = PeerStatus_EARLY;
    IFLOG(inf(TH_ID, LS_APL"#early init#"))
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

// STATUS

RetCode peer_automa::set_status(PeerStatus peer_status)
{
    IFLOG(dbg(TH_ID, LS_OPN "[status:%d]", __func__, peer_status))
    scoped_mx smx(peer_mon_);
    peer_status_ = peer_status;
    publ_.on_status_change(peer_status_);
    peer_mon_.notify_all();
    IFLOG(dbg(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode peer_automa::await_for_status_reached_or_outdated(PeerStatus test,
                                                          PeerStatus &current,
                                                          time_t sec,
                                                          long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(peer_mon_);
    if(peer_status_ < PeerStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    while(peer_status_ < test) {
        int pthres;
        if((pthres = peer_mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    current = peer_status_;
    IFLOG(dbg(TH_ID, LS_CLO "test:%d [reached or outdated] current:%d",
              __func__,
              test,
              peer_status_))
    return rcode;
}

RetCode peer_automa::await_for_status_change(PeerStatus &peer_status,
                                             time_t sec,
                                             long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(peer_mon_);
    if(peer_status_ < PeerStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    while(peer_status == peer_status_) {
        int pthres;
        if((pthres = peer_mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    IFLOG(dbg(TH_ID,
              LS_CLO "status:%d [changed] current:%d", __func__,
              peer_status,
              peer_status_))
    peer_status = peer_status_;
    return rcode;
}

// MACHINE STATE

//ACTIONS

RetCode peer_automa::start(int argc,
                           char *argv[],
                           bool spawn_new_thread)
{
    IFLOG(trc(TH_ID, LS_OPN "[peer_id:%d]", __func__, peer_id_))
    if(peer_status_ != PeerStatus_EARLY && peer_status_ != PeerStatus_STOPPED) {
        IFLOG(err(TH_ID, LS_CLO "[res:%d]", __func__, vlg::RetCode_BADSTTS))
        return vlg::RetCode_BADSTTS;
    }
    peer_argc_ = argc;
    peer_argv_ = argv;
    peer_last_error_ = vlg::RetCode_OK;
    if(peer_status_ == PeerStatus_EARLY) {
        const char *peer_name = get_automa_name();
        if(!peer_name) {
            IFLOG(fat(TH_ID, LS_APL"#early FAIL: invalid peer name#"))
            return vlg::RetCode_EXIT;
        }
        peer_name_ = peer_name;
        const unsigned int *peer_ver = get_automa_version();
        if(!peer_ver) {
            IFLOG(fat(TH_ID, LS_APL"#early FAIL: invalid peer version#"))
            return vlg::RetCode_EXIT;
        }
        memcpy(peer_ver_, peer_ver, sizeof(peer_ver_));
        if((peer_last_error_ = step_welcome())) {
            IFLOG(fat(TH_ID, LS_APL"#welcome FAIL with res:%d#", peer_last_error_))
            step_dying_breath();
            return vlg::RetCode_EXIT;
        }
        if((peer_last_error_ = step_init())) {
            IFLOG(fat(TH_ID, LS_APL"#init FAIL with res:%d#", peer_last_error_))
            step_dying_breath();
            return vlg::RetCode_EXIT;
        }
    } else {
        set_status(PeerStatus_RESTART_REQUESTED);
    }
    if((peer_last_error_ = step_start())) {
        IFLOG(fat(TH_ID, LS_APL"#start FAIL with res:%d#", peer_last_error_))
        step_dying_breath();
        return vlg::RetCode_EXIT;
    }
    peer_automa_th *peer_thd = nullptr;
    if(spawn_new_thread) {
        IFLOG(inf(TH_ID, LS_APL"#spawning new thread#"))
        peer_thd = new peer_automa_th(peer_id_, *this);
        peer_thd->start();
    } else {
        IFLOG(inf(TH_ID, LS_APL"#peer automa on caller thread#"))
        peer_last_error_ = running_cycle();
    }
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode peer_automa::stop(bool force_disconnect)
{
    IFLOG(trc(TH_ID, LS_OPN "[peer_id:%d]", __func__, peer_id_))
    if(peer_status_ != PeerStatus_RUNNING) {
        IFLOG(err(TH_ID, LS_CLO "[res:%d]", __func__, vlg::RetCode_BADSTTS))
        return vlg::RetCode_BADSTTS;
    }
    force_disconnect_on_stop_ = force_disconnect;
    RetCode rcode = set_stop_request();
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

//LIFECYCL

RetCode peer_automa::running_cycle()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode handlers_res = vlg::RetCode_OK;
    PeerStatus p_status = PeerStatus_ZERO;
    bool move_running = true;
    do {
        if(move_running) {
            if((peer_last_error_ = handlers_res = step_move_running())) {
                IFLOG(cri(TH_ID, LS_APL"#move running FAIL with res:%d#", handlers_res))
                step_dying_breath();
                return vlg::RetCode_EXIT;
            }
            //here we are RUNNING
            p_status = PeerStatus_RUNNING;
            await_for_status_change(p_status);
            move_running = false;
        }
        switch(p_status) {
            case PeerStatus_STOP_REQUESTED:
                IFLOG(inf(TH_ID, LS_APL"#peer requested to stop#"))
                if((peer_last_error_ = handlers_res = step_stop())) {
                    IFLOG(cri(TH_ID, LS_APL"#stop FAIL with res:%d#", handlers_res))
                    step_dying_breath();
                    return vlg::RetCode_EXIT;
                }
                break;
            case PeerStatus_ERROR:
                IFLOG(inf(TH_ID, LS_APL"#calling error handler#"))
                if((peer_last_error_ = handlers_res = on_automa_error())) {
                    IFLOG(cri(TH_ID, LS_APL"#error handler FAIL with res:%d#", handlers_res))
                    step_dying_breath();
                    return vlg::RetCode_EXIT;
                }
                if(peer_exit_required_) {
                    IFLOG(fat(TH_ID, LS_APL"#error handler required peer to exit#"))
                    return vlg::RetCode_EXIT;
                } else {
                    IFLOG(wrn(TH_ID, LS_APL"#peer recovered - check is needed#"))
                    move_running = true;
                }
                break;
            default:
                IFLOG(fat(TH_ID, LS_APL"#peer in unknown status#"))
                step_dying_breath();
                return vlg::RetCode_EXIT;
        }
    } while(peer_status_ != PeerStatus_STOPPED);
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
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

const char *peer_welcome_fmt = LS_EMP "@%s %s" "@%s %u.%u.%u" "@%s %s";

RetCode peer_automa::step_welcome()
{
    if(peer_status_ > PeerStatus_EARLY) {
        IFLOG(err(TH_ID, LS_CLO "[res:%d]", __func__, vlg::RetCode_BADSTTS))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID, peer_welcome_fmt, "PEER:",
              peer_name_,
              "VER:",
              peer_ver_[0],
              peer_ver_[1],
              peer_ver_[2],
              "ARCH:",
              vlg::get_arch()))
    set_status(PeerStatus_WELCOMED);
    IFLOG(inf(TH_ID, LS_APL"#welcomed#"))
    return vlg::RetCode_OK;
}

RetCode peer_automa::step_init()
{
    if(peer_status_ > PeerStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO "[res:%d]", __func__, vlg::RetCode_BADSTTS))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID, LS_APL"#initializing#"))
    set_status(PeerStatus_INITIALIZING);
    if(!configured_) {
        bool params_chance = false;
        if(peer_argc_ < 2) {
            IFLOG(wrn(TH_ID, LS_APL"#init# [invalid argc:%d - peer will try to load params file]", peer_argc_))
            params_chance = true;
        }
        if(params_chance || !strcmp("-file", peer_argv_[1])) {
            if((peer_last_error_ = peer_conf_ldr_.init(peer_cfg_file_path_name_.length() ? peer_cfg_file_path_name_.c_str() :
                                                       "params"))) {
                IFLOG(err(TH_ID, LS_APL"#init FAIL# [loading file params] errcode:%d", peer_last_error_))
                return vlg::RetCode_BADCFG;
            }
        } else {
            if((peer_last_error_ = peer_conf_ldr_.init(peer_argc_, peer_argv_))) {
                IFLOG(err(TH_ID, LS_APL"#init FAIL# [loading argv params] errcode:%d", peer_last_error_))
                return vlg::RetCode_BADCFG;
            }
        }
        if((peer_last_error_ = peer_conf_ldr_.load_config())) {
            IFLOG(err(TH_ID, LS_APL"#init FAIL# [reading params] errcode:%d", peer_last_error_))
            return vlg::RetCode_BADCFG;
        }
        peer_conf_ldr_.enum_params(peer_param_clbk_ud, this);
    } else {
        IFLOG(inf(TH_ID, LS_APL"#init# [this peer has already been configured]"))
    }
    if(peer_last_error_) {
        IFLOG(err(TH_ID, LS_APL"#init FAIL# [bad config]"))
        return vlg::RetCode_BADCFG;
    }
    if((peer_last_error_ = on_automa_init())) {
        IFLOG(err(TH_ID, LS_APL"#init FAIL# [user handler]"))
        return peer_last_error_;
    }
    IFLOG(inf(TH_ID, LS_APL"#init#"))
    set_status(PeerStatus_INITIALIZED);
    return vlg::RetCode_OK;
}

RetCode peer_automa::step_start()
{
    if(peer_status_ > PeerStatus_STARTED) {
        IFLOG(err(TH_ID, LS_CLO "[res:%d]", __func__, vlg::RetCode_BADSTTS))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID, LS_APL"#starting#"))
    set_status(PeerStatus_STARTING);
    if((peer_last_error_ = on_automa_start())) {
        IFLOG(err(TH_ID, LS_APL"#start FAIL#"))
        return peer_last_error_;
    }
    IFLOG(inf(TH_ID, LS_APL"#started#"))
    set_status(PeerStatus_STARTED);
    return vlg::RetCode_OK;
}

RetCode peer_automa::step_move_running()
{
    if(peer_status_ != PeerStatus_STARTED) {
        IFLOG(err(TH_ID, LS_CLO "[res:%d]", __func__, vlg::RetCode_BADSTTS))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID, LS_APL"#move running#"))
    if((peer_last_error_ = on_automa_move_running())) {
        IFLOG(err(TH_ID, LS_APL"#move running FAIL#"))
        return peer_last_error_;
    }
    return set_running();
}

RetCode peer_automa::set_running()
{
    if(peer_status_ > PeerStatus_RUNNING) {
        IFLOG(err(TH_ID, LS_CLO "[res:%d]", __func__, vlg::RetCode_BADSTTS))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID, LS_APL"#running#"))
    set_status(PeerStatus_RUNNING);
    return vlg::RetCode_OK;
}

RetCode peer_automa::step_stop()
{
    if(peer_status_ > PeerStatus_STOPPED) {
        IFLOG(err(TH_ID, LS_CLO "[res:%d]", __func__, vlg::RetCode_BADSTTS))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID, LS_APL"#stopping#"))
    set_status(PeerStatus_STOPPING);
    if((peer_last_error_ = on_automa_stop())) {
        IFLOG(err(TH_ID, LS_APL"#stop FAIL#"))
        return peer_last_error_;
    }
    IFLOG(inf(TH_ID, LS_APL"#stopped#"))
    set_status(PeerStatus_STOPPED);
    return vlg::RetCode_OK;
}

RetCode peer_automa::set_stop_request()
{
    IFLOG(inf(TH_ID, LS_APL"#stop request#"))
    set_status(PeerStatus_STOP_REQUESTED);
    return vlg::RetCode_OK;
}

RetCode peer_automa::set_error()
{
    IFLOG(err(TH_ID, LS_APL"#error#"))
    set_status(PeerStatus_ERROR);
    return vlg::RetCode_OK;
}

void peer_automa::step_dying_breath()
{
    IFLOG(inf(TH_ID, LS_APL"#dying breath#"))
    on_automa_dying_breath();
    IFLOG(inf(TH_ID, LS_APL"#dying breath emitted#"))
    set_status(PeerStatus_DIED);
}

logger *v_log_ = logger::get_logger("vlg");

}
