/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "selector.h"
#include "pr_impl.h"
#include "conn_impl.h"

namespace vlg {

sel_evt::sel_evt(VLG_SELECTOR_Evt evt, conn_impl *conn) :
    evt_(evt),
    con_type_(conn ? conn->con_type_ : ConnectionType_UNDEFINED),
    conn_(conn),
    socket_(conn ? conn->socket_ : INVALID_SOCKET)
{
    memset(&saddr_, 0, sizeof(sockaddr_in));
}

sel_evt::sel_evt(VLG_SELECTOR_Evt evt, std::shared_ptr<incoming_connection> &conn) :
    evt_(evt),
    con_type_(ConnectionType_INGOING),
    conn_(conn.get()->impl_.get()),
    inco_conn_(conn),
    socket_(conn ? conn->get_socket() : INVALID_SOCKET)
{
    memset(&saddr_, 0, sizeof(sockaddr_in));
}

selector::selector(peer_impl &peer) :
    peer_(peer),
    status_(SelectorStatus_TO_INIT),
    nfds_(-1),
    sel_res_(-1),
    udp_ntfy_srv_socket_(INVALID_SOCKET),
    udp_ntfy_cli_socket_(INVALID_SOCKET),
    srv_socket_(INVALID_SOCKET),
    srv_acceptor_(peer),
    inco_exec_srv_(peer_.log_),
    outg_exec_srv_(peer_.log_)
{
    memset(&udp_ntfy_sa_in_, 0, sizeof(udp_ntfy_sa_in_));
    udp_ntfy_sa_in_.sin_family = AF_INET;
    udp_ntfy_sa_in_.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    memset(&srv_sockaddr_in_, 0, sizeof(srv_sockaddr_in_));
    srv_sockaddr_in_.sin_family = AF_INET;
    srv_sockaddr_in_.sin_addr.s_addr = INADDR_ANY;
    FD_ZERO(&read_FDs_);
    FD_ZERO(&write_FDs_);
    sel_timeout_.tv_sec = 0;
    sel_timeout_.tv_usec = 0;
}

selector::~selector()
{
    if(!(status_ <= SelectorStatus_INIT) && !(status_ >= SelectorStatus_STOPPED)) {
        IFLOG(peer_.log_, critical(LS_DTR"[selector:%d is not in a safe state:%d] " LS_EXUNX, __func__, peer_.peer_id_,
                                   status_))
    }
}

RetCode selector::init(unsigned int srv_executors,
                       unsigned int cli_executors)
{
    RET_ON_KO(srv_acceptor_.set_sockaddr_in(srv_sockaddr_in_))
    RET_ON_KO(inco_exec_srv_.init(srv_executors))
    RET_ON_KO(outg_exec_srv_.init(cli_executors))
    RET_ON_KO(create_UDP_notify_srv_sock())
    RET_ON_KO(connect_UDP_notify_cli_sock())
    set_status(SelectorStatus_INIT);
    return RetCode_OK;
}

RetCode selector::on_peer_start_actions()
{
    return start_exec_services();
}

RetCode selector::on_peer_move_running_actions()
{
    return start_conn_objs();
}

RetCode selector::set_status(SelectorStatus status)
{
    scoped_mx smx(mon_);
    status_ = status;
    mon_.notify_all();
    IFLOG(peer_.log_, trace(LS_CLO "[status:%d]", __func__, status))
    return RetCode_OK;
}

RetCode selector::await_for_status_reached(SelectorStatus test,
                                           SelectorStatus &current,
                                           time_t sec,
                                           long nsec)
{
    scoped_mx smx(mon_);
    if(status_ <SelectorStatus_INIT) {
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
    IFLOG(peer_.log_, trace(LS_CLO "test:%d [reached] current:%d", __func__,
                            test,
                            status_))
    return rcode;
}

RetCode selector::create_UDP_notify_srv_sock()
{
    int res = 0, err = 0;
    if((udp_ntfy_srv_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != INVALID_SOCKET) {
        IFLOG(peer_.log_, debug(LS_TRL "[udp_ntfy_srv_socket_:%d][OK]", __func__, udp_ntfy_srv_socket_))
        if(!bind(udp_ntfy_srv_socket_, (sockaddr *)&udp_ntfy_sa_in_, sizeof(udp_ntfy_sa_in_))) {
            IFLOG(peer_.log_, trace(LS_TRL "[udp_ntfy_srv_socket_:%d][bind OK]", __func__, udp_ntfy_srv_socket_))
#if defined WIN32 && defined _MSC_VER
            unsigned long mode = 1; //non-blocking mode
            if((res = ioctlsocket(udp_ntfy_srv_socket_, FIONBIO, &mode))) {
                err = WSAGetLastError();
                IFLOG(peer_.log_, critical(LS_CLO "[udp_ntfy_srv_socket_:%d][ioctlsocket KO][err:%d]", __func__, udp_ntfy_srv_socket_,
                                           err))
                return RetCode_SYSERR;
            } else {
                IFLOG(peer_.log_, trace(LS_TRL "[udp_ntfy_srv_socket_:%d][ioctlsocket OK]", __func__, udp_ntfy_srv_socket_))
            }
#else
            int flags = fcntl(udp_ntfy_srv_socket_, F_GETFL, 0);
            if(flags < 0) {
                return RetCode_KO;
            }
            flags = (flags|O_NONBLOCK);
            if((res = fcntl(udp_ntfy_srv_socket_, F_SETFL, flags))) {
                IFLOG(peer_.log_, critical(LS_CLO "[udp_ntfy_srv_socket_:%d][fcntl KO][err:%d]", __func__, udp_ntfy_srv_socket_,
                                           errno))
                return RetCode_SYSERR;
            } else {
                IFLOG(peer_.log_, trace(LS_TRL "[udp_ntfy_srv_socket_:%d][fcntl OK]", __func__, udp_ntfy_srv_socket_))
            }
#endif
        } else {
#if defined WIN32 && defined _MSC_VER
            err = WSAGetLastError();
#else
            err = errno;
#endif
            IFLOG(peer_.log_, critical(LS_CLO "[udp_ntfy_srv_socket_:%d][bind KO][err:%d]", __func__, udp_ntfy_srv_socket_, err))
            return RetCode_SYSERR;
        }
    } else {
        IFLOG(peer_.log_, critical(LS_CLO "[socket KO][err:%d]", __func__, err))
        return RetCode_SYSERR;
    }
    return RetCode_OK;
}

RetCode selector::connect_UDP_notify_cli_sock()
{
    int err = 0;
    socklen_t len = sizeof(udp_ntfy_sa_in_);
    getsockname(udp_ntfy_srv_socket_, (struct sockaddr *)&udp_ntfy_sa_in_, &len);
    IFLOG(peer_.log_, trace(LS_OPN "[sin_addr:%s, sin_port:%d]", __func__,
                            inet_ntoa(udp_ntfy_sa_in_.sin_addr),
                            htons(udp_ntfy_sa_in_.sin_port)))
    if((udp_ntfy_cli_socket_ = socket(AF_INET, SOCK_DGRAM, 0)) != INVALID_SOCKET) {
        IFLOG(peer_.log_, debug(LS_TRL "[udp_ntfy_cli_socket_:%d][OK]", __func__, udp_ntfy_cli_socket_))
        if((connect(udp_ntfy_cli_socket_, (struct sockaddr *)&udp_ntfy_sa_in_, sizeof(udp_ntfy_sa_in_))) != INVALID_SOCKET) {
            IFLOG(peer_.log_, debug(LS_TRL "[udp_ntfy_cli_socket_:%d][connect OK]", __func__, udp_ntfy_cli_socket_))
        } else {
#if defined WIN32 && defined _MSC_VER
            err = WSAGetLastError();
#endif
            IFLOG(peer_.log_, critical(LS_CLO "[udp_ntfy_cli_socket_:%d][connect KO][err:%d]", __func__, udp_ntfy_cli_socket_,
                                       err))
            return RetCode_SYSERR;
        }
    } else {
        IFLOG(peer_.log_, critical(LS_CLO "[socket KO]", __func__))
        return RetCode_SYSERR;
    }
    return RetCode_OK;
}

RetCode selector::interrupt()
{
    sel_evt *interrupt = new sel_evt(VLG_SELECTOR_Evt_Interrupt, nullptr);
    return notify(interrupt);
}

RetCode selector::notify(const sel_evt *evt)
{
    long bsent = 0;
    while((bsent = send(udp_ntfy_cli_socket_, (const char *)&evt, sizeof(void *), 0)) == SOCKET_ERROR) {
        int err = 0;
#if defined WIN32 && defined _MSC_VER
        err = WSAGetLastError();
#else
        err = errno;
#endif
#if defined WIN32 && defined _MSC_VER
        if(err == WSAEWOULDBLOCK) {
#else
        if(err == EAGAIN || err == EWOULDBLOCK) {
#endif
            //ok we can go ahead
#if defined WIN32 && defined _MSC_VER
        } else if(err == WSAECONNRESET) {
#else
        } else if(err == ECONNRESET) {
#endif
            IFLOG(peer_.log_, error(LS_CLO "[udp_ntfy_cli_socket_:%d][err:%d]", __func__, udp_ntfy_cli_socket_, err))
            return RetCode_KO;
        } else {
            perror(__func__);
            IFLOG(peer_.log_, error(LS_CLO "[udp_ntfy_cli_socket_:%d][errno:%d]", __func__, udp_ntfy_cli_socket_, errno))
            return RetCode_SYSERR;
        }
    }
    return RetCode_OK;
}

RetCode selector::start_exec_services()
{
    RetCode res = RetCode_OK;
    PExecSrvStatus current = PExecSrvStatus_TOINIT;
    if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
        IFLOG(peer_.log_, debug(LS_TRL "[starting server side executor service]", __func__))
        if((res = inco_exec_srv_.start())) {
            IFLOG(peer_.log_, critical(LS_CLO "[starting server side, last_err:%d]", __func__, res))
            return RetCode_KO;
        }
        inco_exec_srv_.await_for_status_reached(PExecSrvStatus_STARTED, current);
        IFLOG(peer_.log_, debug(LS_TRL "[server side executor service started]", __func__))
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
        IFLOG(peer_.log_, debug(LS_TRL "[starting client side executor service]", __func__))
        if((res = outg_exec_srv_.start())) {
            IFLOG(peer_.log_, critical(LS_CLO "[starting client side, last_err:%d]", __func__, res))
            return RetCode_KO;
        }
        outg_exec_srv_.await_for_status_reached(PExecSrvStatus_STARTED, current);
        IFLOG(peer_.log_, debug(LS_TRL "[client side executor service started]", __func__))
    }
    return res;
}

RetCode selector::start_conn_objs()
{
    RetCode res = RetCode_OK;
    if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
        if((res = srv_acceptor_.create_server_socket(srv_socket_))) {
            IFLOG(peer_.log_, critical(LS_CLO "[starting acceptor, last_err:%d]", __func__, res))
            return RetCode_KO;
        }
        FD_SET(srv_socket_, &read_FDs_);
        nfds_ = (int)srv_socket_;
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
        //???
    }
    FD_SET(udp_ntfy_srv_socket_, &read_FDs_);
    nfds_ = ((int)udp_ntfy_srv_socket_ > nfds_) ? (int)udp_ntfy_srv_socket_ : nfds_;
    return res;
}

RetCode selector::process_inco_sock_inco_events()
{
    SOCKET sckt = INVALID_SOCKET;
    if(sel_res_) {
        for(auto it = inco_conn_map_.begin(); it != inco_conn_map_.end(); it++) {
            sckt = it->second->impl_->socket_;
            //first: check if it is readable.
            if(FD_ISSET(sckt, &read_FDs_)) {
                if(it->second->impl_->status_ != ConnectionStatus_ESTABLISHED &&
                        it->second->impl_->status_ != ConnectionStatus_PROTOCOL_HANDSHAKE &&
                        it->second->impl_->status_ != ConnectionStatus_AUTHENTICATED) {
                    IFLOG(peer_.log_, trace(LS_TRL"[socket:%d, connid:%d, status:%d][not eligible for recv()]",
                                            __func__,
                                            sckt,
                                            it->second->impl_->connid_,
                                            it->second->impl_->status_))
                    break;
                }
                inco_conn_process_rdn_buff(it->second);
                if(!(--sel_res_)) {
                    break;
                }
            }
        }
    }
    return RetCode_OK;
}

inline RetCode selector::process_inco_sock_outg_events()
{
    for(auto it = wp_inco_conn_map_.begin(); it != wp_inco_conn_map_.end(); it++) {
        if(FD_ISSET(it->second->impl_->socket_, &write_FDs_)) {
            it->second->impl_->aggr_msgs_and_send_pkt();
            if(!(--sel_res_)) {
                break;
            }
        }
    }
    return RetCode_OK;
}

inline RetCode selector::process_outg_sock_outg_events()
{
    for(auto it = wp_outg_conn_map_.begin(); it != wp_outg_conn_map_.end(); it++) {
        if(FD_ISSET(it->second->socket_, &write_FDs_)) {
            it->second->aggr_msgs_and_send_pkt();
            if(!(--sel_res_)) {
                break;
            }
        }
    }
    return RetCode_OK;
}

RetCode selector::process_outg_sock_inco_events()
{
    //EARLY CONNECTIONS
    if(sel_res_) {
        for(auto it = outg_early_conn_map_.begin(); it != outg_early_conn_map_.end(); it++) {
            //first: check if it is readable.
            if(FD_ISSET(it->second->socket_, &read_FDs_)) {
                if(it->second->status_ != ConnectionStatus_ESTABLISHED &&
                        it->second->status_ != ConnectionStatus_PROTOCOL_HANDSHAKE &&
                        it->second->status_ != ConnectionStatus_AUTHENTICATED) {
                    IFLOG(peer_.log_, trace(LS_TRL"[socket:%d, connid:%d, status:%d][not eligible for recv()]", __func__,
                                            it->second->socket_,
                                            it->second->connid_,
                                            it->second->status_))
                    break;
                }
                outg_conn_process_rdn_buff(it->second);
                if(!(--sel_res_)) {
                    break;
                }
            }
        }
    }
    //PROTO CONNECTED CONNECTIONS
    if(sel_res_) {
        for(auto it = outg_conn_map_.begin(); it != outg_conn_map_.end(); it++) {
            //first: check if it is readable.
            if(FD_ISSET(it->second->socket_, &read_FDs_)) {
                if(it->second->status_ != ConnectionStatus_ESTABLISHED &&
                        it->second->status_ != ConnectionStatus_PROTOCOL_HANDSHAKE &&
                        it->second->status_ != ConnectionStatus_AUTHENTICATED) {
                    IFLOG(peer_.log_, trace(LS_TRL "[socket:%d, connid:%d, status:%d][not eligible for recv()]", __func__,
                                            it->second->socket_,
                                            it->second->connid_,
                                            it->second->status_))
                    break;
                }
                outg_conn_process_rdn_buff(it->second);
                if(!(--sel_res_)) {
                    break;
                }
            }
        }
    }
    return RetCode_OK;
}

RetCode selector::consume_inco_sock_events()
{
    std::shared_ptr<incoming_connection> new_conn_shp;
    if(FD_ISSET(srv_socket_, &read_FDs_)) {
        if(srv_acceptor_.accept(peer_.next_connid(), new_conn_shp)) {
            IFLOG(peer_.log_, critical(LS_CLO "[accepting new connection]", __func__))
            return RetCode_KO;
        }
        if(new_conn_shp->impl_->set_socket_blocking_mode(false)) {
            IFLOG(peer_.log_, critical(LS_CLO "[set socket not blocking]", __func__))
            return RetCode_KO;
        }
        inco_conn_map_[new_conn_shp->get_socket()] = new_conn_shp;
        IFLOG(peer_.log_, debug(LS_CON"[socket:%d, host:%s, port:%d, connid:%d][socket accepted]",
                                new_conn_shp->get_socket(),
                                new_conn_shp->get_host_ip(),
                                new_conn_shp->get_host_port(),
                                new_conn_shp->get_id()))
        --sel_res_;
        if(sel_res_) {
            if(process_inco_sock_inco_events()) {
                IFLOG(peer_.log_, critical(LS_CLO "[processing incoming socket events]", __func__))
                return RetCode_KO;
            }
        }
    } else {
        if(process_inco_sock_inco_events()) {
            IFLOG(peer_.log_, critical(LS_CLO "[processing incoming socket events]", __func__))
            return RetCode_KO;
        }
    }
    return RetCode_OK;
}

inline void selector::FDSET_sockets()
{
    FD_ZERO(&read_FDs_);
    FD_ZERO(&write_FDs_);
    if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
        FDSET_incoming_sockets();
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
        FDSET_outgoing_sockets();
    }
    FDSET_write_incoming_pending_sockets();
    FDSET_write_outgoing_pending_sockets();
    FD_SET(udp_ntfy_srv_socket_, &read_FDs_);
    nfds_ = ((int)udp_ntfy_srv_socket_ > nfds_) ? (int)udp_ntfy_srv_socket_ : nfds_;
}

inline void selector::FDSET_write_incoming_pending_sockets()
{
    auto it = wp_inco_conn_map_.begin();
    while(it != wp_inco_conn_map_.end()) {
        if(it->second->impl_->acc_snd_buff_.available_read() ||
                (it->second->impl_->cpkt_ && it->second->impl_->cpkt_->pkt_b_.available_read()) ||
                it->second->impl_->pkt_sending_q_.size()) {
            FD_SET(it->second->impl_->socket_, &write_FDs_);
            nfds_ = ((int)it->second->impl_->socket_ > nfds_) ? (int)it->second->impl_->socket_ : nfds_;
            it++;
        } else {
            it = wp_inco_conn_map_.erase(it);
        }
    }
}

inline void selector::FDSET_write_outgoing_pending_sockets()
{
    auto it = wp_outg_conn_map_.begin();
    while(it != wp_outg_conn_map_.end()) {
        if(it->second->acc_snd_buff_.available_read() ||
                (it->second->cpkt_ && it->second->cpkt_->pkt_b_.available_read()) ||
                it->second->pkt_sending_q_.size()) {
            FD_SET(it->second->socket_, &write_FDs_);
            nfds_ = ((int)it->second->socket_ > nfds_) ? (int)it->second->socket_ : nfds_;
            it++;
        } else {
            it = wp_outg_conn_map_.erase(it);
        }
    }
}

inline void selector::FDSET_incoming_sockets()
{
    auto it = inco_conn_map_.begin();
    while(it != inco_conn_map_.end()) {
        if(it->second->get_status() == ConnectionStatus_ESTABLISHED ||
                it->second->get_status() == ConnectionStatus_PROTOCOL_HANDSHAKE ||
                it->second->get_status() == ConnectionStatus_AUTHENTICATED) {
            SOCKET inco_sock = it->second->get_socket();
            FD_SET(inco_sock, &read_FDs_);
            nfds_ = ((int)inco_sock > nfds_) ? (int)inco_sock : nfds_;
            it++;
        } else {
            SOCKET inco_sock = it->second->get_socket();
            if(it->second->get_status() != ConnectionStatus_DISCONNECTED) {
                it->second->impl_->close_connection(ConnectivityEventResult_OK, ConnectivityEventType_NETWORK);
            }
            it->second->impl_->ilistener_->on_releaseable(*it->second);
            wp_inco_conn_map_.erase(inco_sock);
            it = inco_conn_map_.erase(it);

        }
    }
    //always set server socket in read fds.
    FD_SET(srv_socket_, &read_FDs_);
    nfds_ = ((int)srv_socket_ > nfds_) ? (int)srv_socket_ : nfds_;
}

inline void selector::FDSET_outgoing_sockets()
{
    auto it_1 = outg_early_conn_map_.begin();
    while(it_1 != outg_early_conn_map_.end()) {
        if(it_1->second->status_ == ConnectionStatus_ESTABLISHED ||
                it_1->second->status_ == ConnectionStatus_PROTOCOL_HANDSHAKE ||
                it_1->second->status_ == ConnectionStatus_AUTHENTICATED) {
            FD_SET(it_1->second->socket_, &read_FDs_);
            nfds_ = ((int)it_1->second->socket_ > nfds_) ? (int)it_1->second->socket_ : nfds_;
            it_1++;
        } else {
            conn_impl *ci = it_1->second;
            it_1 = outg_early_conn_map_.erase(it_1);
            if(ci->status_ != ConnectionStatus_DISCONNECTED) {
                ci->socket_shutdown();
            }
            wp_outg_conn_map_.erase(ci->socket_);
        }
    }
    auto it_2 = outg_conn_map_.begin();
    while(it_2 != outg_conn_map_.end()) {
        if(it_2->second->status_ == ConnectionStatus_ESTABLISHED ||
                it_2->second->status_ == ConnectionStatus_PROTOCOL_HANDSHAKE ||
                it_2->second->status_ == ConnectionStatus_AUTHENTICATED) {
            FD_SET(it_2->second->socket_, &read_FDs_);
            nfds_ = ((int)it_2->second->socket_ > nfds_) ? (int)it_2->second->socket_ : nfds_;
            it_2++;
        } else {
            if(it_2->second->status_ != ConnectionStatus_DISCONNECTED) {
                it_2->second->socket_shutdown();
            }
            wp_outg_conn_map_.erase(it_2->second->socket_);
            it_2 = outg_conn_map_.erase(it_2);
        }
    }
}

inline RetCode selector::manage_disconnect_conn(sel_evt *conn_evt)
{
    conn_evt->conn_->aggr_msgs_and_send_pkt();
    conn_evt->conn_->close_connection(ConnectivityEventResult_OK, ConnectivityEventType_APPLICATIVE);
    return RetCode_OK;
}

inline RetCode selector::add_early_outg_conn(sel_evt *conn_evt)
{
    RetCode rcode = RetCode_OK;
    if((rcode = conn_evt->conn_->establish_connection(conn_evt->saddr_))) {
        return rcode;
    }
    if(conn_evt->conn_->set_socket_blocking_mode(false)) {
        IFLOG(peer_.log_, critical(LS_CLO "[setting socket not blocking]", __func__))
        return RetCode_KO;
    }
    conn_evt->conn_->aggr_msgs_and_send_pkt();
    outg_early_conn_map_[conn_evt->conn_->socket_] = conn_evt->conn_;
    return RetCode_OK;
}

inline RetCode selector::promote_early_outg_conn(conn_impl *conn)
{
    outg_early_conn_map_.erase(conn->socket_);
    outg_conn_map_[conn->socket_] = conn;
    return RetCode_OK;
}

inline RetCode selector::delete_early_outg_conn(conn_impl *conn)
{
    outg_early_conn_map_.erase(conn->socket_);
    return RetCode_OK;
}

inline bool selector::is_still_valid_connection(const sel_evt *evt)
{
    if(evt->con_type_ == ConnectionType_INGOING) {
        return (inco_conn_map_.find(evt->conn_->socket_) != inco_conn_map_.end());
    } else {
        if(evt->evt_ == VLG_SELECTOR_Evt_ConnReqAccepted || evt->evt_ == VLG_SELECTOR_Evt_ConnReqRefused) {
            return (outg_early_conn_map_.find(evt->socket_) != outg_early_conn_map_.end());
        } else {
            return (outg_conn_map_.find(evt->conn_->socket_) != outg_conn_map_.end());
        }
    }
}

RetCode selector::process_asyn_evts()
{
    long brecv = 0, recv_buf_sz = sizeof(void *);
    sel_evt *conn_evt = nullptr;
    bool conn_still_valid = true;
    while((brecv = recv(udp_ntfy_srv_socket_, (char *)&conn_evt, recv_buf_sz, 0)) > 0) {
        if(brecv != recv_buf_sz) {
            IFLOG(peer_.log_, critical(LS_CLO "[brecv != recv_buf_sz]", __func__))
            return RetCode_GENERR;
        }
        if(conn_evt->evt_ != VLG_SELECTOR_Evt_Interrupt) {
            /*check if we still have connection*/
            if(conn_evt->evt_ != VLG_SELECTOR_Evt_ConnectRequest) {
                conn_still_valid = is_still_valid_connection(conn_evt);
            }
            if(conn_still_valid) {
                switch(conn_evt->evt_) {
                    case VLG_SELECTOR_Evt_SendPacket:
                        if(conn_evt->con_type_ == ConnectionType_INGOING) {
                            wp_inco_conn_map_[conn_evt->socket_] = conn_evt->inco_conn_;
                        } else {
                            wp_outg_conn_map_[conn_evt->socket_] = conn_evt->conn_;
                        }
                        break;
                    case VLG_SELECTOR_Evt_ConnectRequest:
                        add_early_outg_conn(conn_evt);
                        break;
                    case VLG_SELECTOR_Evt_Disconnect:
                        manage_disconnect_conn(conn_evt);
                        break;
                    case VLG_SELECTOR_Evt_ConnReqAccepted:
                        promote_early_outg_conn(conn_evt->conn_);
                        break;
                    case VLG_SELECTOR_Evt_ConnReqRefused:
                        delete_early_outg_conn(conn_evt->conn_);
                        break;
                    case VLG_SELECTOR_Evt_Inactivity:
                        //@todo
                        break;
                    default:
                        IFLOG(peer_.log_, critical(LS_CLO "[unknown event]", __func__))
                        break;
                }
            } else {
                IFLOG(peer_.log_, info(LS_TRL "[socket:%d is no longer valid]", __func__, conn_evt->socket_))
            }
        }
        delete conn_evt;
    }
    if(brecv == SOCKET_ERROR) {
        int err = 0;
#if defined WIN32 && defined _MSC_VER
        err = WSAGetLastError();
#else
        err = errno;
#endif
#if defined WIN32 && defined _MSC_VER
        if(err == WSAEWOULDBLOCK) {
#else
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
#endif
            //ok we can go ahead
#if defined WIN32 && defined _MSC_VER
        } else if(errno == WSAECONNRESET) {
#else
        } else if(errno == ECONNRESET) {
#endif
            IFLOG(peer_.log_, error(LS_CLO "[err:%d]", __func__, err))
            return RetCode_KO;
        } else {
            perror(__func__);
            IFLOG(peer_.log_, critical(LS_CLO "[errno:%d]", __func__, errno))
            return RetCode_SYSERR;
        }
    }
    if(!brecv) {
        return RetCode_KO;
    }
    return RetCode_OK;
}

inline RetCode selector::consume_events()
{
    if(FD_ISSET(udp_ntfy_srv_socket_, &read_FDs_)) {
        process_asyn_evts();
        sel_res_--;
    }
    if(sel_res_) {
        if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
            consume_inco_sock_events();
        }
        if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
            if(sel_res_) {
                process_outg_sock_inco_events();
            }
        }
    }
    if(sel_res_) {
        if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
            process_inco_sock_outg_events();
        }
        if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
            if(sel_res_) {
                process_outg_sock_outg_events();
            }
        }
    }
    return RetCode_OK;
}

