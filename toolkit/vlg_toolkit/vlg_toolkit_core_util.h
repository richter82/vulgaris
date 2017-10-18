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

#ifndef BLZ_TOOLKIT_CORE_UTIL_H
#define BLZ_TOOLKIT_CORE_UTIL_H

#include "blz_toolkit_glob.h"

class blz_toolkit_MainWindow;

namespace blz_tlkt {

//-----------------------------------------------------------------------------
// QPlainTextEditApnd
//-----------------------------------------------------------------------------
class QPlainTextEditApnd : public QObject, public vlg::appender {
        Q_OBJECT

        //---ctors
    public:
        QPlainTextEditApnd(blz_toolkit_MainWindow      *btmw);

        blz_toolkit_MainWindow *btmw() const;
        void setBtmw(blz_toolkit_MainWindow *btmw);


    signals:
        void messageReady(vlg::TraceLVL tlvl, const QString &s);


    public:
        virtual void   flush();

        //---pub meths
    public:
        virtual size_t put_msg(vlg::TraceLVL tlvl,
                               const char *sign,
                               uint16_t sign_len,
                               uint32_t id,
                               const char *msg);

        virtual size_t put_msg_plain(const char *msg);

        virtual size_t put_msg_va(vlg::TraceLVL tlvl,
                                  const char *sign,
                                  uint16_t sign_len,
                                  uint32_t id,
                                  const char *msg,
                                  va_list args);

        virtual size_t put_msg_va_plain(const char *msg,
                                        va_list args);


    private:
        blz_toolkit_MainWindow      *btmw_;
};

//-----------------------------------------------------------------------------
// QPlainTextEditApnd_THD
//-----------------------------------------------------------------------------
/*
class QPlainTextEditApnd_THD : public QThread{
Q_OBJECT

public:
QPlainTextEditApnd_THD(QPlainTextEditApnd &apnd, QObject *parent = 0);

void run() Q_DECL_OVERRIDE;

signals:
void messageReady(const QString &s);

private:
QPlainTextEditApnd &apnd_;
};
*/

}

#endif // BLZ_TOOLKIT_CORE_UTIL_H
