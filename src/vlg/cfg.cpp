/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "glob.h"

namespace vlg {

RetCode read_par(str_tok &tknz, param_arg_pair &pap)
{
    std::string tkn, par;
    while(tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        //we expect a par name, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, return RetCode_KO)
        par.assign(tkn);
        // ok we got par name.
        break;
    }
    pap.param = strdup(par.c_str());
    return RetCode_OK;
}

RetCode read_arg(str_tok &tknz, param_arg_pair &pap)
{
    bool null_str_arg = true;
    std::string tkn, arg;
    while(tknz.next_token(tkn, CR_TK_LF CR_TK_RC CR_TK_FF CR_TK_QT, true)) {
        //we expect an arg on 1 line, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, return RetCode_KO)
        if(tkn == CR_TK_QT) {
            // ok we have read final quote.
            break;
        } else {
            arg.assign(tkn);
            null_str_arg = false;
            // ok we got arg.
        }
    }
    if(!null_str_arg) {
        pap.arg = strdup(arg.c_str());
    }
    return RetCode_OK;
}

// config_loader
cfg_ldr::cfg_ldr()
{}

RetCode cfg_ldr::r_load_data(const char *filename)
{
    std::string path;
    path.assign(params_cfg_file_dir_);
    if(path.length() > 0) {
        path.append(CR_FS_SEP);
    }
    path.append(filename);
    FILE *fp = fopen(path.c_str(), "r");
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

RetCode cfg_ldr::init()
{
    return r_load_data("params");
}

RetCode cfg_ldr::init(int argc, char *argv[])
{
    for(int i = 1; i < argc; i++) {
        data_.append(" ");
        data_.append(argv[i]);
    }
    return RetCode_OK;
}

RetCode cfg_ldr::init(const char *fname)
{
    return r_load_data(fname);
}

RetCode cfg_ldr::set_params_file_dir(const char *dir)
{
    params_cfg_file_dir_ = dir;
    return RetCode_OK;
}

RetCode cfg_ldr::load_config()
{
    bool par_read = false;
    str_tok tknz(data_);
    std::string tkn;
    param_arg_pair pap = { 0 };
    while(tknz.next_token(tkn, " \n\r\t\"-", true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, continue)
        if(tkn == "-") {
            if(par_read) {
                lpap_.push_back(pap);
                memset(&pap, 0, sizeof(pap));
            }
            read_par(tknz, pap);
            par_read = true;
            continue;
        } else if(tkn == "\"") {
            if(par_read) {
                read_arg(tknz, pap);
                lpap_.push_back(pap);
                memset(&pap, 0, sizeof(pap));
                par_read = false;
            } else {
                //argument without parameter
                return RetCode_BADARG;
            }
        } else {
            if(par_read) {
                pap.arg = strdup(tkn.c_str());
                lpap_.push_back(pap);
                memset(&pap, 0, sizeof(pap));
                par_read = false;
            } else {
                //argument without parameter
                return RetCode_BADARG;
            }
        }
    }
    if(par_read) {
        lpap_.push_back(pap);
    }
    return RetCode_OK;
}

void cfg_ldr::dump_config()
{
    dump_config(stdout);
}

void cfg_ldr::dump_config(FILE *fd)
{
    std::for_each(lpap_.begin(), lpap_.end(), [&](param_arg_pair &pap) {
        fprintf(fd, "-%-20s %s\n", pap.param, pap.arg ? pap.arg : "");
    });
    fprintf(fd, "----------------------------------------------------\n\n");
}

void cfg_ldr::enum_params(param_callback usr_clbk)
{
    int i = 1;
    std::for_each(lpap_.begin(), lpap_.end(), [&](param_arg_pair &pap) {
        usr_clbk(i++, pap.param, pap.arg);
    });
}

void cfg_ldr::enum_params(param_callback_ud usr_clbk_ud, void *ud)
{
    int i = 1;
    std::for_each(lpap_.begin(), lpap_.end(), [&](param_arg_pair &pap) {
        usr_clbk_ud(i++, pap.param, pap.arg, ud);
    });
}

}
