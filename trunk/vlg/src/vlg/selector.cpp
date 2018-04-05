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

#include "selector.h"
#include "pr_impl.h"
#include "conn_impl.h"

namespace vlg {

selector_event::selector_event(VLG_SELECTOR_Evt evt, conn_impl *conn) :
    evt_(evt),
    con_type_(conn ? conn->con_type_ : ConnectionType_UNDEFINED),
    conn_(conn),
    socket_(conn ? conn->socket_ : INVALID_SOCKET)
{
    memset(&saddr_, 0, sizeof(sockaddr_in));
}

selector_event::selector_event(VLG_SELECTOR_Evt evt, std::shared_ptr<incoming_connection> &conn) :
    evt_(evt),
    con_type_(ConnectionType_INGOING),
    conn_(conn.get()->impl_.get()),
    inco_conn_(conn),
    socket_(conn ? conn->get_socket() : INVALID_SOCKET)
{
    memset(&saddr_, 0, sizeof(sockaddr_in));
}

// asynch_selector

//for non critical errors
#define RETURNZERO_ACT return 0
#define RETURRetCodeKO_ACT return RetCode_KO

//for critical errors
#define SET_ERROR_AND_RETURNZERO_ACT set_status(SelectorStatus_ERROR); return 0;
#define SET_ERROR_AND_RETURRetCodeKO_ACT set_status(SelectorStatus_ERROR); return RetCode_KO;

selector::selector(peer_impl &peer) :
    peer_(peer),
    status_(SelectorStatus_TO_INIT),
    nfds_(-1),
    sel_res_(-1),
    udp_ntfy_srv_socket_(INVALID_SOCKET),
    udp_ntfy_cli_socket_(INVALID_SOCKET),
    srv_listen_socket_(INVALID_SOCKET),
    srv_acceptor_(peer),
    inco_exec_srv_(peer.peer_id_, true),
    outg_exec_srv_(peer.peer_id_, true)
{
    memset(&udp_ntfy_sa_in_, 0, sizeof(udp_ntfy_sa_in_));
    udp_ntfy_sa_in_.sin_family = AF_INET;
    udp_ntfy_sa_in_.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    memset(&srv_sockaddr_in_, 0, sizeof(srv_sockaddr_in_));
    srv_sockaddr_in_.sin_family = AF_INET;
    srv_sockaddr_in_.sin_addr.s_addr = INADDR_ANY;
    FD_ZERO(&read_FDs_);
    FD_ZERO(&write_FDs_);
    FD_ZERO(&excep_FDs_);
    sel_timeout_.tv_sec = 0;
    sel_timeout_.tv_usec = 0;
}

selector::~selector()
{
    if(!(status_ <= SelectorStatus_INIT) && !(status_ >= SelectorStatus_STOPPED)) {
        IFLOG(cri(TH_ID, LS_DTR"[selector:%d is not in a safe state:%d] " LS_EXUNX, __func__, peer_.peer_id_, status_))
    }
}

RetCode selector::init(unsigned int srv_executors,
                       unsigned int cli_executors)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RET_ON_KO(srv_acceptor_.set_sockaddr_in(srv_sockaddr_in_))
    RET_ON_KO(inco_exec_srv_.init(srv_executors))
    RET_ON_KO(outg_exec_srv_.init(cli_executors))
    RET_ON_KO(create_UDP_notify_srv_sock())
    RET_ON_KO(connect_UDP_notify_cli_sock())
    set_status(SelectorStatus_INIT);
    IFLOG(trc(TH_ID, LS_CLO, __func__))
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
    IFLOG(trc(TH_ID, LS_CLO "[status:%d]", __func__, status))
    return RetCode_OK;
}

RetCode selector::await_for_status_reached_or_outdated(SelectorStatus test,
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
    IFLOG(log(rcode ? TL_WRN : TL_DBG, TH_ID, LS_CLO "test:%d [reached or outdated] current:%d", __func__,
              test,
              status_))
    return rcode;
}

