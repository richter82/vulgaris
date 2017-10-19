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

#include "vlg_persistence.h"

#ifdef WIN32
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

namespace vlg {

/***************************
VLG_PERS_DRIV_SQLITE VERSION
***************************/
extern "C" {
    EXP_SYM const char *get_pers_driv_ver_sqlite();
}

/*******************************
VLG_PERS_DRIV_SQLITE ENTRY POINT
*******************************/
extern "C" {
    EXP_SYM persistence_driver_impl *get_pers_driv_sqlite();
}

}
