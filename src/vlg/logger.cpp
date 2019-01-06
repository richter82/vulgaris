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

#define TKLGR   "logger"
#define TKSNK   "sinker"
#define TKGLOB  "global"
#define TKPTTRN "pattern"
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
    fprintf(ferr ? ferr : stderr, LG_INIT_ERR_TK" parsing logger: %s, sinker not found: %s\n", lgr, apn);
    if(ferr) {
        fclose(ferr);
    }
}

void LPrsERR_NOAP(const char *lgr)
{
    FILE *ferr = fopen("log.err", "wa+");
    fprintf(ferr ? ferr : stderr, LG_INIT_ERR_TK" parsing logger: %s, no sinkers\n", lgr);
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

static std::unique_ptr<std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>>> sinkers;

std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>> &get_sinker_map()
{
    if(!sinkers) {
        sinkers.reset(new std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>>());
    }
    return *sinkers;
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
            get_sinker_map().clear();
            spdlog::shutdown();
        }
        return res;
    }

    // trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(),
                std::find_if(s.begin(),
                             s.end(),
                             std::not1(std::ptr_fun<int, int>(std::isspace))));
    }

    // trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(),
                             s.rend(),
                             std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
                s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    RetCode read_expected_token(unsigned long &lnum,
                                const char *add_delims,
                                const char *exp_tok,
                                bool skip_sp_tab,
                                str_tok &tknz) {
        std::string tkn;
        std::string delims(DF_DLM);
        if(add_delims) {
            delims += add_delims;
        }
        while(tknz.next_token(tkn, delims.c_str(), true)) {
            if(skip_sp_tab) {
                SKIP_SP_TAB(tkn)
            }
            if(tkn == exp_tok) {
                return RetCode_OK;
            } else {
                return RetCode_BADCFG;
            }
        }
        return RetCode_BADCFG;
    }

    RetCode skip_sp_tab_and_read_tkn(unsigned long &lnum,
                                     const char *add_delims,
                                     str_tok &tknz,
                                     std::string &tkn_out) {
        std::string tkn;
        std::string delims(DF_DLM);
        if(add_delims) {
            delims += add_delims;
        }
        while(tknz.next_token(tkn, delims.c_str(), true)) {
            SKIP_SP_TAB(tkn)
            DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
            tkn_out.assign(tkn);
            return RetCode_OK;
        }
        return RetCode_BADCFG;
    }

    RetCode read_tkn_no_nwl(unsigned long &lnum,
                            const char *add_delims,
                            str_tok &tknz,
                            std::string &tkn_out) {
        std::string tkn;
        std::string delims(DF_DLM);
        if(add_delims) {
            delims += add_delims;
        }
        if(tknz.next_token(tkn, delims.c_str(), true)) {
            DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
            tkn_out.assign(tkn);
            return RetCode_OK;
        }
        return RetCode_BADCFG;
    }

    RetCode read_tkn_until_nwl(unsigned long &lnum,
                               str_tok &tknz,
                               std::string &tkn_out) {
        std::string tkn;
        if(tknz.next_token(tkn, NL_DLM, true)) {
            DO_CMD_ON_NL(tkn, return RetCode_BADCFG)
            tkn_out.assign(tkn);
            trim(tkn_out);
            return RetCode_OK;
        }
        return RetCode_BADCFG;
    }

    RetCode add_sinker(unsigned long &lnum,
                       str_tok &tknz,
                       const std::string &sinkername,
                       const std::string &aptype,
                       TraceLVL vlglvl) {
        std::shared_ptr<spdlog::sinks::sink> sinker;

        if(aptype == "console") {
            sinker = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        } else if(aptype == "file") {
            std::string apfile;
            RET_ON_KO(read_expected_token(lnum, CM, CM, true, tknz))
            RET_ON_KO(skip_sp_tab_and_read_tkn(lnum, nullptr, tknz, apfile))
            sinker = std::make_shared<spdlog::sinks::basic_file_sink_mt>(apfile.c_str(), true);
        }
        sinker->set_level(get_spdlevel(vlglvl));
        get_sinker_map()[sinkername] = sinker;
        return RetCode_OK;
    }

    RetCode parse_data(std::string &data) {
        RetCode res = RetCode_OK;
        unsigned long lnum = 1;
        std::string tkn, apname, aptype, lggrname, glob_logger_format_patter;
        TraceLVL vlglvl = TL_EVL;

        str_tok tknz(data);
        while(tknz.next_token(tkn, DF_DLM DT EQ, true)) {
            SKIP_SP_TAB(tkn)
            DO_CMD_ON_NL(tkn, lnum++; continue)
            if(tkn == TKSNK) { //sinker
                RET_ON_KO(read_expected_token(lnum, DT, DT, false, tknz))
                RET_ON_KO(read_tkn_no_nwl(lnum, DT EQ, tknz, apname))
                RET_ON_KO(read_expected_token(lnum, EQ, EQ, true, tknz))

                //sinker level
                RET_ON_KO(skip_sp_tab_and_read_tkn(lnum, CM, tknz, tkn))
                vlglvl = get_trace_level_enum(tkn.c_str());
                if(vlglvl == TL_EVL) {
                    LPrsERR_UNEXP(tkn.c_str());
                    return RetCode_BADCFG;
                }
                RET_ON_KO(read_expected_token(lnum, CM, CM, true, tknz))

                //sinker type
                RET_ON_KO(skip_sp_tab_and_read_tkn(lnum, CM, tknz, aptype))
                RET_ON_KO(add_sinker(lnum, tknz, apname, aptype, vlglvl))
            } else if(tkn == TKLGR) { //logger
                RET_ON_KO(read_expected_token(lnum, DT, DT, false, tknz))
                RET_ON_KO(read_tkn_no_nwl(lnum, DT EQ, tknz, lggrname))
                RET_ON_KO(read_expected_token(lnum, EQ, EQ, true, tknz))

                //logger level
                RET_ON_KO(skip_sp_tab_and_read_tkn(lnum, CM, tknz, tkn))
                vlglvl = get_trace_level_enum(tkn.c_str());
                if(vlglvl == TL_EVL) {
                    LPrsERR_UNEXP(tkn.c_str());
                    return RetCode_BADCFG;
                }

                //sinkers
                std::list<std::shared_ptr<spdlog::sinks::sink>> sinks_l;
                while(read_expected_token(lnum, CM, CM, true, tknz) == RetCode_OK) {
                    RET_ON_KO(skip_sp_tab_and_read_tkn(lnum, CM, tknz, tkn))
                    auto it = get_sinker_map().find(tkn);
                    if(it != get_sinker_map().end()) {
                        sinks_l.push_back(it->second);
                        continue;
                    } else {
                        LPrsERR_APNF(lggrname.c_str(), tkn.c_str());
                        return RetCode_BADCFG;
                    }
                }

                std::shared_ptr<spdlog::logger> lggr(new spdlog::logger(lggrname.c_str(), sinks_l.begin(), sinks_l.end()));
                lggr->set_level(get_spdlevel(vlglvl));
                spdlog::register_logger(lggr);
            } else if(tkn == TKGLOB) {  //GLOBAL
                RET_ON_KO(read_expected_token(lnum, DT, DT, false, tknz))
                RET_ON_KO(read_tkn_no_nwl(lnum, DT EQ, tknz, tkn))
                if(tkn == TKLGR) {
                    RET_ON_KO(read_expected_token(lnum, DT, DT, false, tknz))
                    RET_ON_KO(read_tkn_no_nwl(lnum, DT EQ, tknz, tkn))
                    if(tkn == TKPTTRN) {
                        RET_ON_KO(read_expected_token(lnum, EQ, EQ, true, tknz))
                        RET_ON_KO(read_tkn_until_nwl(lnum, tknz, glob_logger_format_patter))
                    } else {
                        LPrsERR_UNEXP(tkn.c_str());
                        return RetCode_BADCFG;
                    }
                } else {
                    LPrsERR_UNEXP(tkn.c_str());
                    return RetCode_BADCFG;
                }
            } else {
                LPrsERR_UNEXP(tkn.c_str());
                return RetCode_BADCFG;
            }
        }
        if(glob_logger_format_patter.length() > 0) {
            spdlog::set_pattern(glob_logger_format_patter);
        }
        spdlog::flush_every(std::chrono::seconds(5));

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
        get_sinker_map().clear();
    }
}

}
