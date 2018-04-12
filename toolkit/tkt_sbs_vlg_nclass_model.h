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

#define VLG_TKT_INT_CELL_COLOR_RST_MSEC 500

class vlg_toolkit_sbs_vlg_class_model;

//-----------------------------------------------------------------------------
// subscription support structs
//-----------------------------------------------------------------------------
enum BlazeSbsEntryAct {
    BlazeSbsEntryActUndef,
    BlazeSbsEntryActAdd,
    BlazeSbsEntryActRwt,
    BlazeSbsEntryActDel
};


class sbs_col_data_timer : public QTimer {
        Q_OBJECT
    public:
        sbs_col_data_timer(VLG_SBS_COL_DATA_ENTRY &parent);

    public slots:
        void OnTimeout();

    signals:
        void SignalCellResetColor(VLG_SBS_COL_DATA_ENTRY &scde);

    private:
        VLG_SBS_COL_DATA_ENTRY &parent_;
};

struct VLG_SBS_COL_DATA_ENTRY {
    VLG_SBS_COL_DATA_ENTRY();
    ~VLG_SBS_COL_DATA_ENTRY();
    BlazeSbsEntryAct col_act_;
    sbs_col_data_timer *color_timer_;
    int row_idx_;
    int col_idx_;
};

struct VLG_SBS_DATA_ENTRY {
    VLG_SBS_DATA_ENTRY(std::shared_ptr<vlg::nclass> &entry,
                       int row_idx,
                       int col_num);

    std::shared_ptr<vlg::nclass> entry_;
    QVector<VLG_SBS_COL_DATA_ENTRY> fld_act_;
};

//------------------------------------------------------------------------------
// VLG_CLASS_ROW_IDX_PAIR  -- HELPER STRUCT
//------------------------------------------------------------------------------
struct VLG_CLASS_ROW_IDX_PAIR {
    VLG_CLASS_ROW_IDX_PAIR(int rowidx = 0, vlg::nclass *obj = nullptr);
    VLG_CLASS_ROW_IDX_PAIR &operator = (const VLG_CLASS_ROW_IDX_PAIR &src);

    int rowidx_;
    std::shared_ptr<vlg::nclass> obj_;
};

//------------------------------------------------------------------------------
// GENERATE REP SECTION BGN
//------------------------------------------------------------------------------

struct ENM_GEN_SBS_REP_REC_UD {
    ENM_GEN_SBS_REP_REC_UD(vlg_toolkit_sbs_vlg_class_model &mdl,
                           const vlg::nentity_manager &bem,
                           std::string *prfx,
                           bool array_fld,
                           unsigned int fld_idx);

    ~ENM_GEN_SBS_REP_REC_UD();

    vlg_toolkit_sbs_vlg_class_model &mdl_;
    const vlg::nentity_manager &bem_;
    std::string *prfx_;

    bool array_fld_;
    unsigned int fld_idx_;   //used to render column name when the field is an array
};

//------------------------------------------------------------------------------
// GENERATE REP SECTION END
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// UPDATE NCLASS ROW SECTION BGN
//------------------------------------------------------------------------------

struct ENM_UPD_CLS_ROW_REC_UD {
    ENM_UPD_CLS_ROW_REC_UD(vlg_toolkit_sbs_vlg_class_model &mdl,
                           const vlg::nentity_manager &bem,
                           const char *obj_ptr,
                           const char *obj_ptr_prev,
                           int rowidx,
                           bool array_fld,
                           unsigned int fld_idx);

    ~ENM_UPD_CLS_ROW_REC_UD();

    vlg_toolkit_sbs_vlg_class_model &mdl_;
    const vlg::nentity_manager &bem_;
    const char *obj_ptr_;
    const char *obj_ptr_prev_;
    int rowidx_; //this is const during enumeration.
    int curcolidx_; //current col index to render.
    bool array_fld_;
    unsigned int fld_idx_; //used to render column name when the field is an array
};

//------------------------------------------------------------------------------
// vlg_toolkit_sbs_vlg_class_model
//------------------------------------------------------------------------------
class vlg_toolkit_sbs_vlg_class_model : public QAbstractTableModel {
        Q_OBJECT

    public:
        vlg_toolkit_sbs_vlg_class_model(const vlg::nentity_desc &edesc,
                                        const vlg::nentity_manager &bem,
                                        QObject *parent = 0);
        ~vlg_toolkit_sbs_vlg_class_model();


        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        QModelIndex index(int row, int column, const QModelIndex &parent) const;

        QVariant headerData(int section,
                            Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;

        QVariant data(const QModelIndex &index, int role) const;

        bool setData(const QModelIndex &index,
                     const QVariant &value,
                     int role=Qt::EditRole);

        bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
        bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

        QHash<int, QString> &hdr_colidx_colname();

    public:
        void AppendHdrColumn(const char *colname);
        void IncrColnum();

    public:
        void offerEntry(std::shared_ptr<vlg::subscription_event> &);

    private:
        void GenerateModelRep();
        void GenerateHeader();
        void updateEntry(int rowidx,
                         vlg::nclass *entry,
                         vlg::nclass *prev_entry);

    public slots:
        void OnCellResetColor(VLG_SBS_COL_DATA_ENTRY &scde);

        /*****
         REP
         ****/
    private:
        const vlg::nentity_desc       &edesc_;
        const vlg::nentity_manager    &bem_;

        //METADATA REP
        int colnum_;
        QHash<int, QString> hdr_colidx_colname_;  //colidx --> col_hdr_name

        //DATA REP
        //helper repo for collected instances: ClassPrimKey  --> [rowid - instance]
        QHash<QString, VLG_CLASS_ROW_IDX_PAIR> data_hlpr_;

        //used by view - index in the list is the row in the view
        QList<VLG_SBS_DATA_ENTRY> data_;
};
