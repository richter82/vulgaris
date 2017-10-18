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

#ifndef BLZ_TOOLKIT_TX_BLZ_CLASS_MODEL_H
#define BLZ_TOOLKIT_TX_BLZ_CLASS_MODEL_H

#include "blz_toolkit_glob.h"

class blz_toolkit_tx_blz_class_model;

//------------------------------------------------------------------------------
// GENERATE REP SECTION BGN
//------------------------------------------------------------------------------

struct ENM_GEN_TX_REP_REC_UD {
    ENM_GEN_TX_REP_REC_UD(blz_toolkit_tx_blz_class_model &mdl,
                          const vlg::entity_manager &bem,
                          vlg::ascii_string *prfx,
                          bool array_fld,
                          unsigned int fld_idx);

    ~ENM_GEN_TX_REP_REC_UD();

    blz_toolkit_tx_blz_class_model &mdl_;
    const vlg::entity_manager &bem_;
    vlg::ascii_string *prfx_;

    bool array_fld_;
    unsigned int fld_idx_;   //used to render column name when the field is an array
};

//------------------------------------------------------------------------------
// GENERATE REP SECTION END
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// blz_toolkit_tx_blz_class_model
//------------------------------------------------------------------------------
class blz_toolkit_tx_blz_class_model : public QAbstractTableModel {
        Q_OBJECT

    public:
        blz_toolkit_tx_blz_class_model(const vlg::entity_desc &edesc,
                                       const vlg::entity_manager &bem,
                                       vlg::transaction_int &tx,
                                       QObject *parent = 0);
        ~blz_toolkit_tx_blz_class_model();

        vlg::nclass *local_obj() const;

        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;

        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;
        QVariant data(const QModelIndex &index, int role) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant &value,
                     int role=Qt::EditRole);
        bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
        bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

        void GenerateModelRep();
        void GenerateHeader();
        void AppendHdrRow(const char *rowname);
        void IncrRownum();

        /*****
         REP
         ****/


    private:
        const vlg::entity_desc       &edesc_;
        const vlg::entity_manager    &bem_;
        vlg::transaction_int         &tx_;
        vlg::nclass             *local_obj_;

        //METADATA REP
        int rownum_;
        QHash<int, QString>     hdr_rowidx_rowname_;  //rowidx --> row_hdr_name
};

#endif // BLZ_TOOLKIT_TX_BLZ_CLASS_MODEL_H
