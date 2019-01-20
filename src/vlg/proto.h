/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#pragma once
#include "glob.h"

/*****************************************
vlg PROTOCOL PUBLIC INTERFACE

VER: 000001
******************************************/

#define WORD_SZ 4   //word length [byte size]

namespace vlg {

/*****************************************
GLOB DEFS
******************************************/

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
BROKER TYPE
******************************************/
enum VLG_BROKER_Type {
    VLG_BROKER_Undef,
    VLG_BROKER_Client,
    VLG_BROKER_Server,
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
struct VLG_WRD_TMSTMP_REC {
    unsigned int            tmstmp;
};

/*****************************************
CLIHBT WORD RECORD
******************************************/
struct VLG_WRD_CLIHBT_REC {
    unsigned short          hbtsec;
};

/*****************************************
SRVCRS WORD RECORD
******************************************/
struct VLG_WRD_SRVCRS_REC {
    ConnectionResult        conres;
    ProtocolCode            errcod;
    unsigned short          agrhbt;
};

/*****************************************
DISWRD WORD RECORD
******************************************/
struct VLG_WRD_DISWRD_REC {
    ProtocolCode            disres;
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
    ProtocolCode        vlgcod;
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
    ProtocolCode            vlgcod;
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
    ProtocolCode            vlgcod;
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

#define VLG_MAX_HDR_SZ 9*4

/*****************************************
GENERIC HEADER STRUCTURE
******************************************/
struct vlg_hdr_rec {
    explicit vlg_hdr_rec() {
        memset(this, 0, sizeof(vlg_hdr_rec));
    }
    explicit vlg_hdr_rec(const vlg_hdr_rec &oth) {
        memcpy(this, &oth, sizeof(vlg_hdr_rec));
    }
    void operator=(const vlg_hdr_rec &oth) {
        memcpy(this, &oth, sizeof(vlg_hdr_rec));
    }
    void reset() {
        memset(this, 0, sizeof(vlg_hdr_rec));
    }
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
};

#define DMP_OUT_BUF_LEN 1024

int dump_vlg_hdr_rec(const vlg_hdr_rec *hdr,
                     char *out);

/*****************************************
 GLOB PROTO UTILS
******************************************/

typedef unsigned int time_stamp;

/*****************************************
 PKT BUILD FUN
******************************************/

//TEST REQUEST
void build_PKT_TSTREQ(time_stamp tstamp,
                      unsigned int connid,
                      g_bbuf *obb);

//HEARTBEAT
void build_PKT_HRTBET(time_stamp tstamp,
                      unsigned int connid,
                      g_bbuf *obb);

//CONNECTION REQUEST
void build_PKT_CONREQ(unsigned short clihbt,
                      g_bbuf *obb);

//CONNECTION RESPONSE
void build_PKT_CONRES(ConnectionResult conres,
                      ProtocolCode errcod,
                      unsigned short agrhbt,
                      unsigned int connid,
                      g_bbuf *obb);

//DISCONNECTED
void build_PKT_DSCOND(ProtocolCode disres,
                      unsigned int connid,
                      g_bbuf *obb);

//TRANSACTION REQUEST
void build_PKT_TXRQST(TransactionRequestType txtype,
                      Action txactn,
                      tx_id *txid,
                      bool rsclrq,
                      Encode enctyp,
                      unsigned int nclsid,
                      unsigned int connid,
                      g_bbuf *obb);

//TRANSACTION RESPONSE
void build_PKT_TXRESP(TransactionResult txresl,
                      ProtocolCode vlgcod,
                      tx_id *txid,
                      bool rescls,
                      Encode enctyp,
                      unsigned int nclsid,
                      g_bbuf *obb);

//SUBSCRIPTION REQUEST
void build_PKT_SBSREQ(SubscriptionType sbstyp,
                      SubscriptionMode sbsmod,
                      SubscriptionFlowType flotyp,
                      SubscriptionDownloadType dwltyp,
                      Encode enctyp,
                      unsigned int nclsid,
                      unsigned int connid,
                      unsigned int rqstid,
                      unsigned int tmstp0,
                      unsigned int tmstp1,
                      g_bbuf *obb);

//SUBSCRIPTION RESPONSE
void build_PKT_SBSRES(SubscriptionResponse sbresl,
                      ProtocolCode vlgcod,
                      unsigned int rqstid,
                      unsigned int sbsrid,
                      g_bbuf *obb);

//SUBSCRIPTION EVENT
void build_PKT_SBSEVT(unsigned int sbsrid,
                      SubscriptionEventType sevttp,
                      Action sbeact,
                      ProtocolCode vlgcod,
                      unsigned int sevtid,
                      unsigned int tmstp0,
                      unsigned int tmstp1,
                      g_bbuf *obb);

//SUBSCRIPTION EVENT ACK
void build_PKT_SBSACK(unsigned int sbsrid,
                      unsigned int sevtid,
                      g_bbuf *obb);

//SUBSCRIPTION STOP REQUEST
void build_PKT_SBSTOP(unsigned int sbsrid,
                      g_bbuf *obb);

//SUBSCRIPTION STOP RESPONSE
void build_PKT_SBSSPR(SubscriptionResponse sbresl,
                      ProtocolCode vlgcod,
                      unsigned int sbsrid,
                      g_bbuf *obb);

/*****************************************
 WORD DECODE FUNCTIONS
 It is supposed that each Decode f reads 32 bit on buffer
******************************************/

inline void Decode_WRD_PKTHDR(const unsigned int *data_in,
                              VLG_WRD_PKTHDR_REC *rec)
{
    //PROVER |VVVVVV00000000000000000000000000|
    unsigned short tus = ((*data_in >> 2) & 0x3F);
    rec->prover = tus;
    //HDRLEN |000000HHHH0000000000000000000000|
    tus = ((*data_in << 2) & 0x000C) | ((*data_in >> 14) & 0x3);
    rec->hdrlen = tus;
    //PKTTYP |0000000000RRRRRR0000000000000000|
    rec->pkttyp = static_cast<VLG_PKT_ID>((*data_in >> 8) & 0x3F);
}

inline void Decode_WRD_PKTLEN(const unsigned int *data_in,
                              VLG_WRD_PKTLEN_REC *rec)
{
    //PKTLEN |LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL|
    rec->pktlen = ntohl(*data_in);
}

inline void Decode_WRD_TMSTMP(const unsigned int *data_in,
                              VLG_WRD_TMSTMP_REC *rec)
{
    //TMSTMP |TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT|
    rec->tmstmp = ntohl(*data_in);
}

inline void Decode_WRD_CLIHBT(const unsigned int *data_in,
                              VLG_WRD_CLIHBT_REC *rec)
{
    //CLIHBT |HHHHHHHH000000000000000000000000|
    rec->hbtsec = *data_in & 0xFF;
}

inline void Decode_WRD_SRVCRS(const unsigned int *data_in,
                              VLG_WRD_SRVCRS_REC *rec)
{
    //CONRES |RRR00000000000000000000000000000|
    rec->conres = static_cast<ConnectionResult>((*data_in >> 5) & 0x3);
    //ERRCOD |000EEEE0000000000000000000000000|
    rec->errcod = static_cast<ProtocolCode>((*data_in >> 1) & 0xF);
    //AGRHBT |00000000AAAAAAAA0000000000000000|
    rec->agrhbt = ((*data_in >> 8) & 0xFF);
}

inline void Decode_WRD_DISWRD(const unsigned int *data_in,
                              VLG_WRD_DISWRD_REC *rec)
{
    //DISRES |RRRR0000000000000000000000000000|
    rec->disres = static_cast<ProtocolCode>(*data_in >> 4);
}

inline void Decode_WRD_TXREQW(const unsigned int *data_in,
                              VLG_WRD_TXREQW_REC *rec)
{
    //TXTYPE |TTTT0000000000000000000000000000|
    rec->txtype = static_cast<TransactionRequestType>((*data_in >> 4) & 0xF);
    //TXACTN |0000AAA0000000000000000000000000|
    rec->txactn = static_cast<Action>((*data_in >> 1) & 0x7);
    //RSCLRQ |0000000b000000000000000000000000|
    rec->rsclrq = static_cast<bool>((*data_in) & 0x1);
}

inline void Decode_WRD_CLSENC(const unsigned int *data_in,
                              VLG_WRD_CLSENC_REC *rec)
{
    //ENCTYP |EEEE0000000000000000000000000000|
    rec->enctyp = static_cast<Encode>((*data_in >> 4) & 0xF);
    //NCLSSID |000000000000DDDDDDDDDDDDDDDDDDDD|
    /******************************************
    HGFE DCBA RQPO NMLI | 0000 VUTS 0000 0000 -->
    0000 0000 0000 VUTS | RQPO NMLI HGFE DCBA
    ******************************************/
    rec->nclsid = ntohl(((*data_in) & 0xFFFF0F00));
}

inline void Decode_WRD_TXRESW(const unsigned int *data_in,
                              VLG_WRD_TXRESW_REC *rec)
{
    //TXRESL |XXX00000000000000000000000000000|
    rec->txresl = static_cast<TransactionResult>((*data_in >> 5) & 0x7);
    //RESCLS |000b0000000000000000000000000000|
    rec->rescls = static_cast<bool>((*data_in >> 4) & 0x1);
    //VLGCOD |0000000000000000RRRRRRRRRRRRRRRR|
    /******************************************
    HGFE DCBA RQPO NMLI | 0000 0000 0000 0000 -->
    0000 0000 0000 0000 | RQPO NMLI HGFE DCBA
    ******************************************/
    rec->vlgcod = static_cast<ProtocolCode>(ntohl(*data_in) & 0xFFFF);
}

inline void Decode_WRD_TXPLID(const unsigned int *data_in,
                              VLG_WRD_TXPLID_REC *rec)
{
    //TXPLID |PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP|
    rec->txplid = ntohl(*data_in);
}

inline void Decode_WRD_TXSVID(const unsigned int *data_in,
                              VLG_WRD_TXSVID_REC *rec)
{
    //TXSVID |SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
    rec->txsvid = ntohl(*data_in);
}

inline void Decode_WRD_TXCNID(const unsigned int *data_in,
                              VLG_WRD_TXCNID_REC *rec)
{
    //TXCNID |CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC|
    rec->txcnid = ntohl(*data_in);
}

inline void Decode_WRD_TXPRID(const unsigned int *data_in,
                              VLG_WRD_TXPRID_REC *rec)
{
    //TXPRID |PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP|
    rec->txprid = ntohl(*data_in);
}

inline void Decode_WRD_CONNID(const unsigned int *data_in,
                              VLG_WRD_CONNID_REC *rec)
{
    //CONNID |CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC|
    rec->connid = ntohl(*data_in);
}

inline void Decode_WRD_RQSTID(const unsigned int *data_in,
                              VLG_WRD_RQSTID_REC *rec)
{
    //RQSTID |RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR|
    rec->rqstid = ntohl(*data_in);
}

inline void Decode_WRD_SBREQW(const unsigned int *data_in,
                              VLG_WRD_SBREQW_REC *rec)
{
    //SBSTYP |00SS 0000 0000 0000 | 0000 0000 0000 0000|
    rec->sbstyp = static_cast<SubscriptionType>((*data_in >> 4) & 0x3);
    //SBSMOD |0000 MMMM 0000 0000 | 0000 0000 0000 0000|
    rec->sbsmod = static_cast<SubscriptionMode>((*data_in) & 0xF);
    //FLOTYP |0000 0000 0FFF 0000 | 0000 0000 0000 0000|
    rec->flotyp = static_cast<SubscriptionFlowType>((*data_in >> 12) & 0x7);
    //DWLTYP |0000 0000 0000 0DDD | 0000 0000 0000 0000|
    rec->dwltyp = static_cast<SubscriptionDownloadType>((*data_in >> 8) & 0x7);
}

inline void Decode_WRD_SBRESW(const unsigned int *data_in,
                              VLG_WRD_SBRESW_REC *rec)
{
    //SBRESL |XXX00000000000000000000000000000|
    rec->sbresl = static_cast<SubscriptionResponse>((*data_in >> 5) & 0x7);
    //VLGCOD |0000000000000000RRRRRRRRRRRRRRRR|
    /******************************************
    HGFE DCBA RQPO NMLI | 0000 0000 0000 0000 -->
    0000 0000 0000 0000 | RQPO NMLI HGFE DCBA
    ******************************************/
    rec->vlgcod = static_cast<ProtocolCode>(ntohl(*data_in) & 0xFFFF);
}

inline void Decode_WRD_SBSRID(const unsigned int *data_in,
                              VLG_WRD_SBSRID_REC *rec)
{
    //SBSRID |SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
    rec->sbsrid = ntohl(*data_in);
}

inline void Decode_WRD_SEVTID(const unsigned int *data_in,
                              VLG_WRD_SEVTID_REC *rec)
{
    //SEVTID |EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE|
    rec->sevtid = ntohl(*data_in);
}

inline void Decode_WRD_SEVTTP(const unsigned int *data_in,
                              VLG_WRD_SEVTTP_REC *rec)
{
    //ENCTYP |TTTT0000000000000000000000000000|
    rec->sevttp = static_cast<SubscriptionEventType>((*data_in >> 4) & 0xF);
    //SBEACT |0000AAA0000000000000000000000000|
    rec->sbeact = static_cast<Action>((*data_in >> 1) & 0x7);
    //VLGCOD |0000000000000000RRRRRRRRRRRRRRRR|
    /******************************************
    HGFE DCBA RQPO NMLI | 0000 0000 0000 0000 -->
    0000 0000 0000 0000 | RQPO NMLI HGFE DCBA
    ******************************************/
    rec->vlgcod = static_cast<ProtocolCode>(ntohl(*data_in) & 0xFFFF);
}

}
