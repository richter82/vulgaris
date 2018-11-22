/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "glob.h"

#define TL_TRC_STR "TRC"
#define TL_DBG_STR "DBG"
#define TL_INF_STR "INF"
#define TL_WRN_STR "WRN"
#define TL_ERR_STR "ERR"
#define TL_CRI_STR "CRI"
#define TL_OFF_STR "OFF"

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

static std::string log_cfg_file_dir;
static std::string log_cfg_file_name_path;

TraceLVL get_trace_level_enum(const char *str)
{
    if(!strcmp(str, TL_TRC_STR)) {
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
    } else if(!strcmp(str, TL_OFF_STR)) {
        return TL_OFF;
    } else {
        return TL_EVL;
    }
}

const char *get_trace_level_string(TraceLVL lvl)
{
    switch(lvl) {
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
        case TL_OFF:
            return TL_OFF_STR;
        default:
            return TL_INF_STR;
    }
}

spdlog::level::level_enum get_spdlevel(TraceLVL lvl)
{
    switch(lvl) {
        case TL_TRC:
            return spdlog::level::level_enum::trace;
        case TL_DBG:
            return spdlog::level::level_enum::debug;
        case TL_INF:
            return spdlog::level::level_enum::info;
        case TL_WRN:
            return spdlog::level::level_enum::warn;
        case TL_ERR:
            return spdlog::level::level_enum::err;
        case TL_CRI:
            return spdlog::level::level_enum::critical;
        case TL_OFF:
            return spdlog::level::level_enum::off;
        default:
            return spdlog::level::level_enum::info;
    }
}

void LPrsERR_FND_EXP(const char *sct, const char *fnd, const char *exp)
{
    FILE *ferr = fopen("log.err", "wa+");
    fprintf(ferr ? ferr : stderr, LG_INIT_ERR_TK" parsing: %s, found: %s, expected: %s\n", sct, fnd, exp);
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
    fprintf(ferr ? ferr : stderr, LG_INIT_ERR_TK" parsing logger: %s, appender not found: %s\n", lgr, apn);
    if(ferr) {
        fclose(ferr);
    }
}

