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

#ifndef BLZ_PROTO_H_
#define BLZ_PROTO_H_

/*****************************************
BLAZE PROTOCOL PUBLIC INTERFACE

VER: 000001
******************************************/

namespace blaze {

class tx_id;

/*****************************************
GLOB DEFS
******************************************/

#define BLZ_BUFF_DEF_SZ 64
#define BLZ_PROTO_VER   0x01      //up to 3F

/*****************************************
BLZ_DUMP_MODE
******************************************/
enum BLZ_DUMP_MODE {
    BLZ_DUMP_MODE_Undef,
    BLZ_DUMP_MODE_Binary,
    BLZ_DUMP_MODE_Hex,
    BLZ_DUMP_MODE_HumanReadable,
};

/*****************************************
BYTE ENCODING ENDIAN TYPE
******************************************/
enum BLZ_ENDIAN_Type {
    BLZ_ENDIAN_Undef,
    BLZ_ENDIAN_Little,
    BLZ_ENDIAN_Big
};

/*****************************************
SELECTOR EVTS
******************************************/
enum BLZ_SELECTOR_Evt {
    BLZ_SELECTOR_Evt_Undef,
    BLZ_SELECTOR_Evt_Interrupt,
    BLZ_SELECTOR_Evt_SendPacket,
    BLZ_SELECTOR_Evt_ConnectRequest,
    BLZ_SELECTOR_Evt_Disconnect,
    BLZ_SELECTOR_Evt_ConnReqAccepted,
    BLZ_SELECTOR_Evt_ConnReqRefused,
    BLZ_SELECTOR_Evt_Inactivity,
};

/*****************************************
PEER TYPE
******************************************/
enum BLZ_PEER_Type {
    BLZ_PEER_Undef,
    BLZ_PEER_Client,
    BLZ_PEER_Server,
};

/*****************************************
NET-PACKET ENUMERATION
******************************************/
enum BLZ_PKT_ID {
    BLZ_PKT_NONE,
    BLZ_PKT_TSTREQ_ID,     /*TEST REQUEST*/
    BLZ_PKT_HRTBET_ID,     /*HEARTBEAT*/
    BLZ_PKT_CONREQ_ID,     /*CONNECTION REQUEST*/
    BLZ_PKT_CONRES_ID,     /*CONNECTION RESPONSE*/
    BLZ_PKT_DSCOND_ID,     /*DISCONNECTED*/
    BLZ_PKT_TXRQST_ID,     /*TRANSACTION REQUEST*/
    BLZ_PKT_TXRESP_ID,     /*TRANSACTION RESPONSE*/
    BLZ_PKT_SBSREQ_ID,     /*SUBSCRIPTION REQUEST*/
    BLZ_PKT_SBSRES_ID,     /*SUBSCRIPTION RESPONSE*/
    BLZ_PKT_SBSEVT_ID,     /*SUBSCRIPTION EVENT*/
    BLZ_PKT_SBSACK_ID,     /*SUBSCRIPTION EVENT ACK*/
    BLZ_PKT_SBSTOP_ID,     /*SUBSCRIPTION STOP REQUEST*/
    BLZ_PKT_SBSSPR_ID,     /*SUBSCRIPTION STOP RESPONSE*/
};

enum BLZ_DecdRes {
    BLZ_DecdResUnkError = -1,
    BLZ_DecdResOK
};

/*****************************************
GLOB STRUCTURES
******************************************/

/*****************************************
PKTHDR WORD RECORD
******************************************/
struct BLZ_WRD_PKTHDR_REC {
    unsigned short          prover;
    unsigned short          hdrlen;
    BLZ_PKT_ID              pkttyp;
};

/*****************************************
PKTLEN WORD RECORD
******************************************/
struct BLZ_WRD_PKTLEN_REC {
    unsigned int            pktlen;
};

/*****************************************
TMSTMP WORD RECORD
******************************************/
typedef struct {
    unsigned int            tmstmp;
} BLZ_WRD_TMSTMP_REC;

/*****************************************
CLIHBT WORD RECORD
******************************************/
struct BLZ_WRD_CLIHBT_REC {
    unsigned short          hbtsec;
};

/*****************************************
SRVCRS WORD RECORD
******************************************/
typedef struct {
    ConnectionResult        conres;
    ConnectionResultReason  errcod;
    unsigned short          agrhbt;
} BLZ_WRD_SRVCRS_REC;

/*****************************************
DISWRD WORD RECORD
******************************************/
struct BLZ_WRD_DISWRD_REC {
    DisconnectionResultReason disres;
};

/*****************************************
TXREQW WORD RECORD
******************************************/
struct BLZ_WRD_TXREQW_REC {
    TransactionRequestType  txtype;
    Action                  txactn;
    bool                    rsclrq;
};

/*****************************************
CLSENC WORD RECORD
******************************************/
struct BLZ_WRD_CLSENC_REC {
    unsigned int        nclsid;
    Encode              enctyp;
};

/*****************************************
TXRESW WORD RECORD
******************************************/
struct BLZ_WRD_TXRESW_REC {
    TransactionResult   txresl;
    ProtocolCode        blzcod;
    bool                rescls;
};

/*****************************************
TXPLID WORD RECORD
******************************************/
struct BLZ_WRD_TXPLID_REC {
    unsigned int        txplid;
};

/*****************************************
TXSVID WORD RECORD
******************************************/
struct BLZ_WRD_TXSVID_REC {
    unsigned int        txsvid;
};

/*****************************************
TXCNID WORD RECORD
******************************************/
struct BLZ_WRD_TXCNID_REC {
    unsigned int        txcnid;
};

/*****************************************
TXPRID WORD RECORD
******************************************/
struct BLZ_WRD_TXPRID_REC {
    unsigned int        txprid;
};

/*****************************************
CONNID WORD RECORD
******************************************/
struct BLZ_WRD_CONNID_REC {
    unsigned int        connid;
};

/*****************************************
RQSTID WORD RECORD
******************************************/
struct BLZ_WRD_RQSTID_REC {
    unsigned int        rqstid;
};

/*****************************************
SBREQW WORD RECORD
******************************************/
struct BLZ_WRD_SBREQW_REC {
    SubscriptionType        sbstyp;
    SubscriptionMode        sbsmod;
    SubscriptionFlowType    flotyp;
    SubscriptionDownloadType    dwltyp;
};

/*****************************************
SBRESW WORD RECORD
******************************************/
struct BLZ_WRD_SBRESW_REC {
    SubscriptionResponse    sbresl;
    ProtocolCode            blzcod;
};

/*****************************************
SBSRID WORD RECORD
******************************************/
struct BLZ_WRD_SBSRID_REC {
    unsigned int            sbsrid;
};

/*****************************************
SEVTID WORD RECORD
******************************************/
struct BLZ_WRD_SEVTID_REC {
    unsigned int            sevtid;
};

/*****************************************
SEVTID WORD RECORD
******************************************/
struct BLZ_WRD_SEVTTP_REC {
    SubscriptionEventType   sevttp;
    Action                  sbeact;
    ProtocolCode            blzcod;
};

/*****************************************
AN UNION TO ADDRESS FIELDS OF ALL WORDS TYPES
******************************************/
union BLZ_WORD {
    BLZ_WRD_PKTHDR_REC      pkthdr;
    BLZ_WRD_PKTLEN_REC      pktlen;
    BLZ_WRD_TMSTMP_REC      tmstmp;
    BLZ_WRD_CLIHBT_REC      clihbt;
    BLZ_WRD_SRVCRS_REC      srvcrs;
    BLZ_WRD_DISWRD_REC      diswrd;
    BLZ_WRD_TXREQW_REC      txreqw;
    BLZ_WRD_CLSENC_REC      clsenc;
    BLZ_WRD_TXRESW_REC      txresw;
    BLZ_WRD_TXPLID_REC      txplid;
    BLZ_WRD_TXSVID_REC      txsvid;
    BLZ_WRD_TXCNID_REC      txcnid;
    BLZ_WRD_TXPRID_REC      txprid;
    BLZ_WRD_CONNID_REC      connid;
    BLZ_WRD_RQSTID_REC      rqstid;
    BLZ_WRD_SBREQW_REC      sbreqw;
    BLZ_WRD_SBRESW_REC      sbresw;
    BLZ_WRD_SBSRID_REC      sbsrid;
    BLZ_WRD_SEVTID_REC      sevtid;
    BLZ_WRD_SEVTTP_REC      sevttp;
};

/*****************************************
GLOB STRUCTURES
******************************************/

/*****************************************
GENERIC HEADER STRUCTURE
******************************************/
typedef struct blz_hdr_rec {
    blz_hdr_rec();
    unsigned int            hdr_bytelen;
    unsigned int            bdy_bytelen;
    BLZ_WRD_PKTHDR_REC      phdr;       //fixed row 0 pkthdr
    BLZ_WORD                row_1;      //dyna  row 1
    BLZ_WORD                row_2;      //dyna  row 2
    BLZ_WORD                row_3;      //dyna  row 3
    BLZ_WORD                row_4;      //dyna  row 4
    BLZ_WORD                row_5;      //dyna  row 5
    BLZ_WORD                row_6;      //dyna  row 6
    BLZ_WORD                row_7;      //dyna  row 7
    BLZ_WORD                row_8;      //dyna  row 8
} *blz_hdr_ptr;

#define DMP_OUT_BUF_LEN 1024

int dump_blz_hdr_rec(const blz_hdr_ptr hdr,
                     char *out);

const char *dump_raw_pkt(bool all, const unsigned char *pkt,
                         size_t pkt_sz,
                         blaze::ascii_string &out);

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
                      blaze::grow_byte_buffer   *obb);

