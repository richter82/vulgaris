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

#include "vlg_toolkit_model_tab.h"
#include "ui_vlg_toolkit_model_tab.h"

//------------------------------------------------------------------------------
// vlg_toolkit_Model_mdl
//------------------------------------------------------------------------------

vlg_toolkit_Model_mdl::vlg_toolkit_Model_mdl(vlg_toolkit_vlg_model &wrapped_mdl,
                                             QObject *parent) :
    wrapped_mdl_(wrapped_mdl),
    QSortFilterProxyModel(parent)
{
    setSourceModel(&wrapped_mdl);
}

vlg_toolkit_vlg_model &vlg_toolkit_Model_mdl::wrapped_mdl()
{
    return wrapped_mdl_;
}


//------------------------------------------------------------------------------
// vlg_toolkit_model_tab
//------------------------------------------------------------------------------

vlg_toolkit_model_tab::vlg_toolkit_model_tab(vlg_toolkit_vlg_model
                                             &vlg_model_loaded_model, QWidget *parent) :
    b_mdl_(vlg_model_loaded_model, this),
    QWidget(parent),
    ui(new Ui::vlg_toolkit_model_tab)
{
    ui->setupUi(this);
    b_mdl_.setSourceModel(&vlg_model_loaded_model);
    ui->peer_model_tree_main_view->setModel(&b_mdl_);
}

vlg_toolkit_model_tab::~vlg_toolkit_model_tab()
{
    delete ui;
}

void vlg_toolkit_model_tab::On_VLG_MODEL_Update()
{
    ui->entities_no_label_disp->setText(QString("%1").arg(
                                            b_mdl_.wrapped_mdl().bem().nentity_count()));
    ui->enum_no_label_disp->setText(QString("%1").arg(
                                        b_mdl_.wrapped_mdl().bem().nenum_count()));
    ui->class_no_label_disp->setText(QString("%1").arg(
                                         b_mdl_.wrapped_mdl().bem().nclass_count()));
}
vlg_toolkit_Model_mdl &vlg_toolkit_model_tab::b_mdl()
{
    return b_mdl_;
}






