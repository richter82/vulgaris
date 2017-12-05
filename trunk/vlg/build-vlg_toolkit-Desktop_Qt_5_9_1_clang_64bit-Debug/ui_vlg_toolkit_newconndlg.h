/********************************************************************************
** Form generated from reading UI file 'vlg_toolkit_newconndlg.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VLG_TOOLKIT_NEWCONNDLG_H
#define UI_VLG_TOOLKIT_NEWCONNDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewConnDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *ln_edt_host;
    QLabel *label_2;
    QLineEdit *ln_edt_port;
    QLabel *label_3;
    QLineEdit *ln_edt_usr;
    QLabel *label_4;
    QLineEdit *ln_edt_psswd;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewConnDialog)
    {
        if (NewConnDialog->objectName().isEmpty())
            NewConnDialog->setObjectName(QStringLiteral("NewConnDialog"));
        NewConnDialog->resize(233, 186);
        verticalLayout = new QVBoxLayout(NewConnDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(NewConnDialog);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        ln_edt_host = new QLineEdit(NewConnDialog);
        ln_edt_host->setObjectName(QStringLiteral("ln_edt_host"));

        formLayout->setWidget(0, QFormLayout::FieldRole, ln_edt_host);

        label_2 = new QLabel(NewConnDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        ln_edt_port = new QLineEdit(NewConnDialog);
        ln_edt_port->setObjectName(QStringLiteral("ln_edt_port"));

        formLayout->setWidget(1, QFormLayout::FieldRole, ln_edt_port);

        label_3 = new QLabel(NewConnDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        ln_edt_usr = new QLineEdit(NewConnDialog);
        ln_edt_usr->setObjectName(QStringLiteral("ln_edt_usr"));

        formLayout->setWidget(2, QFormLayout::FieldRole, ln_edt_usr);

        label_4 = new QLabel(NewConnDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        ln_edt_psswd = new QLineEdit(NewConnDialog);
        ln_edt_psswd->setObjectName(QStringLiteral("ln_edt_psswd"));

        formLayout->setWidget(3, QFormLayout::FieldRole, ln_edt_psswd);


        verticalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(NewConnDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(NewConnDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), NewConnDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NewConnDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewConnDialog);
    } // setupUi

    void retranslateUi(QDialog *NewConnDialog)
    {
        NewConnDialog->setWindowTitle(QApplication::translate("NewConnDialog", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("NewConnDialog", "Host", Q_NULLPTR));
        ln_edt_host->setText(QApplication::translate("NewConnDialog", "127.0.0.1", Q_NULLPTR));
        label_2->setText(QApplication::translate("NewConnDialog", "Port", Q_NULLPTR));
        ln_edt_port->setText(QApplication::translate("NewConnDialog", "12345", Q_NULLPTR));
        label_3->setText(QApplication::translate("NewConnDialog", "User", Q_NULLPTR));
        label_4->setText(QApplication::translate("NewConnDialog", "Password", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class NewConnDialog: public Ui_NewConnDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VLG_TOOLKIT_NEWCONNDLG_H
