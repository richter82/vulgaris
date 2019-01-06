/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "glob.h"

namespace vlg {

/** @brief broker_automa
*/
struct broker_automa {
        explicit broker_automa(broker &, broker_listener &);
        virtual ~broker_automa();

        RetCode set_params_file_path_name(const char *);

        RetCode early_init();
        RetCode start(int argc,
                      char *argv[],
                      bool spawn_new_thread);

        RetCode stop(bool force_disconnect = false);

        RetCode await_for_status_reached(BrokerStatus test,
                                         BrokerStatus &current,
                                         time_t sec = -1,
                                         long nsec = 0);

        RetCode await_for_status_change(BrokerStatus &broker_status,
                                        time_t sec = -1,
                                        long nsec = 0);

        RetCode set_status(BrokerStatus);
        RetCode set_running();
        RetCode set_stop_request();

        RetCode running_cycle();

    private:
        RetCode step_welcome();
        RetCode step_init();
        RetCode step_start();
        RetCode step_stop();
        RetCode step_move_running();
        void step_error();

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
        virtual void on_automa_error() = 0;

    public:
        unsigned int broker_id_;
        unsigned int broker_plid_;
        unsigned int broker_svid_;
        BrokerStatus broker_status_;
        std::string broker_name_;
        unsigned int broker_ver_[4];
        int broker_argc_;
        char **broker_argv_;
        std::string broker_cfg_file_path_name_;

        cfg_ldr broker_conf_ldr_;
        bool configured_;
        RetCode broker_last_error_;

        // stop / dispose
        bool force_disconnect_on_stop_;

        mutable std::mutex broker_mtx_;
        mutable std::condition_variable broker_cv_;

    private:
        static void broker_param_clbk_ud(int pnum,
                                         const char *param,
                                         const char *value,
                                         void *ud);
    public:
        broker &publ_;
        broker_listener &listener_;
        std::shared_ptr<spdlog::logger> log_;
};


}
