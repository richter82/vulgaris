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

#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#include <stdarg.h>
#endif

#include "blaze_logger.h"
#include "blaze_model.h"
#include "blz_glob_int.h"

#define LG_BUF_LEN_16K 16384

#define TL_LOW_STR "LOW"
#define TL_TRC_STR "TRC"
#define TL_DBG_STR "DBG"
#define TL_INF_STR "INF"
#define TL_WRN_STR "WRN"
#define TL_ERR_STR "ERR"
#define TL_CRI_STR "CRI"
#define TL_FAT_STR "FAT"

#define LS_MSEP     ":"
#define LG_INIT_ERR_TK      "LIE"
#define TL_LVL_STR_LEN      3
#define LG_TIME_BUF_LEN     12
#define LG_MAX_APNDS        32
#define LG_MAX_SIGN_LEN     8
#define LG_ID_LEN           8

#define TKLGR   "logger"
#define TKAPND  "appender"
#define S_NL    "{New Line}"
#define S_APNM  "{apname}"
#define S_LGNM  "{lgname}"
#define S_LGSG  "{lgsign}"
#define S_LF    "[Line Flush]"

namespace vlg {

#define LOG_CFG_FILE_DIR_LEN 512
static char log_cfg_file_dir[LOG_CFG_FILE_DIR_LEN] = {0};

#define LOG_CFG_FILE_NAME_PATH_LEN 512
static char log_cfg_file_name_path[LOG_CFG_FILE_NAME_PATH_LEN] = {0};

TraceLVL get_trace_level_enum(const char *str)
{
    if(!strcmp(str, TL_LOW_STR)) {
        return TL_LOW;
    } else if(!strcmp(str, TL_TRC_STR)) {
        return TL_TRC;
    } else if(!strcmp(str, TL_DBG_STR)) {
        return TL_DBG;
    } else if(!strcmp(str, TL_INF_STR)) {
        return TL_INF;
    } else if(!strcmp(str, TL_WRN_STR)) {
        return TL_WRN;
    } else if(!strcmp(str, TL_ERR_STR)) {
        return TL_ERR;
    } else if(!strcmp(str, TL_CRI_STR)) {
        return TL_CRI;
    } else if(!strcmp(str, TL_FAT_STR)) {
        return TL_FAT;
    } else {
        return TL_EVL;
    }
}

const char *get_trace_level_string(TraceLVL lvl)
{
    switch(lvl) {
        case TL_PLN:
            return "";
        case TL_LOW:
            return TL_LOW_STR;
        case TL_TRC:
            return TL_TRC_STR;
        case TL_DBG:
            return TL_DBG_STR;
        case TL_INF:
            return TL_INF_STR;
        case TL_WRN:
            return TL_WRN_STR;
        case TL_ERR:
            return TL_ERR_STR;
        case TL_CRI:
            return TL_CRI_STR;
        case TL_FAT:
            return TL_FAT_STR;
        default:
            return TL_DBG_STR;
    }
}

//-----------------------------
// logger_cfg_loader
//-----------------------------
class logger_cfg_loader {
    public:
        virtual ~logger_cfg_loader() {};
        virtual RetCode load_cfg() = 0;
};

void apnd_dealloc_func(void *ptr)
{
    appender *a_ptr = *(appender **)ptr;
    delete a_ptr;
    free(ptr);
}

//-----------------------------
// appender_obj_mng
//-----------------------------

void *def_alloc_func(size_t type_size, const void *copy);

class appender_obj_mng : public obj_mng {
    public:
        appender_obj_mng() :  obj_mng(sizeof(void *),
                                          def_alloc_func,
                                          apnd_dealloc_func,
                                          memcmp,
                                          memcpy,
                                          MurmurHash3_x86_32) {}

        static appender_obj_mng *instance_;
};

appender_obj_mng *appender_obj_mng::instance_ = NULL;
appender_obj_mng &sngl_apnd_obj_mng()
{
    if(appender_obj_mng::instance_  == NULL) {
        appender_obj_mng::instance_  = new appender_obj_mng();
        if(!appender_obj_mng::instance_) {
            EXIT_ACTION("failed creating sngl_apnd_obj_mng instance\n")
        }
    }
    return *appender_obj_mng::instance_ ;
}

void lggr_dealloc_func(void *ptr)
{
    logger *l_ptr = *(logger **)ptr;
    delete l_ptr;
    free(ptr);
}

class logger_obj_mng : public obj_mng {
    public:
        logger_obj_mng() : obj_mng(sizeof(void *),
                                       def_alloc_func,
                                       lggr_dealloc_func,
                                       memcmp,
                                       memcpy,
                                       MurmurHash3_x86_32) {}

