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

#include "vlg_toolkit_mainwindow.h"
#include "ui_vlg_toolkit_mainwindow.h"
#include "cfg.h"

namespace vlg_tlkt {

unsigned int VLG_TOOLKIT_PEER_VER[] = {0,0,0,0};

//------------------------------------------------------------------------------
// ****VLG_TOOLKIT_PEER****
//------------------------------------------------------------------------------
toolkit_peer::toolkit_peer(vlg_toolkit_MainWindow &widget) : widget_(widget)
{}

const char *toolkit_peer::get_name()
{
    return "toolkit_peer[" __DATE__ "]";
}

const unsigned int *toolkit_peer::get_version()
{
    return VLG_TOOLKIT_PEER_VER;
}

void toolkit_peer::on_status_change(vlg::PeerStatus current)
{
    qDebug() << "peer status:" << current;
    widget_.EmitPeerStatus(current);
}

}

vlg_toolkit_MainWindow::vlg_toolkit_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::vlg_toolkit_MainWindow),
    peer_(*this),
    view_model_loaded_(false),
    vlgmodel_load_list_model_(this),
    pers_dri_file_load_list_model_(this),
    vlg_model_loaded_model_(peer_.get_entity_manager_m(), this),
    pte_apnd_(this),
    flash_info_msg_val_(0)
{
    ui->setupUi(this);
    InitGuiConfig();

    //peer_ logger cfg bgn
    vlg::logger::add_appender_to_all_loggers(&pte_apnd_);
    //peer_ logger cfg end

    //cfg peer models-view connection
    ui->peer_cfg_view_list_vlgmodel->setModel(&vlgmodel_load_list_model_);
    ui->peer_cfg_view_list_vlgpersdriv->setModel(&pers_dri_file_load_list_model_);

    //settings bgn
    QSettings settings;
    settings.beginGroup(KEY_WINDOW);
    if(settings.contains(KEY_SIZE)) {
        resize(settings.value(KEY_SIZE).toSize());
    }
    if(settings.contains(KEY_POSITION)) {
        move(settings.value(KEY_POSITION).toPoint());
    }
    settings.endGroup();
    //settings end

    LoadDefPeerCfgFile();
}

vlg_toolkit_MainWindow::~vlg_toolkit_MainWindow()
{
    vlg::logger::remove_last_appender_from_all_loggers();
    delete ui;
}

void vlg_toolkit_MainWindow::InitGuiConfig()
{
    qRegisterMetaType<vlg::TraceLVL>("vlg::TraceLVL");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<vlg::PeerStatus>("vlg::PeerStatus");
    qRegisterMetaType<vlg::ConnectionStatus>("vlg::ConnectionStatus");
    qRegisterMetaType<vlg::TransactionStatus>("vlg::TransactionStatus");
    qRegisterMetaType<vlg::SubscriptionStatus>("vlg::SubscriptionStatus");
    qRegisterMetaType<std::shared_ptr<vlg::subscription_event>>("std::shared_ptr<vlg::subscription_event>");
    qRegisterMetaType<VLG_SBS_COL_DATA_ENTRY *>("VLG_SBS_COL_DATA_ENTRY");

    ui->action_Start_Peer->setDisabled(true);
    ui->action_Stop_Peer->setDisabled(true);
    ui->actionConnect->setDisabled(true);

    connect(&pte_apnd_,
            SIGNAL(messageReady(vlg::TraceLVL, const QString &)),
            this,
            SLOT(OnLogEvent(vlg::TraceLVL, const QString &)));

    connect(this,
            SIGNAL(Peer_status_change(vlg::PeerStatus)),
            this,
            SLOT(OnPeer_status_change(vlg::PeerStatus)));

    //timout
    connect(this,
            SIGNAL(SignalNewConnectionTimeout(const QString &)),
            this,
            SLOT(OnSetInfoMsg(const QString &)));

    connect(&flash_info_msg_tim_, SIGNAL(timeout()), this, SLOT(OnFlashInfoMsg()));
    connect(&reset_info_msg_tim_, SIGNAL(timeout()), this, SLOT(OnResetInfoMsg()));

    //set cfg all button
    connect(ui->set_peer_cfg_all_button, SIGNAL(clicked()), this,
            SLOT(on_set_peer_params_button_clicked()));
    connect(ui->set_peer_cfg_all_button, SIGNAL(clicked()), this,
            SLOT(on_update_peer_model_button_clicked()));
    connect(ui->set_peer_cfg_all_button, SIGNAL(clicked()), this,
            SLOT(on_set_pers_driver_button_clicked()));

    //vlg_model
    connect(this, SIGNAL(VLG_MODEL_Update_event()), this,
            SLOT(On_VLG_MODEL_Update()));
    connect(this, SIGNAL(VLG_MODEL_Update_event()), &vlg_model_loaded_model_,
            SLOT(OnModelUpdate_event()));
}

