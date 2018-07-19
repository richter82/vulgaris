/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "tkt_sbs_window.h"
#include "ui_tkt_sbs_window.h"
#include "tkt_tx_window.h"

//------------------------------------------------------------------------------
// toolkit_subscription
//------------------------------------------------------------------------------

struct toolkit_subscription_listener : public vlg::outgoing_subscription_listener {
    virtual void on_status_change(vlg::outgoing_subscription &os, vlg::SubscriptionStatus current) override {
        qDebug() << "sbs status:" << current;
        ((toolkit_subscription &)os).widget_.EmitSbsStatus(current);
    }
    virtual void on_start(vlg::outgoing_subscription &) override {}
    virtual void on_stop(vlg::outgoing_subscription &) override {}
    virtual void on_incoming_event(vlg::outgoing_subscription &os,
                                   std::unique_ptr<vlg::subscription_event> &sbs_evt) override {
        std::shared_ptr<vlg::subscription_event> sbs_evt_sh(std::move(sbs_evt));
        ((toolkit_subscription &)os).widget_.SignalSbsEvent(std::move(sbs_evt_sh));
    }
};

static toolkit_subscription_listener tsl;

toolkit_subscription::toolkit_subscription(vlg_toolkit_sbs_window &widget) :
    vlg::outgoing_subscription(tsl),
    widget_(widget)
{}

//------------------------------------------------------------------------------
// vlg_toolkit_sbs_model
//------------------------------------------------------------------------------

vlg_toolkit_sbs_model::vlg_toolkit_sbs_model(vlg_toolkit_sbs_vlg_class_model
                                             &wrapped_mdl,
                                             QObject *parent) :
    wrapped_mdl_(wrapped_mdl),
    QSortFilterProxyModel(parent)
{
    setSourceModel(&wrapped_mdl);
}

bool vlg_toolkit_sbs_model::filterAcceptsRow(int sourceRow,
                                             const QModelIndex &sourceParent) const
{
    return true;
}

void vlg_toolkit_sbs_model::offerEntry(std::shared_ptr<vlg::subscription_event> &sbs_evt)
{
    wrapped_mdl_.offerEntry(sbs_evt);
}

vlg_toolkit_sbs_vlg_class_model &vlg_toolkit_sbs_model::wrapped_mdl()
{
    return wrapped_mdl_;
}

//------------------------------------------------------------------------------
// vlg_toolkit_sbs_window
//------------------------------------------------------------------------------
vlg_toolkit_sbs_window::vlg_toolkit_sbs_window(vlg::outgoing_connection &conn,
                                               const vlg::nentity_desc &edesc,
                                               QWidget *parent) :
    sbs_(*this),
    sbs_mdl_(edesc, conn.get_peer().get_nentity_manager()),
    sbs_mdl_wr_(sbs_mdl_, this),
    QMainWindow(parent),
    ui(new Ui::vlg_toolkit_sbs_window)
{
    ui->setupUi(this);
    ui->vlg_class_sbs_table_view->setModel(&sbs_mdl_wr_);
    ui->connid_label_disp->setText(QString("%1").arg(conn.get_id()));
    SbsStoppedActions();

    connect(this, SIGNAL(SignalSbsStatusChange(vlg::SubscriptionStatus)),this,
            SLOT(OnSbsStatusChange(vlg::SubscriptionStatus)));

    connect(this, SIGNAL(SignalSbsEvent(std::shared_ptr<vlg::subscription_event>)), this,
            SLOT(OnSbsEvent(std::shared_ptr<vlg::subscription_event>)));

    connect(ui->vlg_class_sbs_table_view, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(OnCustomMenuRequested(const QPoint &)));

    sbs_.bind(conn);
    sbs_.set_nclass_id(edesc.get_nclass_id());

    EmitSbsStatus(sbs_.get_status());
}

vlg_toolkit_sbs_window::~vlg_toolkit_sbs_window()
{
    qDebug() << "~vlg_toolkit_sbs_window()";
    vlg::SubscriptionResponse sbs_stop_result = vlg::SubscriptionResponse_UNDEFINED;
    vlg::ProtocolCode sbs_stop_protocode = vlg::ProtocolCode_SUCCESS;
    if(sbs_.get_status() == vlg::SubscriptionStatus_STARTED) {
        sbs_.stop();
        sbs_.await_for_stop_result(sbs_stop_result, sbs_stop_protocode);
    }
    delete ui;
}

void vlg_toolkit_sbs_window::on_actionStart_SBS_triggered()
{
    switch(ui->cfg_sbs_type_cb->currentIndex()) {
        case 0:
            sbs_.set_type(vlg::SubscriptionType_SNAPSHOT);
            break;
        case 1:
            sbs_.set_type(vlg::SubscriptionType_INCREMENTAL);
            break;
        default:
            break;
    }

    switch(ui->cfg_sbs_mode_cb->currentIndex()) {
        case 0:
            sbs_.set_mode(vlg::SubscriptionMode_ALL);
            break;
        case 1:
            sbs_.set_mode(vlg::SubscriptionMode_DOWNLOAD);
            break;
        case 2:
            sbs_.set_mode(vlg::SubscriptionMode_LIVE);
            break;
        default:
            break;
    }

    switch(ui->cfg_sbs_flow_type_cb->currentIndex()) {
        case 0:
            sbs_.set_flow_type(vlg::SubscriptionFlowType_ALL);
            break;
        case 1:
            sbs_.set_flow_type(vlg::SubscriptionFlowType_LAST);
            break;
        default:
            break;
    }

    switch(ui->cfg_sbs_dwnld_type_cb->currentIndex()) {
        case 0:
            sbs_.set_download_type(vlg::SubscriptionDownloadType_ALL);
            break;
        case 1:
            sbs_.set_download_type(vlg::SubscriptionDownloadType_PARTIAL);
            break;
        default:
            break;
    }

    switch(ui->cfg_sbs_class_encode_cb->currentIndex()) {
        case 0:
            sbs_.set_nclass_encode(vlg::Encode_INDEXED_NOT_ZERO);
            break;
        case 1:
            sbs_.set_nclass_encode(vlg::Encode_INDEXED_DELTA);
            break;
        default:
            break;
    }

    sbs_.set_open_timestamp_0(ui->cfg_sbs_ts0_le->text().toUInt());
    sbs_.set_open_timestamp_1(ui->cfg_sbs_ts1_le->text().toUInt());
    sbs_.start();
}

