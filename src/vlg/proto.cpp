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

#include "proto.h"
#include "conn_impl.h"

#define VLG_WRD_BYTE_SIZE       4   //word length [byte size]

/*****************************************
 GLOB PROTO UTILS
******************************************/
namespace vlg {

vlg_hdr_rec::vlg_hdr_rec()
{
    memset(this, 0, sizeof(vlg_hdr_rec));
}

tx_id::tx_id()
{
    memset(this, 0, sizeof(tx_id));
}

int dump_vlg_hdr_rec(const vlg_hdr_rec *hdr, char *out)
{
    int offst = 0;
    offst += sprintf(&out[offst], "HDR_REC{PKTHDR[%02x|%x|%02x]", hdr->phdr.prover,
                     hdr->phdr.hdrlen,
                     hdr->phdr.pkttyp);
    switch(hdr->phdr.pkttyp) {
        case VLG_PKT_NONE:
        case VLG_PKT_TSTREQ_ID:
            /*TEST REQUEST*/
            offst += sprintf(&out[offst], "TMSTMP[%010u]", hdr->row_1.tmstmp.tmstmp);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "CONNID[%010u]", hdr->row_2.connid.connid);
            }
            break;
        case VLG_PKT_HRTBET_ID:
            /*HEARTBEAT*/
            offst += sprintf(&out[offst], "TMSTMP[%010u]", hdr->row_1.tmstmp.tmstmp);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "CONNID[%010u]", hdr->row_2.connid.connid);
            }
            break;
        case VLG_PKT_CONREQ_ID:
            /*CONNECTION REQUEST*/
            offst += sprintf(&out[offst], "CLIHBT[%06u]", hdr->row_1.clihbt.hbtsec);
            break;
        case VLG_PKT_CONRES_ID:
            /*CONNECTION RESPONSE*/
            offst += sprintf(&out[offst], "SRVCRS[%x|%x|%06u]", hdr->row_1.srvcrs.conres,
                             hdr->row_1.srvcrs.errcod,
                             hdr->row_1.srvcrs.agrhbt);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "CONNID[%010u]", hdr->row_2.connid.connid);
            }
            break;
        case VLG_PKT_DSCOND_ID:
            /*DISCONNECTED*/
            offst += sprintf(&out[offst], "DISWRD[%x]", hdr->row_1.diswrd.disres);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "CONNID[%010u]", hdr->row_2.connid.connid);
            }
            break;
        case VLG_PKT_TXRQST_ID:
            /*TRANSACTION REQUEST*/
            offst += sprintf(&out[offst], "TXREQW[%x|%x|%x]", hdr->row_1.txreqw.txtype,
                             hdr->row_1.txreqw.txactn,
                             hdr->row_1.txreqw.rsclrq);
            offst += sprintf(&out[offst], "TXPLID[%010u]", hdr->row_2.txplid.txplid);
            offst += sprintf(&out[offst], "TXSVID[%010u]", hdr->row_3.txsvid.txsvid);
            offst += sprintf(&out[offst], "TXCNID[%010u]", hdr->row_4.txcnid.txcnid);
            offst += sprintf(&out[offst], "TXPRID[%010u]", hdr->row_5.txprid.txprid);
            offst += sprintf(&out[offst], "PKTLEN[%010u]", hdr->row_6.pktlen.pktlen);
            offst += sprintf(&out[offst], "CLSENC[%07u|%x]", hdr->row_7.clsenc.nclsid,
                             hdr->row_7.clsenc.enctyp);
            offst += sprintf(&out[offst], "CONNID[%010u]", hdr->row_8.connid.connid);
            break;
        case VLG_PKT_TXRESP_ID:
            /*TRANSACTION RESPONSE*/
            offst += sprintf(&out[offst], "TXRESW[%x|%x|%04x]", hdr->row_1.txresw.txresl,
                             hdr->row_1.txresw.vlgcod,
                             hdr->row_1.txresw.rescls);
            offst += sprintf(&out[offst], "TXPLID[%u]", hdr->row_2.txplid.txplid);
            offst += sprintf(&out[offst], "TXSVID[%u]", hdr->row_3.txsvid.txsvid);
            offst += sprintf(&out[offst], "TXCNID[%u]", hdr->row_4.txcnid.txcnid);
            offst += sprintf(&out[offst], "TXPRID[%u]", hdr->row_5.txprid.txprid);
            if(hdr->phdr.hdrlen == 8) {
                offst += sprintf(&out[offst], "PKTLEN[%u]", hdr->row_6.pktlen.pktlen);
                offst += sprintf(&out[offst], "CLSENC[%07u|%x]", hdr->row_7.clsenc.nclsid,
                                 hdr->row_7.clsenc.enctyp);
            }
            break;
        case VLG_PKT_SBSREQ_ID:
            /*SUBSCRIPTION REQUEST*/
            offst += sprintf(&out[offst], "SBREQW[%01x|%02x|%02x|%02x]",
                             hdr->row_1.sbreqw.sbstyp,
                             hdr->row_1.sbreqw.sbsmod,
                             hdr->row_1.sbreqw.flotyp,
                             hdr->row_1.sbreqw.dwltyp);
            offst += sprintf(&out[offst], "CLSENC[%07u|%x]", hdr->row_2.clsenc.nclsid,
                             hdr->row_2.clsenc.enctyp);
            offst += sprintf(&out[offst], "CONNID[%010u]", hdr->row_3.connid.connid);
            offst += sprintf(&out[offst], "RQSTID[%010u]", hdr->row_4.rqstid.rqstid);
            if(hdr->phdr.hdrlen == 7) {
                offst += sprintf(&out[offst], "TMSTMP[0][%010u]", hdr->row_5.tmstmp.tmstmp);
                offst += sprintf(&out[offst], "TMSTMP[1][%010u]", hdr->row_6.tmstmp.tmstmp);
            }
            break;
        case VLG_PKT_SBSRES_ID:
            /*SUBSCRIPTION RESPONSE*/
            offst += sprintf(&out[offst], "SBRESW[%x|%x]", hdr->row_1.sbresw.sbresl,
                             hdr->row_1.sbresw.vlgcod);
            offst += sprintf(&out[offst], "RQSTID[%010u]", hdr->row_2.rqstid.rqstid);
            if(hdr->phdr.hdrlen == 4) {
                offst += sprintf(&out[offst], "SBSRID[%010u]", hdr->row_3.sbsrid.sbsrid);
            }
            break;
        case VLG_PKT_SBSEVT_ID:
            /*SUBSCRIPTION EVENT*/
            offst += sprintf(&out[offst], "SBSRID[%010u]", hdr->row_1.sbsrid.sbsrid);
            offst += sprintf(&out[offst], "SEVTTP[%02x|%x|%04x|]", hdr->row_2.sevttp.sevttp,
                             hdr->row_2.sevttp.sbeact,
                             hdr->row_2.sevttp.vlgcod);
            offst += sprintf(&out[offst], "SEVTID[%010u]", hdr->row_3.sevtid.sevtid);
            offst += sprintf(&out[offst], "TMSTMP[0][%010u]", hdr->row_4.tmstmp.tmstmp);
            offst += sprintf(&out[offst], "TMSTMP[1][%010u]", hdr->row_5.tmstmp.tmstmp);
            if(hdr->phdr.hdrlen == 7) {
                offst += sprintf(&out[offst], "PKTLEN[%u]", hdr->row_6.pktlen.pktlen);
            }
            break;
        case VLG_PKT_SBSACK_ID:
            /*SUBSCRIPTION EVENT ACK*/
            offst += sprintf(&out[offst], "SBSRID[%010u]", hdr->row_1.sbsrid.sbsrid);
            offst += sprintf(&out[offst], "SEVTID[%010u]", hdr->row_2.sevtid.sevtid);
            break;
        case VLG_PKT_SBSTOP_ID:
            /*SUBSCRIPTION STOP REQUEST*/
            offst += sprintf(&out[offst], "SBSRID[%010u]", hdr->row_1.sbsrid.sbsrid);
            break;
        case VLG_PKT_SBSSPR_ID:
            /*SUBSCRIPTION STOP RESPONSE*/
            offst += sprintf(&out[offst], "SBRESW[%x|%x]", hdr->row_1.sbresw.sbresl,
                             hdr->row_1.sbresw.vlgcod);
            if(hdr->phdr.hdrlen == 3) {
                offst += sprintf(&out[offst], "SBSRID[%010u]", hdr->row_2.sbsrid.sbsrid);
            }
            break;
        default:
            break;
    }
    offst += sprintf(&out[offst], "}");
    return offst;
}

