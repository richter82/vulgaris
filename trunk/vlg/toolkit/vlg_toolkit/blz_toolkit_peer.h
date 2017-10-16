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

#ifndef BLZ_TOOLKIT_PEER_H
#define BLZ_TOOLKIT_PEER_H

#include "blz_toolkit_glob.h"

namespace blz_tlkt {

//------------------------------------------------------------------------------
// ****BLZ_TOOLKIT_PEER****
//------------------------------------------------------------------------------
class toolkit_peer : public blaze::peer_int {
    public:
        //---ctors
        toolkit_peer(unsigned int id);

        //------------------------------------------------------------------------------
        // BLZ_PEER_LFCYC ABS METHS
        //------------------------------------------------------------------------------
    private:
        virtual blaze::RetCode peer_load_cfg_usr(int pnum, const char *param,
                                                 const char *value);
        virtual blaze::RetCode peer_init_usr();
        virtual blaze::RetCode peer_start_usr();
        virtual blaze::RetCode peer_dying_breath_handler();
        virtual const char *peer_name_usr();
        virtual const unsigned int *peer_ver_usr();

};

}

#endif // BLZ_TOOLKIT_PEER_H