void vlg_toolkit_sbs_window::on_actionStop_SBS_triggered()
{
    sbs_.stop();
}

void vlg_toolkit_sbs_window::OnSbsStatusChange(vlg::SubscriptionStatus
                                               status)
{
    switch(status) {
        case vlg::SubscriptionStatus_INITIALIZED:
            ui->sbs_status_label_disp->setText(QObject::tr("INITIALIZED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : IndianRed; color : black;"));
            break;
        case vlg::SubscriptionStatus_STOPPED:
            ui->sbs_status_label_disp->setText(QObject::tr("STOPPED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : RosyBrown; color : black;"));
            SbsStoppedActions();
            break;
        case vlg::SubscriptionStatus_REQUEST_SENT:
            ui->sbs_status_label_disp->setText(QObject::tr("REQUEST SENT"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LightGreen; color : black;"));
            break;
        case vlg::SubscriptionStatus_STARTED:
            ui->sbs_status_label_disp->setText(QObject::tr("STARTED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LawnGreen; color : black;"));
            SbsStartedActions();
            break;
        case vlg::SubscriptionStatus_RELEASED:
            ui->sbs_status_label_disp->setText(QObject::tr("RELEASED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : black; color : white;"));
            break;
        default:
            break;
    }
}

void vlg_toolkit_sbs_window::OnSbsEvent(std::shared_ptr<vlg::subscription_event> sbs_evt)
{
    if(sbs_evt->get_event_type() != vlg::SubscriptionEventType_DOWNLOAD_END) {
        sbs_mdl_wr_.offerEntry(sbs_evt);
    } else {
    }
}

void vlg_toolkit_sbs_window::OnCustomMenuRequested(const QPoint &pos)
{
    QModelIndex proxy_index = ui->vlg_class_sbs_table_view->indexAt(pos);
    QModelIndex index = sbs_mdl_wr_.mapToSource(proxy_index);
    vlg::nclass *item = static_cast<vlg::nclass *>
                        (index.internalPointer());
    if(!item) {
        return;
    }
    QMenu *custom_menu = new QMenu(QString(""), this);
    QAction *ntx = new QAction("New Transaction", this);
    connect(ntx, SIGNAL(triggered()), this, SLOT(OnNewTxRequested()));
    custom_menu->addAction(ntx);
    custom_menu->popup(ui->vlg_class_sbs_table_view->viewport()->mapToGlobal(pos));
}

void vlg_toolkit_sbs_window::OnNewTxRequested()
{
    QModelIndexList indexes =
        ui->vlg_class_sbs_table_view->selectionModel()->selectedIndexes();

    if(!indexes.count()) {
        return;
    }

    QModelIndex proxy_index = indexes.at(0);
    QModelIndex index = sbs_mdl_wr_.mapToSource(proxy_index);
    vlg::nclass *item = static_cast<vlg::nclass *>(index.internalPointer());

    if(!item) {
        return;
    }

    const vlg::nentity_desc *edesc = sbs_.get_connection().
                                     get_peer().
                                     get_nentity_manager().
                                     get_nentity_descriptor(item->get_id());
    if(!edesc) {
        return;
    }

    /*if last param set to 'this' child will be always on TOP*/
    vlg_toolkit_tx_window *new_tx_window = new vlg_toolkit_tx_window(sbs_.get_connection(),
                                                                     *edesc,
                                                                     item,
                                                                     nullptr);

    new_tx_window->setAttribute(Qt::WA_DeleteOnClose, true);
    new_tx_window->setWindowTitle(QString("[TX][CONNID:%1][NCLASS:%2]").arg(
                                      sbs_.get_connection().get_id()).arg(
                                      item->get_id()));
    new_tx_window->show();
    //new_tx_window->raise();
    //new_tx_window->activateWindow();
}

void vlg_toolkit_sbs_window::EmitSbsStatus(vlg::SubscriptionStatus status)
{
    emit SignalSbsStatusChange(status);
}

void vlg_toolkit_sbs_window::SbsStartedActions()
{
    ui->sbs_params_gbox->setEnabled(false);
    ui->actionStart_SBS->setEnabled(false);
    ui->actionStop_SBS->setEnabled(true);
    ui->sbsid_label_disp->setText(QString("%1").arg(sbs_.get_id()));
}

void vlg_toolkit_sbs_window::SbsStoppedActions()
{
    ui->sbs_params_gbox->setEnabled(true);
    ui->actionStart_SBS->setEnabled(true);
    ui->actionStop_SBS->setEnabled(false);
    ui->sbsid_label_disp->setText(QString("%1").arg(sbs_.get_id()));
}

void vlg_toolkit_sbs_window::closeEvent(QCloseEvent *event)
{
}

