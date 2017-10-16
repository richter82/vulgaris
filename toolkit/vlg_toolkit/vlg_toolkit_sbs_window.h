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

#ifndef BLZ_TOOLKIT_SBS_WINDOW_H
#define BLZ_TOOLKIT_SBS_WINDOW_H

#include "blz_toolkit_blz_model.h"
#include "blz_toolkit_sbs_blz_class_model.h"

namespace Ui {
class blz_toolkit_sbs_window;
}

//------------------------------------------------------------------------------
// blz_toolkit_sbs_model
//------------------------------------------------------------------------------

class blz_toolkit_sbs_model : public QSortFilterProxyModel {

    public:
        blz_toolkit_sbs_model(blz_toolkit_sbs_blz_class_model &wrapped_mdl,
                              QObject *parent = 0);


        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    public:
        void offerEntry(blaze::nclass *entry);
        blz_toolkit_sbs_blz_class_model &wrapped_mdl();

    private:
        blz_toolkit_sbs_blz_class_model &wrapped_mdl_;
};

//------------------------------------------------------------------------------
// blz_toolkit_sbs_window
//------------------------------------------------------------------------------

class blz_toolkit_sbs_window : public QMainWindow {
        Q_OBJECT

    public:
        explicit blz_toolkit_sbs_window(const blaze::entity_desc &edesc,
                                        const blaze::entity_manager &bem,
                                        blaze::subscription_int &sbs,
                                        blz_toolkit_sbs_blz_class_model &mdl,
                                        QWidget *parent = 0);
        ~blz_toolkit_sbs_window();

        blaze::subscription_int &sbs() const;

    protected:
        void closeEvent(QCloseEvent *event);


    private slots:
        void on_actionStart_SBS_triggered();
        void on_actionStop_SBS_triggered();

    public slots:
        void OnSbsStatusChange(blaze::SubscriptionStatus status);
        void OnSbsEvent(blaze::subscription_event_int *sbs_evt);
        void OnCustomMenuRequested(const QPoint &pos);
        void OnNewTxRequested();

    signals:
        void SignalSbsStatusChange(blaze::SubscriptionStatus status);
        void SignalSbsEvent(blaze::subscription_event_int *sbs_evt);

    public:
        void EmitSbsStatus(blaze::SubscriptionStatus status);
        void EmitSbsEvent(blaze::subscription_event_int *sbs_evt);

    public:
        friend void sbs_status_change_hndlr(blaze::subscription_int &sbs,
                                            blaze::SubscriptionStatus status,
                                            void *ud);

        friend void sbs_evt_notify_hndlr(blaze::subscription_int &sbs,
                                         blaze::subscription_event_int &sbs_evt,
                                         void *ud);

    private:
        void SbsStartedActions();
        void SbsStoppedActions();


        /*****
         REP
         ****/

    private:
        blaze::subscription_int &sbs_;
        blz_toolkit_sbs_model sbs_mdl_;


    public:
        Ui::blz_toolkit_sbs_window *ui;

};

#endif // BLZ_TOOLKIT_SBS_WINDOW_H