RetCode selector::create_UDP_notify_srv_sock()
{
    int res = 0, err = 0;
    if((udp_ntfy_srv_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != INVALID_SOCKET) {
        IFLOG(dbg(TH_ID, LS_TRL "[udp_ntfy_srv_socket_:%d][OK]", __func__, udp_ntfy_srv_socket_))
        if(!bind(udp_ntfy_srv_socket_, (sockaddr *)&udp_ntfy_sa_in_, sizeof(udp_ntfy_sa_in_))) {
            IFLOG(trc(TH_ID, LS_TRL "[udp_ntfy_srv_socket_:%d][bind OK]", __func__, udp_ntfy_srv_socket_))
#if defined WIN32 && defined _MSC_VER
            unsigned long mode = 1; //non-blocking mode
            if((res = ioctlsocket(udp_ntfy_srv_socket_, FIONBIO, &mode))) {
                err = WSAGetLastError();
                IFLOG(cri(TH_ID, LS_CLO "[udp_ntfy_srv_socket_:%d][ioctlsocket KO][err:%d]", __func__, udp_ntfy_srv_socket_, err))
                return RetCode_SYSERR;
            } else {
                IFLOG(trc(TH_ID, LS_TRL "[udp_ntfy_srv_socket_:%d][ioctlsocket OK]", __func__, udp_ntfy_srv_socket_))
            }
#else
            int flags = fcntl(udp_ntfy_srv_socket_, F_GETFL, 0);
            if(flags < 0) {
                return RetCode_KO;
            }
            flags = (flags|O_NONBLOCK);
            if((res = fcntl(udp_ntfy_srv_socket_, F_SETFL, flags))) {
                IFLOG(cri(TH_ID, LS_CLO "[udp_ntfy_srv_socket_:%d][fcntl KO][err:%d]", __func__, udp_ntfy_srv_socket_, errno))
                return RetCode_SYSERR;
            } else {
                IFLOG(trc(TH_ID, LS_TRL "[udp_ntfy_srv_socket_:%d][fcntl OK]", __func__, udp_ntfy_srv_socket_))
            }
#endif
        } else {
#if defined WIN32 && defined _MSC_VER
            err = WSAGetLastError();
#else
            err = errno;
#endif
            IFLOG(cri(TH_ID, LS_CLO "[udp_ntfy_srv_socket_:%d][bind KO][err:%d]", __func__, udp_ntfy_srv_socket_, err))
            return RetCode_SYSERR;
        }
    } else {
        IFLOG(cri(TH_ID, LS_CLO "[socket KO][err:%d]", __func__, err))
        return RetCode_SYSERR;
    }
    return RetCode_OK;
}

RetCode selector::connect_UDP_notify_cli_sock()
{
    int err = 0;
    socklen_t len = sizeof(udp_ntfy_sa_in_);
    getsockname(udp_ntfy_srv_socket_, (struct sockaddr *)&udp_ntfy_sa_in_, &len);
    IFLOG(trc(TH_ID, LS_OPN "[sin_addr:%s, sin_port:%d]", __func__,
              inet_ntoa(udp_ntfy_sa_in_.sin_addr),
              htons(udp_ntfy_sa_in_.sin_port)))
    if((udp_ntfy_cli_socket_ = socket(AF_INET, SOCK_DGRAM, 0)) != INVALID_SOCKET) {
        IFLOG(dbg(TH_ID, LS_TRL "[udp_ntfy_cli_socket_:%d][OK]", __func__, udp_ntfy_cli_socket_))
        if((connect(udp_ntfy_cli_socket_, (struct sockaddr *)&udp_ntfy_sa_in_, sizeof(udp_ntfy_sa_in_))) != INVALID_SOCKET) {
            IFLOG(dbg(TH_ID, LS_TRL "[udp_ntfy_cli_socket_:%d][connect OK]", __func__, udp_ntfy_cli_socket_))
        } else {
#if defined WIN32 && defined _MSC_VER
            err = WSAGetLastError();
#endif
            IFLOG(cri(TH_ID, LS_CLO "[udp_ntfy_cli_socket_:%d][connect KO][err:%d]", __func__, udp_ntfy_cli_socket_, err))
            return RetCode_SYSERR;
        }
    } else {
        IFLOG(cri(TH_ID, LS_CLO "[socket KO]", __func__))
        return RetCode_SYSERR;
    }
    return RetCode_OK;
}

RetCode selector::evt_enqueue_and_notify(const selector_event *evt)
{
    RetCode rcode = RetCode_OK;
    rcode = asynch_notify(evt);
    return rcode;
}

RetCode selector::interrupt()
{
    selector_event *interrupt = new selector_event(VLG_SELECTOR_Evt_Interrupt, nullptr);
    return asynch_notify(interrupt);
}

RetCode selector::asynch_notify(const selector_event *evt)
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
            IFLOG(err(TH_ID, LS_CLO "[udp_ntfy_cli_socket_:%d][err:%d]", __func__, udp_ntfy_cli_socket_, err))
            return RetCode_KO;
        } else {
            perror(__func__);
            IFLOG(err(TH_ID, LS_CLO "[udp_ntfy_cli_socket_:%d][errno:%d]", __func__, udp_ntfy_cli_socket_, errno))
            return RetCode_SYSERR;
        }
    }
    return RetCode_OK;
}

