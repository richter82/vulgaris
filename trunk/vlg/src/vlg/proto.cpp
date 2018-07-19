/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "proto.h"
#include "conn_impl.h"

/*****************************************
 GLOB PROTO UTILS
******************************************/
namespace vlg {

int dump_vlg_hdr_rec(const vlg_hdr_rec *hdr, char *out)
{
    int offst = 0;
    offst += sprintf(&out[offst], "HDR_REC{PKTHDR[%x|%x|%x]", hdr->phdr.prover,
                     hdr->phdr.hdrlen,
                     hdr->phdr.pkttyp);
    switch(hdr->phdr.pkttyp) {
        case VLG_PKT_NONE:
        case VLG_PKT_TSTREQ_ID:
            /*TEST REQUEST*/
            offst += sprintf(&out[offst], "TMSTMP[%u]", hdr->row_1.tmstmp.tmstmp);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "CONNID[%u]", hdr->row_2.connid.connid);
            }
            break;
        case VLG_PKT_HRTBET_ID:
            /*HEARTBEAT*/
            offst += sprintf(&out[offst], "TMSTMP[%u]", hdr->row_1.tmstmp.tmstmp);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "CONNID[%u]", hdr->row_2.connid.connid);
            }
            break;
        case VLG_PKT_CONREQ_ID:
            /*CONNECTION REQUEST*/
            offst += sprintf(&out[offst], "CLIHBT[%u]", hdr->row_1.clihbt.hbtsec);
            break;
        case VLG_PKT_CONRES_ID:
            /*CONNECTION RESPONSE*/
            offst += sprintf(&out[offst], "SRVCRS[%x|%x|%u]", hdr->row_1.srvcrs.conres,
                             hdr->row_1.srvcrs.errcod,
                             hdr->row_1.srvcrs.agrhbt);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "CONNID[%u]", hdr->row_2.connid.connid);
            }
            break;
        case VLG_PKT_DSCOND_ID:
            /*DISCONNECTED*/
            offst += sprintf(&out[offst], "DISWRD[%x]", hdr->row_1.diswrd.disres);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "CONNID[%u]", hdr->row_2.connid.connid);
            }
            break;
        case VLG_PKT_TXRQST_ID:
            /*TRANSACTION REQUEST*/
            offst += sprintf(&out[offst], "TXREQW[%x|%x|%x]", hdr->row_1.txreqw.txtype,
                             hdr->row_1.txreqw.txactn,
                             hdr->row_1.txreqw.rsclrq);
            offst += sprintf(&out[offst], "TXPLID[%u]", hdr->row_2.txplid.txplid);
            offst += sprintf(&out[offst], "TXSVID[%u]", hdr->row_3.txsvid.txsvid);
            offst += sprintf(&out[offst], "TXCNID[%u]", hdr->row_4.txcnid.txcnid);
            offst += sprintf(&out[offst], "TXPRID[%u]", hdr->row_5.txprid.txprid);
            offst += sprintf(&out[offst], "PKTLEN[%u]", hdr->row_6.pktlen.pktlen);
            offst += sprintf(&out[offst], "CLSENC[%u|%x]", hdr->row_7.clsenc.nclsid,
                             hdr->row_7.clsenc.enctyp);
            offst += sprintf(&out[offst], "CONNID[%u]", hdr->row_8.connid.connid);
            break;
        case VLG_PKT_TXRESP_ID:
            /*TRANSACTION RESPONSE*/
            offst += sprintf(&out[offst], "TXRESW[%x|%x|%x]", hdr->row_1.txresw.txresl,
                             hdr->row_1.txresw.vlgcod,
                             hdr->row_1.txresw.rescls);
            offst += sprintf(&out[offst], "TXPLID[%u]", hdr->row_2.txplid.txplid);
            offst += sprintf(&out[offst], "TXSVID[%u]", hdr->row_3.txsvid.txsvid);
            offst += sprintf(&out[offst], "TXCNID[%u]", hdr->row_4.txcnid.txcnid);
            offst += sprintf(&out[offst], "TXPRID[%u]", hdr->row_5.txprid.txprid);
            if(hdr->phdr.hdrlen == 8) {
                offst += sprintf(&out[offst], "PKTLEN[%u]", hdr->row_6.pktlen.pktlen);
                offst += sprintf(&out[offst], "CLSENC[%u|%x]", hdr->row_7.clsenc.nclsid,
                                 hdr->row_7.clsenc.enctyp);
            }
            break;
        case VLG_PKT_SBSREQ_ID:
            /*SUBSCRIPTION REQUEST*/
            offst += sprintf(&out[offst], "SBREQW[%x|%x|%x|%x]",
                             hdr->row_1.sbreqw.sbstyp,
                             hdr->row_1.sbreqw.sbsmod,
                             hdr->row_1.sbreqw.flotyp,
                             hdr->row_1.sbreqw.dwltyp);
            offst += sprintf(&out[offst], "CLSENC[%u|%x]", hdr->row_2.clsenc.nclsid,
                             hdr->row_2.clsenc.enctyp);
            offst += sprintf(&out[offst], "CONNID[%u]", hdr->row_3.connid.connid);
            offst += sprintf(&out[offst], "RQSTID[%u]", hdr->row_4.rqstid.rqstid);
            if(hdr->phdr.hdrlen == 7) {
                offst += sprintf(&out[offst], "TMSTMP[0][%u]", hdr->row_5.tmstmp.tmstmp);
                offst += sprintf(&out[offst], "TMSTMP[1][%u]", hdr->row_6.tmstmp.tmstmp);
            }
            break;
        case VLG_PKT_SBSRES_ID:
            /*SUBSCRIPTION RESPONSE*/
            offst += sprintf(&out[offst], "SBRESW[%x|%x]", hdr->row_1.sbresw.sbresl,
                             hdr->row_1.sbresw.vlgcod);
            offst += sprintf(&out[offst], "RQSTID[%u]", hdr->row_2.rqstid.rqstid);
            if(hdr->phdr.hdrlen == 4) {
                offst += sprintf(&out[offst], "SBSRID[%u]", hdr->row_3.sbsrid.sbsrid);
            }
            break;
        case VLG_PKT_SBSEVT_ID:
            /*SUBSCRIPTION EVENT*/
            offst += sprintf(&out[offst], "SBSRID[%u]", hdr->row_1.sbsrid.sbsrid);
            offst += sprintf(&out[offst], "SEVTTP[%x|%x|%x|]", hdr->row_2.sevttp.sevttp,
                             hdr->row_2.sevttp.sbeact,
                             hdr->row_2.sevttp.vlgcod);
            offst += sprintf(&out[offst], "SEVTID[%u]", hdr->row_3.sevtid.sevtid);
            offst += sprintf(&out[offst], "TMSTMP[0][%u]", hdr->row_4.tmstmp.tmstmp);
            offst += sprintf(&out[offst], "TMSTMP[1][%u]", hdr->row_5.tmstmp.tmstmp);
            if(hdr->phdr.hdrlen == 7) {
                offst += sprintf(&out[offst], "PKTLEN[%u]", hdr->row_6.pktlen.pktlen);
            }
            break;
        case VLG_PKT_SBSACK_ID:
            /*SUBSCRIPTION EVENT ACK*/
            offst += sprintf(&out[offst], "SBSRID[%u]", hdr->row_1.sbsrid.sbsrid);
            offst += sprintf(&out[offst], "SEVTID[%u]", hdr->row_2.sevtid.sevtid);
            break;
        case VLG_PKT_SBSTOP_ID:
            /*SUBSCRIPTION STOP REQUEST*/
            offst += sprintf(&out[offst], "SBSRID[%u]", hdr->row_1.sbsrid.sbsrid);
            break;
        case VLG_PKT_SBSSPR_ID:
            /*SUBSCRIPTION STOP RESPONSE*/
            offst += sprintf(&out[offst], "SBRESW[%x|%x]", hdr->row_1.sbresw.sbresl,
                             hdr->row_1.sbresw.vlgcod);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "SBSRID[%u]", hdr->row_2.sbsrid.sbsrid);
            }
            break;
        default:
            break;
    }
    offst += sprintf(&out[offst], "}");
    return offst;
}