void vlg_toolkit_MainWindow::closeEvent(QCloseEvent *event)
{
    QString appName = qApp->applicationName();
    QString text = "<p>Confirm Exit?</p>";
    int res = QMessageBox::question(this, appName, text, QMessageBox::Ok,
                                    QMessageBox::Cancel);
    if(res == QMessageBox::Cancel) {
        event->ignore();
        return;
    }
    peer_.stop(true);
    vlg::PeerStatus current = vlg::PeerStatus_ZERO;
    peer_.await_for_status_reached_or_outdated(vlg::PeerStatus_STOPPED, current);
    QSettings settings;
    settings.beginGroup(KEY_WINDOW);
    settings.setValue(KEY_SIZE, size());
    settings.setValue(KEY_POSITION, pos());
    settings.endGroup();
    event->accept();
}

void vlg_toolkit_MainWindow::on_action_Exit_triggered()
{
    close();
}

void vlg_toolkit_MainWindow::peer_params_clbk_ud(int pnum, const char *param,
                                                 const char *value, void *ud)
{
    vlg_toolkit_MainWindow *mw = (vlg_toolkit_MainWindow *)ud;
    mw->PeerLoadCfgHndl(pnum, param, value);
}

//------------------------------------------------------------------------------
// vlg_toolkit_MainWindow::PeerLoadCfgHndl
//------------------------------------------------------------------------------

vlg::RetCode vlg_toolkit_MainWindow::PeerLoadCfgHndl(int pnum,
                                                     const char *param,
                                                     const char *value)
{
    if(!strcmp(param, "pure_server")) {
        ui->pp_cfg_peer_personality->setCurrentIndex(1);
    }

    if(!strcmp(param, "pure_client")) {
        ui->pp_cfg_peer_personality->setCurrentIndex(0);
    }

    if(!strcmp(param, "load_model")) {
        if(value) {
            if(vlgmodel_load_list_model_.stringList().contains(tr(value))) {
                qDebug() << "[load_model] model already specified:" << value;
                return vlg::RetCode_BADCFG;
            } else {
                int rowc = vlgmodel_load_list_model_.rowCount();
                vlgmodel_load_list_model_.insertRow(rowc);
                QModelIndex index = vlgmodel_load_list_model_.index(rowc, 0);
                vlgmodel_load_list_model_.setData(index, tr(value));
            }
        } else {
            qDebug() << "[load_model] requires argument.";
            return vlg::RetCode_BADCFG;
        }
    }

    if(!strcmp(param, "srv_sin_addr")) {
        if(value) {
            ui->pp_cfg_srv_sin_addr->setText(tr(value));
        } else {
            qDebug() << "[srv_sin_addr] requires argument.";
            return vlg::RetCode_BADCFG;
        }
    }

    if(!strcmp(param, "srv_sin_port")) {
        if(value) {
            ui->pp_cfg_srv_sin_port->setText(tr(value));
        } else {
            qDebug() << "[srv_sin_port] requires argument.";
            return vlg::RetCode_BADCFG;
        }
    }

    if(!strcmp(param, "srv_exectrs")) {
        if(value) {
            ui->pp_cfg_srv_exectrs->setText(tr(value));
        } else {
            qDebug() << "[srv_exectrs] requires argument.";
            return vlg::RetCode_BADCFG;
        }
    }

    if(!strcmp(param, "cli_exectrs")) {
        if(value) {
            ui->pp_cfg_cli_exectrs->setText(tr(value));
        } else {
            qDebug() << "[cli_exectrs] requires argument.";
            return vlg::RetCode_BADCFG;
        }
    }

    if(!strcmp(param, "srv_sbs_exectrs")) {
        if(value) {
            ui->pp_cfg_srv_sbs_exectrs->setText(tr(value));
        } else {
            qDebug() << "[srv_sbs_exectrs] requires argument.";
            return vlg::RetCode_BADCFG;
        }
    }

    if(!strcmp(param, "pers_enabled")) {
        ui->pp_cfg_pers_enabled->setChecked(true);
    }

    if(!strcmp(param, "pers_schema_create")) {
        ui->pp_cfg_pers_schema_create->setChecked(true);
    }

    if(!strcmp(param, "drop_existing_schema")) {
        ui->pp_cfg_drop_existing_schema->setChecked(true);
    }

    if(!strcmp(param, "load_pers_driv")) {
        if(value) {
            if(pers_dri_file_load_list_model_.stringList().contains(tr(value))) {
                qDebug() << "[load_pers_driv] driver already specified:" << value;
                return vlg::RetCode_BADCFG;
            } else {
                int rowc = pers_dri_file_load_list_model_.rowCount();
                pers_dri_file_load_list_model_.insertRow(rowc);
                QModelIndex index = pers_dri_file_load_list_model_.index(rowc, 0);
                pers_dri_file_load_list_model_.setData(index, tr(value));
            }
        } else {
            qDebug() << "[load_pers_driv] requires argument.";
            return vlg::RetCode_BADCFG;
        }
    }

    return vlg::RetCode_OK;
}

