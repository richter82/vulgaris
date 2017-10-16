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

#include "blz_toolkit_tx_window.h"
#include "ui_blz_toolkit_tx_window.h"

//------------------------------------------------------------------------------
// blz_toolkit_tx_model
//------------------------------------------------------------------------------

blz_toolkit_tx_model::blz_toolkit_tx_model(blz_toolkit_tx_blz_class_model
                                           &wrapped_mdl, QObject *parent) :
    wrapped_mdl_(wrapped_mdl),
    QSortFilterProxyModel(parent)
{
    setSourceModel(&wrapped_mdl);
}

bool blz_toolkit_tx_model::filterAcceptsRow(int sourceRow,
                                            const QModelIndex &sourceParent) const
{
    return true;
}

blz_toolkit_tx_blz_class_model &blz_toolkit_tx_model::wrapped_mdl()
{
    return wrapped_mdl_;
}

//------------------------------------------------------------------------------
// blz_toolkit_tx_window
//------------------------------------------------------------------------------

void tx_status_change_hndlr(blaze::transaction_int &trans,
                            blaze::TransactionStatus status,
                            void *ud)
{
    blz_toolkit_tx_window *txw = (blz_toolkit_tx_window *)ud;
    qDebug() << "tx status:" << status;
    txw->EmitTxStatus(status);
}

void tx_closure_hndlr(blaze::transaction_int &trans, void *ud)
{
    blz_toolkit_tx_window *txw = (blz_toolkit_tx_window *)ud;
    txw->EmitTxClosure();
}


blz_toolkit_tx_window::blz_toolkit_tx_window(const blaze::entity_desc &edesc,
                                             const blaze::entity_manager &bem,
                                             blaze::transaction_int &tx,
                                             blz_toolkit_tx_blz_class_model &mdl,
                                             QWidget *parent) :
    bem_(bem),
    tx_(tx),
    tx_mdl_wrp_(mdl),
    QMainWindow(parent),
    ui(new Ui::blz_toolkit_tx_window)
{
    ui->setupUi(this);
    ui->blz_class_tx_table_view->setModel(&tx_mdl_wrp_);
    ui->connid_label_disp->setText(QString("%1").arg(
                                       tx_.get_connection().connid()));
    TxClosedActions();

    tx_.get_collector().retain(&tx_);

    connect(this,
            SIGNAL(SignalTxStatusChange(blaze::TransactionStatus)),
            this,
            SLOT(OnTxStatusChange(blaze::TransactionStatus)));

    connect(this,
            SIGNAL(SignalTxClosure()),
            this,
            SLOT(OnTxClosure()));

    EmitTxStatus(tx_.status());
    tx_.set_transaction_status_change_handler(tx_status_change_hndlr, this);
    tx_.set_transaction_closure_handler(tx_closure_hndlr, this);
}

blz_toolkit_tx_window::~blz_toolkit_tx_window()
{
    qDebug() << "~blz_toolkit_tx_window()";
    blaze::TransactionStatus current = blaze::TransactionStatus_UNDEFINED;
    tx_.set_transaction_status_change_handler(NULL, NULL);
    tx_.set_transaction_closure_handler(NULL, NULL);
    tx_.get_connection().release_transaction(&tx_);

    blaze::collector &c = tx_.get_collector();
    c.release(&tx_);

    delete ui;
}

void blz_toolkit_tx_window::closeEvent(QCloseEvent *event)
{
}

