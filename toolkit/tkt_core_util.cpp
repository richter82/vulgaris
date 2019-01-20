/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#include "tkt_core_util.h"
#include "tkt_mainwindow.h"

#define LG_BUF_LEN_16K 16384

namespace vlg {
TraceLVL get_trace_level_enum(const char *str);
const char *get_trace_level_string(TraceLVL lvl);
}

std::string hexStr(const char *data, int len)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0; i<len; ++i) {
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return ss.str();
}

void FillQstring_FldValue(const char *fld_ptr,
                          const vlg::member_desc &mdesc,
                          QString &out)
{
    switch(mdesc.get_field_vlg_type()) {
        case vlg::Type_ENTITY: {
            if(mdesc.get_field_nentity_type() == vlg::NEntityType_NENUM) {
                int val = *(int *)fld_ptr;
                out = QString("%1").arg(val);
            }
        }
        break;
        case vlg::Type_BOOL: {
            bool val = *(const bool *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_INT16: {
            short val = *(const short *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_UINT16: {
            unsigned short val = *(const unsigned short *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_INT32: {
            int val = *(const int *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_UINT32: {
            unsigned int val = *(const unsigned int *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_INT64: {
            int64_t val = *(const int64_t *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_UINT64: {
            uint64_t val = *(const uint64_t *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_FLOAT32: {
            float val = *(const float *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_FLOAT64: {
            double val = *(const double *)fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_ASCII: {
            char val = *fld_ptr;
            out = QString("%1").arg(val);
        }
        break;
        case vlg::Type_BYTE: {
            out = QString::fromUtf8(fld_ptr, 1);
        }
        break;
        default:
            return;
    }
}

void FillFldValue_Qstring(const QVariant &value,
                          const vlg::member_desc &mdesc,
                          char *fld_ptr)
{
    switch(mdesc.get_field_vlg_type()) {
        case vlg::Type_ENTITY:
            if(mdesc.get_field_nentity_type() == vlg::NEntityType_NENUM) {
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
            if(mdesc.get_field_nmemb() > 1) {
                strncpy(fld_ptr, value.toString().toLatin1().data(), mdesc.get_field_nmemb());
            } else {
                *(char *)fld_ptr = value.toString().size() == 1 ? value.toString().toLatin1().at(0) : 0;
            }
            break;
        case vlg::Type_BYTE:
            memset(fld_ptr, 0, mdesc.get_field_nmemb());
#if defined WIN32 && defined _MSC_VER
            memcpy(fld_ptr, value.toByteArray().constData(), min((uint32_t)value.toByteArray().size(),
                                                                 (uint32_t)mdesc.get_field_nmemb()));
#else
            memcpy(fld_ptr, value.toByteArray().constData(), std::min((uint32_t)value.toByteArray().size(),
                                                                      (uint32_t)mdesc.get_field_nmemb()));
#endif
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
    btmw_(btmw)
{
    set_pattern("[%H:%M:%S:%e][%t][%^%l%$]%v");
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