const char *dump_raw_pkt(bool all,
                         const unsigned char *buf,
                         size_t buf_sz,
                         std::string &out)
{
    char abuff[32];
    size_t buf_offst = 0;
    int i = 0;
    sprintf(abuff, "RAW_%s[", all ? "PKT" : "BDY");
    out.assign(abuff);
    while(buf_offst < buf_sz) {
        sprintf(abuff, "|%d|%08x", i++, *(unsigned int *)&buf[buf_offst]);
        out.append(abuff);
        buf_offst += 4;
    }
    out.append("]");
    return out.c_str();
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
                              vlg::g_bbuf *obb)
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
                              vlg::g_bbuf *obb)
{
    //PKTLEN |LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL|
    obb->append_uint(htonl(*pktlen));
}

inline void Encode_WRD_TMSTMP(const time_stamp *tstamp,
                              vlg::g_bbuf *obb)
{
    //TMSTMP |TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT|
    obb->append_uint(htonl(*tstamp));
}

inline void Encode_WRD_CLIHBT(const unsigned short *hbtsec,
                              vlg::g_bbuf *obb)
{
    unsigned int data_out = 0;
    //CLIHBT |HHHH HHHH 0000 0000|0000 0000 0000 0000|
    data_out = (*hbtsec & 0xFF);
    obb->append_uint(data_out);
}

