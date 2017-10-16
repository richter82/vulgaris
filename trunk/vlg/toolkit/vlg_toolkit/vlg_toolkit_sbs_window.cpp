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

#include "blz_toolkit_sbs_window.h"
#include "ui_blz_toolkit_sbs_window.h"

#include "blz_toolkit_tx_window.h"

//------------------------------------------------------------------------------
// blz_toolkit_sbs_model
//------------------------------------------------------------------------------

blz_toolkit_sbs_model::blz_toolkit_sbs_model(blz_toolkit_sbs_blz_class_model
                                             &wrapped_mdl, QObject *parent) :
    wrapped_mdl_(wrapped_mdl),
    QSortFilterProxyModel(parent)
{
    setSourceModel(&wrapped_mdl);
}

bool blz_toolkit_sbs_model::filterAcceptsRow(int sourceRow,
                                             const QModelIndex &sourceParent) const
{
    return true;
}

void blz_toolkit_sbs_model::offerEntry(blaze::nclass *entry)
{
    wrapped_mdl_.offerEntry(entry);
}

blz_toolkit_sbs_blz_class_model &blz_toolkit_sbs_model::wrapped_mdl()
{
    return wrapped_mdl_;
}


//------------------------------------------------------------------------------
// blz_toolkit_sbs_window
//------------------------------------------------------------------------------

void sbs_status_change_hndlr(blaze::subscription_int &sbs,
                             blaze::SubscriptionStatus status, void *ud)
{
    blz_toolkit_sbs_window *sbw = (blz_toolkit_sbs_window *)ud;
    qDebug() << "sbs status:" << status;
    sbw->EmitSbsStatus(status);
}

void sbs_evt_notify_hndlr(blaze::subscription_int &sbs,
                          blaze::subscription_event_int &sbs_evt,
                          void *ud)
{
    blz_toolkit_sbs_window *sbw = (blz_toolkit_sbs_window *)ud;
    //qDebug() << "sbs evt occurred id:" << sbs_evt.GetEvtid() << " type:" << sbs_evt.GetEvttype();
    sbw->EmitSbsEvent(&sbs_evt);
}

blz_toolkit_sbs_window::blz_toolkit_sbs_window(const blaze::entity_desc &edesc,
                                               const blaze::entity_manager &bem,
                                               blaze::subscription_int &sbs,
                                               blz_toolkit_sbs_blz_class_model &mdl,
                                               QWidget *parent) :
    sbs_(sbs),
    sbs_mdl_(mdl, this),
    QMainWindow(parent),
    ui(new Ui::blz_toolkit_sbs_window)
{
    ui->setupUi(this);
    ui->blz_class_sbs_table_view->setModel(&sbs_mdl_);
    ui->connid_label_disp->setText(QString("%1").arg(
                                       sbs_.get_connection().connid()));

    sbs_.get_collector().retain(&sbs_);

    SbsStoppedActions();

    connect(this, SIGNAL(SignalSbsStatusChange(blaze::SubscriptionStatus)),
            this,
            SLOT(OnSbsStatusChange(blaze::SubscriptionStatus)));
    connect(this, SIGNAL(SignalSbsEvent(blaze::subscription_event_int *)), this,
            SLOT(OnSbsEvent(blaze::subscription_event_int *)));
    connect(ui->blz_class_sbs_table_view,
            SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(OnCustomMenuRequested(const QPoint &)));

    EmitSbsStatus(sbs_.status());
    sbs_.set_subscription_status_change_handler(sbs_status_change_hndlr, this);
    sbs_.set_subscription_event_notify_handler(sbs_evt_notify_hndlr, this);
}

blz_toolkit_sbs_window::~blz_toolkit_sbs_window()
{
    qDebug() << "~blz_toolkit_sbs_window()";
    blaze::SubscriptionStatus current =
        blaze::SubscriptionStatus_UNDEFINED;
    blaze::SubscriptionResponse sbs_stop_result =
        blaze::SubscriptionResponse_UNDEFINED;
    blaze::ProtocolCode sbs_stop_protocode = blaze::ProtocolCode_SUCCESS;
    if(sbs_.status() == blaze::SubscriptionStatus_STARTED) {
        sbs_.stop();
        sbs_.await_for_stop_result(sbs_stop_result, sbs_stop_protocode);
    }
    sbs_.set_subscription_event_notify_handler(NULL, NULL);
    sbs_.set_subscription_status_change_handler(NULL, NULL);
    sbs_.get_connection().release_subscription(&sbs_);

    /*
    blaze::collector &c = sbs_.get_collector();
    c.release(&sbs_);
    */

    delete ui;
}