/*****************************************
 WORD ENCODE FUNCTIONS
 It is supposed that each Encode f writes 32 bit on buffer
******************************************/

/*****************************************
SOME TIPS:
We are encoding from little-endian to big-endian.
For words (32) that are composed, in word sub-fields are encoded with big-endian
order.

[<<] is the left-bit-shift operator, multiplication.
This is used with encoding process. In fact, we are positioning the least
significant bits of a machine value [enum, int] to the newly position in the
protocol word.
Then we compose the protocol word with the [|=] operator, putting the newly
calculated bits in it.

REMEMBER THAT MACHINE [little-endian] INT IS COMPOSED THIS WAY:

1          2           3           4
|hgfe dcba | ponm lkji | xwvu tsrq | FEDC BAzy |

[1 --> 4] [LESS TO MOST SIGNIFICANT BYTE]

MASKS FOR ENCODING:

0x0000000F

1          2           3           4
|0000 1111 | 0000 0000 | 0000 0000 | 0000 0000 |

0x000000F0

1          2           3           4
|1111 0000 | 0000 0000 | 0000 0000 | 0000 0000 |

0x00000F00

1          2           3           4
|0000 0000 | 0000 1111 | 0000 0000 | 0000 0000 |

0x0000F000

1          2           3           4
|0000 0000 | 1111 0000 | 0000 0000 | 0000 0000 |

******************************************/


