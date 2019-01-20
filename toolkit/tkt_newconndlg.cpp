/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#include "tkt_newconndlg.h"

vlg_toolkit_NewConnDlg::vlg_toolkit_NewConnDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewConnDialog)
{
    ui->setupUi(this);
}

vlg_toolkit_NewConnDlg::~vlg_toolkit_NewConnDlg()
{
    delete ui;
    ui = nullptr;
}