RetCode selector::start_exec_services()
{
    RetCode res = RetCode_OK;
    PEXEC_SERVICE_STATUS current = PEXEC_SERVICE_STATUS_ZERO;
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
        IFLOG(dbg(TH_ID, LS_TRL "[starting server side executor service]", __func__))
        if((res = inco_exec_srv_.start())) {
            IFLOG(cri(TH_ID, LS_CLO "[starting server side, last_err:%d]", __func__, res))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        inco_exec_srv_.await_for_status_reached_or_outdated(PEXEC_SERVICE_STATUS_STARTED, current);
        IFLOG(dbg(TH_ID, LS_TRL "[server side executor service started]", __func__))
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
        IFLOG(dbg(TH_ID, LS_TRL "[starting client side executor service]", __func__))
        if((res = outg_exec_srv_.start())) {
            IFLOG(cri(TH_ID, LS_CLO "[starting client side, last_err:%d]", __func__, res))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        outg_exec_srv_.await_for_status_reached_or_outdated(PEXEC_SERVICE_STATUS_STARTED, current);
        IFLOG(dbg(TH_ID, LS_TRL "[client side executor service started]", __func__))
    }
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return res;
}

RetCode selector::start_conn_objs()
{
    RetCode res = RetCode_OK;
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
        if((res = srv_acceptor_.create_server_socket(srv_listen_socket_))) {
            IFLOG(cri(TH_ID, LS_CLO "[starting acceptor, last_err:%d]", __func__, res))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        FD_SET(srv_listen_socket_, &read_FDs_);
        FD_SET(srv_listen_socket_, &excep_FDs_);
        nfds_ = (int)srv_listen_socket_;
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
        //???
    }
    FD_SET(udp_ntfy_srv_socket_, &read_FDs_);
    FD_SET(udp_ntfy_srv_socket_, &excep_FDs_);
    nfds_ = ((int)udp_ntfy_srv_socket_ > nfds_) ? (int)udp_ntfy_srv_socket_ : nfds_;
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return res;
}

RetCode selector::process_inco_sock_inco_events()
{
    SOCKET srv_cli_sock = INVALID_SOCKET;
    RetCode rcode = RetCode_OK, sub_res = RetCode_OK;
    //**** HANDLE INCOMING EVENTS BEGIN****
    if(sel_res_) {
        for(auto it = inco_connid_conn_map_.begin(); it != inco_connid_conn_map_.end(); it++) {
            srv_cli_sock = it->second->get_socket();
            //first: check if it is readable.
            if(FD_ISSET(srv_cli_sock, &read_FDs_)) {
                IFLOG(trc(TH_ID, LS_TRL"[socket:%d, connid:%d][server-side socket became readable]",
                          __func__,
                          srv_cli_sock,
                          it->first))
                if(it->second->impl_->status_ != ConnectionStatus_ESTABLISHED &&
                        it->second->impl_->status_ != ConnectionStatus_PROTOCOL_HANDSHAKE &&
                        it->second->impl_->status_ != ConnectionStatus_AUTHENTICATED) {
                    IFLOG(trc(TH_ID, LS_TRL"[socket:%d, connid:%d, status:%d][not eligible for recv()]",
                              __func__,
                              srv_cli_sock,
                              it->first,
                              it->second->impl_->status_))
                    break;
                }
                std::unique_ptr<vlg_hdr_rec> pkt_hdr(new vlg_hdr_rec());
                std::unique_ptr<g_bbuf> pkt_body(new g_bbuf());
                //read data from socket.
                if(!(rcode = it->second->impl_->recv_single_pkt(pkt_hdr.get(), pkt_body.get()))) {
                    p_tsk *task = nullptr;
                    pkt_body->flip();
                    task = new peer_recv_task_inco_conn(it->second, pkt_hdr, pkt_body);
                    if((sub_res = inco_exec_srv_.submit(task))) {
                        IFLOG(cri(TH_ID, LS_TRL"[socket:%d, connid:%d][submit failed][res:%d]",
                                  __func__,
                                  srv_cli_sock,
                                  it->first,
                                  sub_res))
                    }
                }
                if(!(--sel_res_)) {
                    break;
                }
            }
            //second: check if it is in exception.
            if(FD_ISSET(srv_cli_sock, &excep_FDs_)) {
                IFLOG(wrn(TH_ID, LS_TRL "[socket:%d][server-side socket is in exception]", __func__, srv_cli_sock))
                if(!(--sel_res_)) {
                    break;
                }
            }
        }
    }
    //**** HANDLE INCOMING EVENTS END****
    return RetCode_OK;
}

RetCode selector::process_inco_sock_outg_events()
{
    RetCode rcode = RetCode_OK;
    //**** HANDLE OUTGOING EVENTS BEGIN****
    for(auto it = write_pending_sock_inco_conn_map_.begin(); it != write_pending_sock_inco_conn_map_.end(); it++) {
        if(FD_ISSET(it->first, &write_FDs_)) {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][is writepending]", __func__,
                      it->first,
                      it->second->impl_->connid_))
            g_bbuf *sending_pkt = nullptr;
            if(it->second->impl_->pkt_sending_q_.get(&sending_pkt)) {
                IFLOG(cri(TH_ID, LS_CLO "[reading from packet queue]", __func__))
                SET_ERROR_AND_RETURRetCodeKO_ACT
            }
            //send data to socket.
            if((rcode = it->second->impl_->send_single_pkt(sending_pkt))) {
                IFLOG(err(TH_ID, LS_TRL "[socket:%d, connid:%d][failed sending packet]",
                          __func__,
                          it->first,
                          it->second->impl_->connid_))
            }
            delete sending_pkt;
            if(!(--sel_res_)) {
                break;
            }
        }
        //second: check if it is in exception.
        if(FD_ISSET(it->first, &excep_FDs_)) {
            IFLOG(wrn(TH_ID, LS_TRL "[socket:%d, connid:%d][writepending socket is in exception]", __func__,
                      it->first,
                      it->second->impl_->connid_))
            if(!(--sel_res_)) {
                break;
            }
        }
    }
    //**** HANDLE OUTGOING EVENTS END****
    return RetCode_OK;
}

