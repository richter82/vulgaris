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

#ifndef VLG_C_LOG_H_
#define VLG_C_LOG_H_
#include "vlg.h"

#if defined(__cplusplus)
using namespace vlg;
extern "C" {
#endif

RetCode logger_set_logger_cfg_file_dir(const char *dir);
RetCode logger_set_logger_cfg_file_path_name(const char *file_path);
RetCode logger_load_logger_config();
RetCode logger_load_logger_config_by_name(const char *fname);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
using namespace vlg;
extern "C" {
#endif

size_t log_pln(vlg_logger_wr l,
               const char *fmt,
               ...);

size_t log_trc(vlg_logger_wr l,
               uint32_t id,
               const char *fmt,
               ...);

size_t log_dbg(vlg_logger_wr l,
               uint32_t id,
               const char *fmt,
               ...);

size_t log_inf(vlg_logger_wr l,
               uint32_t id,
               const char *fmt,
               ...);

size_t log_wrn(vlg_logger_wr l,
               uint32_t id,
               const char *fmt,
               ...);

size_t log_err(vlg_logger_wr l,
               uint32_t id,
               const char *fmt,
               ...);

size_t log_cri(vlg_logger_wr l,
               uint32_t id,
               const char *fmt,
               ...);

size_t log_fat(vlg_logger_wr l,
               uint32_t id,
               const char *fmt,
               ...);

//CLASS SPECIFIC

size_t log_trc_class(vlg_logger_wr l,
                     uint32_t id,
                     const net_class_wr obj,
                     int print_class_name,
                     const char *fmt,
                     ...);

size_t log_dbg_class(vlg_logger_wr l,
                     uint32_t id,
                     const net_class_wr obj,
                     int print_class_name,
                     const char *fmt,
                     ...);

size_t log_inf_class(vlg_logger_wr l,
                     uint32_t id,
                     const net_class_wr obj,
                     int print_class_name,
                     const char *fmt,
                     ...);

size_t log_wrn_class(vlg_logger_wr l,
                     uint32_t id,
                     const net_class_wr obj,
                     int print_class_name,
                     const char *fmt,
                     ...);

size_t log_err_class(vlg_logger_wr l,
                     uint32_t id,
                     const net_class_wr obj,
                     int print_class_name,
                     const char *fmt,
                     ...);

size_t log_cri_class(vlg_logger_wr l,
                     uint32_t id,
                     const net_class_wr obj,
                     int print_class_name,
                     const char *fmt,
                     ...);

size_t log_fat_class(vlg_logger_wr l,
                     uint32_t id,
                     const net_class_wr obj,
                     int print_class_name,
                     const char *fmt,
                     ...);

vlg_logger_wr get_vlg_logger_wr(const char *logger_name);

#if defined(__cplusplus)
}
#endif

#endif