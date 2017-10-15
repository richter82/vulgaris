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

#ifdef WIN32
#include <WS2tcpip.h>
#endif
#ifdef __GNUG__
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#include "blz_selector.h"
#include "blz_peer_int.h"
#include "blz_connection_int.h"

namespace blaze {

selector_event::selector_event(BLZ_SELECTOR_Evt evt, connection_int *conn) :
    evt_(evt),
    con_type_(conn ? conn->conn_type() : ConnectionType_UNDEFINED),
    conn_(conn),
    socket_(conn ? conn->get_socket() : INVALID_SOCKET),
    connid_(conn ? conn->connid(): 0)
{
    memset(&saddr_, 0, sizeof(sockaddr_in));
}

//-----------------------------
// asynch_selector
//-----------------------------

//for non critical errors
#define RETURNZERO_ACT return 0
#define RETURRetCodeKO_ACT return blaze::RetCode_KO

//for critical errors
#define SET_ERROR_AND_RETURNZERO_ACT set_status(BLZ_ASYNCH_SELECTOR_STATUS_ERROR); return 0;
#define SET_ERROR_AND_RETURRetCodeKO_ACT set_status(BLZ_ASYNCH_SELECTOR_STATUS_ERROR); return blaze::RetCode_KO;

blaze::logger *selector::log_ = NULL;

selector::selector(peer_int &peer,
                   unsigned int id) :
    peer_(peer),
    id_(id),
    status_(BLZ_ASYNCH_SELECTOR_STATUS_TO_INIT),
    nfds_(-1),
    last_err_(blaze::RetCode_OK),
    sel_res_(-1),
    udp_ntfy_srv_socket_(INVALID_SOCKET),
    udp_ntfy_cli_socket_(INVALID_SOCKET),
    srv_listen_socket_(INVALID_SOCKET),
    srv_acceptor_(peer),
    srv_incoming_sock_map_(blaze::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    srv_exec_serv_(id*4, true),
    cli_early_outgoing_sock_map_(blaze::sngl_ptr_obj_mng(), sizeof(SOCKET)),
    cli_outgoing_sock_map_(blaze::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    cli_exec_serv_(id*5, true),
    write_pending_sockets_(blaze::sngl_ptr_obj_mng(), sizeof(SOCKET))
{
    log_ = blaze::logger::get_logger("asynch_selector");
    IFLOG(trc(TH_ID, LS_CTR "%s(selid:%d)", __func__, id))
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
    IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
    if(!(status_ <= BLZ_ASYNCH_SELECTOR_STATUS_INIT) &&
            !(status_ >= BLZ_ASYNCH_SELECTOR_STATUS_STOPPED)) {
        IFLOG(cri(TH_ID, LS_DTR
                  "%s(ptr:%p) - [selector is not in a safe state:%d] " LS_EXUNX, __func__, this,
                  status_))
    }
}

blaze::RetCode selector::init(unsigned int srv_executors,
                              unsigned int srv_pkt_q_len,
                              unsigned int cli_executors,
                              unsigned int cli_pkt_q_len)
{
    IFLOG(trc(TH_ID, LS_OPN
              "%s(srv_executors:%u, srv_pkt_q_len:%u, cli_executors:%u, cli_pkt_q_len:%u)",
              __func__,
              srv_executors,
              srv_pkt_q_len,
              cli_executors,
              cli_pkt_q_len))
    RETURN_IF_NOT_OK(srv_incoming_sock_map_.init(HM_SIZE_SMALL))
    RETURN_IF_NOT_OK(srv_acceptor_.set_sockaddr_in(srv_sockaddr_in_))
    RETURN_IF_NOT_OK(srv_exec_serv_.init(srv_executors, srv_pkt_q_len))
    RETURN_IF_NOT_OK(cli_early_outgoing_sock_map_.init(HM_SIZE_NANO))
    RETURN_IF_NOT_OK(cli_outgoing_sock_map_.init(HM_SIZE_SMALL))
    RETURN_IF_NOT_OK(cli_exec_serv_.init(cli_executors, cli_pkt_q_len))
    RETURN_IF_NOT_OK(create_UDP_notify_srv_sock())
    RETURN_IF_NOT_OK(connect_UDP_notify_cli_sock())
    RETURN_IF_NOT_OK(write_pending_sockets_.init(HM_SIZE_SMALL))
    set_status(BLZ_ASYNCH_SELECTOR_STATUS_INIT);
    IFLOG(trc(TH_ID, LS_CLO "%s(selid:%d)", __func__, id_))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::on_peer_start_actions()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(selid:%d)", __func__, id_))
    RETURN_IF_NOT_OK(last_err_ = start_exec_services())
    IFLOG(trc(TH_ID, LS_CLO "%s(selid:%d)", __func__, id_))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::on_peer_move_running_actions()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(selid:%d)", __func__, id_))
    RETURN_IF_NOT_OK(last_err_ = start_conn_objs())
    IFLOG(trc(TH_ID, LS_CLO "%s(selid:%d)", __func__, id_))
    return blaze::RetCode_OK;
}

peer_int &selector::peer()
{
    return peer_;
}

BLZ_ASYNCH_SELECTOR_STATUS selector::status() const
{
    return status_;
}

blaze::RetCode selector::set_status(BLZ_ASYNCH_SELECTOR_STATUS status)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(selid:%d, status:%d)", __func__, id_, status))
    CHK_MON_ERR_0(lock)
    status_ = status;
    CHK_MON_ERR_0(notify_all)
    CHK_MON_ERR_0(unlock)
    IFLOG(trc(TH_ID, LS_CLO "%s(selid:%d, status:%d)", __func__, id_, status))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::await_for_status_reached_or_outdated(
    BLZ_ASYNCH_SELECTOR_STATUS test,
    BLZ_ASYNCH_SELECTOR_STATUS &current,
    time_t sec,
    long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(selid:%d, status:%d)", __func__, id_, test))
    CHK_MON_ERR_0(lock)
    if(status_ <BLZ_ASYNCH_SELECTOR_STATUS_INIT) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADSTTS;
    }
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    while(status_ < test) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res =  blaze::RetCode_TIMEOUT;
                break;
            }
        }
    }
    current = status_;
    IFLOG(log(cdrs_res ? blaze::TL_WRN : blaze::TL_DBG, TH_ID,
              LS_CLO "%s(selid:%d) - test:%d [reached or outdated] current:%d", __func__,
              id_, test, status_))
    CHK_MON_ERR_0(unlock)
    return cdrs_res;
}

void selector::set_srv_sock_addr(sockaddr_in srv_sockaddr_in)
{
    srv_sockaddr_in_ = srv_sockaddr_in;
}