RetCode selector::server_socket_shutdown()
{
    int last_err_ = 0;
#if defined WIN32 && defined _MSC_VER
    if((last_err_ = closesocket(srv_socket_))) {
        IFLOG(peer_.log_, error(LS_CLO "[socket:%d][closesocket KO][res:%d]", __func__,
                                srv_socket_, last_err_))
    } else {
        IFLOG(peer_.log_, trace(LS_TRL "[socket:%d][closesocket OK][res:%d]", __func__,
                                srv_socket_, last_err_))
    }
#else
    if((last_err_ = close(srv_socket_))) {
        IFLOG(peer_.log_, error(LS_CLO "[socket:%d][closesocket KO][res:%d]", __func__,
                                srv_socket_, last_err_))
    } else {
        IFLOG(peer_.log_, trace(LS_TRL "[socket:%d][closesocket OK][res:%d]", __func__,
                                srv_socket_, last_err_))
    }
#if 0
    if((last_err_ = shutdown(srv_socket_, SHUT_RDWR))) {
        IFLOG(peer_.log_, error(LS_CLO "[socket:%d][shutdown KO][res:%d]", __func__,
                                srv_socket_, last_err_))
    } else {
        IFLOG(peer_.log_, trace(LS_TRL "[socket:%d][shutdown OK][res:%d]", __func__,
                                srv_socket_, last_err_))
    }
#endif
#endif
    return RetCode_OK;
}

