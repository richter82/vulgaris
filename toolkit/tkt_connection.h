/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#pragma once
#include "tkt_tx_window.h"
#include "tkt_sbs_window.h"

//------------------------------------------------------------------------------
// vlg_toolkit_Conn_mdl
//------------------------------------------------------------------------------

class vlg_toolkit_Conn_mdl : public QSortFilterProxyModel {

    public:
        vlg_toolkit_Conn_mdl(vlg_toolkit_vlg_model &wrapped_mdl, QObject *parent = 0);


        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

        vlg_toolkit_vlg_model &wrapped_mdl();

    private:
        vlg_toolkit_vlg_model &wrapped_mdl_;
};

//------------------------------------------------------------------------------
// vlg_toolkit_Connection
//------------------------------------------------------------------------------
class vlg_toolkit_Connection;
class toolkit_connection : public vlg::outgoing_connection {
    public:
        toolkit_connection(vlg_toolkit_Connection &widget);

        vlg_toolkit_Connection &widget_;
};

//------------------------------------------------------------------------------
// vlg_toolkit_Connection
//------------------------------------------------------------------------------

namespace Ui {
class vlg_toolkit_Connection;
}

class vlg_toolkit_Connection : public QWidget {
        Q_OBJECT

    public:
        explicit vlg_toolkit_Connection(vlg::broker &broker,
                                        const QString &host,
                                        const QString &port,
                                        const QString &usr,
                                        const QString &psswd,
                                        vlg_toolkit_vlg_model &vlg_model_loaded_model,
                                        QMainWindow &m_win,
                                        QWidget *parent = 0);
        ~vlg_toolkit_Connection();

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


        friend void vlg_toolkit_connection_status_change_hndl(vlg::outgoing_connection &conn,
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

    private:
        QMainWindow &m_win_;
        int tab_idx_; //in the QTabWidget
        int tab_id_;
        QTabWidget &parent_;
        toolkit_connection conn_;

    private:
        vlg_toolkit_Conn_mdl b_mdl_;

    private:
        static int count_;

    public:
        Ui::vlg_toolkit_Connection *ui;
        toolkit_connection &conn();

    private slots:
        void on_connect_button_clicked();
        void on_disconnect_button_clicked();
        void on_extend_model_button_clicked();
        void on_new_tx_button_clicked();
        void on_new_sbs_button_clicked();
};
