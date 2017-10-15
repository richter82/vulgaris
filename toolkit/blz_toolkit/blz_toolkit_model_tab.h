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

#ifndef BLZ_TOOLKIT_MODEL_TAB_H
#define BLZ_TOOLKIT_MODEL_TAB_H

#include "blz_toolkit_glob.h"
#include "blz_toolkit_blz_model.h"

//------------------------------------------------------------------------------
// blz_toolkit_Model_mdl
//------------------------------------------------------------------------------

class blz_toolkit_Model_mdl : public QSortFilterProxyModel {

    public:
        blz_toolkit_Model_mdl(blz_toolkit_blz_model &wrapped_mdl, QObject *parent = 0);


        blz_toolkit_blz_model &wrapped_mdl();

    private:
        blz_toolkit_blz_model &wrapped_mdl_;
};

//------------------------------------------------------------------------------
// blz_toolkit_model_tab
//------------------------------------------------------------------------------

namespace Ui {
class blz_toolkit_model_tab;
}

class blz_toolkit_model_tab : public QWidget {
        Q_OBJECT

    public:
        explicit blz_toolkit_model_tab(blz_toolkit_blz_model &blz_model_loaded_model,
                                       QWidget *parent = 0);
        ~blz_toolkit_model_tab();

        blz_toolkit_Model_mdl &b_mdl();


    public slots:
        void On_BLZ_MODEL_Update();

    private:
        blz_toolkit_Model_mdl b_mdl_;

    private:
        Ui::blz_toolkit_model_tab *ui;
};

#endif // BLZ_TOOLKIT_MODEL_TAB_H