sockaddr_in selector::get_srv_sock_addr()
{
    return srv_sockaddr_in_;
}

void selector::set_select_timeout(timeval sel_timeout)
{
    sel_timeout_ = sel_timeout;
}

timeval selector::get_select_timeout()
{
    return sel_timeout_;
}

SOCKET selector::get_UDP_notify_srv_sock()
{
    return udp_ntfy_srv_socket_;
}

SOCKET selector::get_UDP_notify_cli_sock()
{
    return udp_ntfy_cli_socket_;
}

uint32_t selector::inco_conn_count()
{
    return srv_incoming_sock_map_.size();
}

uint32_t selector::outg_conn_count()
{
    return cli_early_outgoing_sock_map_.size() + cli_outgoing_sock_map_.size();
}

blaze::RetCode selector::create_UDP_notify_srv_sock()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(selid:%d)", __func__, id_))
    int res = 0, err = 0;
    if((udp_ntfy_srv_socket_ = socket(AF_INET, SOCK_DGRAM,
                                      IPPROTO_UDP)) != INVALID_SOCKET) {
        IFLOG(dbg(TH_ID, LS_TRL "%s(udp_ntfy_srv_socket_:%d) -socket OK-", __func__,
                  udp_ntfy_srv_socket_))
        if(!bind(udp_ntfy_srv_socket_, (sockaddr *)&udp_ntfy_sa_in_,
                 sizeof(udp_ntfy_sa_in_))) {
            IFLOG(dbg(TH_ID, LS_TRL "%s(udp_ntfy_srv_socket_:%d) -bind OK-", __func__,
                      udp_ntfy_srv_socket_))
#ifdef WIN32
            unsigned long mode = 1; //non-blocking mode
            if((res = ioctlsocket(udp_ntfy_srv_socket_, FIONBIO, &mode))) {
                err = WSAGetLastError();
                IFLOG(cri(TH_ID, LS_CLO "%s(udp_ntfy_srv_socket_:%d, err:%d) -ioctlsocket KO-",
                          __func__, udp_ntfy_srv_socket_, err))
                return blaze::RetCode_SYSERR;
            } else {
                IFLOG(dbg(TH_ID, LS_TRL "%s(udp_ntfy_srv_socket_:%d) -ioctlsocket OK- ",
                          __func__, udp_ntfy_srv_socket_))
            }
#else
            int flags = fcntl(udp_ntfy_srv_socket_, F_GETFL, 0);
            if(flags < 0) {
                return blaze::RetCode_KO;
            }
            flags = (flags|O_NONBLOCK);
            if((res = fcntl(udp_ntfy_srv_socket_, F_SETFL, flags))) {
                IFLOG(cri(TH_ID, LS_CLO "%s(udp_ntfy_srv_socket_:%d, err:%d) -fcntl KO-",
                          __func__, udp_ntfy_srv_socket_,
                          errno))
                return blaze::RetCode_SYSERR;
            } else {
                IFLOG(dbg(TH_ID, LS_TRL "%s(udp_ntfy_srv_socket_:%d) -fcntl OK-", __func__,
                          udp_ntfy_srv_socket_))
            }
#endif
        } else {
#ifdef WIN32
            err = WSAGetLastError();
#else
            err = errno;
#endif
            IFLOG(cri(TH_ID, LS_CLO "%s(udp_ntfy_srv_socket_:%d, err:%d) -bind KO-",
                      __func__, udp_ntfy_srv_socket_, err))
            return blaze::RetCode_SYSERR;
        }
    } else {
        IFLOG(cri(TH_ID, LS_CLO "%s(err:) -socket KO-", __func__, err))
        return blaze::RetCode_SYSERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::connect_UDP_notify_cli_sock()
{
    int err = 0;
    socklen_t len = sizeof(udp_ntfy_sa_in_);
    getsockname(udp_ntfy_srv_socket_, (struct sockaddr *)&udp_ntfy_sa_in_, &len);
    IFLOG(trc(TH_ID, LS_OPN "%s(selid:%d, addr:%s, port:%d)", __func__, id_,
              inet_ntoa(udp_ntfy_sa_in_.sin_addr),
              htons(udp_ntfy_sa_in_.sin_port)))
    if((udp_ntfy_cli_socket_ = socket(AF_INET, SOCK_DGRAM, 0)) != INVALID_SOCKET) {
        IFLOG(dbg(TH_ID, LS_TRL "%s(udp_ntfy_cli_socket_:%d) -socket OK-", __func__,
                  udp_ntfy_cli_socket_))
        if((connect(udp_ntfy_cli_socket_, (struct sockaddr *)&udp_ntfy_sa_in_,
                    sizeof(udp_ntfy_sa_in_))) != INVALID_SOCKET) {
            IFLOG(dbg(TH_ID, LS_TRL "%s(udp_ntfy_cli_socket_:%d) -connect OK-", __func__,
                      udp_ntfy_cli_socket_))
        } else {
#ifdef WIN32
            err = WSAGetLastError();
#endif
            IFLOG(cri(TH_ID, LS_CLO "%s(udp_ntfy_cli_socket_:%d, err:%d) -connect KO-",
                      __func__, udp_ntfy_cli_socket_, err))
            return blaze::RetCode_SYSERR;
        }
    } else {
        IFLOG(cri(TH_ID, LS_CLO "%s() -socket KO-", __func__))
        return blaze::RetCode_SYSERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::evt_enqueue_and_notify(const selector_event *evt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, evt:%p, connid:%u)", __func__, this, evt,
              evt->connid_))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    cdrs_res = asynch_notify(evt);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

blaze::RetCode selector::interrupt()
{
    selector_event *interrupt = new selector_event(
        BLZ_SELECTOR_Evt_Interrupt,
        NULL);
    return asynch_notify(interrupt);
}

blaze::RetCode selector::asynch_notify(const selector_event *evt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(evt:%p, connid:%u)", __func__, evt, evt->connid_))
    long bsent = 0;
    while((bsent = send(udp_ntfy_cli_socket_, (const char *)&evt, sizeof(void *),
                        0)) == SOCKET_ERROR) {
        int err = 0;
#ifdef WIN32
        err = WSAGetLastError();
#else
        err = errno;
#endif
#ifdef WIN32
        if(err == WSAEWOULDBLOCK) {
#else
        if(err == EAGAIN || err == EWOULDBLOCK) {
#endif
            //ok we can go ahead
#ifdef WIN32
        } else if(err == WSAECONNRESET) {
#else
        } else if(err == ECONNRESET) {
#endif
            IFLOG(err(TH_ID, LS_CLO "%s(errcode:%d, udp_ntfy_cli_socket_:%d)", __func__,
                      err,
                      udp_ntfy_cli_socket_))
            return blaze::RetCode_KO;
        } else {
            perror(__func__);
            IFLOG(cri(TH_ID, LS_CLO "%s(errno: %d, err:%d)", __func__, errno, err))
            return blaze::RetCode_SYSERR;
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(bytesent:%d)", __func__, bsent))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::start_exec_services()
{
    blaze::PEXEC_SERVICE_STATUS current = blaze::PEXEC_SERVICE_STATUS_ZERO;
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    if(peer_.personality_ == PeerPersonality_PURE_SERVER ||
            peer_.personality_ == PeerPersonality_BOTH) {
        IFLOG(dbg(TH_ID, LS_TRL "%s() - starting server side executor service..",
                  __func__))
        if((last_err_ = srv_exec_serv_.start())) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - starting server side ,  last_err:%d", __func__,
                      last_err_))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        srv_exec_serv_.await_for_status_reached_or_outdated(
            blaze::PEXEC_SERVICE_STATUS_STARTED, current);
        IFLOG(dbg(TH_ID, LS_TRL "%s() - server side executor service started.",
                  __func__))
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT ||
            peer_.personality_ == PeerPersonality_BOTH) {
        IFLOG(dbg(TH_ID, LS_TRL "%s() - starting client side executor service..",
                  __func__))
        if((last_err_ = cli_exec_serv_.start())) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - starting client side ,  last_err:%d", __func__,
                      last_err_))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        cli_exec_serv_.await_for_status_reached_or_outdated(
            blaze::PEXEC_SERVICE_STATUS_STARTED, current);
        IFLOG(dbg(TH_ID, LS_TRL "%s() - client side executor service started.",
                  __func__))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::start_conn_objs()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    if(peer_.personality_ == PeerPersonality_PURE_SERVER ||
            peer_.personality_ == PeerPersonality_BOTH) {
        if((last_err_ = srv_acceptor_.create_server_socket(srv_listen_socket_))) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - starting acceptor, last_err:%d", __func__,
                      last_err_))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        FD_SET(srv_listen_socket_, &read_FDs_);
        FD_SET(srv_listen_socket_, &excep_FDs_);
        nfds_ = (int)srv_listen_socket_;
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT ||
            peer_.personality_ == PeerPersonality_BOTH) {
        //???
    }
    FD_SET(udp_ntfy_srv_socket_, &read_FDs_);
    FD_SET(udp_ntfy_srv_socket_, &excep_FDs_);
    nfds_ = ((int)udp_ntfy_srv_socket_ > nfds_) ? (int)udp_ntfy_srv_socket_ : nfds_;
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::process_inco_sock_inco_events()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    SOCKET srv_cli_sock = INVALID_SOCKET;
    connection_int *srv_cli_conn = NULL;
    unsigned int connid = 0;
    blaze::RetCode cdrs_res = blaze::RetCode_OK, sub_res = blaze::RetCode_OK;
    //**** HANDLE INCOMING EVENTS BEGIN****
    if(sel_res_) {
        srv_incoming_sock_map_.start_iteration();
        while(!srv_incoming_sock_map_.next(&connid, &srv_cli_conn)) {
            srv_cli_sock = srv_cli_conn->get_socket();
            //first: check if it is readable.
            if(FD_ISSET(srv_cli_sock, &read_FDs_)) {
                IFLOG(trc(TH_ID, LS_TRL
                          "%s(sockid:%d, connid:%d) - server-side socket became readable.", __func__,
                          srv_cli_sock,
                          connid))
                if(srv_cli_conn->status() != ConnectionStatus_ESTABLISHED &&
                        srv_cli_conn->status() != ConnectionStatus_PROTOCOL_HANDSHAKE &&
                        srv_cli_conn->status() != ConnectionStatus_AUTHENTICATED) {
                    IFLOG(trc(TH_ID, LS_TRL
                              "%s(sockid:%d, connid:%d, status:%d) not eligible for recv().", __func__,
                              srv_cli_sock, connid,
                              srv_cli_conn->status()))
                    break;
                }
                blz_hdr_rec *pkt_hdr = new blz_hdr_rec();
                DECLINITH_GBB(pkt_body, BLZ_RECV_BUFF_SZ)
                //read data from socket.
                if(!(cdrs_res = srv_cli_conn->recv_single_pkt(pkt_hdr, pkt_body))) {
                    blaze::p_task *task = NULL;
                    pkt_body->flip();
                    if((task = peer_.new_peer_recv_task(*srv_cli_conn, pkt_hdr, pkt_body))) {
                        IFLOG(trc(TH_ID, LS_TRL
                                  "%s(selid:%d, sockid:%d, connid:%d) - sending task to execution service.",
                                  __func__, id_,
                                  srv_cli_sock, connid))
                        if((sub_res = srv_exec_serv_.submit(task))) {
                            IFLOG(cri(TH_ID, LS_TRL
                                      "%s(selid:%d, sockid:%d, connid:%d) - submit failed, res:%d.", __func__, id_,
                                      srv_cli_sock,
                                      connid,
                                      sub_res))
                        } else {
                            IFLOG(trc(TH_ID, LS_TRL "%s(selid:%d, sockid:%d, connid:%d) - task sent.",
                                      __func__, id_, srv_cli_sock, connid))
                        }
                    } else {
                        IFLOG(cri(TH_ID, LS_TRL "%s(selid:%d, sockid:%d, connid:%d) - new task failed.",
                                  __func__, id_, srv_cli_sock, connid))
                    }
                }
                if(!(--sel_res_)) {
                    break;
                }
            }
            //second: check if it is in exception.
            if(FD_ISSET(srv_cli_sock, &excep_FDs_)) {
                IFLOG(wrn(TH_ID, LS_TRL "%s(sockid:%d) - server-side socket is in exception.",
                          __func__, srv_cli_sock))
                if(!(--sel_res_)) {
                    break;
                }
            }
        }
    }
    //**** HANDLE INCOMING EVENTS END****
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::process_sock_outg_events()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    SOCKET write_pending_sock = INVALID_SOCKET;
    connection_int *wp_conn = NULL;
    write_pending_sockets_.start_iteration();
    //**** HANDLE OUTGOING EVENTS BEGIN****
    while(!write_pending_sockets_.next(&write_pending_sock, &wp_conn)) {
        if(FD_ISSET(write_pending_sock, &write_FDs_)) {
            IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d) - is writepending.", __func__,
                      write_pending_sock,
                      wp_conn->connid()))
            blaze::grow_byte_buffer *sending_pkt = NULL;
            if(wp_conn->pkt_snd_q().get(&sending_pkt)) {
                IFLOG(cri(TH_ID, LS_CLO "%s() - reading from packet queue.", __func__))
                SET_ERROR_AND_RETURRetCodeKO_ACT
            }
            //read data from socket.
            if((cdrs_res = wp_conn->send_single_pkt(sending_pkt))) {
                IFLOG(err(TH_ID, LS_TRL
                          "%s(sockid:%d, connid:%d) - failed sending packet on this writepending socket.",
                          __func__,
                          write_pending_sock, wp_conn->connid()))
            }
            delete sending_pkt;
            if(!(--sel_res_)) {
                break;
            }
        }
        //second: check if it is in exception.
        if(FD_ISSET(write_pending_sock, &excep_FDs_)) {
            IFLOG(wrn(TH_ID, LS_TRL
                      "%s(sockid:%d, connid:%d) - writepending socket is in exception.", __func__,
                      write_pending_sock,
                      wp_conn->connid()))
            if(!(--sel_res_)) {
                break;
            }
        }
    }
    //**** HANDLE OUTGOING EVENTS END****
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::process_outg_sock_inco_events()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    SOCKET outg_sock = INVALID_SOCKET;
    connection_int *outg_conn = NULL;
    blaze::p_task *task = NULL;
    unsigned int connid = 0;
    blaze::RetCode cdrs_res = blaze::RetCode_OK, sub_res = blaze::RetCode_OK;
    //**** HANDLE INCOMING EVENTS BEGIN****
    //EARLY CONNECTIONS
    if(sel_res_) {
        cli_early_outgoing_sock_map_.start_iteration();
        while(!cli_early_outgoing_sock_map_.next(&outg_sock, &outg_conn)) {
            //first: check if it is readable.
            if(FD_ISSET(outg_sock, &read_FDs_)) {
                IFLOG(trc(TH_ID, LS_TRL
                          "%s(sockid:%d) - early-outgoing socket became readable.", __func__, outg_sock))
                if(outg_conn->status() != ConnectionStatus_ESTABLISHED &&
                        outg_conn->status() != ConnectionStatus_PROTOCOL_HANDSHAKE &&
                        outg_conn->status() != ConnectionStatus_AUTHENTICATED) {
                    IFLOG(trc(TH_ID, LS_TRL
                              "%s(sockid:%d, connid:%d, status:%d) - not eligible for recv().", __func__,
                              outg_sock, connid,
                              outg_conn->status()))
                    break;
                }
                blz_hdr_rec *pkt_hdr = new blz_hdr_rec();
                DECLINITH_GBB(pkt_body, BLZ_RECV_BUFF_SZ)
                //read data from socket.
                if(!(cdrs_res = outg_conn->recv_single_pkt(pkt_hdr, pkt_body))) {
                    pkt_body->flip();
                    if((task = peer_.new_peer_recv_task(*outg_conn, pkt_hdr, pkt_body))) {
                        IFLOG(trc(TH_ID, LS_TRL
                                  "%s(selid:%d, sockid:%d) - sending task to execution service.", __func__, id_,
                                  outg_sock))
                        if((sub_res = cli_exec_serv_.submit(task))) {
                            IFLOG(cri(TH_ID, LS_TRL "%s(selid:%d, sockid:%d) - submit failed, res:%d.",
                                      __func__, id_, outg_sock, sub_res))
                        } else {
                            IFLOG(dbg(TH_ID, LS_TRL "%s(selid:%d, sockid:%d) - task sent.", __func__, id_,
                                      outg_sock))
                        }
                    } else {
                        IFLOG(cri(TH_ID, LS_TRL "%s(selid:%d, sockid:%d) - new task failed.", __func__,
                                  id_, outg_sock))
                    }
                }
                if(!(--sel_res_)) {
                    break;
                }
            }
            //second: check if it is in exception.
            if(FD_ISSET(outg_sock, &excep_FDs_)) {
                IFLOG(wrn(TH_ID, LS_TRL
                          "%s(sockid:%d) - early-client-side socket is in exception.", __func__,
                          outg_sock))
                if(!(--sel_res_)) {
                    break;
                }
            }
        }
    }
    //PROTO CONNECTED CONNECTIONS
    if(sel_res_) {
        cli_outgoing_sock_map_.start_iteration();
        while(!cli_outgoing_sock_map_.next(&connid, &outg_conn)) {
            outg_sock = outg_conn->get_socket();
            //first: check if it is readable.
            if(FD_ISSET(outg_sock, &read_FDs_)) {
                IFLOG(trc(TH_ID, LS_TRL
                          "%s(sockid:%d, connid:%d) - client-side socket became readable.", __func__,
                          outg_sock, connid))
                if(outg_conn->status() != ConnectionStatus_ESTABLISHED &&
                        outg_conn->status() != ConnectionStatus_PROTOCOL_HANDSHAKE &&
                        outg_conn->status() != ConnectionStatus_AUTHENTICATED) {
                    IFLOG(trc(TH_ID, LS_TRL
                              "%s(sockid:%d, connid:%d, status:%d) - not eligible for recv().", __func__,
                              outg_sock, connid,
                              outg_conn->status()))
                    break;
                }
                blz_hdr_rec *pkt_hdr = new blz_hdr_rec();
                DECLINITH_GBB(pkt_body, BLZ_RECV_BUFF_SZ)
                //read data from socket.
                if(!(cdrs_res = outg_conn->recv_single_pkt(pkt_hdr, pkt_body))) {
                    pkt_body->flip();
                    if((task = peer_.new_peer_recv_task(*outg_conn, pkt_hdr, pkt_body))) {
                        IFLOG(trc(TH_ID, LS_TRL
                                  "%s(selid:%d, sockid:%d, connid:%d) - sending task to execution service.",
                                  __func__, id_,
                                  outg_sock,
                                  connid))
                        if((sub_res = cli_exec_serv_.submit(task))) {
                            IFLOG(cri(TH_ID, LS_TRL
                                      "%s(selid:%d, sockid:%d, connid:%d) - submit failed, res:%d.", __func__, id_,
                                      outg_sock, connid,
                                      sub_res))
                        } else {
                            IFLOG(dbg(TH_ID, LS_TRL "%s(selid:%d, sockid:%d, connid:%d) - task sent.",
                                      __func__, id_, outg_sock, connid))
                        }
                    } else {
                        IFLOG(cri(TH_ID, LS_TRL "%s(selid:%d, sockid:%d, connid:%d) - new task failed.",
                                  __func__, id_, outg_sock, connid))
                    }
                }
                if(!(--sel_res_)) {
                    break;
                }
            }
            //second: check if it is in exception.
            if(FD_ISSET(outg_sock, &excep_FDs_)) {
                IFLOG(wrn(TH_ID, LS_TRL "%s(sockid:%d) - client-side socket is in exception.",
                          __func__, outg_sock))
                if(!(--sel_res_)) {
                    break;
                }
            }
        }
        //**** HANDLE INCOMING EVENTS END****
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

inline blaze::RetCode selector::consume_inco_sock_events()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    connection_int *inc_conn = NULL;
    unsigned int new_connid = 0;
    if(FD_ISSET(srv_listen_socket_, &read_FDs_)) {
        if(peer_.next_connid(new_connid)) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - generating client connection id.", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        if(srv_acceptor_.accept(new_connid, &inc_conn)) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - accepting new connection.", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        if(inc_conn->set_socket_blocking_mode(false)) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - set socket not blocking.", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        if(srv_incoming_sock_map_.put(&new_connid, &inc_conn)) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - storing new connection.", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        IFLOG(dbg(TH_ID, LS_CON
                  "%s(sockid:%d, host:%s, port:%d, candidate connid:%d) - *socket accepted*.",
                  __func__,
                  inc_conn->get_socket(), inc_conn->get_host_ip(), inc_conn->get_host_port(),
                  inc_conn->connid()))
        --sel_res_;
        if(sel_res_) {
            //in addition to the newly connection, there is also some other message incoming on client sockets.
            IFLOG(trc(TH_ID, LS_SEL "%s() - some events need to be processed. -1-",
                      __func__))
            if(process_inco_sock_inco_events()) {
                IFLOG(cri(TH_ID, LS_CLO "%s() - failed.", __func__))
                SET_ERROR_AND_RETURRetCodeKO_ACT
            }
        }
    } else {
        //we have to handle incoming messages on client sockets
        IFLOG(trc(TH_ID, LS_SEL "%s() - some events need to be processed. -2-",
                  __func__))
        if(process_inco_sock_inco_events()) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - failed.", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

inline blaze::RetCode selector::FDSET_sockets()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    FD_ZERO(&read_FDs_);
    FD_ZERO(&write_FDs_);
    FD_ZERO(&excep_FDs_);
    if(peer_.personality_ == PeerPersonality_PURE_SERVER ||
            peer_.personality_ == PeerPersonality_BOTH) {
        RETURN_IF_NOT_OK(FDSET_incoming_sockets())
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT ||
            peer_.personality_ == PeerPersonality_BOTH) {
        RETURN_IF_NOT_OK(FDSET_outgoing_sockets())
    }
    FDSET_write_pending_sockets();
    FD_SET(udp_ntfy_srv_socket_, &read_FDs_);
    FD_SET(udp_ntfy_srv_socket_, &excep_FDs_);
    nfds_ = ((int)udp_ntfy_srv_socket_ > nfds_) ? (int)udp_ntfy_srv_socket_ : nfds_;
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

inline blaze::RetCode selector::FDSET_write_pending_sockets()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    SOCKET write_pending_sock = INVALID_SOCKET;
    connection_int *wp_conn = NULL;
    write_pending_sockets_.start_iteration();
    while(!write_pending_sockets_.next(&write_pending_sock, &wp_conn)) {
        if(wp_conn->pkt_snd_q().size()) {
            IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d) - [w+]", __func__,
                      write_pending_sock, wp_conn->connid()))
            FD_SET(write_pending_sock, &write_FDs_);
            nfds_ = ((int)write_pending_sock > nfds_) ? (int)write_pending_sock : nfds_;
        } else {
            IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d) - [w-]", __func__,
                      write_pending_sock, wp_conn->connid()))
            if(write_pending_sockets_.remove_in_iteration()) {
                IFLOG(cri(TH_ID, LS_CLO "%s() - failed.", __func__))
                SET_ERROR_AND_RETURRetCodeKO_ACT
            }
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