        static logger_obj_mng *instance_;
};

logger_obj_mng *logger_obj_mng::instance_ = NULL;
logger_obj_mng &sngl_logger_obj_mng()
{
    if(logger_obj_mng::instance_  == NULL) {
        logger_obj_mng::instance_  = new logger_obj_mng();
        if(!logger_obj_mng::instance_) {
            EXIT_ACTION("sngl_logger_obj_mng() - failed create logger_obj_mng instance\n")
        }
    }
    return *logger_obj_mng::instance_ ;
}

bool deflt_log_loaded = false;
hash_map *lgname_lg = NULL;
hash_map &get_logger_map()
{
    if(lgname_lg) {
        return *lgname_lg;
    }
    if(!(lgname_lg = new hash_map(sngl_logger_obj_mng(),
                                  sngl_cstr_obj_mng()))) {
        EXIT_ACTION("get_logger_map() - failed create logger map\n")
    }
    if(lgname_lg->init(HM_SIZE_NANO)) {
        EXIT_ACTION("get_logger_map() - failed init logger map\n")
    }
    return *lgname_lg;
}

RetCode logger::add_appender_to_all_loggers(appender *apnd)
{
    RetCode res = RetCode_OK;
    logger *logger = NULL;
    get_logger_map().start_iteration();
    while(!get_logger_map().next(NULL, &logger)) {
        if((res = logger->add_appender(apnd))) {
            return res;
        }
    }
    return RetCode_OK;
}

void logger::set_level_for_all_loggers(TraceLVL lvl)
{
    logger *logger = NULL;
    get_logger_map().start_iteration();
    while(!get_logger_map().next(NULL, &logger)) {
        logger->set_level(lvl);
    }
}

RetCode logger::remove_last_appender_from_all_loggers()
{
    RetCode res = RetCode_OK;
    logger *logger = NULL;
    get_logger_map().start_iteration();
    while(!get_logger_map().next(NULL, &logger)) {
        if((res = logger->remove_last_appender())) {
            return res;
        }
    }
    return RetCode_OK;
}

void LPrsERR_FND_EXP(const char *sct, const char *fnd, const char *exp)
{
    FILE *ferr = fopen("log.err", "wa+");
    fprintf(ferr ? ferr : stderr,
            LG_INIT_ERR_TK" parsing: %s, found: %s, expected: %s\n", sct, fnd, exp);
    if(ferr) {
        fclose(ferr);
    }
}

void LPrsERR_EXP(const char *sct, const char *exp)
{
    FILE *ferr = fopen("log.err", "wa+");
    fprintf(ferr ? ferr : stderr, LG_INIT_ERR_TK" parsing: %s, expected: %s\n", sct,
            exp);
    if(ferr) {
        fclose(ferr);
    }
}

void LPrsERR_UNEXP(const char *sct, const char *unexp)
{
    FILE *ferr = fopen("log.err", "wa+");
    fprintf(ferr ? ferr : stderr, LG_INIT_ERR_TK" parsing: %s, unexpct. tkn: %s\n",
            sct, unexp);
    if(ferr) {
        fclose(ferr);
    }
}

void LPrsERR_APNF(const char *lgr, const char *apn)
{
    FILE *ferr = fopen("log.err", "wa+");
    fprintf(ferr ? ferr : stderr,
            LG_INIT_ERR_TK" parsing logger: %s, appender not found: %s\n", lgr, apn);
    if(ferr) {
        fclose(ferr);
    }
}

void LPrsERR_NOAP(const char *lgr)
{
    FILE *ferr = fopen("log.err", "wa+");
    fprintf(ferr ? ferr : stderr,
            LG_INIT_ERR_TK" parsing logger: %s, no apnds spec.\n", lgr);
    if(ferr) {
        fclose(ferr);
    }
}

void LPrsERR_UNEXP(const char *unexp)
{
    FILE *ferr = fopen("log.err", "wa+");
    fprintf(ferr ? ferr : stderr, LG_INIT_ERR_TK" unexpct. tkn: %s\n", unexp);
    if(ferr) {
        fclose(ferr);
    }
}

//-----------------------------
// appender_impl
//-----------------------------

hash_map *apnds = NULL;
hash_map &get_appender_map()
{
    if(apnds) {
        return *apnds;
    }
    if(!(apnds = new hash_map(sngl_apnd_obj_mng(),
                              sngl_cstr_obj_mng()))) {
        EXIT_ACTION("get_appender_map() - failed creating apnds map\n")
    }
    if(apnds->init(HM_SIZE_NANO)) {
        EXIT_ACTION("get_appender_map() - failed to init apnds map\n")
    }
    return *apnds;
}

class appender_impl {
    public:
        appender_impl() :
            valid_(true),
            fd_(NULL),
            msg_b_ln_(0) {
        }

        appender_impl(const char *fname) :
            valid_(false),
            fd_(NULL),
            msg_b_ln_(0) {
            fd_ = fopen(fname, "a+");
            valid_ = true;
        }

        appender_impl(FILE *fd) :
            valid_(true),
            fd_(fd),
            msg_b_ln_(0) {
        }

        ~appender_impl() {
            flush();
            if(fd_) {
                fclose(fd_);
            }
        }

        void flush() {
            if(fd_) {
                if(msg_b_ln_) {
                    fflush(fd_);
                }
            }
        }

        size_t put_msg(TraceLVL lvl,
                       const char *sign,
                       uint16_t sign_len,
                       uint32_t id,
                       const char *msg) {
            const char *lvl_str = get_trace_level_string(lvl);
            char msg_b[LG_BUF_LEN_16K];
            uint16_t msg_b_idx = 0;
            memset(msg_b, 0, LG_BUF_LEN_16K);
            render_msg(lvl_str, sign, sign_len, id, msg, msg_b, &msg_b_idx);
            msg_b_idx += (uint16_t)fwrite(msg_b, sizeof(char), msg_b_idx, fd_);
            fflush(fd_);
            return msg_b_idx;
        }

