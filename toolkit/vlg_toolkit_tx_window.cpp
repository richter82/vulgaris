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
// toolkit_transaction
//------------------------------------------------------------------------------

toolkit_transaction::toolkit_transaction(vlg_toolkit_tx_window &widget) : widget_(widget)
{}

void toolkit_transaction::on_status_change(vlg::TransactionStatus current)
{
    qDebug() << "tx status:" << current;
    widget_.EmitTxStatus(current);
}

void toolkit_transaction::on_close()
{
    widget_.EmitTxClosure();
}

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
vlg_toolkit_tx_window::vlg_toolkit_tx_window(vlg::connection &conn,
                                             const vlg::nentity_desc &edesc,
                                             QWidget *parent) :
    tx_(*this),
    tx_mdl_(edesc, conn.get_peer().get_entity_manager(), this),
    tx_mdl_wrp_(tx_mdl_),
    QMainWindow(parent),
    ui(new Ui::vlg_toolkit_tx_window)
{
    ui->setupUi(this);
    ui->vlg_class_tx_table_view->setModel(&tx_mdl_wrp_);
    ui->connid_label_disp->setText(QString("%1").arg(conn.get_id()));
    TxClosedActions();

    connect(this,
            SIGNAL(SignalTxStatusChange(vlg::TransactionStatus)),
            this,
            SLOT(OnTxStatusChange(vlg::TransactionStatus)));

    connect(this,
            SIGNAL(SignalTxClosure()),
            this,
            SLOT(OnTxClosure()));

    tx_.bind(conn);
    tx_.set_request_nclass_id(edesc.get_nclass_id());

    vlg::nclass *sending_obj = nullptr;
    conn.get_peer().get_entity_manager_m().new_nclass_instance(edesc.get_nclass_id(), &sending_obj);
    std::unique_ptr<vlg::nclass> sending_obj_up(sending_obj);
    tx_.set_request_obj(*sending_obj);
    EmitTxStatus(tx_.get_status());
}

vlg_toolkit_tx_window::~vlg_toolkit_tx_window()
{
    qDebug() << "~vlg_toolkit_tx_window()";
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
            ui->txid_label_disp->setText(tr("[%1][%2][%3][%4]").arg(
                                             tx_.get_id().txplid).arg(
                                             tx_.get_id().txsvid).arg(
                                             tx_.get_id().txcnid).arg(
                                             tx_.get_id().txprid));
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
    switch(tx_.get_close_result()) {
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
    if(tx_.get_status() != vlg::TransactionStatus_INITIALIZED) {
        tx_.renew();
    }

    switch(ui->cfg_tx_reqtype_cb->currentIndex()) {
        case 0:
            tx_.set_request_type(vlg::TransactionRequestType_RESERVED);
            break;
        case 1:
            tx_.set_request_type(vlg::TransactionRequestType_SYSTEM);
            break;
        case 2:
            tx_.set_request_type(vlg::TransactionRequestType_SPECIAL);
            break;
        case 3:
            tx_.set_request_type(vlg::TransactionRequestType_OBJECT);
            break;
        default:
            break;
    }

    switch(ui->cfg_act_cb->currentIndex()) {
        case 0:
            tx_.set_request_action(vlg::Action_INSERT);
            break;
        case 1:
            tx_.set_request_action(vlg::Action_UPDATE);
            break;
        case 2:
            tx_.set_request_action(vlg::Action_DELTA);
            break;
        case 3:
            tx_.set_request_action(vlg::Action_DELETE);
            break;
        case 4:
            tx_.set_request_action(vlg::Action_REMOVE);
            break;
        case 5:
            tx_.set_request_action(vlg::Action_RESET);
            break;
        default:
            break;
    }

    switch(ui->cfg_class_encode_cb->currentIndex()) {
        case 0:
            tx_.set_request_nclass_encode(vlg::Encode_INDEXED_NOT_ZERO);
            break;
        case 1:
            tx_.set_request_nclass_encode(vlg::Encode_INDEXED_DELTA);
            break;
        default:
            break;
    }

    tx_.set_request_obj(*tx_mdl_wrp_.wrapped_mdl().local_obj());
    tx_.set_result_obj_required(ui->cfg_res_class_req_cb->isChecked());

    tx_.send();
}

void vlg_toolkit_tx_window::on_actionReNew_TX_triggered()
{
    if(tx_.get_status() != vlg::TransactionStatus_INITIALIZED) {
        tx_.renew();
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
{}

void vlg_toolkit_tx_window::TxClosedActions()
{}