inline blaze::RetCode selector::FDSET_incoming_sockets()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    srv_incoming_sock_map_.start_iteration();
    connection_int *inco_conn = NULL;
    while(!srv_incoming_sock_map_.next(NULL, &inco_conn)) {
        if(inco_conn->status() == ConnectionStatus_ESTABLISHED ||
                inco_conn->status() == ConnectionStatus_PROTOCOL_HANDSHAKE ||
                inco_conn->status() == ConnectionStatus_AUTHENTICATED) {
            SOCKET inco_sock = inco_conn->get_socket();
            IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d) - [<-][s+]", __func__,
                      inco_sock, inco_conn->connid()))
            FD_SET(inco_sock, &read_FDs_);
            FD_SET(inco_sock, &excep_FDs_);
            nfds_ = ((int)inco_sock > nfds_) ? (int)inco_sock : nfds_;
        } else {
            IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d, status:%d) - [<-][s-]",
                      __func__,
                      inco_conn->get_socket(),
                      inco_conn->connid(),
                      inco_conn->status()))
            if(srv_incoming_sock_map_.remove_in_iteration()) {
                IFLOG(cri(TH_ID, LS_CLO "%s() - failed.", __func__))
                SET_ERROR_AND_RETURRetCodeKO_ACT
            }
            SOCKET inco_sock = inco_conn->get_socket();
            if(!write_pending_sockets_.remove(&inco_sock, NULL)) {
                IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d) - [<-][w-]", __func__,
                          inco_sock, inco_conn->connid()))
            }
            if(inco_conn->status() != ConnectionStatus_DISCONNECTED) {
                inco_conn->socket_shutdown();
            }
            /**********************************************************
            autorelease feature for incoming [server side] connections.
            **********************************************************/
            /************************
            RELEASE_ID: CONN_SRV_01
            ************************/
            blaze::collector &c = inco_conn->get_collector();
            c.release(inco_conn);
        }
    }
    //always set server socket in read and exception fds.
    FD_SET(srv_listen_socket_, &read_FDs_);
    FD_SET(srv_listen_socket_, &excep_FDs_);
    nfds_ = ((int)srv_listen_socket_ > nfds_) ? (int)srv_listen_socket_ : nfds_;
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}
inline blaze::RetCode selector::FDSET_outgoing_sockets()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    connection_int *outg_conn = NULL;
    SOCKET outg_sock = INVALID_SOCKET;
    cli_early_outgoing_sock_map_.start_iteration();
    while(!cli_early_outgoing_sock_map_.next(NULL, &outg_conn)) {
        if(outg_conn->status() == ConnectionStatus_ESTABLISHED ||
                outg_conn->status() == ConnectionStatus_PROTOCOL_HANDSHAKE ||
                outg_conn->status() == ConnectionStatus_AUTHENTICATED) {
            outg_sock = outg_conn->get_socket();
            IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d) - [early][->][s+]", __func__,
                      outg_sock, outg_conn->connid()))
            FD_SET(outg_sock, &read_FDs_);
            FD_SET(outg_sock, &excep_FDs_);
            nfds_ = ((int)outg_sock > nfds_) ? (int)outg_sock : nfds_;
        } else {
            IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d, status:%d) - [early][->][s-]",
                      __func__,
                      outg_conn->get_socket(),
                      outg_conn->connid(),
                      outg_conn->status()))
            if(cli_early_outgoing_sock_map_.remove_in_iteration()) {
                IFLOG(cri(TH_ID, LS_CLO "%s() - failed.", __func__))
                SET_ERROR_AND_RETURRetCodeKO_ACT
            }
            if(!write_pending_sockets_.remove(&outg_sock, NULL)) {
                IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d) - [early][->][w-]", __func__,
                          outg_sock, outg_conn->connid()))
            }
            if(outg_conn->status() != ConnectionStatus_DISCONNECTED) {
                outg_conn->socket_shutdown();
            }
            /*for client connections we do not autorelease inactive connections.
              this because it is up to caller to applicatively release them.
              We only release them from collector, allowing others to destroy them.*/
            /************************
             RELEASE_ID: CONN_CLI_01
            ************************/
            blaze::collector &c = outg_conn->get_collector();
            c.release(outg_conn);
        }
    }
    cli_outgoing_sock_map_.start_iteration();
    while(!cli_outgoing_sock_map_.next(NULL, &outg_conn)) {
        if(outg_conn->status() == ConnectionStatus_ESTABLISHED ||
                outg_conn->status() == ConnectionStatus_PROTOCOL_HANDSHAKE ||
                outg_conn->status() == ConnectionStatus_AUTHENTICATED) {
            outg_sock = outg_conn->get_socket();
            IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d) - [->][s+]", __func__,
                      outg_sock, outg_conn->connid()))
            FD_SET(outg_sock, &read_FDs_);
            FD_SET(outg_sock, &excep_FDs_);
            nfds_ = ((int)outg_sock > nfds_) ? (int)outg_sock : nfds_;
        } else {
            IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d, status:%d) - [->][s-]",
                      __func__,
                      outg_conn->get_socket(),
                      outg_conn->connid(),
                      outg_conn->status()))
            if(cli_outgoing_sock_map_.remove_in_iteration()) {
                IFLOG(cri(TH_ID, LS_CLO "%s() - failed.", __func__))
                SET_ERROR_AND_RETURRetCodeKO_ACT
            }
            if(!write_pending_sockets_.remove(&outg_sock, NULL)) {
                IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, connid:%d) - [->][w-]", __func__,
                          outg_sock, outg_conn->connid()))
            }
            if(outg_conn->status() != ConnectionStatus_DISCONNECTED) {
                outg_conn->socket_shutdown();
            }
            /*for client connections we do not autorelease inactive connections.
              this because it is up to caller to applicatively release them.
              We only release them from collector, allowing others to destroy them.*/
            /************************
             RELEASE_ID: CONN_CLI_01
            ************************/
            blaze::collector &c = outg_conn->get_collector();
            c.release(outg_conn);
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

