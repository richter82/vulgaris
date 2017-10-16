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

#include "blaze_c_pers_driv_sqlite.h"
#include "blz_drv_sqlite.h"

namespace blaze {
extern "C" {

    persistence_driver_int_wr persistence_driver_sqlite_get_instance()
    {
        return (persistence_driver_int_wr)get_pers_driv_sqlite();
    }

    const char *persistence_driver_sqlite_get_version()
    {
        return get_pers_driv_ver_sqlite();
    }

    const char *persistence_driver_sqlite_get_name()
    {
        return BLZ_PERS_DRIV_SQLITE_NAME;
    }

}
}

