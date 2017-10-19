/*
 *
 * (C) 2017 - giuseppe.baccini@gmail.com
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

#ifndef VLG_PROTO_H_
#define VLG_PROTO_H_
#include "vlg_globint.h"

/*****************************************
vlg PROTOCOL PUBLIC INTERFACE

VER: 000001
******************************************/

namespace vlg {

class tx_id;

/*****************************************
GLOB DEFS
******************************************/

#define VLG_BUFF_DEF_SZ 64
#define VLG_PROTO_VER   0x01      //up to 3F

/*****************************************
VLG_DUMP_MODE
******************************************/
enum VLG_DUMP_MODE {
    VLG_DUMP_MODE_Undef,
    VLG_DUMP_MODE_Binary,
    VLG_DUMP_MODE_Hex,
    VLG_DUMP_MODE_HumanReadable,
};

/*****************************************
BYTE ENCODING ENDIAN TYPE
******************************************/
enum VLG_ENDIAN_Type {
    VLG_ENDIAN_Undef,
    VLG_ENDIAN_Little,
    VLG_ENDIAN_Big
};

/*****************************************
SELECTOR EVTS
******************************************/
enum VLG_SELECTOR_Evt {
    VLG_SELECTOR_Evt_Undef,
    VLG_SELECTOR_Evt_Interrupt,
    VLG_SELECTOR_Evt_SendPacket,
    VLG_SELECTOR_Evt_ConnectRequest,
    VLG_SELECTOR_Evt_Disconnect,
    VLG_SELECTOR_Evt_ConnReqAccepted,
    VLG_SELECTOR_Evt_ConnReqRefused,
    VLG_SELECTOR_Evt_Inactivity,
};

/*****************************************
PEER TYPE
******************************************/
enum VLG_PEER_Type {
    VLG_PEER_Undef,
    VLG_PEER_Client,
    VLG_PEER_Server,
};

/*****************************************
NET-PACKET ENUMERATION
******************************************/
enum VLG_PKT_ID {
    VLG_PKT_NONE,
    VLG_PKT_TSTREQ_ID,     /*TEST REQUEST*/
    VLG_PKT_HRTBET_ID,     /*HEARTBEAT*/
    VLG_PKT_CONREQ_ID,     /*CONNECTION REQUEST*/
    VLG_PKT_CONRES_ID,     /*CONNECTION RESPONSE*/
    VLG_PKT_DSCOND_ID,     /*DISCONNECTED*/
    VLG_PKT_TXRQST_ID,     /*TRANSACTION REQUEST*/
    VLG_PKT_TXRESP_ID,     /*TRANSACTION RESPONSE*/
    VLG_PKT_SBSREQ_ID,     /*SUBSCRIPTION REQUEST*/
    VLG_PKT_SBSRES_ID,     /*SUBSCRIPTION RESPONSE*/
    VLG_PKT_SBSEVT_ID,     /*SUBSCRIPTION EVENT*/
    VLG_PKT_SBSACK_ID,     /*SUBSCRIPTION EVENT ACK*/
    VLG_PKT_SBSTOP_ID,     /*SUBSCRIPTION STOP REQUEST*/
    VLG_PKT_SBSSPR_ID,     /*SUBSCRIPTION STOP RESPONSE*/
};

enum VLG_DecdRes {
    VLG_DecdResUnkError = -1,
    VLG_DecdResOK
};

/*****************************************
GLOB STRUCTURES
******************************************/

/*****************************************
PKTHDR WORD RECORD
******************************************/
struct VLG_WRD_PKTHDR_REC {
    unsigned short          prover;
    unsigned short          hdrlen;
    VLG_PKT_ID              pkttyp;
};

/*****************************************
PKTLEN WORD RECORD
******************************************/
struct VLG_WRD_PKTLEN_REC {
    unsigned int            pktlen;
};

/*****************************************
TMSTMP WORD RECORD
******************************************/
typedef struct {
    unsigned int            tmstmp;
} VLG_WRD_TMSTMP_REC;

/*****************************************
CLIHBT WORD RECORD
******************************************/
struct VLG_WRD_CLIHBT_REC {
    unsigned short          hbtsec;
};

/*****************************************
SRVCRS WORD RECORD
******************************************/
typedef struct {
    ConnectionResult        conres;
    ConnectionResultReason  errcod;
    unsigned short          agrhbt;
} VLG_WRD_SRVCRS_REC;

/*****************************************
DISWRD WORD RECORD
******************************************/
struct VLG_WRD_DISWRD_REC {
    DisconnectionResultReason disres;
};

/*****************************************
TXREQW WORD RECORD
******************************************/
struct VLG_WRD_TXREQW_REC {
    TransactionRequestType  txtype;
    Action                  txactn;
    bool                    rsclrq;
};

/*****************************************
CLSENC WORD RECORD
******************************************/
struct VLG_WRD_CLSENC_REC {
    unsigned int        nclsid;
    Encode              enctyp;
};

/*****************************************
TXRESW WORD RECORD
******************************************/
struct VLG_WRD_TXRESW_REC {
    TransactionResult   txresl;
    ProtocolCode        blzcod;
    bool                rescls;
};

/*****************************************
TXPLID WORD RECORD
******************************************/
struct VLG_WRD_TXPLID_REC {
    unsigned int        txplid;
};

/*****************************************
TXSVID WORD RECORD
******************************************/
struct VLG_WRD_TXSVID_REC {
    unsigned int        txsvid;
};

/*****************************************
TXCNID WORD RECORD
******************************************/
struct VLG_WRD_TXCNID_REC {
    unsigned int        txcnid;
};

/*****************************************
TXPRID WORD RECORD
******************************************/
struct VLG_WRD_TXPRID_REC {
    unsigned int        txprid;
};

/*****************************************
CONNID WORD RECORD
******************************************/
struct VLG_WRD_CONNID_REC {
    unsigned int        connid;
};

/*****************************************
RQSTID WORD RECORD
******************************************/
struct VLG_WRD_RQSTID_REC {
    unsigned int        rqstid;
};

/*****************************************
SBREQW WORD RECORD
******************************************/
struct VLG_WRD_SBREQW_REC {
    SubscriptionType        sbstyp;
    SubscriptionMode        sbsmod;
    SubscriptionFlowType    flotyp;
    SubscriptionDownloadType    dwltyp;
};

/*****************************************
SBRESW WORD RECORD
******************************************/
struct VLG_WRD_SBRESW_REC {
    SubscriptionResponse    sbresl;
    ProtocolCode            blzcod;
};

/*****************************************
SBSRID WORD RECORD
******************************************/
struct VLG_WRD_SBSRID_REC {
    unsigned int            sbsrid;
};

/*****************************************
SEVTID WORD RECORD
******************************************/
struct VLG_WRD_SEVTID_REC {
    unsigned int            sevtid;
};

/*****************************************
SEVTID WORD RECORD
******************************************/
struct VLG_WRD_SEVTTP_REC {
    SubscriptionEventType   sevttp;
    Action                  sbeact;
    ProtocolCode            blzcod;
};

/*****************************************
AN UNION TO ADDRESS FIELDS OF ALL WORDS TYPES
******************************************/
union VLG_WORD {
    VLG_WRD_PKTHDR_REC      pkthdr;
    VLG_WRD_PKTLEN_REC      pktlen;
    VLG_WRD_TMSTMP_REC      tmstmp;
    VLG_WRD_CLIHBT_REC      clihbt;
    VLG_WRD_SRVCRS_REC      srvcrs;
    VLG_WRD_DISWRD_REC      diswrd;
    VLG_WRD_TXREQW_REC      txreqw;
    VLG_WRD_CLSENC_REC      clsenc;
    VLG_WRD_TXRESW_REC      txresw;
    VLG_WRD_TXPLID_REC      txplid;
    VLG_WRD_TXSVID_REC      txsvid;
    VLG_WRD_TXCNID_REC      txcnid;
    VLG_WRD_TXPRID_REC      txprid;
    VLG_WRD_CONNID_REC      connid;
    VLG_WRD_RQSTID_REC      rqstid;
    VLG_WRD_SBREQW_REC      sbreqw;
    VLG_WRD_SBRESW_REC      sbresw;
    VLG_WRD_SBSRID_REC      sbsrid;
    VLG_WRD_SEVTID_REC      sevtid;
    VLG_WRD_SEVTTP_REC      sevttp;
};

/*****************************************
GLOB STRUCTURES
******************************************/

/*****************************************
GENERIC HEADER STRUCTURE
******************************************/
typedef struct vlg_hdr_rec {
    vlg_hdr_rec();
    unsigned int            hdr_bytelen;
    unsigned int            bdy_bytelen;
    VLG_WRD_PKTHDR_REC      phdr;       //fixed row 0 pkthdr
    VLG_WORD                row_1;      //dyna  row 1
    VLG_WORD                row_2;      //dyna  row 2
    VLG_WORD                row_3;      //dyna  row 3
    VLG_WORD                row_4;      //dyna  row 4
    VLG_WORD                row_5;      //dyna  row 5
    VLG_WORD                row_6;      //dyna  row 6
    VLG_WORD                row_7;      //dyna  row 7
    VLG_WORD                row_8;      //dyna  row 8
} *vlg_hdr_ptr;

#define DMP_OUT_BUF_LEN 1024

int dump_vlg_hdr_rec(const vlg_hdr_ptr hdr,
                     char *out);

const char *dump_raw_pkt(bool all, const unsigned char *pkt,
                         size_t pkt_sz,
                         vlg::ascii_string &out);

/*****************************************
 GLOB PROTO UTILS
******************************************/

typedef unsigned int time_stamp;

/*****************************************
 PKT BUILD FUN
******************************************/

//TEST REQUEST
void build_PKT_TSTREQ(time_stamp                tstamp,
                      unsigned int              connid,
                      vlg::grow_byte_buffer   *obb);

//HEARTBEAT
void build_PKT_HRTBET(time_stamp                tstamp,
                      unsigned int              connid,
                      vlg::grow_byte_buffer   *obb);

//CONNECTION REQUEST
void build_PKT_CONREQ(unsigned short            clihbt,
                      vlg::grow_byte_buffer   *obb);

//CONNECTION RESPONSE
void build_PKT_CONRES(ConnectionResult          conres,
                      ConnectionResultReason    errcod,
                      unsigned short            agrhbt,
                      unsigned int              connid,
                      vlg::grow_byte_buffer   *obb);

//DISCONNECTED
void build_PKT_DSCOND(DisconnectionResultReason disres,
                      unsigned int              connid,
                      vlg::grow_byte_buffer   *obb);

//TRANSACTION REQUEST
void build_PKT_TXRQST(TransactionRequestType    txtype,
                      Action                    txactn,
                      tx_id                     *txid,
                      bool                      rsclrq,
                      Encode                    enctyp,
                      unsigned int              nclsid,
                      unsigned int              connid,
                      vlg::grow_byte_buffer   *obb);

//TRANSACTION RESPONSE
void build_PKT_TXRESP(TransactionResult         txresl,
                      ProtocolCode              blzcod,
                      tx_id                     *txid,
                      bool                      rescls,
                      Encode                    enctyp,
                      unsigned int              nclsid,
                      vlg::grow_byte_buffer   *obb);

//SUBSCRIPTION REQUEST
void build_PKT_SBSREQ(SubscriptionType          sbstyp,
                      SubscriptionMode          sbsmod,
                      SubscriptionFlowType      flotyp,
                      SubscriptionDownloadType  dwltyp,
                      Encode                    enctyp,
                      unsigned int              nclsid,
                      unsigned int              connid,
                      unsigned int              rqstid,
                      unsigned int              tmstp0,
                      unsigned int              tmstp1,
                      vlg::grow_byte_buffer   *obb);

//SUBSCRIPTION RESPONSE
void build_PKT_SBSRES(SubscriptionResponse      sbresl,
                      ProtocolCode              blzcod,
                      unsigned int              rqstid,
                      unsigned int              sbsrid,
                      vlg::grow_byte_buffer   *obb);

//SUBSCRIPTION EVENT
void build_PKT_SBSEVT(unsigned int              sbsrid,
                      SubscriptionEventType     sevttp,
                      Action                    sbeact,
                      ProtocolCode              blzcod,
                      unsigned int              sevtid,
                      unsigned int              tmstp0,
                      unsigned int              tmstp1,
                      vlg::grow_byte_buffer   *obb);

//SUBSCRIPTION EVENT ACK
void build_PKT_SBSACK(unsigned int              sbsrid,
                      unsigned int              sevtid,
                      vlg::grow_byte_buffer   *obb);

//SUBSCRIPTION STOP REQUEST
void build_PKT_SBSTOP(unsigned int              sbsrid,
                      vlg::grow_byte_buffer   *obb);

//SUBSCRIPTION STOP RESPONSE
void build_PKT_SBSSPR(SubscriptionResponse      sbresl,
                      ProtocolCode              blzcod,
                      unsigned int              sbsrid,
                      vlg::grow_byte_buffer   *obb);

}

#endif
