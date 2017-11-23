/*
 *
 * (C) 2015 - giuseppe.baccini@gmail.com
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

#include "vlg_toolkit_peer.h"

namespace vlg_tlkt {

unsigned int VLG_TOOLKIT_PEER_VER[] = {0,0,0,0};

//------------------------------------------------------------------------------
// ****VLG_TOOLKIT_PEER****
//------------------------------------------------------------------------------
toolkit_peer::toolkit_peer()
{}

//------------------------------------------------------------------------------
// VLG_PEER_LFCYC ABS METHS
//------------------------------------------------------------------------------

const char *toolkit_peer::name_handler()
{
    return "toolkit_peer[" __DATE__ "]";
}

const unsigned int *toolkit_peer::version_handler()
{
    return VLG_TOOLKIT_PEER_VER;
}

}
