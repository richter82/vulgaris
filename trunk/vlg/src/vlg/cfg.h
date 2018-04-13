/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "vlg.h"
#include <list>

namespace vlg {

struct param_arg_pair {
    char *param;
    char *arg;
};

/** @brief config_loader class.
*/
struct cfg_ldr {
        typedef void(*param_callback)(int pnum,
                                      const char *param,
                                      const char *value);

        typedef void(*param_callback_ud)(int pnum,
                                         const char *param,
                                         const char *value,
                                         void *ud);

        explicit cfg_ldr();

        RetCode set_params_file_dir(const char *dir);
        RetCode init();
        RetCode init(int argc, char *argv[]);
        RetCode init(const char *file_name);
        RetCode load_config();
        void dump_config();
        void dump_config(FILE *fd);
        void enum_params(param_callback usr_clbk);
        void enum_params(param_callback_ud usr_clbk_ud, void *ud);

    private:
        RetCode r_load_data(const char *filename);

    private:
        std::string params_cfg_file_dir_;
        std::list<param_arg_pair> lpap_;
        std::string data_;
};

}
