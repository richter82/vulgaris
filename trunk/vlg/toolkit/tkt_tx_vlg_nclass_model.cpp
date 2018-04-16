/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "tkt_tx_vlg_nclass_model.h"

//------------------------------------------------------------------------------
// GENERATE REP SECTION BGN
//------------------------------------------------------------------------------

ENM_GEN_TX_REP_REC_UD::ENM_GEN_TX_REP_REC_UD(vlg_toolkit_tx_vlg_class_model &mdl,
                                             const vlg::nentity_manager &bem,
                                             std::string *prfx,
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

bool enum_generate_tx_model_rep(const vlg::member_desc &mmbrd,
                                void *ud)
{
    ENM_GEN_TX_REP_REC_UD *rud = (ENM_GEN_TX_REP_REC_UD *)ud;
    std::string hdr_row_nm;
    hdr_row_nm.assign("");
    std::string idx_prfx;
    idx_prfx.assign(*(rud->prfx_));
    char idx_b[GEN_HDR_FIDX_BUFF] = {0};
    int sprintf_dbg = 0;

    if(rud->array_fld_) {
        sprintf_dbg = sprintf(idx_b, "%s%d", idx_prfx.length() ? "_" : "", rud->fld_idx_);
        idx_prfx.append(idx_b);
    }

    if(mmbrd.get_field_vlg_type() == vlg::Type_ENTITY) {
        if(mmbrd.get_field_nentity_type() == vlg::NEntityType_NENUM) {
            //treat enum as number
            if(mmbrd.get_field_nmemb() > 1) {
                for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                    if(rud->prfx_->length()) {
                        hdr_row_nm.append(idx_prfx);
                        hdr_row_nm.append("_");
                    }
                    sprintf_dbg = sprintf(idx_b, "_%d", i);
                    hdr_row_nm.append(mmbrd.get_member_name());
                    hdr_row_nm.append(idx_b);
                    rud->mdl_.AppendHdrRow(hdr_row_nm.c_str());
                    rud->mdl_.IncrRownum();
                    hdr_row_nm.assign("");
                }
            } else {
                if(rud->prfx_->length()) {
                    hdr_row_nm.append(idx_prfx);
                    hdr_row_nm.append("_");
                }
                hdr_row_nm.append(mmbrd.get_member_name());
                rud->mdl_.AppendHdrRow(hdr_row_nm.c_str());
                rud->mdl_.IncrRownum();
            }
        } else {
            //class, struct is a recursive step.
            ENM_GEN_TX_REP_REC_UD rrud = *rud;
            std::string rprfx;
            rprfx.assign(idx_prfx);
            if(rprfx.length()) {
                rprfx.append("_");
            }
            rprfx.append(mmbrd.get_member_name());
            rrud.prfx_ = &rprfx;
            const vlg::nentity_desc *edsc = rud->bem_.get_nentity_descriptor(mmbrd.get_field_user_type());
            if(edsc) {
                if(mmbrd.get_field_nmemb() > 1) {
                    rrud.array_fld_ = true;
                    for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                        rrud.fld_idx_ = i;
                        edsc->enum_member_descriptors(enum_generate_tx_model_rep, &rrud);
                    }
                } else {
                    edsc->enum_member_descriptors(enum_generate_tx_model_rep, &rrud);
                }
            }
        }
    } else {
        //primitive type
        if(mmbrd.get_field_vlg_type() == vlg::Type_ASCII || mmbrd.get_field_vlg_type() == vlg::Type_BYTE) {
            if(rud->prfx_->length()) {
                hdr_row_nm.append(idx_prfx);
                hdr_row_nm.append("_");
            }
            hdr_row_nm.append(mmbrd.get_member_name());
            rud->mdl_.AppendHdrRow(hdr_row_nm.c_str());
            rud->mdl_.IncrRownum();
        } else if(mmbrd.get_field_nmemb() > 1) {
            for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                if(rud->prfx_->length()) {
                    hdr_row_nm.append(idx_prfx);
                    hdr_row_nm.append("_");
                }
                sprintf_dbg = sprintf(idx_b, "_%d", i);
                hdr_row_nm.append(mmbrd.get_member_name());
                hdr_row_nm.append(idx_b);
                rud->mdl_.AppendHdrRow(hdr_row_nm.c_str());
                rud->mdl_.IncrRownum();
                hdr_row_nm.assign("");
            }
        } else {
            if(rud->prfx_->length()) {
                hdr_row_nm.append(idx_prfx);
                hdr_row_nm.append("_");
            }
            hdr_row_nm.append(mmbrd.get_member_name());
            rud->mdl_.AppendHdrRow(hdr_row_nm.c_str());
            rud->mdl_.IncrRownum();
        }
    }
    return true;
}

