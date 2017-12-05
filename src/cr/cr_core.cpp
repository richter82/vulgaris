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

#include "cr.h"

namespace vlg {


// #VER#
const char *crlib_ver(void)
{
    static char str[] = "lib.crlib.ver.0.0.0.date:" __DATE__;
    return str;
}


// GET ARCH
const char *get_arch()
{
#if !defined(_M_X64) && defined(WIN32)
    return "x86@NT";
#endif
#if defined(_M_X64) && defined(WIN32)
    return "x86_64@NT";
#endif
#if defined(__amd64__) && defined(__linux)
    return "x86_64@linux";
#endif
#if defined(__linux)
    return "x86_64@linux";
#endif
#if defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
    return "x86_64@ios_sim";
#elif TARGET_OS_IPHONE
    return "x86_64@ios";
#elif TARGET_OS_MAC
    return "x86_64@osx";
#endif
#endif
}

}


