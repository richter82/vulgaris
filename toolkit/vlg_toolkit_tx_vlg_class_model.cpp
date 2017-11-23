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

#include "vlg_toolkit_tx_vlg_class_model.h"

//------------------------------------------------------------------------------
// GENERATE REP SECTION BGN
//------------------------------------------------------------------------------

ENM_GEN_TX_REP_REC_UD::ENM_GEN_TX_REP_REC_UD(vlg_toolkit_tx_vlg_class_model
                                             &mdl,
                                             const vlg::nentity_manager &bem,
                                             vlg::ascii_string *prfx,
                                             bool array_fld,
                                             unsigned int fld_idx) :
    mdl_(mdl),
    bem_(bem),
    prfx_(prfx),
    array_fld_(array_fld),
    fld_idx_(fld_idx)
{}

ENM_GEN_TX_REP_REC_UD::~ENM_GEN_TX_REP_REC_UD()
{}

void enum_generate_tx_model_rep(const vlg::hash_map &map, const void *key,
                                const void *ptr, void *ud)
{
    ENM_GEN_TX_REP_REC_UD *rud = (ENM_GEN_TX_REP_REC_UD *)ud;
    const vlg::member_desc *mmbrd = *(const vlg::member_desc **)ptr;
    vlg::ascii_string hdr_row_nm;
    hdr_row_nm.assign("");
    vlg::ascii_string idx_prfx;
    idx_prfx.assign(*(rud->prfx_));
    char idx_b[GEN_HDR_FIDX_BUFF] = {0};
    int sprintf_dbg = 0;

    if(rud->array_fld_) {
        sprintf_dbg = sprintf(idx_b, "%s%d", idx_prfx.length() ? "_" : "",
                              rud->fld_idx_);
        idx_prfx.append(idx_b);
    }

    if(mmbrd->get_field_vlg_type() == vlg::Type_ENTITY) {
        if(mmbrd->get_field_nentity_type() == vlg::NEntityType_NENUM) {
            //treat enum as number
            if(mmbrd->get_field_nmemb() > 1) {
                for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                    if(rud->prfx_->length()) {
                        hdr_row_nm.append(idx_prfx);
                        hdr_row_nm.append("_");
                    }
                    sprintf_dbg = sprintf(idx_b, "_%d", i);
                    hdr_row_nm.append(mmbrd->get_member_name());
                    hdr_row_nm.append(idx_b);
                    rud->mdl_.AppendHdrRow(hdr_row_nm.internal_buff());
                    rud->mdl_.IncrRownum();
                    hdr_row_nm.assign("");
                }
            } else {
                if(rud->prfx_->length()) {
                    hdr_row_nm.append(idx_prfx);
                    hdr_row_nm.append("_");
                }
                hdr_row_nm.append(mmbrd->get_member_name());
                rud->mdl_.AppendHdrRow(hdr_row_nm.internal_buff());
                rud->mdl_.IncrRownum();
            }
        } else {
            //class, struct is a recursive step.
            ENM_GEN_TX_REP_REC_UD rrud = *rud;
            vlg::ascii_string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd->get_member_name());
            rrud.prfx_ = &rprfx;
            const vlg::nentity_desc *edsc = NULL;
            if(!rud->bem_.get_nentity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const vlg::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
                if(mmbrd->get_field_nmemb() > 1) {
                    rrud.array_fld_ = true;
                    for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                        rrud.fld_idx_ = i;
                        nm_desc.enum_elements(enum_generate_tx_model_rep, &rrud);
                    }
                } else {
                    nm_desc.enum_elements(enum_generate_tx_model_rep, &rrud);
                }
            } else {
                //ERROR
            }
        }
    } else {
        //primitive type
        if(mmbrd->get_field_vlg_type() == vlg::Type_ASCII) {
            if(rud->prfx_->length()) {
                hdr_row_nm.append(idx_prfx);
                hdr_row_nm.append("_");
            }
            hdr_row_nm.append(mmbrd->get_member_name());
            rud->mdl_.AppendHdrRow(hdr_row_nm.internal_buff());
            rud->mdl_.IncrRownum();
        } else if(mmbrd->get_field_nmemb() > 1) {
            for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                if(rud->prfx_->length()) {
                    hdr_row_nm.append(idx_prfx);
                    hdr_row_nm.append("_");
                }
                sprintf_dbg = sprintf(idx_b, "_%d", i);
                hdr_row_nm.append(mmbrd->get_member_name());
                hdr_row_nm.append(idx_b);
                rud->mdl_.AppendHdrRow(hdr_row_nm.internal_buff());
                rud->mdl_.IncrRownum();
                hdr_row_nm.assign("");
            }
        } else {
            if(rud->prfx_->length()) {
                hdr_row_nm.append(idx_prfx);
                hdr_row_nm.append("_");
            }
            hdr_row_nm.append(mmbrd->get_member_name());
            rud->mdl_.AppendHdrRow(hdr_row_nm.internal_buff());
            rud->mdl_.IncrRownum();
        }
    }
}

