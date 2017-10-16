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

#include "blz_toolkit_model_tab.h"
#include "ui_blz_toolkit_model_tab.h"

//------------------------------------------------------------------------------
// blz_toolkit_Model_mdl
//------------------------------------------------------------------------------

blz_toolkit_Model_mdl::blz_toolkit_Model_mdl(blz_toolkit_blz_model &wrapped_mdl,
                                             QObject *parent) :
    wrapped_mdl_(wrapped_mdl),
    QSortFilterProxyModel(parent)
{
    setSourceModel(&wrapped_mdl);
}

blz_toolkit_blz_model &blz_toolkit_Model_mdl::wrapped_mdl()
{
    return wrapped_mdl_;
}


//------------------------------------------------------------------------------
// blz_toolkit_model_tab
//------------------------------------------------------------------------------

blz_toolkit_model_tab::blz_toolkit_model_tab(blz_toolkit_blz_model
                                             &blz_model_loaded_model, QWidget *parent) :
    b_mdl_(blz_model_loaded_model, this),
    QWidget(parent),
    ui(new Ui::blz_toolkit_model_tab)
{
    ui->setupUi(this);
    b_mdl_.setSourceModel(&blz_model_loaded_model);
    ui->peer_model_tree_main_view->setModel(&b_mdl_);
}

blz_toolkit_model_tab::~blz_toolkit_model_tab()
{
    delete ui;
}

void blz_toolkit_model_tab::On_BLZ_MODEL_Update()
{
    ui->entities_no_label_disp->setText(QString("%1").arg(
                                            b_mdl_.wrapped_mdl().bem().entity_count()));
    ui->enum_no_label_disp->setText(QString("%1").arg(
                                        b_mdl_.wrapped_mdl().bem().enum_count()));
    ui->class_no_label_disp->setText(QString("%1").arg(
                                         b_mdl_.wrapped_mdl().bem().nclass_count()));
}
blz_toolkit_Model_mdl &blz_toolkit_model_tab::b_mdl()
{
    return b_mdl_;
}






