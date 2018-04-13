/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#pragma warning (disable: 4100)

#include <sstream>
#include <iomanip>

#include <QtCore>
#include <QVector>

#include "vlg_logger.h"
#include "vlg_model.h"
#include "vlg_peer.h"
#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg_subscription.h"

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
                          const vlg::member_desc &mdesc,
                          QString &out);

void FillFldValue_Qstring(const QVariant &value,
                          const vlg::member_desc &mdesc,
                          char *fld_ptr);

struct VLG_SBS_COL_DATA_ENTRY;
