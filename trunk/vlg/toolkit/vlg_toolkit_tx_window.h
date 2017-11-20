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

#ifndef VLG_TOOLKIT_TX_WINDOW_H
#define VLG_TOOLKIT_TX_WINDOW_H

#include "vlg_toolkit_vlg_model.h"
#include "vlg_toolkit_tx_vlg_class_model.h"

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
        explicit vlg_toolkit_tx_window(const vlg::entity_desc &edesc,
                                       const vlg::entity_manager &bem,
                                       vlg::transaction_impl &tx,
                                       vlg_toolkit_tx_vlg_class_model &mdl,
                                       QWidget *parent = 0);
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

        /*****
         REP
         ****/

    private:
        const vlg::entity_manager &bem_;
        vlg::transaction_impl &tx_;
        vlg_toolkit_tx_model tx_mdl_wrp_;


    private:
        Ui::vlg_toolkit_tx_window *ui;
};

#endif // VLG_TOOLKIT_TX_WINDOW_H
