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

#if defined WIN32 && defined _MSC_VER
#include <WS2tcpip.h>
#endif
#ifdef __GNUG__
#include <unistd.h>
#include <fcntl.h>
#endif
#include "pr_impl.h"
#include "conn_impl.h"
#include "tx_impl.h"
#include "sbs_impl.h"

namespace vlg {

const std_shared_ptr_obj_mng<connection> conn_std_shp_omng;

}

namespace vlg {

//transaction_factory

incoming_transaction_factory *default_tx_factory = nullptr;
incoming_transaction_factory &incoming_transaction_factory::default_factory()
{
    if(default_tx_factory  == nullptr) {
        default_tx_factory  = new incoming_transaction_factory();
        if(!default_tx_factory) {
            EXIT_ACTION
        }
    }
    return *default_tx_factory;
}

incoming_transaction_factory::incoming_transaction_factory()
{}

incoming_transaction_factory::~incoming_transaction_factory()
{}

transaction &incoming_transaction_factory::make_incoming_transaction(connection &conn)
{
    return *new transaction();
}

}

namespace vlg {

//subscription_factory

incoming_subscription_factory *default_sbs_factory = nullptr;
incoming_subscription_factory &incoming_subscription_factory::default_factory()
{
    if(default_sbs_factory  == nullptr) {
        default_sbs_factory  = new incoming_subscription_factory();
        if(!default_sbs_factory) {
            EXIT_ACTION
        }
    }
    return *default_sbs_factory;
}

incoming_subscription_factory::incoming_subscription_factory()
{}

incoming_subscription_factory::~incoming_subscription_factory()
{}

subscription &incoming_subscription_factory::make_incoming_subscription(connection &conn)
{
    return *new subscription();
}

}