//HEARTBEAT
void build_PKT_HRTBET(time_stamp                tstamp,
                      unsigned int              connid,
                      blaze::grow_byte_buffer   *obb);

//CONNECTION REQUEST
void build_PKT_CONREQ(unsigned short            clihbt,
                      blaze::grow_byte_buffer   *obb);

//CONNECTION RESPONSE
void build_PKT_CONRES(ConnectionResult          conres,
                      ConnectionResultReason    errcod,
                      unsigned short            agrhbt,
                      unsigned int              connid,
                      blaze::grow_byte_buffer   *obb);

//DISCONNECTED
void build_PKT_DSCOND(DisconnectionResultReason disres,
                      unsigned int              connid,
                      blaze::grow_byte_buffer   *obb);

//TRANSACTION REQUEST
void build_PKT_TXRQST(TransactionRequestType    txtype,
                      Action                    txactn,
                      tx_id                     *txid,
                      bool                      rsclrq,
                      Encode                    enctyp,
                      unsigned int              nclsid,
                      unsigned int              connid,
                      blaze::grow_byte_buffer   *obb);

//TRANSACTION RESPONSE
void build_PKT_TXRESP(TransactionResult         txresl,
                      ProtocolCode              blzcod,
                      tx_id                     *txid,
                      bool                      rescls,
                      Encode                    enctyp,
                      unsigned int              nclsid,
                      blaze::grow_byte_buffer   *obb);

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
                      blaze::grow_byte_buffer   *obb);

