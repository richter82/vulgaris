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

/** @file cr_global.h
    @brief Main definition header for blaze-core library.
*/

/** @cond UNDOC */

#ifndef CR_GLOBAL_H_
#define CR_GLOBAL_H_

/** @endcond */

#if defined(__cplusplus)
namespace blaze {
#endif

/**
@return
*/
const char *get_arch(void);

/**
@return
*/
const char *crlib_ver(void);

#if defined(__cplusplus)
}
#endif

#endif
