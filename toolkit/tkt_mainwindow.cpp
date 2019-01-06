/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "tkt_mainwindow.h"
#include "ui_tkt_mainwindow.h"
#include "cfg.h"

namespace vlg_tlkt {

static unsigned int VLG_TOOLKIT_PEER_VER[] = {0, 0, 0, 0};

//------------------------------------------------------------------------------
// ****VLG_TOOLKIT_PEER****
//------------------------------------------------------------------------------

struct toolkit_broker_listener : public vlg::broker_listener {
    virtual vlg::RetCode on_load_config(vlg::broker &,
                                        int,
                                        const char *,
                                        const char *) override {
        return vlg::RetCode_OK;
    }

    virtual ~toolkit_broker_listener() = default;

    virtual vlg::RetCode on_init(vlg::broker &) override {
        return vlg::RetCode_OK;
    }
    virtual vlg::RetCode on_starting(vlg::broker &) override {
        return vlg::RetCode_OK;
    }
    virtual vlg::RetCode on_stopping(vlg::broker &) override {
        return vlg::RetCode_OK;
    }
    virtual vlg::RetCode on_move_running(vlg::broker &) override {
        return vlg::RetCode_OK;
    }
    virtual void on_error(vlg::broker &) override {}

    void on_status_change(vlg::broker &p, vlg::BrokerStatus current) override {
        qDebug() << "broker status:" << current;
        ((toolkit_broker &)p).widget_.EmitBrokerStatus(current);
    }

    virtual vlg::RetCode on_incoming_connection(vlg::broker &, std::shared_ptr<vlg::incoming_connection> &) override {
        return vlg::RetCode_OK;
    }
};

static toolkit_broker_listener tpl;

toolkit_broker::toolkit_broker(vlg_toolkit_MainWindow &widget) : vlg::broker(tpl), widget_(widget)
{}

const char *toolkit_broker::get_name()
{
    return "toolkit_broker[" __DATE__ "]";
}

const unsigned int *toolkit_broker::get_version()
{
    return VLG_TOOLKIT_PEER_VER;
}

}

vlg_toolkit_MainWindow::vlg_toolkit_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    broker_(*this),
    view_model_loaded_(false),
    vlgmodel_load_list_model_(this),
    pers_dri_file_load_list_model_(this),
    vlg_model_loaded_model_(broker_.get_nentity_manager(), this),
    ui(new Ui::vlg_toolkit_MainWindow),
    flash_info_msg_val_(0),
    apnd_(new vlg_tlkt::QPlainTextEditApnd(this))
{
    ui->setupUi(this);
    InitGuiConfig();

    //broker_ logger cfg bgn
    spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {
        l->sinks().push_back(apnd_);
    });
    //broker_ logger cfg end

    //cfg broker models-view connection
    ui->broker_cfg_view_list_vlgmodel->setModel(&vlgmodel_load_list_model_);
    ui->broker_cfg_view_list_vlgpersdriv->setModel(&pers_dri_file_load_list_model_);

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

    LoadDefBrokerCfgFile();
}

vlg_toolkit_MainWindow::~vlg_toolkit_MainWindow()
{
    delete ui;
    ui = nullptr;
}

