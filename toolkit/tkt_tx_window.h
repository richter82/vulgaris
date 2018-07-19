/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "tkt_vlg_model.h"
#include "tkt_tx_vlg_nclass_model.h"

//------------------------------------------------------------------------------
// toolkit_transaction
//------------------------------------------------------------------------------
class vlg_toolkit_tx_window;
class toolkit_transaction : public vlg::outgoing_transaction {
    public:
        toolkit_transaction(vlg_toolkit_tx_window &widget);

        vlg_toolkit_tx_window &widget_;
};

namespace Ui {
class vlg_toolkit_tx_window;
}

//------------------------------------------------------------------------------
// vlg_toolkit_tx_model
//------------------------------------------------------------------------------

class vlg_toolkit_tx_model : public QSortFilterProxyModel {

    public:
        vlg_toolkit_tx_model(vlg_toolkit_tx_vlg_class_model &wrapped_mdl,
                             QObject *parent = 0);


        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

        vlg_toolkit_tx_vlg_class_model &wrapped_mdl();

    private:
        vlg_toolkit_tx_vlg_class_model &wrapped_mdl_;
};

//------------------------------------------------------------------------------
// vlg_toolkit_tx_window
//------------------------------------------------------------------------------

class vlg_toolkit_tx_window : public QMainWindow {
        Q_OBJECT

    public:
        explicit vlg_toolkit_tx_window(vlg::outgoing_connection &conn,
                                       const vlg::nentity_desc &edesc,
                                       const vlg::nclass *opt_img = nullptr,
                                       QWidget *parent = nullptr);
        ~vlg_toolkit_tx_window();

    protected:
        void closeEvent(QCloseEvent *event);


    public slots:
        void OnTxStatusChange(vlg::TransactionStatus status);
        void OnTxClosure();

    private slots:
        void on_actionSend_TX_triggered();
        void on_actionReNew_TX_triggered();

    public:
        void EmitTxStatus(vlg::TransactionStatus status);
        void EmitTxClosure();

    signals:
        void SignalTxStatusChange(vlg::TransactionStatus status);
        void SignalTxClosure();


    private:
        void TxFlyingActions();
        void TxClosedActions();

    private:
        toolkit_transaction tx_;
        vlg_toolkit_tx_vlg_class_model tx_mdl_;
        vlg_toolkit_tx_model tx_mdl_wrp_;

    private:
        Ui::vlg_toolkit_tx_window *ui;
};
