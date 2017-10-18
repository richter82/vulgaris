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

#ifndef BLZ_ACCEPTOR_H_
#define BLZ_ACCEPTOR_H_
#include "blaze_logger.h"
#include "blz_glob_int.h"

namespace vlg {

class peer_int;
class connection_int;

#ifdef WIN32
vlg::RetCode WSA_init(vlg::logger *log_);
vlg::RetCode WSA_destroy(vlg::logger *log_);
#endif

//-----------------------------
// acceptor
//-----------------------------
class acceptor {
    public:
        //---ctors
        acceptor(peer_int &peer);
        ~acceptor();

    public:
        peer_int   &peer();

        vlg::RetCode  set_sockaddr_in(sockaddr_in &serv_sockaddr_in);

        vlg::RetCode  create_server_socket(SOCKET &serv_socket);

        vlg::RetCode  accept(unsigned int new_connid,
                               connection_int **new_conn_desc);

    private:
        peer_int        &peer_;
        SOCKET          serv_socket_;
        sockaddr_in     serv_sockaddr_in_;

    protected:
        static vlg::logger   *log_;

};

}

#endif