RetCode selector::process_outg_sock_outg_events()
{
    RetCode rcode = RetCode_OK;
    //**** HANDLE OUTGOING EVENTS BEGIN****
    for(auto it = write_pending_sock_outg_conn_map_.begin(); it != write_pending_sock_outg_conn_map_.end(); it++) {
        if(FD_ISSET(it->first, &write_FDs_)) {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][is write-pending]", __func__,
                      it->first,
                      it->second->connid_))
            g_bbuf *sending_pkt = nullptr;
            if(it->second->pkt_sending_q_.get(&sending_pkt)) {
                IFLOG(cri(TH_ID, LS_CLO "[reading from packet queue]", __func__))
                SET_ERROR_AND_RETURRetCodeKO_ACT
            }
            //send data to socket.
            if((rcode = it->second->send_single_pkt(sending_pkt))) {
                IFLOG(err(TH_ID, LS_TRL "[socket:%d, connid:%d][failed sending packet]",
                          __func__,
                          it->first,
                          it->second->connid_))
            }
            delete sending_pkt;
            if(!(--sel_res_)) {
                break;
            }
        }
        //second: check if it is in exception.
        if(FD_ISSET(it->first, &excep_FDs_)) {
            IFLOG(wrn(TH_ID, LS_TRL "[socket:%d, connid:%d][writepending socket is in exception]", __func__,
                      it->first,
                      it->second->connid_))
            if(!(--sel_res_)) {
                break;
            }
        }
    }
    //**** HANDLE OUTGOING EVENTS END****
    return RetCode_OK;
}

RetCode selector::process_outg_sock_inco_events()
{
    p_tsk *task = nullptr;
    unsigned int connid = 0;
    RetCode rcode = RetCode_OK, sub_res = RetCode_OK;
    //**** HANDLE INCOMING EVENTS BEGIN****
    //EARLY CONNECTIONS
    if(sel_res_) {
        for(auto it = outg_early_sock_conn_map_.begin(); it != outg_early_sock_conn_map_.end(); it++) {
            //first: check if it is readable.
            if(FD_ISSET(it->first, &read_FDs_)) {
                IFLOG(trc(TH_ID, LS_TRL"[socket:%d][early-outgoing socket became readable]", __func__, it->first))
                if(it->second->status_ != ConnectionStatus_ESTABLISHED &&
                        it->second->status_ != ConnectionStatus_PROTOCOL_HANDSHAKE &&
                        it->second->status_ != ConnectionStatus_AUTHENTICATED) {
                    IFLOG(trc(TH_ID, LS_TRL"[socket:%d, connid:%d, status:%d][not eligible for recv()]", __func__,
                              it->first,
                              connid,
                              it->second->status_))
                    break;
                }
                std::unique_ptr<vlg_hdr_rec> pkt_hdr(new vlg_hdr_rec());
                std::unique_ptr<g_bbuf> pkt_body(new g_bbuf());
                //read data from socket.
                if(!(rcode = it->second->recv_single_pkt(pkt_hdr.get(), pkt_body.get()))) {
                    pkt_body->flip();
                    task = new peer_recv_task_outg_conn(*it->second->opubl_, pkt_hdr, pkt_body);
                    if((sub_res = outg_exec_srv_.submit(task))) {
                        IFLOG(cri(TH_ID, LS_TRL "[socket:%d][submit failed][res:%d]",
                                  __func__,
                                  it->first,
                                  sub_res))
                    }
                }
                if(!(--sel_res_)) {
                    break;
                }
            }
            //second: check if it is in exception.
            if(FD_ISSET(it->first, &excep_FDs_)) {
                IFLOG(wrn(TH_ID, LS_TRL "[socket:%d][early-client-side socket is in exception]", __func__, it->first))
                if(!(--sel_res_)) {
                    break;
                }
            }
        }
    }
    //PROTO CONNECTED CONNECTIONS
    if(sel_res_) {
        for(auto it = outg_connid_conn_map_.begin(); it != outg_connid_conn_map_.end(); it++) {
            //first: check if it is readable.
            if(FD_ISSET(it->second->socket_, &read_FDs_)) {
                IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][client-side socket became readable]", __func__,
                          it->second->socket_,
                          connid))
                if(it->second->status_ != ConnectionStatus_ESTABLISHED &&
                        it->second->status_ != ConnectionStatus_PROTOCOL_HANDSHAKE &&
                        it->second->status_ != ConnectionStatus_AUTHENTICATED) {
                    IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d, status:%d][not eligible for recv()]", __func__,
                              it->second->socket_,
                              connid,
                              it->second->status_))
                    break;
                }
                std::unique_ptr<vlg_hdr_rec> pkt_hdr(new vlg_hdr_rec());
                std::unique_ptr<g_bbuf> pkt_body(new g_bbuf());
                //read data from socket.
                if(!(rcode = it->second->recv_single_pkt(pkt_hdr.get(), pkt_body.get()))) {
                    pkt_body->flip();
                    task = new peer_recv_task_outg_conn(*it->second->opubl_, pkt_hdr, pkt_body);
                    if((sub_res = outg_exec_srv_.submit(task))) {
                        IFLOG(cri(TH_ID, LS_TRL "[socket:%d, connid:%d][submit failed][res:%d]",
                                  __func__,
                                  it->second->socket_,
                                  connid,
                                  sub_res))
                    }
                }
                if(!(--sel_res_)) {
                    break;
                }
            }
            //second: check if it is in exception.
            if(FD_ISSET(it->second->socket_, &excep_FDs_)) {
                IFLOG(wrn(TH_ID, LS_TRL "[socket:%d][client-side socket is in exception]", __func__, it->second->socket_))
                if(!(--sel_res_)) {
                    break;
                }
            }
        }
        //**** HANDLE INCOMING EVENTS END****
    }
    return RetCode_OK;
}