void vlg_toolkit_MainWindow::InitGuiConfig()
{
    qRegisterMetaType<spdlog::level::level_enum>("spdlog::level::level_enum");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<vlg::BrokerStatus>("vlg::BrokerStatus");
    qRegisterMetaType<vlg::ConnectionStatus>("vlg::ConnectionStatus");
    qRegisterMetaType<vlg::TransactionStatus>("vlg::TransactionStatus");
    qRegisterMetaType<vlg::SubscriptionStatus>("vlg::SubscriptionStatus");
    qRegisterMetaType<std::shared_ptr<vlg::subscription_event>>("std::shared_ptr<vlg::subscription_event>");
    qRegisterMetaType<VLG_SBS_COL_DATA_ENTRY *>("VLG_SBS_COL_DATA_ENTRY");

    ui->action_Start_Broker->setDisabled(true);
    ui->action_Stop_Broker->setDisabled(true);
    ui->actionConnect->setDisabled(true);

    connect(apnd_.get(),
            SIGNAL(messageReady(spdlog::level::level_enum, const QString &)),
            this,
            SLOT(OnLogEvent(spdlog::level::level_enum, const QString &)));

    connect(this,
            SIGNAL(Broker_status_change(vlg::BrokerStatus)),
            this,
            SLOT(OnBroker_status_change(vlg::BrokerStatus)));

    //timout
    connect(this,
            SIGNAL(SignalNewConnectionTimeout(const QString &)),
            this,
            SLOT(OnSetInfoMsg(const QString &)));

    connect(&flash_info_msg_tim_, SIGNAL(timeout()), this, SLOT(OnFlashInfoMsg()));
    connect(&reset_info_msg_tim_, SIGNAL(timeout()), this, SLOT(OnResetInfoMsg()));

    //set cfg all button
    connect(ui->set_broker_cfg_all_button, SIGNAL(clicked()), this,
            SLOT(on_set_broker_params_button_clicked()));
    connect(ui->set_broker_cfg_all_button, SIGNAL(clicked()), this,
            SLOT(on_update_broker_model_button_clicked()));
    connect(ui->set_broker_cfg_all_button, SIGNAL(clicked()), this,
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
    broker_.stop(true);
    vlg::BrokerStatus current = vlg::BrokerStatus_ZERO;
    broker_.await_for_status_reached(vlg::BrokerStatus_STOPPED, current);
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

void vlg_toolkit_MainWindow::broker_params_clbk_ud(int pnum,
                                                   const char *param,
                                                   const char *value,
                                                   void *ud)
{
    vlg_toolkit_MainWindow *mw = (vlg_toolkit_MainWindow *)ud;
    mw->BrokerLoadCfgHndl(pnum, param, value);
}

//------------------------------------------------------------------------------
// vlg_toolkit_MainWindow::BrokerLoadCfgHndl
//------------------------------------------------------------------------------

vlg::RetCode vlg_toolkit_MainWindow::BrokerLoadCfgHndl(int,
                                                       const char *param,
                                                       const char *value)
{
    if(!strcmp(param, "pure_server")) {
        ui->pp_cfg_broker_personality->setCurrentIndex(1);
    }

    if(!strcmp(param, "pure_client")) {
        ui->pp_cfg_broker_personality->setCurrentIndex(0);
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
    vlg::cfg_ldr broker_conf_ldr;
    if((res = broker_conf_ldr.init(fileName_cstr))) {
        qDebug() << "broker_conf_ldr.Init() failed." << res;
        return;
    }
    if((res = broker_conf_ldr.load_config())) {
        qDebug() << "broker_conf_ldr.LoadCfg() failed." << res;
        return;
    }
    broker_conf_ldr.enum_params(broker_params_clbk_ud, this);
}

void vlg_toolkit_MainWindow::on_set_broker_params_button_clicked()
{
    switch(ui->pp_cfg_broker_personality->currentIndex()) {
        case 0:
            broker_.set_personality(vlg::BrokerPersonality_PURE_CLIENT);
            break;
        case 1:
            broker_.set_personality(vlg::BrokerPersonality_PURE_SERVER);
            break;
        case 2:
            broker_.set_personality(vlg::BrokerPersonality_BOTH);
            break;
        default:
            break;
    }

    if(ui->pp_cfg_srv_sin_addr->text() != "") {
        broker_.set_server_address(ui->pp_cfg_srv_sin_addr->text().toLocal8Bit().data());
    }
    broker_.set_server_port(atoi(ui->pp_cfg_srv_sin_port->text().toLocal8Bit().data()));
    broker_.set_server_transaction_service_executor_size(atoi(ui->pp_cfg_srv_exectrs->text().toLocal8Bit().data()));
    broker_.set_client_transaction_service_executor_size(atoi(ui->pp_cfg_cli_exectrs->text().toLocal8Bit().data()));
    broker_.set_server_subscription_service_executor_size(atoi(ui->pp_cfg_srv_sbs_exectrs->text().toLocal8Bit().data()));
    broker_.set_persistent(ui->pp_cfg_pers_enabled->isChecked());
    broker_.set_create_persistent_schema(ui->pp_cfg_pers_schema_create->isChecked());
    broker_.set_drop_existing_persistent_schema(ui->pp_cfg_drop_existing_schema->isChecked());
    broker_.set_configured(true);
    ui->broker_cfg_status_label->setText(tr("Set"));
    ui->broker_cfg_status_label->setStyleSheet(tr("background-color : LightGreen; color : black;"));
    ui->action_Start_Broker->setEnabled(true);
}

void vlg_toolkit_MainWindow::on_update_broker_model_button_clicked()
{
    int numRows = vlgmodel_load_list_model_.rowCount();
    for(int row = 0; row < numRows; ++row) {
        QModelIndex index = vlgmodel_load_list_model_.index(row, 0);
        broker_.add_load_model(vlgmodel_load_list_model_.data(index,
                                                              Qt::DisplayRole).toString().toLocal8Bit().data());
    }
    ui->broker_model_label_status->setText(tr("Set"));
    ui->broker_model_label_status->setStyleSheet(
        tr("background-color : LightGreen; color : black;"));
}

void vlg_toolkit_MainWindow::on_set_pers_driver_button_clicked()
{
    int numRows = pers_dri_file_load_list_model_.rowCount();
    for(int row = 0; row < numRows; ++row) {
        QModelIndex index = pers_dri_file_load_list_model_.index(row, 0);
        broker_.add_load_persistent_driver(pers_dri_file_load_list_model_.data(index,
                                                                               Qt::DisplayRole).toString().toLocal8Bit().data());
    }
    ui->broker_pers_driv_status_label->setText(tr("Set"));
    ui->broker_pers_driv_status_label->setStyleSheet(
        tr("background-color : LightGreen; color : black;"));
}

void vlg_toolkit_MainWindow::on_action_Start_Broker_triggered()
{
    ui->broker_Tab->widget(0)->setEnabled(false);
    broker_.start(0, nullptr, true);
}

void vlg_toolkit_MainWindow::OnBroker_status_change(vlg::BrokerStatus status)
{
    switch(status) {
        case vlg::BrokerStatus_ZERO:
            ui->broker_status_label_display->setText(QObject::tr("ZERO"));
            break;
        case vlg::BrokerStatus_EARLY:
            ui->broker_status_label_display->setText(QObject::tr("EARLY"));
            ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : Beige; color : black;"));
            break;
        case vlg::BrokerStatus_WELCOMED:
            ui->broker_status_label_display->setText(QObject::tr("WELCOMED"));
            ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : Cornsilk; color : black;"));
            break;
        case vlg::BrokerStatus_INITIALIZING:
            ui->broker_status_label_display->setText(QObject::tr("INITIALIZING"));
            ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : PowderBlue; color : black;"));
            break;
        case vlg::BrokerStatus_INITIALIZED:
            ui->broker_status_label_display->setText(QObject::tr("INITIALIZED"));
            ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : Aquamarine; color : black;"));
            break;
        case vlg::BrokerStatus_STARTING:
            ui->broker_status_label_display->setText(QObject::tr("STARTING"));
            ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : Coral; color : black;"));
            break;
        case vlg::BrokerStatus_STARTED:
            ui->broker_status_label_display->setText(QObject::tr("STARTED"));
            ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : DarkOrange; color : black;"));
            break;
        case vlg::BrokerStatus_RUNNING:
            Status_RUNNING_Actions();
            break;
        case vlg::BrokerStatus_STOP_REQUESTED:
            ui->broker_status_label_display->setText(QObject::tr("STOP REQUESTED"));
            ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : Indigo; color : white;"));
            ui->action_Stop_Broker->setEnabled(false);
            ui->actionConnect->setEnabled(false);
            break;
        case vlg::BrokerStatus_STOPPING:
            ui->broker_status_label_display->setText(QObject::tr("STOPPING"));
            ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : IndianRed; color : black;"));
            break;
        case vlg::BrokerStatus_STOPPED:
            Status_STOPPED_Actions();
            break;
        case vlg::BrokerStatus_ERROR:
            ui->broker_status_label_display->setText(QObject::tr("ERROR"));
            ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : Red; color : black;"));
            Status_STOPPED_Actions();
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
    ui->main_info_label->setStyleSheet(QObject::tr("background-color : Red; color : black; font-weight: bold;"));
    reset_info_msg_tim_.start(VLG_TKT_INT_SINGL_SHOT_TIMER_CAPTMSG_RST_MSEC);
    flash_info_msg_tim_.start(VLG_TKT_INT_REPT_SHOT_TIMER_CAPTMSG_FLAS_MSEC);
}

void vlg_toolkit_MainWindow::OnFlashInfoMsg()
{
    if(reset_info_msg_tim_.isActive()) {
        if(++flash_info_msg_val_ % 2) {
            ui->main_info_label->setStyleSheet(QObject::tr("background-color : black; color : Red; font-weight: bold;"));
        } else {
            ui->main_info_label->setStyleSheet(QObject::tr("background-color : Red; color : black; font-weight: bold;"));
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

void vlg_toolkit_MainWindow::EmitBrokerStatus(vlg::BrokerStatus status)
{
    emit Broker_status_change(status);
}

void vlg_toolkit_MainWindow::on_action_Stop_Broker_triggered()
{
    broker_.stop(true);
    //ui->broker_Tab->widget(0)->setEnabled(true);
}

void vlg_toolkit_MainWindow::Status_RUNNING_Actions()
{
    ui->broker_status_label_display->setText(QObject::tr("RUNNING"));
    ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : DarkSeaGreen; color : black;"));
    ui->action_Start_Broker->setEnabled(false);
    ui->action_Stop_Broker->setEnabled(true);
    ui->actionConnect->setEnabled(true);
    if(!view_model_loaded_) {
        view_model_loaded_ = true;
        AddNewModelTab();
        emit VLG_MODEL_Update_event();
    }
}

void vlg_toolkit_MainWindow::Status_STOPPED_Actions()
{
    ui->broker_status_label_display->setText(QObject::tr("STOPPED"));
    ui->broker_status_label_display->setStyleSheet(QObject::tr("background-color : Silver; color : black;"));
    ui->action_Start_Broker->setEnabled(true);
}

void vlg_toolkit_MainWindow::AddNewModelTab()
{
    vlg_toolkit_model_tab *mt = new vlg_toolkit_model_tab(vlg_model_loaded_model_,
                                                          ui->broker_Tab);
    QIcon icon_model;
    icon_model.addFile(QStringLiteral(":/icon/icons/social-buffer.png"), QSize(), QIcon::Normal, QIcon::Off);
    QString tab_name = QString("Broker Model");
    ui->broker_Tab->addTab(mt, icon_model, tab_name);
    connect(this, SIGNAL(VLG_MODEL_Update_event()), mt, SLOT(On_VLG_MODEL_Update()));
    QSortFilterProxyModel &mt_mod = mt->b_mdl();
    connect(this, SIGNAL(VLG_MODEL_Update_event()), &mt_mod, SLOT(invalidate()));
}

void vlg_toolkit_MainWindow::AddNewConnectionTab(const QString &host,
                                                 const QString &port,
                                                 const QString &usr,
                                                 const QString &psswd)
{
    vlg_toolkit_Connection *ct = new vlg_toolkit_Connection(broker_,
                                                            host,
                                                            port,
                                                            usr,
                                                            psswd,
                                                            vlg_model_loaded_model_,
                                                            *this,
                                                            ui->broker_Tab);
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
    int tab_idx = ui->broker_Tab->addTab(ct, icon_flash, tab_name);
    ct->setTab_idx(tab_idx);
}

void vlg_toolkit_MainWindow::LoadDefBrokerCfgFile()
{
    vlg::RetCode res = vlg::RetCode_OK;
    const char *fileName_cstr = "params";
    vlg::cfg_ldr broker_conf_ldr;
    if((res = broker_conf_ldr.init(fileName_cstr))) {
        qDebug() << "broker_conf_ldr.init() failed." << res;
        return;
    }
    if((res = broker_conf_ldr.load_config())) {
        qDebug() << "broker_conf_ldr.load_config() failed." << res;
        return;
    }
    broker_conf_ldr.enum_params(broker_params_clbk_ud, this);
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

void vlg_toolkit_MainWindow::OnLogEvent(spdlog::level::level_enum tlvl,
                                        const QString &msg)
{
    if(!ui) {
        return;
    }
    QString beginHtml;
    switch(tlvl) {
        case spdlog::level::level_enum::trace:
            beginHtml = "<p style=\"color: Beige; background-color: Black\">";
            break;
        case spdlog::level::level_enum::debug:
            beginHtml = "<p style=\"color: Khaki; background-color: Black\">";
            break;
        case spdlog::level::level_enum::info:
            beginHtml = "<p style=\"color: DarkSeaGreen; background-color: Black\">";
            break;
        case spdlog::level::level_enum::warn:
            beginHtml = "<p style=\"color: Orange; background-color: Black\">";
            break;
        case spdlog::level::level_enum::err:
            beginHtml = "<p style=\"color: Red; background-color: Black\">";
            break;
        case spdlog::level::level_enum::critical:
            beginHtml = "<p style=\"color: White; background-color: Red\">";
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

void vlg_toolkit_MainWindow::on_broker_Tab_tabCloseRequested(int index)
{
    if(index == 0 || index == 1) {
        return;
    }
    vlg_toolkit_Connection *connwidg = static_cast<vlg_toolkit_Connection *>
                                       (ui->broker_Tab->widget(index));
    ui->broker_Tab->removeTab(index);
    delete connwidg;
}

void vlg_toolkit_MainWindow::on_actionTrace_triggered()
{
    spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {
        l->set_level(spdlog::level::level_enum::trace);
    });
}

void vlg_toolkit_MainWindow::on_actionDebug_triggered()
{
    spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {
        l->set_level(spdlog::level::level_enum::debug);
    });
}

void vlg_toolkit_MainWindow::on_actionInfo_triggered()
{
    spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {
        l->set_level(spdlog::level::level_enum::info);
    });
}

void vlg_toolkit_MainWindow::on_actionWarning_triggered()
{
    spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {
        l->set_level(spdlog::level::level_enum::warn);
    });
}

void vlg_toolkit_MainWindow::on_actionError_triggered()
{
    spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {
        l->set_level(spdlog::level::level_enum::err);
    });
}

void vlg_toolkit_MainWindow::on_actionCritical_triggered()
{
    spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {
        l->set_level(spdlog::level::level_enum::critical);
    });
}

void vlg_toolkit_MainWindow::on_actionClean_Console_triggered()
{
    ui->logr_apnd_plainTextEdit->clear();
}
