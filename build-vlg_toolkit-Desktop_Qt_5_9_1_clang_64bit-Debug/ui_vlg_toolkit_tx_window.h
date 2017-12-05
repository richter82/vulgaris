/********************************************************************************
** Form generated from reading UI file 'vlg_toolkit_tx_window.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VLG_TOOLKIT_TX_WINDOW_H
#define UI_VLG_TOOLKIT_TX_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
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

class Ui_vlg_toolkit_tx_window
{
public:
    QAction *actionSend_TX;
    QAction *actionReNew_TX;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_7;
    QFrame *conn_status_frame;
    QHBoxLayout *horizontalLayout_8;
    QHBoxLayout *horizontalLayout_9;
    QLabel *sbs_status_label;
    QLabel *tx_status_label_disp;
    QFrame *conn_status_frame_8;
    QHBoxLayout *horizontalLayout_24;
    QHBoxLayout *horizontalLayout_25;
    QLabel *port_label_5;
    QLabel *connid_tx_res_disp;
    QFrame *conn_status_frame_7;
    QHBoxLayout *horizontalLayout_20;
    QHBoxLayout *horizontalLayout_21;
    QLabel *port_label_3;
    QLabel *txid_label_disp;
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
    QComboBox *cfg_tx_reqtype_cb;
    QGroupBox *conn_status_groupBox_11;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *cfg_act_cb;
    QGroupBox *conn_status_groupBox_12;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *cfg_class_encode_cb;
    QFrame *conn_status_frame_13;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QCheckBox *cfg_res_class_req_cb;
    QSpacerItem *horizontalSpacer;
    QTableView *vlg_class_tx_table_view;
    QMenuBar *menubar;
    QMenu *menuTransaction;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *vlg_toolkit_tx_window)
    {
        if (vlg_toolkit_tx_window->objectName().isEmpty())
            vlg_toolkit_tx_window->setObjectName(QStringLiteral("vlg_toolkit_tx_window"));
        vlg_toolkit_tx_window->resize(800, 678);
        vlg_toolkit_tx_window->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icon/icons/forward.png"), QSize(), QIcon::Normal, QIcon::Off);
        vlg_toolkit_tx_window->setWindowIcon(icon);
        actionSend_TX = new QAction(vlg_toolkit_tx_window);
        actionSend_TX->setObjectName(QStringLiteral("actionSend_TX"));
        actionSend_TX->setIcon(icon);
        actionReNew_TX = new QAction(vlg_toolkit_tx_window);
        actionReNew_TX->setObjectName(QStringLiteral("actionReNew_TX"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icon/icons/loop.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReNew_TX->setIcon(icon1);
        centralwidget = new QWidget(vlg_toolkit_tx_window);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
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

        tx_status_label_disp = new QLabel(conn_status_frame);
        tx_status_label_disp->setObjectName(QStringLiteral("tx_status_label_disp"));
        tx_status_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        tx_status_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_9->addWidget(tx_status_label_disp);


        horizontalLayout_8->addLayout(horizontalLayout_9);


        horizontalLayout_7->addWidget(conn_status_frame);

        conn_status_frame_8 = new QFrame(frame);
        conn_status_frame_8->setObjectName(QStringLiteral("conn_status_frame_8"));
        conn_status_frame_8->setFrameShape(QFrame::Box);
        conn_status_frame_8->setFrameShadow(QFrame::Sunken);
        horizontalLayout_24 = new QHBoxLayout(conn_status_frame_8);
        horizontalLayout_24->setObjectName(QStringLiteral("horizontalLayout_24"));
        horizontalLayout_25 = new QHBoxLayout();
        horizontalLayout_25->setObjectName(QStringLiteral("horizontalLayout_25"));
        port_label_5 = new QLabel(conn_status_frame_8);
        port_label_5->setObjectName(QStringLiteral("port_label_5"));

        horizontalLayout_25->addWidget(port_label_5);

        connid_tx_res_disp = new QLabel(conn_status_frame_8);
        connid_tx_res_disp->setObjectName(QStringLiteral("connid_tx_res_disp"));
        connid_tx_res_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        connid_tx_res_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_25->addWidget(connid_tx_res_disp);


        horizontalLayout_24->addLayout(horizontalLayout_25);


        horizontalLayout_7->addWidget(conn_status_frame_8);

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

        txid_label_disp = new QLabel(conn_status_frame_7);
        txid_label_disp->setObjectName(QStringLiteral("txid_label_disp"));
        txid_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        txid_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_21->addWidget(txid_label_disp);


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
        QSizePolicy sizePolicy1(QSizePolicy::Ignored, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(sbs_params_gbox->sizePolicy().hasHeightForWidth());
        sbs_params_gbox->setSizePolicy(sizePolicy1);
        horizontalLayout_6 = new QHBoxLayout(sbs_params_gbox);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        conn_status_groupBox_10 = new QGroupBox(sbs_params_gbox);
        conn_status_groupBox_10->setObjectName(QStringLiteral("conn_status_groupBox_10"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(conn_status_groupBox_10->sizePolicy().hasHeightForWidth());
        conn_status_groupBox_10->setSizePolicy(sizePolicy2);
        horizontalLayout = new QHBoxLayout(conn_status_groupBox_10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        cfg_tx_reqtype_cb = new QComboBox(conn_status_groupBox_10);
        cfg_tx_reqtype_cb->setObjectName(QStringLiteral("cfg_tx_reqtype_cb"));

        horizontalLayout->addWidget(cfg_tx_reqtype_cb);


        horizontalLayout_6->addWidget(conn_status_groupBox_10);

        conn_status_groupBox_11 = new QGroupBox(sbs_params_gbox);
        conn_status_groupBox_11->setObjectName(QStringLiteral("conn_status_groupBox_11"));
        sizePolicy2.setHeightForWidth(conn_status_groupBox_11->sizePolicy().hasHeightForWidth());
        conn_status_groupBox_11->setSizePolicy(sizePolicy2);
        horizontalLayout_2 = new QHBoxLayout(conn_status_groupBox_11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        cfg_act_cb = new QComboBox(conn_status_groupBox_11);
        cfg_act_cb->setObjectName(QStringLiteral("cfg_act_cb"));

        horizontalLayout_2->addWidget(cfg_act_cb);


        horizontalLayout_6->addWidget(conn_status_groupBox_11);

        conn_status_groupBox_12 = new QGroupBox(sbs_params_gbox);
        conn_status_groupBox_12->setObjectName(QStringLiteral("conn_status_groupBox_12"));
        sizePolicy2.setHeightForWidth(conn_status_groupBox_12->sizePolicy().hasHeightForWidth());
        conn_status_groupBox_12->setSizePolicy(sizePolicy2);
        horizontalLayout_3 = new QHBoxLayout(conn_status_groupBox_12);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        cfg_class_encode_cb = new QComboBox(conn_status_groupBox_12);
        cfg_class_encode_cb->setObjectName(QStringLiteral("cfg_class_encode_cb"));

        horizontalLayout_3->addWidget(cfg_class_encode_cb);


        horizontalLayout_6->addWidget(conn_status_groupBox_12);

        conn_status_frame_13 = new QFrame(sbs_params_gbox);
        conn_status_frame_13->setObjectName(QStringLiteral("conn_status_frame_13"));
        sizePolicy2.setHeightForWidth(conn_status_frame_13->sizePolicy().hasHeightForWidth());
        conn_status_frame_13->setSizePolicy(sizePolicy2);
        conn_status_frame_13->setFrameShape(QFrame::Box);
        conn_status_frame_13->setFrameShadow(QFrame::Sunken);
        horizontalLayout_4 = new QHBoxLayout(conn_status_frame_13);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label = new QLabel(conn_status_frame_13);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_4->addWidget(label);

        cfg_res_class_req_cb = new QCheckBox(conn_status_frame_13);
        cfg_res_class_req_cb->setObjectName(QStringLiteral("cfg_res_class_req_cb"));
        cfg_res_class_req_cb->setChecked(true);

        horizontalLayout_4->addWidget(cfg_res_class_req_cb);


        horizontalLayout_6->addWidget(conn_status_frame_13);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);


        verticalLayout->addWidget(sbs_params_gbox);

        vlg_class_tx_table_view = new QTableView(centralwidget);
        vlg_class_tx_table_view->setObjectName(QStringLiteral("vlg_class_tx_table_view"));
        vlg_class_tx_table_view->setAlternatingRowColors(true);
        vlg_class_tx_table_view->setSortingEnabled(true);

        verticalLayout->addWidget(vlg_class_tx_table_view);

        vlg_toolkit_tx_window->setCentralWidget(centralwidget);
        menubar = new QMenuBar(vlg_toolkit_tx_window);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menuTransaction = new QMenu(menubar);
        menuTransaction->setObjectName(QStringLiteral("menuTransaction"));
        vlg_toolkit_tx_window->setMenuBar(menubar);
        statusbar = new QStatusBar(vlg_toolkit_tx_window);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        vlg_toolkit_tx_window->setStatusBar(statusbar);
        toolBar = new QToolBar(vlg_toolkit_tx_window);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        vlg_toolkit_tx_window->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menuTransaction->menuAction());
        menuTransaction->addAction(actionSend_TX);
        menuTransaction->addAction(actionReNew_TX);
        toolBar->addAction(actionSend_TX);
        toolBar->addAction(actionReNew_TX);

        retranslateUi(vlg_toolkit_tx_window);

        cfg_tx_reqtype_cb->setCurrentIndex(3);
        cfg_class_encode_cb->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(vlg_toolkit_tx_window);
    } // setupUi

    void retranslateUi(QMainWindow *vlg_toolkit_tx_window)
    {
        vlg_toolkit_tx_window->setWindowTitle(QApplication::translate("vlg_toolkit_tx_window", "MainWindow", Q_NULLPTR));
        actionSend_TX->setText(QApplication::translate("vlg_toolkit_tx_window", "Send", Q_NULLPTR));
        actionReNew_TX->setText(QApplication::translate("vlg_toolkit_tx_window", "ReNew", Q_NULLPTR));
        sbs_status_label->setText(QApplication::translate("vlg_toolkit_tx_window", "Status", Q_NULLPTR));
        tx_status_label_disp->setText(QApplication::translate("vlg_toolkit_tx_window", "-", Q_NULLPTR));
        port_label_5->setText(QApplication::translate("vlg_toolkit_tx_window", "TXRES", Q_NULLPTR));
        connid_tx_res_disp->setText(QApplication::translate("vlg_toolkit_tx_window", "-", Q_NULLPTR));
        port_label_3->setText(QApplication::translate("vlg_toolkit_tx_window", "TXID", Q_NULLPTR));
        txid_label_disp->setText(QApplication::translate("vlg_toolkit_tx_window", "-", Q_NULLPTR));
        port_label_2->setText(QApplication::translate("vlg_toolkit_tx_window", "CONNID", Q_NULLPTR));
        connid_label_disp->setText(QApplication::translate("vlg_toolkit_tx_window", "-", Q_NULLPTR));
        sbs_params_gbox->setTitle(QApplication::translate("vlg_toolkit_tx_window", "Transaction Params", Q_NULLPTR));
        conn_status_groupBox_10->setTitle(QApplication::translate("vlg_toolkit_tx_window", "Trans. Req. Type", Q_NULLPTR));
        cfg_tx_reqtype_cb->clear();
        cfg_tx_reqtype_cb->insertItems(0, QStringList()
         << QApplication::translate("vlg_toolkit_tx_window", "RESERVED", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_tx_window", "SYSTEM", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_tx_window", "SPECIAL", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_tx_window", "OBJECT", Q_NULLPTR)
        );
        conn_status_groupBox_11->setTitle(QApplication::translate("vlg_toolkit_tx_window", "Action", Q_NULLPTR));
        cfg_act_cb->clear();
        cfg_act_cb->insertItems(0, QStringList()
         << QApplication::translate("vlg_toolkit_tx_window", "INSERT", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_tx_window", "UPDATE", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_tx_window", "DELTA", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_tx_window", "DELETE", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_tx_window", "REMOVE", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_tx_window", "RESET", Q_NULLPTR)
        );
        conn_status_groupBox_12->setTitle(QApplication::translate("vlg_toolkit_tx_window", "Encoding", Q_NULLPTR));
        cfg_class_encode_cb->clear();
        cfg_class_encode_cb->insertItems(0, QStringList()
         << QApplication::translate("vlg_toolkit_tx_window", "INDEXED_NOT_ZERO", Q_NULLPTR)
         << QApplication::translate("vlg_toolkit_tx_window", "INDEXED_DELTA", Q_NULLPTR)
        );
        label->setText(QString());
        cfg_res_class_req_cb->setText(QApplication::translate("vlg_toolkit_tx_window", "ResClassRequred", Q_NULLPTR));
        menuTransaction->setTitle(QApplication::translate("vlg_toolkit_tx_window", "Transaction", Q_NULLPTR));
        toolBar->setWindowTitle(QApplication::translate("vlg_toolkit_tx_window", "toolBar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class vlg_toolkit_tx_window: public Ui_vlg_toolkit_tx_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VLG_TOOLKIT_TX_WINDOW_H
