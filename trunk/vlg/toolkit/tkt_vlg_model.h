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
#include "tkt_glob.h"

//------------------------------------------------------------------------------
// VLG_MODEL_ITEM
//------------------------------------------------------------------------------
enum VLG_MODEL_ITEM_TYPE {
    VLG_MODEL_ITEM_TYPE_Undef,
    VLG_MODEL_ITEM_TYPE_ROOT,     //ROOT
    VLG_MODEL_ITEM_TYPE_EDESC,     //nentity_desc
    VLG_MODEL_ITEM_TYPE_MDESC,     //member_desc
};

class model_item {
    public:
        model_item(VLG_MODEL_ITEM_TYPE item_type,
                   const vlg::nentity_desc *edesc,
                   const vlg::member_desc *mdesc,
                   model_item *parent = NULL);

        ~model_item();

        void cfgHeader();
        void appendChild(model_item *child);
        model_item *child(int row);
        int childCount()  const;
        int columnCount() const;
        QVariant data(int column) const;
        int row() const;
        model_item *parent();

        VLG_MODEL_ITEM_TYPE item_type() const;

        const vlg::nentity_desc *edesc() const;
        const vlg::member_desc *mdesc() const;

    private:
        QList<QString> header_;

    private:
        QList<model_item *> childItems_;
        model_item *parentItem_;

    private:
        VLG_MODEL_ITEM_TYPE item_type_;
        const vlg::nentity_desc *edesc_;
        const vlg::member_desc *mdesc_;
};


//------------------------------------------------------------------------------
// vlg_toolkit_vlg_model
//------------------------------------------------------------------------------

class vlg_toolkit_vlg_model : public QAbstractItemModel {
        Q_OBJECT

    public slots:
        void OnModelUpdate_event();

    public:
        vlg_toolkit_vlg_model(vlg::nentity_manager &bem, QObject *parent = 0);
        ~vlg_toolkit_vlg_model();

        //overriden methods
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;

        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;

        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;
        QVariant data(const QModelIndex &index, int role) const;

        model_item *rootItem() const;
        vlg::nentity_manager &bem() const;

        QList<model_item *> parents() const;

    private:
        void updateModelData(model_item *parent);

        //bem source for this model.
    private:
        vlg::nentity_manager &bem_;
        model_item *rootItem_;
};