inline RetCode selector::consume_inco_sock_events()
{
    std::shared_ptr<incoming_connection> new_conn_shp;
    unsigned int new_connid = 0;
    if(FD_ISSET(srv_listen_socket_, &read_FDs_)) {
        if(peer_.next_connid(new_connid)) {
            IFLOG(cri(TH_ID, LS_CLO "[generating client connection id]", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        if(srv_acceptor_.accept(new_connid, new_conn_shp)) {
            IFLOG(cri(TH_ID, LS_CLO "[accepting new connection]", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        if(new_conn_shp->impl_->set_socket_blocking_mode(false)) {
            IFLOG(cri(TH_ID, LS_CLO "[set socket not blocking]", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        inco_connid_conn_map_[new_connid] = new_conn_shp;
        IFLOG(dbg(TH_ID, LS_CON"[socket:%d, host:%s, port:%d, connid:%d][socket accepted]",
                  new_conn_shp->get_socket(),
                  new_conn_shp->get_host_ip(),
                  new_conn_shp->get_host_port(),
                  new_conn_shp->get_id()))
        --sel_res_;
        if(sel_res_) {
            //in addition to the newly connection, there is also some other message incoming on client sockets.
            IFLOG(trc(TH_ID, LS_SEL "+some events need to be processed+", __func__))
            if(process_inco_sock_inco_events()) {
                IFLOG(cri(TH_ID, LS_CLO "[processing incoming socket events]", __func__))
                SET_ERROR_AND_RETURRetCodeKO_ACT
            }
        }
    } else {
        //we have to handle incoming messages on client sockets
        IFLOG(trc(TH_ID, LS_SEL "+some events need to be processed+", __func__))
        if(process_inco_sock_inco_events()) {
            IFLOG(cri(TH_ID, LS_CLO "[processing incoming socket events]", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
    }
    return RetCode_OK;
}

inline RetCode selector::FDSET_sockets()
{
    FD_ZERO(&read_FDs_);
    FD_ZERO(&write_FDs_);
    FD_ZERO(&excep_FDs_);
    if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
        RET_ON_KO(FDSET_incoming_sockets())
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
        RET_ON_KO(FDSET_outgoing_sockets())
    }
    FDSET_write_incoming_pending_sockets();
    FDSET_write_outgoing_pending_sockets();
    FD_SET(udp_ntfy_srv_socket_, &read_FDs_);
    FD_SET(udp_ntfy_srv_socket_, &excep_FDs_);
    nfds_ = ((int)udp_ntfy_srv_socket_ > nfds_) ? (int)udp_ntfy_srv_socket_ : nfds_;
    return RetCode_OK;
}

RetCode selector::FDSET_write_incoming_pending_sockets()
{
    auto it = write_pending_sock_inco_conn_map_.begin();
    while(it != write_pending_sock_inco_conn_map_.end()) {
        if(it->second->impl_->pkt_sending_q_.size()) {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][iwp+]", __func__,
                      it->first,
                      it->second->get_id()))
            FD_SET(it->first, &write_FDs_);
            nfds_ = ((int)it->first > nfds_) ? (int)it->first : nfds_;
            it++;
        } else {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][iwp-]", __func__,
                      it->first,
                      it->second->get_id()))
            it = write_pending_sock_inco_conn_map_.erase(it);
        }
    }
    return RetCode_OK;
}

RetCode selector::FDSET_write_outgoing_pending_sockets()
{
    auto it = write_pending_sock_outg_conn_map_.begin();
    while(it != write_pending_sock_outg_conn_map_.end()) {
        if(it->second->pkt_sending_q_.size()) {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][owp+]", __func__,
                      it->first,
                      it->second->connid_))
            FD_SET(it->first, &write_FDs_);
            nfds_ = ((int)it->first > nfds_) ? (int)it->first : nfds_;
            it++;
        } else {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][owp-]", __func__,
                      it->first,
                      it->second->connid_))
            it = write_pending_sock_outg_conn_map_.erase(it);
        }
    }
    return RetCode_OK;
}

inline RetCode selector::FDSET_incoming_sockets()
{
    auto it = inco_connid_conn_map_.begin();
    while(it != inco_connid_conn_map_.end()) {
        if(it->second->get_status() == ConnectionStatus_ESTABLISHED ||
                it->second->get_status() == ConnectionStatus_PROTOCOL_HANDSHAKE ||
                it->second->get_status() == ConnectionStatus_AUTHENTICATED) {
            SOCKET inco_sock = it->second->get_socket();
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][irp+]",
                      __func__,
                      inco_sock,
                      it->first))
            FD_SET(inco_sock, &read_FDs_);
            FD_SET(inco_sock, &excep_FDs_);
            nfds_ = ((int)inco_sock > nfds_) ? (int)inco_sock : nfds_;
            it++;
        } else {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d, status:%d][irp-]",
                      __func__,
                      it->second->get_socket(),
                      it->first,
                      it->second->get_status()))
            SOCKET inco_sock = it->second->get_socket();
            if(it->second->get_status() != ConnectionStatus_DISCONNECTED) {
                it->second->impl_->socket_shutdown();
                it->second->impl_->notify_disconnection(ConnectivityEventResult_OK, ConnectivityEventType_NETWORK);
            }
            if(write_pending_sock_inco_conn_map_.erase(inco_sock)) {
                IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][<][iwp-]",
                          __func__,
                          inco_sock,
                          it->first))
            }
            it = inco_connid_conn_map_.erase(it);
        }
    }
    //always set server socket in read and exception fds.
    FD_SET(srv_listen_socket_, &read_FDs_);
    FD_SET(srv_listen_socket_, &excep_FDs_);
    nfds_ = ((int)srv_listen_socket_ > nfds_) ? (int)srv_listen_socket_ : nfds_;
    return RetCode_OK;
}