inline void Encode_WRD_SRVCRS(const VLG_WRD_SRVCRS_REC *rec,
                              vlg::g_bbuf *obb)
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
                              vlg::g_bbuf *obb)
{
    unsigned int data_out = 0;
    //DISRES |RRRR0000000000000000000000000000|
    data_out = ((rec->disres << 4) & 0xF0);
    obb->append_uint(data_out);
}

inline void Encode_WRD_TXREQW(const VLG_WRD_TXREQW_REC *rec,
                              vlg::g_bbuf *obb)
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
                              vlg::g_bbuf *obb)
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
                              vlg::g_bbuf *obb)
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
                              vlg::g_bbuf *obb)
{
    //TXPLID |PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP|
    obb->append_uint(htonl(*txplid));
}

inline void Encode_WRD_TXSVID(const unsigned int *txsvid,
                              vlg::g_bbuf *obb)
{
    //TXSVID |SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
    obb->append_uint(htonl(*txsvid));
}

inline void Encode_WRD_TXCNID(const unsigned int *txcnid,
                              vlg::g_bbuf *obb)
{
    //TXCNID |CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC|
    obb->append_uint(htonl(*txcnid));
}

inline void Encode_WRD_TXPRID(const unsigned int *txprid,
                              vlg::g_bbuf *obb)
{
    //TXPRID |PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP|
    obb->append_uint(htonl(*txprid));
}

inline void Encode_WRD_CONNID(const unsigned int *connid,
                              vlg::g_bbuf *obb)
{
    //CONNID |CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC|
    obb->append_uint(htonl(*connid));
}

inline void Encode_WRD_RQSTID(const unsigned int *rqstid,
                              vlg::g_bbuf *obb)
{
    //RQSTID |RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR|
    obb->append_uint(htonl(*rqstid));
}

inline void Encode_WRD_SBREQW(const VLG_WRD_SBREQW_REC *rec,
                              vlg::g_bbuf *obb)
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
                              vlg::g_bbuf *obb)
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
                              vlg::g_bbuf *obb)
{
    //SBSRID |SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
    obb->append_uint(htonl(*sbsrid));
}

inline void Encode_WRD_SEVTID(const unsigned int *sevtid,
                              vlg::g_bbuf *obb)
{
    //SEVTID |EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE|
    obb->append_uint(htonl(*sevtid));
}

inline void Encode_WRD_SEVTTP(const VLG_WRD_SEVTTP_REC *rec,
                              vlg::g_bbuf *obb)
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

/*****************************************
 PKT BUILD FUN
******************************************/

//TEST REQUEST
void build_PKT_TSTREQ(time_stamp tstamp,
                      unsigned int connid,
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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
                      vlg::g_bbuf *obb)
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

// CONNECTION RECV METHS

inline RetCode connection_impl::recv_single_hdr_row(unsigned int *hdr_row)
{
    RetCode rcode = vlg::RetCode_OK;
    bool stay = true;
    long brecv = 0, tot_brecv = 0, recv_buf_sz = VLG_WRD_BYTE_SIZE;
    char buff[VLG_WRD_BYTE_SIZE] = {0};
    while(stay) {
        while((tot_brecv < VLG_WRD_BYTE_SIZE) && ((brecv = recv(socket_, &buff[tot_brecv], recv_buf_sz, 0)) > 0)) {
            tot_brecv += brecv;
            recv_buf_sz -= brecv;
            IFLOG(trc(TH_ID, LS_TRL "[recv:%d, tot_recv:%d, recv_buf_sz:%d]",
                      __func__, brecv, tot_brecv, recv_buf_sz))
        }
        if(tot_brecv != VLG_WRD_BYTE_SIZE) {
            if((rcode = socket_excptn_hndl(brecv)) != vlg::RetCode_SCKEAGN) {
                stay = false;
            } else {
                rcode = vlg::RetCode_OK;
            }
        } else {
            break;
        }
    }
    memcpy(hdr_row, buff, VLG_WRD_BYTE_SIZE);
    return rcode;
}