inline void Encode_WRD_PKTHDR(const VLG_WRD_PKTHDR_REC *rec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //PROVER |VVVV VV00 0000 0000|0000 0000 0000 0000|
    data_out |= ((rec->prover << 2) & 0xFC);
    //HDRLEN |0000 00HH HH00 0000|0000 0000 0000 0000|
    /******************************************
    0000 DCBA 0000 0000 -->
    0000 00DC BA00 0000
    ******************************************/
    data_out |= (((rec->hdrlen >> 2) | (rec->hdrlen << 14)) & 0xC003);
    //PKTTYP |0000 0000 00RR RRRR|0000 0000 0000 0000|
    data_out |= ((rec->pkttyp << 8) & 0x3F00);
    obb->append_uint(data_out);
}

inline void Encode_WRD_PKTLEN(const unsigned int *pktlen,
                              g_bbuf *obb)
{
    //PKTLEN |LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL|
    obb->append_uint(htonl(*pktlen));
}

inline void Encode_WRD_TMSTMP(const time_stamp *tstamp,
                              g_bbuf *obb)
{
    //TMSTMP |TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT|
    obb->append_uint(htonl(*tstamp));
}

inline void Encode_WRD_CLIHBT(const unsigned short *hbtsec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //CLIHBT |HHHH HHHH 0000 0000|0000 0000 0000 0000|
    data_out = (*hbtsec & 0xFF);
    obb->append_uint(data_out);
}

inline void Encode_WRD_SRVCRS(const VLG_WRD_SRVCRS_REC *rec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //CONRES |RRR0 0000 0000 0000|0000 0000 0000 0000|
    data_out |= ((rec->conres << 5) & 0xE0);
    //ERRCOD |000E EEE0 0000 0000|0000 0000 0000 0000|
    data_out |= ((rec->errcod << 1) & 0x1E);
    //AGRHBT |0000 0000 AAAA AAAA|0000 0000 0000 0000|
    data_out |= (rec->agrhbt << 8);
    obb->append_uint(data_out);
}

inline void Encode_WRD_DISWRD(const VLG_WRD_DISWRD_REC *rec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //DISRES |RRRR0000000000000000000000000000|
    data_out = ((rec->disres << 4) & 0xF0);
    obb->append_uint(data_out);
}

