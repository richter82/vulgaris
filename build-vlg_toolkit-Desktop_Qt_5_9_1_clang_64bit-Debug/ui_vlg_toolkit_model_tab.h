/********************************************************************************
** Form generated from reading UI file 'vlg_toolkit_model_tab.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VLG_TOOLKIT_MODEL_TAB_H
#define UI_VLG_TOOLKIT_MODEL_TAB_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_vlg_toolkit_model_tab
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *vlg_model_top_frame;
    QHBoxLayout *horizontalLayout;
    QFrame *conn_status_frame;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *conn_status_label;
    QLabel *entities_no_label_disp;
    QFrame *conn_status_frame_5;
    QHBoxLayout *horizontalLayout_12;
    QHBoxLayout *horizontalLayout_13;
    QLabel *last_sock_err;
    QLabel *enum_no_label_disp;
    QFrame *conn_status_frame_3;
    QHBoxLayout *horizontalLayout_8;
    QHBoxLayout *horizontalLayout_9;
    QLabel *host_label;
    QLabel *class_no_label_disp;
    QSpacerItem *horizontalSpacer;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_15;
    QTreeView *peer_model_tree_main_view;

    void setupUi(QWidget *vlg_toolkit_model_tab)
    {
        if (vlg_toolkit_model_tab->objectName().isEmpty())
            vlg_toolkit_model_tab->setObjectName(QStringLiteral("vlg_toolkit_model_tab"));
        vlg_toolkit_model_tab->resize(850, 546);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icon/icons/forward.png"), QSize(), QIcon::Normal, QIcon::Off);
        vlg_toolkit_model_tab->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(vlg_toolkit_model_tab);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        vlg_model_top_frame = new QFrame(vlg_toolkit_model_tab);
        vlg_model_top_frame->setObjectName(QStringLiteral("vlg_model_top_frame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(vlg_model_top_frame->sizePolicy().hasHeightForWidth());
        vlg_model_top_frame->setSizePolicy(sizePolicy);
        vlg_model_top_frame->setFrameShape(QFrame::StyledPanel);
        vlg_model_top_frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(vlg_model_top_frame);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        conn_status_frame = new QFrame(vlg_model_top_frame);
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

        entities_no_label_disp = new QLabel(conn_status_frame);
        entities_no_label_disp->setObjectName(QStringLiteral("entities_no_label_disp"));
        entities_no_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        entities_no_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(entities_no_label_disp);


        horizontalLayout_3->addLayout(horizontalLayout_2);


        horizontalLayout->addWidget(conn_status_frame);

        conn_status_frame_5 = new QFrame(vlg_model_top_frame);
        conn_status_frame_5->setObjectName(QStringLiteral("conn_status_frame_5"));
        conn_status_frame_5->setFrameShape(QFrame::Box);
        conn_status_frame_5->setFrameShadow(QFrame::Sunken);
        horizontalLayout_12 = new QHBoxLayout(conn_status_frame_5);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        last_sock_err = new QLabel(conn_status_frame_5);
        last_sock_err->setObjectName(QStringLiteral("last_sock_err"));

        horizontalLayout_13->addWidget(last_sock_err);

        enum_no_label_disp = new QLabel(conn_status_frame_5);
        enum_no_label_disp->setObjectName(QStringLiteral("enum_no_label_disp"));
        enum_no_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        enum_no_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(enum_no_label_disp);


        horizontalLayout_12->addLayout(horizontalLayout_13);


        horizontalLayout->addWidget(conn_status_frame_5);

        conn_status_frame_3 = new QFrame(vlg_model_top_frame);
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

        class_no_label_disp = new QLabel(conn_status_frame_3);
        class_no_label_disp->setObjectName(QStringLiteral("class_no_label_disp"));
        class_no_label_disp->setStyleSheet(QStringLiteral("background-color : light grey; color : black;"));
        class_no_label_disp->setAlignment(Qt::AlignCenter);

        horizontalLayout_9->addWidget(class_no_label_disp);


        horizontalLayout_8->addLayout(horizontalLayout_9);


        horizontalLayout->addWidget(conn_status_frame_3);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(vlg_model_top_frame);

        frame_2 = new QFrame(vlg_toolkit_model_tab);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame_2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        peer_model_tree_main_view = new QTreeView(frame_2);
        peer_model_tree_main_view->setObjectName(QStringLiteral("peer_model_tree_main_view"));
        peer_model_tree_main_view->setAlternatingRowColors(true);
        peer_model_tree_main_view->setSortingEnabled(true);

        horizontalLayout_15->addWidget(peer_model_tree_main_view);


        verticalLayout_3->addLayout(horizontalLayout_15);


        verticalLayout->addWidget(frame_2);


        retranslateUi(vlg_toolkit_model_tab);

        QMetaObject::connectSlotsByName(vlg_toolkit_model_tab);
    } // setupUi

    void retranslateUi(QWidget *vlg_toolkit_model_tab)
    {
        vlg_toolkit_model_tab->setWindowTitle(QApplication::translate("vlg_toolkit_model_tab", "Form", Q_NULLPTR));
        conn_status_label->setText(QApplication::translate("vlg_toolkit_model_tab", "Entities No.", Q_NULLPTR));
        entities_no_label_disp->setText(QApplication::translate("vlg_toolkit_model_tab", "Undef", Q_NULLPTR));
        last_sock_err->setText(QApplication::translate("vlg_toolkit_model_tab", "Enum No.", Q_NULLPTR));
        enum_no_label_disp->setText(QApplication::translate("vlg_toolkit_model_tab", "Undef", Q_NULLPTR));
        host_label->setText(QApplication::translate("vlg_toolkit_model_tab", "Class No.", Q_NULLPTR));
        class_no_label_disp->setText(QApplication::translate("vlg_toolkit_model_tab", "Undef", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class vlg_toolkit_model_tab: public Ui_vlg_toolkit_model_tab {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VLG_TOOLKIT_MODEL_TAB_H