        size_t put_msg_plain(const char *msg) {
            char msg_b[LG_BUF_LEN_16K];
            uint16_t msg_b_idx = 0;
            memset(msg_b, 0, LG_BUF_LEN_16K);
            rended_msg_pln(msg, msg_b, &msg_b_idx);
            msg_b_idx += (uint16_t)fwrite(msg_b, sizeof(char), msg_b_idx, fd_);
            fflush(fd_);
            return msg_b_idx;
        }

        size_t put_msg_va(TraceLVL lvl,
                          const char *sign,
                          uint16_t sign_len,
                          uint32_t id,
                          const char *msg,
                          va_list args) {
            const char *lvl_str = get_trace_level_string(lvl);
            char msg_b[LG_BUF_LEN_16K];
            uint16_t msg_b_idx = 0;
            render_msg_va(lvl_str, sign, sign_len, id, msg, msg_b, &msg_b_idx, args);
            msg_b_idx += (uint16_t)fwrite(msg_b, sizeof(char), msg_b_idx, fd_);
            fflush(fd_);
            return msg_b_idx;
        }

        size_t put_msg_va_plain(const char *msg,
                                va_list args) {
            char msg_b[LG_BUF_LEN_16K];
            uint16_t msg_b_idx = 0;
            render_msg_va_pln(msg, msg_b, &msg_b_idx, args);
            msg_b_idx += (uint16_t)fwrite(msg_b, sizeof(char), msg_b_idx, fd_);
            fflush(fd_);
            return msg_b_idx;
        }


        size_t rended_msg_pln(const char *msg,
                              char *buff,
                              uint16_t *buff_len) {
            //timestamp
            char *msg_b = buff;
            uint16_t msg_b_idx = 0;
            //usr message
            if(msg && msg[0]) {
                msg_b_idx += sprintf(&msg_b[msg_b_idx], "%s", msg);
            }
            msg_b[msg_b_idx++] = '\n';
            *buff_len = msg_b_idx;
            return msg_b_idx;
        }

        size_t render_msg(const char *lvl_str,
                          const char *sign,
                          uint16_t sign_len,
                          uint32_t id,
                          const char *msg,
                          char *buff,
                          uint16_t *buff_len) {
            //timestamp
            char *msg_b = buff;
            uint16_t msg_b_idx = 0;
            struct timeval tv;
            gettimeofday(&tv, NULL);
            time_t secs = (time_t)tv.tv_sec;
            struct tm *gmt = localtime(&secs);
            sprintf(msg_b, "%02d:%02d:%02d.%03ld",
                    gmt->tm_hour,
                    gmt->tm_min,
                    gmt->tm_sec,
                    (tv.tv_usec / 1000L));
            msg_b_idx += LG_TIME_BUF_LEN;
            msg_b[msg_b_idx++] = '|';
            //trace level string
            memcpy(&msg_b[msg_b_idx], lvl_str, TL_LVL_STR_LEN);
            msg_b_idx += TL_LVL_STR_LEN;
            msg_b[msg_b_idx++] = '|';
            //signature
            if(sign_len) {
                memcpy(&msg_b[msg_b_idx], sign, sign_len);
                msg_b_idx += sign_len;
                msg_b[msg_b_idx++] = '|';
            }
            //id
            if(id) {
                sprintf(&msg_b[msg_b_idx], "%08u", id);
                msg_b_idx += LG_ID_LEN;
                msg_b[msg_b_idx++] = '|';
            }
            //usr message
            if(msg && msg[0]) {
                msg_b_idx += sprintf(&msg_b[msg_b_idx], "%s", msg);
            }
            msg_b[msg_b_idx++] = '\n';
            *buff_len = msg_b_idx;
            return msg_b_idx;
        }

        size_t render_msg_va_pln(const char *msg,
                                 char *buff,
                                 uint16_t *buff_len,
                                 va_list args) {
            char *msg_b = buff;
            uint16_t msg_b_idx = 0;
            //usr message
            if(msg && msg[0]) {
                msg_b_idx += vsprintf(&msg_b[msg_b_idx], msg, args);
            }
            msg_b[msg_b_idx++] = '\n';
            *buff_len = msg_b_idx;
            return msg_b_idx;
        }

        size_t render_msg_va(const char *lvl_str,
                             const char *sign,
                             uint16_t sign_len,
                             uint32_t id,
                             const char *msg,
                             char *buff,
                             uint16_t *buff_len,
                             va_list args) {
            //timestamp
            char *msg_b = buff;
            uint16_t msg_b_idx = 0;
            struct timeval tv;
            gettimeofday(&tv, NULL);
            time_t secs = (time_t)tv.tv_sec;
            struct tm *gmt = localtime(&secs);
            sprintf(msg_b, "%02d:%02d:%02d.%03ld",
                    gmt->tm_hour,
                    gmt->tm_min,
                    gmt->tm_sec,
                    (tv.tv_usec / 1000L));
            msg_b_idx += LG_TIME_BUF_LEN;
            msg_b[msg_b_idx++] = '|';
            //trace level string
            memcpy(&msg_b[msg_b_idx], lvl_str, TL_LVL_STR_LEN);
            msg_b_idx += TL_LVL_STR_LEN;
            msg_b[msg_b_idx++] = '|';
            //signature
            if(sign_len) {
                memcpy(&msg_b[msg_b_idx], sign, sign_len);
                msg_b_idx += sign_len;
                msg_b[msg_b_idx++] = '|';
            }
            //id
            if(id) {
                sprintf(&msg_b[msg_b_idx], "%08u", id);
                msg_b_idx += LG_ID_LEN;
                msg_b[msg_b_idx++] = '|';
            }
            //usr message
            if(msg && msg[0]) {
                msg_b_idx += vsprintf(&msg_b[msg_b_idx], msg, args);
            }
            msg_b[msg_b_idx++] = '\n';
            *buff_len = msg_b_idx;
            return msg_b_idx;
        }

