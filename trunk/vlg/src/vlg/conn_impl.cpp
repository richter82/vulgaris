/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "pr_impl.h"
#include "conn_impl.h"
#include "tx_impl.h"
#include "sbs_impl.h"

namespace vlg {
const std_shared_ptr_obj_mng<incoming_connection> conn_std_shp_omng;
}

namespace vlg {

//transaction_factory

incoming_transaction_factory *default_tx_factory = nullptr;
incoming_transaction_factory &incoming_transaction_factory::default_factory()
{
    if(default_tx_factory  == nullptr) {
        default_tx_factory  = new incoming_transaction_factory();
    }
    return *default_tx_factory;
}

incoming_transaction_factory::incoming_transaction_factory()
{}

incoming_transaction_factory::~incoming_transaction_factory()
{}

incoming_transaction &incoming_transaction_factory::make_incoming_transaction(std::shared_ptr<incoming_connection> &c)
{
    return *new incoming_transaction(c);
}

}

namespace vlg {

//subscription_factory

incoming_subscription_factory *default_sbs_factory = nullptr;
incoming_subscription_factory &incoming_subscription_factory::default_factory()
{
    if(default_sbs_factory  == nullptr) {
        default_sbs_factory  = new incoming_subscription_factory();
    }
    return *default_sbs_factory;
}

incoming_subscription_factory::incoming_subscription_factory()
{}

incoming_subscription_factory::~incoming_subscription_factory()
{}

incoming_subscription &incoming_subscription_factory::make_incoming_subscription(std::shared_ptr<incoming_connection>
                                                                                 &c)
{
    return *new incoming_subscription(c);
}

}

namespace vlg {

conn_impl::conn_impl(incoming_connection &ipubl,
                     peer &p,
                     incoming_connection_listener &listener) :
    con_type_(ConnectionType_INGOING),
    socket_(INVALID_SOCKET),
    last_socket_err_(0),
    con_evt_res_(ConnectivityEventResult_OK),
    connectivity_evt_type_(ConnectivityEventType_UNDEFINED),
    connid_(0),
    status_(ConnectionStatus_INITIALIZED),
    conres_(ConnectionResult_UNDEFINED),
    conrescode_(ProtocolCode_UNDEFINED),
    cli_agrhbt_(0),
    srv_agrhbt_(0),
    disconrescode_(ProtocolCode_UNDEFINED),
    connect_evt_occur_(false),
    pkt_ch_st_(PktChasingStatus_HDRLen),
    rdn_buff_(RCV_SND_BUF_SZ),
    pkt_sending_q_(conn_pkt_up_omng),
    acc_snd_buff_(RCV_SND_BUF_SZ),
    ipubl_(&ipubl),
    opubl_(nullptr),
    ilistener_(&listener),
    olistener_(nullptr),
    peer_(p.impl_.get())
{}

conn_impl::conn_impl(outgoing_connection &opubl,
                     outgoing_connection_listener &listener) :
    con_type_(ConnectionType_OUTGOING),
    socket_(INVALID_SOCKET),
    last_socket_err_(0),
    con_evt_res_(ConnectivityEventResult_OK),
    connectivity_evt_type_(ConnectivityEventType_UNDEFINED),
    connid_(0),
    status_(ConnectionStatus_INITIALIZED),
    conres_(ConnectionResult_UNDEFINED),
    conrescode_(ProtocolCode_UNDEFINED),
    cli_agrhbt_(0),
    srv_agrhbt_(0),
    disconrescode_(ProtocolCode_UNDEFINED),
    connect_evt_occur_(false),
    pkt_ch_st_(PktChasingStatus_HDRLen),
    rdn_buff_(RCV_SND_BUF_SZ),
    pkt_sending_q_(conn_pkt_up_omng),
    acc_snd_buff_(RCV_SND_BUF_SZ),
    ipubl_(nullptr),
    opubl_(&opubl),
    ilistener_(nullptr),
    olistener_(&listener),
    peer_(nullptr)
{}

RetCode conn_impl::set_connection_established()
{
    return set_connection_established(socket_);
}

RetCode conn_impl::set_connection_established(SOCKET socket)
{
    socket_ = socket;
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    IFLOG(peer_->log_, dbg(TH_ID,
                           LS_CON"[connection established: socket:%d, host:%s, port:%d][connid:%d]",
                           socket_,
                           inet_ntoa(saddr.sin_addr),
                           ntohs(saddr.sin_port),
                           connid_))
    set_status(ConnectionStatus_ESTABLISHED);
    return RetCode_OK;
}

RetCode conn_impl::set_status(ConnectionStatus status)
{
    IFLOG(peer_->log_, trc(TH_ID, LS_OPN "[status:%d]", __func__, status))
    scoped_mx smx(mon_);
    status_ = status;
    if(con_type_ == ConnectionType_INGOING) {
        ilistener_->on_status_change(*ipubl_, status_);
    } else {
        olistener_->on_status_change(*opubl_, status_);
    }
    mon_.notify_all();
    return RetCode_OK;
}

RetCode conn_impl::await_for_status_reached(ConnectionStatus test,
                                            ConnectionStatus &current,
                                            time_t sec,
                                            long nsec)
{
    scoped_mx smx(mon_);
    if(status_ < ConnectionStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(status_ < test) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  RetCode_TIMEOUT;
                break;
            }
        }
    }
    current = status_;
    IFLOG(peer_->log_, log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO "test:%d [reached] current:%d", __func__, test, status_))
    return rcode;
}

RetCode conn_impl::await_for_status_change(ConnectionStatus &status,
                                           time_t sec,
                                           long nsec)
{
    scoped_mx smx(mon_);
    if(status_ < ConnectionStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(status == status_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  RetCode_TIMEOUT;
                break;
            }
        }
    }
    IFLOG(peer_->log_, log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO "status:%d [changed] current:%d", __func__,
                           status,
                           status_))
    status = status_;
    return rcode;
}

RetCode conn_impl::set_proto_connected()
{
    if(status_ != ConnectionStatus_ESTABLISHED) {
        return RetCode_BADSTTS;
    }
    set_status(ConnectionStatus_PROTOCOL_HANDSHAKE);
    return RetCode_OK;
}

RetCode conn_impl::set_disconnecting()
{
    set_status(ConnectionStatus_DISCONNECTING);
    return RetCode_OK;
}

RetCode conn_impl::set_socket_disconnected()
{
    IFLOG(peer_->log_, inf(TH_ID, LS_CON"[connid:%d][disconnected]", connid_))
    set_status(ConnectionStatus_DISCONNECTED);
    return RetCode_OK;
}

RetCode conn_impl::set_appl_connected()
{
    if(status_ != ConnectionStatus_PROTOCOL_HANDSHAKE) {
        return RetCode_BADSTTS;
    }
    set_status(ConnectionStatus_AUTHENTICATED);
    return RetCode_OK;
}

const char *conn_impl::get_host_ip() const
{
    if(socket_ == INVALID_SOCKET) {
        return "invalid address";
    }
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    return inet_ntoa(saddr.sin_addr);
}

unsigned short conn_impl::get_host_port() const
{
    if(socket_ == INVALID_SOCKET) {
        return 0;
    }
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    return ntohs(saddr.sin_port);
}

RetCode conn_impl::close_connection(ConnectivityEventResult cer,
                                    ConnectivityEventType cet)
{
    socket_shutdown();
    clean_rdn_rep();
    release_all_children();
    notify_disconnection(cer, cet);
    return RetCode_OK;
}

RetCode conn_impl::set_socket_blocking_mode(bool blocking)
{
#if defined WIN32 && defined _MSC_VER
    unsigned long mode = blocking ? 0 : 1;
    return (ioctlsocket(socket_, FIONBIO, &mode) == 0) ? RetCode_OK : RetCode_KO;
#else
    int flags = fcntl(socket_, F_GETFL, 0);
    if(flags < 0) {
        return RetCode_KO;
    }
    flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
    return (fcntl(socket_, F_SETFL, flags) == 0) ? RetCode_OK : RetCode_KO;
#endif
}

