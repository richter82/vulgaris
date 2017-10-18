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

#ifndef BLZ_TOOLKIT_TX_WINDOW_H
#define BLZ_TOOLKIT_TX_WINDOW_H

#include "blz_toolkit_blz_model.h"
#include "blz_toolkit_tx_blz_class_model.h"

namespace Ui {
class blz_toolkit_tx_window;
}

//------------------------------------------------------------------------------
// blz_toolkit_tx_model
//------------------------------------------------------------------------------

class blz_toolkit_tx_model : public QSortFilterProxyModel {

    public:
        blz_toolkit_tx_model(blz_toolkit_tx_blz_class_model &wrapped_mdl,
                             QObject *parent = 0);


        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

        blz_toolkit_tx_blz_class_model &wrapped_mdl();

    private:
        blz_toolkit_tx_blz_class_model &wrapped_mdl_;
};

//------------------------------------------------------------------------------
// blz_toolkit_tx_window
//------------------------------------------------------------------------------

class blz_toolkit_tx_window : public QMainWindow {
        Q_OBJECT

    public:
        explicit blz_toolkit_tx_window(const vlg::entity_desc &edesc,
                                       const vlg::entity_manager &bem,
                                       vlg::transaction_int &tx,
                                       blz_toolkit_tx_blz_class_model &mdl,
                                       QWidget *parent = 0);
        ~blz_toolkit_tx_window();

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
        vlg::transaction_int &tx_;
        blz_toolkit_tx_model tx_mdl_wrp_;


    private:
        Ui::blz_toolkit_tx_window *ui;
};

#endif // BLZ_TOOLKIT_TX_WINDOW_H
