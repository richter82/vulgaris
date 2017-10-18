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

#ifndef BLZ_TOOLKIT_CONNECTION_H
#define BLZ_TOOLKIT_CONNECTION_H

#include "blz_toolkit_tx_window.h"
#include "blz_toolkit_sbs_window.h"

//------------------------------------------------------------------------------
// blz_toolkit_Conn_mdl
//------------------------------------------------------------------------------

class blz_toolkit_Conn_mdl : public QSortFilterProxyModel {

    public:
        blz_toolkit_Conn_mdl(blz_toolkit_blz_model &wrapped_mdl, QObject *parent = 0);


        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

        blz_toolkit_blz_model &wrapped_mdl();

    private:
        blz_toolkit_blz_model &wrapped_mdl_;
};

//------------------------------------------------------------------------------
// blz_toolkit_Connection
//------------------------------------------------------------------------------

namespace Ui {
class blz_toolkit_Connection;
}

class blz_toolkit_Connection : public QWidget {
        Q_OBJECT

    public:
        explicit blz_toolkit_Connection(vlg::connection_int &conn,
                                        const QString &host,
                                        const QString &port,
                                        const QString &usr,
                                        const QString &psswd,
                                        blz_toolkit_blz_model &blz_model_loaded_model,
                                        QMainWindow &m_win,
                                        QWidget *parent = 0);
        ~blz_toolkit_Connection();

    public slots:
        void OnConnStatusChange(vlg::ConnectionStatus status);
        void OnTestSlot();
        void OnCustomMenuRequested(const QPoint &pos);

    signals:
        void SignalConnStatusChange(vlg::ConnectionStatus status);
        void SignalConnectionTimeout(const QString &msg);
        void SignalDisconnectionTimeout(const QString &msg);

    public:
        void EmitConnStatus(vlg::ConnectionStatus status);


        friend void blz_toolkit_connection_status_change_hndl(vlg::connection_int
                                                              &conn,
                                                              vlg::ConnectionStatus status,
                                                              void *ud);

    private:
        void UpdateTabHeader();
        void ConnectionUpActions();
        void ConnectionDownActions();

    public:
        static int count();
        static int NextCount();
        static void setCount(int count);

        int tab_id() const;
        void setTab_id(int tab_id);

        int tab_idx() const;
        void setTab_idx(int tab_idx);


        /*****
         REP
         ****/

    private:
        QMainWindow &m_win_;
        int tab_idx_; //in the QTabWidget
        int tab_id_;
        QTabWidget &parent_;
        vlg::connection_int &conn_;

    private:
        blz_toolkit_Conn_mdl b_mdl_;

    private:
        static int count_;

    public:
        Ui::blz_toolkit_Connection *ui;
    private slots:
        void on_connect_button_clicked();
        void on_disconnect_button_clicked();
        void on_extend_model_button_clicked();
        void on_new_tx_button_clicked();
        void on_new_sbs_button_clicked();
};

#endif // BLZ_TOOLKIT_CONNECTION_H
