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

#ifndef VLG_ACCEPTOR_H_
#define VLG_ACCEPTOR_H_
#include "glob.h"

namespace vlg {

#if defined WIN32 && defined _MSC_VER
RetCode WSA_init();
RetCode WSA_destroy();
#endif

struct acceptor {
    acceptor(peer_impl &peer);
    ~acceptor();

    RetCode set_sockaddr_in(sockaddr_in &serv_sockaddr_in);
    RetCode create_server_socket(SOCKET &serv_socket);
    RetCode accept(unsigned int new_connid,
                   std::shared_ptr<connection> &new_connection);

    peer_impl &peer_;
    SOCKET serv_socket_;
    sockaddr_in serv_sockaddr_in_;
};

}

#endif