#define RCVSNGLROW if((rcode = recv_single_hdr_row(&hdr_row))) return rcode;

RetCode connection_impl::recv_and_decode_hdr(vlg_hdr_rec *pkt_hdr)
{
    RetCode rcode = vlg::RetCode_OK;
    unsigned int hdr_row = 0;
    RCVSNGLROW
    Decode_WRD_PKTHDR(&hdr_row, &pkt_hdr->phdr);
    pkt_hdr->hdr_bytelen = pkt_hdr->phdr.hdrlen * VLG_WRD_BYTE_SIZE;
    switch(pkt_hdr->phdr.pkttyp) {
        case VLG_PKT_TSTREQ_ID:
            /*TEST REQUEST*/
            RCVSNGLROW
            Decode_WRD_TMSTMP(&hdr_row, &pkt_hdr->row_1.tmstmp);
            if(pkt_hdr->phdr.hdrlen == 3) {
                RCVSNGLROW
                Decode_WRD_CONNID(&hdr_row, &pkt_hdr->row_2.connid);
            }
            break;
        case VLG_PKT_HRTBET_ID:
            /*HEARTBEAT*/
            RCVSNGLROW
            Decode_WRD_TMSTMP(&hdr_row, &pkt_hdr->row_1.tmstmp);
            if(pkt_hdr->phdr.hdrlen == 3) {
                RCVSNGLROW
                Decode_WRD_CONNID(&hdr_row, &pkt_hdr->row_2.connid);
            }
            break;
        case VLG_PKT_CONREQ_ID:
            /*CONNECTION REQUEST*/
            RCVSNGLROW
            Decode_WRD_CLIHBT(&hdr_row, &pkt_hdr->row_1.clihbt);
            break;
        case VLG_PKT_CONRES_ID:
            /*CONNECTION RESPONSE*/
            RCVSNGLROW
            Decode_WRD_SRVCRS(&hdr_row, &pkt_hdr->row_1.srvcrs);
            if(pkt_hdr->phdr.hdrlen == 3) {
                RCVSNGLROW
                Decode_WRD_CONNID(&hdr_row, &pkt_hdr->row_2.connid);
            }
            break;
        case VLG_PKT_DSCOND_ID:
            /*DISCONNECTED*/
            RCVSNGLROW
            Decode_WRD_DISWRD(&hdr_row, &pkt_hdr->row_1.diswrd);
            if(pkt_hdr->phdr.hdrlen == 3) {
                RCVSNGLROW
                Decode_WRD_CONNID(&hdr_row, &pkt_hdr->row_2.connid);
            }
            break;
        case VLG_PKT_TXRQST_ID:
            /*TRANSACTION REQUEST*/
            RCVSNGLROW
            Decode_WRD_TXREQW(&hdr_row, &pkt_hdr->row_1.txreqw);
            RCVSNGLROW
            Decode_WRD_TXPLID(&hdr_row, &pkt_hdr->row_2.txplid);
            RCVSNGLROW
            Decode_WRD_TXSVID(&hdr_row, &pkt_hdr->row_3.txsvid);
            RCVSNGLROW
            Decode_WRD_TXCNID(&hdr_row, &pkt_hdr->row_4.txcnid);
            RCVSNGLROW
            Decode_WRD_TXPRID(&hdr_row, &pkt_hdr->row_5.txprid);
            RCVSNGLROW
            Decode_WRD_PKTLEN(&hdr_row, &pkt_hdr->row_6.pktlen);
            RCVSNGLROW
            Decode_WRD_CLSENC(&hdr_row, &pkt_hdr->row_7.clsenc);
            RCVSNGLROW
            Decode_WRD_CONNID(&hdr_row, &pkt_hdr->row_8.connid);
            pkt_hdr->bdy_bytelen = pkt_hdr->row_6.pktlen.pktlen - pkt_hdr->hdr_bytelen;
            break;
        case VLG_PKT_TXRESP_ID:
            /*TRANSACTION RESPONSE*/
            RCVSNGLROW
            Decode_WRD_TXRESW(&hdr_row, &pkt_hdr->row_1.txresw);
            RCVSNGLROW
            Decode_WRD_TXPLID(&hdr_row, &pkt_hdr->row_2.txplid);
            RCVSNGLROW
            Decode_WRD_TXSVID(&hdr_row, &pkt_hdr->row_3.txsvid);
            RCVSNGLROW
            Decode_WRD_TXCNID(&hdr_row, &pkt_hdr->row_4.txcnid);
            RCVSNGLROW
            Decode_WRD_TXPRID(&hdr_row, &pkt_hdr->row_5.txprid);
            if(pkt_hdr->phdr.hdrlen == 8) {
                RCVSNGLROW
                Decode_WRD_PKTLEN(&hdr_row, &pkt_hdr->row_6.pktlen);
                RCVSNGLROW
                Decode_WRD_CLSENC(&hdr_row, &pkt_hdr->row_7.clsenc);
                pkt_hdr->bdy_bytelen = pkt_hdr->row_6.pktlen.pktlen - pkt_hdr->hdr_bytelen;
            }
            break;
        case VLG_PKT_SBSREQ_ID:
            /*SUBSCRIPTION REQUEST*/
            RCVSNGLROW
            Decode_WRD_SBREQW(&hdr_row, &pkt_hdr->row_1.sbreqw);
            RCVSNGLROW
            Decode_WRD_CLSENC(&hdr_row, &pkt_hdr->row_2.clsenc);
            RCVSNGLROW
            Decode_WRD_CONNID(&hdr_row, &pkt_hdr->row_3.connid);
            RCVSNGLROW
            Decode_WRD_RQSTID(&hdr_row, &pkt_hdr->row_4.rqstid);
            if(pkt_hdr->phdr.hdrlen == 7) {
                RCVSNGLROW
                Decode_WRD_TMSTMP(&hdr_row, &pkt_hdr->row_5.tmstmp); //timestamp 0
                RCVSNGLROW
                Decode_WRD_TMSTMP(&hdr_row, &pkt_hdr->row_6.tmstmp); //timestamp 1
            }
            break;
        case VLG_PKT_SBSRES_ID:
            /*SUBSCRIPTION RESPONSE*/
            RCVSNGLROW
            Decode_WRD_SBRESW(&hdr_row, &pkt_hdr->row_1.sbresw);
            RCVSNGLROW
            Decode_WRD_RQSTID(&hdr_row, &pkt_hdr->row_2.rqstid);
            if(pkt_hdr->phdr.hdrlen == 4) {
                RCVSNGLROW
                Decode_WRD_SBSRID(&hdr_row, &pkt_hdr->row_3.sbsrid);
            }
            break;
        case VLG_PKT_SBSEVT_ID:
            /*SUBSCRIPTION EVENT*/
            RCVSNGLROW
            Decode_WRD_SBSRID(&hdr_row, &pkt_hdr->row_1.sbsrid);
            RCVSNGLROW
            Decode_WRD_SEVTTP(&hdr_row, &pkt_hdr->row_2.sevttp);
            RCVSNGLROW
            Decode_WRD_SEVTID(&hdr_row, &pkt_hdr->row_3.sevtid);
            RCVSNGLROW
            Decode_WRD_TMSTMP(&hdr_row, &pkt_hdr->row_4.tmstmp); //timestamp 0
            RCVSNGLROW
            Decode_WRD_TMSTMP(&hdr_row, &pkt_hdr->row_5.tmstmp); //timestamp 1
            if(pkt_hdr->phdr.hdrlen == 7) {
                RCVSNGLROW
                Decode_WRD_PKTLEN(&hdr_row, &pkt_hdr->row_6.pktlen);
                pkt_hdr->bdy_bytelen = pkt_hdr->row_6.pktlen.pktlen - pkt_hdr->hdr_bytelen;
            }
            break;
        case VLG_PKT_SBSACK_ID:
            /*SUBSCRIPTION EVENT ACK*/
            RCVSNGLROW
            Decode_WRD_SBSRID(&hdr_row, &pkt_hdr->row_1.sbsrid);
            RCVSNGLROW
            Decode_WRD_SEVTID(&hdr_row, &pkt_hdr->row_2.sevtid);
            break;
        case VLG_PKT_SBSTOP_ID:
            /*SUBSCRIPTION STOP REQUEST*/
            RCVSNGLROW
            Decode_WRD_SBSRID(&hdr_row, &pkt_hdr->row_1.sbsrid);
            break;
        case VLG_PKT_SBSSPR_ID:
            /*SUBSCRIPTION STOP RESPONSE*/
            RCVSNGLROW
            Decode_WRD_SBRESW(&hdr_row, &pkt_hdr->row_1.sbresw);
            if(pkt_hdr->phdr.hdrlen == 3) {
                RCVSNGLROW
                Decode_WRD_SBSRID(&hdr_row, &pkt_hdr->row_2.sbsrid);
            }
            break;
        default:
            return vlg::RetCode_DRPPKT;
    }
    return rcode;
}

}
