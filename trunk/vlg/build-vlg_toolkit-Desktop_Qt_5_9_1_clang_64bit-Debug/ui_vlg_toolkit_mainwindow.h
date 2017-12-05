/********************************************************************************
** Form generated from reading UI file 'vlg_toolkit_mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VLG_TOOLKIT_MAINWINDOW_H
#define UI_VLG_TOOLKIT_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_vlg_toolkit_MainWindow
{
public:
    QAction *action_Load_Config;
    QAction *action_Exit;
    QAction *actionConnect;
    QAction *action_Start_Peer;
    QAction *action_Stop_Peer;
    QAction *actionTrace;
    QAction *actionDebug;
    QAction *actionInfo;
    QAction *actionWarning;
    QAction *actionError;
    QAction *actionCritical;
    QAction *actionFatal;
    QAction *actionClean_Console;
    QAction *actionLow;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_9;
    QFrame *peer_Status;
    QHBoxLayout *horizontalLayout;
    QLabel *peer_status_label_display;
    QLabel *main_info_label;
    QSplitter *splitter;
    QTabWidget *peer_Tab;
    QWidget *peer_config;
    QVBoxLayout *verticalLayout_6;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *set_peer_cfg_all_button;
    QFrame *peer_Status_4;
    QHBoxLayout *horizontalLayout_8;
    QLabel *peer_status_label_4;
    QLabel *peer_cfg_status_label;
    QFrame *peer_Status_3;
    QHBoxLayout *horizontalLayout_7;
    QLabel *peer_status_label_3;
    QLabel *peer_model_label_status;
    QFrame *peer_Status_2;
    QHBoxLayout *horizontalLayout_6;
    QLabel *peer_status_label_2;
    QLabel *peer_pers_driv_status_label;
    QSpacerItem *horizontalSpacer_4;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_5;
    QComboBox *pp_cfg_peer_personality;
    QGridLayout *gridLayout;
    QLineEdit *pp_cfg_srv_sin_port;
    QLineEdit *pp_cfg_srv_sin_addr;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *pp_cfg_srv_exectrs;
    QLabel *label_4;
    QLineEdit *pp_cfg_cli_exectrs;
    QLabel *label;
    QLabel *label_9;
    QLineEdit *pp_cfg_srv_sbs_exectrs;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *pp_cfg_pers_enabled;
    QCheckBox *pp_cfg_pers_schema_create;
    QCheckBox *pp_cfg_drop_existing_schema;
    QPushButton *set_peer_params_button;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_7;
    QListView *peer_cfg_view_list_vlgmodel;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *update_peer_model_button;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_8;
    QListView *peer_cfg_view_list_vlgpersdriv;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *set_pers_driver_button;
    QSpacerItem *verticalSpacer;
    QPlainTextEdit *logr_apnd_plainTextEdit;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menuConnection;
    QMenu *menuAbout;
    QMenu *menuPeer;
    QMenu *menuLog;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *vlg_toolkit_MainWindow)
    {
        if (vlg_toolkit_MainWindow->objectName().isEmpty())
            vlg_toolkit_MainWindow->setObjectName(QStringLiteral("vlg_toolkit_MainWindow"));
        vlg_toolkit_MainWindow->resize(924, 729);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(vlg_toolkit_MainWindow->sizePolicy().hasHeightForWidth());
        vlg_toolkit_MainWindow->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icon/icons/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        vlg_toolkit_MainWindow->setWindowIcon(icon);
        vlg_toolkit_MainWindow->setIconSize(QSize(32, 32));
        action_Load_Config = new QAction(vlg_toolkit_MainWindow);
        action_Load_Config->setObjectName(QStringLiteral("action_Load_Config"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icon/icons/gear-b.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Load_Config->setIcon(icon1);
        action_Exit = new QAction(vlg_toolkit_MainWindow);
        action_Exit->setObjectName(QStringLiteral("action_Exit"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icon/icons/log-out.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Exit->setIcon(icon2);
        actionConnect = new QAction(vlg_toolkit_MainWindow);
        actionConnect->setObjectName(QStringLiteral("actionConnect"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icon/icons/flash.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionConnect->setIcon(icon3);
        action_Start_Peer = new QAction(vlg_toolkit_MainWindow);
        action_Start_Peer->setObjectName(QStringLiteral("action_Start_Peer"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icon/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Start_Peer->setIcon(icon4);
        action_Stop_Peer = new QAction(vlg_toolkit_MainWindow);
        action_Stop_Peer->setObjectName(QStringLiteral("action_Stop_Peer"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icon/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Stop_Peer->setIcon(icon5);
        actionTrace = new QAction(vlg_toolkit_MainWindow);
        actionTrace->setObjectName(QStringLiteral("actionTrace"));
        actionDebug = new QAction(vlg_toolkit_MainWindow);
        actionDebug->setObjectName(QStringLiteral("actionDebug"));
        actionInfo = new QAction(vlg_toolkit_MainWindow);
        actionInfo->setObjectName(QStringLiteral("actionInfo"));
        actionWarning = new QAction(vlg_toolkit_MainWindow);
        actionWarning->setObjectName(QStringLiteral("actionWarning"));
        actionError = new QAction(vlg_toolkit_MainWindow);
        actionError->setObjectName(QStringLiteral("actionError"));
        actionCritical = new QAction(vlg_toolkit_MainWindow);
        actionCritical->setObjectName(QStringLiteral("actionCritical"));
        actionFatal = new QAction(vlg_toolkit_MainWindow);
        actionFatal->setObjectName(QStringLiteral("actionFatal"));
        actionClean_Console = new QAction(vlg_toolkit_MainWindow);
        actionClean_Console->setObjectName(QStringLiteral("actionClean_Console"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/icon/icons/trash-b.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClean_Console->setIcon(icon6);
        actionLow = new QAction(vlg_toolkit_MainWindow);
        actionLow->setObjectName(QStringLiteral("actionLow"));
        centralWidget = new QWidget(vlg_toolkit_MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(3);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(-1, -1, -1, 1);
        frame_2 = new QFrame(centralWidget);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        frame_2->setLineWidth(0);
        horizontalLayout_9 = new QHBoxLayout(frame_2);
        horizontalLayout_9->setSpacing(0);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        peer_Status = new QFrame(frame_2);
        peer_Status->setObjectName(QStringLiteral("peer_Status"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(peer_Status->sizePolicy().hasHeightForWidth());
        peer_Status->setSizePolicy(sizePolicy1);
        peer_Status->setAutoFillBackground(true);
        peer_Status->setFrameShape(QFrame::Box);
        peer_Status->setFrameShadow(QFrame::Sunken);
        horizontalLayout = new QHBoxLayout(peer_Status);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(2, 2, 2, 2);
        peer_status_label_display = new QLabel(peer_Status);
        peer_status_label_display->setObjectName(QStringLiteral("peer_status_label_display"));
        sizePolicy1.setHeightForWidth(peer_status_label_display->sizePolicy().hasHeightForWidth());
        peer_status_label_display->setSizePolicy(sizePolicy1);
        peer_status_label_display->setMinimumSize(QSize(60, 0));
        QFont font;
        font.setPointSize(12);
        font.setBold(false);
        font.setWeight(50);
        peer_status_label_display->setFont(font);
        peer_status_label_display->setStyleSheet(QStringLiteral("background-color  : AntiqueWhite; color : black;"));
        peer_status_label_display->setLineWidth(0);
        peer_status_label_display->setAlignment(Qt::AlignCenter);
        peer_status_label_display->setMargin(0);

        horizontalLayout->addWidget(peer_status_label_display);


        horizontalLayout_9->addWidget(peer_Status);

        main_info_label = new QLabel(frame_2);
        main_info_label->setObjectName(QStringLiteral("main_info_label"));
        main_info_label->setStyleSheet(QStringLiteral("background-color  : Azure ; color : black;"));

        horizontalLayout_9->addWidget(main_info_label);


        verticalLayout_2->addWidget(frame_2);

        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        peer_Tab = new QTabWidget(splitter);
        peer_Tab->setObjectName(QStringLiteral("peer_Tab"));
        peer_Tab->setMinimumSize(QSize(0, 500));
        peer_Tab->setTabsClosable(true);
        peer_config = new QWidget();
        peer_config->setObjectName(QStringLiteral("peer_config"));
        verticalLayout_6 = new QVBoxLayout(peer_config);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        groupBox_4 = new QGroupBox(peer_config);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        QFont font1;
        font1.setKerning(true);
        groupBox_4->setFont(font1);
        horizontalLayout_11 = new QHBoxLayout(groupBox_4);
        horizontalLayout_11->setSpacing(1);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        horizontalLayout_11->setContentsMargins(1, 1, 1, 1);
        set_peer_cfg_all_button = new QPushButton(groupBox_4);
        set_peer_cfg_all_button->setObjectName(QStringLiteral("set_peer_cfg_all_button"));
        set_peer_cfg_all_button->setStyleSheet(QStringLiteral(""));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/icon/icons/checkmark-circled.png"), QSize(), QIcon::Normal, QIcon::Off);
        set_peer_cfg_all_button->setIcon(icon7);

        horizontalLayout_11->addWidget(set_peer_cfg_all_button);

        peer_Status_4 = new QFrame(groupBox_4);
        peer_Status_4->setObjectName(QStringLiteral("peer_Status_4"));
        sizePolicy1.setHeightForWidth(peer_Status_4->sizePolicy().hasHeightForWidth());
        peer_Status_4->setSizePolicy(sizePolicy1);
        peer_Status_4->setAutoFillBackground(true);
        peer_Status_4->setFrameShape(QFrame::Box);
        peer_Status_4->setFrameShadow(QFrame::Sunken);
        horizontalLayout_8 = new QHBoxLayout(peer_Status_4);
        horizontalLayout_8->setSpacing(2);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(2, 2, 2, 2);
        peer_status_label_4 = new QLabel(peer_Status_4);
        peer_status_label_4->setObjectName(QStringLiteral("peer_status_label_4"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(peer_status_label_4->sizePolicy().hasHeightForWidth());
        peer_status_label_4->setSizePolicy(sizePolicy2);
        peer_status_label_4->setMinimumSize(QSize(0, 0));
        QFont font2;
        font2.setBold(true);
        font2.setWeight(75);
        peer_status_label_4->setFont(font2);

        horizontalLayout_8->addWidget(peer_status_label_4);

        peer_cfg_status_label = new QLabel(peer_Status_4);
        peer_cfg_status_label->setObjectName(QStringLiteral("peer_cfg_status_label"));
        sizePolicy1.setHeightForWidth(peer_cfg_status_label->sizePolicy().hasHeightForWidth());
        peer_cfg_status_label->setSizePolicy(sizePolicy1);
        peer_cfg_status_label->setMinimumSize(QSize(60, 0));
        peer_cfg_status_label->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        peer_cfg_status_label->setAlignment(Qt::AlignCenter);
        peer_cfg_status_label->setMargin(0);

        horizontalLayout_8->addWidget(peer_cfg_status_label);


        horizontalLayout_11->addWidget(peer_Status_4);

        peer_Status_3 = new QFrame(groupBox_4);
        peer_Status_3->setObjectName(QStringLiteral("peer_Status_3"));
        sizePolicy1.setHeightForWidth(peer_Status_3->sizePolicy().hasHeightForWidth());
        peer_Status_3->setSizePolicy(sizePolicy1);
        peer_Status_3->setAutoFillBackground(true);
        peer_Status_3->setFrameShape(QFrame::Box);
        peer_Status_3->setFrameShadow(QFrame::Sunken);
        horizontalLayout_7 = new QHBoxLayout(peer_Status_3);
        horizontalLayout_7->setSpacing(2);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(2, 2, 2, 2);
        peer_status_label_3 = new QLabel(peer_Status_3);
        peer_status_label_3->setObjectName(QStringLiteral("peer_status_label_3"));
        sizePolicy2.setHeightForWidth(peer_status_label_3->sizePolicy().hasHeightForWidth());
        peer_status_label_3->setSizePolicy(sizePolicy2);
        peer_status_label_3->setMinimumSize(QSize(0, 0));
        peer_status_label_3->setFont(font2);

        horizontalLayout_7->addWidget(peer_status_label_3);

        peer_model_label_status = new QLabel(peer_Status_3);
        peer_model_label_status->setObjectName(QStringLiteral("peer_model_label_status"));
        sizePolicy1.setHeightForWidth(peer_model_label_status->sizePolicy().hasHeightForWidth());
        peer_model_label_status->setSizePolicy(sizePolicy1);
        peer_model_label_status->setMinimumSize(QSize(60, 0));
        peer_model_label_status->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        peer_model_label_status->setAlignment(Qt::AlignCenter);
        peer_model_label_status->setMargin(0);

        horizontalLayout_7->addWidget(peer_model_label_status);


        horizontalLayout_11->addWidget(peer_Status_3);

        peer_Status_2 = new QFrame(groupBox_4);
        peer_Status_2->setObjectName(QStringLiteral("peer_Status_2"));
        sizePolicy1.setHeightForWidth(peer_Status_2->sizePolicy().hasHeightForWidth());
        peer_Status_2->setSizePolicy(sizePolicy1);
        peer_Status_2->setAutoFillBackground(true);
        peer_Status_2->setFrameShape(QFrame::Box);
        peer_Status_2->setFrameShadow(QFrame::Sunken);
        horizontalLayout_6 = new QHBoxLayout(peer_Status_2);
        horizontalLayout_6->setSpacing(2);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(2, 2, 2, 2);
        peer_status_label_2 = new QLabel(peer_Status_2);
        peer_status_label_2->setObjectName(QStringLiteral("peer_status_label_2"));
        sizePolicy2.setHeightForWidth(peer_status_label_2->sizePolicy().hasHeightForWidth());
        peer_status_label_2->setSizePolicy(sizePolicy2);
        peer_status_label_2->setMinimumSize(QSize(0, 0));
        peer_status_label_2->setFont(font2);

        horizontalLayout_6->addWidget(peer_status_label_2);

        peer_pers_driv_status_label = new QLabel(peer_Status_2);
        peer_pers_driv_status_label->setObjectName(QStringLiteral("peer_pers_driv_status_label"));
        sizePolicy1.setHeightForWidth(peer_pers_driv_status_label->sizePolicy().hasHeightForWidth());
        peer_pers_driv_status_label->setSizePolicy(sizePolicy1);
        peer_pers_driv_status_label->setMinimumSize(QSize(60, 0));
        peer_pers_driv_status_label->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        peer_pers_driv_status_label->setAlignment(Qt::AlignCenter);
        peer_pers_driv_status_label->setMargin(0);

        horizontalLayout_6->addWidget(peer_pers_driv_status_label);


        horizontalLayout_11->addWidget(peer_Status_2);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_4);


        verticalLayout_6->addWidget(groupBox_4);

        frame = new QFrame(peer_config);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy3);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(frame);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        groupBox->setMinimumSize(QSize(230, 320));
        verticalLayout_9 = new QVBoxLayout(groupBox);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_2->addWidget(label_5);

        pp_cfg_peer_personality = new QComboBox(groupBox);
        pp_cfg_peer_personality->setObjectName(QStringLiteral("pp_cfg_peer_personality"));
        pp_cfg_peer_personality->setMaxVisibleItems(10);
        pp_cfg_peer_personality->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

        horizontalLayout_2->addWidget(pp_cfg_peer_personality);


        verticalLayout_9->addLayout(horizontalLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pp_cfg_srv_sin_port = new QLineEdit(groupBox);
        pp_cfg_srv_sin_port->setObjectName(QStringLiteral("pp_cfg_srv_sin_port"));

        gridLayout->addWidget(pp_cfg_srv_sin_port, 1, 1, 1, 1);

        pp_cfg_srv_sin_addr = new QLineEdit(groupBox);
        pp_cfg_srv_sin_addr->setObjectName(QStringLiteral("pp_cfg_srv_sin_addr"));

        gridLayout->addWidget(pp_cfg_srv_sin_addr, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        pp_cfg_srv_exectrs = new QLineEdit(groupBox);
        pp_cfg_srv_exectrs->setObjectName(QStringLiteral("pp_cfg_srv_exectrs"));

        gridLayout->addWidget(pp_cfg_srv_exectrs, 2, 1, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 2, 0, 1, 1);

        pp_cfg_cli_exectrs = new QLineEdit(groupBox);
        pp_cfg_cli_exectrs->setObjectName(QStringLiteral("pp_cfg_cli_exectrs"));

        gridLayout->addWidget(pp_cfg_cli_exectrs, 3, 1, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 3, 0, 1, 1);

        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout->addWidget(label_9, 4, 0, 1, 1);

        pp_cfg_srv_sbs_exectrs = new QLineEdit(groupBox);
        pp_cfg_srv_sbs_exectrs->setObjectName(QStringLiteral("pp_cfg_srv_sbs_exectrs"));

        gridLayout->addWidget(pp_cfg_srv_sbs_exectrs, 4, 1, 1, 1);


        verticalLayout_9->addLayout(gridLayout);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        pp_cfg_pers_enabled = new QCheckBox(groupBox);
        pp_cfg_pers_enabled->setObjectName(QStringLiteral("pp_cfg_pers_enabled"));

        verticalLayout_4->addWidget(pp_cfg_pers_enabled);

        pp_cfg_pers_schema_create = new QCheckBox(groupBox);
        pp_cfg_pers_schema_create->setObjectName(QStringLiteral("pp_cfg_pers_schema_create"));

        verticalLayout_4->addWidget(pp_cfg_pers_schema_create);

        pp_cfg_drop_existing_schema = new QCheckBox(groupBox);
        pp_cfg_drop_existing_schema->setObjectName(QStringLiteral("pp_cfg_drop_existing_schema"));

        verticalLayout_4->addWidget(pp_cfg_drop_existing_schema);


        verticalLayout_9->addLayout(verticalLayout_4);


        verticalLayout->addWidget(groupBox);

        set_peer_params_button = new QPushButton(frame);
        set_peer_params_button->setObjectName(QStringLiteral("set_peer_params_button"));
        sizePolicy1.setHeightForWidth(set_peer_params_button->sizePolicy().hasHeightForWidth());
        set_peer_params_button->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(set_peer_params_button);


        horizontalLayout_5->addLayout(verticalLayout);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        groupBox_2 = new QGroupBox(frame);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        groupBox_2->setMinimumSize(QSize(230, 100));
        verticalLayout_7 = new QVBoxLayout(groupBox_2);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        peer_cfg_view_list_vlgmodel = new QListView(groupBox_2);
        peer_cfg_view_list_vlgmodel->setObjectName(QStringLiteral("peer_cfg_view_list_vlgmodel"));
        sizePolicy1.setHeightForWidth(peer_cfg_view_list_vlgmodel->sizePolicy().hasHeightForWidth());
        peer_cfg_view_list_vlgmodel->setSizePolicy(sizePolicy1);
        peer_cfg_view_list_vlgmodel->setMaximumSize(QSize(200, 500));

        verticalLayout_7->addWidget(peer_cfg_view_list_vlgmodel);


        verticalLayout_3->addWidget(groupBox_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        update_peer_model_button = new QPushButton(frame);
        update_peer_model_button->setObjectName(QStringLiteral("update_peer_model_button"));
        sizePolicy1.setHeightForWidth(update_peer_model_button->sizePolicy().hasHeightForWidth());
        update_peer_model_button->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(update_peer_model_button);


        verticalLayout_3->addLayout(horizontalLayout_3);


        horizontalLayout_5->addLayout(verticalLayout_3);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        groupBox_3 = new QGroupBox(frame);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        sizePolicy1.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy1);
        groupBox_3->setMinimumSize(QSize(230, 100));
        verticalLayout_8 = new QVBoxLayout(groupBox_3);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        peer_cfg_view_list_vlgpersdriv = new QListView(groupBox_3);
        peer_cfg_view_list_vlgpersdriv->setObjectName(QStringLiteral("peer_cfg_view_list_vlgpersdriv"));
        sizePolicy1.setHeightForWidth(peer_cfg_view_list_vlgpersdriv->sizePolicy().hasHeightForWidth());
        peer_cfg_view_list_vlgpersdriv->setSizePolicy(sizePolicy1);
        peer_cfg_view_list_vlgpersdriv->setMaximumSize(QSize(200, 500));

        verticalLayout_8->addWidget(peer_cfg_view_list_vlgpersdriv);


        verticalLayout_5->addWidget(groupBox_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        set_pers_driver_button = new QPushButton(frame);
        set_pers_driver_button->setObjectName(QStringLiteral("set_pers_driver_button"));
        sizePolicy1.setHeightForWidth(set_pers_driver_button->sizePolicy().hasHeightForWidth());
        set_pers_driver_button->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(set_pers_driver_button);


        verticalLayout_5->addLayout(horizontalLayout_4);


        horizontalLayout_5->addLayout(verticalLayout_5);


        verticalLayout_6->addWidget(frame);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer);

        peer_Tab->addTab(peer_config, icon1, QString());
        splitter->addWidget(peer_Tab);
        logr_apnd_plainTextEdit = new QPlainTextEdit(splitter);
        logr_apnd_plainTextEdit->setObjectName(QStringLiteral("logr_apnd_plainTextEdit"));
        QFont font3;
        font3.setFamily(QStringLiteral("Lucida Console"));
        font3.setPointSize(8);
        font3.setStyleStrategy(QFont::NoAntialias);
        logr_apnd_plainTextEdit->setFont(font3);
        logr_apnd_plainTextEdit->setStyleSheet(QStringLiteral(""));
        logr_apnd_plainTextEdit->setLineWidth(0);
        logr_apnd_plainTextEdit->setReadOnly(true);
        logr_apnd_plainTextEdit->setMaximumBlockCount(-1);
        logr_apnd_plainTextEdit->setCenterOnScroll(false);
        splitter->addWidget(logr_apnd_plainTextEdit);

        verticalLayout_2->addWidget(splitter);

        vlg_toolkit_MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(vlg_toolkit_MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 924, 21));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_File->setGeometry(QRect(339, 100, 143, 103));
        menuConnection = new QMenu(menuBar);
        menuConnection->setObjectName(QStringLiteral("menuConnection"));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QStringLiteral("menuAbout"));
        menuPeer = new QMenu(menuBar);
        menuPeer->setObjectName(QStringLiteral("menuPeer"));
        menuLog = new QMenu(menuBar);
        menuLog->setObjectName(QStringLiteral("menuLog"));
        vlg_toolkit_MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(vlg_toolkit_MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        vlg_toolkit_MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(vlg_toolkit_MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        vlg_toolkit_MainWindow->setStatusBar(statusBar);
#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(pp_cfg_srv_sin_addr);
        label_3->setBuddy(pp_cfg_srv_sin_port);
        label_4->setBuddy(pp_cfg_srv_exectrs);
        label->setBuddy(pp_cfg_cli_exectrs);
        label_9->setBuddy(pp_cfg_srv_sbs_exectrs);
#endif // QT_NO_SHORTCUT

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menuPeer->menuAction());
        menuBar->addAction(menuConnection->menuAction());
        menuBar->addAction(menuLog->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menu_File->addAction(action_Load_Config);
        menu_File->addSeparator();
        menu_File->addAction(action_Exit);
        menuConnection->addSeparator();
        menuConnection->addAction(actionConnect);
        menuPeer->addSeparator();
        menuPeer->addAction(action_Start_Peer);
        menuPeer->addAction(action_Stop_Peer);
        menuLog->addAction(actionLow);
        menuLog->addAction(actionTrace);
        menuLog->addAction(actionDebug);
        menuLog->addAction(actionInfo);
        menuLog->addAction(actionWarning);
        menuLog->addAction(actionError);
        menuLog->addAction(actionCritical);
        menuLog->addAction(actionFatal);
        menuLog->addSeparator();
        menuLog->addAction(actionClean_Console);
        mainToolBar->addAction(action_Load_Config);
        mainToolBar->addSeparator();
        mainToolBar->addAction(action_Start_Peer);
        mainToolBar->addAction(action_Stop_Peer);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionConnect);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionClean_Console);

        retranslateUi(vlg_toolkit_MainWindow);

        peer_Tab->setCurrentIndex(0);
        pp_cfg_peer_personality->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(vlg_toolkit_MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *vlg_toolkit_MainWindow)
    {
        vlg_toolkit_MainWindow->setWindowTitle(QApplication::translate("vlg_toolkit_MainWindow", "vlg_TOOLKIT", Q_NULLPTR));
        action_Load_Config->setText(QApplication::translate("vlg_toolkit_MainWindow", "&Load Config", Q_NULLPTR));
        action_Exit->setText(QApplication::translate("vlg_toolkit_MainWindow", "&Exit", Q_NULLPTR));
        actionConnect->setText(QApplication::translate("vlg_toolkit_MainWindow", "&Connect", Q_NULLPTR));
#ifndef QT_NO_STATUSTIP
        actionConnect->setStatusTip(QApplication::translate("vlg_toolkit_MainWindow", "Connect to peer", Q_NULLPTR));
#endif // QT_NO_STATUSTIP
        action_Start_Peer->setText(QApplication::translate("vlg_toolkit_MainWindow", "&Start Peer", Q_NULLPTR));
        action_Stop_Peer->setText(QApplication::translate("vlg_toolkit_MainWindow", "S&top Peer", Q_NULLPTR));
        actionTrace->setText(QApplication::translate("vlg_toolkit_MainWindow", "Trace", Q_NULLPTR));
        actionDebug->setText(QApplication::translate("vlg_toolkit_MainWindow", "Debug", Q_NULLPTR));
        actionInfo->setText(QApplication::translate("vlg_toolkit_MainWindow", "Info", Q_NULLPTR));
        actionWarning->setText(QApplication::translate("vlg_toolkit_MainWindow", "Warning", Q_NULLPTR));
        actionError->setText(QApplication::translate("vlg_toolkit_MainWindow", "Error", Q_NULLPTR));
        actionCritical->setText(QApplication::translate("vlg_toolkit_MainWindow", "Critical", Q_NULLPTR));
        actionFatal->setText(QApplication::translate("vlg_toolkit_MainWindow", "Fatal", Q_NULLPTR));
        actionClean_Console->setText(QApplication::translate("vlg_toolkit_MainWindow", "Clean Console", Q_NULLPTR));
        actionLow->setText(QApplication::translate("vlg_toolkit_MainWindow", "Low", Q_NULLPTR));
        peer_status_label_display->setText(QString());
        main_info_label->setText(QString());
        groupBox_4->setTitle(QApplication::translate("vlg_toolkit_MainWindow", "settings", Q_NULLPTR));
        set_peer_cfg_all_button->setText(QApplication::translate("vlg_toolkit_MainWindow", "SET ALL", Q_NULLPTR));
        peer_status_label_4->setText(QApplication::translate("vlg_toolkit_MainWindow", "Peer Config", Q_NULLPTR));
        peer_cfg_status_label->setText(QApplication::translate("vlg_toolkit_MainWindow", "unset", Q_NULLPTR));
        peer_status_label_3->setText(QApplication::translate("vlg_toolkit_MainWindow", "Peer Model", Q_NULLPTR));
        peer_model_label_status->setText(QApplication::translate("vlg_toolkit_MainWindow", "unset", Q_NULLPTR));
        peer_status_label_2->setText(QApplication::translate("vlg_toolkit_MainWindow", "Peer Pers Divers", Q_NULLPTR));
        peer_pers_driv_status_label->setText(QApplication::translate("vlg_toolkit_MainWindow", "unset", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("vlg_toolkit_MainWindow", "Peer Params", Q_NULLPTR));
        label_5->setText(QApplication::translate("vlg_toolkit_MainWindow", "peer_personality:", Q_NULLPTR));
        pp_cfg_peer_personality->clear();
        pp_cfg_peer_personality->insertItems(0, QStringList()
         << QApplication::translate("vlg_toolkit_MainWindow", "pure client", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_MainWindow", "pure server", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_MainWindow", "both", Q_NULLPTR)
        );
#ifndef QT_NO_WHATSTHIS
        pp_cfg_peer_personality->setWhatsThis(QApplication::translate("vlg_toolkit_MainWindow", "<html><head/><body><p><br/></p></body></html>", Q_NULLPTR));
#endif // QT_NO_WHATSTHIS
        pp_cfg_peer_personality->setCurrentText(QApplication::translate("vlg_toolkit_MainWindow", "both", Q_NULLPTR));
        label_2->setText(QApplication::translate("vlg_toolkit_MainWindow", "srv_sin_addr:", Q_NULLPTR));
        label_3->setText(QApplication::translate("vlg_toolkit_MainWindow", "srv_sin_port:", Q_NULLPTR));
        label_4->setText(QApplication::translate("vlg_toolkit_MainWindow", "srv_exectrs:", Q_NULLPTR));
        label->setText(QApplication::translate("vlg_toolkit_MainWindow", "cli_exectrs:", Q_NULLPTR));
        label_9->setText(QApplication::translate("vlg_toolkit_MainWindow", "srv_sbs_exectrs", Q_NULLPTR));
        pp_cfg_pers_enabled->setText(QApplication::translate("vlg_toolkit_MainWindow", "pers_enabled", Q_NULLPTR));
        pp_cfg_pers_schema_create->setText(QApplication::translate("vlg_toolkit_MainWindow", "pers_schema_create", Q_NULLPTR));
        pp_cfg_drop_existing_schema->setText(QApplication::translate("vlg_toolkit_MainWindow", "drop_existing_schema", Q_NULLPTR));
        set_peer_params_button->setText(QApplication::translate("vlg_toolkit_MainWindow", "Set Peer Params", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("vlg_toolkit_MainWindow", "Peer Models", Q_NULLPTR));
        update_peer_model_button->setText(QApplication::translate("vlg_toolkit_MainWindow", "Set Peer Model", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("vlg_toolkit_MainWindow", "Peer Pers Drivers", Q_NULLPTR));
        set_pers_driver_button->setText(QApplication::translate("vlg_toolkit_MainWindow", "Set Pers Driver", Q_NULLPTR));
        peer_Tab->setTabText(peer_Tab->indexOf(peer_config), QApplication::translate("vlg_toolkit_MainWindow", "Peer Config", Q_NULLPTR));
        menu_File->setTitle(QApplication::translate("vlg_toolkit_MainWindow", "&File", Q_NULLPTR));
        menuConnection->setTitle(QApplication::translate("vlg_toolkit_MainWindow", "&Connection", Q_NULLPTR));
        menuAbout->setTitle(QApplication::translate("vlg_toolkit_MainWindow", "&About", Q_NULLPTR));
        menuPeer->setTitle(QApplication::translate("vlg_toolkit_MainWindow", "&Peer", Q_NULLPTR));
        menuLog->setTitle(QApplication::translate("vlg_toolkit_MainWindow", "Logging", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class vlg_toolkit_MainWindow: public Ui_vlg_toolkit_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VLG_TOOLKIT_MAINWINDOW_H