RetCode selector::stop_and_clean()
{
    if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
        for(auto it = inco_conn_map_.begin(); it != inco_conn_map_.end(); ++it)
            if(it->second->get_status() != ConnectionStatus_DISCONNECTED) {
                it->second->impl_->close_connection(ConnectivityEventResult_OK, ConnectivityEventType_APPLICATIVE);
                it->second->impl_->ilistener_->on_releaseable(*it->second);
            }
        wp_inco_conn_map_.clear();
        inco_conn_map_.clear();
        server_socket_shutdown();
        inco_exec_srv_.shutdown();
        inco_exec_srv_.await_termination();
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
        for(auto it = outg_conn_map_.begin(); it != outg_conn_map_.end(); ++it)
            if(it->second->status_ != ConnectionStatus_DISCONNECTED) {
                it->second->close_connection(ConnectivityEventResult_OK, ConnectivityEventType_APPLICATIVE);
            }
        outg_conn_map_.clear();
        for(auto it = outg_early_conn_map_.begin(); it != outg_early_conn_map_.end(); ++it)
            if(it->second->status_ != ConnectionStatus_DISCONNECTED) {
                it->second->close_connection(ConnectivityEventResult_OK, ConnectivityEventType_APPLICATIVE);
            }
        outg_early_conn_map_.clear();
        outg_exec_srv_.shutdown();
        outg_exec_srv_.await_termination();
    }
    FD_ZERO(&read_FDs_);
    FD_ZERO(&write_FDs_);
    return RetCode_OK;
}

