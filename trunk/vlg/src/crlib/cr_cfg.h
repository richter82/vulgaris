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

#ifndef CR_CFG_H_
#define CR_CFG_H_
#include "blaze_ascii_string.h"

namespace blaze {

/** @brief config_loader class.

*/
class config_loader_rep;
class config_loader {

    public:
        /**
        @param pnum
        @param param
        @param value
        @return
        */
        typedef void(*param_callback)(int pnum,
                                      const char *param,
                                      const char *value);

        /**
        @param pnum
        @param param
        @param value
        @param ud
        @return
        */
        typedef void(*param_callback_ud)(int pnum,
                                         const char *param,
                                         const char *value,
                                         void *ud);

    public:
        explicit config_loader();
        ~config_loader();

        RetCode set_params_file_dir(const char *dir);
        RetCode init();
        RetCode init(int argc,
                     char *argv[]);
        RetCode init(const char *file_name);
        RetCode destroy();

        RetCode load_config();
        void dump_config();
        void dump_config(FILE *fd);
        void enum_params(param_callback usr_clbk);
        void enum_params(param_callback_ud usr_clbk_ud,
                         void *ud);

    private:
        config_loader_rep *impl_;
};

}

#endif