void blz_toolkit_sbs_window::on_actionStart_SBS_triggered()
{
    switch(ui->cfg_sbs_type_cb->currentIndex()) {
        case 0:
            sbs_.set_sbstyp(blaze::SubscriptionType_SNAPSHOT);
            break;
        case 1:
            sbs_.set_sbstyp(blaze::SubscriptionType_INCREMENTAL);
            break;
        default:
            break;
    }

    switch(ui->cfg_sbs_mode_cb->currentIndex()) {
        case 0:
            sbs_.set_sbsmod(blaze::SubscriptionMode_ALL);
            break;
        case 1:
            sbs_.set_sbsmod(blaze::SubscriptionMode_DOWNLOAD);
            break;
        case 2:
            sbs_.set_sbsmod(blaze::SubscriptionMode_LIVE);
            break;
        default:
            break;
    }

    switch(ui->cfg_sbs_flow_type_cb->currentIndex()) {
        case 0:
            sbs_.set_flotyp(blaze::SubscriptionFlowType_ALL);
            break;
        case 1:
            sbs_.set_flotyp(blaze::SubscriptionFlowType_LAST);
            break;
        default:
            break;
    }

    switch(ui->cfg_sbs_dwnld_type_cb->currentIndex()) {
        case 0:
            sbs_.set_dwltyp(blaze::SubscriptionDownloadType_ALL);
            break;
        case 1:
            sbs_.set_dwltyp(blaze::SubscriptionDownloadType_PARTIAL);
            break;
        default:
            break;
    }

    switch(ui->cfg_sbs_class_encode_cb->currentIndex()) {
        case 0:
            sbs_.set_enctyp(blaze::Encode_INDEXED_NOT_ZERO);
            break;
        case 1:
            sbs_.set_enctyp(blaze::Encode_INDEXED_DELTA);
            break;
        default:
            break;
    }

    sbs_.set_open_tmstp0(ui->cfg_sbs_ts0_le->text().toUInt());
    sbs_.set_open_tmstp1(ui->cfg_sbs_ts1_le->text().toUInt());
    sbs_.start();
}

void blz_toolkit_sbs_window::on_actionStop_SBS_triggered()
{
    sbs_.stop();
}

