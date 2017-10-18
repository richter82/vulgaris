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

#ifndef BLZ_C_PERS_DRIV_SQLITE_H_
#define BLZ_C_PERS_DRIV_SQLITE_H_
#include "vlg.h"

#if defined(__cplusplus)
using namespace vlg;
extern "C" {
#endif

persistence_driver_int_wr    persistence_driver_sqlite_get_instance();
const char                  *persistence_driver_sqlite_get_version();
const char                  *persistence_driver_sqlite_get_name();

#if defined(__cplusplus)
}
#endif

#endif