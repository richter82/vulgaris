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
#ifdef __GNUG__
#include <errno.h>
#include <unistd.h>
#endif
#include "vlg_acceptor_impl.h"
#include "vlg_peer_impl.h"
#include "vlg_connection_impl.h"

namespace vlg {

#ifdef WIN32
static bool init_WSA = false;
vlg::RetCode WSA_init(vlg::logger *log_)
{
    if(init_WSA) {
        return vlg::RetCode_OK;
    }
    WSAData wsaData;
    int nCode;
    if((nCode = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0) {
        IFLOG(cri(TH_ID, LS_WSA"[WSADATA error code:%d]", nCode))
        return vlg::RetCode_KO;
    }
    IFLOG(inf(TH_ID, LS_WSA"[WSADATA loaded]"))
    init_WSA = true;
    return vlg::RetCode_OK;
}

vlg::RetCode WSA_destroy(vlg::logger *log_)
{
    if(!init_WSA) {
        return vlg::RetCode_OK;
    }
    WSACleanup();
    IFLOG(inf(TH_ID, LS_WSA"%s() - [WSADATA cleaned up]"))
    return vlg::RetCode_OK;
}

#endif

/*****************************************
 VLG_ACCEPTOR
******************************************/

vlg::logger *acceptor::log_ = NULL;

acceptor::acceptor(peer_impl &peer) :
    peer_(peer),
    serv_socket_(INVALID_SOCKET)
{
    log_ = vlg::logger::get_logger("acceptor");
    IFLOG(trc(TH_ID, LS_CTR "%s", __func__))
    memset(&serv_sockaddr_in_, 0, sizeof(serv_sockaddr_in_));
}

acceptor::~acceptor()
{
    IFLOG(trc(TH_ID, LS_DTR "%s(srv_implrf:%s, srv_port:%d)", __func__,
              inet_ntoa(serv_sockaddr_in_.sin_addr),
              ntohs(serv_sockaddr_in_.sin_port)))
    if(serv_socket_ != INVALID_SOCKET) {
#ifdef WIN32
        if(closesocket(serv_socket_) == SOCKET_ERROR) {
#else
        if(close(serv_socket_) == SOCKET_ERROR) {
#if 0
            if(shutdown(serv_socket_, SHUT_RDWR) == SOCKET_ERROR) {
#endif
#endif
            IFLOG(err(TH_ID, LS_DTR "%s() -closesocket KO-", __func__))
        }
    }
}

peer_impl &acceptor::peer() {
    return peer_;
}

vlg::RetCode acceptor::set_sockaddr_in(sockaddr_in &serv_sockaddr_in) {
    serv_sockaddr_in_ = serv_sockaddr_in;
    return vlg::RetCode_OK;
}

vlg::RetCode acceptor::create_server_socket(SOCKET &serv_socket) {
    IFLOG(trc(TH_ID, LS_OPN "%s(srv_interf:%s, srv_port:%d)", __func__,
              inet_ntoa(serv_sockaddr_in_.sin_addr),
              ntohs(serv_sockaddr_in_.sin_port)))
    if((serv_socket = serv_socket_ = socket(AF_INET, SOCK_STREAM,
                                            0)) != INVALID_SOCKET) {
        IFLOG(dbg(TH_ID, LS_TRL "%s(srv_sockid:%d) -socket OK-", __func__,
                  serv_socket))
        if(!bind(serv_socket_, (sockaddr *)&serv_sockaddr_in_, sizeof(sockaddr_in))) {
            IFLOG(dbg(TH_ID, LS_TRL "%s(srv_sockid:%d) -bind OK-", __func__,
                      serv_socket))
            if(!listen(serv_socket_, SOMAXCONN)) {
                IFLOG(dbg(TH_ID, LS_TRL "%s(srv_sockid:%d) -listen OK-", __func__,
                          serv_socket))
            } else {
                IFLOG(err(TH_ID, LS_CLO "%s(srv_sockid:%d) -listen KO-", __func__,
                          serv_socket_))
                return vlg::RetCode_SYSERR;
            }
        } else {
            int err = 0;
#ifdef WIN32
            err = WSAGetLastError();
#else
            err = errno;
#endif
            IFLOG(err(TH_ID, LS_CLO "%s(srv_sockid:%d, err:%d) -bind KO-",
                      __func__,
                      serv_socket_,
                      err))
            return vlg::RetCode_SYSERR;
        }
    } else {
        IFLOG(err(TH_ID, LS_CLO "%s() -socket KO-", __func__))
        return vlg::RetCode_SYSERR;
    }
    IFLOG(inf(TH_ID, LS_CLO "%s(srv_interf:%s, srv_port:%d)",
              __func__,
              inet_ntoa(serv_sockaddr_in_.sin_addr),
              ntohs(serv_sockaddr_in_.sin_port)))
    return vlg::RetCode_OK;
}

vlg::RetCode acceptor::accept(unsigned int new_connid,
                              connection_impl **new_conn) {
    IFLOG(trc(TH_ID, LS_OPN "%s(newconnid:%d, new_conn:%p)", __func__, new_connid,
              new_conn))
    SOCKET socket = INVALID_SOCKET;
    struct sockaddr_in addr;
#ifdef WIN32
    int len = sizeof(addr);
#else
    socklen_t len = 0;
#endif
    memset(&addr, 0, sizeof(addr));
    if((socket = ::accept(serv_socket_, (sockaddr *)&addr,
                          &len)) == INVALID_SOCKET) {
        int err = 0;
#ifdef WIN32
        err = WSAGetLastError();
#else
        err = errno;
#endif
        IFLOG(err(TH_ID, LS_CLO "%s(err:%d) -accept KO-", __func__, err))
        return vlg::RetCode_SYSERR;
    } else {
        IFLOG(dbg(TH_ID, LS_TRL
                  "%s(sockid:%d, host:%s, port:%d) -accept OK- [new socket connection accepted - candidate connid:%d]",
                  __func__,
                  socket,
                  inet_ntoa(addr.sin_addr),
                  ntohs(addr.sin_port),
                  new_connid))
    }
    peer_.new_connection(new_conn, peer_.conn_factory(),
                         ConnectionType_INGOING, new_connid,
                         peer_.conn_factory_ud());
    //server side adoption for autorelease.
    /************************
     RETAIN_ID: CONN_SRV_01
    ************************/
    vlg::collector &c = (*new_conn)->get_collector();
    c.retain(*new_conn);
    vlg::RetCode rcode = vlg::RetCode_OK;
    if((rcode = (*new_conn)->set_connection_established(socket))) {
        IFLOG(err(TH_ID, LS_CLO
                  "%s(new_conn:%p) - setting connection established fail with res:%d", __func__,
                  *new_conn,
                  rcode))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(new_conn:%p, res:%d)", __func__, *new_conn, rcode))
    return rcode;
}

}