RetCode conn_impl::socket_shutdown()
{
    IFLOG(peer_->log_, trc(TH_ID, LS_OPN "[socket:%d]", __func__, socket_))
    int last_socket_err_ = 0;
#if defined WIN32 && defined _MSC_VER
    if((last_socket_err_ = closesocket(socket_))) {
        IFLOG(peer_->log_, err(TH_ID, LS_TRL "[closesocket KO][res:%d]", __func__, socket_, last_socket_err_))
    } else {
        IFLOG(peer_->log_, trc(TH_ID, LS_TRL "[closesocket OK]", __func__, socket_))
    }
#else
    if((last_socket_err_ = close(socket_))) {
        IFLOG(peer_->log_, err(TH_ID, LS_TRL "[closesocket KO][res:%d]", __func__, socket_, last_socket_err_))
    } else {
        IFLOG(peer_->log_, trc(TH_ID, LS_TRL "[closesocket OK]", __func__, socket_))
    }
#if 0
    if((last_socket_err_ = shutdown(socket_, SHUT_RDWR))) {
        IFLOG(peer_->log_, err(TH_ID, LS_TRL "[closesocket KO][res:%d]", __func__, socket_, last_socket_err_))
    } else {
        IFLOG(peer_->log_, trc(TH_ID, LS_TRL "[closesocket OK]", __func__, socket_))
    }
#endif
#endif
    set_socket_disconnected();
    return RetCode_OK;
}