RetCode selector::inco_conn_process_rdn_buff(std::shared_ptr<incoming_connection> &ic)
{
    RetCode rcode = ic->impl_->recv_bytes();
    while(!(rcode = ic->impl_->chase_pkt())) {
        std::shared_ptr<p_tsk> task(new peer_recv_task_inco_conn(ic,
                                                                 ic->impl_->curr_rdn_hdr_,
                                                                 ic->impl_->curr_rdn_body_));
        inco_exec_srv_.submit(task);
        ic->impl_->curr_rdn_hdr_.reset();
    }
    return rcode;
}

RetCode selector::outg_conn_process_rdn_buff(conn_impl *oci)
{
    RetCode rcode = oci->recv_bytes();
    while(!(rcode = oci->chase_pkt())) {
        std::shared_ptr<p_tsk> task(new peer_recv_task_outg_conn(*oci->opubl_,
                                                                 oci->curr_rdn_hdr_,
                                                                 oci->curr_rdn_body_));
        outg_exec_srv_.submit(task);
        oci->curr_rdn_hdr_.reset();
    }
    return rcode;
}

void *selector::run()
{
    SelectorStatus current = SelectorStatus_UNDEF;
    if(status_ != SelectorStatus_INIT && status_ != SelectorStatus_REQUEST_READY) {
        IFLOG(peer_.log_, error(LS_CLO "[status_=%d, exp:2][BAD STATUS]", __func__, status_))
        return 0;
    }
    do {
        IFLOG(peer_.log_, debug(LS_SEL"+wait for go-ready request+"))
        await_for_status_reached(SelectorStatus_REQUEST_READY, current);
        IFLOG(peer_.log_, debug(LS_SEL"+go ready requested, going ready+"))
        set_status(SelectorStatus_READY);
        IFLOG(peer_.log_, debug(LS_SEL"+wait for go-select request+"))
        await_for_status_reached(SelectorStatus_REQUEST_SELECT, current);
        IFLOG(peer_.log_, debug(LS_SEL"+go-select requested, going select+"))
        set_status(SelectorStatus_SELECT);
        timeval sel_timeout = sel_timeout_;
        while(status_ == SelectorStatus_SELECT) {
            if((sel_res_ = select(nfds_+1, &read_FDs_, &write_FDs_, 0, 0)) > 0) {
                consume_events();
            } else if(!sel_res_) {
                //timeout
                IFLOG(peer_.log_, trace(LS_SEL"+select() [timeout]+", __func__))
            } else {
                //error
                IFLOG(peer_.log_, error(LS_SEL"+select() [error:%d]+", __func__, sel_res_))
                set_status(SelectorStatus_ERROR);
            }
            FDSET_sockets();
            sel_timeout = sel_timeout_;
        }
        if(status_ == SelectorStatus_REQUEST_STOP) {
            IFLOG(peer_.log_, debug(LS_SEL"+stop requested, clean initiated+"))
            stop_and_clean();
            break;
        }
        if(status_ == SelectorStatus_ERROR) {
            IFLOG(peer_.log_, error(LS_SEL"+error occurred, clean initiated+"))
            stop_and_clean();
            break;
        }
    } while(true);
    set_status(SelectorStatus_STOPPED);
    int stop_res = stop();
    IFLOG(peer_.log_, trace(LS_CLO "[res:%d]", __func__, stop_res))
    return 0;
}

}
