/********************************************************************************
** Form generated from reading UI file 'vlg_toolkit_connection.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VLG_TOOLKIT_CONNECTION_H
#define UI_VLG_TOOLKIT_CONNECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_vlg_toolkit_Connection
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QFrame *conn_status_frame;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *conn_status_label;
    QLabel *conn_status_label_disp;
    QFrame *conn_status_frame_2;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout_7;
    QLabel *sock_label;
    QLabel *sock_label_disp;
    QFrame *conn_status_frame_3;
    QHBoxLayout *horizontalLayout_8;
    QHBoxLayout *horizontalLayout_9;
    QLabel *host_label;
    QLabel *host_label_disp;
    QFrame *conn_status_frame_4;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_11;
    QLabel *port_label;
    QLabel *port_label_disp;
    QFrame *conn_status_frame_6;
    QHBoxLayout *horizontalLayout_16;
    QHBoxLayout *horizontalLayout_17;
    QLabel *port_label_2;
    QLabel *connid_label_disp;
    QSpacerItem *horizontalSpacer;
    QGroupBox *cp_group_box;
    QHBoxLayout *horizontalLayout_23;
    QFrame *conn_status_frame_10;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_24;
    QLabel *conn_status_label_5;
    QLineEdit *cp_host_addr;
    QFrame *conn_status_frame_9;
    QHBoxLayout *horizontalLayout_18;
    QHBoxLayout *horizontalLayout_22;
    QLabel *conn_status_label_4;
    QLineEdit *cp_host_port;
    QFrame *conn_status_frame_8;
    QHBoxLayout *horizontalLayout_19;
    QHBoxLayout *horizontalLayout_20;
    QLabel *conn_status_label_3;
    QLineEdit *cp_user;
    QFrame *conn_status_frame_7;
    QHBoxLayout *horizontalLayout_21;
    QHBoxLayout *horizontalLayout_5;
    QLabel *conn_status_label_2;
    QLineEdit *cp_psswd;
    QSpacerItem *horizontalSpacer_4;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_14;
    QPushButton *connect_button;
    QPushButton *disconnect_button;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_15;
    QTreeView *peer_model_tree_view;
    QGroupBox *model_operations_box;
    QVBoxLayout *verticalLayout_2;
    QPushButton *extend_model_button;
    QPushButton *new_tx_button;
    QPushButton *new_sbs_button;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QWidget *vlg_toolkit_Connection)
    {
        if (vlg_toolkit_Connection->objectName().isEmpty())
            vlg_toolkit_Connection->setObjectName(QStringLiteral("vlg_toolkit_Connection"));
        vlg_toolkit_Connection->resize(949, 555);
        verticalLayout = new QVBoxLayout(vlg_toolkit_Connection);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(vlg_toolkit_Connection);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        conn_status_frame = new QFrame(frame);
        conn_status_frame->setObjectName(QStringLiteral("conn_status_frame"));
        conn_status_frame->setFrameShape(QFrame::Box);
        conn_status_frame->setFrameShadow(QFrame::Sunken);
        horizontalLayout_3 = new QHBoxLayout(conn_status_frame);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        conn_status_label = new QLabel(conn_status_frame);
        conn_status_label->setObjectName(QStringLiteral("conn_status_label"));

        horizontalLayout_2->addWidget(conn_status_label);

        conn_status_label_disp = new QLabel(conn_status_frame);
        conn_status_label_disp->setObjectName(QStringLiteral("conn_status_label_disp"));
        conn_status_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        conn_status_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(conn_status_label_disp);


        horizontalLayout_3->addLayout(horizontalLayout_2);


        horizontalLayout->addWidget(conn_status_frame);

        conn_status_frame_2 = new QFrame(frame);
        conn_status_frame_2->setObjectName(QStringLiteral("conn_status_frame_2"));
        conn_status_frame_2->setFrameShape(QFrame::Box);
        conn_status_frame_2->setFrameShadow(QFrame::Sunken);
        horizontalLayout_6 = new QHBoxLayout(conn_status_frame_2);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        sock_label = new QLabel(conn_status_frame_2);
        sock_label->setObjectName(QStringLiteral("sock_label"));

        horizontalLayout_7->addWidget(sock_label);

        sock_label_disp = new QLabel(conn_status_frame_2);
        sock_label_disp->setObjectName(QStringLiteral("sock_label_disp"));
        sock_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        sock_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_7->addWidget(sock_label_disp);


        horizontalLayout_6->addLayout(horizontalLayout_7);


        horizontalLayout->addWidget(conn_status_frame_2);

        conn_status_frame_3 = new QFrame(frame);
        conn_status_frame_3->setObjectName(QStringLiteral("conn_status_frame_3"));
        conn_status_frame_3->setFrameShape(QFrame::Box);
        conn_status_frame_3->setFrameShadow(QFrame::Sunken);
        horizontalLayout_8 = new QHBoxLayout(conn_status_frame_3);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        host_label = new QLabel(conn_status_frame_3);
        host_label->setObjectName(QStringLiteral("host_label"));

        horizontalLayout_9->addWidget(host_label);

        host_label_disp = new QLabel(conn_status_frame_3);
        host_label_disp->setObjectName(QStringLiteral("host_label_disp"));
        host_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        host_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_9->addWidget(host_label_disp);


        horizontalLayout_8->addLayout(horizontalLayout_9);


        horizontalLayout->addWidget(conn_status_frame_3);

        conn_status_frame_4 = new QFrame(frame);
        conn_status_frame_4->setObjectName(QStringLiteral("conn_status_frame_4"));
        conn_status_frame_4->setFrameShape(QFrame::Box);
        conn_status_frame_4->setFrameShadow(QFrame::Sunken);
        horizontalLayout_10 = new QHBoxLayout(conn_status_frame_4);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        port_label = new QLabel(conn_status_frame_4);
        port_label->setObjectName(QStringLiteral("port_label"));

        horizontalLayout_11->addWidget(port_label);

        port_label_disp = new QLabel(conn_status_frame_4);
        port_label_disp->setObjectName(QStringLiteral("port_label_disp"));
        port_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        port_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_11->addWidget(port_label_disp);


        horizontalLayout_10->addLayout(horizontalLayout_11);


        horizontalLayout->addWidget(conn_status_frame_4);

        conn_status_frame_6 = new QFrame(frame);
        conn_status_frame_6->setObjectName(QStringLiteral("conn_status_frame_6"));
        conn_status_frame_6->setFrameShape(QFrame::Box);
        conn_status_frame_6->setFrameShadow(QFrame::Sunken);
        horizontalLayout_16 = new QHBoxLayout(conn_status_frame_6);
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QStringLiteral("horizontalLayout_17"));
        port_label_2 = new QLabel(conn_status_frame_6);
        port_label_2->setObjectName(QStringLiteral("port_label_2"));

        horizontalLayout_17->addWidget(port_label_2);

        connid_label_disp = new QLabel(conn_status_frame_6);
        connid_label_disp->setObjectName(QStringLiteral("connid_label_disp"));
        connid_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        connid_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_17->addWidget(connid_label_disp);


        horizontalLayout_16->addLayout(horizontalLayout_17);


        horizontalLayout->addWidget(conn_status_frame_6);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(frame);

        cp_group_box = new QGroupBox(vlg_toolkit_Connection);
        cp_group_box->setObjectName(QStringLiteral("cp_group_box"));
        horizontalLayout_23 = new QHBoxLayout(cp_group_box);
        horizontalLayout_23->setObjectName(QStringLiteral("horizontalLayout_23"));
        conn_status_frame_10 = new QFrame(cp_group_box);
        conn_status_frame_10->setObjectName(QStringLiteral("conn_status_frame_10"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(conn_status_frame_10->sizePolicy().hasHeightForWidth());
        conn_status_frame_10->setSizePolicy(sizePolicy1);
        conn_status_frame_10->setFrameShape(QFrame::Box);
        conn_status_frame_10->setFrameShadow(QFrame::Sunken);
        horizontalLayout_4 = new QHBoxLayout(conn_status_frame_10);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_24 = new QHBoxLayout();
        horizontalLayout_24->setObjectName(QStringLiteral("horizontalLayout_24"));
        conn_status_label_5 = new QLabel(conn_status_frame_10);
        conn_status_label_5->setObjectName(QStringLiteral("conn_status_label_5"));

        horizontalLayout_24->addWidget(conn_status_label_5);

        cp_host_addr = new QLineEdit(conn_status_frame_10);
        cp_host_addr->setObjectName(QStringLiteral("cp_host_addr"));

        horizontalLayout_24->addWidget(cp_host_addr);


        horizontalLayout_4->addLayout(horizontalLayout_24);


        horizontalLayout_23->addWidget(conn_status_frame_10);

        conn_status_frame_9 = new QFrame(cp_group_box);
        conn_status_frame_9->setObjectName(QStringLiteral("conn_status_frame_9"));
        sizePolicy1.setHeightForWidth(conn_status_frame_9->sizePolicy().hasHeightForWidth());
        conn_status_frame_9->setSizePolicy(sizePolicy1);
        conn_status_frame_9->setFrameShape(QFrame::Box);
        conn_status_frame_9->setFrameShadow(QFrame::Sunken);
        horizontalLayout_18 = new QHBoxLayout(conn_status_frame_9);
        horizontalLayout_18->setObjectName(QStringLiteral("horizontalLayout_18"));
        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setObjectName(QStringLiteral("horizontalLayout_22"));
        conn_status_label_4 = new QLabel(conn_status_frame_9);
        conn_status_label_4->setObjectName(QStringLiteral("conn_status_label_4"));

        horizontalLayout_22->addWidget(conn_status_label_4);

        cp_host_port = new QLineEdit(conn_status_frame_9);
        cp_host_port->setObjectName(QStringLiteral("cp_host_port"));

        horizontalLayout_22->addWidget(cp_host_port);


        horizontalLayout_18->addLayout(horizontalLayout_22);


        horizontalLayout_23->addWidget(conn_status_frame_9);

        conn_status_frame_8 = new QFrame(cp_group_box);
        conn_status_frame_8->setObjectName(QStringLiteral("conn_status_frame_8"));
        sizePolicy1.setHeightForWidth(conn_status_frame_8->sizePolicy().hasHeightForWidth());
        conn_status_frame_8->setSizePolicy(sizePolicy1);
        conn_status_frame_8->setFrameShape(QFrame::Box);
        conn_status_frame_8->setFrameShadow(QFrame::Sunken);
        horizontalLayout_19 = new QHBoxLayout(conn_status_frame_8);
        horizontalLayout_19->setObjectName(QStringLiteral("horizontalLayout_19"));
        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName(QStringLiteral("horizontalLayout_20"));
        conn_status_label_3 = new QLabel(conn_status_frame_8);
        conn_status_label_3->setObjectName(QStringLiteral("conn_status_label_3"));

        horizontalLayout_20->addWidget(conn_status_label_3);

        cp_user = new QLineEdit(conn_status_frame_8);
        cp_user->setObjectName(QStringLiteral("cp_user"));

        horizontalLayout_20->addWidget(cp_user);


        horizontalLayout_19->addLayout(horizontalLayout_20);


        horizontalLayout_23->addWidget(conn_status_frame_8);

        conn_status_frame_7 = new QFrame(cp_group_box);
        conn_status_frame_7->setObjectName(QStringLiteral("conn_status_frame_7"));
        sizePolicy1.setHeightForWidth(conn_status_frame_7->sizePolicy().hasHeightForWidth());
        conn_status_frame_7->setSizePolicy(sizePolicy1);
        conn_status_frame_7->setFrameShape(QFrame::Box);
        conn_status_frame_7->setFrameShadow(QFrame::Sunken);
        horizontalLayout_21 = new QHBoxLayout(conn_status_frame_7);
        horizontalLayout_21->setObjectName(QStringLiteral("horizontalLayout_21"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        conn_status_label_2 = new QLabel(conn_status_frame_7);
        conn_status_label_2->setObjectName(QStringLiteral("conn_status_label_2"));

        horizontalLayout_5->addWidget(conn_status_label_2);

        cp_psswd = new QLineEdit(conn_status_frame_7);
        cp_psswd->setObjectName(QStringLiteral("cp_psswd"));

        horizontalLayout_5->addWidget(cp_psswd);


        horizontalLayout_21->addLayout(horizontalLayout_5);


        horizontalLayout_23->addWidget(conn_status_frame_7);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_23->addItem(horizontalSpacer_4);


        verticalLayout->addWidget(cp_group_box);

        frame_2 = new QFrame(vlg_toolkit_Connection);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame_2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        groupBox = new QGroupBox(frame_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        horizontalLayout_14 = new QHBoxLayout(groupBox);
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        connect_button = new QPushButton(groupBox);
        connect_button->setObjectName(QStringLiteral("connect_button"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icon/icons/flash.png"), QSize(), QIcon::Normal, QIcon::Off);
        connect_button->setIcon(icon);

        horizontalLayout_14->addWidget(connect_button);

        disconnect_button = new QPushButton(groupBox);
        disconnect_button->setObjectName(QStringLiteral("disconnect_button"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icon/icons/flash-off.png"), QSize(), QIcon::Normal, QIcon::Off);
        disconnect_button->setIcon(icon1);
        disconnect_button->setFlat(false);

        horizontalLayout_14->addWidget(disconnect_button);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer_2);


        verticalLayout_3->addWidget(groupBox);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        peer_model_tree_view = new QTreeView(frame_2);
        peer_model_tree_view->setObjectName(QStringLiteral("peer_model_tree_view"));
        peer_model_tree_view->setContextMenuPolicy(Qt::CustomContextMenu);
        peer_model_tree_view->setAlternatingRowColors(true);
        peer_model_tree_view->setSortingEnabled(true);

        horizontalLayout_15->addWidget(peer_model_tree_view);

        model_operations_box = new QGroupBox(frame_2);
        model_operations_box->setObjectName(QStringLiteral("model_operations_box"));
        verticalLayout_2 = new QVBoxLayout(model_operations_box);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        extend_model_button = new QPushButton(model_operations_box);
        extend_model_button->setObjectName(QStringLiteral("extend_model_button"));
        extend_model_button->setEnabled(false);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icon/icons/social-buffer.png"), QSize(), QIcon::Normal, QIcon::Off);
        extend_model_button->setIcon(icon2);

        verticalLayout_2->addWidget(extend_model_button);

        new_tx_button = new QPushButton(model_operations_box);
        new_tx_button->setObjectName(QStringLiteral("new_tx_button"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icon/icons/forward.png"), QSize(), QIcon::Normal, QIcon::Off);
        new_tx_button->setIcon(icon3);

        verticalLayout_2->addWidget(new_tx_button);

        new_sbs_button = new QPushButton(model_operations_box);
        new_sbs_button->setObjectName(QStringLiteral("new_sbs_button"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icon/icons/social-rss.png"), QSize(), QIcon::Normal, QIcon::Off);
        new_sbs_button->setIcon(icon4);

        verticalLayout_2->addWidget(new_sbs_button);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout_15->addWidget(model_operations_box);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_15->addItem(horizontalSpacer_3);


        verticalLayout_3->addLayout(horizontalLayout_15);


        verticalLayout->addWidget(frame_2);

#ifndef QT_NO_SHORTCUT
        conn_status_label_5->setBuddy(cp_psswd);
        conn_status_label_4->setBuddy(cp_psswd);
        conn_status_label_3->setBuddy(cp_psswd);
        conn_status_label_2->setBuddy(cp_psswd);
#endif // QT_NO_SHORTCUT

        retranslateUi(vlg_toolkit_Connection);

        QMetaObject::connectSlotsByName(vlg_toolkit_Connection);
    } // setupUi

    void retranslateUi(QWidget *vlg_toolkit_Connection)
    {
        vlg_toolkit_Connection->setWindowTitle(QApplication::translate("vlg_toolkit_Connection", "Form", Q_NULLPTR));
        conn_status_label->setText(QApplication::translate("vlg_toolkit_Connection", "Status", Q_NULLPTR));
        conn_status_label_disp->setText(QApplication::translate("vlg_toolkit_Connection", "-", Q_NULLPTR));
        sock_label->setText(QApplication::translate("vlg_toolkit_Connection", "Socket", Q_NULLPTR));
        sock_label_disp->setText(QApplication::translate("vlg_toolkit_Connection", "-", Q_NULLPTR));
        host_label->setText(QApplication::translate("vlg_toolkit_Connection", "Host", Q_NULLPTR));
        host_label_disp->setText(QApplication::translate("vlg_toolkit_Connection", "-", Q_NULLPTR));
        port_label->setText(QApplication::translate("vlg_toolkit_Connection", "Port", Q_NULLPTR));
        port_label_disp->setText(QApplication::translate("vlg_toolkit_Connection", "-", Q_NULLPTR));
        port_label_2->setText(QApplication::translate("vlg_toolkit_Connection", "CONNID", Q_NULLPTR));
        connid_label_disp->setText(QApplication::translate("vlg_toolkit_Connection", "-", Q_NULLPTR));
        cp_group_box->setTitle(QApplication::translate("vlg_toolkit_Connection", "Connection Params", Q_NULLPTR));
        conn_status_label_5->setText(QApplication::translate("vlg_toolkit_Connection", "Host", Q_NULLPTR));
        conn_status_label_4->setText(QApplication::translate("vlg_toolkit_Connection", "Port", Q_NULLPTR));
        conn_status_label_3->setText(QApplication::translate("vlg_toolkit_Connection", "User", Q_NULLPTR));
        conn_status_label_2->setText(QApplication::translate("vlg_toolkit_Connection", "Password", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("vlg_toolkit_Connection", "Connection Operations", Q_NULLPTR));
        connect_button->setText(QApplication::translate("vlg_toolkit_Connection", "Connect", Q_NULLPTR));
        disconnect_button->setText(QApplication::translate("vlg_toolkit_Connection", "Disconnect", Q_NULLPTR));
        model_operations_box->setTitle(QApplication::translate("vlg_toolkit_Connection", "Model Operations", Q_NULLPTR));
        extend_model_button->setText(QApplication::translate("vlg_toolkit_Connection", "Extend Model", Q_NULLPTR));
        new_tx_button->setText(QApplication::translate("vlg_toolkit_Connection", "New TX", Q_NULLPTR));
        new_sbs_button->setText(QApplication::translate("vlg_toolkit_Connection", "New SBS", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class vlg_toolkit_Connection: public Ui_vlg_toolkit_Connection {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VLG_TOOLKIT_CONNECTION_H
