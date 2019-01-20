/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#pragma once
#include "tkt_glob.h"
#include <mutex>
class vlg_toolkit_MainWindow;

namespace vlg_tlkt {

//-----------------------------------------------------------------------------
// QPlainTextEditApnd
//-----------------------------------------------------------------------------
class QPlainTextEditApnd : public QObject, public spdlog::sinks::base_sink<std::mutex> {
        Q_OBJECT

        //---ctors
    public:
        QPlainTextEditApnd(vlg_toolkit_MainWindow *btmw);

        vlg_toolkit_MainWindow *btmw() const;
        void setBtmw(vlg_toolkit_MainWindow *btmw);


    signals:
        void messageReady(spdlog::level::level_enum tlvl, const QString &s);

    protected:
        void sink_it_(const spdlog::details::log_msg &msg) override {
            fmt::memory_buffer formatted;
            formatter_->format(msg, formatted);
            emit messageReady(msg.level, tr(fmt::to_string(formatted).c_str()));
        }

        void flush_() override {}

    private:
        vlg_toolkit_MainWindow *btmw_;
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
