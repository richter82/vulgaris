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

#ifndef BLZ_TOOLKIT_MAINWINDOW_H
#define BLZ_TOOLKIT_MAINWINDOW_H

#include "blz_toolkit_core_util.h"
#include "blz_toolkit_blz_model.h"
#include "blz_toolkit_peer.h"
#include "blz_toolkit_newconndlg.h"
#include "blz_toolkit_model_tab.h"
#include "blz_toolkit_connection.h"

namespace Ui {
class blz_toolkit_MainWindow;
}

class blz_toolkit_MainWindow : public QMainWindow {
        Q_OBJECT

    protected:
        void closeEvent(QCloseEvent *event);

    public:
        explicit blz_toolkit_MainWindow(QWidget *parent = 0);
        ~blz_toolkit_MainWindow();

    public:
        void InitGuiConfig();

    private slots:
        void on_action_Exit_triggered();
        void on_action_Load_Config_triggered();
        void on_set_peer_params_button_clicked();
        void on_update_peer_model_button_clicked();
        void on_set_pers_driver_button_clicked();
        void on_action_Start_Peer_triggered();
        void on_action_Stop_Peer_triggered();
        void on_actionConnect_triggered();
        void on_peer_Tab_tabCloseRequested(int index);
        void on_actionLow_triggered();
        void on_actionTrace_triggered();
        void on_actionFatal_triggered();
        void on_actionDebug_triggered();
        void on_actionInfo_triggered();
        void on_actionWarning_triggered();
        void on_actionError_triggered();
        void on_actionCritical_triggered();
        void on_actionClean_Console_triggered();

    public slots:
        void OnLogEvent(vlg::TraceLVL tlvl, const QString &msg);
        void OnPeer_status_change(vlg::PeerStatus status);
        void On_BLZ_MODEL_Update();
        void OnSetInfoMsg(const QString &msg);
        void OnFlashInfoMsg();
        void OnResetInfoMsg();

    signals:
        void Peer_status_change(vlg::PeerStatus status);
        void BLZ_MODEL_Update_event();
        void SignalNewConnectionTimeout(const QString &msg);

    public:
        void EmitPeerStatus(vlg::PeerStatus status);

        friend void blz_toolkit_peer_lfcyc_status_change_hndlr(vlg::peer_automa
                                                               &peer,
                                                               vlg::PeerStatus status,
                                                               void *ud);
    private:
        void Status_RUNNING_Actions();
        void Status_STOPPED_Actions();

    private:
        void AddNewModelTab();
        void AddNewConnectionTab(vlg::connection_int &new_conn,
                                 const QString &host,
                                 const QString &port,
                                 const QString &usr,
                                 const QString &psswd);
        void LoadDefPeerCfgFile();

        //peer cfg loading
    public:
        static void peer_params_clbk_ud(int pnum,
                                        const char *param,
                                        const char *value,
                                        void *ud);

    private:
        vlg::RetCode PeerLoadCfgHndl(int pnum,
                                       const char *param,
                                       const char *value);

        /******
        REP
        ****/

        //peer
    private:
        blz_tlkt::toolkit_peer  peer_;

        //MODEL: BLZ_MODEL(s) TO LOAD by FILE
    private:
        bool view_model_loaded_;
        QStringListModel blzmodel_load_list_model_;

        //MODEL: PERS-DRIV(s) TO LOAD by FILE
    private:
        QStringListModel pers_dri_file_load_list_model_;

    private:
        //MODEL: BLZ_MODEL(s) LOADED.
        blz_toolkit_blz_model blz_model_loaded_model_;

        //business-rep
    private:
        blz_tlkt::QPlainTextEditApnd pte_apnd_;

        //static gui rep
    private:
        Ui::blz_toolkit_MainWindow *ui;

    private:
        QTimer reset_info_msg_tim_;
        QTimer flash_info_msg_tim_;
        int flash_info_msg_val_;
};

#endif // BLZ_TOOLKIT_MAINWINDOW_H