//SUBSCRIPTION RESPONSE
void build_PKT_SBSRES(SubscriptionResponse      sbresl,
                      ProtocolCode              blzcod,
                      unsigned int              rqstid,
                      unsigned int              sbsrid,
                      blaze::grow_byte_buffer   *obb);

//SUBSCRIPTION EVENT
void build_PKT_SBSEVT(unsigned int              sbsrid,
                      SubscriptionEventType     sevttp,
                      Action                    sbeact,
                      ProtocolCode              blzcod,
                      unsigned int              sevtid,
                      unsigned int              tmstp0,
                      unsigned int              tmstp1,
                      blaze::grow_byte_buffer   *obb);

//SUBSCRIPTION EVENT ACK
void build_PKT_SBSACK(unsigned int              sbsrid,
                      unsigned int              sevtid,
                      blaze::grow_byte_buffer   *obb);

//SUBSCRIPTION STOP REQUEST
void build_PKT_SBSTOP(unsigned int              sbsrid,
                      blaze::grow_byte_buffer   *obb);

//SUBSCRIPTION STOP RESPONSE
void build_PKT_SBSSPR(SubscriptionResponse      sbresl,
                      ProtocolCode              blzcod,
                      unsigned int              sbsrid,
                      blaze::grow_byte_buffer   *obb);

}

#endif
