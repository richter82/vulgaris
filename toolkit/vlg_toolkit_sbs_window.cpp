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

#include "vlg_toolkit_sbs_window.h"
#include "ui_vlg_toolkit_sbs_window.h"
#include "vlg_toolkit_tx_window.h"

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

void vlg_toolkit_sbs_model::offerEntry(vlg::nclass *entry)
{
    wrapped_mdl_.offerEntry(entry);
}

vlg_toolkit_sbs_vlg_class_model &vlg_toolkit_sbs_model::wrapped_mdl()
{
    return wrapped_mdl_;
}


//------------------------------------------------------------------------------
// vlg_toolkit_sbs_window
//------------------------------------------------------------------------------

void sbs_status_change_hndlr(vlg::subscription &sbs,
                             vlg::SubscriptionStatus status, void *ud)
{
    vlg_toolkit_sbs_window *sbw = (vlg_toolkit_sbs_window *)ud;
    qDebug() << "sbs status:" << status;
    sbw->EmitSbsStatus(status);
}

void sbs_evt_notify_hndlr(vlg::subscription &sbs,
                          vlg::subscription_event &sbs_evt,
                          void *ud)
{
    vlg_toolkit_sbs_window *sbw = (vlg_toolkit_sbs_window *)ud;
    //qDebug() << "sbs evt occurred id:" << sbs_evt.GetEvtid() << " type:" << sbs_evt.GetEvttype();
    sbs_evt.get_collector().retain(&sbs_evt);
    sbw->EmitSbsEvent(&sbs_evt);
}

vlg_toolkit_sbs_window::vlg_toolkit_sbs_window(const vlg::nentity_desc &edesc,
                                               const vlg::nentity_manager &bem,
                                               vlg::subscription &sbs,
                                               vlg_toolkit_sbs_vlg_class_model &mdl,
                                               QWidget *parent) :
    sbs_(sbs),
    sbs_mdl_(mdl, this),
    QMainWindow(parent),
    ui(new Ui::vlg_toolkit_sbs_window)
{
    ui->setupUi(this);
    ui->vlg_class_sbs_table_view->setModel(&sbs_mdl_);
    ui->connid_label_disp->setText(QString("%1").arg(
                                       sbs_.get_connection()->get_connection_id()));
    SbsStoppedActions();

    connect(this, SIGNAL(SignalSbsStatusChange(vlg::SubscriptionStatus)),
            this,
            SLOT(OnSbsStatusChange(vlg::SubscriptionStatus)));
    connect(this, SIGNAL(SignalSbsEvent(vlg::subscription_event *)), this,
            SLOT(OnSbsEvent(vlg::subscription_event *)));
    connect(ui->vlg_class_sbs_table_view,
            SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(OnCustomMenuRequested(const QPoint &)));

    EmitSbsStatus(sbs_.get_status());
    sbs_.set_status_change_handler(sbs_status_change_hndlr, this);
    sbs_.set_event_notify_handler(sbs_evt_notify_hndlr, this);
}

vlg_toolkit_sbs_window::~vlg_toolkit_sbs_window()
{
    qDebug() << "~vlg_toolkit_sbs_window()";
    vlg::SubscriptionStatus current =
        vlg::SubscriptionStatus_UNDEFINED;
    vlg::SubscriptionResponse sbs_stop_result =
        vlg::SubscriptionResponse_UNDEFINED;
    vlg::ProtocolCode sbs_stop_protocode = vlg::ProtocolCode_SUCCESS;
    if(sbs_.get_status() == vlg::SubscriptionStatus_STARTED) {
        sbs_.stop();
        sbs_.await_for_stop_result(sbs_stop_result, sbs_stop_protocode);
    }
    sbs_.set_event_notify_handler(NULL, NULL);
    sbs_.set_status_change_handler(NULL, NULL);
    delete &sbs_;
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
        case vlg::SubscriptionStatus_UNDEFINED:
            ui->sbs_status_label_disp->setText(QObject::tr("UNDEFINED"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case vlg::SubscriptionStatus_EARLY:
            ui->sbs_status_label_disp->setText(QObject::tr("EARLY"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
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
        case vlg::SubscriptionStatus_ERROR:
            ui->sbs_status_label_disp->setText(QObject::tr("ERROR"));
            ui->sbs_status_label_disp->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        default:
            break;
    }
}

void vlg_toolkit_sbs_window::OnSbsEvent(vlg::subscription_event *sbs_evt)
{
    //qDebug() << "OnSbsEvent slot called";
    if(sbs_evt->get_event_type() !=
            vlg::SubscriptionEventType_DOWNLOAD_END) {
        sbs_mdl_.offerEntry(sbs_evt->get_object());
    } else {
    }
    sbs_evt->get_collector().release(sbs_evt);
}

void vlg_toolkit_sbs_window::OnCustomMenuRequested(const QPoint &pos)
{
    QModelIndex proxy_index = ui->vlg_class_sbs_table_view->indexAt(pos);
    QModelIndex index = sbs_mdl_.mapToSource(proxy_index);
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
    QModelIndex index = sbs_mdl_.mapToSource(proxy_index);
    vlg::nclass *item =
        static_cast<vlg::nclass *>(index.internalPointer());

    if(!item) {
        return;
    }

    const vlg::nentity_desc *edesc = NULL;
    sbs_.get_connection()->get_peer()->get_entity_manager().get_nentity_descriptor(
        item->get_nclass_id(),
        &edesc);

    if(!edesc) {
        return;
    }

    vlg::transaction &new_tx = *new vlg::transaction();
    new_tx.bind(*sbs_.get_connection());
    new_tx.set_request_obj(item);

    vlg_toolkit_tx_vlg_class_model *tx_mdl =
        new vlg_toolkit_tx_vlg_class_model(*edesc,
                                           sbs_.get_connection()->get_peer()->get_entity_manager(),
                                           new_tx,
                                           this);

    /*if last param set to 'this' child will be always on TOP*/
    vlg_toolkit_tx_window *new_tx_window =
        new vlg_toolkit_tx_window(*edesc,
                                  sbs_.get_connection()->get_peer()->get_entity_manager(),
                                  new_tx,
                                  *tx_mdl,
                                  NULL);

    new_tx_window->setAttribute(Qt::WA_DeleteOnClose, true);
    new_tx_window->setWindowTitle(QString("[TX][CONNID:%1][NCLASS:%2]").arg(
                                      sbs_.get_connection()->get_connection_id()).arg(
                                      item->get_nclass_id()));
    new_tx_window->show();
    //new_tx_window->raise();
    //new_tx_window->activateWindow();
}

void vlg_toolkit_sbs_window::EmitSbsStatus(vlg::SubscriptionStatus status)
{
    emit SignalSbsStatusChange(status);
}

void vlg_toolkit_sbs_window::EmitSbsEvent(vlg::subscription_event *sbs_evt)
{
    vlg::collector &c = sbs_evt->get_collector();
    c.retain(sbs_evt);
    emit SignalSbsEvent(sbs_evt);
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

vlg::subscription &vlg_toolkit_sbs_window::sbs() const
{
    return sbs_;
}

void vlg_toolkit_sbs_window::closeEvent(QCloseEvent *event)
{
}