inline void Encode_WRD_TXREQW(const VLG_WRD_TXREQW_REC *rec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //TXTYPE |TTTT0000000000000000000000000000|
    data_out |= ((rec->txtype << 4) & 0xF0);
    //TXACTN |0000AAA0000000000000000000000000|
    data_out |= ((rec->txactn << 1) & 0xE);
    //RSCLRQ |0000000b000000000000000000000000|
    data_out |= ((rec->rsclrq) & 0x1);
    obb->append_uint(data_out);
}

inline void Encode_WRD_CLSENC(const VLG_WRD_CLSENC_REC *rec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //ENCTYP |EEEE0000000000000000000000000000|
    data_out |= ((rec->enctyp << 4) & 0xF0);
    //NCLSSID |000000000000DDDDDDDDDDDDDDDDDDDD|
    /******************************************
    HGFE DCBA RQPO NMLI | 0000 VUTS 0000 0000 -->
    0000 0000 0000 VUTS | RQPO NMLI HGFE DCBA
    ******************************************/
    data_out |= (htonl(rec->nclsid) & 0xFFFF0000);
    obb->append_uint(data_out);
}

inline void Encode_WRD_TXRESW(const VLG_WRD_TXRESW_REC *rec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //TXRESL |XXX00000000000000000000000000000|
    data_out |= ((rec->txresl << 5) & 0xE0);
    //RESCLS |000b0000000000000000000000000000|
    data_out |= ((rec->rescls << 4) & 0x10);
    //TXRCOD |0000000000000000RRRRRRRRRRRRRRRR|
    /******************************************
    HGFE DCBA RQPO NMLI | 0000 0000 0000 0000 -->
    0000 0000 0000 0000 | RQPO NMLI HGFE DCBA
    ******************************************/
    int tui = rec->vlgcod;
    data_out |= (htonl(tui) & 0xFFFF0000);
    obb->append_uint(data_out);
}

inline void Encode_WRD_TXPLID(const unsigned int *txplid,
                              g_bbuf *obb)
{
    //TXPLID |PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP|
    obb->append_uint(htonl(*txplid));
}

inline void Encode_WRD_TXSVID(const unsigned int *txsvid,
                              g_bbuf *obb)
{
    //TXSVID |SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
    obb->append_uint(htonl(*txsvid));
}

inline void Encode_WRD_TXCNID(const unsigned int *txcnid,
                              g_bbuf *obb)
{
    //TXCNID |CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC|
    obb->append_uint(htonl(*txcnid));
}

inline void Encode_WRD_TXPRID(const unsigned int *txprid,
                              g_bbuf *obb)
{
    //TXPRID |PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP|
    obb->append_uint(htonl(*txprid));
}

inline void Encode_WRD_CONNID(const unsigned int *connid,
                              g_bbuf *obb)
{
    //CONNID |CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC|
    obb->append_uint(htonl(*connid));
}

inline void Encode_WRD_RQSTID(const unsigned int *rqstid,
                              g_bbuf *obb)
{
    //RQSTID |RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR|
    obb->append_uint(htonl(*rqstid));
}

inline void Encode_WRD_SBREQW(const VLG_WRD_SBREQW_REC *rec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //SBSTYP |00SS 0000 0000 0000 | 0000 0000 0000 0000|
    data_out |= ((rec->sbstyp << 4) & 0x30);
    //SBSMOD |0000 MMMM 0000 0000 | 0000 0000 0000 0000|
    data_out |= ((rec->sbsmod) & 0xF);
    //FLOTYP |0000 0000 0FFF 0000 | 0000 0000 0000 0000|
    data_out |= ((rec->flotyp << 12) & 0x7000);
    //DWLTYP |0000 0000 0000 0DDD | 0000 0000 0000 0000|
    data_out |= ((rec->dwltyp << 8) & 0x700);
    obb->append_uint(data_out);
}

inline void Encode_WRD_SBRESW(const VLG_WRD_SBRESW_REC *rec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //TXRESL |XXX00000000000000000000000000000|
    data_out |= ((rec->sbresl << 5) & 0xE0);
    //TXRCOD |0000000000000000RRRRRRRRRRRRRRRR|
    /******************************************
    HGFE DCBA RQPO NMLI | 0000 0000 0000 0000 -->
    0000 0000 0000 0000 | RQPO NMLI HGFE DCBA
    ******************************************/
    unsigned int tui = rec->vlgcod;
    data_out |= (htonl(tui) & 0xFFFF0000);
    obb->append_uint(data_out);
}