inline RetCode selector::FDSET_outgoing_sockets()
{
    auto it_1 = outg_early_sock_conn_map_.begin();
    while(it_1 != outg_early_sock_conn_map_.end()) {
        if(it_1->second->status_ == ConnectionStatus_ESTABLISHED ||
                it_1->second->status_ == ConnectionStatus_PROTOCOL_HANDSHAKE ||
                it_1->second->status_ == ConnectionStatus_AUTHENTICATED) {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][early][orp+]", __func__,
                      it_1->second->socket_,
                      it_1->second->connid_))
            FD_SET(it_1->second->socket_, &read_FDs_);
            FD_SET(it_1->second->socket_, &excep_FDs_);
            nfds_ = ((int)it_1->second->socket_ > nfds_) ? (int)it_1->second->socket_ : nfds_;
            it_1++;
        } else {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d, status:%d][early][[orp-]",
                      __func__,
                      it_1->second->socket_,
                      it_1->second->connid_,
                      it_1->second->status_))
            it_1 = outg_early_sock_conn_map_.erase(it_1);
            if(it_1->second->status_ != ConnectionStatus_DISCONNECTED) {
                it_1->second->socket_shutdown();
            }
            if(write_pending_sock_outg_conn_map_.erase(it_1->second->socket_)) {
                IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][early][owp-]",
                          __func__,
                          it_1->second->socket_,
                          it_1->second->connid_))
            }
        }
    }
    auto it_2 = outg_connid_conn_map_.begin();
    while(it_2 != outg_connid_conn_map_.end()) {
        if(it_2->second->status_ == ConnectionStatus_ESTABLISHED ||
                it_2->second->status_ == ConnectionStatus_PROTOCOL_HANDSHAKE ||
                it_2->second->status_ == ConnectionStatus_AUTHENTICATED) {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][orp+]",
                      __func__,
                      it_2->second->socket_,
                      it_2->second->connid_))
            FD_SET(it_2->second->socket_, &read_FDs_);
            FD_SET(it_2->second->socket_, &excep_FDs_);
            nfds_ = ((int)it_2->second->socket_ > nfds_) ? (int)it_2->second->socket_ : nfds_;
            it_2++;
        } else {
            IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d, status:%d][orp-]",
                      __func__,
                      it_2->second->socket_,
                      it_2->second->connid_,
                      it_2->second->status_))
            if(it_2->second->status_ != ConnectionStatus_DISCONNECTED) {
                it_2->second->socket_shutdown();
            }
            if(write_pending_sock_outg_conn_map_.erase(it_2->second->socket_)) {
                IFLOG(trc(TH_ID, LS_TRL "[socket:%d, connid:%d][owp-]",
                          __func__,
                          it_2->second->socket_,
                          it_2->second->connid_))
            }
            it_2 = outg_connid_conn_map_.erase(it_2);
        }
    }
    return RetCode_OK;
}

