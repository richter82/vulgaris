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

#pragma once
#include "tkt_vlg_model.h"
#include "tkt_sbs_vlg_nclass_model.h"

//------------------------------------------------------------------------------
// toolkit_subscription
//------------------------------------------------------------------------------
class vlg_toolkit_sbs_window;
class toolkit_subscription : public vlg::outgoing_subscription {
    public:
        toolkit_subscription(vlg_toolkit_sbs_window &widget);

        virtual void on_status_change(vlg::SubscriptionStatus current) override;
        virtual void on_incoming_event(std::unique_ptr<vlg::subscription_event> &) override;

    private:
        vlg_toolkit_sbs_window &widget_;
};


namespace Ui {
class vlg_toolkit_sbs_window;
}

//------------------------------------------------------------------------------
// vlg_toolkit_sbs_model
//------------------------------------------------------------------------------

class vlg_toolkit_sbs_model : public QSortFilterProxyModel {

    public:
        vlg_toolkit_sbs_model(vlg_toolkit_sbs_vlg_class_model &wrapped_mdl,
                              QObject *parent = 0);


        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    public:
        void offerEntry(std::shared_ptr<vlg::subscription_event> &);
        vlg_toolkit_sbs_vlg_class_model &wrapped_mdl();

    private:
        vlg_toolkit_sbs_vlg_class_model &wrapped_mdl_;
};

//------------------------------------------------------------------------------
// vlg_toolkit_sbs_window
//------------------------------------------------------------------------------

class vlg_toolkit_sbs_window : public QMainWindow {
        Q_OBJECT

    public:
        explicit vlg_toolkit_sbs_window(vlg::outgoing_connection &conn,
                                        const vlg::nentity_desc &edesc,
                                        QWidget *parent = 0);
        ~vlg_toolkit_sbs_window();

    protected:
        void closeEvent(QCloseEvent *event);


    private slots:
        void on_actionStart_SBS_triggered();
        void on_actionStop_SBS_triggered();

    public slots:
        void OnSbsStatusChange(vlg::SubscriptionStatus status);
        void OnSbsEvent(std::shared_ptr<vlg::subscription_event>);
        void OnCustomMenuRequested(const QPoint &pos);
        void OnNewTxRequested();

    signals:
        void SignalSbsStatusChange(vlg::SubscriptionStatus status);
        void SignalSbsEvent(std::shared_ptr<vlg::subscription_event>);

    public:
        void EmitSbsStatus(vlg::SubscriptionStatus status);

    private:
        void SbsStartedActions();
        void SbsStoppedActions();

    private:
        toolkit_subscription  sbs_;
        vlg_toolkit_sbs_vlg_class_model sbs_mdl_;
        vlg_toolkit_sbs_model sbs_mdl_wr_;

    public:
        Ui::vlg_toolkit_sbs_window *ui;

};
