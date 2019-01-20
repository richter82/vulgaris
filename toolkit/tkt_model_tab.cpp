/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#include "tkt_model_tab.h"
#include "ui_tkt_model_tab.h"

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
    ui->broker_model_tree_main_view->setModel(&b_mdl_);
}

vlg_toolkit_model_tab::~vlg_toolkit_model_tab()
{
    delete ui;
    ui = nullptr;
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