    private:
        bool valid_;
        FILE *fd_;
        uint16_t msg_b_ln_;
};

//-----------------------------
// appender
//-----------------------------
appender::appender()
{
    impl_ = new appender_impl();
}

appender::appender(const char *fname)
{
    impl_ = new appender_impl(fname);
}

appender::appender(FILE *fd)
{
    impl_ = new appender_impl(fd);
}

appender::~appender()
{
    if(impl_) {
        delete impl_;
    }
}

void appender::flush()
{
    impl_->flush();
}

size_t appender::put_msg(TraceLVL lvl,
                         const char *sign,
                         uint16_t sign_len,
                         uint32_t id,
                         const char *msg)
{
    return impl_->put_msg(lvl,
                          sign,
                          sign_len,
                          id,
                          msg);
}

size_t appender::put_msg_plain(const char *msg)
{
    return impl_->put_msg_plain(msg);
}

size_t appender::put_msg_va(TraceLVL lvl,
                            const char *sign,
                            uint16_t sign_len,
                            uint32_t id,
                            const char *msg,
                            va_list args)
{
    return impl_->put_msg_va(lvl,
                             sign,
                             sign_len,
                             id,
                             msg,
                             args);
}

size_t appender::put_msg_va_plain(const char *msg, va_list args)
{
    return impl_->put_msg_va_plain(msg, args);
}


size_t appender::render_msg_pln(const char *msg,
                                char *buff,
                                uint16_t *buff_len)
{
    return impl_->rended_msg_pln(msg, buff, buff_len);
}

size_t appender::render_msg(const char *lvl_str,
                            const char *sign,
                            uint16_t sign_len,
                            uint32_t id,
                            const char *msg,
                            char *buff,
                            uint16_t *buff_len)
{
    return impl_->render_msg(lvl_str,
                             sign,
                             sign_len,
                             id,
                             msg,
                             buff,
                             buff_len);
}

size_t appender::render_msg_va_pln(const char *msg,
                                   char *buff,
                                   uint16_t *buff_len,
                                   va_list args)
{
    return impl_->render_msg_va_pln(msg,
                                    buff,
                                    buff_len,
                                    args);
}

size_t appender::render_msg_va(const char *lvl_str,
                               const char *sign,
                               uint16_t sign_len,
                               uint32_t id,
                               const char *msg,
                               char *buff,
                               uint16_t *buff_len,
                               va_list args)
{
    return impl_->render_msg_va(lvl_str,
                                sign,
                                sign_len,
                                id,
                                msg,
                                buff,
                                buff_len,
                                args);
}

//-----------------------------
// std_logger_cfg_loader
//-----------------------------
class std_logger_cfg_loader : public logger_cfg_loader {
    public:

        std_logger_cfg_loader(const char *fname) : log_cfg_(NULL) {
            strncpy(fname_, fname, CR_MAX_SRC_FILE_NAME_LEN);
        }

        RetCode init() {
            ascii_string path;
            path.assign(log_cfg_file_dir);
            if(path.length() > 0) {
                path.append(CR_FS_SEP);
            }
            path.append(fname_);
            log_cfg_ = fopen(path.internal_buff(), "r");
            if(!log_cfg_) {
                return RetCode_KO;
            }
            return RetCode_OK;
        }

        RetCode destroy() {
            if(fclose(log_cfg_)) {
                return RetCode_KO;
            }
            return RetCode_OK;
        }

        virtual RetCode load_cfg() {
            ascii_string data;
            char line_buf[CR_MAX_SRC_LINE_LEN+2];
            while(!feof(log_cfg_)) {
                if(fgets(line_buf, CR_MAX_SRC_LINE_LEN+2, log_cfg_)) {
                    if(is_blank_line(line_buf) || is_comment_line(line_buf)) {
                        continue;
                    } else {
                        data.append(line_buf);
                    }
                }
            }
            RetCode res = RetCode_OK;
            if((res = parse_data(data))) {
                FILE *ferr = fopen("log.err", "wa+");
                fprintf(ferr ? ferr : stderr, "%s() - %s [PARS]\n", __func__, LG_INIT_ERR_TK);
                if(ferr) {
                    fclose(ferr);
                }
                get_appender_map().clear();
                get_logger_map().clear();
            }
            return res;
        }

    private:

        RetCode parse_lines_max(ascii_string_tok &toknz, int &lnmax) {
            ascii_string tkn;
            while(!toknz.next_token(tkn)) {
                CR_SKIP_SP_TABS(tkn)
                if(is_new_line(tkn)) {
                    LPrsERR_FND_EXP(TKLGR, S_NL, S_LF);
                    return RetCode_BADCFG;
                } else if(tkn == CR_TK_COMA) {
                    LPrsERR_UNEXP(TKAPND, CR_TK_COMA);
                    return RetCode_BADCFG;
                } else {
                    lnmax = atoi(tkn.internal_buff());
                    return RetCode_OK;
                }
            }
            LPrsERR_EXP(TKLGR, S_LF);
            return RetCode_BADCFG;
        }

