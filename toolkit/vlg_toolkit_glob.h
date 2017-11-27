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

#ifndef VLG_TOOLKIT_GLOB_H
#define VLG_TOOLKIT_GLOB_H

#include <QtCore>
#include <QVector>

#include "vlg_logger.h"
#include "vlg_model.h"
#include "vlg_peer.h"
#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg_subscription.h"
#include "cr_structs_mt.h"
#include "cr_ascii_string.h"

#include <QMainWindow>
#include <QtGui>
#include <QtWidgets>

#define KEY_WINDOW      "vlg_toolkit_window"
#define KEY_POSITION    "position"
#define KEY_SIZE        "size"

#define GEN_HDR_VAL_BUFF 256
#define GEN_HDR_FIDX_BUFF 16

//-----------------------------------------------------------------------------
// INTERNAL TIMEOUT VALUE FOR AWAIT OPERATIONS (SECONDS)
//-----------------------------------------------------------------------------
#define VLG_TKT_INT_AWT_TIMEOUT 4
#define VLG_TKT_INT_REPT_SHOT_TIMER_CAPTMSG_FLAS_MSEC 150
#define VLG_TKT_INT_SINGL_SHOT_TIMER_CAPTMSG_RST_MSEC 4000

void FillQstring_FldValue(const char *fld_ptr,
                          const vlg::member_desc *mdesc,
                          QString &out);

void FillFldValue_Qstring(const QVariant &value,
                          const vlg::member_desc *mdesc,
                          char *fld_ptr);

//-----------------------------------------------------------------------------
// entity_desc_impl, partial
//-----------------------------------------------------------------------------
namespace vlg {
class nentity_desc_impl {
    public:
        const vlg::hash_map &GetMap_NM_MMBRDSC() const;
        const vlg::hash_map &GetMap_KEYID_KDESC() const;
};
}

struct VLG_SBS_COL_DATA_ENTRY;

#endif // VLG_TOOLKIT_GLOB_H
