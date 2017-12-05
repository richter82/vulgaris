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

#ifndef VLG_PEER_AUTOMA_H_
#define VLG_PEER_AUTOMA_H_
#include "vlg_logger.h"
#include "vlg_globint.h"

#define VLG_PEER_NAME_LEN 256
#define VLG_PEER_CFG_FILE_PATH_NAME_LEN 512

namespace vlg {

/** @brief peer_automa class.
*/
class peer_automa {
        friend class peer_automa_th;

        typedef void (*status_change)(peer_automa &peer,
                                      PeerStatus status,
                                      void *ud);

        typedef bool (*eval_condition)(peer_automa *peer);

    public:
        peer_automa(unsigned int peer_id);
        virtual ~peer_automa();

        RetCode set_params_file_path_name(const char *file_path);

        // GETTERS
    public:
        const char         *get_name();
        const unsigned int *get_version();
        unsigned int        get_ver_major();
        unsigned int        get_ver_minor();
        unsigned int        get_ver_mant();
        unsigned int        get_ver_arch();
        bool                is_configured();

        // SETTERS
    public:
        void                set_configured(bool configured);

        // ACTIONS
    public:
        RetCode    early_init();

        RetCode    start(int argc,
                         char *argv[],
                         bool spawn_new_thread);

        RetCode    stop(bool force_disconnect = false);

        // ASYNCHRO HNDLRS
    public:
        void set_status_change_handler(status_change hndlr,
                                       void *ud);

        // SYNCHRO
    public:
        RetCode await_for_status_reached_or_outdated(PeerStatus test,
                                                     PeerStatus &current,
                                                     time_t sec = -1,
                                                     long nsec = 0);

        RetCode await_for_condition(eval_condition cond_cllbk,
                                    time_t sec = -1,
                                    long nsec = 0);

        RetCode await_for_status_change(PeerStatus &peer_status,
                                        time_t sec = -1,
                                        long nsec = 0);

        // STATUS
    public:
        PeerStatus  get_status();
    protected:
        RetCode     set_status(PeerStatus peer_status);
        RetCode     set_running();
        RetCode     set_stop_request();
        RetCode     set_error();

        // LIFECYCLE
    private:
        RetCode    running_cycle();
        RetCode    step_welcome();
        RetCode    step_init();
        RetCode    step_start();
        RetCode    step_stop();
        RetCode    step_move_running();
        RetCode    step_dying_breath();

        // LIFECYCLE handlers
        virtual const char          *name_handler()      = 0;
        virtual const unsigned int  *version_handler()   = 0;

        virtual RetCode             load_config_handler(int pnum,
                                                        const char *param,
                                                        const char *value)  = 0;
    public:
        virtual RetCode    on_early_init()      = 0;
        virtual RetCode    on_init()            = 0;
        virtual RetCode    on_start()           = 0;
        virtual RetCode    on_stop()            = 0;
        virtual RetCode    on_move_running()    = 0;
        virtual RetCode    on_error()           = 0;
        virtual RetCode    on_dying_breath()    = 0;

    protected:
        unsigned int    peer_id_;
        unsigned int    peer_plid_;
        unsigned int    peer_svid_;
        PeerStatus      peer_status_;
        char            peer_name_[VLG_PEER_NAME_LEN];
        unsigned int    peer_ver_[4];
        int             peer_argc_;
        char            **peer_argv_;
        char            peer_cfg_file_path_name_[VLG_PEER_CFG_FILE_PATH_NAME_LEN];

        vlg::config_loader  peer_conf_ldr_;
        bool                configured_;
        RetCode             peer_last_error_;
        bool                peer_exit_required_;

        //--asynch status
        status_change   psc_hndl_;
        void            *psc_hndl_ud_;

        // stop / dispose
        bool                        force_disconnect_on_stop_;
        mutable vlg::synch_monitor  peer_mon_;

    private:
        static void vlg_peer_param_clbk_ud(int pnum,
                                           const char *param,
                                           const char *value,
                                           void *ud);
    public:
        static vlg::logger     *peer_log_;
};


}

#endif