        RetCode read_dot(unsigned long &lnum, ascii_string_tok &tknz) {
            ascii_string tkn;
            while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_DOT, true)) {
                if(tkn == CR_TK_DOT) {
                    break;
                } else {
                    return RetCode_BADCFG;
                }
            }
            return RetCode_OK;
        }

        RetCode skip_sp_tab_and_read_eq(unsigned long &lnum, ascii_string_tok &tknz) {
            ascii_string tkn;
            while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_EQUAL, true)) {
                CR_SKIP_SP_TABS(tkn)
                CR_DO_CMD_ON_NEWLINE(tkn, return RetCode_BADCFG)
                if(tkn == CR_TK_EQUAL) {
                    break;
                } else {
                    return RetCode_BADCFG;
                }
            }
            return RetCode_OK;
        }

        RetCode read_tkn(unsigned long &lnum, ascii_string_tok &tknz,
                         ascii_string &tkn_out) {
            ascii_string tkn;
            if(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_DOT CR_TK_EQUAL, true)) {
                CR_DO_CMD_ON_NEWLINE(tkn, return RetCode_BADCFG)
                tkn_out.assign(tkn);
                return RetCode_OK;
            }
            return RetCode_BADCFG;
        }

        RetCode parse_apnd(ascii_string &apname, ascii_string_tok &toknz) {
            RetCode res = RetCode_OK;
            int lnmax;
            ascii_string tkn;
            ascii_string app_file_name;
            appender *apnd = NULL;
            bool apnd_parsed = false, skip = false;
            while(!apnd_parsed && !toknz.next_token(tkn, ",\n\r\f\t ", true)) {
                CR_SKIP_SP_TABS(tkn)
                if(tkn == "stdout") {
                    while(!toknz.next_token(tkn, NULL, true)) {
                        CR_SKIP_SP_TABS(tkn)
                        if(is_new_line(tkn)) {
                            if(!(apnd = new appender(stdout))) {
                                return RetCode_MEMERR;
                            }
                            apnd_parsed = skip = true;
                            break;
                        } else if(tkn == CR_TK_COMA) {
                            if((res = parse_lines_max(toknz, lnmax))) {
                                return res;
                            }
                            if(!(apnd = new appender(stdout))) {
                                return RetCode_MEMERR;
                            }
                            apnd_parsed = true;
                            break;
                        } else {
                            LPrsERR_UNEXP(TKAPND, tkn.internal_buff());
                            return RetCode_BADCFG;
                        }
                    }
                } else if(tkn == "file") {
                    while(!toknz.next_token(tkn, NULL, true)) {
                        CR_SKIP_SP_TABS(tkn)
                        CR_BREAK_ON_TKN(tkn, CR_TK_COMA)
                        if(is_new_line(tkn)) {
                            LPrsERR_EXP(TKAPND, S_NL);
                            return RetCode_BADCFG;
                        } else {
                            LPrsERR_UNEXP(TKAPND, tkn.internal_buff());
                        }
                    }
                    while(!toknz.next_token(tkn, NULL, true)) {
                        CR_SKIP_SP_TABS(tkn)
                        if(tkn == CR_TK_COMA) {
                            LPrsERR_UNEXP(TKAPND, CR_TK_COMA);
                            return RetCode_BADCFG;
                        }
                        if(is_new_line(tkn)) {
                            LPrsERR_UNEXP(TKAPND, S_NL);
                            return RetCode_BADCFG;
                        }
                        break;
                    }
                    RETURN_IF_NOT_OK(app_file_name.assign(tkn))
                    while(!toknz.next_token(tkn, NULL, true)) {
                        CR_SKIP_SP_TABS(tkn)
                        CR_BREAK_ON_TKN(tkn, CR_TK_COMA)
                        if(is_new_line(tkn)) {
                            if(!(apnd = new appender(app_file_name.internal_buff()))) {
                                return RetCode_MEMERR;
                            }
                            apnd_parsed = skip = true;
                            break;
                        }
                    }
                    if(apnd_parsed) {
                        break;
                    }
                    if((res = parse_lines_max(toknz, lnmax))) {
                        return res;
                    }
                    if(!(apnd = new appender(app_file_name.internal_buff()))) {
                        return RetCode_MEMERR;
                    }
                    break;
                } else if(is_new_line(tkn)) {
                    LPrsERR_UNEXP(TKAPND, S_NL);
                    return RetCode_BADCFG;
                } else if(tkn == CR_TK_COMA) {
                    LPrsERR_UNEXP(TKAPND, CR_TK_COMA);
                    return RetCode_BADCFG;
                } else {
                    LPrsERR_UNEXP(TKAPND, tkn.internal_buff());
                    return RetCode_BADCFG;
                }
            }
            while(!skip && !toknz.next_token(tkn, "\n\r\f\t ", true)) {
                CR_SKIP_SP_TABS(tkn)
                if(is_new_line(tkn)) {
                    break;
                }
                LPrsERR_UNEXP(TKAPND, tkn.internal_buff());
                return RetCode_BADCFG;
            }
            return get_appender_map().put(apname.internal_buff(), &apnd);
        }

        RetCode parse_lggr(ascii_string &lggrname,
                           ascii_string &lgsign,
                           ascii_string_tok &toknz) {
            ascii_string tkn;
            logger *lggr = NULL;
            bool lggr_parsed = false;
            while(!lggr_parsed && !toknz.next_token(tkn, ",\n\r\f\t ", true)) {
                CR_SKIP_SP_TABS(tkn)
                TraceLVL lvl = TL_DBG;
                if((lvl = get_trace_level_enum(tkn.internal_buff())) >= 0) {
                    uint16_t apnds_n = 0;
                    appender *apnds_l[LG_MAX_APNDS];
                    memset(apnds_l, 0, sizeof(apnds_l));
                    while(!toknz.next_token(tkn, NULL, true)) {
                        CR_SKIP_SP_TABS(tkn)
                        if(tkn == CR_TK_COMA) {
                            while(!toknz.next_token(tkn, NULL, true)) {
                                CR_SKIP_SP_TABS(tkn)
                                if(is_new_line(tkn)) {
                                    LPrsERR_UNEXP(TKLGR, S_NL);
                                    return RetCode_BADCFG;
                                }
                                if(tkn == CR_TK_COMA) {
                                    LPrsERR_UNEXP(TKLGR, CR_TK_COMA);
                                    return RetCode_BADCFG;
                                }
                                break;
                            }
                            if(!get_appender_map().get(tkn.internal_buff(), &apnds_l[apnds_n++])) {
                                continue;
                            } else {
                                LPrsERR_APNF(lggrname.internal_buff(), tkn.internal_buff());
                                return RetCode_BADCFG;
                            }
                        }
                        if(is_new_line(tkn)) {
                            if(apnds_n) {
                                lggr_parsed = true;
                                break;
                            } else {
                                LPrsERR_NOAP(lggrname.internal_buff());
                                return RetCode_BADCFG;
                            }
                        }
                    }
                    if(!(lggr = new logger())) {
                        return RetCode_MEMERR;
                    }
                    RETURN_IF_NOT_OK(lggr->init(lvl, lgsign.internal_buff(), apnds_l, apnds_n))
                } else if(is_new_line(tkn)) {
                    LPrsERR_UNEXP(TKLGR, S_NL);
                    return RetCode_BADCFG;
                } else if(tkn == ",") {
                    LPrsERR_UNEXP(TKLGR, CR_TK_COMA);
                    return RetCode_BADCFG;
                } else {
                    LPrsERR_UNEXP(TKLGR, tkn.internal_buff());
                    return RetCode_BADCFG;
                }
            }
            return get_logger_map().put(lggrname.internal_buff(), &lggr);
        }

        RetCode parse_data(ascii_string &data) {
            RetCode res = RetCode_OK;
            unsigned long lnum = 1;
            ascii_string tkn, apname, lggrname, lgsign;
            ascii_string_tok tknz;
            RETURN_IF_NOT_OK(tknz.init(data))
            while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_DOT CR_TK_EQUAL, true)) {
                CR_SKIP_SP_TABS(tkn)
                CR_DO_CMD_ON_NEWLINE(tkn, lnum++; continue)
                if(tkn == TKAPND) {
                    //APPENDER
                    RETURN_IF_NOT_OK(read_dot(lnum, tknz))
                    RETURN_IF_NOT_OK(read_tkn(lnum, tknz, apname))
                    RETURN_IF_NOT_OK(skip_sp_tab_and_read_eq(lnum, tknz))
                    if((res = parse_apnd(apname, tknz))) {
                        break;
                    }
                } else if(tkn == TKLGR) {
                    //LOGGER
                    RETURN_IF_NOT_OK(read_dot(lnum, tknz))
                    RETURN_IF_NOT_OK(read_tkn(lnum, tknz, lggrname))
                    RETURN_IF_NOT_OK(read_dot(lnum, tknz))
                    RETURN_IF_NOT_OK(read_tkn(lnum, tknz, lgsign))
                    RETURN_IF_NOT_OK(skip_sp_tab_and_read_eq(lnum, tknz))
                    if((res = parse_lggr(lggrname, lgsign, tknz))) {
                        break;
                    }
                } else {
                    LPrsERR_UNEXP(tkn.internal_buff());
                    return RetCode_BADCFG;
                }
            }
            return res;
        }

    private:
        char fname_[CR_MAX_SRC_FILE_NAME_LEN+1];
        FILE *log_cfg_;
};

