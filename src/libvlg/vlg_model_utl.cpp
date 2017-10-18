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

#include "blaze_logger.h"
#include "blaze_model.h"
#include "blz_glob_int.h"

#define LG_BUF_LEN_16K 16384

namespace vlg {
extern bool deflt_log_loaded;
}

namespace vlg {

vlg::hash_map *blz_lgname_lg = NULL;
vlg::hash_map &get_blz_logger_map()
{
    if(blz_lgname_lg) {
        return *blz_lgname_lg;
    }
    if(!(blz_lgname_lg = new vlg::hash_map(vlg::sngl_ptr_obj_mng(),
                                             vlg::sngl_cstr_obj_mng()))) {
        EXIT_ACTION("get_blz_logger_map() - failed create blz_lgname_lg map\n")
    }
    if(blz_lgname_lg->init(HM_SIZE_NANO)) {
        EXIT_ACTION("get_blz_logger_map() - failed init blz_lgname_lg map\n")
    }
    return *blz_lgname_lg;
}

nclass_logger *get_nclass_logger(const char *logger_name)
{
    if(!vlg::deflt_log_loaded) {
        COMMAND_IF_NOT_OK(vlg::logger::load_logger_config(), NO_ACTION)
        vlg::deflt_log_loaded = true;
    }
    nclass_logger *b_log = NULL;
    IF_RetCode_OK_CMD(get_blz_logger_map().get(logger_name, &b_log), return b_log)
    vlg::logger *log = NULL;
    vlg::logger::get_logger(logger_name, &log);
    if(!log) {
        COMMAND_IF_NOT_OK(vlg::logger::get_logger("root", &log), NO_ACTION)
    }
    if(log) {
        b_log = new nclass_logger();
        b_log->set_rep_from(*log);
        get_blz_logger_map().put(logger_name, &b_log);
        return b_log;
    }
    return NULL;
}

//-----------------------------
// BLZ_LOGGER
//-----------------------------

nclass_logger::nclass_logger()
{}

nclass_logger::~nclass_logger()
{}

size_t nclass_logger::trc_class(uint32_t id,
                                const nclass *obj,
                                bool print_class_name,
                                const char *msg,
                                ...)
{
    size_t blen = 0;
    char msg_b[LG_BUF_LEN_16K];
    if(level() <= vlg::TL_TRC) {
        va_list args;
        va_start(args, msg);
        blen = vsprintf(msg_b, msg, args);
        va_end(args);
        obj->pretty_dump_to_buffer(&msg_b[blen], print_class_name);
        blen = dbgm(id, msg_b);
    }
    return blen;
}


size_t nclass_logger::dbg_class(uint32_t id,
                                const nclass *obj,
                                bool print_class_name,
                                const char *msg,
                                ...)
{
    size_t blen = 0;
    char msg_b[LG_BUF_LEN_16K];
    if(level() <= vlg::TL_DBG) {
        va_list args;
        va_start(args, msg);
        blen = vsprintf(msg_b, msg, args);
        va_end(args);
        obj->pretty_dump_to_buffer(&msg_b[blen], print_class_name);
        blen = dbgm(id, msg_b);
    }
    return blen;
}

size_t nclass_logger::inf_class(uint32_t id,
                                const nclass *obj,
                                bool print_class_name,
                                const char *msg,
                                ...)
{
    size_t blen = 0;
    char msg_b[LG_BUF_LEN_16K];
    if(level() <= vlg::TL_INF) {
        va_list args;
        va_start(args, msg);
        blen = vsprintf(msg_b, msg, args);
        va_end(args);
        obj->pretty_dump_to_buffer(&msg_b[blen], print_class_name);
        blen = infm(id, msg_b);
    }
    return blen;
}

size_t nclass_logger::wrn_class(uint32_t id,
                                const nclass *obj,
                                bool print_class_name,
                                const char *msg,
                                ...)
{
    size_t blen = 0;
    char msg_b[LG_BUF_LEN_16K];
    if(level() <= vlg::TL_WRN) {
        va_list args;
        va_start(args, msg);
        blen = vsprintf(msg_b, msg, args);
        va_end(args);
        obj->pretty_dump_to_buffer(&msg_b[blen], print_class_name);
        blen = wrnm(id, msg_b);
    }
    return blen;
}

size_t nclass_logger::err_class(uint32_t id,
                                const nclass *obj,
                                bool print_class_name,
                                const char *msg,
                                ...)
{
    size_t blen = 0;
    char msg_b[LG_BUF_LEN_16K];
    if(level() <= vlg::TL_ERR) {
        va_list args;
        va_start(args, msg);
        blen = vsprintf(msg_b, msg, args);
        va_end(args);
        obj->pretty_dump_to_buffer(&msg_b[blen], print_class_name);
        blen = errm(id, msg_b);
    }
    return blen;
}

size_t nclass_logger::cri_class(uint32_t id,
                                const nclass *obj,
                                bool print_class_name,
                                const char *msg,
                                ...)
{
    size_t blen = 0;
    char msg_b[LG_BUF_LEN_16K];
    if(level() <= vlg::TL_CRI) {
        va_list args;
        va_start(args, msg);
        blen = vsprintf(msg_b, msg, args);
        va_end(args);
        obj->pretty_dump_to_buffer(&msg_b[blen], print_class_name);
        blen = crim(id, msg_b);
    }
    return blen;
}

size_t nclass_logger::fat_class(uint32_t id,
                                const nclass *obj,
                                bool print_class_name,
                                const char *msg,
                                ...)
{
    size_t blen = 0;
    char msg_b[LG_BUF_LEN_16K];
    if(level() <= vlg::TL_FAT) {
        va_list args;
        va_start(args, msg);
        blen = vsprintf(msg_b, msg, args);
        va_end(args);
        obj->pretty_dump_to_buffer(&msg_b[blen], print_class_name);
        blen = fatm(id, msg_b);
    }
    return blen;
}


}