inline void Encode_WRD_SBSRID(const unsigned int *sbsrid,
                              g_bbuf *obb)
{
    //SBSRID |SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
    obb->append_uint(htonl(*sbsrid));
}

inline void Encode_WRD_SEVTID(const unsigned int *sevtid,
                              g_bbuf *obb)
{
    //SEVTID |EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE|
    obb->append_uint(htonl(*sevtid));
}

inline void Encode_WRD_SEVTTP(const VLG_WRD_SEVTTP_REC *rec,
                              g_bbuf *obb)
{
    unsigned int data_out = 0;
    //SEVTTP |TTTT0000000000000000000000000000|
    data_out |= ((rec->sevttp << 4) & 0xF0);
    //SBEACT |0000AAA0000000000000000000000000|
    data_out |= ((rec->sbeact << 1) & 0xE);
    /******************************************
    HGFE DCBA RQPO NMLI | 0000 0000 0000 0000 -->
    0000 0000 0000 0000 | RQPO NMLI HGFE DCBA
    ******************************************/
    unsigned int tui = rec->vlgcod;
    data_out |= (htonl(tui) & 0xFFFF0000);
    obb->append_uint(data_out);
}

/*****************************************
 PKT BUILD FUN
******************************************/

//TEST REQUEST
void build_PKT_TSTREQ(time_stamp tstamp,
                      unsigned int connid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    pkthdr.hdrlen = connid ? 3 : 2;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_TSTREQ_ID;
    Encode_WRD_PKTHDR(&pkthdr, obb);
    Encode_WRD_TMSTMP(&tstamp, obb);
    if(connid) {
        Encode_WRD_CONNID(&connid, obb);
    }
}

//HEARTBEAT
void build_PKT_HRTBET(time_stamp tstamp,
                      unsigned int connid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    pkthdr.hdrlen = connid ? 3 : 2;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_HRTBET_ID;
    Encode_WRD_PKTHDR(&pkthdr, obb);
    Encode_WRD_TMSTMP(&tstamp, obb);
    if(connid) {
        Encode_WRD_CONNID(&connid, obb);
    }
}

//CONNECTION REQUEST
void build_PKT_CONREQ(unsigned short clihbt,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    pkthdr.hdrlen = 2;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_CONREQ_ID;
    Encode_WRD_PKTHDR(&pkthdr, obb);
    Encode_WRD_CLIHBT(&clihbt, obb);
}

//CONNECTION RESPONSE
void build_PKT_CONRES(ConnectionResult conres,
                      ProtocolCode errcod,
                      unsigned short agrhbt,
                      unsigned int connid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    pkthdr.hdrlen = connid ? 3 : 2;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_CONRES_ID;
    VLG_WRD_SRVCRS_REC srvcrs;
    srvcrs.conres = conres;
    srvcrs.errcod = errcod;
    srvcrs.agrhbt = agrhbt;
    Encode_WRD_PKTHDR(&pkthdr, obb);
    Encode_WRD_SRVCRS(&srvcrs, obb);
    if(connid) {
        Encode_WRD_CONNID(&connid, obb);
    }
}

//DISCONNECTED
void build_PKT_DSCOND(ProtocolCode disres,
                      unsigned int connid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    pkthdr.hdrlen = connid ? 3 : 2;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_DSCOND_ID;
    VLG_WRD_DISWRD_REC diswrd;
    diswrd.disres = disres;
    Encode_WRD_PKTHDR(&pkthdr, obb);
    Encode_WRD_DISWRD(&diswrd, obb);
    if(connid) {
        Encode_WRD_CONNID(&connid, obb);
    }
}

