/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */
#ifdef __GNUG__
#include <errno.h>
#include <unistd.h>
#endif
#include "acceptor.h"
#include "brk_impl.h"
#include "conn_impl.h"

namespace vlg {

#if defined WIN32 && defined _MSC_VER
static bool init_WSA = false;
RetCode WSA_init(std::shared_ptr<spdlog::logger> &log)
{
    if(init_WSA) {
        return RetCode_OK;
    }
    WSAData wsaData;
    int nCode;
    if((nCode = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        IFLOG(log, critical(LS_SYS"[WSADATA error code:{}]", nCode))
        return RetCode_KO;
    }
    IFLOG(log, info(LS_SYS"[WSADATA loaded]"))
    init_WSA = true;
    return RetCode_OK;
}

RetCode WSA_destroy(std::shared_ptr<spdlog::logger> &log)
{
    if(!init_WSA) {
        return RetCode_OK;
    }
    WSACleanup();
    IFLOG(log, info(LS_SYS"[WSADATA cleaned up]"))
    return RetCode_OK;
}

#endif

acceptor::acceptor(broker_impl &broker) :
    broker_(broker),
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
            IFLOG(broker_.log_, error(LS_DTR "closesocket KO", __func__))
        }
    }
}

RetCode acceptor::set_sockaddr_in(sockaddr_in &serv_sockaddr_in) {
    serv_sockaddr_in_ = serv_sockaddr_in;
    return RetCode_OK;
}

RetCode acceptor::create_server_socket(SOCKET &serv_socket) {
    IFLOG(broker_.log_, info(LS_OPN "[interface:{}, port:{}]",
                             __func__,
                             inet_ntoa(serv_sockaddr_in_.sin_addr),
                             ntohs(serv_sockaddr_in_.sin_port)))
    if((serv_socket = serv_socket_ = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET) {
        IFLOG(broker_.log_, debug(LS_TRL "[socket:{}][OK]", __func__, serv_socket))
        if(!bind(serv_socket_, (sockaddr *)&serv_sockaddr_in_, sizeof(sockaddr_in))) {
            IFLOG(broker_.log_, debug(LS_TRL "[bind OK]", __func__))
            if(!listen(serv_socket_, SOMAXCONN)) {
                IFLOG(broker_.log_, debug(LS_TRL "[listen OK]", __func__))
            } else {
                IFLOG(broker_.log_, error(LS_CLO "[listen KO]", __func__))
                return RetCode_SYSERR;
            }
        } else {
            int err = 0;
#if defined WIN32 && defined _MSC_VER
            err = WSAGetLastError();
#else
            err = errno;
#endif
            IFLOG(broker_.log_, error(LS_CLO "[bind KO][err:{}]", __func__, err))
            return RetCode_SYSERR;
        }
    } else {
        IFLOG(broker_.log_, error(LS_CLO "[socket KO]", __func__))
        return RetCode_SYSERR;
    }
    return RetCode_OK;
}

RetCode acceptor::accept(unsigned int new_connid, std::shared_ptr<incoming_connection> &new_connection) {
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
        IFLOG(broker_.log_, error(LS_CLO "[accept KO][err:{}]", __func__, err))
        return RetCode_SYSERR;
    } else {
        IFLOG(broker_.log_, debug(LS_TRL "[socket:{}, host:{}, port:{}][accept OK][candidate connid:{}]",
                                  __func__,
                                  socket,
                                  inet_ntoa(addr.sin_addr),
                                  ntohs(addr.sin_port),
                                  new_connid))
    }

    broker_.new_incoming_connection(new_connection, new_connid);
    new_connection->impl_->con_type_ = ConnectionType_INGOING;
    new_connection->impl_->connid_ = new_connid;

    RetCode rcode = RetCode_OK;
    if((rcode = new_connection->impl_->set_connection_established(socket))) {
        IFLOG(broker_.log_, error(LS_CLO "[new_connid:{} - setting connection established fail with res:{}]", __func__,
                                  new_connid,
                                  rcode))
    }
    return rcode;
}

}