//------------------------------------------------------------------------------
// GENERATE REP SECTION END
//------------------------------------------------------------------------------

vlg_toolkit_tx_vlg_class_model::vlg_toolkit_tx_vlg_class_model(
    const vlg::nentity_desc &edesc,
    const vlg::nentity_manager &bem,
    vlg::transaction &tx,
    QObject *parent) :
    edesc_(edesc),
    bem_(bem),
    tx_(tx),
    local_obj_(NULL),
    rownum_(0),
    QAbstractTableModel(parent)
{
    bem.new_nclass_instance(tx_.get_request_nclass_id(), &local_obj_);
    if(tx_.get_request_obj()) {
        local_obj_->set_from(tx_.get_request_obj());
    }
    GenerateModelRep();
}

vlg_toolkit_tx_vlg_class_model::~vlg_toolkit_tx_vlg_class_model()
{
    if(local_obj_) {
        delete local_obj_;
    }
}

void vlg_toolkit_tx_vlg_class_model::GenerateModelRep()
{
    GenerateHeader();
}

void vlg_toolkit_tx_vlg_class_model::GenerateHeader()
{
    vlg::ascii_string prfx;
    prfx.assign("");
    const vlg::hash_map &nm_desc = edesc_.get_opaque()->GetMap_NM_MMBRDSC();
    ENM_GEN_TX_REP_REC_UD gen_rep_rud(*this, bem_, &prfx, false, 0);
    nm_desc.enum_elements(enum_generate_tx_model_rep, &gen_rep_rud);
    int test = 0;
}

void vlg_toolkit_tx_vlg_class_model::IncrRownum()
{
    rownum_++;
}

vlg::nclass *vlg_toolkit_tx_vlg_class_model::local_obj() const
{
    return local_obj_;
}

int vlg_toolkit_tx_vlg_class_model::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int vlg_toolkit_tx_vlg_class_model::rowCount(const QModelIndex &parent) const
{
    return rownum_;
}

QVariant vlg_toolkit_tx_vlg_class_model::headerData(int section,
                                                    Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }
    if(orientation == Qt::Vertical) {
        return hdr_rowidx_rowname_[section];
    } else {
        if(section == 0) {
            return tr("Value");
        }
    }
    return QVariant();
}

QVariant vlg_toolkit_tx_vlg_class_model::data(const QModelIndex &index,
                                              int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(index.row() >= rownum_ || index.row() < 0) {
        return QVariant();
    }
    if(index.column() != 0) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        if(local_obj_) {
            const vlg::member_desc *obj_fld_mdesc = NULL;
            char *obj_fld_ptr = NULL;
            if((obj_fld_ptr = local_obj_->get_term_field_ref_by_plain_idx(index.row(), bem_,
                                                                          &obj_fld_mdesc))) {
                QString out;
                if((obj_fld_mdesc->get_field_vlg_type() == vlg::Type_ASCII) &&
                        obj_fld_mdesc->get_field_nmemb() > 1) {
                    out = QString::fromLatin1(obj_fld_ptr, obj_fld_mdesc->get_field_nmemb());
                } else {
                    FillQstring_FldValue(obj_fld_ptr, obj_fld_mdesc, out);
                }
                return out;
            } else {
                return tr("error");
            }
        } else {
            return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags vlg_toolkit_tx_vlg_class_model::flags(const QModelIndex &index)
const
{
    if(!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool vlg_toolkit_tx_vlg_class_model::setData(const QModelIndex &index,
                                             const QVariant &value, int role)
{
    if(!index.isValid()) {
        return false;
    }
    if(index.row() >= rownum_ || index.row() < 0) {
        return false;
    }
    if(index.column() != 0) {
        return false;
    }
    if(role == Qt::EditRole) {
        if(local_obj_) {
            const vlg::member_desc *obj_fld_mdesc = NULL;
            char *obj_fld_ptr = NULL;
            if((obj_fld_ptr = local_obj_->get_term_field_ref_by_plain_idx(index.row(), bem_,
                                                                          &obj_fld_mdesc))) {
                FillFldValue_Qstring(value, obj_fld_mdesc, obj_fld_ptr);
                emit(dataChanged(index, index));
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    return false;
}

bool vlg_toolkit_tx_vlg_class_model::insertRows(int position, int rows,
                                                const QModelIndex &index)
{
    return false;
}

bool vlg_toolkit_tx_vlg_class_model::removeRows(int position, int rows,
                                                const QModelIndex &index)
{
    return false;
}

void vlg_toolkit_tx_vlg_class_model::AppendHdrRow(const char *rowname)
{
    hdr_rowidx_rowname_[rownum_] = QString(rowname);
}