//TRANSACTION REQUEST
void build_PKT_TXRQST(TransactionRequestType txtype,
                      Action txactn,
                      tx_id *txid,
                      bool rsclrq,
                      Encode enctyp,
                      unsigned int nclsid,
                      unsigned int connid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    VLG_WRD_TXREQW_REC txreqw;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_TXRQST_ID;
    txreqw.txtype = txtype;
    txreqw.txactn = txactn;
    txreqw.rsclrq = rsclrq;
    if(txreqw.txtype == TransactionRequestType_OBJECT) {
        pkthdr.hdrlen = 9;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_TXREQW(&txreqw, obb);
        Encode_WRD_TXPLID(&txid->txplid, obb);
        Encode_WRD_TXSVID(&txid->txsvid, obb);
        Encode_WRD_TXCNID(&txid->txcnid, obb);
        Encode_WRD_TXPRID(&txid->txprid, obb);
        obb->advance_pos_write(4); //reserved space for pktlen
        VLG_WRD_CLSENC_REC clsenc;
        clsenc.enctyp = enctyp;
        clsenc.nclsid = nclsid;
        Encode_WRD_CLSENC(&clsenc, obb);
        Encode_WRD_CONNID(&connid, obb);
    } else {
        pkthdr.hdrlen = 7;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_TXREQW(&txreqw, obb);
        Encode_WRD_TXPLID(&txid->txplid, obb);
        Encode_WRD_TXSVID(&txid->txsvid, obb);
        Encode_WRD_TXCNID(&txid->txcnid, obb);
        Encode_WRD_TXPRID(&txid->txprid, obb);
        Encode_WRD_CONNID(&connid, obb);
    }
}

//TRANSACTION RESPONSE
void build_PKT_TXRESP(TransactionResult txresl,
                      ProtocolCode vlgcod,
                      tx_id *txid,
                      bool rescls,
                      Encode enctyp,
                      unsigned int nclsid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    VLG_WRD_TXRESW_REC txresw;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_TXRESP_ID;
    txresw.txresl = txresl;
    txresw.vlgcod = vlgcod;
    txresw.rescls = rescls;
    if(rescls) {
        pkthdr.hdrlen = 8;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_TXRESW(&txresw, obb);
        Encode_WRD_TXPLID(&txid->txplid, obb);
        Encode_WRD_TXSVID(&txid->txsvid, obb);
        Encode_WRD_TXCNID(&txid->txcnid, obb);
        Encode_WRD_TXPRID(&txid->txprid, obb);
        obb->advance_pos_write(4); //reserved space for pktlen
        VLG_WRD_CLSENC_REC clsenc;
        clsenc.enctyp = enctyp;
        clsenc.nclsid = nclsid;
        Encode_WRD_CLSENC(&clsenc, obb);
    } else {
        pkthdr.hdrlen = 6;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_TXRESW(&txresw, obb);
        Encode_WRD_TXPLID(&txid->txplid, obb);
        Encode_WRD_TXSVID(&txid->txsvid, obb);
        Encode_WRD_TXCNID(&txid->txcnid, obb);
        Encode_WRD_TXPRID(&txid->txprid, obb);
    }
}

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
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    VLG_WRD_SBREQW_REC sbreqw;
    VLG_WRD_CLSENC_REC clsenc;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_SBSREQ_ID;
    sbreqw.sbstyp = sbstyp;
    sbreqw.sbsmod = sbsmod;
    sbreqw.flotyp = flotyp;
    sbreqw.dwltyp = dwltyp;
    clsenc.enctyp = enctyp;
    clsenc.nclsid = nclsid;
    if(dwltyp == SubscriptionDownloadType_PARTIAL) {
        pkthdr.hdrlen = 7;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_SBREQW(&sbreqw, obb);
        Encode_WRD_CLSENC(&clsenc, obb);
        Encode_WRD_CONNID(&connid, obb);
        Encode_WRD_RQSTID(&rqstid, obb);
        Encode_WRD_TMSTMP(&tmstp0, obb);
        Encode_WRD_TMSTMP(&tmstp1, obb);
    } else {
        pkthdr.hdrlen = 5;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_SBREQW(&sbreqw, obb);
        Encode_WRD_CLSENC(&clsenc, obb);
        Encode_WRD_CONNID(&connid, obb);
        Encode_WRD_RQSTID(&rqstid, obb);
    }
}

