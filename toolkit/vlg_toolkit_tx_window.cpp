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

#include "vlg_toolkit_tx_window.h"
#include "ui_vlg_toolkit_tx_window.h"

//------------------------------------------------------------------------------
// vlg_toolkit_tx_model
//------------------------------------------------------------------------------

vlg_toolkit_tx_model::vlg_toolkit_tx_model(vlg_toolkit_tx_vlg_class_model
                                           &wrapped_mdl, QObject *parent) :
    wrapped_mdl_(wrapped_mdl),
    QSortFilterProxyModel(parent)
{
    setSourceModel(&wrapped_mdl);
}

bool vlg_toolkit_tx_model::filterAcceptsRow(int sourceRow,
                                            const QModelIndex &sourceParent) const
{
    return true;
}

vlg_toolkit_tx_vlg_class_model &vlg_toolkit_tx_model::wrapped_mdl()
{
    return wrapped_mdl_;
}

//------------------------------------------------------------------------------
// vlg_toolkit_tx_window
//------------------------------------------------------------------------------

void tx_status_change_hndlr(vlg::transaction_impl &trans,
                            vlg::TransactionStatus status,
                            void *ud)
{
    vlg_toolkit_tx_window *txw = (vlg_toolkit_tx_window *)ud;
    qDebug() << "tx status:" << status;
    txw->EmitTxStatus(status);
}

void tx_closure_hndlr(vlg::transaction_impl &trans, void *ud)
{
    vlg_toolkit_tx_window *txw = (vlg_toolkit_tx_window *)ud;
    txw->EmitTxClosure();
}


vlg_toolkit_tx_window::vlg_toolkit_tx_window(const vlg::entity_desc &edesc,
                                             const vlg::entity_manager &bem,
                                             vlg::transaction_impl &tx,
                                             vlg_toolkit_tx_vlg_class_model &mdl,
                                             QWidget *parent) :
    bem_(bem),
    tx_(tx),
    tx_mdl_wrp_(mdl),
    QMainWindow(parent),
    ui(new Ui::vlg_toolkit_tx_window)
{
    ui->setupUi(this);
    ui->vlg_class_tx_table_view->setModel(&tx_mdl_wrp_);
    ui->connid_label_disp->setText(QString("%1").arg(
                                       tx_.get_connection().connid()));
    TxClosedActions();

    tx_.get_collector().retain(&tx_);

    connect(this,
            SIGNAL(SignalTxStatusChange(vlg::TransactionStatus)),
            this,
            SLOT(OnTxStatusChange(vlg::TransactionStatus)));

    connect(this,
            SIGNAL(SignalTxClosure()),
            this,
            SLOT(OnTxClosure()));

    EmitTxStatus(tx_.status());
    tx_.set_transaction_status_change_handler(tx_status_change_hndlr, this);
    tx_.set_transaction_closure_handler(tx_closure_hndlr, this);
}

vlg_toolkit_tx_window::~vlg_toolkit_tx_window()
{
    qDebug() << "~vlg_toolkit_tx_window()";
    vlg::TransactionStatus current = vlg::TransactionStatus_UNDEFINED;
    tx_.set_transaction_status_change_handler(NULL, NULL);
    tx_.set_transaction_closure_handler(NULL, NULL);
    tx_.get_connection().release_transaction(&tx_);

    vlg::collector &c = tx_.get_collector();
    c.release(&tx_);

    delete ui;
}

void vlg_toolkit_tx_window::closeEvent(QCloseEvent *event)
{
}

