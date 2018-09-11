/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "glob.h"

namespace vlg {

#if defined WIN32 && defined _MSC_VER
RetCode WSA_init(logger *log);
RetCode WSA_destroy(logger *log);
#endif

struct acceptor {
    acceptor(peer_impl &peer);
    ~acceptor();

    RetCode set_sockaddr_in(sockaddr_in &serv_sockaddr_in);
    RetCode create_server_socket(SOCKET &serv_socket);
    RetCode accept(unsigned int new_connid, std::shared_ptr<incoming_connection> &new_connection);

    peer_impl &peer_;
    SOCKET serv_socket_;
    sockaddr_in serv_sockaddr_in_;
};

}
