/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
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