void vlg_toolkit_tx_window::OnTxStatusChange(vlg::TransactionStatus
                                             status)
{
    switch(status) {
        case vlg::TransactionStatus_UNDEFINED:
            ui->tx_status_label_disp->setText(QObject::tr("UNDEFINED"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case vlg::TransactionStatus_EARLY:
            ui->tx_status_label_disp->setText(QObject::tr("EARLY"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case vlg::TransactionStatus_INITIALIZED:
            ui->tx_status_label_disp->setText(QObject::tr("INITIALIZED"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : IndianRed; color : black;"));
            break;
        case vlg::TransactionStatus_PREPARED:
            ui->tx_status_label_disp->setText(QObject::tr("PREPARED"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : RosyBrown; color : black;"));
            ui->txid_label_disp->setText(tr("[%1][%2][%3][%4]").arg(tx_.tx_id_PLID()).arg(
                                             tx_.tx_id_SVID()).arg(
                                             tx_.tx_id_CNID()).arg(tx_.tx_id_PRID()));
            break;
        case vlg::TransactionStatus_FLYING:
            ui->tx_status_label_disp->setText(QObject::tr("FLYING"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LightGreen; color : black;"));
            break;
        case vlg::TransactionStatus_CLOSED:
            ui->tx_status_label_disp->setText(QObject::tr("CLOSED"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LawnGreen; color : black;"));
            break;
        case vlg::TransactionStatus_ERROR:
            ui->tx_status_label_disp->setText(QObject::tr("ERROR"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        default:
            break;
    }
}

void vlg_toolkit_tx_window::OnTxClosure()
{
    switch(tx_.tx_res()) {
        case vlg::TransactionResult_COMMITTED:
            ui->connid_tx_res_disp->setText(QObject::tr("COMMITTED"));
            ui->connid_tx_res_disp->setStyleSheet(
                QObject::tr("background-color : LightGreen; color : black;"));
            break;
        case vlg::TransactionResult_FAILED:
            ui->connid_tx_res_disp->setText(QObject::tr("FAILED"));
            ui->connid_tx_res_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        case vlg::TransactionResult_ABORTED:
            ui->connid_tx_res_disp->setText(QObject::tr("ABORTED"));
            ui->connid_tx_res_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        default:
            break;
    }
}

void vlg_toolkit_tx_window::on_actionSend_TX_triggered()
{
    if(tx_.status() != vlg::TransactionStatus_INITIALIZED) {
        tx_.re_new();
    }

    switch(ui->cfg_tx_reqtype_cb->currentIndex()) {
        case 0:
            tx_.set_tx_req_type(vlg::TransactionRequestType_RESERVED);
            break;
        case 1:
            tx_.set_tx_req_type(vlg::TransactionRequestType_SYSTEM);
            break;
        case 2:
            tx_.set_tx_req_type(vlg::TransactionRequestType_SPECIAL);
            break;
        case 3:
            tx_.set_tx_req_type(vlg::TransactionRequestType_OBJECT);
            break;
        default:
            break;
    }

    switch(ui->cfg_act_cb->currentIndex()) {
        case 0:
            tx_.set_tx_act(vlg::Action_INSERT);
            break;
        case 1:
            tx_.set_tx_act(vlg::Action_UPDATE);
            break;
        case 2:
            tx_.set_tx_act(vlg::Action_DELTA);
            break;
        case 3:
            tx_.set_tx_act(vlg::Action_DELETE);
            break;
        case 4:
            tx_.set_tx_act(vlg::Action_REMOVE);
            break;
        case 5:
            tx_.set_tx_act(vlg::Action_RESET);
            break;
        default:
            break;
    }

    switch(ui->cfg_class_encode_cb->currentIndex()) {
        case 0:
            tx_.set_tx_req_class_encode(vlg::Encode_INDEXED_NOT_ZERO);
            break;
        case 1:
            tx_.set_tx_req_class_encode(vlg::Encode_INDEXED_DELTA);
            break;
        default:
            break;
    }

    tx_.set_request_obj(tx_mdl_wrp_.wrapped_mdl().local_obj());
    tx_.set_result_class_req(ui->cfg_res_class_req_cb->isChecked());

    tx_.prepare();
    tx_.send();
}

void vlg_toolkit_tx_window::on_actionReNew_TX_triggered()
{
    if(tx_.status() != vlg::TransactionStatus_INITIALIZED) {
        tx_.re_new();
    }
}

void vlg_toolkit_tx_window::EmitTxStatus(vlg::TransactionStatus status)
{
    emit SignalTxStatusChange(status);
}

void vlg_toolkit_tx_window::EmitTxClosure()
{
    emit SignalTxClosure();
}

void vlg_toolkit_tx_window::TxFlyingActions()
{

}

void vlg_toolkit_tx_window::TxClosedActions()
{

}
