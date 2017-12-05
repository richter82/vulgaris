/********************************************************************************
** Form generated from reading UI file 'vlg_toolkit_sbs_window.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VLG_TOOLKIT_SBS_WINDOW_H
#define UI_VLG_TOOLKIT_SBS_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_vlg_toolkit_sbs_window
{
public:
    QAction *actionStart_SBS;
    QAction *actionStop_SBS;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_7;
    QFrame *conn_status_frame;
    QHBoxLayout *horizontalLayout_8;
    QHBoxLayout *horizontalLayout_9;
    QLabel *sbs_status_label;
    QLabel *sbs_status_label_disp;
    QFrame *conn_status_frame_7;
    QHBoxLayout *horizontalLayout_20;
    QHBoxLayout *horizontalLayout_21;
    QLabel *port_label_3;
    QLabel *sbsid_label_disp;
    QFrame *conn_status_frame_6;
    QHBoxLayout *horizontalLayout_18;
    QHBoxLayout *horizontalLayout_19;
    QLabel *port_label_2;
    QLabel *connid_label_disp;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *sbs_params_gbox;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *conn_status_groupBox_10;
    QHBoxLayout *horizontalLayout;
    QComboBox *cfg_sbs_type_cb;
    QGroupBox *conn_status_groupBox_11;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *cfg_sbs_mode_cb;
    QGroupBox *conn_status_groupBox_12;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *cfg_sbs_flow_type_cb;
    QGroupBox *conn_status_groupBox_13;
    QHBoxLayout *horizontalLayout_4;
    QComboBox *cfg_sbs_dwnld_type_cb;
    QGroupBox *conn_status_groupBox_14;
    QHBoxLayout *horizontalLayout_5;
    QComboBox *cfg_sbs_class_encode_cb;
    QFrame *conn_status_frame_15;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label;
    QLineEdit *cfg_sbs_ts0_le;
    QFrame *conn_status_frame_16;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_2;
    QLineEdit *cfg_sbs_ts1_le;
    QSpacerItem *horizontalSpacer;
    QTableView *vlg_class_sbs_table_view;
    QMenuBar *menubar;
    QMenu *menuSubscription;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *vlg_toolkit_sbs_window)
    {
        if (vlg_toolkit_sbs_window->objectName().isEmpty())
            vlg_toolkit_sbs_window->setObjectName(QStringLiteral("vlg_toolkit_sbs_window"));
        vlg_toolkit_sbs_window->setWindowModality(Qt::NonModal);
        vlg_toolkit_sbs_window->resize(1116, 555);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(vlg_toolkit_sbs_window->sizePolicy().hasHeightForWidth());
        vlg_toolkit_sbs_window->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icon/icons/social-rss.png"), QSize(), QIcon::Normal, QIcon::Off);
        vlg_toolkit_sbs_window->setWindowIcon(icon);
        actionStart_SBS = new QAction(vlg_toolkit_sbs_window);
        actionStart_SBS->setObjectName(QStringLiteral("actionStart_SBS"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icon/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStart_SBS->setIcon(icon1);
        actionStop_SBS = new QAction(vlg_toolkit_sbs_window);
        actionStop_SBS->setObjectName(QStringLiteral("actionStop_SBS"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icon/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStop_SBS->setIcon(icon2);
        centralwidget = new QWidget(vlg_toolkit_sbs_window);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_7 = new QHBoxLayout(frame);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        conn_status_frame = new QFrame(frame);
        conn_status_frame->setObjectName(QStringLiteral("conn_status_frame"));
        conn_status_frame->setFrameShape(QFrame::Box);
        conn_status_frame->setFrameShadow(QFrame::Sunken);
        horizontalLayout_8 = new QHBoxLayout(conn_status_frame);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        sbs_status_label = new QLabel(conn_status_frame);
        sbs_status_label->setObjectName(QStringLiteral("sbs_status_label"));

        horizontalLayout_9->addWidget(sbs_status_label);

        sbs_status_label_disp = new QLabel(conn_status_frame);
        sbs_status_label_disp->setObjectName(QStringLiteral("sbs_status_label_disp"));
        sbs_status_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        sbs_status_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_9->addWidget(sbs_status_label_disp);


        horizontalLayout_8->addLayout(horizontalLayout_9);


        horizontalLayout_7->addWidget(conn_status_frame);

        conn_status_frame_7 = new QFrame(frame);
        conn_status_frame_7->setObjectName(QStringLiteral("conn_status_frame_7"));
        conn_status_frame_7->setFrameShape(QFrame::Box);
        conn_status_frame_7->setFrameShadow(QFrame::Sunken);
        horizontalLayout_20 = new QHBoxLayout(conn_status_frame_7);
        horizontalLayout_20->setObjectName(QStringLiteral("horizontalLayout_20"));
        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName(QStringLiteral("horizontalLayout_21"));
        port_label_3 = new QLabel(conn_status_frame_7);
        port_label_3->setObjectName(QStringLiteral("port_label_3"));

        horizontalLayout_21->addWidget(port_label_3);

        sbsid_label_disp = new QLabel(conn_status_frame_7);
        sbsid_label_disp->setObjectName(QStringLiteral("sbsid_label_disp"));
        sbsid_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        sbsid_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_21->addWidget(sbsid_label_disp);


        horizontalLayout_20->addLayout(horizontalLayout_21);


        horizontalLayout_7->addWidget(conn_status_frame_7);

        conn_status_frame_6 = new QFrame(frame);
        conn_status_frame_6->setObjectName(QStringLiteral("conn_status_frame_6"));
        conn_status_frame_6->setFrameShape(QFrame::Box);
        conn_status_frame_6->setFrameShadow(QFrame::Sunken);
        horizontalLayout_18 = new QHBoxLayout(conn_status_frame_6);
        horizontalLayout_18->setObjectName(QStringLiteral("horizontalLayout_18"));
        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName(QStringLiteral("horizontalLayout_19"));
        port_label_2 = new QLabel(conn_status_frame_6);
        port_label_2->setObjectName(QStringLiteral("port_label_2"));

        horizontalLayout_19->addWidget(port_label_2);

        connid_label_disp = new QLabel(conn_status_frame_6);
        connid_label_disp->setObjectName(QStringLiteral("connid_label_disp"));
        connid_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        connid_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_19->addWidget(connid_label_disp);


        horizontalLayout_18->addLayout(horizontalLayout_19);


        horizontalLayout_7->addWidget(conn_status_frame_6);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(frame);

        sbs_params_gbox = new QGroupBox(centralwidget);
        sbs_params_gbox->setObjectName(QStringLiteral("sbs_params_gbox"));
        QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(sbs_params_gbox->sizePolicy().hasHeightForWidth());
        sbs_params_gbox->setSizePolicy(sizePolicy2);
        horizontalLayout_6 = new QHBoxLayout(sbs_params_gbox);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        conn_status_groupBox_10 = new QGroupBox(sbs_params_gbox);
        conn_status_groupBox_10->setObjectName(QStringLiteral("conn_status_groupBox_10"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(conn_status_groupBox_10->sizePolicy().hasHeightForWidth());
        conn_status_groupBox_10->setSizePolicy(sizePolicy3);
        horizontalLayout = new QHBoxLayout(conn_status_groupBox_10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        cfg_sbs_type_cb = new QComboBox(conn_status_groupBox_10);
        cfg_sbs_type_cb->setObjectName(QStringLiteral("cfg_sbs_type_cb"));

        horizontalLayout->addWidget(cfg_sbs_type_cb);


        horizontalLayout_6->addWidget(conn_status_groupBox_10);

        conn_status_groupBox_11 = new QGroupBox(sbs_params_gbox);
        conn_status_groupBox_11->setObjectName(QStringLiteral("conn_status_groupBox_11"));
        sizePolicy3.setHeightForWidth(conn_status_groupBox_11->sizePolicy().hasHeightForWidth());
        conn_status_groupBox_11->setSizePolicy(sizePolicy3);
        horizontalLayout_2 = new QHBoxLayout(conn_status_groupBox_11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        cfg_sbs_mode_cb = new QComboBox(conn_status_groupBox_11);
        cfg_sbs_mode_cb->setObjectName(QStringLiteral("cfg_sbs_mode_cb"));

        horizontalLayout_2->addWidget(cfg_sbs_mode_cb);


        horizontalLayout_6->addWidget(conn_status_groupBox_11);

        conn_status_groupBox_12 = new QGroupBox(sbs_params_gbox);
        conn_status_groupBox_12->setObjectName(QStringLiteral("conn_status_groupBox_12"));
        sizePolicy3.setHeightForWidth(conn_status_groupBox_12->sizePolicy().hasHeightForWidth());
        conn_status_groupBox_12->setSizePolicy(sizePolicy3);
        horizontalLayout_3 = new QHBoxLayout(conn_status_groupBox_12);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        cfg_sbs_flow_type_cb = new QComboBox(conn_status_groupBox_12);
        cfg_sbs_flow_type_cb->setObjectName(QStringLiteral("cfg_sbs_flow_type_cb"));

        horizontalLayout_3->addWidget(cfg_sbs_flow_type_cb);


        horizontalLayout_6->addWidget(conn_status_groupBox_12);

        conn_status_groupBox_13 = new QGroupBox(sbs_params_gbox);
        conn_status_groupBox_13->setObjectName(QStringLiteral("conn_status_groupBox_13"));
        sizePolicy3.setHeightForWidth(conn_status_groupBox_13->sizePolicy().hasHeightForWidth());
        conn_status_groupBox_13->setSizePolicy(sizePolicy3);
        horizontalLayout_4 = new QHBoxLayout(conn_status_groupBox_13);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        cfg_sbs_dwnld_type_cb = new QComboBox(conn_status_groupBox_13);
        cfg_sbs_dwnld_type_cb->setObjectName(QStringLiteral("cfg_sbs_dwnld_type_cb"));

        horizontalLayout_4->addWidget(cfg_sbs_dwnld_type_cb);


        horizontalLayout_6->addWidget(conn_status_groupBox_13);

        conn_status_groupBox_14 = new QGroupBox(sbs_params_gbox);
        conn_status_groupBox_14->setObjectName(QStringLiteral("conn_status_groupBox_14"));
        sizePolicy3.setHeightForWidth(conn_status_groupBox_14->sizePolicy().hasHeightForWidth());
        conn_status_groupBox_14->setSizePolicy(sizePolicy3);
        horizontalLayout_5 = new QHBoxLayout(conn_status_groupBox_14);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        cfg_sbs_class_encode_cb = new QComboBox(conn_status_groupBox_14);
        cfg_sbs_class_encode_cb->setObjectName(QStringLiteral("cfg_sbs_class_encode_cb"));

        horizontalLayout_5->addWidget(cfg_sbs_class_encode_cb);


        horizontalLayout_6->addWidget(conn_status_groupBox_14);

        conn_status_frame_15 = new QFrame(sbs_params_gbox);
        conn_status_frame_15->setObjectName(QStringLiteral("conn_status_frame_15"));
        sizePolicy3.setHeightForWidth(conn_status_frame_15->sizePolicy().hasHeightForWidth());
        conn_status_frame_15->setSizePolicy(sizePolicy3);
        conn_status_frame_15->setFrameShape(QFrame::Box);
        conn_status_frame_15->setFrameShadow(QFrame::Sunken);
        horizontalLayout_11 = new QHBoxLayout(conn_status_frame_15);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        label = new QLabel(conn_status_frame_15);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_11->addWidget(label);

        cfg_sbs_ts0_le = new QLineEdit(conn_status_frame_15);
        cfg_sbs_ts0_le->setObjectName(QStringLiteral("cfg_sbs_ts0_le"));

        horizontalLayout_11->addWidget(cfg_sbs_ts0_le);


        horizontalLayout_6->addWidget(conn_status_frame_15);

        conn_status_frame_16 = new QFrame(sbs_params_gbox);
        conn_status_frame_16->setObjectName(QStringLiteral("conn_status_frame_16"));
        sizePolicy3.setHeightForWidth(conn_status_frame_16->sizePolicy().hasHeightForWidth());
        conn_status_frame_16->setSizePolicy(sizePolicy3);
        conn_status_frame_16->setFrameShape(QFrame::Box);
        conn_status_frame_16->setFrameShadow(QFrame::Sunken);
        horizontalLayout_12 = new QHBoxLayout(conn_status_frame_16);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        label_2 = new QLabel(conn_status_frame_16);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_12->addWidget(label_2);

        cfg_sbs_ts1_le = new QLineEdit(conn_status_frame_16);
        cfg_sbs_ts1_le->setObjectName(QStringLiteral("cfg_sbs_ts1_le"));

        horizontalLayout_12->addWidget(cfg_sbs_ts1_le);


        horizontalLayout_6->addWidget(conn_status_frame_16);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);


        verticalLayout->addWidget(sbs_params_gbox);

        vlg_class_sbs_table_view = new QTableView(centralwidget);
        vlg_class_sbs_table_view->setObjectName(QStringLiteral("vlg_class_sbs_table_view"));
        vlg_class_sbs_table_view->setContextMenuPolicy(Qt::CustomContextMenu);
        vlg_class_sbs_table_view->setSortingEnabled(true);

        verticalLayout->addWidget(vlg_class_sbs_table_view);

        vlg_toolkit_sbs_window->setCentralWidget(centralwidget);
        menubar = new QMenuBar(vlg_toolkit_sbs_window);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1116, 21));
        menuSubscription = new QMenu(menubar);
        menuSubscription->setObjectName(QStringLiteral("menuSubscription"));
        vlg_toolkit_sbs_window->setMenuBar(menubar);
        statusbar = new QStatusBar(vlg_toolkit_sbs_window);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        vlg_toolkit_sbs_window->setStatusBar(statusbar);
        toolBar = new QToolBar(vlg_toolkit_sbs_window);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        vlg_toolkit_sbs_window->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menuSubscription->menuAction());
        menuSubscription->addAction(actionStart_SBS);
        menuSubscription->addAction(actionStop_SBS);
        toolBar->addAction(actionStart_SBS);
        toolBar->addAction(actionStop_SBS);

        retranslateUi(vlg_toolkit_sbs_window);

        cfg_sbs_class_encode_cb->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(vlg_toolkit_sbs_window);
    } // setupUi

    void retranslateUi(QMainWindow *vlg_toolkit_sbs_window)
    {
        vlg_toolkit_sbs_window->setWindowTitle(QApplication::translate("vlg_toolkit_sbs_window", "MainWindow", Q_NULLPTR));
        actionStart_SBS->setText(QApplication::translate("vlg_toolkit_sbs_window", "Start", Q_NULLPTR));
        actionStop_SBS->setText(QApplication::translate("vlg_toolkit_sbs_window", "Stop", Q_NULLPTR));
        sbs_status_label->setText(QApplication::translate("vlg_toolkit_sbs_window", "Status", Q_NULLPTR));
        sbs_status_label_disp->setText(QApplication::translate("vlg_toolkit_sbs_window", "-", Q_NULLPTR));
        port_label_3->setText(QApplication::translate("vlg_toolkit_sbs_window", "SBSID", Q_NULLPTR));
        sbsid_label_disp->setText(QApplication::translate("vlg_toolkit_sbs_window", "-", Q_NULLPTR));
        port_label_2->setText(QApplication::translate("vlg_toolkit_sbs_window", "CONNID", Q_NULLPTR));
        connid_label_disp->setText(QApplication::translate("vlg_toolkit_sbs_window", "-", Q_NULLPTR));
        sbs_params_gbox->setTitle(QApplication::translate("vlg_toolkit_sbs_window", "Subscription Params", Q_NULLPTR));
        conn_status_groupBox_10->setTitle(QApplication::translate("vlg_toolkit_sbs_window", "Subscr. Type", Q_NULLPTR));
        cfg_sbs_type_cb->clear();
        cfg_sbs_type_cb->insertItems(0, QStringList()
         << QApplication::translate("vlg_toolkit_sbs_window", "SNAPSHOT", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_sbs_window", "INCREMENTAL", Q_NULLPTR)
        );
        conn_status_groupBox_11->setTitle(QApplication::translate("vlg_toolkit_sbs_window", "Subscr. Mode", Q_NULLPTR));
        cfg_sbs_mode_cb->clear();
        cfg_sbs_mode_cb->insertItems(0, QStringList()
         << QApplication::translate("vlg_toolkit_sbs_window", "ALL", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_sbs_window", "DOWNLOAD", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_sbs_window", "LIVE", Q_NULLPTR)
        );
        conn_status_groupBox_12->setTitle(QApplication::translate("vlg_toolkit_sbs_window", "Subscr. Flow Type", Q_NULLPTR));
        cfg_sbs_flow_type_cb->clear();
        cfg_sbs_flow_type_cb->insertItems(0, QStringList()
         << QApplication::translate("vlg_toolkit_sbs_window", "ALL", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_sbs_window", "LAST", Q_NULLPTR)
        );
        conn_status_groupBox_13->setTitle(QApplication::translate("vlg_toolkit_sbs_window", "Subscr. Downl. Type", Q_NULLPTR));
        cfg_sbs_dwnld_type_cb->clear();
        cfg_sbs_dwnld_type_cb->insertItems(0, QStringList()
         << QApplication::translate("vlg_toolkit_sbs_window", "ALL", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_sbs_window", "PARTIAL", Q_NULLPTR)
        );
        conn_status_groupBox_14->setTitle(QApplication::translate("vlg_toolkit_sbs_window", "Encoding", Q_NULLPTR));
        cfg_sbs_class_encode_cb->clear();
        cfg_sbs_class_encode_cb->insertItems(0, QStringList()
         << QApplication::translate("vlg_toolkit_sbs_window", "INDEXED_NOT_ZERO", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_sbs_window", "INDEXED_DELTA", Q_NULLPTR)
        );
        label->setText(QApplication::translate("vlg_toolkit_sbs_window", "TS0", Q_NULLPTR));
        cfg_sbs_ts0_le->setText(QApplication::translate("vlg_toolkit_sbs_window", "0", Q_NULLPTR));
        label_2->setText(QApplication::translate("vlg_toolkit_sbs_window", "TS1", Q_NULLPTR));
        cfg_sbs_ts1_le->setText(QApplication::translate("vlg_toolkit_sbs_window", "0", Q_NULLPTR));
        menuSubscription->setTitle(QApplication::translate("vlg_toolkit_sbs_window", "Subscription", Q_NULLPTR));
        toolBar->setWindowTitle(QApplication::translate("vlg_toolkit_sbs_window", "toolBar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class vlg_toolkit_sbs_window: public Ui_vlg_toolkit_sbs_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VLG_TOOLKIT_SBS_WINDOW_H
