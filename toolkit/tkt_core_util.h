/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "tkt_glob.h"

class vlg_toolkit_MainWindow;

namespace vlg_tlkt {

//-----------------------------------------------------------------------------
// QPlainTextEditApnd
//-----------------------------------------------------------------------------
class QPlainTextEditApnd : public QObject, public vlg::appender {
        Q_OBJECT

        //---ctors
    public:
        QPlainTextEditApnd(vlg_toolkit_MainWindow      *btmw);

        vlg_toolkit_MainWindow *btmw() const;
        void setBtmw(vlg_toolkit_MainWindow *btmw);


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
        vlg_toolkit_MainWindow      *btmw_;
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
