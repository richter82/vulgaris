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

#pragma once
#include "glob.h"

namespace vlg {

/** @brief peer_automa
*/
struct peer_automa {
        explicit peer_automa(peer &);
        virtual ~peer_automa();

        RetCode set_params_file_path_name(const char *);

        RetCode early_init();
        RetCode start(int argc,
                      char *argv[],
                      bool spawn_new_thread);

        RetCode stop(bool force_disconnect = false);

        RetCode await_for_status_reached_or_outdated(PeerStatus test,
                                                     PeerStatus &current,
                                                     time_t sec = -1,
                                                     long nsec = 0);

        RetCode await_for_status_change(PeerStatus &peer_status,
                                        time_t sec = -1,
                                        long nsec = 0);

        RetCode set_status(PeerStatus);
        RetCode set_running();
        RetCode set_stop_request();
        RetCode set_error();

        RetCode running_cycle();

    private:
        RetCode step_welcome();
        RetCode step_init();
        RetCode step_start();
        RetCode step_stop();
        RetCode step_move_running();
        void step_dying_breath();

        virtual const char *get_automa_name() = 0;
        virtual const unsigned int *get_automa_version() = 0;

        virtual RetCode on_automa_load_config(int pnum,
                                              const char *param,
                                              const char *value) = 0;

        virtual RetCode on_automa_early_init() = 0;
        virtual RetCode on_automa_init() = 0;
        virtual RetCode on_automa_start() = 0;
        virtual RetCode on_automa_stop() = 0;
        virtual RetCode on_automa_move_running() = 0;
        virtual RetCode on_automa_error() = 0;
        virtual void on_automa_dying_breath() = 0;

    public:
        unsigned int peer_id_;
        unsigned int peer_plid_;
        unsigned int peer_svid_;
        PeerStatus peer_status_;
        std::string peer_name_;
        unsigned int peer_ver_[4];
        int peer_argc_;
        char **peer_argv_;
        std::string peer_cfg_file_path_name_;

        cfg_ldr peer_conf_ldr_;
        bool configured_;
        RetCode peer_last_error_;
        bool peer_exit_required_;

        // stop / dispose
        bool force_disconnect_on_stop_;
        mutable mx peer_mon_;

    private:
        static void peer_param_clbk_ud(int pnum,
                                       const char *param,
                                       const char *value,
                                       void *ud);
    public:
        peer &publ_;
};


}