void blz_toolkit_sbs_window::OnSbsStatusChange(blaze::SubscriptionStatus
                                               status)
{
    switch(status) {
        case blaze::SubscriptionStatus_UNDEFINED:
            ui->sbs_status_label_disp->setText(QObject::tr("UNDEFINED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case blaze::SubscriptionStatus_EARLY:
            ui->sbs_status_label_disp->setText(QObject::tr("EARLY"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case blaze::SubscriptionStatus_INITIALIZED:
            ui->sbs_status_label_disp->setText(QObject::tr("INITIALIZED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : IndianRed; color : black;"));
            break;
        case blaze::SubscriptionStatus_STOPPED:
            ui->sbs_status_label_disp->setText(QObject::tr("STOPPED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : RosyBrown; color : black;"));
            SbsStoppedActions();
            break;
        case blaze::SubscriptionStatus_REQUEST_SENT:
            ui->sbs_status_label_disp->setText(QObject::tr("REQUEST SENT"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LightGreen; color : black;"));
            break;
        case blaze::SubscriptionStatus_STARTED:
            ui->sbs_status_label_disp->setText(QObject::tr("STARTED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : LawnGreen; color : black;"));
            SbsStartedActions();
            break;
        case blaze::SubscriptionStatus_RELEASED:
            ui->sbs_status_label_disp->setText(QObject::tr("RELEASED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : black; color : white;"));
            break;
        case blaze::SubscriptionStatus_ERROR:
            ui->sbs_status_label_disp->setText(QObject::tr("ERROR"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        default:
            break;
    }
}

void blz_toolkit_sbs_window::OnSbsEvent(blaze::subscription_event_int *sbs_evt)
{
    //qDebug() << "OnSbsEvent slot called";
    if(sbs_evt->get_evttype() !=
            blaze::SubscriptionEventType_DOWNLOAD_END) {
        sbs_mdl_.offerEntry(sbs_evt->get_obj());
    } else {
    }
    blaze::collector &c = sbs_evt->get_collector();
    c.release(sbs_evt);
}

void blz_toolkit_sbs_window::OnCustomMenuRequested(const QPoint &pos)
{
    QModelIndex proxy_index = ui->blz_class_sbs_table_view->indexAt(pos);
    QModelIndex index = sbs_mdl_.mapToSource(proxy_index);
    blaze::nclass *item = static_cast<blaze::nclass *>
                          (index.internalPointer());
    if(!item) {
        return;
    }
    QMenu *custom_menu = new QMenu(QString(""), this);
    QAction *ntx = new QAction("New Transaction", this);
    connect(ntx, SIGNAL(triggered()), this, SLOT(OnNewTxRequested()));
    custom_menu->addAction(ntx);
    custom_menu->popup(ui->blz_class_sbs_table_view->viewport()->mapToGlobal(pos));
}

void blz_toolkit_sbs_window::OnNewTxRequested()
{
    QModelIndexList indexes =
        ui->blz_class_sbs_table_view->selectionModel()->selectedIndexes();

    if(!indexes.count()) {
        return;
    }

    QModelIndex proxy_index = indexes.at(0);
    QModelIndex index = sbs_mdl_.mapToSource(proxy_index);
    blaze::nclass *item =
        static_cast<blaze::nclass *>(index.internalPointer());

    if(!item) {
        return;
    }

    const blaze::entity_desc *edesc = NULL;
    sbs_.get_connection().peer().get_em().get_entity_descriptor(
        item->get_nclass_id(),
        &edesc);

    if(!edesc) {
        return;
    }

    blaze::transaction_int *new_tx = NULL;
    sbs_.get_connection().new_transaction(&new_tx);
    new_tx->set_request_obj(item);

    blz_toolkit_tx_blz_class_model *tx_mdl =
        new blz_toolkit_tx_blz_class_model(*edesc,
                                           sbs_.peer().get_em(),
                                           *new_tx,
                                           this);

    /*if last param set to 'this' child will be always on TOP*/
    blz_toolkit_tx_window *new_tx_window =
        new blz_toolkit_tx_window(*edesc,
                                  sbs_.get_connection().peer().get_em(),
                                  *new_tx,
                                  *tx_mdl,
                                  NULL);

    new_tx_window->setAttribute(Qt::WA_DeleteOnClose, true);
    new_tx_window->setWindowTitle(QString("[TX][CONNID:%1][NCLASS:%2]").arg(
                                      sbs_.get_connection().connid()).arg(
                                      item->get_nclass_id()));
    new_tx_window->show();
    //new_tx_window->raise();
    //new_tx_window->activateWindow();
}

void blz_toolkit_sbs_window::EmitSbsStatus(blaze::SubscriptionStatus
                                           status)
{
    emit SignalSbsStatusChange(status);
}

void blz_toolkit_sbs_window::EmitSbsEvent(blaze::subscription_event_int
                                          *sbs_evt)
{
    blaze::collector &c = sbs_evt->get_collector();
    c.retain(sbs_evt);
    emit SignalSbsEvent(sbs_evt);
}

void blz_toolkit_sbs_window::SbsStartedActions()
{
    ui->sbs_params_gbox->setEnabled(false);
    ui->actionStart_SBS->setEnabled(false);
    ui->actionStop_SBS->setEnabled(true);
    ui->sbsid_label_disp->setText(QString("%1").arg(sbs_.sbsid()));
}

void blz_toolkit_sbs_window::SbsStoppedActions()
{
    ui->sbs_params_gbox->setEnabled(true);
    ui->actionStart_SBS->setEnabled(true);
    ui->actionStop_SBS->setEnabled(false);
    ui->sbsid_label_disp->setText(QString("%1").arg(sbs_.sbsid()));
}

blaze::subscription_int &blz_toolkit_sbs_window::sbs() const
{
    return sbs_;
}

void blz_toolkit_sbs_window::closeEvent(QCloseEvent *event)
{
}

