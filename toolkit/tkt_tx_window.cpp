/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "tkt_tx_window.h"
#include "ui_tkt_tx_window.h"

//------------------------------------------------------------------------------
// toolkit_transaction
//------------------------------------------------------------------------------

struct toolkit_transaction_listener : public vlg::outgoing_transaction_listener {
    virtual void on_status_change(vlg::outgoing_transaction &ot, vlg::TransactionStatus current) override {
        qDebug() << "tx status:" << current;
        ((toolkit_transaction &)ot).widget_.EmitTxStatus(current);
    }
    virtual void on_close(vlg::outgoing_transaction &ot) override {
        ((toolkit_transaction &)ot).widget_.EmitTxClosure();
    }
};

static toolkit_transaction_listener ttl;

toolkit_transaction::toolkit_transaction(vlg_toolkit_tx_window &widget) : vlg::outgoing_transaction(ttl),
    widget_(widget)
{}

//------------------------------------------------------------------------------
// vlg_toolkit_tx_model
//------------------------------------------------------------------------------

vlg_toolkit_tx_model::vlg_toolkit_tx_model(vlg_toolkit_tx_vlg_class_model &wrapped_mdl, QObject *parent) :
    wrapped_mdl_(wrapped_mdl),
    QSortFilterProxyModel(parent)
{
    setSourceModel(&wrapped_mdl);
}

bool vlg_toolkit_tx_model::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
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
vlg_toolkit_tx_window::vlg_toolkit_tx_window(vlg::outgoing_connection &conn,
                                             const vlg::nentity_desc &edesc,
                                             const vlg::nclass *opt_img,
                                             QWidget *parent) :
    tx_(*this),
    tx_mdl_(edesc, conn.get_broker().get_nentity_manager(), this),
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

    if(opt_img) {
        tx_mdl_.local_obj()->set_from(*opt_img);
    }

    EmitTxStatus(tx_.get_status());
}

vlg_toolkit_tx_window::~vlg_toolkit_tx_window()
{
    qDebug() << "~vlg_toolkit_tx_window()";
    delete ui;
    ui = nullptr;
}

void vlg_toolkit_tx_window::closeEvent(QCloseEvent *event)
{
}

void vlg_toolkit_tx_window::OnTxStatusChange(vlg::TransactionStatus
                                             status)
{
    switch(status) {
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
