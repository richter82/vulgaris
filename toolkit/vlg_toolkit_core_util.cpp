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

#include "vlg_toolkit_core_util.h"
#include "vlg_toolkit_mainwindow.h"

#define LG_BUF_LEN_16K 16384

namespace vlg {
TraceLVL get_trace_level_enum(const char *str);
const char *get_trace_level_string(TraceLVL lvl);
}

void FillQstring_FldValue(const char *fld_ptr,
                          const vlg::member_desc *mdesc,
                          QString &out)
{
    switch(mdesc->get_field_vlg_type()) {
        case vlg::Type_ENTITY: {
            if(mdesc->get_field_nentity_type() == vlg::NEntityType_NENUM) {
                int val = *(int *)fld_ptr;
                out = QString("%1").arg(val);
            }
        }
        break;
        case vlg::Type_BOOL: {
            bool val = *(bool *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_INT16: {
            short val = *(short *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_UINT16: {
            unsigned short val = *(unsigned short *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_INT32: {
            int val = *(int *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_UINT32: {
            unsigned int val = *(unsigned int *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_INT64: {
            int64_t val = *(int64_t *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_UINT64: {
            uint64_t val = *(uint64_t *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_FLOAT32: {
            float val = *(float *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_FLOAT64: {
            double val = *(double *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_ASCII: {
            char val = *fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        default:
            return;
    }
}

void FillFldValue_Qstring(const QVariant &value,
                          const vlg::member_desc *mdesc,
                          char *fld_ptr)
{
    switch(mdesc->get_field_vlg_type()) {
        case vlg::Type_ENTITY:
            if(mdesc->get_field_nentity_type() == vlg::NEntityType_NENUM) {
                *(int *)fld_ptr = value.toInt();
            }
            break;
        case vlg::Type_BOOL:
            *(bool *)fld_ptr = value.toBool();
            break;
        case vlg::Type_INT16:
            *(short *)fld_ptr = value.toInt();
            break;
        case vlg::Type_UINT16:
            *(unsigned short *)fld_ptr = value.toUInt();
            break;
        case vlg::Type_INT32:
            *(int *)fld_ptr = value.toInt();
            break;
        case vlg::Type_UINT32:
            *(unsigned int *)fld_ptr = value.toUInt();
            break;
        case vlg::Type_INT64:
            *(int64_t *)fld_ptr = value.toLongLong();
            break;
        case vlg::Type_UINT64:
            *(uint64_t *)fld_ptr = value.toULongLong();
            break;
        case vlg::Type_FLOAT32:
            *(float *)fld_ptr = value.toFloat();
            break;
        case vlg::Type_FLOAT64:
            *(double *)fld_ptr = value.toDouble();
            break;
        case vlg::Type_ASCII:
            if(mdesc->get_field_nmemb() > 1) {
                strncpy(fld_ptr, value.toString().toLatin1().data(), mdesc->get_field_nmemb());
            } else {
                *(char *)fld_ptr = value.toChar().toLatin1();
            }
            break;
        default:
            break;
    }
}

namespace vlg_tlkt {

//-----------------------------------------------------------------------------
// QPlainTextEditApnd
//-----------------------------------------------------------------------------

QPlainTextEditApnd::QPlainTextEditApnd(vlg_toolkit_MainWindow *btmw) :
    appender(),
    btmw_(btmw)
{}

void QPlainTextEditApnd::flush()
{}

size_t QPlainTextEditApnd::put_msg(vlg::TraceLVL tlvl,
                                   const char *sign,
                                   uint16_t sign_len,
                                   uint32_t id,
                                   const char *msg)
{
    const char *lvl_str = vlg::get_trace_level_string(tlvl);
    char msg_b[LG_BUF_LEN_16K] = {0};
    uint16_t msg_b_idx = 0;
    memset(msg_b, 0, LG_BUF_LEN_16K);
    render_msg(lvl_str, sign, sign_len, id, msg, msg_b, &msg_b_idx);
    msg_b[msg_b_idx-1] = '\0';
    emit messageReady(tlvl, tr(msg_b));
    return msg_b_idx;
}

size_t QPlainTextEditApnd::put_msg_plain(const char *msg)
{
    char msg_b[LG_BUF_LEN_16K] = {0};
    uint16_t msg_b_idx = 0;
    memset(msg_b, 0, LG_BUF_LEN_16K);
    render_msg_pln(msg, msg_b, &msg_b_idx);
    msg_b[msg_b_idx-1] = '\0';
    emit messageReady(vlg::TL_PLN, tr(msg_b));
    return msg_b_idx;
}

size_t QPlainTextEditApnd::put_msg_va(vlg::TraceLVL tlvl,
                                      const char *sign,
                                      uint16_t sign_len,
                                      uint32_t id,
                                      const char *msg,
                                      va_list args)
{
    const char *lvl_str = vlg::get_trace_level_string(tlvl);
    char msg_b[LG_BUF_LEN_16K] = {0};
    uint16_t msg_b_idx = 0;
    render_msg_va(lvl_str, sign, sign_len, id, msg, msg_b, &msg_b_idx, args);
    msg_b[msg_b_idx-1] = '\0';
    emit messageReady(tlvl, tr(msg_b));
    return msg_b_idx;
}

size_t QPlainTextEditApnd::put_msg_va_plain(const char *msg, va_list args)
{
    char msg_b[LG_BUF_LEN_16K] = {0};
    uint16_t msg_b_idx = 0;
    render_msg_va_pln(msg, msg_b, &msg_b_idx, args);
    msg_b[msg_b_idx-1] = '\0';
    emit messageReady(vlg::TL_PLN, tr(msg_b));
    return msg_b_idx;
}

vlg_toolkit_MainWindow *QPlainTextEditApnd::btmw() const
{
    return btmw_;
}

void QPlainTextEditApnd::setBtmw(vlg_toolkit_MainWindow *btmw)
{
    btmw_ = btmw;
}

//-----------------------------------------------------------------------------
// QPlainTextEditApnd_THD
//-----------------------------------------------------------------------------
/*
QPlainTextEditApnd_THD::QPlainTextEditApnd_THD(QPlainTextEditApnd &apnd, QObject *parent) :
apnd_(apnd),
QThread(parent)
{}

void QPlainTextEditApnd_THD::run()
{
   char msg_b[LG_BUF_LEN_16K] = {0};
   while(1){
        apnd_.messages().Get(msg_b);
        emit messageReady(QString(msg_b));
        memset(msg_b, 0, LG_BUF_LEN_16K);
   }
}
*/


}
