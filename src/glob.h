/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once

#if defined WIN32 && defined _MSC_VER
#include <WS2tcpip.h>
#undef min
#endif
#ifdef __GNUG__
#include <unistd.h>
#include <fcntl.h>
#endif

#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include "vlg/timing.h"
#include "vlg/parse.h"
#include "vlg/dl.h"
#include "vlg/cfg.h"
#include "vlg/bbuf.h"
#include "vlg/concurr.h"
#include "vlg/proto.h"

#include "vlg_model.h"
#include "vlg_peer.h"
#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg_subscription.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
//#include "spdlog/sinks/rotating_file_sink.h"
//#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/fmt/ostr.h"

#if defined WIN32 && defined _MSC_VER
#define __func__ __FUNCTION__
#endif
#ifdef __GNUG__
#define NOINLINE __attribute__ ((noinline))
#else
#define NOINLINE
#endif

#define CR_MAX_SRC_LINE_LEN 1024
#define VLG_TK_COMMENT      "#"
#define VLG_MDL_NAME_LEN    256

#define RCV_SND_BUF_SZ 8192

#define LS_WEL "WL|"
#define LS_EMP "  |"
#define LS_SYS "SY|"
#define LS_CTR "-C|{}"
#define LS_DTR "~D|{}"
#define LS_OPN "->|{} "
#define LS_CLO "<-|{} "
#define LS_TRL "--|{} "
#define LS_EXE "EX|{} "
#define LS_PRS "PS|{} "
#define LS_SEL "SL|"
#define LS_OUT ">>|"
#define LS_INC "<<|"
#define LS_TRX "TX|"
#define LS_TXO "T>|"
#define LS_TXI "T<|"
#define LS_STM "SM|{} "
#define LS_QRY "QR|{} "
#define LS_APL "AP|"
#define LS_PAR "PA|"
#define LS_MDL "MD|{} "
#define LS_DRV "DV|"
#define LS_CON "CN|"
#define LS_SBS "SB|"
#define LS_SBO "S>|"
#define LS_SBI "S<|"

#define LS_EXUNX " @EXPECT THE UNEXPECTED! "

#define RET_ON_KO(fun)\
{\
    RetCode res;\
    if((res = fun)){\
        return res;\
    }\
}

#define CMD_ON_OK(fun, cmd)\
{\
    int res;\
    if(!(res = fun)){\
        cmd;\
    }\
}

#define CMD_ON_KO(fun, cmd)\
{\
    int res;\
    if((res = fun)){\
        cmd;\
    }\
}

#define CMD_ON_NUL(ptr, cmd)\
{\
    if(!(ptr)){\
        cmd;\
    }\
}

#define NO_ACTION ;
#define VLGDEFLOG "vlglog"

namespace vlg {

const extern std_shared_ptr_obj_mng<nclass> ncls_std_shp_omng;
const extern std_shared_ptr_obj_mng<incoming_connection> conn_std_shp_omng;
const extern std_shared_ptr_obj_mng<incoming_transaction> tx_std_shp_omng;
const extern std_shared_ptr_obj_mng<incoming_subscription> sbs_std_shp_omng;
const extern std_shared_ptr_obj_mng<subscription_event> sbse_std_shp_omng;

std::shared_ptr<spdlog::logger> get_logger(const char *logger_name);

#define IFLOG(log, meth) if(log) log->meth;
#define DTOR_TRC(log) IFLOG(log, trace(LS_DTR, __func__))

struct spdlog_nclass_type {
    nclass &nc;
    template<typename OStream>
    friend OStream &operator<<(OStream &os, const spdlog_nclass_type &c) {
        c.nc.to_string(os);
        return os;
    }
};

inline void *grow_buff_or_die(void *buffer, size_t current_size, size_t amount)
{
    void *nout = nullptr;
    if(buffer) {
        CMD_ON_NUL(nout = realloc(buffer, current_size + amount), exit(1))
    } else {
        CMD_ON_NUL(nout = malloc(amount), exit(1))
    }
    return nout;
}

inline void assign_hex_str(const char *data, size_t len, std::string &assign_to)
{
    std::stringstream ss;
    ss << std::hex;
    for(size_t i = 0; i<len; ++i) {
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    assign_to.assign(ss.str());
}

inline void append_hex_str(const char *data, size_t len, std::string &append_to)
{
    std::stringstream ss;
    ss << std::hex;
    for(size_t i = 0; i<len; ++i) {
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    append_to.append(ss.str());
}

}