namespace vlg {

connection_impl::connection_impl(connection &publ) :
    peer_(nullptr),
    con_type_(ConnectionType_UNDEFINED),
    socket_(INVALID_SOCKET),
    last_socket_err_(0),
    con_evt_res_(ConnectivityEventResult_OK),
    connectivity_evt_type_(ConnectivityEventType_UNDEFINED),
    connid_(0),
    status_(ConnectionStatus_UNDEFINED),
    conres_(ConnectionResult_UNDEFINED),
    conrescode_(ProtocolCode_UNDEFINED),
    cli_agrhbt_(0),
    srv_agrhbt_(0),
    disconrescode_(ProtocolCode_UNDEFINED),
    connect_evt_occur_(false),
    pkt_sending_q_(vlg::sngl_ptr_obj_mng()),
    inco_flytx_map_(HMSz_1031, vlg::tx_std_shp_omng, sizeof(tx_id)),
    outg_flytx_map_(HMSz_1031, vlg::sngl_ptr_obj_mng(), sizeof(tx_id)),
    inco_nclassid_sbs_map_(HMSz_1031, vlg::sbs_std_shp_omng, sizeof(unsigned int)),
    inco_sbsid_sbs_map_(HMSz_1031, vlg::sbs_std_shp_omng, sizeof(unsigned int)),
    outg_reqid_sbs_map_(HMSz_23, vlg::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    outg_sbsid_sbs_map_(HMSz_1031, vlg::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    prid_(0),
    reqid_(0),
    sbsid_(0),
    publ_(publ),
    tx_factory_publ_(&incoming_transaction_factory::default_factory()),
    sbs_factory_publ_(&incoming_subscription_factory::default_factory())
{
    set_status(ConnectionStatus_INITIALIZED);
}

connection_impl::~connection_impl()
{
    if(status_ == ConnectionStatus_ESTABLISHED ||
            status_ == ConnectionStatus_PROTOCOL_HANDSHAKE ||
            status_ == ConnectionStatus_AUTHENTICATED ||
            status_ == ConnectionStatus_DISCONNECTING) {
        IFLOG(cri(TH_ID, LS_DTR
                  "[connection:%d is not in a safe state:%d] " LS_EXUNX,
                  __func__,
                  connid_,
                  status_))
    }
}

RetCode connection_impl::set_connection_established()
{
    return set_connection_established(socket_);
}

RetCode connection_impl::set_connection_established(SOCKET socket)
{
    socket_ = socket;
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    IFLOG(dbg(TH_ID,
              LS_CON"[connection established: socket:%d, host:%s, port:%d][connid:%d]",
              socket_,
              inet_ntoa(saddr.sin_addr),
              ntohs(saddr.sin_port),
              connid_))
    set_status(ConnectionStatus_ESTABLISHED);
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode connection_impl::set_status(ConnectionStatus status)
{
    IFLOG(trc(TH_ID, LS_OPN "[status:%d]", __func__, status))
    scoped_mx smx(mon_);
    status_ = status;
    publ_.on_status_change(status_);
    mon_.notify_all();
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode connection_impl::await_for_status_reached_or_outdated(ConnectionStatus test,
                                                              ConnectionStatus &current,
                                                              time_t sec,
                                                              long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(mon_);
    if(status_ < ConnectionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    while(status_ < test) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    current = status_;
    IFLOG(log(rcode ? vlg::TL_WRN : vlg::TL_DBG, TH_ID, LS_CLO "test:%d [reached or outdated] current:%d",
              __func__, test, status_))
    return rcode;
}

RetCode connection_impl::await_for_status_change(ConnectionStatus &status,
                                                 time_t sec,
                                                 long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(mon_);
    if(status_ < ConnectionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    while(status == status_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    IFLOG(log(rcode ? vlg::TL_WRN : vlg::TL_DBG, TH_ID, LS_CLO "status:%d [changed] current:%d", __func__,
              status,
              status_))
    status = status_;
    return rcode;
}

RetCode connection_impl::set_proto_connected()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(status_ != ConnectionStatus_ESTABLISHED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    set_status(ConnectionStatus_PROTOCOL_HANDSHAKE);
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode connection_impl::set_disconnecting()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    set_status(ConnectionStatus_DISCONNECTING);
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode connection_impl::set_socket_disconnected()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    IFLOG(inf(TH_ID, LS_CON"[connid:%d][disconnected]", connid_))
    set_status(ConnectionStatus_DISCONNECTED);
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode connection_impl::set_proto_error(RetCode cause_res)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(status_ == ConnectionStatus_DISCONNECTED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    set_status(ConnectionStatus_PROTOCOL_ERROR);
    IFLOG(err(TH_ID, LS_CLO "[res:%d, last_sock_err:%d]", __func__, cause_res, last_socket_err_))
    return vlg::RetCode_OK;
}

RetCode connection_impl::set_socket_error(RetCode cause_res)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(status_ < ConnectionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(err(TH_ID, LS_CLO "[res:%d, last_sock_err:%d]", __func__, cause_res, last_socket_err_))
    set_status(ConnectionStatus_SOCKET_ERROR);
    return vlg::RetCode_OK;
}

RetCode connection_impl::set_internal_error(RetCode cause_res)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(status_ < ConnectionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(cri(TH_ID, LS_CLO "[res:%d, last_sock_err:%d]", __func__, cause_res, last_socket_err_))
    set_status(ConnectionStatus_ERROR);
    return vlg::RetCode_OK;
}

RetCode connection_impl::set_appl_connected()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(status_ != ConnectionStatus_PROTOCOL_HANDSHAKE) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID, LS_CLO, __func__))
    set_status(ConnectionStatus_AUTHENTICATED);
    return vlg::RetCode_OK;
}

const char *connection_impl::get_host_ip() const
{
    if(socket_ == INVALID_SOCKET) {
        return "invalid address";
    }
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    return inet_ntoa(saddr.sin_addr);
}

unsigned short connection_impl::get_host_port() const
{
    if(socket_ == INVALID_SOCKET) {
        return 0;
    }
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    return ntohs(saddr.sin_port);
}

unsigned int connection_impl::next_prid()
{
    scoped_mx smx(mon_);
    return ++prid_;
}

unsigned int connection_impl::next_reqid()
{
    scoped_mx smx(mon_);
    return ++reqid_;
}

unsigned int connection_impl::next_sbsid()
{
    scoped_mx smx(mon_);
    return ++sbsid_;
}

// TCP/IP

RetCode connection_impl::set_socket_blocking_mode(bool blocking)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
#if defined WIN32 && defined _MSC_VER
    unsigned long mode = blocking ? 0 : 1;
    return (ioctlsocket(socket_, FIONBIO, &mode) == 0) ? vlg::RetCode_OK : vlg::RetCode_KO;
#else
    int flags = fcntl(socket_, F_GETFL, 0);
    if(flags < 0) {
        return vlg::RetCode_KO;
    }
    flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
    return (fcntl(socket_, F_SETFL, flags) == 0) ? vlg::RetCode_OK : vlg::RetCode_KO;
#endif
    IFLOG(trc(TH_ID, LS_CLO, __func__))
}

RetCode connection_impl::socket_excptn_hndl(long sock_op_res)
{
    RetCode rcode = vlg::RetCode_OK;
    if(sock_op_res == SOCKET_ERROR) {
#if defined WIN32 && defined _MSC_VER
        last_socket_err_ = WSAGetLastError();
#else
        last_socket_err_ = errno;
#endif
#if defined WIN32 && defined _MSC_VER
        if(last_socket_err_ == WSAEWOULDBLOCK) {
#else
        if(last_socket_err_ == EAGAIN) {
            return vlg::RetCode_SCKEAGN;
        } else if(last_socket_err_ == EWOULDBLOCK) {
#endif
            rcode = vlg::RetCode_SCKWBLK;
#if defined WIN32 && defined _MSC_VER
        } else if(last_socket_err_ == WSAECONNRESET) {
#else
        } else if(last_socket_err_ == ECONNRESET) {
#endif
            IFLOG(err(TH_ID, LS_CON"[connid:%d][socket:%d][connection reset by peer][err:%d]",
                      connid_,
                      socket_,
                      last_socket_err_))
            rcode = vlg::RetCode_SCKCLO;
        } else {
            perror(__func__);
            IFLOG(err(TH_ID, LS_CON"[connid:%d][socket:%d][connection socket error][errno:%d][err:%d]",
                      connid_,
                      socket_,
                      errno,
                      last_socket_err_))
            rcode = vlg::RetCode_SCKERR;
        }
    } else if(!sock_op_res) {
        /*typically we can arrive here on client applicative disconnections*/
        IFLOG(dbg(TH_ID, LS_CON"[connid:%d][socket:%d][connection socket was closed by peer]",
                  connid_,
                  socket_))
        rcode = vlg::RetCode_SCKCLO;
    } else {
        IFLOG(err(TH_ID, LS_CON "[connid:%d][socket:%d][connection unk. error]",
                  connid_,
                  socket_))
        rcode = vlg::RetCode_UNKERR;
    }
    return rcode;
}

RetCode connection_impl::establish_connection(sockaddr_in &params)
{
    IFLOG(dbg(TH_ID, LS_OPN "[host:%s - port:%d]",
              __func__,
              inet_ntoa(params.sin_addr),
              htons(params.sin_port)))
    RetCode rcode = vlg::RetCode_OK;
    int connect_res = 0;
    if((socket_ = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET) {
        IFLOG(trc(TH_ID, LS_TRL "[socket:%d][OK]", __func__, socket_))
        socklen_t len = sizeof(sockaddr_in);
        if((connect_res = connect(socket_, (struct sockaddr *)&params, len)) != INVALID_SOCKET) {
            IFLOG(dbg(TH_ID, LS_TRL "[socket:%d][connect OK]", __func__, socket_))
        } else {
#if defined WIN32 && defined _MSC_VER
            last_socket_err_ = WSAGetLastError();
#else
            last_socket_err_ = errno;
#endif
            IFLOG(dbg(TH_ID, LS_CLO "[connect KO][err:%d]", __func__, last_socket_err_))
            IFLOG(err(TH_ID, LS_CON "[connection failed][err:%d]", last_socket_err_))
            if((rcode = socket_shutdown())) {
                IFLOG(err(TH_ID, LS_CLO "[failed closing socket][err:%d]", __func__, rcode))
            }
        }
    } else {
#if defined WIN32 && defined _MSC_VER
        last_socket_err_ = WSAGetLastError();
#else
        last_socket_err_ = errno;
#endif
        IFLOG(err(TH_ID, LS_CLO "[socket KO][err:%d]", __func__, last_socket_err_))
    }
    if(!connect_res) {
        rcode = set_connection_established(socket_);
    } else {
        notify_for_connectivity_result(ConnectivityEventResult_KO, ConnectivityEventType_NETWORK);
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode connection_impl::send_single_pkt(vlg::g_bbuf *pkt_bbuf)
{
    std::string raw_pkt;
    IFLOG(trc(TH_ID, LS_OPN "[socket:%d, sndbuffsz:%u]",
              __func__,
              socket_,
              pkt_bbuf->limit()))
    if(!pkt_bbuf->limit() || !pkt_bbuf) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADARG;
    }
    if(v_log_ && v_log_->level() <= vlg::TL_TRC) {
        std::string tmp;
        v_log_->trc(TH_ID, LS_OUT  "%s", dump_raw_pkt(true,
                                                      pkt_bbuf->buffer(),
                                                      pkt_bbuf->limit(),
                                                      tmp));
    }
    RetCode rcode = vlg::RetCode_OK;
    bool stay = true;
    long bsent = 0, tot_bsent = 0, remaining = (long)pkt_bbuf->limit();
    while(stay) {
        while(remaining &&
                ((bsent = send(socket_,
                               &pkt_bbuf->buffer_as_char()[pkt_bbuf->position()],
                               (int)pkt_bbuf->limit(), 0)) > 0)) {
            pkt_bbuf->advance_pos_read(bsent);
            tot_bsent += bsent;
            remaining -= bsent;
            IFLOG(trc(TH_ID, LS_TRL "[sent:%d, tot_sent:%d, remaining:%d]",
                      __func__,
                      bsent,
                      tot_bsent,
                      remaining))
        }
        if(remaining) {
            if(((rcode = socket_excptn_hndl(bsent)) != vlg::RetCode_SCKEAGN) ||
                    (rcode != vlg::RetCode_SCKWBLK)) {
                rcode = vlg::RetCode_OK;
                stay = false;
            }
        } else {
            break;
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "[socket:%d, sent:%d, remaining:%d][res:%d]",
              __func__,
              socket_,
              tot_bsent,
              remaining,
              rcode))
    return rcode;
}

RetCode connection_impl::recv_body(unsigned int bodylen,
                                   vlg::g_bbuf *pkt_body)
{
    RetCode rcode = vlg::RetCode_OK;
    bool stay = true;
    unsigned int tot_brecv = 0;
    long brecv = 0, recv_buf_sz = bodylen;
    pkt_body->ensure_capacity(bodylen);
    while(stay) {
        while((tot_brecv < bodylen) && ((brecv = recv(socket_,
                                                      &pkt_body->buffer_as_char()[pkt_body->position()],
                                                      recv_buf_sz, 0)) > 0)) {
            pkt_body->move_pos_write(brecv);
            tot_brecv += brecv;
            recv_buf_sz -= brecv;
            IFLOG(trc(TH_ID, LS_TRL "[recv:%d, tot_recv:%u, recv_buf_sz:%d]",
                      __func__,
                      brecv,
                      tot_brecv,
                      recv_buf_sz))
        }
        if(tot_brecv != bodylen) {
            if((rcode = socket_excptn_hndl(brecv)) != vlg::RetCode_SCKEAGN) {
                rcode = vlg::RetCode_OK;
                stay = false;
            }
        } else {
            break;
        }
    }
    pkt_body->flip();
    return rcode;
}

RetCode connection_impl::recv_single_pkt(vlg_hdr_rec *pkt_hdr,
                                         vlg::g_bbuf *pkt_body)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    if(!pkt_hdr || !pkt_body) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADARG;
    }
    int tot_brecv = 0;
    RetCode rcode = vlg::RetCode_OK;
    //first phase: decode pkt header.
    switch((rcode = recv_and_decode_hdr(pkt_hdr))) {
        case vlg::RetCode_OK:
            break;
        case vlg::RetCode_SCKCLO:
            socket_shutdown();
            notify_disconnection(ConnectivityEventResult_OK, ConnectivityEventType_NETWORK);
            break;
        case vlg::RetCode_SCKERR:
            socket_shutdown();
            set_socket_error(rcode);
            notify_disconnection(ConnectivityEventResult_KO, ConnectivityEventType_NETWORK);
            break;
        case vlg::RetCode_SCKWBLK:
        case vlg::RetCode_DRPPKT:
            socket_shutdown();
            set_proto_error(rcode);
            notify_disconnection(ConnectivityEventResult_KO, ConnectivityEventType_PROTOCOL);
            break;
        default:
            socket_shutdown();
            set_internal_error(rcode);
            notify_disconnection(ConnectivityEventResult_KO, ConnectivityEventType_UNDEFINED);
            break;
    }
    //END first phase
    if(pkt_hdr->bdy_bytelen) {
        //second phase: read eventual remaining body.
        switch((rcode = recv_body(pkt_hdr->bdy_bytelen, pkt_body))) {
            case vlg::RetCode_OK:
                break;
            case vlg::RetCode_SCKCLO:
                socket_shutdown();
                notify_disconnection(ConnectivityEventResult_OK, ConnectivityEventType_NETWORK);
                break;
            case vlg::RetCode_SCKERR:
                socket_shutdown();
                set_socket_error(rcode);
                notify_disconnection(ConnectivityEventResult_KO, ConnectivityEventType_NETWORK);
                break;
            case vlg::RetCode_SCKWBLK:
            case vlg::RetCode_DRPPKT:
                socket_shutdown();
                set_proto_error(rcode);
                notify_disconnection(ConnectivityEventResult_KO, ConnectivityEventType_PROTOCOL);
                break;
            default:
                socket_shutdown();
                set_internal_error(rcode);
                notify_disconnection(ConnectivityEventResult_KO, ConnectivityEventType_UNDEFINED);
                break;
        }
        //END second phase
    }
    if(!rcode) {
        tot_brecv = pkt_hdr->hdr_bytelen + pkt_hdr->bdy_bytelen;
        if(v_log_ && v_log_->level() <= vlg::TL_TRC) {
            char dump_buf[DMP_OUT_BUF_LEN] = {0};
            dump_vlg_hdr_rec(pkt_hdr, dump_buf);
            v_log_->trc(TH_ID, LS_INC "%s", dump_buf);
        }
        if(pkt_hdr->bdy_bytelen && v_log_ && v_log_->level() <= vlg::TL_TRC) {
            std::string tmp;
            v_log_->trc(TH_ID, LS_INC "%s", dump_raw_pkt(false,
                                                         pkt_body->buffer(),
                                                         pkt_body->limit(),
                                                         tmp));
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "[tot-recv:%d, hdr_len:%d, body_len:%d][res:%d]",
              __func__,
              tot_brecv,
              pkt_hdr->hdr_bytelen,
              pkt_hdr->bdy_bytelen,
              rcode))
    return rcode;
}

RetCode connection_impl::socket_shutdown()
{
    IFLOG(trc(TH_ID, LS_OPN "[socket:%d]", __func__, socket_))
    int last_socket_err_ = 0;
#if defined WIN32 && defined _MSC_VER
    if((last_socket_err_ = closesocket(socket_))) {
        IFLOG(err(TH_ID, LS_TRL "[closesocket KO][res:%d]", __func__, socket_, last_socket_err_))
    } else {
        IFLOG(trc(TH_ID, LS_TRL "[closesocket OK]", __func__, socket_))
    }
#else
    if((last_socket_err_ = close(socket_))) {
        IFLOG(err(TH_ID, LS_TRL "[closesocket KO][res:%d]", __func__, socket_, last_socket_err_))
    } else {
        IFLOG(trc(TH_ID, LS_TRL "[closesocket OK]", __func__, socket_))
    }
#if 0
    if((last_socket_err_ = shutdown(socket_, SHUT_RDWR))) {
        IFLOG(err(TH_ID, LS_TRL "[closesocket KO][res:%d]", __func__, socket_, last_socket_err_))
    } else {
        IFLOG(trc(TH_ID, LS_TRL "[closesocket OK]", __func__, socket_))
    }
#endif
#endif
    set_socket_disconnected();
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

// CONNECTIVITY

RetCode connection_impl::client_connect(sockaddr_in &params)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    if(peer_->peer_status_ != PeerStatus_RUNNING) {
        IFLOG(err(TH_ID, LS_CLO "[invalid peer status][%d]", __func__, peer_->peer_status_))
        return vlg::RetCode_BADSTTS;
    }
    if(status_ != ConnectionStatus_INITIALIZED && status_ != ConnectionStatus_DISCONNECTED) {
        IFLOG(err(TH_ID, LS_CLO "[invalid connection status][%d]", __func__, status_))
        return vlg::RetCode_BADSTTS;
    }

    connect_evt_occur_ = false;
    g_bbuf *gbb = new g_bbuf(2);
    build_PKT_CONREQ(cli_agrhbt_, gbb);
    gbb->flip();
    RET_ON_KO(pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_ConnectRequest, &publ_);
    memcpy(&evt->saddr_, &params, sizeof(sockaddr_in));
    if((rcode = peer_->selector_.evt_enqueue_and_notify(evt))) {
        set_status(ConnectionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode connection_impl::server_send_connect_res(std::shared_ptr<vlg::connection> &inco_conn)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    if(status_ != ConnectionStatus_ESTABLISHED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    g_bbuf *gbb = new g_bbuf(3*4);
    build_PKT_CONRES(conres_,
                     conrescode_,
                     srv_agrhbt_,
                     connid_,
                     gbb);
    gbb->flip();
    RET_ON_KO(pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, inco_conn);
    if((rcode = peer_->selector_.evt_enqueue_and_notify(evt))) {
        set_status(ConnectionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode connection_impl::disconnect(ProtocolCode disres)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    if(status_ != ConnectionStatus_PROTOCOL_HANDSHAKE && status_ != ConnectionStatus_AUTHENTICATED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID, LS_CON"[connid:%d][socket:%d][sending disconnection][disconrescode:%d]", connid_, socket_, disres))
    set_disconnecting();
    g_bbuf *gbb = new g_bbuf(3);
    if(con_type_ == ConnectionType_OUTGOING) {
        build_PKT_DSCOND(disres, connid_, gbb);
    } else {
        build_PKT_DSCOND(disres, 0, gbb);
    }
    gbb->flip();
    RET_ON_KO(pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_Disconnect, &publ_);
    if((rcode = peer_->selector_.evt_enqueue_and_notify(evt))) {
        set_status(ConnectionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode connection_impl::await_for_connection_result(ConnectivityEventResult &con_evt_res,
                                                     ConnectivityEventType &connectivity_evt_type,
                                                     time_t sec,
                                                     long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(status_ < ConnectionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    scoped_mx smx(mon_);
    while(!connect_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    con_evt_res = con_evt_res_;
    connectivity_evt_type = connectivity_evt_type_;
    IFLOG(log(rcode ? vlg::TL_WRN : vlg::TL_DBG, TH_ID, LS_CLO
              "[res:%d, socket:%d, last_socket_err:%d, status:%d) - [connection result available] - con_evt_res:%d connectivity_evt_type:%d, conres:%d, resultcode:%d]",
              __func__,
              rcode,
              socket_,
              last_socket_err_,
              status_,
              con_evt_res_,
              connectivity_evt_type_,
              conres_,
              conrescode_))
    connect_evt_occur_ = false;
    return rcode;
}

RetCode connection_impl::notify_for_connectivity_result(ConnectivityEventResult con_evt_res,
                                                        ConnectivityEventType connectivity_evt_type)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    scoped_mx smx(mon_);
    connect_evt_occur_ = true;
    con_evt_res_ = con_evt_res;
    connectivity_evt_type_ = connectivity_evt_type;
    mon_.notify_all();
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode connection_impl::notify_connection(ConnectivityEventResult con_evt_res,
                                           ConnectivityEventType connectivity_evt_type)
{
    publ_.on_connect(con_evt_res, connectivity_evt_type);
    return notify_for_connectivity_result(con_evt_res, connectivity_evt_type);
}

RetCode connection_impl::notify_disconnection(ConnectivityEventResult con_evt_res,
                                              ConnectivityEventType connectivity_evt_type)
{
    publ_.on_disconnect(con_evt_res, connectivity_evt_type);
    return notify_for_connectivity_result(con_evt_res, connectivity_evt_type);
}

RetCode connection_impl::await_for_disconnection_result(ConnectivityEventResult &con_evt_res,
                                                        ConnectivityEventType &connectivity_evt_type,
                                                        time_t sec,
                                                        long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    scoped_mx smx(mon_);
    if(status_ < ConnectionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO, __func__))
        return vlg::RetCode_BADSTTS;
    }
    RetCode rcode = vlg::RetCode_OK;
    while(!connect_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  vlg::RetCode_TIMEOUT;
                break;
            }
        }
    }
    con_evt_res = con_evt_res_;
    connectivity_evt_type = connectivity_evt_type_;
    IFLOG(log(rcode ? vlg::TL_WRN : vlg::TL_DBG, TH_ID, LS_CLO
              "[connid:%d, res:%d, socket:%d, last_socket_err:%d, status:%d) - [disconnection result available] - con_evt_res:%d connectivity_evt_type:%d, conres:%d, resultcode:%d]",
              __func__,
              connid_,
              rcode,
              socket_,
              last_socket_err_,
              status_,
              con_evt_res_,
              connectivity_evt_type_,
              conres_,
              conrescode_))
    connect_evt_occur_ = false;
    return rcode;
}

RetCode connection_impl::recv_connection_request(const vlg_hdr_rec *pkt_hdr,
                                                 std::shared_ptr<vlg::connection> &inco_conn)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;

    if(status_ == ConnectionStatus_PROTOCOL_HANDSHAKE || status_ == ConnectionStatus_AUTHENTICATED) {
        set_disconnecting();
        conres_ = ConnectionResult_REFUSED;
        conrescode_ = ProtocolCode_ALREADY_CONNECTED;
        IFLOG(wrn(TH_ID, LS_CLO "[socket:%d][peer already connected - connid:%d]",
                  __func__,
                  socket_,
                  connid_))
        server_send_connect_res(inco_conn);
        return vlg::RetCode_KO;
    }

    if(status_ != ConnectionStatus_ESTABLISHED) {
        set_disconnecting();
        conres_ = ConnectionResult_REFUSED;
        conrescode_ = ProtocolCode_INVALID_CONNECTION_STATUS;
        IFLOG(err(TH_ID, LS_CLO "[socket:%d][invalid connection status - connid:%d]",
                  __func__,
                  socket_,
                  connid_))
        server_send_connect_res(inco_conn);
        return vlg::RetCode_KO;
    }

    conres_ = ConnectionResult_ACCEPTED;
    conrescode_ = ProtocolCode_SUCCESS;
    srv_agrhbt_ = pkt_hdr->row_1.clihbt.hbtsec;
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);

    if(!(rcode = peer_->publ_.accept_connection(*inco_conn))) {
        if((rcode = server_send_connect_res(inco_conn))) {
            set_internal_error(rcode);
            IFLOG(err(TH_ID, LS_CON"[error responding to peer: socket:%d, host:%s, port:%d]",
                      socket_,
                      inet_ntoa(saddr.sin_addr),
                      ntohs(saddr.sin_port)))
        } else {
            set_proto_connected();
            publ_.on_connect(ConnectivityEventResult_OK,
                             ConnectivityEventType_PROTOCOL);
            IFLOG(inf(TH_ID, LS_CON"[peer: socket:%d, host:%s, port:%d is now connected with connid:%d]",
                      socket_,
                      inet_ntoa(saddr.sin_addr),
                      ntohs(saddr.sin_port),
                      connid_))
        }
    } else {
        set_disconnecting();
        conres_ = ConnectionResult_REFUSED;
        conrescode_ = ProtocolCode_APPLICATIVE_REJECT;
        server_send_connect_res(inco_conn);
        IFLOG(inf(TH_ID,
                  LS_CON"[peer: socket:%d, host:%s, port:%d peer applicatively reject new connection]",
                  socket_,
                  inet_ntoa(saddr.sin_addr),
                  ntohs(saddr.sin_port),
                  connid_))
    }
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return rcode;
}

RetCode connection_impl::recv_connection_response(const vlg_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;
    ConnectivityEventResult con_evt_res = ConnectivityEventResult_OK;
    conres_ = pkt_hdr->row_1.srvcrs.conres;
    conrescode_ = pkt_hdr->row_1.srvcrs.errcod;
    srv_agrhbt_ = pkt_hdr->row_1.srvcrs.agrhbt;
    connid_ = pkt_hdr->row_2.connid.connid;
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_Undef, &publ_);
    switch(conres_) {
        case ConnectionResult_ACCEPTED:
            IFLOG(inf(TH_ID, LS_CON"[connection accepted by peer][connid:%d][socket:%d]", connid_, socket_))
            set_proto_connected();
            evt->evt_ = VLG_SELECTOR_Evt_ConnReqAccepted;
            break;
        case ConnectionResult_CONDITIONALLY_ACCEPTED:
            IFLOG(inf(TH_ID, LS_CON"[connection accepted by peer - with reserve][connid:%d][socket:%d]", connid_, socket_))
            set_proto_connected();
            evt->evt_ = VLG_SELECTOR_Evt_ConnReqAccepted;
            break;
        case ConnectionResult_REFUSED:
            IFLOG(wrn(TH_ID, LS_CON"[connection refused by peer]"))
            evt->evt_ = VLG_SELECTOR_Evt_ConnReqRefused;
            con_evt_res = ConnectivityEventResult_KO;
            break;
        default:
            IFLOG(err(TH_ID, LS_CON"[protocol error]"))
            evt->evt_ = VLG_SELECTOR_Evt_ConnReqRefused;
            con_evt_res = ConnectivityEventResult_KO;
            break;
    }
    if((rcode = peer_->selector_.evt_enqueue_and_notify(evt))) {
        set_internal_error(rcode);
        return rcode;
    }
    publ_.on_connect(con_evt_res, ConnectivityEventType_PROTOCOL);
    notify_for_connectivity_result(con_evt_res, ConnectivityEventType_PROTOCOL);
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode connection_impl::recv_disconnection(const vlg_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    disconrescode_ = pkt_hdr->row_1.diswrd.disres;
    IFLOG(inf(TH_ID, LS_CON"[connid:%d][socket:%d][received disconnection - disconrescode:%d]",
              connid_,
              socket_,
              disconrescode_))
    set_disconnecting();
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

RetCode connection_impl::recv_test_request(const vlg_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(pkt_hdr:%p)", __func__, pkt_hdr))
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_UNSP;
}

// TRANSACTIONAL

RetCode connection_impl::next_tx_id(tx_id &txid)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    txid.txplid = peer_->peer_plid_;
    txid.txsvid = peer_->peer_svid_;
    txid.txcnid = connid_;
    txid.txprid = next_prid();
    IFLOG(trc(TH_ID, LS_CLO "generated txid: [%08x%08x%08x%08x]",
              __func__,
              txid.txplid,
              txid.txsvid,
              txid.txcnid,
              txid.txprid))
    return vlg::RetCode_OK;
}

RetCode connection_impl::new_incoming_transaction(std::shared_ptr<vlg::transaction> &incoming_transaction,
                                                  std::shared_ptr<vlg::connection> &inco_conn,
                                                  bool compute_txid)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    transaction &publ = tx_factory_publ_->make_incoming_transaction(publ_);
    publ.impl_->set_connection_sh(inco_conn);
    incoming_transaction.reset(&publ);
    if(compute_txid) {
        next_tx_id((*incoming_transaction->impl_).txid_);
    }
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

/*only server*/
RetCode connection_impl::recv_tx_request(const vlg_hdr_rec *pkt_hdr,
                                         vlg::g_bbuf *pkt_body,
                                         std::shared_ptr<vlg::connection> &inco_conn)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;
    bool skip_appl_mng = false;
    bool aborted = false;
    std::shared_ptr<vlg::transaction> trans;
    new_incoming_transaction(trans, inco_conn, false);
    vlg::transaction_impl *timpl = trans->impl_.get();

    timpl->txtype_ = pkt_hdr->row_1.txreqw.txtype;
    timpl->txactn_ = pkt_hdr->row_1.txreqw.txactn;
    timpl->rsclrq_ = pkt_hdr->row_1.txreqw.rsclrq;
    timpl->txid_.txplid = pkt_hdr->row_2.txplid.txplid;
    timpl->txid_.txsvid = pkt_hdr->row_3.txsvid.txsvid;
    timpl->txid_.txcnid = pkt_hdr->row_4.txcnid.txcnid;
    timpl->txid_.txprid = pkt_hdr->row_5.txprid.txprid;
    vlg::rt_mark_time(&trans->impl_->start_mark_tim_);

    IFLOG(inf(TH_ID, LS_TRX"[%08x%08x%08x%08x][TXTYPE:%d, TXACT:%d, RSCLREQ:%d]",
              pkt_hdr->row_2.txplid.txplid,
              pkt_hdr->row_3.txsvid.txsvid,
              pkt_hdr->row_4.txcnid.txcnid,
              pkt_hdr->row_5.txprid.txprid,
              pkt_hdr->row_1.txresw.txresl,
              pkt_hdr->row_1.txresw.vlgcod,
              pkt_hdr->row_1.txresw.rescls))

    if(!inco_flytx_map_.contains_key(&timpl->txid_)) {
        timpl->tx_res_ = TransactionResult_FAILED;
        timpl->result_code_ = ProtocolCode_TRANSACTION_ALREADY_FLYING;
        timpl->rescls_ = false;
        aborted = true;
        IFLOG(err(TH_ID, LS_TRX"[same tx already flying]"))
    } else {
        inco_flytx_map_.put(&timpl->txid_, &trans);
    }

    if((rcode = publ_.on_incoming_transaction(trans))) {
        IFLOG(inf(TH_ID, LS_CON"[connection:%d applicatively refused new transaction]", connid_))
        timpl->tx_res_ = TransactionResult_FAILED;
        timpl->result_code_ = ProtocolCode_TRANSACTION_SERVER_ABORT;
        timpl->rescls_ = false;
        skip_appl_mng = true;
    }

    if(!aborted) {
        timpl->set_flying();
        if(pkt_hdr->phdr.hdrlen == 9) {
            timpl->req_nclassid_ = pkt_hdr->row_7.clsenc.nclsid;
            timpl->req_clsenc_ = pkt_hdr->row_7.clsenc.enctyp;
            timpl->res_clsenc_ = pkt_hdr->row_7.clsenc.enctyp;
            nclass *req_obj = nullptr;
            if((rcode = peer_->nem_.new_nclass_instance(timpl->req_nclassid_, &req_obj))) {
                timpl->tx_res_ = TransactionResult_FAILED;
                timpl->result_code_ = ProtocolCode_MALFORMED_REQUEST;
                timpl->rescls_ = false;
                skip_appl_mng = true;
                IFLOG(err(TH_ID, LS_TRX"[tx request receive failed - new_nclass_instance:%d, nclass_id:%d]",
                          rcode, timpl->req_nclassid_))
            } else {
                timpl->set_request_obj_on_request(*req_obj);
                if((rcode = req_obj->restore(&peer_->nem_, timpl->req_clsenc_, pkt_body))) {
                    timpl->tx_res_ = TransactionResult_FAILED;
                    timpl->result_code_ = ProtocolCode_MALFORMED_REQUEST;
                    timpl->rescls_ = false;
                    skip_appl_mng = true;
                    IFLOG(err(TH_ID, LS_TRX"[tx request receive failed - nclass restore fail:%d, nclass_id:%d]",
                              rcode, timpl->req_nclassid_))
                }
            }
        }
        if(!skip_appl_mng) {
            trans->on_request();
        }
    }

    if((rcode = timpl->send_response())) {
        IFLOG(err(TH_ID, LS_TRX"[tx response sending failed res:%d]", rcode))
    }

    if((rcode = inco_flytx_map_.remove(&timpl->txid_, nullptr))) {
        IFLOG(cri(TH_ID, LS_TRX"[error removing tx from flying map - res:%d]", rcode))
    } else {
        IFLOG(trc(TH_ID, LS_TRX"[tx removed from flying map]"))
    }

    if(aborted) {
        timpl->set_aborted();
    } else {
        timpl->set_closed();
    }

    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode connection_impl::recv_tx_response(const vlg_hdr_rec *pkt_hdr,
                                          vlg::g_bbuf *pkt_body)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;
    bool aborted = false;
    tx_id txid;
    txid.txplid = pkt_hdr->row_2.txplid.txplid;
    txid.txsvid = pkt_hdr->row_3.txsvid.txsvid;
    txid.txcnid = pkt_hdr->row_4.txcnid.txcnid;
    txid.txprid = pkt_hdr->row_5.txprid.txprid;

    IFLOG(dbg(TH_ID, LS_INC"[%08x%08x%08x%08x][TXRES:%d, TXRESCODE:%d, RESCLS:%d] ",
              txid.txplid,
              txid.txsvid,
              txid.txcnid,
              txid.txprid,
              pkt_hdr->row_1.txresw.txresl,
              pkt_hdr->row_1.txresw.vlgcod,
              pkt_hdr->row_1.txresw.rescls))

    transaction_impl *trans = nullptr;
    if((rcode = outg_flytx_map_.get(&txid, &trans))) {
        IFLOG(err(TH_ID, LS_CLO"[tx not found, aborting]", __func__))
        return vlg::RetCode_ABORT;
    }

    trans->tx_res_ = pkt_hdr->row_1.txresw.txresl;
    trans->result_code_ = pkt_hdr->row_1.txresw.vlgcod;
    trans->rescls_ = pkt_hdr->row_1.txresw.rescls;
    if(pkt_hdr->phdr.hdrlen == 8) {
        trans->res_nclassid_ = pkt_hdr->row_7.clsenc.nclsid;
        trans->res_clsenc_ = pkt_hdr->row_7.clsenc.enctyp;
        nclass *nobj = nullptr;
        if((rcode = peer_->nem_.new_nclass_instance(trans->res_nclassid_, &nobj))) {
            IFLOG(err(TH_ID, LS_TRX"[tx response receive failed - new_nclass_instance:%d, nclass_id:%d]",
                      rcode,
                      trans->res_nclassid_))
            aborted = true;
        }

        trans->set_result_obj_on_response(*nobj);

        if((rcode = nobj->restore(&peer_->nem_, trans->res_clsenc_, pkt_body))) {
            IFLOG(err(TH_ID, LS_TRX"[tx response receive failed - nclass restore fail:%d, nclass_id:%d]",
                      rcode,
                      trans->res_nclassid_))
            aborted = true;
        }
    }

    if((rcode = outg_flytx_map_.remove(&trans->txid_, nullptr))) {
        IFLOG(cri(TH_ID, LS_TRX"[error removing tx from flying map - res:%d]", rcode))
    } else {
        IFLOG(trc(TH_ID, LS_TRX"[tx removed from flying map]"))
    }

    if(aborted) {
        trans->set_aborted();
    } else {
        trans->set_closed();
    }

    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

// SUBSCRIPTION
RetCode connection_impl::new_incoming_subscription(std::shared_ptr<vlg::subscription> &incoming_sbs,
                                                   std::shared_ptr<vlg::connection> &inco_conn)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    subscription &publ = sbs_factory_publ_->make_incoming_subscription(publ_);
    publ.impl_->set_connection_sh(inco_conn);
    incoming_sbs.reset(&publ);
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return vlg::RetCode_OK;
}

//client only
RetCode connection_impl::detach_subscription(subscription_impl *subscription)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    if(subscription->conn_->impl_.get() != this) {
        IFLOG(err(TH_ID, LS_CLO "[connid:%d][subscription:%d is not mine]", __func__,
                  connid_,
                  subscription->sbsid_))
        return vlg::RetCode_KO;
    }
    if(subscription->status_ == SubscriptionStatus_REQUEST_SENT ||
            subscription->status_ == SubscriptionStatus_STARTED ||
            subscription->status_ == SubscriptionStatus_RELEASED) {
        IFLOG(wrn(TH_ID, LS_CLO"[connid:%d][subscription:%d is not in a valid state for detaching:%d]",
                  __func__,
                  connid_,
                  subscription->sbsid_,
                  subscription->status_))
        return vlg::RetCode_BADSTTS;
    }
    if(con_type_ == ConnectionType_INGOING) {
        IFLOG(err(TH_ID, LS_CLO "[connid:%d][invalid subscription type]", __func__,
                  connid_,
                  subscription->sbsid_))
        return vlg::RetCode_KO;
    } else {
        outg_reqid_sbs_map_.remove(&subscription->reqid_, nullptr);
        outg_sbsid_sbs_map_.remove(&subscription->sbsid_, nullptr);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode connection_impl::release_subscription(subscription_impl *subscription)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    if(subscription->conn_->impl_.get() != this) {
        IFLOG(err(TH_ID, LS_CLO "[connid:%d][subscription:%d is not mine]", __func__,
                  connid_,
                  subscription->sbsid_))
        return vlg::RetCode_KO;
    }
    if(subscription->status_ == SubscriptionStatus_REQUEST_SENT ||
            subscription->status_ == SubscriptionStatus_STARTED ||
            subscription->status_ == SubscriptionStatus_RELEASED) {
        IFLOG(wrn(TH_ID, LS_CLO"[connid:%d][subscription:%d is not in a valid state for releasing:%d]",
                  __func__,
                  connid_,
                  subscription->sbsid_,
                  subscription->status_))
        return vlg::RetCode_BADSTTS;
    }
    if(con_type_ == ConnectionType_INGOING) {
        subscription->release_initial_query();
        peer_->remove_subscriber(subscription);
        inco_sbsid_sbs_map_.remove(&subscription->sbsid_, nullptr);
        inco_nclassid_sbs_map_.remove(&subscription->nclassid_, nullptr);
        subscription->set_released();
    } else {
        outg_reqid_sbs_map_.remove(&subscription->reqid_, nullptr);
        outg_sbsid_sbs_map_.remove(&subscription->sbsid_, nullptr);
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

/*server only*/
RetCode connection_impl::recv_sbs_start_request(const vlg_hdr_rec *pkt_hdr,
                                                std::shared_ptr<vlg::connection> &inco_conn)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;
    unsigned int sbsid = next_sbsid();

    std::shared_ptr<vlg::subscription> sbs_sh;
    new_incoming_subscription(sbs_sh, inco_conn);
    vlg::subscription_impl *inc_sbs = sbs_sh->impl_.get();

    inc_sbs->sbstyp_ = pkt_hdr->row_1.sbreqw.sbstyp;
    inc_sbs->sbsmod_ = pkt_hdr->row_1.sbreqw.sbsmod;
    inc_sbs->flotyp_ = pkt_hdr->row_1.sbreqw.flotyp;
    inc_sbs->dwltyp_ = pkt_hdr->row_1.sbreqw.dwltyp;
    inc_sbs->enctyp_ = pkt_hdr->row_2.clsenc.enctyp;
    inc_sbs->nclassid_ = pkt_hdr->row_2.clsenc.nclsid;
    inc_sbs->reqid_ = pkt_hdr->row_4.rqstid.rqstid;
    if(pkt_hdr->phdr.hdrlen == 7) {
        inc_sbs->open_tmstp0_ = pkt_hdr->row_5.tmstmp.tmstmp;
        inc_sbs->open_tmstp1_ = pkt_hdr->row_6.tmstmp.tmstmp;
    }

    IFLOG(inf(TH_ID,
              LS_SBI"[CONNID:%010u-REQID:%010u][SBSTYP:%d, SBSMOD:%d, FLOTYP:%d, DWLTYP:%d, ENCTYP:%d, NCLSSID:%d, TMSTP0:%u, TMSTP1:%u]",
              connid_,
              inc_sbs->reqid_,
              inc_sbs->sbstyp_,
              inc_sbs->sbsmod_,
              inc_sbs->flotyp_,
              inc_sbs->dwltyp_,
              inc_sbs->enctyp_,
              inc_sbs->nclassid_,
              inc_sbs->open_tmstp0_,
              inc_sbs->open_tmstp1_))

    nentity_desc const *edesc = peer_->nem_.get_nentity_descriptor(inc_sbs->nclassid_);
    if(!edesc) {
        inc_sbs->sbresl_ = SubscriptionResponse_KO;
        inc_sbs->last_vlgcod_ = ProtocolCode_UNSUPPORTED_REQUEST;
        IFLOG(err(TH_ID, LS_SBS"[unsupported nclass_id requested in subscription: %u]", inc_sbs->nclassid_))
    } else {
        if(!(rcode = inco_nclassid_sbs_map_.contains_key(&inc_sbs->nclassid_))) {
            inc_sbs->sbresl_ = SubscriptionResponse_KO;
            inc_sbs->last_vlgcod_ = ProtocolCode_SUBSCRIPTION_ALREADY_STARTED;
            IFLOG(err(TH_ID, LS_SBS"[subscription on this connection:%d already started for nclass_id:%d]", connid_,
                      inc_sbs->nclassid_))
        } else {
            if((rcode = inco_sbsid_sbs_map_.put(&sbsid, &sbs_sh))) {
                inc_sbs->sbresl_ = SubscriptionResponse_KO;
                inc_sbs->last_vlgcod_ = ProtocolCode_SERVER_ERROR;
                IFLOG(cri(TH_ID, LS_SBS"[error putting subscription into sbsid map - res:%d]", rcode))
            }
            if((rcode = inco_nclassid_sbs_map_.put(&inc_sbs->nclassid_, &sbs_sh))) {
                inc_sbs->sbresl_ = SubscriptionResponse_KO;
                inc_sbs->last_vlgcod_ = ProtocolCode_SERVER_ERROR;
                IFLOG(cri(TH_ID, LS_SBS"[error putting subscription into nclass_id map - res:%d]", rcode))
            }
            inc_sbs->sbsid_ = sbsid;
            inc_sbs->sbresl_ = SubscriptionResponse_OK;
            inc_sbs->last_vlgcod_ = ProtocolCode_SUCCESS;
        }
    }

    if((rcode = publ_.on_incoming_subscription(sbs_sh))) {
        IFLOG(inf(TH_ID, LS_CON"[connection:%d applicatively refused new subscription]", connid_))
        inc_sbs->sbresl_ = SubscriptionResponse_KO;
        inc_sbs->last_vlgcod_ = ProtocolCode_SERVER_ERROR;
    }

    if((rcode = inc_sbs->send_start_response())) {
        IFLOG(err(TH_ID, LS_SBS"[subscription response sending failed][res:%d]", rcode))
    }

    if(inc_sbs->sbresl_ == SubscriptionResponse_OK || inc_sbs->sbresl_ == SubscriptionResponse_PARTIAL) {
        if((rcode = peer_->add_subscriber(inc_sbs))) {
            IFLOG(cri(TH_ID, LS_SBS"[error binding subscription to peer][res:%d]", rcode))
            inc_sbs->set_error();
        } else {
            if(inc_sbs->sbsmod_ == SubscriptionMode_ALL || inc_sbs->sbsmod_ == SubscriptionMode_DOWNLOAD) {
                //do initial query.
                if(!(rcode = inc_sbs->execute_initial_query())) {
                    inc_sbs->initial_query_ended_ = false;
                } else {
                    inc_sbs->initial_query_ended_ = true;
                }
            }
            inc_sbs->set_started();
            if(!inc_sbs->initial_query_ended_) {
                inc_sbs->safe_submit_dwnl_event();
            }
        }
    }

    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return rcode;
}

/*client only*/
RetCode connection_impl::recv_sbs_start_response(const vlg_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;
    unsigned int sbsid = ((pkt_hdr->row_1.sbresw.sbresl == SubscriptionResponse_OK) ||
                          (pkt_hdr->row_1.sbresw.sbresl == SubscriptionResponse_PARTIAL)) ?
                         pkt_hdr->row_3.sbsrid.sbsrid : 0;
    IFLOG(inf(TH_ID,
              LS_SBI"[CONNID:%010u-REQID:%010u][SBSRES:%d, VLGCOD:%d, SBSID:%u]",
              connid_,
              pkt_hdr->row_2.rqstid.rqstid,
              pkt_hdr->row_1.sbresw.sbresl,
              pkt_hdr->row_1.sbresw.vlgcod,
              sbsid))

    subscription_impl *subscription = nullptr;
    if((rcode = outg_reqid_sbs_map_.remove(&pkt_hdr->row_2.rqstid.rqstid, &subscription))) {
        IFLOG(err(TH_ID, LS_SBS"[subscription not found, aborting.]"))
        return vlg::RetCode_KO;
    }

    subscription->sbsid_ = sbsid;
    subscription->sbresl_ = pkt_hdr->row_1.sbresw.sbresl;
    subscription->last_vlgcod_ = pkt_hdr->row_1.sbresw.vlgcod;
    outg_sbsid_sbs_map_.put(&sbsid, &subscription);

    if(!sbsid) {
        subscription->set_stopped();
    } else {
        subscription->set_started();
    }

    subscription->notify_for_start_stop_result();
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return rcode;
}

RetCode connection_impl::recv_sbs_evt(const vlg_hdr_rec *pkt_hdr,
                                      vlg::g_bbuf *pkt_body)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;
    bool mng = true;
    IFLOG(dbg(TH_ID, LS_SBS
              "[CONNID:%010u-SBSID:%010u][EVTID:%d, EVTTYP:%d, PRTCOD:%d, TMSTMP[0]:%d, TMSTMP[1]:%d, ACT:%d]",
              connid_,
              pkt_hdr->row_1.sbsrid.sbsrid,
              pkt_hdr->row_3.sevtid.sevtid,
              pkt_hdr->row_2.sevttp.sevttp,
              pkt_hdr->row_2.sevttp.vlgcod,
              pkt_hdr->row_4.tmstmp.tmstmp,
              pkt_hdr->row_5.tmstmp.tmstmp,
              pkt_hdr->row_2.sevttp.sbeact))
    subscription_impl *subscription = nullptr;
    if((rcode = outg_sbsid_sbs_map_.get(&pkt_hdr->row_1.sbsrid.sbsrid, &subscription))) {
        IFLOG(cri(TH_ID, LS_SBS"[error getting subscription from sbsid map][res:%d]", rcode))
        mng = false;
    } else {
        if(mng && (rcode = subscription->receive_event(pkt_hdr, pkt_body))) {
            IFLOG(wrn(TH_ID, LS_SBS"[subscription event:%d management failed][res:%d]",
                      pkt_hdr->row_3.sevtid.sevtid,
                      rcode))
        }
    }
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return rcode;
}

//srv
RetCode connection_impl::recv_sbs_evt_ack(const vlg_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;
    std::shared_ptr<vlg::subscription> sbs_sh;
    if((rcode = inco_sbsid_sbs_map_.get(&pkt_hdr->row_1.sbsrid.sbsrid, &sbs_sh))) {
        IFLOG(cri(TH_ID, LS_SBS"[error on subscription event ack getting subscription from sbsid map][res:%d]", rcode))
    } else {
        if((rcode = sbs_sh->impl_->receive_event_ack(pkt_hdr))) {
            IFLOG(wrn(TH_ID, LS_SBS"[error on subscription event ack, management failed][res:%d]", rcode))
        }
    }
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return rcode;
}

//srv
RetCode connection_impl::recv_sbs_stop_request(const vlg_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;
    unsigned int sbsid = pkt_hdr->row_1.sbsrid.sbsrid;
    IFLOG(inf(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][stop request]", connid_, sbsid))
    SubscriptionResponse sbresl = SubscriptionResponse_OK;
    ProtocolCode protocode = ProtocolCode_SUCCESS;
    std::shared_ptr<vlg::subscription> sbs_sh;
    if((rcode = inco_sbsid_sbs_map_.get(&sbsid, &sbs_sh))) {
        IFLOG(err(TH_ID, LS_SBS"[error on subscription stop getting subscription from sbsid map][res:%d]", rcode))
        sbresl = SubscriptionResponse_KO;
        protocode = ProtocolCode_SUBSCRIPTION_NOT_FOUND;
    } else {
        sbs_sh->impl_->set_stopped();
        release_subscription(sbs_sh->impl_.get());
    }
    g_bbuf *gbb = new g_bbuf();
    build_PKT_SBSSPR(sbresl,
                     protocode,
                     sbsid,
                     gbb);
    gbb->flip();
    RET_ON_KO(pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(VLG_SELECTOR_Evt_SendPacket, sbs_sh->impl_->conn_sh_);
    rcode = peer_->selector_.evt_enqueue_and_notify(evt);
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

//cli
RetCode connection_impl::recv_sbs_stop_response(const vlg_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = vlg::RetCode_OK;
    IFLOG(inf(TH_ID, LS_INC LS_SBS"[CONNID:%010u-SBSID:%010u][SBSRES:%d, VLGCOD:%d]",
              connid_,
              pkt_hdr->row_2.sbsrid.sbsrid,
              pkt_hdr->row_1.sbresw.sbresl,
              pkt_hdr->row_1.sbresw.vlgcod))
    subscription_impl *subscription = nullptr;
    if((rcode = outg_sbsid_sbs_map_.get(&pkt_hdr->row_2.sbsrid.sbsrid, &subscription))) {
        IFLOG(cri(TH_ID, LS_SBS"[error getting subscription from sbsid map][res:%d]", rcode))
    } else {
        subscription->notify_for_start_stop_result();
        if(pkt_hdr->row_1.sbresw.sbresl == SubscriptionResponse_OK) {
            subscription->set_stopped();
            detach_subscription(subscription);
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

}