inline RetCode selector::manage_disconnect_conn(selector_event *conn_evt)
{
    RetCode rcode = RetCode_OK;
    g_bbuf *sending_pkt = nullptr;
    if(conn_evt->conn_->pkt_sending_q_.get(&sending_pkt)) {
        IFLOG(cri(TH_ID, LS_CLO "[reading from packet queue]", __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    if((rcode = conn_evt->conn_->send_single_pkt(sending_pkt))) {
        IFLOG(err(TH_ID, LS_TRL "[socket:%d][failed sending disconnection packet]",
                  __func__,
                  conn_evt->conn_->socket_))
        delete sending_pkt;
        return RetCode_KO;
    }
    delete sending_pkt;
    conn_evt->conn_->socket_shutdown();
    conn_evt->conn_->notify_disconnection(ConnectivityEventResult_OK, ConnectivityEventType_APPLICATIVE);
    return rcode;
}


inline RetCode selector::add_early_outg_conn(selector_event *conn_evt)
{
    RetCode rcode = RetCode_OK;
    g_bbuf *sending_pkt = nullptr;
    if((rcode = conn_evt->conn_->establish_connection(conn_evt->saddr_))) {
        if(conn_evt->conn_->pkt_sending_q_.get(&sending_pkt)) {
            IFLOG(cri(TH_ID, LS_CLO "[reading from packet queue]", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        delete sending_pkt;
        return rcode;
    }
    if(conn_evt->conn_->set_socket_blocking_mode(false)) {
        IFLOG(cri(TH_ID, LS_CLO "[setting socket not blocking]", __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    if(conn_evt->conn_->pkt_sending_q_.get(&sending_pkt)) {
        IFLOG(cri(TH_ID, LS_CLO "[reading from packet queue]", __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    if((rcode = conn_evt->conn_->send_single_pkt(sending_pkt))) {
        IFLOG(err(TH_ID, LS_TRL "[socket:%d][failed sending conn-req packet]",
                  __func__,
                  conn_evt->conn_->socket_))
        delete sending_pkt;
        return rcode;
    }
    delete sending_pkt;
    outg_early_sock_conn_map_[conn_evt->conn_->socket_] = conn_evt->conn_;
    return rcode;
}

inline RetCode selector::promote_early_outg_conn(conn_impl *conn)
{
    outg_early_sock_conn_map_.erase(conn->socket_);
    outg_connid_conn_map_[conn->connid_] = conn;
    return RetCode_OK;
}

inline RetCode selector::delete_early_outg_conn(conn_impl *conn)
{
    outg_early_sock_conn_map_.erase(conn->socket_);
    return RetCode_OK;
}

inline bool selector::is_still_valid_connection(const selector_event *evt)
{
    if(evt->con_type_ == ConnectionType_INGOING) {
        return (inco_connid_conn_map_.find(evt->conn_->connid_) != inco_connid_conn_map_.end());
    } else {
        if(evt->evt_ == VLG_SELECTOR_Evt_ConnReqAccepted || evt->evt_ == VLG_SELECTOR_Evt_ConnReqRefused) {
            return (outg_early_sock_conn_map_.find(evt->socket_) != outg_early_sock_conn_map_.end());
        } else {
            return (outg_connid_conn_map_.find(evt->conn_->connid_) != outg_connid_conn_map_.end());
        }
    }
}

RetCode selector::consume_asynch_events()
{
    long brecv = 0, recv_buf_sz = sizeof(void *);
    selector_event *conn_evt = nullptr;
    bool conn_still_valid = true;
    while((brecv = recv(udp_ntfy_srv_socket_, (char *)&conn_evt, recv_buf_sz, 0)) > 0) {
        if(brecv != recv_buf_sz) {
            IFLOG(cri(TH_ID, LS_CLO "[brecv != recv_buf_sz]", __func__))
            return RetCode_GENERR;
        }
        IFLOG(trc(TH_ID,
                  LS_TRL"[asynch-event][evt_type:%d, socket:%d, connid:%u]",
                  __func__,
                  conn_evt->evt_,
                  conn_evt->socket_,
                  conn_evt->conn_ ? conn_evt->conn_->connid_ : -1))
        if(conn_evt->evt_ != VLG_SELECTOR_Evt_Interrupt) {
            /*check if we still have connection*/
            if(conn_evt->evt_ != VLG_SELECTOR_Evt_ConnectRequest) {
                conn_still_valid = is_still_valid_connection(conn_evt);
            }
            if(conn_still_valid) {
                switch(conn_evt->evt_) {
                    case VLG_SELECTOR_Evt_SendPacket:
                        if(conn_evt->con_type_ == ConnectionType_INGOING) {
                            write_pending_sock_inco_conn_map_[conn_evt->socket_] = conn_evt->inco_conn_;
                        } else {
                            write_pending_sock_outg_conn_map_[conn_evt->socket_] = conn_evt->conn_;
                        }
                        break;
                    case VLG_SELECTOR_Evt_ConnectRequest:     /*can originate in client peer only*/
                        add_early_outg_conn(conn_evt);
                        break;
                    case VLG_SELECTOR_Evt_Disconnect:         /*can originate in both peer*/
                        manage_disconnect_conn(conn_evt);
                        break;
                    case VLG_SELECTOR_Evt_ConnReqAccepted:    /*can originate in client peer only*/
                        promote_early_outg_conn(conn_evt->conn_);
                        break;
                    case VLG_SELECTOR_Evt_ConnReqRefused:     /*can originate in client peer only*/
                        delete_early_outg_conn(conn_evt->conn_);
                        break;
                    case VLG_SELECTOR_Evt_Inactivity:         /*can originate in both peer*/
                        //@todo
                        break;
                    default:
                        IFLOG(cri(TH_ID, LS_CLO "[unknown event]", __func__))
                        break;
                }
            } else {
                IFLOG(inf(TH_ID, LS_TRL "[socket:%d is no longer valid]", __func__, conn_evt->socket_))
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
            IFLOG(err(TH_ID, LS_CLO "[err:%d]", __func__, err))
            return RetCode_KO;
        } else {
            perror(__func__);
            IFLOG(cri(TH_ID, LS_CLO "[errno:%d]", __func__, errno))
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
        RET_ON_KO(consume_asynch_events())
        sel_res_--;
    }
    if(sel_res_) {
        if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
            RET_ON_KO(consume_inco_sock_events())
        }
        if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
            if(sel_res_) {
                RET_ON_KO(process_outg_sock_inco_events())
            }
        }
    }
    if(sel_res_) {
        if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
            RET_ON_KO(process_inco_sock_outg_events())
        }
        if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
            if(sel_res_) {
                RET_ON_KO(process_outg_sock_outg_events())
            }
        }
    }
    return RetCode_OK;
}

RetCode selector::server_socket_shutdown()
{
    IFLOG(trc(TH_ID, LS_OPN"[socket:%d]", __func__, srv_listen_socket_))
    int last_err_ = 0;
#if defined WIN32 && defined _MSC_VER
    if((last_err_ = closesocket(srv_listen_socket_))) {
        IFLOG(err(TH_ID, LS_CLO "[socket:%d][closesocket KO][res:%d]", __func__,
                  srv_listen_socket_, last_err_))
    } else {
        IFLOG(trc(TH_ID, LS_TRL "[socket:%d][closesocket OK][res:%d]", __func__,
                  srv_listen_socket_, last_err_))
    }
#else
    if((last_err_ = close(srv_listen_socket_))) {
        IFLOG(err(TH_ID, LS_CLO "[socket:%d][closesocket KO][res:%d]", __func__,
                  srv_listen_socket_, last_err_))
    } else {
        IFLOG(trc(TH_ID, LS_TRL "[socket:%d][closesocket OK][res:%d]", __func__,
                  srv_listen_socket_, last_err_))
    }
#if 0
    if((last_err_ = shutdown(srv_listen_socket_, SHUT_RDWR))) {
        IFLOG(err(TH_ID, LS_CLO "[socket:%d][shutdown KO][res:%d]", __func__,
                  srv_listen_socket_, last_err_))
    } else {
        IFLOG(trc(TH_ID, LS_TRL "[socket:%d][shutdown OK][res:%d]", __func__,
                  srv_listen_socket_, last_err_))
    }
#endif
#endif
    IFLOG(trc(TH_ID, LS_CLO, __func__))
    return RetCode_OK;
}

RetCode selector::stop_and_clean()
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = RetCode_OK;
    if(peer_.personality_ == PeerPersonality_PURE_SERVER || peer_.personality_ == PeerPersonality_BOTH) {
        std::for_each(inco_connid_conn_map_.begin(), inco_connid_conn_map_.end(), [](auto &it) {
            if(it.second->get_status() != ConnectionStatus_DISCONNECTED) {
                it.second->impl_->socket_shutdown();
                it.second->impl_->notify_disconnection(ConnectivityEventResult_OK, ConnectivityEventType_APPLICATIVE);
            }
        });
        inco_connid_conn_map_.clear();
        server_socket_shutdown();
        inco_exec_srv_.shutdown();
        inco_exec_srv_.await_termination();
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT || peer_.personality_ == PeerPersonality_BOTH) {
        std::for_each(outg_connid_conn_map_.begin(), outg_connid_conn_map_.end(), [](auto &it) {
            if(it.second->status_ != ConnectionStatus_DISCONNECTED) {
                it.second->socket_shutdown();
                it.second->notify_disconnection(ConnectivityEventResult_OK, ConnectivityEventType_APPLICATIVE);
            }
        });
        outg_connid_conn_map_.clear();
        std::for_each(outg_early_sock_conn_map_.begin(), outg_early_sock_conn_map_.end(), [](auto &it) {
            if(it.second->status_ != ConnectionStatus_DISCONNECTED) {
                it.second->socket_shutdown();
                it.second->notify_disconnection(ConnectivityEventResult_OK, ConnectivityEventType_APPLICATIVE);
            }
        });
        outg_early_sock_conn_map_.clear();
        outg_exec_srv_.shutdown();
        outg_exec_srv_.await_termination();
    }
    FD_ZERO(&read_FDs_);
    FD_ZERO(&write_FDs_);
    FD_ZERO(&excep_FDs_);
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

void *selector::run()
{
    SelectorStatus current = SelectorStatus_UNDEF;
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(status_ != SelectorStatus_INIT && status_ != SelectorStatus_REQUEST_READY) {
        IFLOG(err(TH_ID, LS_CLO "[status_=%d, exp:2][BAD STATUS]", __func__, status_))
        SET_ERROR_AND_RETURNZERO_ACT;
    }
    do {
        IFLOG(dbg(TH_ID, LS_SEL"+wait for go-ready request+"))
        await_for_status_reached_or_outdated(SelectorStatus_REQUEST_READY, current);
        IFLOG(dbg(TH_ID, LS_SEL"+go ready requested, going ready+"))
        set_status(SelectorStatus_READY);
        IFLOG(dbg(TH_ID, LS_SEL"+wait for go-select request+"))
        await_for_status_reached_or_outdated(SelectorStatus_REQUEST_SELECT, current);
        IFLOG(dbg(TH_ID, LS_SEL"+go-select requested, going select+"))
        set_status(SelectorStatus_SELECT);
        timeval sel_timeout = sel_timeout_;
        while(status_ == SelectorStatus_SELECT) {
            IFLOG(low(TH_ID, LS_SEL"+calling select()+", __func__))
            if((sel_res_ = select(nfds_+1, &read_FDs_, &write_FDs_, &excep_FDs_, 0)) > 0) {
                IFLOG(low(TH_ID, LS_SEL"+select() [res:%d]+", __func__, sel_res_))
                consume_events();
            } else if(!sel_res_) {
                //timeout
                IFLOG(trc(TH_ID, LS_SEL"+select() [timeout]+", __func__))
            } else {
                //error
                IFLOG(err(TH_ID, LS_SEL"+select() [error:%d]+", __func__, sel_res_))
                set_status(SelectorStatus_ERROR);
            }
            FDSET_sockets();
            sel_timeout = sel_timeout_;
        }
        if(status_ == SelectorStatus_REQUEST_STOP) {
            IFLOG(dbg(TH_ID, LS_SEL"+stop requested, clean initiated+"))
            stop_and_clean();
            break;
        }
        if(status_ == SelectorStatus_ERROR) {
            IFLOG(err(TH_ID, LS_SEL"+error occurred, clean initiated+"))
            stop_and_clean();
            peer_.set_error();
            break;
        }
    } while(true);
    set_status(SelectorStatus_STOPPED);
    int stop_res = stop();
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, stop_res))
    return 0;
}

}