//-----------------------------
// logger_impl
//-----------------------------
class logger_impl {
    public:
        logger_impl(TraceLVL level,
                    const char *sign,
                    appender *apnds[],
                    uint16_t apnds_n) :
            lvl_(level),
            sign_len_((uint16_t)min(strlen(sign), LG_MAX_SIGN_LEN)),
            apnds_n_(apnds_n),
            l_evt_obs_(sizeof(void *)) {
            memset(sign_, 0, sizeof(sign_));
            strncpy(sign_, sign, sign_len_);
            for(uint16_t i = 0; i < apnds_n; i++) {
                apnds_[i] = apnds[i];
            }
            l_evt_obs_.init();
        }

        inline size_t spread_va_pln(const char *msg, va_list args) {
            for(uint16_t i = 0; i < apnds_n_-1; i++) {
                apnds_[i]->put_msg_va_plain(msg, args);
            }
            return apnds_[apnds_n_-1]->put_msg_va_plain(msg, args);
        }

        inline size_t spread_pln(const char *msg) {
            for(uint16_t i = 0; i < apnds_n_-1; i++) {
                apnds_[i]->put_msg_plain(msg);
            }
            return apnds_[apnds_n_-1]->put_msg_plain(msg);
        }

        inline size_t spread_va(TraceLVL level,
                                uint32_t id,
                                const char *msg,
                                va_list args) {
            for(uint16_t i = 0; i < apnds_n_-1; i++) {
                apnds_[i]->put_msg_va(level, sign_, sign_len_, id, msg, args);
            }
            return apnds_[apnds_n_-1]->put_msg_va(level,
                                                  sign_,
                                                  sign_len_,
                                                  id,
                                                  msg,
                                                  args);
        }