//------------------------------------------------------------------------------
// GENERATE REP SECTION END
//------------------------------------------------------------------------------

vlg_toolkit_tx_vlg_class_model::vlg_toolkit_tx_vlg_class_model(const vlg::nentity_desc &edesc,
                                                               const vlg::nentity_manager &bem,
                                                               QObject *parent) :
    edesc_(edesc),
    bem_(bem),
    rownum_(0),
    QAbstractTableModel(parent)
{
    bem.new_nclass_instance(edesc.get_nclass_id(), local_obj_);
    GenerateModelRep();
}

vlg_toolkit_tx_vlg_class_model::~vlg_toolkit_tx_vlg_class_model()
{}

void vlg_toolkit_tx_vlg_class_model::GenerateModelRep()
{
    GenerateHeader();
}

void vlg_toolkit_tx_vlg_class_model::GenerateHeader()
{
    std::string prfx;
    prfx.assign("");
    ENM_GEN_TX_REP_REC_UD gen_rep_rud(*this, bem_, &prfx, false, 0);
    edesc_.enum_member_descriptors(enum_generate_tx_model_rep, &gen_rep_rud);
}

void vlg_toolkit_tx_vlg_class_model::IncrRownum()
{
    rownum_++;
}

vlg::nclass *vlg_toolkit_tx_vlg_class_model::local_obj() const
{
    return local_obj_.get();
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
                                                    Qt::Orientation orientation,
                                                    int role) const
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
            if((obj_fld_ptr = local_obj_->get_field_address_by_column_number(index.row(),
                                                                             bem_,
                                                                             &obj_fld_mdesc))) {
                QString out;
                if((obj_fld_mdesc->get_field_vlg_type() == vlg::Type_ASCII) && obj_fld_mdesc->get_field_nmemb() > 1) {
                    out = QString::fromLatin1(obj_fld_ptr, (int)obj_fld_mdesc->get_field_nmemb());
                } else if((obj_fld_mdesc->get_field_vlg_type() == vlg::Type_BYTE) && obj_fld_mdesc->get_field_nmemb() > 1) {
                    out = QString::fromUtf8(obj_fld_ptr, (int)obj_fld_mdesc->get_field_nmemb());
                } else {
                    FillQstring_FldValue(obj_fld_ptr, *obj_fld_mdesc, out);
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

Qt::ItemFlags vlg_toolkit_tx_vlg_class_model::flags(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool vlg_toolkit_tx_vlg_class_model::setData(const QModelIndex &index,
                                             const QVariant &value,
                                             int role)
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
            if((obj_fld_ptr = local_obj_->get_field_address_by_column_number(index.row(),
                                                                             bem_,
                                                                             &obj_fld_mdesc))) {
                FillFldValue_Qstring(value, *obj_fld_mdesc, obj_fld_ptr);
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

bool vlg_toolkit_tx_vlg_class_model::insertRows(int position,
                                                int rows,
                                                const QModelIndex &index)
{
    return false;
}

bool vlg_toolkit_tx_vlg_class_model::removeRows(int position,
                                                int rows,
                                                const QModelIndex &index)
{
    return false;
}

void vlg_toolkit_tx_vlg_class_model::AppendHdrRow(const char *rowname)
{
    hdr_rowidx_rowname_[rownum_] = QString(rowname);
}