void vlg_toolkit_MainWindow::on_action_Load_Config_triggered()
{
    vlg::RetCode res = vlg::RetCode_OK;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load Config"),
                                                    QDir::currentPath(),
                                                    tr("config (*.*)"));
    if(fileName.isEmpty()) {
        qDebug() << "fileName empty.";
        return;
    }
    QByteArray ba = fileName.toLocal8Bit();
    const char *fileName_cstr = ba.data();
    vlg::cfg_ldr peer_conf_ldr;
    if((res = peer_conf_ldr.init(fileName_cstr))) {
        qDebug() << "peer_conf_ldr.Init() failed." << res;
        return;
    }
    if((res = peer_conf_ldr.load_config())) {
        qDebug() << "peer_conf_ldr.LoadCfg() failed." << res;
        return;
    }
    peer_conf_ldr.enum_params(peer_params_clbk_ud, this);
}

void vlg_toolkit_MainWindow::on_set_peer_params_button_clicked()
{
    switch(ui->pp_cfg_peer_personality->currentIndex()) {
        case 0:
            peer_.set_personality(vlg::PeerPersonality_PURE_CLIENT);
            break;
        case 1:
            peer_.set_personality(vlg::PeerPersonality_PURE_SERVER);
            break;
        case 2:
            peer_.set_personality(vlg::PeerPersonality_BOTH);
            break;
        default:
            break;
    }

    if(ui->pp_cfg_srv_sin_addr->text() != "") {
        peer_.set_server_address(ui->pp_cfg_srv_sin_addr->text().toLocal8Bit().data());
    }
    peer_.set_server_port(atoi(ui->pp_cfg_srv_sin_port->text().toLocal8Bit().data()));
    peer_.set_server_transaction_service_executor_size(atoi(
                                                           ui->pp_cfg_srv_exectrs->text().toLocal8Bit().data()));
    peer_.set_client_transaction_service_executor_size(atoi(
                                                           ui->pp_cfg_cli_exectrs->text().toLocal8Bit().data()));
    peer_.set_server_subscription_service_executor_size(atoi(
                                                            ui->pp_cfg_srv_sbs_exectrs->text().toLocal8Bit().data()));
    peer_.set_persistent(ui->pp_cfg_pers_enabled->isChecked());
    peer_.set_create_persistent_schema(ui->pp_cfg_pers_schema_create->isChecked());
    peer_.set_drop_existing_persistent_schema(ui->pp_cfg_drop_existing_schema->isChecked());
    peer_.set_configured(true);
    ui->peer_cfg_status_label->setText(tr("Set"));
    ui->peer_cfg_status_label->setStyleSheet(tr("background-color : LightGreen; color : black;"));
    ui->action_Start_Peer->setEnabled(true);
}

void vlg_toolkit_MainWindow::on_update_peer_model_button_clicked()
{
    int numRows = vlgmodel_load_list_model_.rowCount();
    for(int row = 0; row < numRows; ++row) {
        QModelIndex index = vlgmodel_load_list_model_.index(row, 0);
        peer_.add_load_model(vlgmodel_load_list_model_.data(index,
                                                            Qt::DisplayRole).toString().toLocal8Bit().data());
    }
    ui->peer_model_label_status->setText(tr("Set"));
    ui->peer_model_label_status->setStyleSheet(
        tr("background-color : LightGreen; color : black;"));
}

