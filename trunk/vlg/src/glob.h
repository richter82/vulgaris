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

#if defined WIN32 && defined _MSC_VER
#include <WS2tcpip.h>
#endif
#ifdef __GNUG__
#include <unistd.h>
#include <fcntl.h>
#endif

#include "vlg/timing.h"
#include "vlg/parse.h"
#include "vlg/dl.h"
#include "vlg/cfg.h"
#include "vlg/bbuf.h"
#include "vlg/concurr.h"
#include "vlg/proto.h"

#include "vlg_model.h"
#include "vlg_logger.h"
#include "vlg_peer.h"
#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg_subscription.h"

#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <list>
#include <algorithm>

#if defined WIN32 && defined _MSC_VER
#define __func__ __FUNCTION__
#endif
#ifdef __GNUG__
#define NOINLINE __attribute__ ((noinline))
#else
#define NOINLINE
#endif
#if defined WIN32 && defined _MSC_VER
#define TH_ID GetCurrentThreadId()
#else
#define TH_ID ((unsigned int)((unsigned long)pthread_self()))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define CR_MAX_SRC_LINE_LEN 1024
#define VLG_TK_COMMENT      "#"
#define VLG_MDL_NAME_LEN    256

#define LS_WEL "WL|"
#define LS_EMP "  |"
#define LS_CTR "-C|%s"
#define LS_DTR "~D|%s"
#define LS_OPN "->|%s "
#define LS_CLO "<-|%s "
#define LS_TRL "--|%s "
#define LS_EXE "EX|%s "
#define LS_PRS "PS|%s "
#define LS_SEL "SL|"
#define LS_OUT ">>|"
#define LS_INC "<<|"
#define LS_TRX "TX|"
#define LS_TXO "T>|"
#define LS_TXI "T<|"
#define LS_STM "SM|%s "
#define LS_QRY "QR|%s "
#define LS_APL "AP|"
#define LS_PAR "PA|"
#define LS_MDL "MD|%s "
#define LS_DRV "DV|"
#define LS_CON "CN|"
#define LS_SBS "SB|"
#define LS_SBO "S>|"
#define LS_SBI "S<|"

#define LS_EXUNX    " @EXPECT THE UNEXPECTED! "

#if defined WIN32 && defined _MSC_VER
#define snprintf sprintf_s
#endif

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
#define EXIT_ACTION \
{\
    FILE *ferr = fopen("log.err", "w+");\
    fprintf(ferr ? ferr : stderr, "EXIT TRIGGERED AT: %s-%d\n", __func__, __LINE__);\
    if(ferr) fclose(ferr);\
    exit(1);\
}

namespace vlg {
void *grow_buff_or_die(void *buffer,
                       size_t current_size,
                       size_t amount);

const extern std_shared_ptr_obj_mng<nclass> ncls_std_shp_omng;
const extern std_shared_ptr_obj_mng<incoming_connection> conn_std_shp_omng;
const extern std_shared_ptr_obj_mng<incoming_transaction> tx_std_shp_omng;
const extern std_shared_ptr_obj_mng<incoming_subscription> sbs_std_shp_omng;
const extern std_shared_ptr_obj_mng<subscription_event> sbse_std_shp_omng;

extern logger *v_log_;

#define IFLOG(meth)\
if(vlg::v_log_){\
    vlg::v_log_->meth;\
}

#define CTOR_TRC IFLOG(trc(TH_ID, LS_CTR "[%p]", __func__, this))
#define DTOR_TRC IFLOG(trc(TH_ID, LS_DTR "[%p]", __func__, this))

}


#endif