//SUBSCRIPTION RESPONSE
void build_PKT_SBSRES(SubscriptionResponse sbresl,
                      ProtocolCode vlgcod,
                      unsigned int rqstid,
                      unsigned int sbsrid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    VLG_WRD_SBRESW_REC sbresw;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_SBSRES_ID;
    sbresw.sbresl = sbresl;
    sbresw.vlgcod = vlgcod;
    if(sbsrid) {
        pkthdr.hdrlen = 4;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_SBRESW(&sbresw, obb);
        Encode_WRD_RQSTID(&rqstid, obb);
        Encode_WRD_SBSRID(&sbsrid, obb);
    } else {
        pkthdr.hdrlen = 3;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_SBRESW(&sbresw, obb);
        Encode_WRD_RQSTID(&rqstid, obb);
    }
}

//SUBSCRIPTION EVENT
void build_PKT_SBSEVT(unsigned int sbsrid,
                      SubscriptionEventType sevttp,
                      Action sbeact,
                      ProtocolCode vlgcod,
                      unsigned int sevtid,
                      unsigned int tmstp0,
                      unsigned int tmstp1,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    VLG_WRD_SEVTTP_REC sevttpw;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_SBSEVT_ID;
    sevttpw.sevttp = sevttp;
    sevttpw.sbeact = sbeact;
    sevttpw.vlgcod = vlgcod;
    if(sevttp != SubscriptionEventType_DOWNLOAD_END) {
        pkthdr.hdrlen = 7;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_SBSRID(&sbsrid, obb);
        Encode_WRD_SEVTTP(&sevttpw, obb);
        Encode_WRD_SEVTID(&sevtid, obb);
        Encode_WRD_TMSTMP(&tmstp0, obb);
        Encode_WRD_TMSTMP(&tmstp1, obb);
        obb->advance_pos_write(4); //reserved space for pktlen
    } else {
        pkthdr.hdrlen = 6;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_SBSRID(&sbsrid, obb);
        Encode_WRD_SEVTTP(&sevttpw, obb);
        Encode_WRD_SEVTID(&sevtid, obb);
        Encode_WRD_TMSTMP(&tmstp0, obb); /*maybe not necessary*/
        Encode_WRD_TMSTMP(&tmstp1, obb); /*maybe not necessary*/
    }
}

//SUBSCRIPTION EVENT ACK
void build_PKT_SBSACK(unsigned int sbsrid,
                      unsigned int sevtid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    pkthdr.hdrlen = 3;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_SBSACK_ID;
    Encode_WRD_PKTHDR(&pkthdr, obb);
    Encode_WRD_SBSRID(&sbsrid, obb);
    Encode_WRD_SEVTID(&sevtid, obb);
}

//SUBSCRIPTION STOP REQUEST
void build_PKT_SBSTOP(unsigned int sbsrid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_SBSTOP_ID;
    pkthdr.hdrlen = 2;
    Encode_WRD_PKTHDR(&pkthdr, obb);
    Encode_WRD_SBSRID(&sbsrid, obb);
}

//SUBSCRIPTION STOP RESPONSE
void build_PKT_SBSSPR(SubscriptionResponse sbresl,
                      ProtocolCode vlgcod,
                      unsigned int sbsrid,
                      g_bbuf *obb)
{
    VLG_WRD_PKTHDR_REC pkthdr;
    VLG_WRD_SBRESW_REC sbresw;
    pkthdr.prover = VLG_PROTO_VER;
    pkthdr.pkttyp = VLG_PKT_SBSSPR_ID;
    sbresw.sbresl = sbresl;
    sbresw.vlgcod = vlgcod;
    if(sbsrid) {
        pkthdr.hdrlen = 3;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_SBRESW(&sbresw, obb);
        Encode_WRD_SBSRID(&sbsrid, obb);
    } else {
        pkthdr.hdrlen = 2;
        Encode_WRD_PKTHDR(&pkthdr, obb);
        Encode_WRD_SBRESW(&sbresw, obb);
    }
}

}