void vlg_toolkit_MainWindow::on_set_pers_driver_button_clicked()
{
    int numRows = pers_dri_file_load_list_model_.rowCount();
    for(int row = 0; row < numRows; ++row) {
        QModelIndex index = pers_dri_file_load_list_model_.index(row, 0);
        peer_.add_load_persistent_driver(pers_dri_file_load_list_model_.data(index,
                                                                             Qt::DisplayRole).toString().toLocal8Bit().data());
    }
    ui->peer_pers_driv_status_label->setText(tr("Set"));
    ui->peer_pers_driv_status_label->setStyleSheet(
        tr("background-color : LightGreen; color : black;"));
}

void vlg_toolkit_MainWindow::on_action_Start_Peer_triggered()
{
    ui->peer_Tab->widget(0)->setEnabled(false);
    peer_.start(0, 0, true);
}

void vlg_toolkit_MainWindow::OnPeer_status_change(vlg::PeerStatus status)
{
    switch(status) {
        case vlg::PeerStatus_ZERO:
            ui->peer_status_label_display->setText(QObject::tr("ZERO"));
            break;
        case vlg::PeerStatus_EARLY:
            ui->peer_status_label_display->setText(QObject::tr("EARLY"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : Beige; color : black;"));
            break;
        case vlg::PeerStatus_WELCOMED:
            ui->peer_status_label_display->setText(QObject::tr("WELCOMED"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : Cornsilk; color : black;"));
            break;
        case vlg::PeerStatus_INITIALIZING:
            ui->peer_status_label_display->setText(QObject::tr("INITIALIZING"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : PowderBlue; color : black;"));
            break;
        case vlg::PeerStatus_INITIALIZED:
            ui->peer_status_label_display->setText(QObject::tr("INITIALIZED"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : Aquamarine; color : black;"));
            break;
        case vlg::PeerStatus_STARTING:
            ui->peer_status_label_display->setText(QObject::tr("STARTING"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : Coral; color : black;"));
            break;
        case vlg::PeerStatus_STARTED:
            ui->peer_status_label_display->setText(QObject::tr("STARTED"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : DarkOrange; color : black;"));
            break;
        case vlg::PeerStatus_RUNNING:
            Status_RUNNING_Actions();
            break;
        case vlg::PeerStatus_STOP_REQUESTED:
            ui->peer_status_label_display->setText(QObject::tr("STOP REQUESTED"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : Indigo; color : white;"));
            ui->action_Stop_Peer->setEnabled(false);
            ui->actionConnect->setEnabled(false);
            break;
        case vlg::PeerStatus_STOPPING:
            ui->peer_status_label_display->setText(QObject::tr("STOPPING"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : IndianRed; color : black;"));
            break;
        case vlg::PeerStatus_STOPPED:
            Status_STOPPED_Actions();
            break;
        case vlg::PeerStatus_DIED:
            ui->peer_status_label_display->setText(QObject::tr("DIED"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : Black; color : white;"));
            break;
        case vlg::PeerStatus_ERROR:
            ui->peer_status_label_display->setText(QObject::tr("ERROR"));
            ui->peer_status_label_display->setStyleSheet(
                QObject::tr("background-color : Red; color : black;"));
            break;
        default:
            break;
    }
}

void vlg_toolkit_MainWindow::On_VLG_MODEL_Update()
{}

void vlg_toolkit_MainWindow::OnSetInfoMsg(const QString &msg)
{
    ui->main_info_label->setText(QObject::tr("TIMEOUT: [%1]").arg(msg));
    ui->main_info_label->setStyleSheet(
        QObject::tr("background-color : Red; color : black; font-weight: bold;"));
    reset_info_msg_tim_.start(VLG_TKT_INT_SINGL_SHOT_TIMER_CAPTMSG_RST_MSEC);
    flash_info_msg_tim_.start(VLG_TKT_INT_REPT_SHOT_TIMER_CAPTMSG_FLAS_MSEC);
}

void vlg_toolkit_MainWindow::OnFlashInfoMsg()
{
    if(reset_info_msg_tim_.isActive()) {
        if(++flash_info_msg_val_ % 2) {
            ui->main_info_label->setStyleSheet(
                QObject::tr("background-color : black; color : Red; font-weight: bold;"));
        } else {
            ui->main_info_label->setStyleSheet(
                QObject::tr("background-color : Red; color : black; font-weight: bold;"));
        }
    } else {
        flash_info_msg_val_ = 0;
        flash_info_msg_tim_.stop();
    }
}

void vlg_toolkit_MainWindow::OnResetInfoMsg()
{
    reset_info_msg_tim_.stop();
    ui->main_info_label->setText("");
    ui->main_info_label->setStyleSheet(
        QObject::tr("background-color : Azure; color : black;"));
}

void vlg_toolkit_MainWindow::EmitPeerStatus(vlg::PeerStatus status)
{
    emit Peer_status_change(status);
}

void vlg_toolkit_MainWindow::on_action_Stop_Peer_triggered()
{
    peer_.stop(true);
    //ui->peer_Tab->widget(0)->setEnabled(true);
}

void vlg_toolkit_MainWindow::Status_RUNNING_Actions()
{
    ui->peer_status_label_display->setText(QObject::tr("RUNNING"));
    ui->peer_status_label_display->setStyleSheet(QObject::tr("background-color : DarkSeaGreen; color : black;"));
    ui->action_Start_Peer->setEnabled(false);
    ui->action_Stop_Peer->setEnabled(true);
    ui->actionConnect->setEnabled(true);
    if(!view_model_loaded_) {
        view_model_loaded_ = true;
        AddNewModelTab();
        emit VLG_MODEL_Update_event();
    }
}

void vlg_toolkit_MainWindow::Status_STOPPED_Actions()
{
    ui->peer_status_label_display->setText(QObject::tr("STOPPED"));
    ui->peer_status_label_display->setStyleSheet(QObject::tr("background-color : Silver; color : black;"));
    ui->action_Start_Peer->setEnabled(true);
}

void vlg_toolkit_MainWindow::AddNewModelTab()
{
    vlg_toolkit_model_tab *mt = new vlg_toolkit_model_tab(vlg_model_loaded_model_,
                                                          ui->peer_Tab);
    QIcon icon_model;
    icon_model.addFile(QStringLiteral(":/icon/icons/social-buffer.png"), QSize(), QIcon::Normal, QIcon::Off);
    QString tab_name = QString("Peer Model");
    ui->peer_Tab->addTab(mt, icon_model, tab_name);
    connect(this, SIGNAL(VLG_MODEL_Update_event()), mt, SLOT(On_VLG_MODEL_Update()));
    QSortFilterProxyModel &mt_mod = mt->b_mdl();
    connect(this, SIGNAL(VLG_MODEL_Update_event()), &mt_mod, SLOT(invalidate()));
}

void vlg_toolkit_MainWindow::AddNewConnectionTab(const QString &host,
                                                 const QString &port,
                                                 const QString &usr,
                                                 const QString &psswd)
{
    vlg_toolkit_Connection *ct = new vlg_toolkit_Connection(peer_,
                                                            host,
                                                            port,
                                                            usr,
                                                            psswd,
                                                            vlg_model_loaded_model_,
                                                            *this,
                                                            ui->peer_Tab);
    QIcon icon_flash;
    vlg::ConnectivityEventResult con_evt_res = vlg::ConnectivityEventResult_UNDEFINED;
    vlg::ConnectivityEventType connectivity_evt_type = vlg::ConnectivityEventType_UNDEFINED;
    if(ct->conn().await_for_connection_result(con_evt_res,
                                              connectivity_evt_type,
                                              VLG_TKT_INT_AWT_TIMEOUT,
                                              0) == vlg::RetCode_TIMEOUT) {
        emit SignalNewConnectionTimeout(QString("establishing new connection"));
    }
    if(con_evt_res == vlg::ConnectivityEventResult_OK) {
        icon_flash.addFile(QStringLiteral(":/icon/icons/flash_green.png"), QSize(),
                           QIcon::Normal, QIcon::Off);
    } else {
        icon_flash.addFile(QStringLiteral(":/icon/icons/flash_red.png"), QSize(),
                           QIcon::Normal, QIcon::Off);
    }
    QString tab_name = QString("[%1][%2][%3]").arg(ct->tab_id()).arg(host, port);
    int tab_idx = ui->peer_Tab->addTab(ct, icon_flash, tab_name);
    ct->setTab_idx(tab_idx);
}

void vlg_toolkit_MainWindow::LoadDefPeerCfgFile()
{
    vlg::RetCode res = vlg::RetCode_OK;
    const char *fileName_cstr = "params";
    vlg::cfg_ldr peer_conf_ldr;
    if((res = peer_conf_ldr.init(fileName_cstr))) {
        qDebug() << "peer_conf_ldr.init() failed." << res;
        return;
    }
    if((res = peer_conf_ldr.load_config())) {
        qDebug() << "peer_conf_ldr.load_config() failed." << res;
        return;
    }
    peer_conf_ldr.enum_params(peer_params_clbk_ud, this);
}

void vlg_toolkit_MainWindow::on_actionConnect_triggered()
{
    vlg_toolkit_NewConnDlg conn_dlg(this);
    conn_dlg.exec();
    if(conn_dlg.result() == QDialog::Accepted) {
        AddNewConnectionTab(conn_dlg.ui->ln_edt_host->text(),
                            conn_dlg.ui->ln_edt_port->text(),
                            conn_dlg.ui->ln_edt_usr->text(),
                            conn_dlg.ui->ln_edt_psswd->text());
    } else {

    }
}

void vlg_toolkit_MainWindow::OnLogEvent(vlg::TraceLVL tlvl,
                                        const QString &msg)
{
    QString beginHtml;
    switch(tlvl) {
        case vlg::TL_PLN:
            beginHtml = "<p style=\"color: Black; background-color: White\">";
            break;
        case vlg::TL_LOW:
            beginHtml = "<p style=\"color: Beige; background-color: Black\">";
            break;
        case vlg::TL_TRC:
            beginHtml = "<p style=\"color: Beige; background-color: Black\">";
            break;
        case vlg::TL_DBG:
            beginHtml = "<p style=\"color: Khaki; background-color: Black\">";
            break;
        case vlg::TL_INF:
            beginHtml = "<p style=\"color: DarkSeaGreen; background-color: Black\">";
            break;
        case vlg::TL_WRN:
            beginHtml = "<p style=\"color: Orange; background-color: Black\">";
            break;
        case vlg::TL_ERR:
            beginHtml = "<p style=\"color: Red; background-color: Black\">";
            break;
        case vlg::TL_CRI:
            beginHtml = "<p style=\"color: White; background-color: Red\">";
            break;
        case vlg::TL_FAT:
            beginHtml = "<p style=\"color: Black; background-color: Red\">";
            break;
        default:
            break;
    }
    QString endHtml = "</p>";
    QString escp_msg = msg.toHtmlEscaped();
    QString line = beginHtml % escp_msg % endHtml;
    QTextCursor cursor = ui->logr_apnd_plainTextEdit->textCursor();
    ui->logr_apnd_plainTextEdit->appendHtml(line);
    cursor.movePosition(QTextCursor::End);
    ui->logr_apnd_plainTextEdit->setTextCursor(cursor);
}

void vlg_toolkit_MainWindow::on_peer_Tab_tabCloseRequested(int index)
{
    if(index == 0 || index == 1) {
        return;
    }
    vlg_toolkit_Connection *connwidg = static_cast<vlg_toolkit_Connection *>
                                       (ui->peer_Tab->widget(index));
    ui->peer_Tab->removeTab(index);
    delete connwidg;
}

void vlg_toolkit_MainWindow::on_actionLow_triggered()
{
    vlg::logger::set_level_for_all_loggers(vlg::TL_LOW);
}

void vlg_toolkit_MainWindow::on_actionTrace_triggered()
{
    vlg::logger::set_level_for_all_loggers(vlg::TL_TRC);
}

void vlg_toolkit_MainWindow::on_actionDebug_triggered()
{
    vlg::logger::set_level_for_all_loggers(vlg::TL_DBG);
}

void vlg_toolkit_MainWindow::on_actionInfo_triggered()
{
    vlg::logger::set_level_for_all_loggers(vlg::TL_INF);
}

void vlg_toolkit_MainWindow::on_actionWarning_triggered()
{
    vlg::logger::set_level_for_all_loggers(vlg::TL_WRN);
}

void vlg_toolkit_MainWindow::on_actionError_triggered()
{
    vlg::logger::set_level_for_all_loggers(vlg::TL_ERR);
}

void vlg_toolkit_MainWindow::on_actionCritical_triggered()
{
    vlg::logger::set_level_for_all_loggers(vlg::TL_CRI);
}

void vlg_toolkit_MainWindow::on_actionFatal_triggered()
{
    vlg::logger::set_level_for_all_loggers(vlg::TL_FAT);
}

void vlg_toolkit_MainWindow::on_actionClean_Console_triggered()
{
    ui->logr_apnd_plainTextEdit->clear();
}
