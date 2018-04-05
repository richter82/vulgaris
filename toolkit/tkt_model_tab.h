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

#ifndef VLG_TOOLKIT_MODEL_TAB_H
#define VLG_TOOLKIT_MODEL_TAB_H

#include "tkt_glob.h"
#include "tkt_vlg_model.h"

//------------------------------------------------------------------------------
// vlg_toolkit_Model_mdl
//------------------------------------------------------------------------------

class vlg_toolkit_Model_mdl : public QSortFilterProxyModel {

    public:
        vlg_toolkit_Model_mdl(vlg_toolkit_vlg_model &wrapped_mdl, QObject *parent = 0);


        vlg_toolkit_vlg_model &wrapped_mdl();

    private:
        vlg_toolkit_vlg_model &wrapped_mdl_;
};

//------------------------------------------------------------------------------
// vlg_toolkit_model_tab
//------------------------------------------------------------------------------

namespace Ui {
class vlg_toolkit_model_tab;
}

class vlg_toolkit_model_tab : public QWidget {
        Q_OBJECT

    public:
        explicit vlg_toolkit_model_tab(vlg_toolkit_vlg_model &vlg_model_loaded_model,
                                       QWidget *parent = 0);
        ~vlg_toolkit_model_tab();

        vlg_toolkit_Model_mdl &b_mdl();


    public slots:
        void On_VLG_MODEL_Update();

    private:
        vlg_toolkit_Model_mdl b_mdl_;

    private:
        Ui::vlg_toolkit_model_tab *ui;
};

#endif // VLG_TOOLKIT_MODEL_TAB_H
