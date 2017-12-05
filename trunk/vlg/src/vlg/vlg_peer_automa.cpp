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

#include "cr.h"
#include "vlg_peer_automa.h"

namespace vlg {


// VLG_PEER_LFCYC_PTHD

class peer_automa_th : public vlg::p_thread {
    public:
        peer_automa_th(unsigned int id, peer_automa &peer) :
            id_(id),
            peer_(peer) {
            log_ = vlg::logger::get_logger("peer_lifecyc_th");
            IFLOG(trc(TH_ID, LS_CTR "%s(id:%d)", __func__, id))
        }

        virtual void *run() {
            IFLOG(trc(TH_ID, LS_OPN "%s(id:%d)", __func__, id_))
            IFLOG(inf(TH_ID, LS_APL"[peer running cycle on this thread]"))
            peer_.running_cycle();
            int stop_res = stop();
            IFLOG(trc(TH_ID, LS_CLO "%s(lifecyc:%d) - end, stop_res:%d -", __func__, id_, stop_res))
            return 0;
        }

    private:
        unsigned int id_;
        peer_automa &peer_;

    protected:
        static vlg::logger *log_;
};

vlg::logger *peer_automa_th::log_ = nullptr;


// VLG_PEER_LFCYC


vlg::logger *peer_automa::peer_log_ = nullptr;

void peer_automa::vlg_peer_param_clbk_ud(int pnum,
                                         const char *param,
                                         const char *value,
                                         void *ud)
{
    peer_automa *peer = static_cast<peer_automa *>(ud);
    RetCode res = peer->load_config_handler(pnum, param, value);
    if(res) {
        peer->peer_last_error_ = res;
    }
    IFLOG2(peer_automa::peer_log_, pln(" -%-20s %s", param, value ? value : ""))
}


// VLG_PEER_LFCYC CTORS - INIT - DESTROY


peer_automa::peer_automa(unsigned int peer_id) :
    peer_id_(peer_id),
    peer_plid_(0),
    peer_svid_(0),
    peer_status_(PeerStatus_ZERO),
    peer_argc_(0),
    peer_argv_(nullptr),
    configured_(false),
    peer_last_error_(vlg::RetCode_OK),
    peer_exit_required_(false),
    psc_hndl_(nullptr),
    psc_hndl_ud_(nullptr),
    force_disconnect_on_stop_(false)
{
    peer_log_ = vlg::logger::get_logger("peer_lifecyc");
    IFLOG2(peer_log_, trc(TH_ID, LS_CTR "%s", __func__))
    memset(&peer_name_[0], 0, sizeof(peer_name_));
    memset(&peer_ver_[0], 0, sizeof(peer_ver_));
    memset(&peer_cfg_file_path_name_[0], 0, sizeof(peer_cfg_file_path_name_));
}

peer_automa::~peer_automa()
{
    IFLOG2(peer_log_, trc(TH_ID, LS_DTR "%s(peer_id:%d)", __func__, peer_id_))
    if(!(peer_status_ <= PeerStatus_INITIALIZED) &&
            !(peer_status_ >= PeerStatus_STOPPED)) {
        IFLOG2(peer_log_, cri(TH_ID,
                              LS_DTR "%s(ptr:%p) - [peer is not in a safe state:%d] " LS_EXUNX, __func__,
                              this,
                              peer_status_))
    }
}

RetCode peer_automa::set_params_file_path_name(const char *file_path)
{
    strncpy(peer_cfg_file_path_name_,file_path, VLG_PEER_CFG_FILE_PATH_NAME_LEN);
    return vlg::RetCode_OK;
}

RetCode peer_automa::early_init()
{
    IFLOG2(peer_log_, trc(TH_ID, LS_OPN "%s(peer_id:%d)", __func__, peer_id_))
    if((peer_last_error_ = on_early_init())) {
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(peer_id:%d) [early init fail]", __func__, peer_id_))
        return peer_last_error_;
    }
    peer_status_ = PeerStatus_EARLY;
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[early init]"))
    IFLOG2(peer_log_, trc(TH_ID, LS_CLO "%s(peer_id:%d)", __func__, peer_id_))
    return vlg::RetCode_OK;
}

const char *peer_automa::get_name()
{
    return peer_name_;
}

const unsigned int *peer_automa::get_version()
{
    return peer_ver_;
}

unsigned int peer_automa::get_ver_major()
{
    return peer_ver_[0];
}

unsigned int peer_automa::get_ver_minor()
{
    return peer_ver_[1];
}

unsigned int peer_automa::get_ver_mant()
{
    return peer_ver_[2];
}

unsigned int peer_automa::get_ver_arch()
{
    return peer_ver_[3];
}


// GETTERS / SETTERS


bool peer_automa::is_configured()
{
    return configured_;
}

void peer_automa::set_configured(bool val)
{
    configured_ = val;
}


// STATUS


PeerStatus peer_automa::get_status()
{
    return peer_status_;
}

void peer_automa::set_status_change_handler(status_change
                                            hndlr, void *ud)
{
    psc_hndl_ = hndlr;
    psc_hndl_ud_ = ud;
}

RetCode peer_automa::set_status(PeerStatus peer_status)
{
    IFLOG2(peer_log_, dbg(TH_ID, LS_OPN "%s(peer_id:%d, status:%d)", __func__,
                          peer_id_, peer_status))
    CHK_CUST_MON_ERR(peer_mon_, lock)
    peer_status_ = peer_status;
    if(psc_hndl_) {
        psc_hndl_(*this, peer_status, psc_hndl_ud_);
    }
    CHK_CUST_MON_ERR(peer_mon_, notify_all)
    CHK_CUST_MON_ERR(peer_mon_, unlock)
    IFLOG2(peer_log_, dbg(TH_ID, LS_CLO "%s(peer_id:%d, status:%d)", __func__,
                          peer_id_, peer_status))
    return vlg::RetCode_OK;
}

RetCode peer_automa::await_for_status_reached_or_outdated(
    PeerStatus test,
    PeerStatus &current,
    time_t sec,
    long nsec)
{
    IFLOG2(peer_log_, trc(TH_ID, LS_OPN "%s(peer_id:%d, test:%d)", __func__, peer_id_, test))
    CHK_CUST_MON_ERR(peer_mon_, lock)
    if(peer_status_ < PeerStatus_INITIALIZED) {
        CHK_CUST_MON_ERR(peer_mon_, unlock)
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s", __func__))
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
    IFLOG2(peer_log_, dbg(TH_ID,
                          LS_CLO "%s(peer_id:%d, res:%d) - test:%d [reached or outdated] current:%d",
                          __func__,
                          peer_id_, rcode,
                          test,
                          peer_status_))
    CHK_CUST_MON_ERR(peer_mon_, unlock)
    return rcode;
}

RetCode peer_automa::await_for_condition(eval_condition cond_cllbk,
                                         time_t sec,
                                         long nsec)
{
    IFLOG2(peer_log_, trc(TH_ID, LS_OPN "%s(peer_id:%d, callback:%p)", __func__, peer_id_, cond_cllbk))
    CHK_CUST_MON_ERR(peer_mon_, lock)
    if(peer_status_ < PeerStatus_INITIALIZED) {
        CHK_CUST_MON_ERR(peer_mon_, unlock)
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(peer_id:%d, callback:%p)", __func__, peer_id_, cond_cllbk))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    while(!cond_cllbk(this)) {
        int pthres;
        if((pthres = peer_mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode = vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    CHK_CUST_MON_ERR(peer_mon_, unlock)
    IFLOG2(peer_log_, dbg(TH_ID,
                          LS_CLO "%s(peer_id:%d, res:%d) - [condition denoted by this callback:%p has been reached]",
                          __func__,
                          peer_id_, rcode, cond_cllbk))
    return rcode;
}

RetCode peer_automa::await_for_status_change(PeerStatus
                                             &peer_status,
                                             time_t sec,
                                             long nsec)
{
    IFLOG2(peer_log_, trc(TH_ID, LS_OPN "%s(peer_id:%d, peer_status:%d)", __func__, peer_id_, peer_status))
    CHK_CUST_MON_ERR(peer_mon_, lock)
    if(peer_status_ < PeerStatus_INITIALIZED) {
        CHK_CUST_MON_ERR(peer_mon_, unlock)
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s", __func__))
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
    peer_status = peer_status_;
    IFLOG2(peer_log_, dbg(TH_ID,
                          LS_CLO "%s(peer_id:%d, res:%d) - status:%d [changed] current:%d", __func__,
                          peer_id_,
                          rcode, peer_status,
                          peer_status_))
    CHK_CUST_MON_ERR(peer_mon_, unlock)
    return rcode;
}


// MACHINE STATE


//ACTIONS

RetCode peer_automa::start(int argc,
                           char *argv[],
                           bool spawn_new_thread)
{
    IFLOG2(peer_log_, trc(TH_ID, LS_OPN "%s(peer_id:%d)", __func__, peer_id_))
    if(peer_status_ != PeerStatus_EARLY && peer_status_ != PeerStatus_STOPPED) {
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(curstatus:%d)", __func__, peer_status_))
        return vlg::RetCode_BADSTTS;
    }
    peer_argc_ = argc;
    peer_argv_ = argv;
    RetCode handlers_res = vlg::RetCode_OK;
    peer_last_error_ = vlg::RetCode_OK;
    if(peer_status_ == PeerStatus_EARLY) {
        const char *peer_name = name_handler();
        if(!peer_name) {
            IFLOG2(peer_log_, fat(TH_ID, LS_APL"[early FAIL: invalid peer name]"))
            return vlg::RetCode_EXIT;
        }
        strncpy(peer_name_, peer_name, VLG_PEER_NAME_LEN-1);
        const unsigned int *peer_ver = version_handler();
        if(!peer_ver) {
            IFLOG2(peer_log_, fat(TH_ID, LS_APL"[early FAIL: invalid peer version]"))
            return vlg::RetCode_EXIT;
        }
        memcpy(peer_ver_, peer_ver, sizeof(peer_ver_));
        if((peer_last_error_ = step_welcome())) {
            IFLOG2(peer_log_, fat(TH_ID, LS_APL"[welcome FAIL with res:%d]", peer_last_error_))
            if((handlers_res = step_dying_breath())) {
                IFLOG2(peer_log_, cri(TH_ID, LS_APL"[dying breath FAIL with res:%d]", handlers_res))
            }
            return vlg::RetCode_EXIT;
        }
        if((peer_last_error_ = step_init())) {
            IFLOG2(peer_log_, fat(TH_ID, LS_APL"[init FAIL with res:%d]", peer_last_error_))
            if((handlers_res = step_dying_breath())) {
                IFLOG2(peer_log_, cri(TH_ID, LS_APL"[dying breath FAIL with res:%d]", handlers_res))
            }
            return vlg::RetCode_EXIT;
        }
    } else {
        set_status(PeerStatus_RESTART_REQUESTED);
    }
    if((peer_last_error_ = step_start())) {
        IFLOG2(peer_log_, fat(TH_ID, LS_APL"[start FAIL with res:%d]", peer_last_error_))
        if((handlers_res = step_dying_breath())) {
            IFLOG2(peer_log_, cri(TH_ID, LS_APL"[dying breath FAIL with res:%d]", handlers_res))
        }
        return vlg::RetCode_EXIT;
    }
    peer_automa_th *peer_thd = nullptr;
    if(spawn_new_thread) {
        IFLOG2(peer_log_, inf(TH_ID, LS_APL"[spawning new thread]"))
        peer_thd = new peer_automa_th(peer_id_, *this);
        peer_thd->start();
    } else {
        IFLOG2(peer_log_, inf(TH_ID, LS_APL"[peer automa on caller thread]"))
        peer_last_error_ = running_cycle();
    }
    IFLOG2(peer_log_, trc(TH_ID, LS_CLO "%s(peer_id:%d)", __func__, peer_id_))
    return vlg::RetCode_OK;
}

RetCode peer_automa::stop(bool force_disconnect)
{
    IFLOG2(peer_log_, trc(TH_ID, LS_OPN "%s(peer_id:%d)", __func__, peer_id_))
    if(peer_status_ != PeerStatus_RUNNING) {
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(curstatus:%d)", __func__, peer_status_))
        return vlg::RetCode_BADSTTS;
    }
    force_disconnect_on_stop_ = force_disconnect;
    RetCode rcode = set_stop_request();
    IFLOG2(peer_log_, trc(TH_ID, LS_CLO "%s(peer_id:%d, res:%d)", __func__, peer_id_, rcode))
    return rcode;
}

//LIFECYCL

RetCode peer_automa::running_cycle()
{
    IFLOG2(peer_log_, trc(TH_ID, LS_OPN "%s(peer_id:%d)", __func__, peer_id_))
    RetCode handlers_res = vlg::RetCode_OK;
    PeerStatus p_status = PeerStatus_ZERO;
    bool move_running = true;
    do {
        if(move_running) {
            if((peer_last_error_ = handlers_res = step_move_running())) {
                IFLOG2(peer_log_, cri(TH_ID, LS_APL"[move running FAIL with res:%d]", handlers_res))
                if((handlers_res = step_dying_breath())) {
                    IFLOG2(peer_log_, cri(TH_ID, LS_APL"[dying breath FAIL with res:%d]", handlers_res))
                }
                return vlg::RetCode_EXIT;
            }
            //here we are RUNNING
            p_status = PeerStatus_RUNNING;
            await_for_status_change(p_status);
            move_running = false;
        }
        switch(p_status) {
            case PeerStatus_STOP_REQUESTED:
                IFLOG2(peer_log_, inf(TH_ID, LS_APL"[peer requested to stop]"))
                if((peer_last_error_ = handlers_res = step_stop())) {
                    IFLOG2(peer_log_, cri(TH_ID, LS_APL"[stop FAIL with res:%d]", handlers_res))
                    if((handlers_res = step_dying_breath())) {
                        IFLOG2(peer_log_, cri(TH_ID, LS_APL"[dying breath FAIL with res:%d]", handlers_res))
                    }
                    return vlg::RetCode_EXIT;
                }
                break;
            case PeerStatus_ERROR:
                IFLOG2(peer_log_, inf(TH_ID, LS_APL"[calling error handler]"))
                if((peer_last_error_ = handlers_res = on_error())) {
                    IFLOG2(peer_log_, cri(TH_ID, LS_APL"[error handler FAIL with res:%d]", handlers_res))
                    if((handlers_res = step_dying_breath())) {
                        IFLOG2(peer_log_, cri(TH_ID, LS_APL"[dying breath FAIL with res:%d]", handlers_res))
                    }
                    return vlg::RetCode_EXIT;
                }
                if(peer_exit_required_) {
                    IFLOG2(peer_log_, fat(TH_ID, LS_APL"[ERROR HANDLER REQUIRED PEER TO EXIT]"))
                    return vlg::RetCode_EXIT;
                } else {
                    IFLOG2(peer_log_, wrn(TH_ID, LS_APL"[peer recovered - CHECK IS NEEDED]"))
                    move_running = true;
                }
                break;
            default:
                IFLOG2(peer_log_, fat(TH_ID, LS_APL"[PEER IN UNKNOWN STATUS]"))
                if((handlers_res = step_dying_breath())) {
                    IFLOG2(peer_log_, cri(TH_ID, LS_APL"[dying breath FAIL with res:%d]", handlers_res))
                }
                return vlg::RetCode_EXIT;
        }
    } while(get_status() != PeerStatus_STOPPED);
    IFLOG2(peer_log_, trc(TH_ID, LS_CLO "%s(peer_id:%d)", __func__, peer_id_))
    return vlg::RetCode_OK;
}

const char *peer_welcome_fmt = LS_WEL  "@%s %s" "@%s %u.%u.%u" "@%s %s";

RetCode peer_automa::step_welcome()
{
    if(peer_status_ > PeerStatus_EARLY) {
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(curstatus:%d)", __func__, peer_status_))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG2(peer_log_, inf(TH_ID, peer_welcome_fmt, "PEER:",
                          peer_name_,
                          "VER:",
                          peer_ver_[0],
                          peer_ver_[1],
                          peer_ver_[2],
                          "ARCH:",
                          vlg::get_arch()))
    set_status(PeerStatus_WELCOMED);
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[welcomed]"))
    return vlg::RetCode_OK;
}

RetCode peer_automa::step_init()
{
    if(peer_status_ > PeerStatus_INITIALIZED) {
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(curstatus:%d)", __func__, peer_status_))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[initializing]"))
    set_status(PeerStatus_INITIALIZING);
    if(!configured_) {
        bool params_chance = false;
        if(peer_argc_ < 2) {
            IFLOG2(peer_log_, wrn(TH_ID,
                                  LS_APL"[init][invalid argc:%d - peer will try to load params file]",
                                  peer_argc_))
            params_chance = true;
        }
        if(params_chance || !strcmp("-file", peer_argv_[1])) {
            if((peer_last_error_ = peer_conf_ldr_.init(strlen(peer_cfg_file_path_name_) ? peer_cfg_file_path_name_ : "params"))) {
                IFLOG2(peer_log_, err(TH_ID, LS_APL"[init FAIL][loading file params] errcode:%d", peer_last_error_))
                return vlg::RetCode_BADCFG;
            }
        } else {
            if((peer_last_error_ = peer_conf_ldr_.init(peer_argc_, peer_argv_))) {
                IFLOG2(peer_log_, err(TH_ID, LS_APL"[init FAIL][loading argv params] errcode:%d", peer_last_error_))
                return vlg::RetCode_BADCFG;
            }
        }
        if((peer_last_error_ = peer_conf_ldr_.load_config())) {
            IFLOG2(peer_log_, err(TH_ID, LS_APL"[init FAIL][reading params] errcode:%d", peer_last_error_))
            return vlg::RetCode_BADCFG;
        }
        peer_conf_ldr_.enum_params(vlg_peer_param_clbk_ud, this);
    } else {
        IFLOG2(peer_log_, inf(TH_ID, LS_APL"[init][this peer has already been configured]"))
    }
    if(peer_last_error_) {
        IFLOG2(peer_log_, err(TH_ID, LS_APL"[init FAIL][bad config]"))
        return vlg::RetCode_BADCFG;
    }
    if((peer_last_error_ = on_init())) {
        IFLOG2(peer_log_, err(TH_ID, LS_APL"[init FAIL][user handler]"))
        return peer_last_error_;
    }
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[init]"))
    set_status(PeerStatus_INITIALIZED);
    return vlg::RetCode_OK;
}

RetCode peer_automa::step_start()
{
    if(peer_status_ > PeerStatus_STARTED) {
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(curstatus:%d)", __func__, peer_status_))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[starting]"))
    set_status(PeerStatus_STARTING);
    if((peer_last_error_ = on_start())) {
        IFLOG2(peer_log_, err(TH_ID, LS_APL"[start FAIL]"))
        return peer_last_error_;
    }
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[started]"))
    set_status(PeerStatus_STARTED);
    return vlg::RetCode_OK;
}

RetCode peer_automa::step_move_running()
{
    if(peer_status_ != PeerStatus_STARTED) {
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(curstatus:%d)", __func__, peer_status_))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[move running]"))
    if((peer_last_error_ = on_move_running())) {
        IFLOG2(peer_log_, err(TH_ID, LS_APL"[move running FAIL]"))
        return peer_last_error_;
    }
    return set_running();
}

RetCode peer_automa::set_running()
{
    if(peer_status_ > PeerStatus_RUNNING) {
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(curstatus:%d)", __func__, peer_status_))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[RUNNING]"))
    set_status(PeerStatus_RUNNING);
    return vlg::RetCode_OK;
}

RetCode peer_automa::step_stop()
{
    if(peer_status_ > PeerStatus_STOPPED) {
        IFLOG2(peer_log_, err(TH_ID, LS_CLO "%s(curstatus:%d)", __func__, peer_status_))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[stopping]"))
    set_status(PeerStatus_STOPPING);
    if((peer_last_error_ = on_stop())) {
        IFLOG2(peer_log_, err(TH_ID, LS_APL"[stop FAIL]"))
        return peer_last_error_;
    }
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[stopped]"))
    set_status(PeerStatus_STOPPED);
    return vlg::RetCode_OK;
}

RetCode peer_automa::set_stop_request()
{
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[stop request]"))
    set_status(PeerStatus_STOP_REQUESTED);
    return vlg::RetCode_OK;
}

RetCode peer_automa::set_error()
{
    IFLOG2(peer_log_, err(TH_ID, LS_APL"[ERROR]"))
    set_status(PeerStatus_ERROR);
    return vlg::RetCode_OK;
}

RetCode peer_automa::step_dying_breath()
{
    IFLOG2(peer_log_, inf(TH_ID, LS_APL"[DYING BREATH]"))
    RetCode rcode = vlg::RetCode_OK;
    if((rcode = on_dying_breath()) == vlg::RetCode_OK) {
        IFLOG2(peer_log_, inf(TH_ID, LS_APL"[dying breath emitted]"))
    } else {
        IFLOG2(peer_log_, cri(TH_ID, LS_APL"[dying breath emitted][%d]", peer_last_error_))
    }
    rcode = set_status(PeerStatus_DIED);
    if(rcode) {
        IFLOG2(peer_log_, cri(TH_ID, LS_APL"[dying breath FAIL with res:%d]", rcode))
    }
    return rcode;
}

}