        inline size_t spread(TraceLVL level, uint32_t id, const char *msg) {
            for(uint16_t i = 0; i < apnds_n_-1; i++) {
                apnds_[i]->put_msg(level, sign_, sign_len_, id, msg);
            }
            return apnds_[apnds_n_-1]->put_msg(level, sign_, sign_len_, id, msg);
        }

        inline void set_level(TraceLVL level) {
            lvl_ = level;
        }

        TraceLVL   lvl_;
        uint16_t    sign_len_;
        char        sign_[LG_MAX_SIGN_LEN+1];
        uint16_t    apnds_n_;
        appender    *apnds_[LG_MAX_APNDS];
        linked_list l_evt_obs_;
};

#define LGGRREP_SPREADVA_PLN(msg, args)\
for(uint16_t i = 0; i < impl_->apnds_n_; i++){\
    va_start(args, msg);\
    blen = impl_->apnds_[i]->put_msg_va_plain(msg, args);\
    va_end(args);\
}

#define LGGRREP_SPREADVA(level, id, msg, args)\
for(uint16_t i = 0; i < impl_->apnds_n_; i++){\
    va_start(args, msg);\
    blen = impl_->apnds_[i]->put_msg_va(level,\
                                        impl_->sign_,\
                                        impl_->sign_len_,\
                                        id,\
                                        msg,\
                                        args);\
    va_end(args);\
}

//-----------------------------
// logger
//-----------------------------
RetCode logger::set_logger_cfg_file_dir(const char *dir)
{
    strncpy(log_cfg_file_dir,dir, LOG_CFG_FILE_DIR_LEN);
    return RetCode_OK;
}

RetCode logger::set_logger_cfg_file_path_name(const char *file_path)
{
    strncpy(log_cfg_file_name_path,file_path, LOG_CFG_FILE_NAME_PATH_LEN);
    return RetCode_OK;
}

RetCode logger::load_logger_config()
{
    return strlen(log_cfg_file_name_path) ?
           load_logger_config(log_cfg_file_name_path) :
           load_logger_config("logger.cfg");
}

RetCode logger::load_logger_config(const char *fname)
{
    std_logger_cfg_loader scl(fname);
    RETURN_IF_NOT_OK(scl.init())
    RETURN_IF_NOT_OK(scl.load_cfg())
    RETURN_IF_NOT_OK(scl.destroy())
    return RetCode_OK;
}

logger *logger::get_logger(const char *logger_name)
{
    if(!deflt_log_loaded) {
        COMMAND_IF_NOT_OK(load_logger_config(), NO_ACTION)
        deflt_log_loaded = true;
    }
    logger *log = NULL;
    COMMAND_IF_NOT_OK(get_logger(logger_name, &log), NO_ACTION)
    if(!log) {
        COMMAND_IF_NOT_OK(get_logger("root", &log), NO_ACTION)
    }
    return log;
}

RetCode logger::get_logger(const char *logger_name, logger **lggr)
{
    logger *l_ptr = NULL;
    if(!logger_name || !logger_name[0]) {
        return RetCode_BADARG;
    }
    if(!get_logger_map().get(logger_name, &l_ptr)) {
        *lggr = l_ptr;
        return RetCode_OK;
    } else {
        return RetCode_KO;
    }
}

logger::logger() : impl_(NULL)
{}

logger::~logger()
{
    if(impl_) {
        delete impl_;
    }
}

void logger::set_rep_from(logger &other)
{
    impl_ = other.get_opaque();
}

RetCode logger::init(TraceLVL level,
                     const char *sign,
                     appender *apnds[],
                     uint16_t apnds_n)
{
    impl_ = new logger_impl(level, sign, apnds, apnds_n);
    if(!impl_) {
        return RetCode_MEMERR;
    }
    return RetCode_OK;
}

TraceLVL logger::level()
{
    return impl_->lvl_;
}

logger_impl *logger::get_opaque()
{
    return impl_;
}

appender **logger::get_appenders()
{
    return &impl_->apnds_[0];
}

uint16_t logger::get_appender_count()
{
    return impl_->apnds_n_;
}

void logger::set_level(TraceLVL level)
{
    impl_->set_level(level);
}

RetCode logger::add_appender(appender *apnd)
{
    if(!apnd) {
        return RetCode_BADARG;
    }
    if(impl_->apnds_n_ < LG_MAX_APNDS) {
        impl_->apnds_[impl_->apnds_n_++] = apnd;
    } else {
        return RetCode_OVRSZ;
    }
    return RetCode_OK;
}

RetCode logger::remove_last_appender()
{
    if(impl_->apnds_n_) {
        impl_->apnds_n_--;
    }
    return RetCode_OK;
}

