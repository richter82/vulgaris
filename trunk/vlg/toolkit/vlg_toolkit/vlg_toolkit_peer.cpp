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
toolkit_peer::toolkit_peer(unsigned int id) : peer_int(id)
{}

//------------------------------------------------------------------------------
// VLG_PEER_LFCYC ABS METHS
//------------------------------------------------------------------------------

vlg::RetCode toolkit_peer::peer_load_cfg_usr(int pnum, const char *param,
                                             const char *value)
{
    return peer_int::peer_load_cfg_usr(pnum, param, value);
}

vlg::RetCode toolkit_peer::peer_init_usr()
{
    IFLOG(dbg(TH_ID, LS_APL"[CALLED VLG_TOOLKIT_PEER APPLINIT HNDL]"))
    return peer_int::peer_init_usr();
}

vlg::RetCode toolkit_peer::peer_start_usr()
{
    IFLOG(dbg(TH_ID, LS_APL"[CALLED VLG_TOOLKIT_PEER APPLSTART HNDL]"))
    return peer_int::peer_start_usr();
}

vlg::RetCode toolkit_peer::peer_dying_breath_handler()
{
    IFLOG(dbg(TH_ID, LS_APL"[CALLED VLG_TOOLKIT_PEER DYINGBRTH HNDL]"))
    return peer_int::peer_dying_breath_handler();
}

const char *toolkit_peer::peer_name_usr()
{
    return "toolkit_peer[" __DATE__ "]";
}

const unsigned int *toolkit_peer::peer_ver_usr()
{
    return VLG_TOOLKIT_PEER_VER;
}

}
