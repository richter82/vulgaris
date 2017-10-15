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
#include "blz_acceptor_int.h"
#include "blz_peer_int.h"
#include "blz_connection_int.h"

namespace blaze {

#ifdef WIN32
static bool init_WSA = false;
blaze::RetCode WSA_init(blaze::logger *log_)
{
    if(init_WSA) {
        return blaze::RetCode_OK;
    }
    WSAData wsaData;
    int nCode;
    if((nCode = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0) {
        IFLOG(cri(TH_ID, LS_WSA"[WSADATA error code:%d]", nCode))
        return blaze::RetCode_KO;
    }
    IFLOG(inf(TH_ID, LS_WSA"[WSADATA loaded]"))
    init_WSA = true;
    return blaze::RetCode_OK;
}

blaze::RetCode WSA_destroy(blaze::logger *log_)
{
    if(!init_WSA) {
        return blaze::RetCode_OK;
    }
    WSACleanup();
    IFLOG(inf(TH_ID, LS_WSA"%s() - [WSADATA cleaned up]"))
    return blaze::RetCode_OK;
}

#endif

/*****************************************
 BLZ_ACCEPTOR
******************************************/

blaze::logger *acceptor::log_ = NULL;

acceptor::acceptor(peer_int &peer) :
    peer_(peer),
    serv_socket_(INVALID_SOCKET)
{
    log_ = blaze::logger::get_logger("acceptor");
    IFLOG(trc(TH_ID, LS_CTR "%s", __func__))
    memset(&serv_sockaddr_in_, 0, sizeof(serv_sockaddr_in_));
}

acceptor::~acceptor()
{
    IFLOG(trc(TH_ID, LS_DTR "%s(srv_intrf:%s, srv_port:%d)", __func__,
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

peer_int &acceptor::peer() {
    return peer_;
}

blaze::RetCode acceptor::set_sockaddr_in(sockaddr_in &serv_sockaddr_in) {
    serv_sockaddr_in_ = serv_sockaddr_in;
    return blaze::RetCode_OK;
}

blaze::RetCode acceptor::create_server_socket(SOCKET &serv_socket) {
    IFLOG(trc(TH_ID, LS_OPN "%s(srv_intrf:%s, srv_port:%d)", __func__,
              inet_ntoa(serv_sockaddr_in_.sin_addr),
              ntohs(serv_sockaddr_in_.sin_port)))
    if((serv_socket = serv_socket_ = socket(AF_INET, SOCK_STREAM,
                                            0)) != INVALID_SOCKET) {
        IFLOG(dbg(TH_ID, LS_TRL "%s(srv_sockid:%d) -socket OK-", __func__, serv_socket))
        if(!bind(serv_socket_, (sockaddr *)&serv_sockaddr_in_, sizeof(sockaddr_in))) {
            IFLOG(dbg(TH_ID, LS_TRL "%s(srv_sockid:%d) -bind OK-", __func__, serv_socket))
            if(!listen(serv_socket_, SOMAXCONN)) {
                IFLOG(dbg(TH_ID, LS_TRL "%s(srv_sockid:%d) -listen OK-", __func__, serv_socket))
            } else {
                IFLOG(err(TH_ID, LS_CLO "%s(srv_sockid:%d) -listen KO-", __func__,
                          serv_socket_))
                return blaze::RetCode_SYSERR;
            }
        } else {
            int err = 0;
#ifdef WIN32
            err = WSAGetLastError();
#else
            err = errno;
#endif
            IFLOG(err(TH_ID, LS_CLO "%s(srv_sockid:%d, err:%d) -bind KO-", __func__,
                      serv_socket_, err))
            return blaze::RetCode_SYSERR;
        }
    } else {
        IFLOG(err(TH_ID, LS_CLO "%s() -socket KO-", __func__))
        return blaze::RetCode_SYSERR;
    }
    IFLOG(inf(TH_ID, LS_CLO "%s(srv_intrf:%s, srv_port:%d)", __func__,
              inet_ntoa(serv_sockaddr_in_.sin_addr),
              ntohs(serv_sockaddr_in_.sin_port)))
    return blaze::RetCode_OK;
}

blaze::RetCode acceptor::accept(unsigned int new_connid,
                                connection_int **new_conn) {
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
        return blaze::RetCode_SYSERR;
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
    blaze::collector &c = (*new_conn)->get_collector();
    c.retain(*new_conn);
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if((cdrs_res = (*new_conn)->set_connection_established(socket))) {
        IFLOG(err(TH_ID, LS_CLO
                  "%s(new_conn:%p) - setting connection established fail with res:%d", __func__,
                  *new_conn,
                  cdrs_res))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(new_conn:%p, res:%d)", __func__, *new_conn,
              cdrs_res))
    return cdrs_res;
}

}
