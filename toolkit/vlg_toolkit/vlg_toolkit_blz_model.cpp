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

#include "blz_toolkit_blz_model.h"

//------------------------------------------------------------------------------
// BLZ_MODEL_ITEM
//------------------------------------------------------------------------------

model_item::model_item(BLZ_MODEL_ITEM_TYPE item_type,
                       const vlg::entity_desc *edesc,
                       const vlg::member_desc *mdesc,
                       model_item *parent) :
    item_type_(item_type),
    edesc_(edesc),
    mdesc_(mdesc),
    parentItem_(parent)
{
    if(item_type == BLZ_MODEL_ITEM_TYPE_ROOT) {
        cfgHeader();
    }
}

model_item::~model_item()
{
    qDeleteAll(childItems_);
}

void model_item::cfgHeader()
{
    header_ << "Name";
    header_ << "Namespace";
    header_ << "Entity Type";
    header_ << "Entity CLASSID";
    header_ << "Field No.";
    header_ << "Persistent";
    header_ << "vlg Type";
    header_ << "TypeSize (1)";
    header_ << "NMEMB";
    header_ << "Field CLASSID";
    header_ << "Field usr. Type";
    header_ << "Field Entity Type";
}

void model_item::appendChild(model_item *item)
{
    childItems_.append(item);
}

model_item *model_item::child(int row)
{
    return childItems_.value(row);
}

int model_item::childCount() const
{
    return childItems_.count();
}

int model_item::columnCount() const
{
    return 13;
}

QVariant model_item::data(int column) const
{
    if(item_type_ == BLZ_MODEL_ITEM_TYPE_ROOT) {
        if(column < header_.size()) {
            return header_[column];
        }
    } else if(item_type_ == BLZ_MODEL_ITEM_TYPE_EDESC) {
        if(edesc_) {
            switch(column) {
                case 0:
                    return QString(edesc_->get_entity_name());
                case 1:
                    return QString(edesc_->get_entity_namespace());
                case 2:
                    if(edesc_->get_entity_type() != vlg::EntityType_UNDEFINED) {
                        return QString(string_from_EntityType(edesc_->get_entity_type()));
                    }
                    break;
                case 3:
                    if(edesc_->get_entity_type() == vlg::EntityType_NCLASS) {
                        return QString("%1").arg(edesc_->get_nclass_id());
                    }
                    break;
                case 4:
                    return QString("%1").arg(edesc_->get_entity_member_num());
                case 5:
                    return QString(edesc_->is_persistent() ? "Yes" : "No");
                default:
                    break;
            }
        }
    } else if(item_type_ == BLZ_MODEL_ITEM_TYPE_MDESC) {
        if(mdesc_) {
            switch(column) {
                case 0:
                    return QString(mdesc_->get_member_name());
                case 6:
                    return QString(string_from_Type(mdesc_->get_field_blz_type()));
                case 7:
                    return QString("%1").arg(mdesc_->get_field_type_size());
                case 8:
                    return QString("%1").arg(mdesc_->get_field_nmemb());
                case 9:
                    if(mdesc_->get_field_entity_type() == vlg::EntityType_NCLASS) {
                        return QString("%1").arg(mdesc_->get_field_class_id());
                    }
                    break;
                case 10:
                    return QString(mdesc_->get_field_user_type());
                case 11:
                    if(mdesc_->get_field_entity_type() != vlg::EntityType_UNDEFINED) {
                        return QString(string_from_EntityType(mdesc_->get_field_entity_type()));
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return QVariant();
}

model_item *model_item::parent()
{
    return parentItem_;
}
BLZ_MODEL_ITEM_TYPE model_item::item_type() const
{
    return item_type_;
}

const vlg::entity_desc *model_item::edesc() const
{
    return edesc_;
}

const vlg::member_desc *model_item::mdesc() const
{
    return mdesc_;
}

int model_item::row() const
{
    if(parentItem_) {
        return parentItem_->childItems_.indexOf(const_cast<model_item *>(this));
    }
    return 0;
}


//------------------------------------------------------------------------------
// blz_toolkit_blz_model
//------------------------------------------------------------------------------

void blz_toolkit_blz_model::OnModelUpdate_event()
{
    updateModelData(rootItem_);
}

blz_toolkit_blz_model::blz_toolkit_blz_model(vlg::entity_manager &bem,
                                             QObject *parent) :
    QAbstractItemModel(parent),
    bem_(bem),
    rootItem_(NULL)
{
    rootItem_ = new model_item(BLZ_MODEL_ITEM_TYPE_ROOT, NULL, NULL, NULL);
}

blz_toolkit_blz_model::~blz_toolkit_blz_model()
{
    delete rootItem_;
}

QModelIndex blz_toolkit_blz_model::index(int row, int column,
                                         const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    model_item *parentItem = NULL;

    if(!parent.isValid()) {
        parentItem = rootItem_;
    } else {
        parentItem = static_cast<model_item *>(parent.internalPointer());
    }
    model_item *childItem = parentItem->child(row);
    if(childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex blz_toolkit_blz_model::parent(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return QModelIndex();
    }

    model_item *childItem = static_cast<model_item *>(index.internalPointer());
    model_item *parentItem = childItem->parent();

    if(parentItem == rootItem_) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int blz_toolkit_blz_model::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return static_cast<model_item *>(parent.internalPointer())->columnCount();
    } else {
        return rootItem_->columnCount();
    }
}

int blz_toolkit_blz_model::rowCount(const QModelIndex &parent) const
{
    model_item *parentItem = NULL;
    if(parent.column() > 0) {
        return 0;
    }
    if(!parent.isValid()) {
        parentItem = rootItem_;
    } else {
        parentItem = static_cast<model_item *>(parent.internalPointer());
    }
    return parentItem->childCount();
}

QVariant blz_toolkit_blz_model::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    model_item *item = static_cast<model_item *>(index.internalPointer());

    return item->data(index.column());
}

QVariant blz_toolkit_blz_model::headerData(int section,
                                           Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }
    if(orientation == Qt::Horizontal) {
        return rootItem_->data(section);
    } else {
        return QVariant();
    }
    return QVariant();
}

void EnumMdscEDescFunc(const vlg::member_desc &desc, void *ud, bool &stop)
{
    model_item *bmi_parent = (model_item *)ud;
    model_item *mdsc_item = new model_item(BLZ_MODEL_ITEM_TYPE_MDESC,
                                           NULL,
                                           &desc,
                                           bmi_parent);
    bmi_parent->appendChild(mdsc_item);
}


void EnumBemEdescF(const vlg::entity_desc &entity_desc, void *ud, bool &stop)
{
    blz_toolkit_blz_model *btbm = (blz_toolkit_blz_model *)ud;
    model_item *edsc_item = new model_item(BLZ_MODEL_ITEM_TYPE_EDESC,
                                           &entity_desc,
                                           NULL,
                                           btbm->rootItem());
    btbm->rootItem()->appendChild(edsc_item);
    entity_desc.enum_member_descriptors(EnumMdscEDescFunc, edsc_item);
}

void blz_toolkit_blz_model::updateModelData(model_item *parent)
{
    bem_.enum_entity_descriptors(EnumBemEdescF, this);
}

vlg::entity_manager &blz_toolkit_blz_model::bem() const
{
    return bem_;
}

model_item *blz_toolkit_blz_model::rootItem() const
{
    return rootItem_;
}