size_t logger::log(TraceLVL level, uint32_t id, const char *msg, ...)
{
    size_t blen = 0;
    va_list args;
    switch(level) {
        case TL_PLN:
            LGGRREP_SPREADVA_PLN(msg, args);
            break;
        case TL_TRC:
            if(impl_->lvl_ <= TL_TRC) {
                LGGRREP_SPREADVA(TL_TRC, id, msg, args)
            }
            break;
        case TL_DBG:
            if(impl_->lvl_ <= TL_DBG) {
                LGGRREP_SPREADVA(TL_DBG, id, msg, args)
            }
            break;
        case TL_INF:
            if(impl_->lvl_ <= TL_INF) {
                LGGRREP_SPREADVA(TL_INF, id, msg, args)
            }
            break;
        case TL_WRN:
            if(impl_->lvl_ <= TL_WRN) {
                LGGRREP_SPREADVA(TL_WRN, id, msg, args)
            }
            break;
        case TL_ERR:
            if(impl_->lvl_ <= TL_ERR) {
                LGGRREP_SPREADVA(TL_ERR, id, msg, args)
            }
            break;
        case TL_CRI:
            if(impl_->lvl_ <= TL_CRI) {
                LGGRREP_SPREADVA(TL_CRI, id, msg, args)
            }
            break;
        case TL_FAT:
            if(impl_->lvl_ <= TL_FAT) {
                LGGRREP_SPREADVA(TL_FAT, id, msg, args)
            }
            break;
        default:
            break;
    }
    return blen;
}

size_t logger::pln(const char *msg, ...)
{
    size_t blen = 0;
    va_list args;
    LGGRREP_SPREADVA_PLN(msg, args)
    return blen;
}

size_t logger::low(uint32_t id, const char *msg, ...)
{
    size_t blen = 0;
    va_list args;
    if(impl_->lvl_ <= TL_LOW) {
        LGGRREP_SPREADVA(TL_LOW, id, msg, args)
    }
    return blen;
}

size_t logger::trc(uint32_t id, const char *msg, ...)
{
    size_t blen = 0;
    va_list args;
    if(impl_->lvl_ <= TL_TRC) {
        LGGRREP_SPREADVA(TL_TRC, id, msg, args)
    }
    return blen;
}

size_t logger::dbg(uint32_t id, const char *msg,...)
{
    size_t blen = 0;
    va_list args;
    if(impl_->lvl_ <= TL_DBG) {
        LGGRREP_SPREADVA(TL_DBG, id, msg, args)
    }
    return blen;
}

size_t logger::inf(uint32_t id, const char *msg,...)
{
    size_t blen = 0;
    va_list args;
    if(impl_->lvl_ <= TL_INF) {
        LGGRREP_SPREADVA(TL_INF, id, msg, args)
    }
    return blen;
}

size_t logger::wrn(uint32_t id, const char *msg,...)
{
    size_t blen = 0;
    va_list args;
    if(impl_->lvl_ <= TL_WRN) {
        LGGRREP_SPREADVA(TL_WRN, id, msg, args)
    }
    return blen;
}

size_t logger::err(uint32_t id, const char *msg,...)
{
    size_t blen = 0;
    va_list args;
    if(impl_->lvl_ <= TL_ERR) {
        LGGRREP_SPREADVA(TL_ERR, id, msg, args)
    }
    return blen;
}

size_t logger::cri(uint32_t id, const char *msg,...)
{
    size_t blen = 0;
    va_list args;
    if(impl_->lvl_ <= TL_CRI) {
        LGGRREP_SPREADVA(TL_CRI, id, msg, args)
    }
    return blen;
}

size_t logger::fat(uint32_t id, const char *msg,...)
{
    size_t blen = 0;
    va_list args;
    if(impl_->lvl_ <= TL_FAT) {
        LGGRREP_SPREADVA(TL_FAT, id, msg, args)
    }
    return blen;
}

size_t logger::plnm(const char *msg)
{
    return impl_->spread_pln(msg);
}

size_t logger::lowm(uint32_t id, const char *msg)
{
    return impl_->spread(TL_LOW, id, msg);
}

size_t logger::trcm(uint32_t id, const char *msg)
{
    return impl_->spread(TL_TRC,id, msg);
}

size_t logger::dbgm(uint32_t id, const char *msg)
{
    return impl_->spread(TL_DBG,id, msg);
}

size_t logger::infm(uint32_t id, const char *msg)
{
    return impl_->spread(TL_INF,id, msg);
}

size_t logger::wrnm(uint32_t id, const char *msg)
{
    return impl_->spread(TL_WRN,id, msg);
}

size_t logger::errm(uint32_t id, const char *msg)
{
    return impl_->spread(TL_ERR,id, msg);
}

size_t logger::crim(uint32_t id, const char *msg)
{
    return impl_->spread(TL_CRI,id, msg);
}

size_t logger::fatm(uint32_t id, const char *msg)
{
    return impl_->spread(TL_FAT,id, msg);
}

}

//C inferf.

extern "C" {

    vlg::RetCode logger_set_logger_cfg_file_dir(const char *dir)
    {
        return vlg::logger::set_logger_cfg_file_dir(dir);
    }

    vlg::RetCode logger_set_logger_cfg_file_path_name(const char *file_path)
    {
        return vlg::logger::set_logger_cfg_file_path_name(file_path);
    }

    vlg::RetCode logger_load_logger_config()
    {
        return vlg::logger::load_logger_config();
    }

    vlg::RetCode logger_load_logger_config_by_name(const char *fname)
    {
        return vlg::logger::load_logger_config(fname);
    }

}
