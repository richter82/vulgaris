/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "tkt_core_util.h"
#include "tkt_vlg_model.h"
#include "tkt_newconndlg.h"
#include "tkt_model_tab.h"
#include "tkt_connection.h"

class vlg_toolkit_MainWindow;

namespace vlg_tlkt {
//------------------------------------------------------------------------------
// ****VLG_TOOLKIT_PEER****
//------------------------------------------------------------------------------

class toolkit_broker : public vlg::broker {
    public:
        toolkit_broker(vlg_toolkit_MainWindow &widget);

    private:
        virtual const char *get_name() override;
        virtual const unsigned int *get_version() override;

    public:
        vlg_toolkit_MainWindow &widget_;
};

}

namespace Ui {
class vlg_toolkit_MainWindow;
}

class vlg_toolkit_MainWindow : public QMainWindow {
        Q_OBJECT

    protected:
        void closeEvent(QCloseEvent *event);

    public:
        explicit vlg_toolkit_MainWindow(QWidget *parent = nullptr);
        ~vlg_toolkit_MainWindow();

    public:
        void InitGuiConfig();

    private slots:
        void on_action_Exit_triggered();
        void on_action_Load_Config_triggered();
        void on_set_broker_params_button_clicked();
        void on_update_broker_model_button_clicked();
        void on_set_pers_driver_button_clicked();
        void on_action_Start_Broker_triggered();
        void on_action_Stop_Broker_triggered();
        void on_actionConnect_triggered();
        void on_broker_Tab_tabCloseRequested(int index);
        void on_actionTrace_triggered();
        void on_actionDebug_triggered();
        void on_actionInfo_triggered();
        void on_actionWarning_triggered();
        void on_actionError_triggered();
        void on_actionCritical_triggered();
        void on_actionClean_Console_triggered();

    public slots:
        void OnLogEvent(spdlog::level::level_enum tlvl, const QString &msg);
        void OnBroker_status_change(vlg::BrokerStatus status);
        void On_VLG_MODEL_Update();
        void OnSetInfoMsg(const QString &msg);
        void OnFlashInfoMsg();
        void OnResetInfoMsg();

    signals:
        void Broker_status_change(vlg::BrokerStatus status);
        void VLG_MODEL_Update_event();
        void SignalNewConnectionTimeout(const QString &msg);

    public:
        void EmitBrokerStatus(vlg::BrokerStatus status);

        friend void vlg_toolkit_broker_status_change_hndlr(vlg::broker &p,
                                                           vlg::BrokerStatus status,
                                                           void *ud);
    private:
        void Status_RUNNING_Actions();
        void Status_STOPPED_Actions();

    private:
        void AddNewModelTab();
        void AddNewConnectionTab(const QString &host,
                                 const QString &port,
                                 const QString &usr,
                                 const QString &psswd);
        void LoadDefBrokerCfgFile();

        //broker cfg loading
    public:
        static void broker_params_clbk_ud(int pnum,
                                          const char *param,
                                          const char *value,
                                          void *ud);

    private:
        vlg::RetCode BrokerLoadCfgHndl(int pnum,
                                       const char *param,
                                       const char *value);
        //broker
    private:
        vlg_tlkt::toolkit_broker broker_;

        //MODEL: VLG_MODEL(s) TO LOAD by FILE
    private:
        bool view_model_loaded_;
        QStringListModel vlgmodel_load_list_model_;

        //MODEL: PERS-DRIV(s) TO LOAD by FILE
    private:
        QStringListModel pers_dri_file_load_list_model_;

    private:
        //MODEL: VLG_MODEL(s) LOADED.
        vlg_toolkit_vlg_model vlg_model_loaded_model_;

        //static gui rep
    private:
        Ui::vlg_toolkit_MainWindow *ui;

    private:
        QTimer reset_info_msg_tim_;
        QTimer flash_info_msg_tim_;
        int flash_info_msg_val_;
        std::shared_ptr<vlg_tlkt::QPlainTextEditApnd> apnd_;
};
