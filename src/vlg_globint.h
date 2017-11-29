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

#ifndef VLG_GLOB_INT_H_
#define VLG_GLOB_INT_H_

#include "cr_time.h"
#include "cr_parse.h"
#include "cr_dl.h"
#include "cr_cfg.h"
#include "cr_structs_mt.h"
#include "cr_bbuf.h"

#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#ifdef __GNUG__
#define SOCKET int
#define INVALID_SOCKET (~0)
#define SOCKET_ERROR   (-1)
#endif

#if defined(_MSC_VER)
#define __func__ __FUNCTION__
#endif

#ifdef __GNUG__
#define NOINLINE __attribute__ ((noinline))
#else
#define NOINLINE
#endif

#if defined(_MSC_VER)
#define TH_ID GetCurrentThreadId()
#else
#define TH_ID ((unsigned int)((unsigned long)pthread_self()))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#if defined(_MSC_VER)
#define VLG_STRDUP _strdup
#else
#define VLG_STRDUP strdup
#endif

#define VLG_TK_COMMENT      "#"
#define VLG_MDL_NAME_LEN    256

#define LS_CTR "CTR|"
#define LS_DTR "~DR|"
#define LS_OPN "-->|"
#define LS_CLO "<--|"
#define LS_TRL "---|"
#define LS_EXE "EXE|"
#define LS_STA "STA|"
#define LS_WEL "WEL|"
#define LS_DIE "DIE|"
#define LS_MAN "MAN|"
#define LS_WSA "WSA|"
#define LS_RTM "RTM|"
#define LS_DUM "DUM|"
#define LS_PRS "PRS|"
#define LS_DMP "DMP|"
#define LS_PKT "PKT|"
#define LS_SEL "SEL|"
#define LS_OUT ">>>|"
#define LS_INC "<<<|"
#define LS_TRX "TRX|"
#define LS_TXT "TX-|"
#define LS_TXO "TX>|"
#define LS_TXI "TX<|"
#define LS_TXR "TXR|"
#define LS_STM "STM|"
#define LS_QRY "QRY|"
#define LS_APL "APL|"
#define LS_GDB "GDB|"
#define LS_PAR "PAR|"
#define LS_MDL "MDL|"
#define LS_DRV "DRV|"
#define LS_CON "CON|"
#define LS_SBS "SBS|"
#define LS_SBT "SB-|"
#define LS_SBO "SB>|"
#define LS_SBI "SB<|"

#define IFLOG(meth)\
if(log_){\
    log_->meth;\
}

#define IFLOG2(log, meth)\
if(log){\
    log->meth;\
}

#define LS_EXUNX    " @EXPECT THE UNEXPECTED! "
#define D_W_R_COLL  "OBJ WAS COLLECTED!"

#if defined(_MSC_VER)
#define snprintf sprintf_s
#endif

#define DECLINITH_GBB(bbuf, bsz)\
vlg::grow_byte_buffer *bbuf = new vlg::grow_byte_buffer();\
bbuf->init(bsz);

#define RETURN_IF_NOT_OK(fun)\
{\
    vlg::RetCode res;\
    if((res = fun)){\
        return res;\
    }\
}

#define IF_RetCode_OK_CMD(fun, cmd)\
{\
    int res;\
    if(!(res = fun)){\
        cmd;\
        }\
}

#define COMMAND_IF_NOT_OK(fun, cmd)\
{\
    int res;\
    if((res = fun)){\
        cmd;\
    }\
}

#define COMMAND_IF_NULL(ptr, cmd)\
{\
    if(!(ptr)){\
        cmd;\
    }\
}

#define NO_ACTION ;
#define EXIT_ACTION \
{\
    FILE *ferr = fopen("log.err", "w+");\
    fprintf(ferr ? ferr : stderr, "EXIT TRIGGERED AT: %s-%d\n", __func__, __LINE__);\
    if(ferr) fclose(ferr);\
    exit(1);\
}

#define RET_PTH_ERR_1(func, arg1)\
{\
    int pthres = 0;\
    if((pthres = func(arg1)))\
    return pthres;\
}

#define RET_PTH_ERR_2(func, arg1, arg2)\
{\
    int pthres = 0;\
    if((pthres = func(arg1, arg2)))\
    return pthres;\
}

#define CHK_MON_ERR_0(meth)\
{\
    int pthres = 0;\
    if((pthres = mon_.meth())){\
        return vlg::RetCode_PTHERR;\
    }\
}

#define CHK_PTH_ERR_1(func, arg1)\
{\
    int pthres = 0;\
    if((pthres = func(arg1))){\
        return  vlg::RetCode_PTHERR;\
        }\
}

#define CHK_PTH_ERR_2(func, arg1, arg2)\
{\
    int pthres = 0;\
    if((pthres = func(arg1, arg2))){\
        return  vlg::RetCode_PTHERR;\
    }\
}

#define CHK_CUST_MON_ERR(mon, meth)\
{\
    int pthres = 0;\
    if((pthres = mon.meth())){\
        return  vlg::RetCode_PTHERR;\
    }\
}

namespace vlg {
/**
@param buffer
@param current_size
@param amount
@return
*/
void *grow_buff_or_die(void *buffer,
                       size_t current_size,
                       size_t amount);


/** @brief collector_stat class.
*/
class collector;
class collector_stat_impl;
class collector_stat {
    public:
        static collector_stat &get_instance();

    public:
        explicit collector_stat();
        ~collector_stat();

    public:
        RetCode add_collector(collector &coll);
        RetCode start_monitoring(unsigned int freq_sec,
                                 TraceLVL at_level);

        RetCode stop_monitoring();

    private:
        collector_stat_impl *impl_;
};

}


#endif
