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

#ifndef VLG_DRV_SQLITE_H_
#define VLG_DRV_SQLITE_H_
#include "vlg.h"

#if defined WIN32 && defined _MSC_VER
#ifdef VLG_PERS_DRIV_EXPORTS
#define EXP_SYM __declspec(dllexport)
#else
#ifdef VLG_PERS_DRIV_IMPORTS
#define EXP_SYM __declspec(dllimport)
#else
#define EXP_SYM
#endif
#endif
#else
#ifdef __linux
#define EXP_SYM
#endif
#if defined (__MACH__) || defined (__APPLE__)
#define EXP_SYM __attribute__((visibility("default")))
#endif
#endif

#define VLG_PERS_DRIV_SQLITE_ID     101
#define VLG_PERS_DRIV_SQLITE_NAME   "sqlite"

#if defined(__cplusplus)
namespace vlg {
#endif

#if defined(__cplusplus)
struct persistence_driver;
#else
typedef struct persistence_driver persistence_driver;
#endif

#if defined(__cplusplus)
extern "C" {
#endif
EXP_SYM const char *get_pers_driv_ver_sqlite();
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
extern "C" {
#endif
EXP_SYM RetCode set_db_data_dir_sqlite(const char *dir);
#if defined(__cplusplus)
}
#endif

/*******************************
VLG_PERS_DRIV_SQLITE ENTRY POINT
*******************************/
#if defined(__cplusplus)
extern "C" {
#endif
EXP_SYM persistence_driver *get_pers_driv_sqlite(logger *log);
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
}
#endif
#endif