inline blaze::RetCode selector::manage_disconnect_conn(
    selector_event *conn_evt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    blaze::grow_byte_buffer *sending_pkt = NULL;
    if(conn_evt->conn_->pkt_snd_q().get(&sending_pkt)) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - reading from queue.", __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    if((cdrs_res = conn_evt->conn_->send_single_pkt(sending_pkt))) {
        IFLOG(err(TH_ID, LS_TRL "%s(sockid:%d) - failed sending disconnection packet.",
                  __func__,
                  conn_evt->conn_->get_socket()))
        delete sending_pkt;
        return blaze::RetCode_KO;
    }
    delete sending_pkt;
    conn_evt->conn_->clean_best_effort();
    conn_evt->conn_->socket_shutdown();
    conn_evt->conn_->on_disconnect(ConnectivityEventResult_OK,
                                   ConnectivityEventType_APPLICATIVE);
    conn_evt->conn_->notify_for_connectivity_result(
        ConnectivityEventResult_OK,
        ConnectivityEventType_APPLICATIVE);
    IFLOG(trc(TH_ID, LS_CLO "%s%s", __func__, "(res:%d)", cdrs_res))
    return cdrs_res;
}


inline blaze::RetCode selector::add_early_outg_conn(selector_event *conn_evt)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    blaze::grow_byte_buffer *sending_pkt = NULL;
    if((cdrs_res = conn_evt->conn_->establish_connection(conn_evt->saddr_))) {
        if(conn_evt->conn_->pkt_snd_q().get(&sending_pkt)) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - reading from queue.", __func__))
            SET_ERROR_AND_RETURRetCodeKO_ACT
        }
        delete sending_pkt;
        return cdrs_res;
    }
    if(conn_evt->conn_->set_socket_blocking_mode(false)) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - setting socket not blocking.", __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    SOCKET new_conn_socket = conn_evt->conn_->get_socket();
    if(conn_evt->conn_->pkt_snd_q().get(&sending_pkt)) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - reading from queue.", __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    if((cdrs_res = conn_evt->conn_->send_single_pkt(sending_pkt))) {
        IFLOG(err(TH_ID, LS_TRL "%s(sockid:%d) - failed sending conn-req packet.",
                  __func__, new_conn_socket))
        delete sending_pkt;
        return cdrs_res;
    }
    delete sending_pkt;
    if(cli_early_outgoing_sock_map_.put(&new_conn_socket, &conn_evt->conn_)) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - storing new early outgoing connection.",
                  __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