void LPrsERR_NOAP(const char *lgr)
{
    FILE *ferr = fopen("log.err", "wa+");
    fprintf(ferr ? ferr : stderr, LG_INIT_ERR_TK" parsing logger: %s, no apnds spec.\n", lgr);
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

// appender_impl

static std::unique_ptr<std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>>> apnds;

std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>> &get_appender_map()
{
    if(!apnds) {
        apnds.reset(new std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>>());
    }
    return *apnds;
}

// logger_cfg_loader

struct logger_cfg_loader {
    virtual ~logger_cfg_loader() {};
    virtual RetCode load_cfg() = 0;
};

// std_logger_cfg_loader

struct std_logger_cfg_loader : public logger_cfg_loader {
    std_logger_cfg_loader(const char *fname) :
        fname_(fname),
        log_cfg_(nullptr) {
    }

    RetCode init() {
        std::string path;
        path.assign(log_cfg_file_dir);
        if(path.length() > 0) {
            path.append(FS_SEP);
        }
        path.append(fname_);
        log_cfg_ = fopen(path.c_str(), "r");
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
        std::string data;
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
            spdlog::shutdown();
        }
        return res;
    }

    RetCode parse_lines_max(str_tok &toknz, int &lnmax) {
        std::string tkn;
        while(toknz.next_token(tkn)) {
            SKIP_SP_TAB(tkn)
            if(is_new_line(tkn)) {
                LPrsERR_FND_EXP(TKLGR, S_NL, S_LF);
                return RetCode_BADCFG;
            } else if(tkn == CM) {
                LPrsERR_UNEXP(TKAPND, CM);
                return RetCode_BADCFG;
            } else {
                lnmax = atoi(tkn.c_str());
                return RetCode_OK;
            }
        }
        LPrsERR_EXP(TKLGR, S_LF);
        return RetCode_BADCFG;
    }

    RetCode read_dot(unsigned long &lnum, str_tok &tknz) {
        std::string tkn;
        while(tknz.next_token(tkn, DF_DLM DT, true)) {
            if(tkn == DT) {
                break;
            } else {
                return RetCode_BADCFG;
            }
        }
        return RetCode_OK;
    }

    RetCode skip_sp_tab_and_read_eq(unsigned long &lnum, str_tok &tknz) {
        std::string tkn;
        while(tknz.next_token(tkn, DF_DLM EQ, true)) {
            SKIP_SP_TAB(tkn)
            DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
            if(tkn == EQ) {
                break;
            } else {
                return RetCode_BADCFG;
            }
        }
        return RetCode_OK;
    }

    RetCode read_tkn(unsigned long &lnum, str_tok &tknz,
                     std::string &tkn_out) {
        std::string tkn;
        if(tknz.next_token(tkn, DF_DLM DT EQ, true)) {
            DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
            tkn_out.assign(tkn);
            return RetCode_OK;
        }
        return RetCode_BADCFG;
    }

    RetCode parse_apnd(std::string &apname, str_tok &toknz) {
        RetCode res = RetCode_OK;
        int lnmax;
        std::string tkn;
        std::string app_file_name;
        std::shared_ptr<spdlog::sinks::sink> apnd;
        TraceLVL lvl = TL_EVL;
        bool apnd_parsed = false, skip = false;
        while(!apnd_parsed && toknz.next_token(tkn, ",\n\r\f\t ", true)) {
            SKIP_SP_TAB(tkn)
            lvl = get_trace_level_enum(tkn.c_str());
            if(lvl >= 0) {
                if(tkn == "console") {
                    while(toknz.next_token(tkn, nullptr, true)) {
                        SKIP_SP_TAB(tkn)
                        if(is_new_line(tkn)) {
                            apnd = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                            apnd_parsed = skip = true;
                            break;
                        } else if(tkn == CM) {
                            if((res = parse_lines_max(toknz, lnmax))) {
                                return res;
                            }
                            apnd = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                            apnd_parsed = true;
                            break;
                        } else {
                            LPrsERR_UNEXP(TKAPND, tkn.c_str());
                            return RetCode_BADCFG;
                        }
                    }
                } else if(tkn == "file") {
                    while(toknz.next_token(tkn, nullptr, true)) {
                        SKIP_SP_TAB(tkn)
                        BRK_ON_TKN(tkn, CM)
                        if(is_new_line(tkn)) {
                            LPrsERR_EXP(TKAPND, S_NL);
                            return RetCode_BADCFG;
                        } else {
                            LPrsERR_UNEXP(TKAPND, tkn.c_str());
                        }
                    }
                    while(toknz.next_token(tkn, nullptr, true)) {
                        SKIP_SP_TAB(tkn)
                        if(tkn == CM) {
                            LPrsERR_UNEXP(TKAPND, CM);
                            return RetCode_BADCFG;
                        }
                        if(is_new_line(tkn)) {
                            LPrsERR_UNEXP(TKAPND, S_NL);
                            return RetCode_BADCFG;
                        }
                        break;
                    }
                    app_file_name.assign(tkn);
                    while(toknz.next_token(tkn, nullptr, true)) {
                        SKIP_SP_TAB(tkn)
                        BRK_ON_TKN(tkn, CM)
                        if(is_new_line(tkn)) {
                            apnd = std::make_shared<spdlog::sinks::basic_file_sink_mt>(app_file_name.c_str(), true);
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
                    apnd = std::make_shared<spdlog::sinks::basic_file_sink_mt>(app_file_name.c_str(), true);
                    break;
                }
            } else if(is_new_line(tkn)) {
                LPrsERR_UNEXP(TKAPND, S_NL);
                return RetCode_BADCFG;
            } else if(tkn == CM) {
                LPrsERR_UNEXP(TKAPND, CM);
                return RetCode_BADCFG;
            } else {
                LPrsERR_UNEXP(TKAPND, tkn.c_str());
                return RetCode_BADCFG;
            }
        }
        while(!skip && toknz.next_token(tkn, "\n\r\f\t ", true)) {
            SKIP_SP_TAB(tkn)
            if(is_new_line(tkn)) {
                break;
            }
            LPrsERR_UNEXP(TKAPND, tkn.c_str());
            return RetCode_BADCFG;
        }
        apnd->set_level(get_spdlevel(lvl));
        get_appender_map()[apname] = apnd;
        return RetCode_OK;
    }

    RetCode parse_lggr(std::string &lggrname,
                       str_tok &toknz) {
        std::string tkn;
        std::shared_ptr<spdlog::logger> lggr;
        TraceLVL lvl = TL_EVL;

        bool lggr_parsed = false;
        while(!lggr_parsed && toknz.next_token(tkn, ",\n\r\f\t ", true)) {
            SKIP_SP_TAB(tkn)
            lvl = get_trace_level_enum(tkn.c_str());
            if(lvl >= 0) {
                std::list<std::shared_ptr<spdlog::sinks::sink>> apnds_l;

                while(toknz.next_token(tkn, nullptr, true)) {
                    SKIP_SP_TAB(tkn)
                    if(tkn == CM) {
                        while(toknz.next_token(tkn, nullptr, true)) {
                            SKIP_SP_TAB(tkn)
                            if(is_new_line(tkn)) {
                                LPrsERR_UNEXP(TKLGR, S_NL);
                                return RetCode_BADCFG;
                            }
                            if(tkn == CM) {
                                LPrsERR_UNEXP(TKLGR, CM);
                                return RetCode_BADCFG;
                            }
                            break;
                        }
                        auto it = get_appender_map().find(tkn);
                        if(it != get_appender_map().end()) {
                            apnds_l.push_back(it->second);
                            continue;
                        } else {
                            LPrsERR_APNF(lggrname.c_str(), tkn.c_str());
                            return RetCode_BADCFG;
                        }
                    }
                    if(is_new_line(tkn)) {
                        if(!apnds_l.empty()) {
                            lggr_parsed = true;
                            break;
                        } else {
                            LPrsERR_NOAP(lggrname.c_str());
                            return RetCode_BADCFG;
                        }
                    }
                }
                lggr.reset(new spdlog::logger(lggrname.c_str(), apnds_l.begin(), apnds_l.end()));
            } else if(is_new_line(tkn)) {
                LPrsERR_UNEXP(TKLGR, S_NL);
                return RetCode_BADCFG;
            } else if(tkn == ",") {
                LPrsERR_UNEXP(TKLGR, CM);
                return RetCode_BADCFG;
            } else {
                LPrsERR_UNEXP(TKLGR, tkn.c_str());
                return RetCode_BADCFG;
            }
        }
        lggr->set_level(get_spdlevel(lvl));
        spdlog::register_logger(lggr);
        return RetCode_OK;
    }

    RetCode parse_data(std::string &data) {
        RetCode res = RetCode_OK;
        unsigned long lnum = 1;
        std::string tkn, apname, lggrname;
        str_tok tknz(data);
        while(tknz.next_token(tkn, DF_DLM DT EQ, true)) {
            SKIP_SP_TAB(tkn)
            DO_CMD_ON_NL(tkn, lnum++; continue)
            if(tkn == TKAPND) {
                //APPENDER
                RET_ON_KO(read_dot(lnum, tknz))
                RET_ON_KO(read_tkn(lnum, tknz, apname))
                RET_ON_KO(skip_sp_tab_and_read_eq(lnum, tknz))
                if((res = parse_apnd(apname, tknz))) {
                    break;
                }
            } else if(tkn == TKLGR) {
                //LOGGER
                RET_ON_KO(read_dot(lnum, tknz))
                RET_ON_KO(read_tkn(lnum, tknz, lggrname))
                RET_ON_KO(skip_sp_tab_and_read_eq(lnum, tknz))
                if((res = parse_lggr(lggrname, tknz))) {
                    break;
                }
            } else {
                LPrsERR_UNEXP(tkn.c_str());
                return RetCode_BADCFG;
            }
        }
        return res;
    }

    std::string fname_;
    FILE *log_cfg_;
};

// logger

std::shared_ptr<spdlog::logger> get_logger(const char *logger_name)
{
    return spdlog::get(logger_name) ? spdlog::get(logger_name) : spdlog::default_logger();
}

extern "C" {
    RetCode syslog_set_cfg_file_dir(const char *dir)
    {
        log_cfg_file_dir = dir;
        return RetCode_OK;
    }

    RetCode syslog_set_cfg_file_path_name(const char *file_path)
    {
        log_cfg_file_name_path = file_path;
        return RetCode_OK;
    }

    RetCode syslog_load_config()
    {
        return log_cfg_file_name_path.size() ?
               syslog_load_config_by_fname(log_cfg_file_name_path.c_str()) :
               syslog_load_config_by_fname("logger.cfg");
    }

    RetCode syslog_load_config_by_fname(const char *fname)
    {
        std_logger_cfg_loader scl(fname);
        RET_ON_KO(scl.init())
        RET_ON_KO(scl.load_cfg())
        RET_ON_KO(scl.destroy())
        return RetCode_OK;
    }

    logger *syslog_get_retained(const char *logger_name)
    {
        return (logger *) new std::shared_ptr<spdlog::logger>(get_logger(logger_name));
    }

    void syslog_release_retained(logger *l)
    {
        delete(std::shared_ptr<spdlog::logger> *)(l);
    }

	void syslog_unload()
	{
		spdlog::shutdown();
		get_appender_map().clear();
	}
}

}