void blz_toolkit_tx_window::OnTxStatusChange(blaze::TransactionStatus
                                             status)
{
    switch(status) {
        case blaze::TransactionStatus_UNDEFINED:
            ui->tx_status_label_disp->setText(QObject::tr("UNDEFINED"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case blaze::TransactionStatus_EARLY:
            ui->tx_status_label_disp->setText(QObject::tr("EARLY"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case blaze::TransactionStatus_INITIALIZED:
            ui->tx_status_label_disp->setText(QObject::tr("INITIALIZED"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : IndianRed; color : black;"));
            break;
        case blaze::TransactionStatus_PREPARED:
            ui->tx_status_label_disp->setText(QObject::tr("PREPARED"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : RosyBrown; color : black;"));
            ui->txid_label_disp->setText(tr("[%1][%2][%3][%4]").arg(tx_.tx_id_PLID()).arg(
                                             tx_.tx_id_SVID()).arg(
                                             tx_.tx_id_CNID()).arg(tx_.tx_id_PRID()));
            break;
        case blaze::TransactionStatus_FLYING:
            ui->tx_status_label_disp->setText(QObject::tr("FLYING"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LightGreen; color : black;"));
            break;
        case blaze::TransactionStatus_CLOSED:
            ui->tx_status_label_disp->setText(QObject::tr("CLOSED"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LawnGreen; color : black;"));
            break;
        case blaze::TransactionStatus_ERROR:
            ui->tx_status_label_disp->setText(QObject::tr("ERROR"));
            ui->tx_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        default:
            break;
    }
}

void blz_toolkit_tx_window::OnTxClosure()
{
    switch(tx_.tx_res()) {
        case blaze::TransactionResult_COMMITTED:
            ui->connid_tx_res_disp->setText(QObject::tr("COMMITTED"));
            ui->connid_tx_res_disp->setStyleSheet(
                QObject::tr("background-color : LightGreen; color : black;"));
            break;
        case blaze::TransactionResult_FAILED:
            ui->connid_tx_res_disp->setText(QObject::tr("FAILED"));
            ui->connid_tx_res_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        case blaze::TransactionResult_ABORTED:
            ui->connid_tx_res_disp->setText(QObject::tr("ABORTED"));
            ui->connid_tx_res_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        default:
            break;
    }
}

void blz_toolkit_tx_window::on_actionSend_TX_triggered()
{
    if(tx_.status() != blaze::TransactionStatus_INITIALIZED) {
        tx_.re_new();
    }

    switch(ui->cfg_tx_reqtype_cb->currentIndex()) {
        case 0:
            tx_.set_tx_req_type(blaze::TransactionRequestType_RESERVED);
            break;
        case 1:
            tx_.set_tx_req_type(blaze::TransactionRequestType_SYSTEM);
            break;
        case 2:
            tx_.set_tx_req_type(blaze::TransactionRequestType_SPECIAL);
            break;
        case 3:
            tx_.set_tx_req_type(blaze::TransactionRequestType_OBJECT);
            break;
        default:
            break;
    }

    switch(ui->cfg_act_cb->currentIndex()) {
        case 0:
            tx_.set_tx_act(blaze::Action_INSERT);
            break;
        case 1:
            tx_.set_tx_act(blaze::Action_UPDATE);
            break;
        case 2:
            tx_.set_tx_act(blaze::Action_DELTA);
            break;
        case 3:
            tx_.set_tx_act(blaze::Action_DELETE);
            break;
        case 4:
            tx_.set_tx_act(blaze::Action_REMOVE);
            break;
        case 5:
            tx_.set_tx_act(blaze::Action_RESET);
            break;
        default:
            break;
    }

    switch(ui->cfg_class_encode_cb->currentIndex()) {
        case 0:
            tx_.set_tx_req_class_encode(blaze::Encode_INDEXED_NOT_ZERO);
            break;
        case 1:
            tx_.set_tx_req_class_encode(blaze::Encode_INDEXED_DELTA);
            break;
        default:
            break;
    }

    tx_.set_request_obj(tx_mdl_wrp_.wrapped_mdl().local_obj());
    tx_.set_result_class_req(ui->cfg_res_class_req_cb->isChecked());

    tx_.prepare();
    tx_.send();
}

void blz_toolkit_tx_window::on_actionReNew_TX_triggered()
{
    if(tx_.status() != blaze::TransactionStatus_INITIALIZED) {
        tx_.re_new();
    }
}

void blz_toolkit_tx_window::EmitTxStatus(blaze::TransactionStatus status)
{
    emit SignalTxStatusChange(status);
}

void blz_toolkit_tx_window::EmitTxClosure()
{
    emit SignalTxClosure();
}

void blz_toolkit_tx_window::TxFlyingActions()
{

}

void blz_toolkit_tx_window::TxClosedActions()
{

}
