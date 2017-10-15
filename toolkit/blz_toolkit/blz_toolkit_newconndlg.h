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

#ifndef NEWCONNDIALOG_H
#define NEWCONNDIALOG_H

#include "ui_blz_toolkit_newconndlg.h"
#include "blz_toolkit_glob.h"

class blz_toolkit_NewConnDlg : public QDialog {
        Q_OBJECT

    public:
        explicit blz_toolkit_NewConnDlg(QWidget *parent = 0);
        ~blz_toolkit_NewConnDlg();

    public:
        Ui::NewConnDialog *ui;
};

#endif // NEWCONNDIALOG_H