inline blaze::RetCode selector::promote_early_outg_conn(connection_int *conn)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    SOCKET conn_socket = conn->get_socket();
    unsigned int connid = conn->connid();
    if(cli_early_outgoing_sock_map_.remove(&conn_socket, NULL)) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - removing entry from early_cli_srv_sock_map_.",
                  __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    if(cli_outgoing_sock_map_.put(&connid, &conn)) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - adding entry to srv_outgoing_sock_map_.",
                  __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

inline blaze::RetCode selector::delete_early_outg_conn(connection_int *conn)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    SOCKET conn_socket = conn->get_socket();
    if(cli_early_outgoing_sock_map_.remove(&conn_socket, NULL)) {
        IFLOG(cri(TH_ID, LS_CLO "%s() - removing entry from early_cli_srv_sock_map_.",
                  __func__))
        SET_ERROR_AND_RETURRetCodeKO_ACT
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

inline bool selector::is_still_valid_connection(const selector_event
                                                *evt)
{
    if(evt->con_type_ == ConnectionType_INGOING) {
        return (srv_incoming_sock_map_.contains_key(&evt->connid_) ==
                blaze::RetCode_OK);
    } else {
        if(evt->evt_ == BLZ_SELECTOR_Evt_ConnReqAccepted ||
                evt->evt_ == BLZ_SELECTOR_Evt_ConnReqRefused) {
            return (cli_early_outgoing_sock_map_.contains_key(&evt->socket_) ==
                    blaze::RetCode_OK);
        } else {
            return (cli_outgoing_sock_map_.contains_key(&evt->connid_) ==
                    blaze::RetCode_OK);
        }
    }
}

blaze::RetCode selector::consume_asynch_events()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    long brecv = 0, recv_buf_sz = sizeof(void *);
    selector_event *conn_evt = NULL;
    bool conn_still_valid = true;
    while((brecv = recv(udp_ntfy_srv_socket_, (char *)&conn_evt, recv_buf_sz,
                        0)) > 0) {
        if(brecv != recv_buf_sz) {
            IFLOG(cri(TH_ID, LS_CLO "%s() - (brecv != recv_buf_sz)", __func__))
            return blaze::RetCode_GENERR;
        }
        IFLOG(trc(TH_ID,
                  LS_TRL"ASYNCH-EVENT:[evt:%p, evt_type:%d, conn:%p, socket:%d, connid:%u]",
                  conn_evt,
                  conn_evt->evt_,
                  conn_evt->conn_,
                  conn_evt->socket_,
                  conn_evt->connid_))
        if(conn_evt->evt_ != BLZ_SELECTOR_Evt_Interrupt) {
            /*check if we still have connection*/
            if(conn_evt->evt_ != BLZ_SELECTOR_Evt_ConnectRequest) {
                conn_still_valid = is_still_valid_connection(conn_evt);
            }
            if(conn_still_valid) {
                switch(conn_evt->evt_) {
                    case BLZ_SELECTOR_Evt_SendPacket:
                        if(write_pending_sockets_.put(&conn_evt->socket_, &(conn_evt->conn_))) {
                            IFLOG(cri(TH_ID, LS_CLO "%s() - storing new connection.", __func__))
                            SET_ERROR_AND_RETURRetCodeKO_ACT
                        }
                        break;
                    case BLZ_SELECTOR_Evt_ConnectRequest:     /*can originate in client peer only*/
                        add_early_outg_conn(conn_evt);
                        break;
                    case BLZ_SELECTOR_Evt_Disconnect:         /*can originate in both peer*/
                        manage_disconnect_conn(conn_evt);
                        break;
                    case BLZ_SELECTOR_Evt_ConnReqAccepted:    /*can originate in client peer only*/
                        promote_early_outg_conn(conn_evt->conn_);
                        break;
                    case BLZ_SELECTOR_Evt_ConnReqRefused:     /*can originate in client peer only*/
                        delete_early_outg_conn(conn_evt->conn_);
                        break;
                    case BLZ_SELECTOR_Evt_Inactivity:         /*can originate in both peer*/
                        //@todo
                        break;
                    default:
                        IFLOG(cri(TH_ID, LS_CLO "%s() - unk. evt.", __func__))
                        break;
                }
            } else {
                IFLOG(inf(TH_ID, LS_TRL
                          "%s() - [connection(%p, socket:%d) is no longer valid.]", __func__,
                          conn_evt->conn_,
                          conn_evt->socket_))
            }
        }
        delete conn_evt;
    }
    if(brecv == SOCKET_ERROR) {
        int err = 0;
#ifdef WIN32
        err = WSAGetLastError();
#else
        err = errno;
#endif
#ifdef WIN32
        if(err == WSAEWOULDBLOCK) {
#else
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
#endif
            //ok we can go ahead
#ifdef WIN32
        } else if(errno == WSAECONNRESET) {
#else
        } else if(errno == ECONNRESET) {
#endif
            IFLOG(err(TH_ID, LS_CLO "%s(err:%d)", __func__, err))
            return blaze::RetCode_KO;
        } else {
            perror(__func__);
            IFLOG(cri(TH_ID, LS_CLO "%s(err:%d)", __func__, err))
            return blaze::RetCode_SYSERR;
        }
    }
    if(!brecv) {
        return blaze::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

inline blaze::RetCode selector::consume_events()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    if(FD_ISSET(udp_ntfy_srv_socket_, &read_FDs_)) {
        IFLOG(trc(TH_ID, LS_TRL "%s() - asynh event occurred.", __func__))
        RETURN_IF_NOT_OK(consume_asynch_events())
        sel_res_--;
    }
    if(sel_res_) {
        if(peer_.personality_ == PeerPersonality_PURE_SERVER ||
                peer_.personality_ == PeerPersonality_BOTH) {
            RETURN_IF_NOT_OK(consume_inco_sock_events())
        }
        if(peer_.personality_ == PeerPersonality_PURE_CLIENT ||
                peer_.personality_ == PeerPersonality_BOTH) {
            if(sel_res_) {
                RETURN_IF_NOT_OK(process_outg_sock_inco_events())
            }
        }
    }
    if(sel_res_) {
        RETURN_IF_NOT_OK(process_sock_outg_events())
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::server_socket_shutdown()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d)", __func__, srv_listen_socket_))
    int last_err_ = 0;
#ifdef WIN32
    if((last_err_ = closesocket(srv_listen_socket_))) {
        IFLOG(err(TH_ID, LS_CLO "%s(sockid:%d, res:%d) -closesocket KO-", __func__,
                  srv_listen_socket_, last_err_))
    } else {
        IFLOG(dbg(TH_ID, LS_TRL "%s(sockid:%d, res:%d) -closesocket OK-", __func__,
                  srv_listen_socket_, last_err_))
    }
#else
    if((last_err_ = close(srv_listen_socket_))) {
        IFLOG(err(TH_ID, LS_CLO "%s(sockid:%d, res:%d) -close KO-", __func__,
                  srv_listen_socket_, last_err_))
    } else {
        IFLOG(dbg(TH_ID, LS_TRL "%s(sockid:%d, res:%d) -close OK-", __func__,
                  srv_listen_socket_, last_err_))
    }
#if 0
    if((last_err_ = shutdown(srv_listen_socket_, SHUT_RDWR))) {
        IFLOG(err(TH_ID, LS_CLO "%s(sockid:%d, res:%d) -shutdown KO-", __func__,
                  srv_listen_socket_, last_err_))
    } else {
        IFLOG(dbg(TH_ID, LS_TRL "%s(sockid:%d, res:%d) -shutdown OK-", __func__,
                  srv_listen_socket_, last_err_))
    }
#endif
#endif
    IFLOG(trc(TH_ID, LS_CLO "%s(sockid:%d)", __func__, srv_listen_socket_))
    return blaze::RetCode_OK;
}

blaze::RetCode selector::stop_and_clean()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    connection_int *conn = NULL;
    if(peer_.personality_ == PeerPersonality_PURE_SERVER ||
            peer_.personality_ == PeerPersonality_BOTH) {
        srv_incoming_sock_map_.start_iteration();
        while(!srv_incoming_sock_map_.next(NULL, &conn)) {
            conn->clean_best_effort();
            if(conn->status() != ConnectionStatus_DISCONNECTED) {
                conn->socket_shutdown();
                conn->on_disconnect(ConnectivityEventResult_OK,
                                    ConnectivityEventType_APPLICATIVE);
                conn->notify_for_connectivity_result(ConnectivityEventResult_OK,
                                                     ConnectivityEventType_APPLICATIVE);
            }
            /**********************************************************
            autorelease feature for incoming [server side] connections.
            **********************************************************/
            /************************
            RELEASE_ID: CONN_SRV_01
            ************************/
            blaze::collector &c = conn->get_collector();
            c.release(conn);
            srv_incoming_sock_map_.remove_in_iteration();
        }
        server_socket_shutdown();
        srv_exec_serv_.shutdown();
        srv_exec_serv_.await_termination();
        IFLOG(dbg(TH_ID, LS_TRL "%s() - srv_exec_serv_ terminated.", __func__))
    }
    if(peer_.personality_ == PeerPersonality_PURE_CLIENT ||
            peer_.personality_ == PeerPersonality_BOTH) {
        cli_outgoing_sock_map_.start_iteration();
        while(!cli_outgoing_sock_map_.next(NULL, &conn)) {
            conn->clean_best_effort();
            if(conn->status() != ConnectionStatus_DISCONNECTED) {
                conn->socket_shutdown();
                conn->on_disconnect(ConnectivityEventResult_OK,
                                    ConnectivityEventType_APPLICATIVE);
                conn->notify_for_connectivity_result(ConnectivityEventResult_OK,
                                                     ConnectivityEventType_APPLICATIVE);
            }
            /*for client connections we do not autorelease inactive connections.
              this because it is up to caller to applicatively release them.
              We only release them from collector, allowing others to destroy them.*/
            /************************
             RELEASE_ID: CONN_CLI_01
            ************************/
            blaze::collector &c = conn->get_collector();
            c.release(conn);
            cli_outgoing_sock_map_.remove_in_iteration();
        }
        cli_early_outgoing_sock_map_.start_iteration();
        while(!cli_early_outgoing_sock_map_.next(NULL, &conn)) {
            conn->clean_best_effort();
            if(conn->status() != ConnectionStatus_DISCONNECTED) {
                conn->socket_shutdown();
                conn->on_disconnect(ConnectivityEventResult_OK,
                                    ConnectivityEventType_APPLICATIVE);
                conn->notify_for_connectivity_result(ConnectivityEventResult_OK,
                                                     ConnectivityEventType_APPLICATIVE);
            }
            /*for client connections we do not autorelease inactive connections.
              this because it is up to caller to applicatively release them.
              We only release them from collector, allowing others to destroy them.*/
            /************************
             RELEASE_ID: CONN_CLI_01
            ************************/
            blaze::collector &c = conn->get_collector();
            c.release(conn);
            cli_early_outgoing_sock_map_.remove_in_iteration();
        }
        cli_exec_serv_.shutdown();
        cli_exec_serv_.await_termination();
        IFLOG(dbg(TH_ID, LS_TRL "%s() - cli_exec_serv_ terminated.", __func__))
    }
    FD_ZERO(&read_FDs_);
    FD_ZERO(&write_FDs_);
    FD_ZERO(&excep_FDs_);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

void *selector::run()
{
    BLZ_ASYNCH_SELECTOR_STATUS current = BLZ_ASYNCH_SELECTOR_STATUS_UNDEF;
    IFLOG(trc(TH_ID, LS_OPN "%s(selid:%d)", __func__, id_))
    if(status_ != BLZ_ASYNCH_SELECTOR_STATUS_INIT &&
            status_ != BLZ_ASYNCH_SELECTOR_STATUS_REQUEST_READY) {
        IFLOG(err(TH_ID, LS_CLO "%s(status_=%d, exp:2) - [BAD STATUS]", __func__,
                  status_))
        SET_ERROR_AND_RETURNZERO_ACT;
    }
    do {
        IFLOG(dbg(TH_ID, LS_TRL"[selector][wait for go-ready request]"))
        await_for_status_reached_or_outdated(BLZ_ASYNCH_SELECTOR_STATUS_REQUEST_READY,
                                             current);
        IFLOG(dbg(TH_ID, LS_TRL"[selector][go ready requested, going ready]"))
        set_status(BLZ_ASYNCH_SELECTOR_STATUS_READY);
        IFLOG(dbg(TH_ID, LS_TRL"[selector][wait for go-select request]"))
        await_for_status_reached_or_outdated(BLZ_ASYNCH_SELECTOR_STATUS_REQUEST_SELECT,
                                             current);
        IFLOG(dbg(TH_ID, LS_TRL"[selector][go-select requested, going select]"))
        set_status(BLZ_ASYNCH_SELECTOR_STATUS_SELECT);
        timeval sel_timeout = sel_timeout_;
        while(status_ == BLZ_ASYNCH_SELECTOR_STATUS_SELECT) {
            IFLOG(low(TH_ID, LS_SEL"[selector][calling select() ..]"))
            if((sel_res_ = select(nfds_+1, &read_FDs_, &write_FDs_, &excep_FDs_, 0)) > 0) {
                IFLOG(low(TH_ID, LS_SEL"[selector][select() return:%d]", sel_res_))
                consume_events();
            } else if(!sel_res_) {
                //timeout
                IFLOG(trc(TH_ID, LS_SEL"[selector][select() timeout]"))
            } else {
                //error
                IFLOG(err(TH_ID, LS_SEL"[selector][select() error:%d]", sel_res_))
                set_status(BLZ_ASYNCH_SELECTOR_STATUS_ERROR);
            }
            FDSET_sockets();
            sel_timeout = sel_timeout_;
        }
        if(status_ == BLZ_ASYNCH_SELECTOR_STATUS_REQUEST_STOP) {
            IFLOG(dbg(TH_ID, LS_TRL"[selector][stop requested, clean initiated]"))
            stop_and_clean();
            break;
        }
        if(status_ == BLZ_ASYNCH_SELECTOR_STATUS_ERROR) {
            IFLOG(err(TH_ID, LS_TRL"[selector][error occurred, clean initiated]"))
            stop_and_clean();
            peer_.set_peer_error();
            break;
        }
    } while(true);
    set_status(BLZ_ASYNCH_SELECTOR_STATUS_STOPPED);
    int stop_res = stop();
    IFLOG(trc(TH_ID, LS_CLO "%s(selid:%d) - end, stop_res:%d -", __func__, id_,
              stop_res))
    return 0;
}

}
