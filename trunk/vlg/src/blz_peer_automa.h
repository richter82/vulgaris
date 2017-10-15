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

#ifndef BLZ_PEER_AUTOMA_H_
#define BLZ_PEER_AUTOMA_H_
#include "blaze_logger.h"
#include "blz_glob_int.h"

#define BLZ_PEER_NAME_LEN 256
#define BLZ_PEER_CFG_FILE_PATH_NAME_LEN 512

namespace blaze {

//-----------------------------
// peer_automa
//-----------------------------
class peer_automa {
        friend class peer_automa_th;

        typedef void (*peer_lfcyc_status_change_hndlr)(peer_automa &peer,
                                                       PeerStatus status,
                                                       void *ud);

        typedef bool (*peer_lfcyc_eval_condition)(peer_automa *peer);

    public:
        //---ctors
        peer_automa(unsigned int peer_id);
        virtual ~peer_automa();

        blaze::RetCode set_params_file_path_name(const char *file_path);

        //-----------------------------
        // GETTERS
        //-----------------------------
    public:
        const char         *peer_name();
        const unsigned int *peer_ver();
        unsigned int        peer_ver_major();
        unsigned int        peer_ver_minor();
        unsigned int        peer_ver_mant();
        unsigned int        peer_ver_arch();
        bool                is_configured();

        //-----------------------------
        // SETTERS
        //-----------------------------
    public:
        void                set_configured(bool configured);

        //-----------------------------
        // ACTIONS
        //-----------------------------
    public:
        blaze::RetCode    early_init();

        blaze::RetCode    start_peer(int argc,
                                     char *argv[],
                                     bool spawn_new_thread);

        blaze::RetCode    stop_peer(bool force_disconnect = false);

        //-----------------------------
        // ASYNCHRO HNDLRS
        //-----------------------------
    public:
        void set_peer_status_change_hndlr(peer_lfcyc_status_change_hndlr hndlr,
                                          void *ud);

        //-----------------------------
        // SYNCHRO
        //-----------------------------
    public:
        blaze::RetCode await_for_peer_status_reached_or_outdated(PeerStatus test,
                                                                 PeerStatus &current,
                                                                 time_t sec = -1,
                                                                 long nsec = 0);

        blaze::RetCode await_for_peer_status_condition(peer_lfcyc_eval_condition
                                                       cond_cllbk,
                                                       time_t sec = -1,
                                                       long nsec = 0);

        blaze::RetCode await_for_peer_status_change(PeerStatus &peer_status,
                                                    time_t sec = -1,
                                                    long nsec = 0);

        //-----------------------------
        // STATUS
        //-----------------------------
    public:
        PeerStatus   peer_status();
    protected:
        blaze::RetCode    set_peer_status(PeerStatus peer_status);

        blaze::RetCode    set_peer_running();
        blaze::RetCode    set_peer_stop_request();
        blaze::RetCode    set_peer_error();

        //-----------------------------
        // LIFECYCLE
        //-----------------------------
    private:
        blaze::RetCode    peer_life_cycle();
        blaze::RetCode    peer_welcome();
        blaze::RetCode    peer_init();
        blaze::RetCode    peer_start();
        blaze::RetCode    peer_stop();
        blaze::RetCode    peer_move_running();
        blaze::RetCode    peer_dying_breath();

        //-----------------------------
        // LIFECYCLE - Usr subclass entrypoints
        //-----------------------------
        virtual const char             *peer_name_usr();
        virtual const unsigned int     *peer_ver_usr();
        virtual blaze::RetCode          peer_load_cfg_usr(int pnum,
                                                          const char *param,
                                                          const char *value);

        //-----------------------------
        // LIFECYCLE - Usr opt. subclass entrypoints
        //-----------------------------
    public:
        virtual blaze::RetCode    peer_early_init_usr();
        virtual blaze::RetCode    peer_init_usr();
        virtual blaze::RetCode    peer_start_usr();
        virtual blaze::RetCode    peer_stop_usr();
        virtual blaze::RetCode    peer_move_running_usr();

        //-----------------------------
        // LIFECYCLE - Usr opt. subclass handlers
        //-----------------------------
        virtual blaze::RetCode    peer_error_handler();
        virtual blaze::RetCode    peer_dying_breath_handler();

    protected:
        unsigned int    peer_id_;
        unsigned int    peer_plid_;
        unsigned int    peer_svid_;
        PeerStatus      peer_status_;
        char            peer_name_[BLZ_PEER_NAME_LEN];
        unsigned int    peer_ver_[4];
        int         peer_argc_;
        char        **peer_argv_;
        char        peer_cfg_file_path_name_[BLZ_PEER_CFG_FILE_PATH_NAME_LEN];

        blaze::config_loader    peer_conf_ldr_;
        bool                    configured_;
        blaze::RetCode          peer_last_error_;
        bool                    peer_exit_required_;

        //--synch status
        peer_lfcyc_status_change_hndlr  psc_hndl_;
        void                            *psc_hndl_ud_;

        // stop / dispose
        bool                            force_disconnect_on_stop_;
        mutable blaze::synch_monitor    peer_mon_;

    private:
        static void blz_peer_param_clbk_ud(int pnum, const char *param,
                                           const char *value, void *ud);
    public:
        static blaze::logger     *peer_log_;
};


}

#endif