RetCode conn_impl::establish_connection(sockaddr_in &params)
{
    IFLOG(peer_->log_, dbg(TH_ID, LS_OPN "[host:%s - port:%d]",
                           __func__,
                           inet_ntoa(params.sin_addr),
                           htons(params.sin_port)))
    RetCode rcode = RetCode_OK;
    int connect_res = 0;
    if((socket_ = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET) {
        IFLOG(peer_->log_, trc(TH_ID, LS_TRL "[socket:%d][OK]", __func__, socket_))
        socklen_t len = sizeof(sockaddr_in);
        if((connect_res = connect(socket_, (struct sockaddr *)&params, len)) != INVALID_SOCKET) {
            IFLOG(peer_->log_, dbg(TH_ID, LS_TRL "[socket:%d][connect OK]", __func__, socket_))
        } else {
#if defined WIN32 && defined _MSC_VER
            last_socket_err_ = WSAGetLastError();
#else
            last_socket_err_ = errno;
#endif
            IFLOG(peer_->log_, err(TH_ID, LS_CON "[connection failed][err:%d]", last_socket_err_))
            if((rcode = socket_shutdown())) {
                IFLOG(peer_->log_, err(TH_ID, LS_CLO "[failed closing socket][err:%d]", __func__, rcode))
            }
        }
    } else {
#if defined WIN32 && defined _MSC_VER
        last_socket_err_ = WSAGetLastError();
#else
        last_socket_err_ = errno;
#endif
        IFLOG(peer_->log_, err(TH_ID, LS_CLO "[socket KO][err:%d]", __func__, last_socket_err_))
    }
    if(!connect_res) {
        rcode = set_connection_established(socket_);
    } else {
        notify_for_connectivity_result(ConnectivityEventResult_KO, ConnectivityEventType_NETWORK);
        rcode = RetCode_KO;
    }
    return rcode;
}

RetCode conn_impl::disconnect(ProtocolCode disres)
{
    if(status_ != ConnectionStatus_PROTOCOL_HANDSHAKE && status_ != ConnectionStatus_AUTHENTICATED) {
        return RetCode_BADSTTS;
    }
    IFLOG(peer_->log_, inf(TH_ID, LS_CON"[connid:%d][socket:%d][sending disconnection][disconrescode:%d]", connid_, socket_,
                           disres))
    set_disconnecting();

    g_bbuf gbb;
    if(con_type_ == ConnectionType_OUTGOING) {
        build_PKT_DSCOND(disres, connid_, &gbb);
    } else {
        build_PKT_DSCOND(disres, 0, &gbb);
    }

    std::unique_ptr<conn_pkt> cpkt(new conn_pkt(nullptr, std::move(gbb)));
    pkt_sending_q_.put(&cpkt);
    sel_evt *evt = new sel_evt(VLG_SELECTOR_Evt_Disconnect, this);
    return peer_->selector_.notify(evt);
}

RetCode conn_impl::notify_for_connectivity_result(ConnectivityEventResult con_evt_res,
                                                  ConnectivityEventType connectivity_evt_type)
{
    scoped_mx smx(mon_);
    connect_evt_occur_ = true;
    con_evt_res_ = con_evt_res;
    connectivity_evt_type_ = connectivity_evt_type;
    mon_.notify_all();
    return RetCode_OK;
}

RetCode conn_impl::notify_disconnection(ConnectivityEventResult con_evt_res,
                                        ConnectivityEventType connectivity_evt_type)
{
    if(con_type_ == ConnectionType_INGOING) {
        ilistener_->on_disconnect(*ipubl_, con_evt_res, connectivity_evt_type);
    } else {
        olistener_->on_disconnect(*opubl_, con_evt_res, connectivity_evt_type);
    }
    return notify_for_connectivity_result(con_evt_res, connectivity_evt_type);
}

RetCode conn_impl::await_for_disconnection_result(ConnectivityEventResult &con_evt_res,
                                                  ConnectivityEventType &connectivity_evt_type,
                                                  time_t sec,
                                                  long nsec)
{
    IFLOG(peer_->log_, trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    scoped_mx smx(mon_);
    if(status_ < ConnectionStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    while(!connect_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  RetCode_TIMEOUT;
                break;
            }
        }
    }
    con_evt_res = con_evt_res_;
    connectivity_evt_type = connectivity_evt_type_;
    IFLOG(peer_->log_, log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO
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

// ****************************************************************************
// CONNECTION SEND/RECV METHS
// ****************************************************************************

RetCode conn_impl::sckt_hndl_err(long sock_op_res)
{
    RetCode rcode = RetCode_OK;
    if(sock_op_res == SOCKET_ERROR) {
#if defined WIN32 && defined _MSC_VER
        last_socket_err_ = WSAGetLastError();
#else
        last_socket_err_ = errno;
#endif
#if defined WIN32 && defined _MSC_VER
        if(last_socket_err_ == WSAEWOULDBLOCK) {
#else
        if(last_socket_err_ == EAGAIN || last_socket_err_ == EWOULDBLOCK) {
#endif
            rcode = RetCode_SCKWBLK;
#if defined WIN32 && defined _MSC_VER
        } else if(last_socket_err_ == WSAECONNRESET) {
#else
        } else if(last_socket_err_ == ECONNRESET) {
#endif
            IFLOG(peer_->log_, err(TH_ID, LS_CON"[connid:%d][socket:%d][connection reset by peer][err:%d]",
                                   connid_,
                                   socket_,
                                   last_socket_err_))
            rcode = RetCode_SCKCLO;
        } else {
            perror(__func__);
            IFLOG(peer_->log_, err(TH_ID, LS_CON"[connid:%d][socket:%d][connection socket error][errno:%d][err:%d]",
                                   connid_,
                                   socket_,
                                   errno,
                                   last_socket_err_))
            rcode = RetCode_SCKERR;
        }
    } else if(!sock_op_res) {
        /*typically we can arrive here on client applicative disconnections*/
        IFLOG(peer_->log_, dbg(TH_ID, LS_CON"[connid:%d][socket:%d][connection socket was closed by peer]",
                               connid_,
                               socket_))
        rcode = RetCode_SCKCLO;
    } else {
        IFLOG(peer_->log_, err(TH_ID, LS_CON "[connid:%d][socket:%d][connection unk. error]",
                               connid_,
                               socket_))
        rcode = RetCode_UNKERR;
    }

    switch((rcode)) {
        case RetCode_SCKCLO:
            close_connection(ConnectivityEventResult_OK, ConnectivityEventType_NETWORK);
            break;
        case RetCode_SCKERR:
        case RetCode_UNKERR:
            close_connection(ConnectivityEventResult_KO, ConnectivityEventType_NETWORK);
            break;
        default:
            break;
    }
    return rcode;
}

RetCode conn_impl::recv_bytes()
{
    RetCode rcode = RetCode_OK;
    rdn_buff_.set_write();
    long brecv = 0, buf_rem_len = (long)rdn_buff_.remaining();
    while(buf_rem_len && ((brecv = recv(socket_,
                                        &rdn_buff_.buf_[rdn_buff_.pos_],
                                        buf_rem_len, 0)) > 0)) {
        rdn_buff_.move_pos_write(brecv);
        buf_rem_len -= brecv;
    }
    if(brecv<=0) {
        rcode = sckt_hndl_err(brecv);
    }
    return rcode;
}

RetCode conn_impl::chase_pkt()
{
    RetCode rcode = RetCode_PARTPKT;
    bool pkt_rdy = false, stay = true;
    rdn_buff_.set_read();
    while(stay && rdn_buff_.available_read() && !pkt_rdy) {
        switch(pkt_ch_st_) {
            case PktChasingStatus_HDRLen:
                if(rdn_buff_.available_read() >= WORD_SZ) {
                    unsigned int hdr_row = 0;
                    rdn_buff_.read_uint(&hdr_row);
                    Decode_WRD_PKTHDR(&hdr_row, &curr_rdn_hdr_.phdr);
                    curr_rdn_hdr_.hdr_bytelen = curr_rdn_hdr_.phdr.hdrlen * WORD_SZ;
                    pkt_ch_st_ = PktChasingStatus_HDR;
                } else {
                    stay = false;
                }
                break;
            case PktChasingStatus_HDR:
                if(rdn_buff_.available_read() >= (curr_rdn_hdr_.hdr_bytelen - WORD_SZ)) {
                    RetCode dres = read_decode_hdr();
                    if(dres == RetCode_MALFORM) {
                        set_disconnecting();
                        close_connection(ConnectivityEventResult_KO, ConnectivityEventType_PROTOCOL);
                        stay = false;
                    }
                    if(curr_rdn_hdr_.bdy_bytelen) {
                        pkt_ch_st_ = PktChasingStatus_Body;
                    } else {
                        pkt_ch_st_ = PktChasingStatus_HDRLen;
                        pkt_rdy = true;
                    }
                } else {
                    stay = false;
                }
                break;
            case PktChasingStatus_Body:
                if(rdn_buff_.available_read()) {
                    if(!curr_rdn_body_) {
                        curr_rdn_body_.reset(new g_bbuf(curr_rdn_hdr_.bdy_bytelen));
                    }
                    rdn_buff_.read(std::min(curr_rdn_body_->remaining(), rdn_buff_.available_read()), *curr_rdn_body_);
                    if(curr_rdn_body_->remaining()) {
                        stay = false;
                    } else {
                        curr_rdn_body_->set_read();
                        pkt_ch_st_ = PktChasingStatus_HDRLen;
                        pkt_rdy = true;
                    }
                } else {
                    stay = false;
                }
                break;
            default:
                break;
        }
    }
    size_t avl_rd = rdn_buff_.available_read();
    if(avl_rd) {
        rdn_buff_.set_mark();
        if(avl_rd < VLG_MAX_HDR_SZ) {
            rdn_buff_.compact();
        }
    } else {
        rdn_buff_.reset();
    }
    return pkt_rdy ? RetCode_OK : rcode;
}

RetCode conn_impl::read_decode_hdr()
{
    RetCode rcode = RetCode_OK;
    unsigned int hdr_row = 0;
    switch(curr_rdn_hdr_.phdr.pkttyp) {
        case VLG_PKT_TSTREQ_ID:
            /*TEST REQUEST*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TMSTMP(&hdr_row, &curr_rdn_hdr_.row_1.tmstmp);
            if(curr_rdn_hdr_.phdr.hdrlen == 3) {
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_CONNID(&hdr_row, &curr_rdn_hdr_.row_2.connid);
            }
            break;
        case VLG_PKT_HRTBET_ID:
            /*HEARTBEAT*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TMSTMP(&hdr_row, &curr_rdn_hdr_.row_1.tmstmp);
            if(curr_rdn_hdr_.phdr.hdrlen == 3) {
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_CONNID(&hdr_row, &curr_rdn_hdr_.row_2.connid);
            }
            break;
        case VLG_PKT_CONREQ_ID:
            /*CONNECTION REQUEST*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_CLIHBT(&hdr_row, &curr_rdn_hdr_.row_1.clihbt);
            break;
        case VLG_PKT_CONRES_ID:
            /*CONNECTION RESPONSE*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SRVCRS(&hdr_row, &curr_rdn_hdr_.row_1.srvcrs);
            if(curr_rdn_hdr_.phdr.hdrlen == 3) {
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_CONNID(&hdr_row, &curr_rdn_hdr_.row_2.connid);
            }
            break;
        case VLG_PKT_DSCOND_ID:
            /*DISCONNECTED*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_DISWRD(&hdr_row, &curr_rdn_hdr_.row_1.diswrd);
            if(curr_rdn_hdr_.phdr.hdrlen == 3) {
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_CONNID(&hdr_row, &curr_rdn_hdr_.row_2.connid);
            }
            break;
        case VLG_PKT_TXRQST_ID:
            /*TRANSACTION REQUEST*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXREQW(&hdr_row, &curr_rdn_hdr_.row_1.txreqw);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXPLID(&hdr_row, &curr_rdn_hdr_.row_2.txplid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXSVID(&hdr_row, &curr_rdn_hdr_.row_3.txsvid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXCNID(&hdr_row, &curr_rdn_hdr_.row_4.txcnid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXPRID(&hdr_row, &curr_rdn_hdr_.row_5.txprid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_PKTLEN(&hdr_row, &curr_rdn_hdr_.row_6.pktlen);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_CLSENC(&hdr_row, &curr_rdn_hdr_.row_7.clsenc);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_CONNID(&hdr_row, &curr_rdn_hdr_.row_8.connid);
            curr_rdn_hdr_.bdy_bytelen = curr_rdn_hdr_.row_6.pktlen.pktlen - curr_rdn_hdr_.hdr_bytelen;
            break;
        case VLG_PKT_TXRESP_ID:
            /*TRANSACTION RESPONSE*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXRESW(&hdr_row, &curr_rdn_hdr_.row_1.txresw);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXPLID(&hdr_row, &curr_rdn_hdr_.row_2.txplid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXSVID(&hdr_row, &curr_rdn_hdr_.row_3.txsvid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXCNID(&hdr_row, &curr_rdn_hdr_.row_4.txcnid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TXPRID(&hdr_row, &curr_rdn_hdr_.row_5.txprid);
            if(curr_rdn_hdr_.phdr.hdrlen == 8) {
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_PKTLEN(&hdr_row, &curr_rdn_hdr_.row_6.pktlen);
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_CLSENC(&hdr_row, &curr_rdn_hdr_.row_7.clsenc);
                curr_rdn_hdr_.bdy_bytelen = curr_rdn_hdr_.row_6.pktlen.pktlen - curr_rdn_hdr_.hdr_bytelen;
            }
            break;
        case VLG_PKT_SBSREQ_ID:
            /*SUBSCRIPTION REQUEST*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SBREQW(&hdr_row, &curr_rdn_hdr_.row_1.sbreqw);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_CLSENC(&hdr_row, &curr_rdn_hdr_.row_2.clsenc);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_CONNID(&hdr_row, &curr_rdn_hdr_.row_3.connid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_RQSTID(&hdr_row, &curr_rdn_hdr_.row_4.rqstid);
            if(curr_rdn_hdr_.phdr.hdrlen == 7) {
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_TMSTMP(&hdr_row, &curr_rdn_hdr_.row_5.tmstmp); //timestamp 0
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_TMSTMP(&hdr_row, &curr_rdn_hdr_.row_6.tmstmp); //timestamp 1
            }
            break;
        case VLG_PKT_SBSRES_ID:
            /*SUBSCRIPTION RESPONSE*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SBRESW(&hdr_row, &curr_rdn_hdr_.row_1.sbresw);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_RQSTID(&hdr_row, &curr_rdn_hdr_.row_2.rqstid);
            if(curr_rdn_hdr_.phdr.hdrlen == 4) {
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_SBSRID(&hdr_row, &curr_rdn_hdr_.row_3.sbsrid);
            }
            break;
        case VLG_PKT_SBSEVT_ID:
            /*SUBSCRIPTION EVENT*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SBSRID(&hdr_row, &curr_rdn_hdr_.row_1.sbsrid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SEVTTP(&hdr_row, &curr_rdn_hdr_.row_2.sevttp);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SEVTID(&hdr_row, &curr_rdn_hdr_.row_3.sevtid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TMSTMP(&hdr_row, &curr_rdn_hdr_.row_4.tmstmp); //timestamp 0
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_TMSTMP(&hdr_row, &curr_rdn_hdr_.row_5.tmstmp); //timestamp 1
            if(curr_rdn_hdr_.phdr.hdrlen == 7) {
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_PKTLEN(&hdr_row, &curr_rdn_hdr_.row_6.pktlen);
                curr_rdn_hdr_.bdy_bytelen = curr_rdn_hdr_.row_6.pktlen.pktlen - curr_rdn_hdr_.hdr_bytelen;
            }
            break;
        case VLG_PKT_SBSACK_ID:
            /*SUBSCRIPTION EVENT ACK*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SBSRID(&hdr_row, &curr_rdn_hdr_.row_1.sbsrid);
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SEVTID(&hdr_row, &curr_rdn_hdr_.row_2.sevtid);
            break;
        case VLG_PKT_SBSTOP_ID:
            /*SUBSCRIPTION STOP REQUEST*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SBSRID(&hdr_row, &curr_rdn_hdr_.row_1.sbsrid);
            break;
        case VLG_PKT_SBSSPR_ID:
            /*SUBSCRIPTION STOP RESPONSE*/
            rdn_buff_.read_uint(&hdr_row);
            Decode_WRD_SBRESW(&hdr_row, &curr_rdn_hdr_.row_1.sbresw);
            if(curr_rdn_hdr_.phdr.hdrlen == 3) {
                rdn_buff_.read_uint(&hdr_row);
                Decode_WRD_SBSRID(&hdr_row, &curr_rdn_hdr_.row_2.sbsrid);
            }
            break;
        default:
            return RetCode_MALFORM;
    }
    return rcode;
}

void conn_impl::clean_rdn_rep()
{
    pkt_ch_st_ = PktChasingStatus_HDRLen;
    rdn_buff_.reset();
    curr_rdn_hdr_.reset();
    curr_rdn_body_.release();
}

RetCode conn_impl::send_acc_buff()
{
    acc_snd_buff_.set_read();
    if(!acc_snd_buff_.limit_) {
        return RetCode_BADARG;
    }
    RetCode rcode = RetCode_OK;
    bool stay = true;
    long bsent = 0, tot_bsent = 0, remaining = (long)acc_snd_buff_.available_read();
    while(stay) {
        while(remaining && ((bsent = send(socket_,
                                          &acc_snd_buff_.buf_[acc_snd_buff_.pos_],
                                          (int)remaining, 0)) > 0)) {
            acc_snd_buff_.advance_pos_read(bsent);
            tot_bsent += bsent;
            remaining -= bsent;
        }
        if(remaining) {
            if((rcode = sckt_hndl_err(bsent)) == RetCode_SCKWBLK) {
                acc_snd_buff_.set_mark();
            }
            stay = false;
            break;
        } else {
            acc_snd_buff_.reset();
            stay = false;
            break;
        }
    }
    if(peer_->log_ && peer_->log_->level() <= TL_TRC) {

        peer_->log_->trace(LS_CLO "[socket:%d, sent:%d, remaining:%d][res:%d]",
                           __func__,
                           socket_,
                           tot_bsent,
                           remaining,
                           rcode);
    }
    return rcode;
}

RetCode conn_impl::aggr_msgs_and_send_pkt()
{
    RetCode rcode = RetCode_OK;
    /*  1
        try to fill accumulating buffer with current packet and queued messages.
    */
    do {
        acc_snd_buff_.set_read();
        while(acc_snd_buff_.available_read() < acc_snd_buff_.capacity()) {
            if(cpkt_ && cpkt_->pkt_b_.available_read()) {
                acc_snd_buff_.set_write();
                if(!acc_snd_buff_.append_no_rsz(cpkt_->pkt_b_)) {
                    //accumulating buffer filled.
                    break;
                } else {
                    //current packet has completely read, it can be replaced.
                }
            } else {
                if(pkt_sending_q_.size()) {
                    pkt_sending_q_.take(&cpkt_);
                    cpkt_->pkt_b_.set_read();
                } else {
                    //current packet read and empty queue
                    break;
                }
            }
        }
        /*  2
            send accumulating buffer
        */
    } while(!(rcode = send_acc_buff()) && (cpkt_->pkt_b_.available_read() || pkt_sending_q_.size()));
    return rcode;
}

}

namespace vlg {

incoming_connection_impl::incoming_connection_impl(incoming_connection &publ,
                                                   peer &p,
                                                   incoming_connection_listener &listener) :
    conn_impl(publ, p, listener),
    inco_flytx_map_(HMSz_1031, tx_std_shp_omng, sizeof(tx_id)),
    inco_nclassid_sbs_map_(HMSz_1031, sbs_std_shp_omng, sizeof(unsigned int)),
    inco_sbsid_sbs_map_(HMSz_1031, sbs_std_shp_omng, sizeof(unsigned int)),
    sbsid_(0),
    tx_factory_publ_(&incoming_transaction_factory::default_factory()),
    sbs_factory_publ_(&incoming_subscription_factory::default_factory())
{}

incoming_connection_impl::~incoming_connection_impl()
{
    if(status_ == ConnectionStatus_ESTABLISHED ||
            status_ == ConnectionStatus_PROTOCOL_HANDSHAKE ||
            status_ == ConnectionStatus_AUTHENTICATED ||
            status_ == ConnectionStatus_DISCONNECTING) {
        IFLOG(peer_->log_, cri(TH_ID, LS_DTR
                               "[connection:%d is not in a safe state:%d] " LS_EXUNX,
                               __func__,
                               connid_,
                               status_))
    }
}

static void(release_all_inco_sbs)(const s_hm &map,
                                  const void *key,
                                  void *ptr,
                                  void *ud)
{
    incoming_connection_impl *iconn = (incoming_connection_impl *)ud;
    std::shared_ptr<incoming_subscription> *isbs_sh = (std::shared_ptr<incoming_subscription> *)ptr;
    iconn->release_subscription(isbs_sh->get()->impl_.get());
}

void incoming_connection_impl::release_all_children()
{
    inco_sbsid_sbs_map_.enum_elements_safe_write(release_all_inco_sbs, this);
    inco_sbsid_sbs_map_.clear();
    inco_nclassid_sbs_map_.clear();
}

unsigned int incoming_connection_impl::next_sbsid()
{
    scoped_mx smx(mon_);
    return ++sbsid_;
}

RetCode incoming_connection_impl::server_send_connect_res(std::shared_ptr<incoming_connection> &inco_conn)
{
    if(status_ != ConnectionStatus_ESTABLISHED) {
        return RetCode_BADSTTS;
    }

    g_bbuf gbb;
    build_PKT_CONRES(conres_,
                     conrescode_,
                     srv_agrhbt_,
                     connid_,
                     &gbb);

    std::unique_ptr<conn_pkt> cpkt(new conn_pkt(nullptr, std::move(gbb)));
    pkt_sending_q_.put(&cpkt);
    sel_evt *evt = new sel_evt(VLG_SELECTOR_Evt_SendPacket, inco_conn);
    return peer_->selector_.notify(evt);
}

RetCode incoming_connection_impl::recv_connection_request(const vlg_hdr_rec *pkt_hdr,
                                                          std::shared_ptr<incoming_connection> &inco_conn)
{
    IFLOG(peer_->log_, trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    RetCode rcode = RetCode_OK;

    if(status_ == ConnectionStatus_PROTOCOL_HANDSHAKE || status_ == ConnectionStatus_AUTHENTICATED) {
        set_disconnecting();
        conres_ = ConnectionResult_REFUSED;
        conrescode_ = ProtocolCode_ALREADY_CONNECTED;
        IFLOG(peer_->log_, wrn(TH_ID, LS_CLO "[socket:%d][peer already connected - connid:%d]",
                               __func__,
                               socket_,
                               connid_))
        server_send_connect_res(inco_conn);
        return RetCode_KO;
    }

    if(status_ != ConnectionStatus_ESTABLISHED) {
        set_disconnecting();
        conres_ = ConnectionResult_REFUSED;
        conrescode_ = ProtocolCode_INVALID_CONNECTION_STATUS;
        IFLOG(peer_->log_, err(TH_ID, LS_CLO "[socket:%d][invalid connection status - connid:%d]",
                               __func__,
                               socket_,
                               connid_))
        server_send_connect_res(inco_conn);
        return RetCode_KO;
    }

    conres_ = ConnectionResult_ACCEPTED;
    conrescode_ = ProtocolCode_SUCCESS;
    srv_agrhbt_ = pkt_hdr->row_1.clihbt.hbtsec;
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);

    if(!(rcode = peer_->listener_.on_incoming_connection(peer_->publ_, inco_conn))) {
        if((rcode = server_send_connect_res(inco_conn))) {
            set_disconnecting();
            IFLOG(peer_->log_, err(TH_ID, LS_CON"[error responding to peer: socket:%d, host:%s, port:%d]",
                                   socket_,
                                   inet_ntoa(saddr.sin_addr),
                                   ntohs(saddr.sin_port)))
        } else {
            set_proto_connected();
            IFLOG(peer_->log_, inf(TH_ID, LS_CON"[peer: socket:%d, host:%s, port:%d is now connected with connid:%d]",
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
        IFLOG(peer_->log_, inf(TH_ID,
                               LS_CON"[peer: socket:%d, host:%s, port:%d peer applicatively reject new connection]",
                               socket_,
                               inet_ntoa(saddr.sin_addr),
                               ntohs(saddr.sin_port),
                               connid_))
    }
    return rcode;
}

RetCode incoming_connection_impl::recv_disconnection(const vlg_hdr_rec *pkt_hdr)
{
    IFLOG(peer_->log_, trc(TH_ID, LS_OPN "[connid:%d]", __func__, connid_))
    disconrescode_ = pkt_hdr->row_1.diswrd.disres;
    IFLOG(peer_->log_, inf(TH_ID, LS_CON"[connid:%d][socket:%d][received disconnection - disconrescode:%d]",
                           connid_,
                           socket_,
                           disconrescode_))
    set_disconnecting();
    return RetCode_OK;
}

RetCode incoming_connection_impl::recv_test_request(const vlg_hdr_rec *pkt_hdr)
{
    return RetCode_UNSP;
}

RetCode incoming_connection_impl::new_incoming_transaction(std::shared_ptr<incoming_transaction> &inco_tx,
                                                           std::shared_ptr<incoming_connection> &inco_conn)
{
    incoming_transaction &publ = tx_factory_publ_->make_incoming_transaction(inco_conn);
    inco_tx.reset(&publ);
    return RetCode_OK;
}

RetCode incoming_connection_impl::recv_tx_request(const vlg_hdr_rec *pkt_hdr,
                                                  g_bbuf *pkt_body,
                                                  std::shared_ptr<incoming_connection> &inco_conn)
{
    RetCode rcode = RetCode_OK;
    bool skip_appl_mng = false;
    bool aborted = false;
    std::shared_ptr<incoming_transaction> trans;
    new_incoming_transaction(trans, inco_conn);
    incoming_transaction_impl *timpl = trans->impl_.get();

    timpl->txtype_ = pkt_hdr->row_1.txreqw.txtype;
    timpl->txactn_ = pkt_hdr->row_1.txreqw.txactn;
    timpl->rsclrq_ = pkt_hdr->row_1.txreqw.rsclrq;
    timpl->txid_.txplid = pkt_hdr->row_2.txplid.txplid;
    timpl->txid_.txsvid = pkt_hdr->row_3.txsvid.txsvid;
    timpl->txid_.txcnid = pkt_hdr->row_4.txcnid.txcnid;
    timpl->txid_.txprid = pkt_hdr->row_5.txprid.txprid;
    rt_mark_time(&trans->impl_->start_mark_tim_);

    IFLOG(peer_->log_, inf(TH_ID, LS_TRX"[%08x%08x%08x%08x][TXTYPE:%d, TXACT:%d, RSCLREQ:%d]",
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
        IFLOG(peer_->log_, err(TH_ID, LS_TRX"[same tx already flying]"))
    } else {
        inco_flytx_map_.put(&timpl->txid_, &trans);
    }

    if((rcode = ilistener_->on_incoming_transaction(*ipubl_, trans))) {
        IFLOG(peer_->log_, inf(TH_ID, LS_CON"[connection:%d applicatively refused new transaction]", connid_))
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
            std::unique_ptr<nclass> req_obj;
            if((rcode = peer_->nem_.new_nclass_instance(timpl->req_nclassid_, req_obj))) {
                timpl->tx_res_ = TransactionResult_FAILED;
                timpl->result_code_ = ProtocolCode_MALFORMED_REQUEST;
                timpl->rescls_ = false;
                skip_appl_mng = true;
                IFLOG(peer_->log_, err(TH_ID, LS_TRX"[tx request receive failed - new_nclass_instance:%d, nclass_id:%d]",
                                       rcode, timpl->req_nclassid_))
            } else {
                if((rcode = req_obj->restore(&peer_->nem_, timpl->req_clsenc_, pkt_body))) {
                    timpl->tx_res_ = TransactionResult_FAILED;
                    timpl->result_code_ = ProtocolCode_MALFORMED_REQUEST;
                    timpl->rescls_ = false;
                    skip_appl_mng = true;
                    IFLOG(peer_->log_, err(TH_ID, LS_TRX"[tx request receive failed - nclass restore fail:%d, nclass_id:%d]",
                                           rcode, timpl->req_nclassid_))
                }
                timpl->set_request_obj_on_request(req_obj);
            }
        }
        if(!skip_appl_mng) {
            timpl->ilistener_->on_request(*timpl->ipubl_);
        }
    }

    if((rcode = timpl->send_response())) {
        IFLOG(peer_->log_, err(TH_ID, LS_TRX"[tx response sending failed res:%d]", rcode))
    }

    if((rcode = inco_flytx_map_.remove(&timpl->txid_, nullptr))) {
        IFLOG(peer_->log_, cri(TH_ID, LS_TRX"[error removing tx from flying map - res:%d]", rcode))
    }

    if(aborted) {
        timpl->set_aborted();
    } else {
        timpl->set_closed();
    }

    timpl->ilistener_->on_releaseable(*timpl->ipubl_);
    return rcode;
}

RetCode incoming_connection_impl::new_incoming_subscription(std::shared_ptr<incoming_subscription> &incoming_sbs,
                                                            std::shared_ptr<incoming_connection> &inco_conn)
{
    incoming_subscription &publ = sbs_factory_publ_->make_incoming_subscription(inco_conn);
    incoming_sbs.reset(&publ);
    return RetCode_OK;
}

RetCode incoming_connection_impl::release_subscription(incoming_subscription_impl *subscription)
{
    RetCode rcode = RetCode_OK;
    if(subscription->conn_ != this) {
        IFLOG(peer_->log_, err(TH_ID, LS_CLO "[connid:%d][subscription:%d is not mine]", __func__,
                               connid_,
                               subscription->sbsid_))
        return RetCode_KO;
    }
    if(subscription->status_ == SubscriptionStatus_REQUEST_SENT ||
            subscription->status_ == SubscriptionStatus_STARTED ||
            subscription->status_ == SubscriptionStatus_RELEASED) {
        IFLOG(peer_->log_, wrn(TH_ID, LS_TRL"[connid:%d][subscription:%d released in status:%d]",
                               __func__,
                               connid_,
                               subscription->sbsid_,
                               subscription->status_))
    }
    subscription->release_initial_query();
    peer_->remove_subscriber(subscription);
    subscription->set_released();
    subscription->ilistener_->on_releaseable(*subscription->ipubl_);
    return rcode;
}

RetCode incoming_connection_impl::recv_sbs_start_request(const vlg_hdr_rec *pkt_hdr,
                                                         std::shared_ptr<incoming_connection> &inco_conn)
{
    RetCode rcode = RetCode_OK;
    unsigned int sbsid = next_sbsid();

    std::shared_ptr<incoming_subscription> sbs_sh;
    new_incoming_subscription(sbs_sh, inco_conn);
    incoming_subscription_impl *inc_sbs = sbs_sh->impl_.get();

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

    IFLOG(peer_->log_, inf(TH_ID,
                           LS_SBI"[CONNID:%u-REQID:%u][SBSTYP:%d, SBSMOD:%d, FLOTYP:%d, DWLTYP:%d, ENCTYP:%d, NCLSSID:%d, TMSTP0:%u, TMSTP1:%u]",
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
        IFLOG(peer_->log_, err(TH_ID, LS_SBS"[unsupported nclass_id requested in subscription: %u]", inc_sbs->nclassid_))
    } else {
        if(!(rcode = inco_nclassid_sbs_map_.contains_key(&inc_sbs->nclassid_))) {
            inc_sbs->sbresl_ = SubscriptionResponse_KO;
            inc_sbs->last_vlgcod_ = ProtocolCode_SUBSCRIPTION_ALREADY_STARTED;
            IFLOG(peer_->log_, err(TH_ID, LS_SBS"[subscription on this connection:%d already started for nclass_id:%d]", connid_,
                                   inc_sbs->nclassid_))
        } else {
            if((rcode = inco_sbsid_sbs_map_.put(&sbsid, &sbs_sh))) {
                inc_sbs->sbresl_ = SubscriptionResponse_KO;
                inc_sbs->last_vlgcod_ = ProtocolCode_SERVER_ERROR;
                IFLOG(peer_->log_, cri(TH_ID, LS_SBS"[error putting subscription into sbsid map - res:%d]", rcode))
            }
            if((rcode = inco_nclassid_sbs_map_.put(&inc_sbs->nclassid_, &sbs_sh))) {
                inc_sbs->sbresl_ = SubscriptionResponse_KO;
                inc_sbs->last_vlgcod_ = ProtocolCode_SERVER_ERROR;
                IFLOG(peer_->log_, cri(TH_ID, LS_SBS"[error putting subscription into nclass_id map - res:%d]", rcode))
            }
            inc_sbs->sbsid_ = sbsid;
            inc_sbs->sbresl_ = SubscriptionResponse_OK;
            inc_sbs->last_vlgcod_ = ProtocolCode_SUCCESS;
        }
    }

    if((rcode = ilistener_->on_incoming_subscription(*ipubl_, sbs_sh))) {
        IFLOG(peer_->log_, inf(TH_ID, LS_CON"[connection:%d applicatively refused new subscription]", connid_))
        inc_sbs->sbresl_ = SubscriptionResponse_KO;
        inc_sbs->last_vlgcod_ = ProtocolCode_SERVER_ERROR;
    }

    if((rcode = inc_sbs->send_start_response())) {
        IFLOG(peer_->log_, err(TH_ID, LS_SBS"[subscription response sending failed][res:%d]", rcode))
    }

    if(inc_sbs->sbresl_ == SubscriptionResponse_OK || inc_sbs->sbresl_ == SubscriptionResponse_PARTIAL) {
        if((rcode = peer_->add_subscriber(inc_sbs))) {
            IFLOG(peer_->log_, cri(TH_ID, LS_SBS"[error binding subscription to peer][res:%d]", rcode))
            inc_sbs->set_stopped();
        } else {
            inc_sbs->set_started();
            if(inc_sbs->sbsmod_ == SubscriptionMode_ALL || inc_sbs->sbsmod_ == SubscriptionMode_DOWNLOAD) {
                //do initial query.
                if(!(rcode = inc_sbs->execute_initial_query())) {
                    inc_sbs->initial_query_ended_ = false;
                }
            }
            if(!inc_sbs->initial_query_ended_) {
                inc_sbs->send_initial_query();
            }
        }
    }
    return rcode;
}

RetCode incoming_connection_impl::recv_sbs_stop_request(const vlg_hdr_rec *pkt_hdr,
                                                        std::shared_ptr<incoming_connection> &inco_conn)
{
    RetCode rcode = RetCode_OK;
    unsigned int sbsid = pkt_hdr->row_1.sbsrid.sbsrid;
    IFLOG(peer_->log_, inf(TH_ID, LS_SBS"[CONNID:%u-SBSID:%u][stop request]", connid_, sbsid))
    SubscriptionResponse sbresl = SubscriptionResponse_OK;
    ProtocolCode protocode = ProtocolCode_SUCCESS;
    std::shared_ptr<incoming_subscription> sbs_sh;
    if((rcode = inco_sbsid_sbs_map_.get(&sbsid, &sbs_sh))) {
        IFLOG(peer_->log_, err(TH_ID, LS_SBS"[error on subscription stop getting subscription from sbsid map][res:%d]", rcode))
        sbresl = SubscriptionResponse_KO;
        protocode = ProtocolCode_SUBSCRIPTION_NOT_FOUND;
    } else {
        sbs_sh->impl_->set_stopped();
        release_subscription(sbs_sh->impl_.get());
        inco_sbsid_sbs_map_.remove(&sbs_sh->impl_->sbsid_, nullptr);
        inco_nclassid_sbs_map_.remove(&sbs_sh->impl_->nclassid_, nullptr);
    }

    g_bbuf gbb;
    build_PKT_SBSSPR(sbresl,
                     protocode,
                     sbsid,
                     &gbb);

    std::unique_ptr<conn_pkt> cpkt(new conn_pkt(nullptr, std::move(gbb)));
    pkt_sending_q_.put(&cpkt);
    sel_evt *evt = new sel_evt(VLG_SELECTOR_Evt_SendPacket, inco_conn);
    rcode = peer_->selector_.notify(evt);
    return rcode;
}

}

namespace vlg {

outgoing_connection_impl::outgoing_connection_impl(outgoing_connection &publ,
                                                   outgoing_connection_listener &listener) :
    conn_impl(publ, listener),
    outg_flytx_map_(HMSz_1031, sngl_ptr_obj_mng(), sizeof(tx_id)),
    outg_reqid_sbs_map_(HMSz_23, sngl_ptr_obj_mng(), sizeof(unsigned int)),
    outg_sbsid_sbs_map_(HMSz_1031, sngl_ptr_obj_mng(), sizeof(unsigned int)),
    prid_(0),
    reqid_(0)
{}

outgoing_connection_impl::~outgoing_connection_impl()
{
    if(status_ == ConnectionStatus_ESTABLISHED ||
            status_ == ConnectionStatus_PROTOCOL_HANDSHAKE ||
            status_ == ConnectionStatus_AUTHENTICATED) {
        IFLOG(peer_->log_, wrn(TH_ID, LS_DTR
                               "[connection:%d in status:%d, closing..]",
                               __func__,
                               connid_,
                               status_))

        disconnect(ProtocolCode_UNDEFINED);
        ConnectivityEventResult ce = ConnectivityEventResult_UNDEFINED;
        ConnectivityEventType cet = ConnectivityEventType_UNDEFINED;
        await_for_disconnection_result(ce, cet);
    }
}

unsigned int outgoing_connection_impl::next_prid()
{
    scoped_mx smx(mon_);
    return ++prid_;
}

unsigned int outgoing_connection_impl::next_reqid()
{
    scoped_mx smx(mon_);
    return ++reqid_;
}

static void(stop_all_outg_sbs)(const s_hm &map,
                               const void *key,
                               void *ptr,
                               void *ud)
{
    outgoing_subscription_impl *osbs = *(outgoing_subscription_impl **)ptr;
    osbs->set_stopped();
}

void outgoing_connection_impl::release_all_children()
{
    outg_sbsid_sbs_map_.enum_elements_safe_write(stop_all_outg_sbs, this);
    outg_sbsid_sbs_map_.clear();
    outg_reqid_sbs_map_.clear();
}

RetCode outgoing_connection_impl::client_connect(sockaddr_in &params)
{
    if(peer_->peer_status_ != PeerStatus_RUNNING) {
        IFLOG(peer_->log_, err(TH_ID, LS_CLO "[invalid peer status][%d]", __func__, peer_->peer_status_))
        return RetCode_BADSTTS;
    }
    if(status_ != ConnectionStatus_INITIALIZED && status_ != ConnectionStatus_DISCONNECTED) {
        IFLOG(peer_->log_, err(TH_ID, LS_CLO "[invalid connection status][%d]", __func__, status_))
        return RetCode_BADSTTS;
    }

    connect_evt_occur_ = false;
    g_bbuf gbb;
    build_PKT_CONREQ(cli_agrhbt_, &gbb);

    std::unique_ptr<conn_pkt> cpkt(new conn_pkt(nullptr, std::move(gbb)));
    pkt_sending_q_.put(&cpkt);
    sel_evt *evt = new sel_evt(VLG_SELECTOR_Evt_ConnectRequest, this);
    memcpy(&evt->saddr_, &params, sizeof(sockaddr_in));
    return peer_->selector_.notify(evt);
}

RetCode outgoing_connection_impl::await_for_connection_result(ConnectivityEventResult &con_evt_res,
                                                              ConnectivityEventType &connectivity_evt_type,
                                                              time_t sec,
                                                              long nsec)
{
    if(status_ < ConnectionStatus_INITIALIZED) {
        return RetCode_BADSTTS;
    }
    RetCode rcode = RetCode_OK;
    scoped_mx smx(mon_);
    while(!connect_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                rcode =  RetCode_TIMEOUT;
                break;
            }
        }
    }
    con_evt_res = con_evt_res_;
    connectivity_evt_type = connectivity_evt_type_;
    IFLOG(peer_->log_, log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO
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

RetCode outgoing_connection_impl::notify_connection(ConnectivityEventResult con_evt_res,
                                                    ConnectivityEventType connectivity_evt_type)
{
    olistener_->on_connect(*opubl_, con_evt_res, connectivity_evt_type);
    return notify_for_connectivity_result(con_evt_res, connectivity_evt_type);
}

RetCode outgoing_connection_impl::recv_connection_response(const vlg_hdr_rec *pkt_hdr)
{
    RetCode rcode = RetCode_OK;
    ConnectivityEventResult con_evt_res = ConnectivityEventResult_OK;
    conres_ = pkt_hdr->row_1.srvcrs.conres;
    conrescode_ = pkt_hdr->row_1.srvcrs.errcod;
    srv_agrhbt_ = pkt_hdr->row_1.srvcrs.agrhbt;
    connid_ = pkt_hdr->row_2.connid.connid;
    sel_evt *evt = new sel_evt(VLG_SELECTOR_Evt_Undef, this);
    switch(conres_) {
        case ConnectionResult_ACCEPTED:
            IFLOG(peer_->log_, inf(TH_ID, LS_CON"[connection accepted by peer][connid:%d][socket:%d]", connid_, socket_))
            set_proto_connected();
            evt->evt_ = VLG_SELECTOR_Evt_ConnReqAccepted;
            break;
        case ConnectionResult_CONDITIONALLY_ACCEPTED:
            IFLOG(peer_->log_, inf(TH_ID, LS_CON"[connection accepted by peer - with reserve][connid:%d][socket:%d]", connid_,
                                   socket_))
            set_proto_connected();
            evt->evt_ = VLG_SELECTOR_Evt_ConnReqAccepted;
            break;
        case ConnectionResult_REFUSED:
            IFLOG(peer_->log_, wrn(TH_ID, LS_CON"[connection refused by peer]"))
            evt->evt_ = VLG_SELECTOR_Evt_ConnReqRefused;
            con_evt_res = ConnectivityEventResult_KO;
            break;
        default:
            IFLOG(peer_->log_, err(TH_ID, LS_CON"[protocol error]"))
            evt->evt_ = VLG_SELECTOR_Evt_ConnReqRefused;
            con_evt_res = ConnectivityEventResult_KO;
            break;
    }
    if((rcode = peer_->selector_.notify(evt))) {
        set_disconnecting();
        return rcode;
    }
    olistener_->on_connect(*opubl_, con_evt_res, ConnectivityEventType_PROTOCOL);
    notify_for_connectivity_result(con_evt_res, ConnectivityEventType_PROTOCOL);
    return RetCode_OK;
}

RetCode outgoing_connection_impl::recv_disconnection(const vlg_hdr_rec *pkt_hdr)
{
    disconrescode_ = pkt_hdr->row_1.diswrd.disres;
    IFLOG(peer_->log_, inf(TH_ID, LS_CON"[connid:%d][socket:%d][received disconnection - disconrescode:%d]",
                           connid_,
                           socket_,
                           disconrescode_))
    set_disconnecting();
    return RetCode_OK;
}

RetCode outgoing_connection_impl::recv_test_request(const vlg_hdr_rec *pkt_hdr)
{
    return RetCode_UNSP;
}

RetCode outgoing_connection_impl::next_tx_id(tx_id &txid)
{
    txid.txplid = peer_->peer_plid_;
    txid.txsvid = peer_->peer_svid_;
    txid.txcnid = connid_;
    txid.txprid = next_prid();
    return RetCode_OK;
}

RetCode outgoing_connection_impl::recv_tx_response(const vlg_hdr_rec *pkt_hdr,
                                                   g_bbuf *pkt_body)
{
    RetCode rcode = RetCode_OK;
    bool aborted = false;
    tx_id txid;
    txid.txplid = pkt_hdr->row_2.txplid.txplid;
    txid.txsvid = pkt_hdr->row_3.txsvid.txsvid;
    txid.txcnid = pkt_hdr->row_4.txcnid.txcnid;
    txid.txprid = pkt_hdr->row_5.txprid.txprid;

    IFLOG(peer_->log_, dbg(TH_ID, LS_INC"[%08x%08x%08x%08x][TXRES:%d, TXRESCODE:%d, RESCLS:%d] ",
                           txid.txplid,
                           txid.txsvid,
                           txid.txcnid,
                           txid.txprid,
                           pkt_hdr->row_1.txresw.txresl,
                           pkt_hdr->row_1.txresw.vlgcod,
                           pkt_hdr->row_1.txresw.rescls))

    outgoing_transaction_impl *trans = nullptr;
    if((rcode = outg_flytx_map_.get(&txid, &trans))) {
        IFLOG(peer_->log_, err(TH_ID, LS_CLO"[tx not found, aborting]", __func__))
        return RetCode_ABORT;
    }

    trans->tx_res_ = pkt_hdr->row_1.txresw.txresl;
    trans->result_code_ = pkt_hdr->row_1.txresw.vlgcod;
    trans->rescls_ = pkt_hdr->row_1.txresw.rescls;
    if(pkt_hdr->phdr.hdrlen == 8) {
        trans->res_nclassid_ = pkt_hdr->row_7.clsenc.nclsid;
        trans->res_clsenc_ = pkt_hdr->row_7.clsenc.enctyp;
        std::unique_ptr<nclass> res_obj;
        if((rcode = peer_->nem_.new_nclass_instance(trans->res_nclassid_, res_obj))) {
            IFLOG(peer_->log_, err(TH_ID, LS_TRX"[tx response receive failed - new_nclass_instance:%d, nclass_id:%d]",
                                   rcode,
                                   trans->res_nclassid_))
            aborted = true;
        }
        if((rcode = res_obj->restore(&peer_->nem_, trans->res_clsenc_, pkt_body))) {
            IFLOG(peer_->log_, err(TH_ID, LS_TRX"[tx response receive failed - nclass restore fail:%d, nclass_id:%d]",
                                   rcode,
                                   trans->res_nclassid_))
            aborted = true;
        }
        trans->set_result_obj_on_response(res_obj);
    }

    if((rcode = outg_flytx_map_.remove(&trans->txid_, nullptr))) {
        IFLOG(peer_->log_, cri(TH_ID, LS_TRX"[error removing tx from flying map - res:%d]", rcode))
    }

    if(aborted) {
        trans->set_aborted();
    } else {
        trans->set_closed();
    }

    IFLOG(peer_->log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode outgoing_connection_impl::release_subscription(outgoing_subscription_impl *subscription)
{
    if(subscription->conn_ != this) {
        IFLOG(peer_->log_, err(TH_ID, LS_CLO "[connid:%d][subscription:%d is not mine]", __func__,
                               connid_,
                               subscription->sbsid_))
        return RetCode_KO;
    }
    if(subscription->status_ == SubscriptionStatus_REQUEST_SENT ||
            subscription->status_ == SubscriptionStatus_STARTED ||
            subscription->status_ == SubscriptionStatus_RELEASED) {
        IFLOG(peer_->log_, wrn(TH_ID, LS_TRL"[connid:%d][subscription:%d released in status:%d]",
                               __func__,
                               connid_,
                               subscription->sbsid_,
                               subscription->status_))
    }
    outg_reqid_sbs_map_.remove(&subscription->reqid_, nullptr);
    outg_sbsid_sbs_map_.remove(&subscription->sbsid_, nullptr);
    return RetCode_OK;
}

RetCode outgoing_connection_impl::recv_sbs_start_response(const vlg_hdr_rec *pkt_hdr)
{
    RetCode rcode = RetCode_OK;
    unsigned int sbsid = ((pkt_hdr->row_1.sbresw.sbresl == SubscriptionResponse_OK) ||
                          (pkt_hdr->row_1.sbresw.sbresl == SubscriptionResponse_PARTIAL)) ?
                         pkt_hdr->row_3.sbsrid.sbsrid : 0;
    IFLOG(peer_->log_, inf(TH_ID,
                           LS_SBI"[CONNID:%u-REQID:%u][SBSRES:%d, VLGCOD:%d, SBSID:%u]",
                           connid_,
                           pkt_hdr->row_2.rqstid.rqstid,
                           pkt_hdr->row_1.sbresw.sbresl,
                           pkt_hdr->row_1.sbresw.vlgcod,
                           sbsid))

    outgoing_subscription_impl *subscription = nullptr;
    if((rcode = outg_reqid_sbs_map_.remove(&pkt_hdr->row_2.rqstid.rqstid, &subscription))) {
        IFLOG(peer_->log_, err(TH_ID, LS_SBS"[subscription not found, aborting.]"))
        return RetCode_KO;
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
    return rcode;
}

RetCode outgoing_connection_impl::recv_sbs_evt(const vlg_hdr_rec *pkt_hdr,
                                               g_bbuf *pkt_body)
{
    RetCode rcode = RetCode_OK;
    bool mng = true;
    IFLOG(peer_->log_, dbg(TH_ID, LS_SBS
                           "[CONNID:%u-SBSID:%u][EVTID:%d, EVTTYP:%d, PRTCOD:%d, TMSTMP[0]:%d, TMSTMP[1]:%d, ACT:%d]",
                           connid_,
                           pkt_hdr->row_1.sbsrid.sbsrid,
                           pkt_hdr->row_3.sevtid.sevtid,
                           pkt_hdr->row_2.sevttp.sevttp,
                           pkt_hdr->row_2.sevttp.vlgcod,
                           pkt_hdr->row_4.tmstmp.tmstmp,
                           pkt_hdr->row_5.tmstmp.tmstmp,
                           pkt_hdr->row_2.sevttp.sbeact))
    outgoing_subscription_impl *subscription = nullptr;
    if((rcode = outg_sbsid_sbs_map_.get(&pkt_hdr->row_1.sbsrid.sbsrid, &subscription))) {
        IFLOG(peer_->log_, cri(TH_ID, LS_SBS"[error getting subscription from sbsid map][res:%d]", rcode))
        mng = false;
    } else {
        if(mng && (rcode = subscription->receive_event(pkt_hdr, pkt_body))) {
            IFLOG(peer_->log_, wrn(TH_ID, LS_SBS"[subscription event:%d management failed][res:%d]",
                                   pkt_hdr->row_3.sevtid.sevtid,
                                   rcode))
        }
    }
    return rcode;
}

RetCode outgoing_connection_impl::recv_sbs_stop_response(const vlg_hdr_rec *pkt_hdr)
{
    RetCode rcode = RetCode_OK;
    IFLOG(peer_->log_, inf(TH_ID, LS_INC LS_SBS"[CONNID:%u-SBSID:%u][SBSRES:%d, VLGCOD:%d]",
                           connid_,
                           pkt_hdr->row_2.sbsrid.sbsrid,
                           pkt_hdr->row_1.sbresw.sbresl,
                           pkt_hdr->row_1.sbresw.vlgcod))
    outgoing_subscription_impl *subscription = nullptr;
    if((rcode = outg_sbsid_sbs_map_.get(&pkt_hdr->row_2.sbsrid.sbsrid, &subscription))) {
        IFLOG(peer_->log_, wrn(TH_ID, LS_SBS"[no subscription from sbsid map]"))
    } else {
        outg_reqid_sbs_map_.remove(&subscription->reqid_, nullptr);
        outg_sbsid_sbs_map_.remove(&subscription->sbsid_, nullptr);
        subscription->notify_for_start_stop_result();
        if(pkt_hdr->row_1.sbresw.sbresl == SubscriptionResponse_OK) {
            subscription->set_stopped();
        }
    }
    return rcode;
}

}
