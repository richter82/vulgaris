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

#include "vlg_globint.h"

namespace vlg {

#define PARAMS_CFG_FILE_DIR_LEN 1024

struct param_arg_pair {
    char *param;
    char *arg;
};

RetCode read_par(ascii_string_tok &tknz, param_arg_pair &pap)
{
    ascii_string tkn, par;
    while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        //we expect a par name, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, return RetCode_KO)
        RETURN_IF_NOT_OK(par.assign(tkn))
        // ok we got par name.
        break;
    }
    pap.param = par.new_buffer();
    return RetCode_OK;
}

RetCode read_arg(ascii_string_tok &tknz, param_arg_pair &pap)
{
    bool null_str_arg = true;
    ascii_string tkn, arg;
    while(!tknz.next_token(tkn, CR_TK_LF CR_TK_RC CR_TK_FF CR_TK_QT, true)) {
        //we expect an arg on 1 line, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, return RetCode_KO)
        if(tkn == CR_TK_QT) {
            // ok we have read final quote.
            break;
        } else {
            RETURN_IF_NOT_OK(arg.assign(tkn))
            null_str_arg = false;
            // ok we got arg.
        }
    }
    if(!null_str_arg) {
        pap.arg = arg.new_buffer();
    }
    return RetCode_OK;
}

//-----------------------------
// config_loader_rep
class config_loader_rep {
    public:
        config_loader_rep() : lpap_(sizeof(param_arg_pair)), data_() {
            memset(params_cfg_file_dir_, 0, sizeof(params_cfg_file_dir_));
        }

        RetCode r_init(int argc, char *argv[]) {
            RETURN_IF_NOT_OK(data_.assign(""))
            RETURN_IF_NOT_OK(lpap_.init())
            for(int i = 1; i < argc; i++) {
                RETURN_IF_NOT_OK(data_.append(" "))
                RETURN_IF_NOT_OK(data_.append(argv[i]))
            }
            return RetCode_OK;
        }

        RetCode r_init(const char *filename) {
            RETURN_IF_NOT_OK(data_.assign(""))
            RETURN_IF_NOT_OK(lpap_.init())
            return r_load_data(filename);
        }

        RetCode r_load_data(const char *filename) {
            ascii_string path;
            path.assign(params_cfg_file_dir_);
            if(path.length() > 0) {
                path.append(CR_FS_SEP);
            }
            path.append(filename);
            FILE *fp = fopen(path.internal_buff(), "r");
            char line_buf[CR_MAX_SRC_LINE_LEN + 2];
            if(fp) {
                while(!feof(fp)) {
                    if(fgets(line_buf, CR_MAX_SRC_LINE_LEN + 2, fp)) {
                        if(is_blank_line(line_buf)) {
                            continue;
                        } else {
                            data_.append(line_buf);
                        }
                    }
                }
                fclose(fp);
            } else {
                return RetCode_IOERR;
            }
            return RetCode_OK;
        }

        RetCode r_load_cfg() {
            bool par_read = false;
            ascii_string_tok tknz;
            RETURN_IF_NOT_OK(tknz.init(data_))
            ascii_string tkn;
            param_arg_pair pap = { 0 };
            while(!tknz.next_token(tkn, " \n\r\t\"-", true)) {
                CR_SKIP_SP_TABS(tkn)
                CR_DO_CMD_ON_NEWLINE(tkn, continue)
                if(tkn == "-") {
                    if(par_read) {
                        RETURN_IF_NOT_OK(lpap_.push_back(&pap))
                        memset(&pap, 0, sizeof(pap));
                    }
                    RETURN_IF_NOT_OK(read_par(tknz, pap))
                    par_read = true;
                    continue;
                } else if(tkn == "\"") {
                    if(par_read) {
                        RETURN_IF_NOT_OK(read_arg(tknz, pap))
                        RETURN_IF_NOT_OK(lpap_.push_back(&pap))
                        memset(&pap, 0, sizeof(pap));
                        par_read = false;
                    } else {
                        //argument without parameter
                        return RetCode_BADARG;
                    }
                } else {
                    if(par_read) {
                        pap.arg = tkn.new_buffer();
                        RETURN_IF_NOT_OK(lpap_.push_back(&pap))
                        memset(&pap, 0, sizeof(pap));
                        par_read = false;
                    } else {
                        //argument without parameter
                        return RetCode_BADARG;
                    }
                }
            }
            if(par_read) {
                RETURN_IF_NOT_OK(lpap_.push_back(&pap))
            }
            return RetCode_OK;
        }

        char params_cfg_file_dir_[PARAMS_CFG_FILE_DIR_LEN];
        linked_list lpap_;
        ascii_string data_;
};

//-----------------------------
// config_loader
config_loader::config_loader() : impl_(NULL)
{}

RetCode config_loader::init()
{
    return init("params");
}

RetCode config_loader::init(int argc, char *argv[])
{
    if(!(impl_ = new config_loader_rep())) {
        return RetCode_MEMERR;
    }
    return impl_->r_init(argc, argv);
}

RetCode config_loader::init(const char *fname)
{
    if(!(impl_ = new config_loader_rep())) {
        return RetCode_MEMERR;
    }
    return impl_->r_init(fname);
}

RetCode config_loader::destroy()
{
    if(impl_) {
        delete impl_;
    }
    return RetCode_OK;
}

config_loader::~config_loader() {}

RetCode config_loader::set_params_file_dir(const char *dir)
{
    strncpy(impl_->params_cfg_file_dir_, dir, PARAMS_CFG_FILE_DIR_LEN);
    return RetCode_OK;
}

RetCode config_loader::load_config()
{
    return impl_->r_load_cfg();
}

void config_loader::dump_config()
{
    dump_config(stdout);
}

void config_loader::dump_config(FILE *fd)
{
    impl_->lpap_.start_iteration();
    param_arg_pair pap;
    while(!impl_->lpap_.next(&pap)) {
        fprintf(fd, "-%-20s %s\n", pap.param, pap.arg ? pap.arg : "");
    }
    fprintf(fd, "----------------------------------------------------\n\n");
}

void config_loader::enum_params(param_callback usr_clbk)
{
    int i = 1;
    impl_->lpap_.start_iteration();
    param_arg_pair pap;
    while(!impl_->lpap_.next(&pap)) {
        usr_clbk(i++, pap.param, pap.arg);
    }
}

void config_loader::enum_params(param_callback_ud usr_clbk_ud, void *ud)
{
    int i = 1;
    impl_->lpap_.start_iteration();
    param_arg_pair pap;
    while(!impl_->lpap_.next(&pap)) {
        usr_clbk_ud(i++, pap.param, pap.arg, ud);
    }
}

}
