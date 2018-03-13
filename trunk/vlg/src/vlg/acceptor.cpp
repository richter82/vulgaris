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
#include "acceptor.h"
#include "pr_impl.h"
#include "conn_impl.h"

namespace vlg {

#if defined WIN32 && defined _MSC_VER
static bool init_WSA = false;
RetCode WSA_init()
{
    if(init_WSA) {
        return vlg::RetCode_OK;
    }
    WSAData wsaData;
    int nCode;
    if((nCode = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0) {
        IFLOG(cri(TH_ID, LS_EMP"[WSADATA error code:%d]", nCode))
        return vlg::RetCode_KO;
    }
    IFLOG(inf(TH_ID, LS_EMP"[WSADATA loaded]"))
    init_WSA = true;
    return vlg::RetCode_OK;
}

RetCode WSA_destroy()
{
    if(!init_WSA) {
        return vlg::RetCode_OK;
    }
    WSACleanup();
    IFLOG(inf(TH_ID, LS_EMP"[WSADATA cleaned up]"))
    return vlg::RetCode_OK;
}

#endif

/*****************************************
 VLG_ACCEPTOR
******************************************/

acceptor::acceptor(peer_impl &peer) :
    peer_(peer),
    serv_socket_(INVALID_SOCKET)
{
    memset(&serv_sockaddr_in_, 0, sizeof(serv_sockaddr_in_));
}

acceptor::~acceptor()
{
    if(serv_socket_ != INVALID_SOCKET) {
#if defined WIN32 && defined _MSC_VER
        if(closesocket(serv_socket_) == SOCKET_ERROR) {
#else
        if(close(serv_socket_) == SOCKET_ERROR) {
#if 0
            if(shutdown(serv_socket_, SHUT_RDWR) == SOCKET_ERROR) {
#endif
#endif
            IFLOG(err(TH_ID, LS_DTR "closesocket KO", __func__))
        }
    }
}

RetCode acceptor::set_sockaddr_in(sockaddr_in &serv_sockaddr_in) {
    serv_sockaddr_in_ = serv_sockaddr_in;
    return vlg::RetCode_OK;
}

RetCode acceptor::create_server_socket(SOCKET &serv_socket) {
    IFLOG(inf(TH_ID, LS_OPN "[interface:%s, port:%d]",
              __func__,
              inet_ntoa(serv_sockaddr_in_.sin_addr),
              ntohs(serv_sockaddr_in_.sin_port)))
    if((serv_socket = serv_socket_ = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET) {
        IFLOG(dbg(TH_ID, LS_TRL "[socket:%d][OK]", __func__, serv_socket))
        if(!bind(serv_socket_, (sockaddr *)&serv_sockaddr_in_, sizeof(sockaddr_in))) {
            IFLOG(dbg(TH_ID, LS_TRL "[bind OK]", __func__))
            if(!listen(serv_socket_, SOMAXCONN)) {
                IFLOG(dbg(TH_ID, LS_TRL "[listen OK]", __func__))
            } else {
                IFLOG(err(TH_ID, LS_CLO "[listen KO]", __func__))
                return vlg::RetCode_SYSERR;
            }
        } else {
            int err = 0;
#if defined WIN32 && defined _MSC_VER
            err = WSAGetLastError();
#else
            err = errno;
#endif
            IFLOG(err(TH_ID, LS_CLO "[bind KO][err:%d]", __func__, err))
            return vlg::RetCode_SYSERR;
        }
    } else {
        IFLOG(err(TH_ID, LS_CLO "[socket KO]", __func__))
        return vlg::RetCode_SYSERR;
    }
    IFLOG(trc(TH_ID, LS_CLO, __func__));
    return vlg::RetCode_OK;
}

RetCode acceptor::accept(unsigned int new_connid,
                         std::shared_ptr<connection> &new_connection) {
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    SOCKET socket = INVALID_SOCKET;
    struct sockaddr_in addr;
#if defined WIN32 && defined _MSC_VER
    int len = sizeof(addr);
#else
    socklen_t len = 0;
#endif
    memset(&addr, 0, sizeof(addr));
    if((socket = ::accept(serv_socket_, (sockaddr *)&addr, &len)) == INVALID_SOCKET) {
        int err = 0;
#if defined WIN32 && defined _MSC_VER
        err = WSAGetLastError();
#else
        err = errno;
#endif
        IFLOG(err(TH_ID, LS_CLO "[accept KO][err:%d]", __func__, err))
        return vlg::RetCode_SYSERR;
    } else {
        IFLOG(dbg(TH_ID, LS_TRL "[socket:%d, host:%s, port:%d][accept OK][candidate connid:%d]",
                  __func__,
                  socket,
                  inet_ntoa(addr.sin_addr),
                  ntohs(addr.sin_port),
                  new_connid))
    }

    peer_.new_incoming_connection(new_connection, new_connid);
    new_connection->impl_->con_type_ = ConnectionType_INGOING;
    new_connection->impl_->connid_ = new_connid;

    RetCode rcode = vlg::RetCode_OK;
    if((rcode = new_connection->impl_->set_connection_established(socket))) {
        IFLOG(err(TH_ID, LS_CLO "[new_connid:%d - setting connection established fail with res:%d]", __func__,
                  new_connid,
                  rcode))
    }
    IFLOG(trc(TH_ID, LS_CLO "[new_connid:%d][res:%d]", __func__, new_connid, rcode))
    return rcode;
}

}
