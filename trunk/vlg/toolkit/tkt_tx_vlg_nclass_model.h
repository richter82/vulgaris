/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "tkt_glob.h"

class vlg_toolkit_tx_vlg_class_model;

//------------------------------------------------------------------------------
// GENERATE REP SECTION BGN
//------------------------------------------------------------------------------
struct ENM_GEN_TX_REP_REC_UD {
    ENM_GEN_TX_REP_REC_UD(vlg_toolkit_tx_vlg_class_model &mdl,
                          const vlg::nentity_manager &bem,
                          std::string *prfx,
                          bool array_fld,
                          unsigned int fld_idx);

    ~ENM_GEN_TX_REP_REC_UD();

    vlg_toolkit_tx_vlg_class_model &mdl_;
    const vlg::nentity_manager &bem_;
    std::string *prfx_;

    bool array_fld_;
    unsigned int fld_idx_;   //used to render column name when the field is an array
};

//------------------------------------------------------------------------------
// vlg_toolkit_tx_vlg_class_model
//------------------------------------------------------------------------------
class vlg_toolkit_tx_vlg_class_model : public QAbstractTableModel {
        Q_OBJECT

    public:
        vlg_toolkit_tx_vlg_class_model(const vlg::nentity_desc &edesc,
                                       const vlg::nentity_manager &bem,
                                       QObject *parent = 0);

        ~vlg_toolkit_tx_vlg_class_model();

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

    private:
        const vlg::nentity_desc &edesc_;
        const vlg::nentity_manager &bem_;
        std::unique_ptr<vlg::nclass> local_obj_;

        //METADATA REP
        int rownum_;
        QHash<int, QString> hdr_rowidx_rowname_;  //rowidx --> row_hdr_name
};
