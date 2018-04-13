/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "ui_tkt_newconndlg.h"
#include "tkt_glob.h"

class vlg_toolkit_NewConnDlg : public QDialog {
        Q_OBJECT

    public:
        explicit vlg_toolkit_NewConnDlg(QWidget *parent = 0);
        ~vlg_toolkit_NewConnDlg();

    public:
        Ui::NewConnDialog *ui;
};
