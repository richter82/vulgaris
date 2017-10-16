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
#include <unistd.h>
#include <fcntl.h>
#endif
#include "blz_peer_int.h"
#include "blz_connection_int.h"
#include "blz_transaction_int.h"
#include "blz_subscription_int.h"

namespace blaze {

// BLZ_CONNECTION CTORS - INIT - DESTROY

//BLZ_CONNECTION MEMORY

class connection_int_inst_collector : public blaze::collector {
    public:
        connection_int_inst_collector() : blaze::collector("connection_int") {}

};

blaze::collector *conn_inst_coll_ = NULL;
blaze::collector &conn_inst_collector()
{
    if(conn_inst_coll_) {
        return *conn_inst_coll_;
    }
    if(!(conn_inst_coll_ = new connection_int_inst_collector())) {
        EXIT_ACTION("failed creating connection_int::conn_inst_coll_\n")
    }
    return *conn_inst_coll_;
}

blaze::collector &connection_int::get_collector()
{
    return conn_inst_collector();
}

blaze::logger *connection_int::log_ = NULL;

connection_int::connection_int(peer_int &peer, ConnectionType con_type,
                               unsigned int connid) :
    peer_(peer),
    con_type_(con_type),
    socket_(INVALID_SOCKET),
    last_socket_err_(0),
    con_evt_res_(ConnectivityEventResult_OK),
    connectivity_evt_type_(ConnectivityEventType_UNDEFINED),
    connid_(connid),
    status_(ConnectionStatus_UNDEFINED),
    conres_(ConnectionResult_UNDEFINED),
    conrescode_(ConnectionResultReason_NO_ERROR),
    cli_agrhbt_(0),
    srv_agrhbt_(0),
    disconrescode_(DisconnectionResultReason_UNDEFINED),
    csc_hndl_(NULL),
    csc_hndl_ud_(NULL),
    connect_evt_occur_(false),
    pkt_sending_q_(blaze::sngl_ptr_obj_mng()),
    srv_flytx_repo_(blaze::sngl_ptr_obj_mng(), sizeof(tx_id)),
    cli_flytx_repo_(blaze::sngl_ptr_obj_mng(), sizeof(tx_id)),
    srv_classid_sbs_repo_(blaze::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    srv_sbsid_sbs_repo_(blaze::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    cli_reqid_sbs_repo_(blaze::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    cli_sbsid_sbs_repo_(blaze::sngl_ptr_obj_mng(), sizeof(unsigned int)),
    tx_factory_(NULL),
    tx_factory_ud_(NULL),
    sbs_factory_(NULL),
    sbs_factory_ud_(NULL),
    prid_(0),
    reqid_(0),
    sbsid_(0)
{
    log_ = blaze::logger::get_logger("connection_int");
    if(connid_) {
        IFLOG(trc(TH_ID, LS_CTR "%s()(candidate connid: %d)", __func__, connid_))
    } else {
        IFLOG(trc(TH_ID, LS_CTR "%s()(connid not yet known)", __func__))
    }
}

connection_int::~connection_int()
{
    blaze::collector &c = get_collector();
    if(c.is_instance_collected(this)) {
        IFLOG(cri(TH_ID, LS_DTR "%s(ptr:%p)" D_W_R_COLL LS_EXUNX,
                  __func__,
                  this))
    }
    if(status_ == ConnectionStatus_ESTABLISHED ||
            status_ == ConnectionStatus_PROTOCOL_HANDSHAKE ||
            status_ == ConnectionStatus_AUTHENTICATED ||
            status_ == ConnectionStatus_DISCONNECTING) {
        IFLOG(cri(TH_ID, LS_DTR
                  "%s(ptr:%p) - [connection is not in a safe state:%d] " LS_EXUNX, __func__,
                  this,
                  status_))
    }
    IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
}

blaze::RetCode connection_int::clean_best_effort()
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    mon_.lock();
    //TRANSACTIONS
    transaction_int *tx = NULL;
    if(con_type_ == ConnectionType_OUTGOING) {
        cli_flytx_repo_.lock_write();
        cli_flytx_repo_.start_iteration();
        while(!cli_flytx_repo_.next(NULL, &tx)) {
            tx->set_tx_result_code(ProtocolCode_TRANSACTION_CLIENT_ABORT);
            tx->set_aborted();
            blaze::collector &c = tx->get_collector();
            c.release(tx);
            cli_flytx_repo_.remove_in_iteration();
        }
        cli_flytx_repo_.unlock();
    }
    if(con_type_ == ConnectionType_INGOING) {
        srv_flytx_repo_.lock_write();
        srv_flytx_repo_.start_iteration();
        while(!srv_flytx_repo_.next(NULL, &tx)) {
            tx->set_tx_result_code(ProtocolCode_TRANSACTION_CLIENT_ABORT);
            tx->set_aborted();
            srv_flytx_repo_.remove_in_iteration();
            release_transaction(tx);
        }
        srv_flytx_repo_.unlock();
    }
    //SUBSCRIPTIONS
    subscription_int *sbs = NULL;
    blaze::linked_list support_list(blaze::sngl_ptr_obj_mng());
    support_list.init();
    if(con_type_ == ConnectionType_OUTGOING) {
        //REQID
        cli_reqid_sbs_repo_.start_iteration();
        while(!cli_reqid_sbs_repo_.next(NULL, &sbs)) {
            sbs->set_stopped();
            support_list.push_back(&sbs);
        }
        support_list.start_iteration();
        while(!support_list.next(&sbs)) {
            release_subscription(sbs);
        }
        //SBSID
        support_list.clear();
        cli_sbsid_sbs_repo_.start_iteration();
        while(!cli_sbsid_sbs_repo_.next(NULL, &sbs)) {
            sbs->set_stopped();
            support_list.push_back(&sbs);
        }
        support_list.start_iteration();
        while(!support_list.next(&sbs)) {
            release_subscription(sbs);
        }
    }
    if(con_type_ == ConnectionType_INGOING) {
        srv_sbsid_sbs_repo_.start_iteration();
        while(!srv_sbsid_sbs_repo_.next(NULL, &sbs)) {
            sbs->set_stopped();
            support_list.push_back(&sbs);
        }
        support_list.start_iteration();
        while(!support_list.next(&sbs)) {
            release_subscription(sbs);
        }
    }
    clean_packet_snd_q();
    mon_.unlock();
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

void connection_int::clean_packet_snd_q()
{
    if(pkt_sending_q_.size()) {
        blaze::grow_byte_buffer *pkt = NULL;
        while((!pkt_sending_q_.get(0, 0, &pkt))) {
            delete pkt;
        }
    }
}

blaze::RetCode connection_int::init(unsigned int pkt_sending_q_capcty)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, pkt_sending_q_capcty:%u)", __func__, this,
              pkt_sending_q_capcty))
    RETURN_IF_NOT_OK(pkt_sending_q_.init(pkt_sending_q_capcty))
    RETURN_IF_NOT_OK(srv_flytx_repo_.init(HM_SIZE_SMALL))
    RETURN_IF_NOT_OK(cli_flytx_repo_.init(HM_SIZE_SMALL))
    RETURN_IF_NOT_OK(srv_classid_sbs_repo_.init(HM_SIZE_SMALL))
    RETURN_IF_NOT_OK(srv_sbsid_sbs_repo_.init(HM_SIZE_SMALL))
    RETURN_IF_NOT_OK(cli_reqid_sbs_repo_.init(HM_SIZE_NANO))
    RETURN_IF_NOT_OK(cli_sbsid_sbs_repo_.init(HM_SIZE_SMALL))
    set_status(ConnectionStatus_INITIALIZED);
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

// STATUS

ConnectionStatus connection_int::status()
{
    return status_;
}

blaze::RetCode connection_int::set_connection_established()
{
    return set_connection_established(socket_);
}

blaze::RetCode connection_int::set_connection_established(SOCKET socket)
{
    socket_ = socket;
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    IFLOG(dbg(TH_ID,
              LS_CON"[connection established: sockid:%d, host:%s, port:%d] (connid: %d)",
              socket_,
              inet_ntoa(saddr.sin_addr),
              ntohs(saddr.sin_port),
              connid_))
    set_status(ConnectionStatus_ESTABLISHED);
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::set_status(ConnectionStatus status)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, status:%d)", __func__, this, status))
    CHK_MON_ERR_0(lock)
    status_ = status;
    if(csc_hndl_) {
        csc_hndl_(*this, status, csc_hndl_ud_);
    }
    CHK_MON_ERR_0(notify_all)
    CHK_MON_ERR_0(unlock)
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::await_for_status_reached_or_outdated(
    ConnectionStatus test,
    ConnectionStatus &current,
    time_t sec,
    long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, test:%d)", __func__, this, test))
    CHK_MON_ERR_0(lock)
    if(status_ < ConnectionStatus_INITIALIZED) {
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
              LS_CLO "%s(connid:%d, res:%d) - status:%d [reached or outdated] current:%d",
              __func__, connid_,
              cdrs_res, test, status_))
    CHK_MON_ERR_0(unlock)
    return cdrs_res;
}

blaze::RetCode connection_int::await_for_status_change(ConnectionStatus
                                                       &status,
                                                       time_t sec,
                                                       long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, status:%d)", __func__, this, status))
    CHK_MON_ERR_0(lock)
    if(status_ < ConnectionStatus_INITIALIZED) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADSTTS;
    }
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    while(status == status_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res =  blaze::RetCode_TIMEOUT;
                break;
            }
        }
    }
    status = status_;
    CHK_MON_ERR_0(unlock)
    IFLOG(log(cdrs_res ? blaze::TL_WRN : blaze::TL_DBG, TH_ID,
              LS_CLO "%s(res:%d) - status:%d [changed] current:%d", __func__, cdrs_res,
              status, status_))
    return cdrs_res;
}

blaze::RetCode connection_int::set_proto_connected()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ != ConnectionStatus_ESTABLISHED) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADSTTS;
    }
    set_status(ConnectionStatus_PROTOCOL_HANDSHAKE);
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::set_disconnecting()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    set_status(ConnectionStatus_DISCONNECTING);
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::set_socket_disconnected()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    IFLOG(inf(TH_ID, LS_CON"[connid:%d][disconnected]", connid_))
    set_status(ConnectionStatus_DISCONNECTED);
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::set_proto_error(blaze::RetCode cause_res)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ == ConnectionStatus_DISCONNECTED) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADSTTS;
    }
    set_status(ConnectionStatus_PROTOCOL_ERROR);
    IFLOG(err(TH_ID, LS_CLO "%s(cause_res:%d, last_sock_err:%d)", __func__,
              cause_res, last_socket_err_))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::set_socket_error(blaze::RetCode cause_res)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ < ConnectionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADSTTS;
    }
    IFLOG(err(TH_ID, LS_CLO "%s(cause_res:%d, last_sock_err:%d)", __func__,
              cause_res, last_socket_err_))
    set_status(ConnectionStatus_SOCKET_ERROR);
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::set_internal_error(blaze::RetCode cause_res)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ < ConnectionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADSTTS;
    }
    IFLOG(cri(TH_ID, LS_CLO "%s(cause_res:%d, last_sock_err:%d)", __func__,
              cause_res, last_socket_err_))
    set_status(ConnectionStatus_ERROR);
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::set_appl_connected()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ != ConnectionStatus_PROTOCOL_HANDSHAKE) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID, LS_CLO "%s", __func__))
    set_status(ConnectionStatus_AUTHENTICATED);
    return blaze::RetCode_OK;
}

void connection_int::set_connection_status_change_handler(
    connection_impl_status_change_hndlr hndlr, void *ud)
{
    csc_hndl_ = hndlr;
    csc_hndl_ud_ = ud;
}

// GETTERS / SETTERS

blaze::synch_hash_map &connection_int::client_fly_tx_map()
{
    return cli_flytx_repo_;
}

blaze::synch_hash_map &connection_int::server_fly_tx_map()
{
    return srv_flytx_repo_;
}

blaze::synch_hash_map &connection_int::class_id_sbs_map()
{
    return srv_classid_sbs_repo_;
}

blaze::synch_hash_map &connection_int::sbsid_sbs_map()
{
    return srv_sbsid_sbs_repo_;
}

blaze::synch_hash_map &connection_int::reqid_sbs_map()
{
    return cli_reqid_sbs_repo_;
}

connection_int::blz_tx_factory_func connection_int::tx_factory() const
{
    return tx_factory_;
}

void *connection_int::tx_factory_ud() const
{
    return tx_factory_ud_;
}

connection_int::blz_sbs_factory_func connection_int::sbs_factory() const
{
    return sbs_factory_;
}

void *connection_int::sbs_factory_ud() const
{
    return sbs_factory_ud_;
}

ConnectionResult connection_int::conn_response() const
{
    return conres_;
}

void connection_int::set_conn_response(ConnectionResult val)
{
    conres_ = val;
}

ConnectionResultReason connection_int::conn_res_code() const
{
    return conrescode_;
}

void connection_int::set_conn_res_code(ConnectionResultReason val)
{
    conrescode_ = val;
}

unsigned short connection_int::client_agrhbt() const
{
    return cli_agrhbt_;
}

void connection_int::set_client_agrhbt(unsigned short val)
{
    cli_agrhbt_ = val;
}

unsigned short connection_int::server_agrhbt() const
{
    return srv_agrhbt_;
}

DisconnectionResultReason connection_int::discon_res_code()  const
{
    return disconrescode_;
}

void connection_int::set_server_agrhbt(unsigned short val)
{
    srv_agrhbt_ = val;
}

void connection_int::set_tx_factory(blz_tx_factory_func val)
{
    tx_factory_ = val;
}

void connection_int::set_tx_factory_ud(void *ud)
{
    tx_factory_ud_ = ud;
}

void connection_int::set_sbs_factory(blz_sbs_factory_func val)
{
    sbs_factory_ = val;
}

void connection_int::set_sbs_factory_ud(void *ud)
{
    sbs_factory_ud_ = ud;
}

int connection_int::get_last_socket_err()  const
{
    return last_socket_err_;
}

SOCKET connection_int::get_socket()  const
{
    return socket_;
}

const char *connection_int::get_host_ip() const
{
    if(socket_ == INVALID_SOCKET) {
        return "invalid address";
    }
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    return inet_ntoa(saddr.sin_addr);
}

unsigned short connection_int::get_host_port() const
{
    if(socket_ == INVALID_SOCKET) {
        return 0;
    }
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    return ntohs(saddr.sin_port);
}

blaze::blocking_queue &connection_int::pkt_snd_q()
{
    return pkt_sending_q_;
}

peer_int &connection_int::peer()
{
    return peer_;
}

ConnectionType connection_int::conn_type()  const
{
    return con_type_;
}

unsigned int connection_int::connid() const
{
    return connid_;
}

blaze::RetCode connection_int::set_connid(unsigned int connid)
{
    connid_ = connid;
    return blaze::RetCode_OK;
}

unsigned int connection_int::next_prid()
{
    mon_.lock();
    prid_++;
    mon_.unlock();
    return prid_;
}

unsigned int connection_int::next_reqid()
{
    mon_.lock();
    reqid_++;
    mon_.unlock();
    return reqid_;
}

unsigned int connection_int::next_sbsid()
{
    mon_.lock();
    sbsid_++;
    mon_.unlock();
    return sbsid_;
}

// TCP/IP

blaze::RetCode connection_int::set_socket_blocking_mode(bool blocking)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(blocking:%d)", __func__, blocking))
#ifdef WIN32
    unsigned long mode = blocking ? 0 : 1;
    return (ioctlsocket(socket_, FIONBIO,
                        &mode) == 0) ? blaze::RetCode_OK : blaze::RetCode_KO;
#else
    int flags = fcntl(socket_, F_GETFL, 0);
    if(flags < 0) {
        return blaze::RetCode_KO;
    }
    flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
    return (fcntl(socket_, F_SETFL,
                  flags) == 0) ? blaze::RetCode_OK : blaze::RetCode_KO;
#endif
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
}

blaze::RetCode connection_int::socket_excptn_hndl(long sock_op_res)
{
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(sock_op_res == SOCKET_ERROR) {
#ifdef WIN32
        last_socket_err_ = WSAGetLastError();
#else
        last_socket_err_ = errno;
#endif
#ifdef WIN32
        if(last_socket_err_ == WSAEWOULDBLOCK) {
#else
        if(last_socket_err_ == EAGAIN) {
            return blaze::RetCode_SCKEAGN;
        } else if(last_socket_err_ == EWOULDBLOCK) {
#endif
            cdrs_res = blaze::RetCode_SCKWBLK;
#ifdef WIN32
        } else if(last_socket_err_ == WSAECONNRESET) {
#else
        } else if(last_socket_err_ == ECONNRESET) {
#endif
            IFLOG(err(TH_ID, LS_CON
                      "[connid:%d][sockid:%d][connection reset by peer][err:%d]",connid_, socket_,
                      last_socket_err_))
            cdrs_res = blaze::RetCode_SCKCLO;
        } else {
            perror(__func__);
            IFLOG(err(TH_ID, LS_CON
                      "[connid:%d][sockid:%d][connection socket error][errno:%d][err:%d]",connid_,
                      socket_, errno,
                      last_socket_err_))
            cdrs_res = blaze::RetCode_SCKERR;
        }
    } else if(!sock_op_res) {
        /*typically we can arrive here on client applicative disconnections*/
        IFLOG(dbg(TH_ID, LS_CON
                  "[connid:%d][sockid:%d][connection socket was closed by peer]",connid_,
                  socket_))
        cdrs_res = blaze::RetCode_SCKCLO;
    } else {
        IFLOG(err(TH_ID, LS_CON "[connid:%d][sockid:%d][connection unk. error]",
                  connid_, socket_))
        cdrs_res = blaze::RetCode_UNKERR;
    }
    return cdrs_res;
}

blaze::RetCode connection_int::establish_connection(sockaddr_in &params)
{
    IFLOG(dbg(TH_ID, LS_OPN "%s() -> [host:%s - port:%d]", __func__,
              inet_ntoa(params.sin_addr), htons(params.sin_port)))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    int connect_res = 0;
    if((socket_ = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET) {
        IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d) -socket OK-", __func__, socket_))
        socklen_t len = sizeof(sockaddr_in);
        if((connect_res = connect(socket_, (struct sockaddr *)&params,
                                  len)) != INVALID_SOCKET) {
            IFLOG(dbg(TH_ID, LS_TRL "%s(sockid:%d) -connect OK-", __func__, socket_))
        } else {
#ifdef WIN32
            last_socket_err_ = WSAGetLastError();
#else
            last_socket_err_ = errno;
#endif
            IFLOG(dbg(TH_ID, LS_CLO "%s(err:%d) -connect KO-", __func__, last_socket_err_))
            IFLOG(err(TH_ID, LS_CON "[connection failed][err:%d]", last_socket_err_))
            if((cdrs_res = socket_shutdown())) {
                IFLOG(err(TH_ID, LS_CLO "%s(err:%d) - failed closing socket.", __func__,
                          cdrs_res))
            }
        }
    } else {
#ifdef WIN32
        last_socket_err_ = WSAGetLastError();
#else
        last_socket_err_ = errno;
#endif
        IFLOG(err(TH_ID, LS_CLO "%s(err:%d) -socket KO-", __func__, last_socket_err_))
    }
    if(!connect_res) {
        if((cdrs_res = set_connection_established(socket_))) {
            IFLOG(err(TH_ID, LS_TRL "%s(res:%d)", __func__, cdrs_res))
        }
    } else {
        notify_for_connectivity_result(ConnectivityEventResult_KO,
                                       ConnectivityEventType_NETWORK);
        cdrs_res = blaze::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

blaze::RetCode connection_int::send_single_pkt(blaze::grow_byte_buffer
                                               *pkt_bbuf)
{
    blaze::ascii_string raw_pkt;
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, sndbuffsz:%u, pkt_bbuf:%p)", __func__,
              socket_, pkt_bbuf->limit(), pkt_bbuf))
    if(!pkt_bbuf->limit() || !pkt_bbuf) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADARG;
    }
    if(log_ && log_->level() <= blaze::TL_TRC) {
        blaze::ascii_string tmp;
        log_->trc(TH_ID, LS_OUT LS_PKT "%s", dump_raw_pkt(true, pkt_bbuf->buffer(),
                                                          pkt_bbuf->limit(), tmp));
    }
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    bool stay = true;
    long bsent = 0, tot_bsent = 0, remaining = (long)pkt_bbuf->limit();
    while(stay) {
        while(remaining &&
                ((bsent = send(socket_, &pkt_bbuf->buffer_as_char()[pkt_bbuf->position()],
                               (int)pkt_bbuf->limit(), 0)) > 0)) {
            pkt_bbuf->advance_pos_read(bsent);
            tot_bsent += bsent;
            remaining -= bsent;
            IFLOG(trc(TH_ID, LS_TRL "%s() - bsent:%d, tot_bsent:%d, remaining:%d", __func__,
                      bsent, tot_bsent, remaining))
        }
        if(remaining) {
            if(((cdrs_res = socket_excptn_hndl(bsent)) != blaze::RetCode_SCKEAGN) ||
                    (cdrs_res != blaze::RetCode_SCKWBLK)) {
                cdrs_res = blaze::RetCode_OK;
                stay = false;
            }
        } else {
            break;
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)(sockid:%d, tot_bsent:%d, remaining:%d)",
              __func__,
              cdrs_res,
              socket_,
              tot_bsent,
              remaining))
    return cdrs_res;
}

blaze::RetCode connection_int::recv_body(unsigned int bodylen,
                                         blaze::grow_byte_buffer *pkt_body)
{
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    bool stay = true;
    unsigned int tot_brecv = 0;
    long brecv = 0, recv_buf_sz = bodylen;
    pkt_body->ensure_capacity(bodylen);
    while(stay) {
        while((tot_brecv < bodylen) &&
                ((brecv = recv(socket_, &pkt_body->buffer_as_char()[pkt_body->position()],
                               recv_buf_sz, 0)) > 0)) {
            pkt_body->move_pos_write(brecv);
            tot_brecv += brecv;
            recv_buf_sz -= brecv;
            IFLOG(trc(TH_ID, LS_TRL "%s() - brecv:%d, tot_brecv:%u, recv_buf_sz:%d",
                      __func__, brecv, tot_brecv, recv_buf_sz))
        }
        if(tot_brecv != bodylen) {
            if((cdrs_res = socket_excptn_hndl(brecv)) != blaze::RetCode_SCKEAGN) {
                cdrs_res = blaze::RetCode_OK;
                stay = false;
            }
        } else {
            break;
        }
    }
    pkt_body->flip();
    return cdrs_res;
}

blaze::RetCode connection_int::recv_single_pkt(blz_hdr_rec *pkt_hdr,
                                               blaze::grow_byte_buffer *pkt_body)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(pkt_hdr:%p, pkt_body:%p)(sockid:%d)", __func__,
              pkt_hdr, pkt_body, socket_))
    if(!pkt_hdr || !pkt_body) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADARG;
    }
    int tot_brecv = 0;
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    //first phase: decode pkt header.
    switch((cdrs_res = recv_and_decode_hdr(pkt_hdr))) {
        case blaze::RetCode_OK:
            break;
        case blaze::RetCode_SCKCLO:
            clean_best_effort();
            socket_shutdown();
            on_disconnect(ConnectivityEventResult_OK,
                          ConnectivityEventType_NETWORK);
            break;
        case blaze::RetCode_SCKERR:
            clean_best_effort();
            socket_shutdown();
            set_socket_error(cdrs_res);
            on_disconnect(ConnectivityEventResult_KO,
                          ConnectivityEventType_NETWORK);
            break;
        case blaze::RetCode_SCKWBLK:
        case blaze::RetCode_DRPPKT:
            clean_best_effort();
            socket_shutdown();
            set_proto_error(cdrs_res);
            on_disconnect(ConnectivityEventResult_KO,
                          ConnectivityEventType_PROTOCOL);
            break;
        default:
            clean_best_effort();
            socket_shutdown();
            set_internal_error(cdrs_res);
            on_disconnect(ConnectivityEventResult_KO,
                          ConnectivityEventType_UNDEFINED);
            break;
    }
    //END first phase
    if(pkt_hdr->bdy_bytelen) {
        //second phase: read eventual remaining body.
        switch((cdrs_res = recv_body(pkt_hdr->bdy_bytelen, pkt_body))) {
            case blaze::RetCode_OK:
                break;
            case blaze::RetCode_SCKCLO:
                clean_best_effort();
                socket_shutdown();
                on_disconnect(ConnectivityEventResult_OK,
                              ConnectivityEventType_NETWORK);
                break;
            case blaze::RetCode_SCKERR:
                clean_best_effort();
                socket_shutdown();
                set_socket_error(cdrs_res);
                on_disconnect(ConnectivityEventResult_KO,
                              ConnectivityEventType_NETWORK);
                break;
            case blaze::RetCode_SCKWBLK:
            case blaze::RetCode_DRPPKT:
                clean_best_effort();
                socket_shutdown();
                set_proto_error(cdrs_res);
                on_disconnect(ConnectivityEventResult_KO,
                              ConnectivityEventType_PROTOCOL);
                break;
            default:
                clean_best_effort();
                socket_shutdown();
                set_internal_error(cdrs_res);
                on_disconnect(ConnectivityEventResult_KO,
                              ConnectivityEventType_UNDEFINED);
                break;
        }
        //END second phase
    }
    if(!cdrs_res) {
        tot_brecv = pkt_hdr->hdr_bytelen + pkt_hdr->bdy_bytelen;
        if(log_ && log_->level() <= blaze::TL_TRC) {
            char dump_buf[DMP_OUT_BUF_LEN] = {0};
            dump_blz_hdr_rec(pkt_hdr, dump_buf);
            log_->trc(TH_ID, LS_INC LS_PKT "%s", dump_buf);
        }
        if(pkt_hdr->bdy_bytelen && log_ && log_->level() <= blaze::TL_TRC) {
            blaze::ascii_string tmp;
            log_->trc(TH_ID, LS_INC LS_PKT "%s", dump_raw_pkt(false, pkt_body->buffer(),
                                                              pkt_body->limit(), tmp));
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)(tot_brecv:%d, hdr_len:%d, bdy_len:%d)",
              __func__,
              cdrs_res,
              tot_brecv,
              pkt_hdr->hdr_bytelen,
              pkt_hdr->bdy_bytelen))
    return cdrs_res;
}

blaze::RetCode connection_int::socket_shutdown()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d)", __func__, socket_))
    int last_socket_err_ = 0;
#ifdef WIN32
    if((last_socket_err_ = closesocket(socket_))) {
        IFLOG(err(TH_ID, LS_CLO "%s(sockid:%d, res:%d) -closesocket KO-", __func__,
                  socket_, last_socket_err_))
    } else {
        IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, res:%d) -closesocket OK-", __func__,
                  socket_, last_socket_err_))
    }
#else
    if((last_socket_err_ = close(socket_))) {
        IFLOG(err(TH_ID, LS_CLO "%s(sockid:%d, res:%d) -close KO-", __func__, socket_,
                  last_socket_err_))
    } else {
        IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, res:%d) -close OK-", __func__, socket_,
                  last_socket_err_))
    }
#if 0
    if((last_socket_err_ = shutdown(socket_, SHUT_RDWR))) {
        IFLOG(err(TH_ID, LS_CLO "%s(sockid:%d, res:%d) -shutdown KO-", __func__,
                  socket_, last_socket_err_))
    } else {
        IFLOG(trc(TH_ID, LS_TRL "%s(sockid:%d, res:%d) -shutdown OK-", __func__,
                  socket_, last_socket_err_))
    }
#endif
#endif
    set_socket_disconnected();
    IFLOG(trc(TH_ID, LS_CLO "%s(sockid:%d)", __func__, socket_))
    return blaze::RetCode_OK;
}

// CONNECTIVITY

blaze::RetCode connection_int::client_connect(sockaddr_in &params)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(peer_.peer_status() != PeerStatus_RUNNING) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p) - invalid peer status.", __func__, this))
        return blaze::RetCode_BADSTTS;
    }
    if(status_ != ConnectionStatus_INITIALIZED &&
            status_ != ConnectionStatus_DISCONNECTED) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p) - invalid connection status.", __func__,
                  this))
        return blaze::RetCode_BADSTTS;
    }

    //client side adoption to avoid userspace deletion.
    /************************
    RETAIN_ID: CONN_CLI_01
    ************************/
    blaze::collector &c = get_collector();
    c.retain(this);

    connect_evt_occur_ = false;
    DECLINITH_GBB(gbb, 2)
    build_PKT_CONREQ(cli_agrhbt_, gbb);
    gbb->flip();
    RETURN_IF_NOT_OK(pkt_sending_q_.put(&gbb))
    selector_event *evt =
        new selector_event(BLZ_SELECTOR_Evt_ConnectRequest, this);
    memcpy(&evt->saddr_, &params, sizeof(sockaddr_in));
    if((cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt))) {
        set_status(ConnectionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p) - RES:%d", __func__, this, cdrs_res))
    return cdrs_res;
}

blaze::RetCode connection_int::server_send_connect_res()
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(status_ != ConnectionStatus_ESTABLISHED) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return blaze::RetCode_BADSTTS;
    }
    DECLINITH_GBB(gbb, 3*4)
    build_PKT_CONRES(conres_,
                     conrescode_,
                     srv_agrhbt_,
                     connid_,
                     gbb);
    gbb->flip();
    RETURN_IF_NOT_OK(pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_SendPacket, this);
    if((cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt))) {
        set_status(ConnectionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p) - RES:%d", __func__, this, cdrs_res))
    return cdrs_res;
}

blaze::RetCode connection_int::disconnect(DisconnectionResultReason
                                          disres)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(status_ != ConnectionStatus_PROTOCOL_HANDSHAKE &&
            status_ != ConnectionStatus_AUTHENTICATED) {
        IFLOG(err(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
        return blaze::RetCode_BADSTTS;
    }
    IFLOG(inf(TH_ID,
              LS_CON"[connid:%d][socket:%d][sending disconnection][disconrescode:%d]",
              connid_, socket_, disres))
    set_disconnecting();
    DECLINITH_GBB(gbb, 3)
    if(con_type_ == ConnectionType_OUTGOING) {
        build_PKT_DSCOND(disres, connid_, gbb);
    } else {
        build_PKT_DSCOND(disres, 0, gbb);
    }
    gbb->flip();
    RETURN_IF_NOT_OK(pkt_sending_q_.put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_Disconnect, this);
    if((cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt))) {
        set_status(ConnectionStatus_ERROR);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p) - RES:%d", __func__, this, cdrs_res))
    return cdrs_res;
}

blaze::RetCode connection_int::await_for_connection_result(
    ConnectivityEventResult
    &con_evt_res,
    ConnectivityEventType &connectivity_evt_type,
    time_t sec,
    long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p)", __func__, this))
    if(status_ < ConnectionStatus_INITIALIZED) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADSTTS;
    }
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    CHK_MON_ERR_0(lock)
    while(!connect_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res =  blaze::RetCode_TIMEOUT;
                break;
            }
        }
    }
    con_evt_res = con_evt_res_;
    connectivity_evt_type = connectivity_evt_type_;
    IFLOG(log(cdrs_res ? blaze::TL_WRN : blaze::TL_DBG, TH_ID, LS_CLO
              "%s(ptr:%p, res:%d, socket:%d, last_socket_err:%d, status:%d) - [connection result available] - con_evt_res:%d connectivity_evt_type:%d, conres:%d, resultcode:%d",
              __func__,
              this,
              cdrs_res,
              socket_,
              last_socket_err_,
              status_,
              con_evt_res_,
              connectivity_evt_type_,
              conres_,
              conrescode_))
    connect_evt_occur_ = false;
    CHK_MON_ERR_0(unlock)
    return cdrs_res;
}

blaze::RetCode connection_int::notify_for_connectivity_result(
    ConnectivityEventResult con_evt_res,
    ConnectivityEventType connectivity_evt_type)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(ptr:%p, connectivity_evt_type:%d)", __func__, this,
              connectivity_evt_type))
    mon_.lock();
    connect_evt_occur_ = true;
    con_evt_res_ = con_evt_res;
    connectivity_evt_type_ = connectivity_evt_type;
    mon_.notify_all();
    mon_.unlock();
    IFLOG(trc(TH_ID, LS_CLO "%s(ptr:%p)", __func__, this))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::await_for_disconnection_result(
    ConnectivityEventResult &con_evt_res,
    ConnectivityEventType &connectivity_evt_type,
    time_t sec,
    long nsec)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(connid:%d)", __func__, connid_))
    CHK_MON_ERR_0(lock)
    if(status_ < ConnectionStatus_INITIALIZED) {
        CHK_MON_ERR_0(unlock)
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADSTTS;
    }
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    while(!connect_evt_occur_) {
        int pthres;
        if((pthres = mon_.wait(sec, nsec))) {
            if(pthres == ETIMEDOUT) {
                cdrs_res =  blaze::RetCode_TIMEOUT;
                break;
            }
        }
    }
    con_evt_res = con_evt_res_;
    connectivity_evt_type = connectivity_evt_type_;
    IFLOG(log(cdrs_res ? blaze::TL_WRN : blaze::TL_DBG, TH_ID, LS_CLO
              "%s(connid:%d, res:%d, socket:%d, last_socket_err:%d, status:%d) - [disconnection result available] - con_evt_res:%d connectivity_evt_type:%d, conres:%d, resultcode:%d",
              __func__,
              connid_,
              cdrs_res,
              socket_,
              last_socket_err_,
              status_,
              con_evt_res_,
              connectivity_evt_type_,
              conres_,
              conrescode_))
    connect_evt_occur_ = false;
    CHK_MON_ERR_0(unlock)
    return cdrs_res;
}

blaze::RetCode connection_int::recv_connection_request(const blz_hdr_rec
                                                       *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, pkt_hdr:%p)", __func__, get_socket(),
              pkt_hdr))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(status() == ConnectionStatus_PROTOCOL_HANDSHAKE ||
            status() == ConnectionStatus_AUTHENTICATED) {
        set_disconnecting();
        set_conn_response(ConnectionResult_REFUSED);
        set_conn_res_code(ConnectionResultReason_ALREADY_CONNECTED);
        IFLOG(wrn(TH_ID, LS_CLO "%s(sockid:%d) [peer already connected - connid:%d]",
                  __func__, get_socket(), connid()))
        server_send_connect_res();
        return blaze::RetCode_KO;
    }
    if(status() != ConnectionStatus_ESTABLISHED) {
        set_disconnecting();
        set_conn_response(ConnectionResult_REFUSED);
        set_conn_res_code(ConnectionResultReason_INVALID_CONNECTION_STATUS);
        IFLOG(err(TH_ID, LS_CLO "%s(sockid:%d) [invalid connection status - connid:%d]",
                  __func__, get_socket(), connid()))
        server_send_connect_res();
        return blaze::RetCode_KO;
    }
    set_conn_response(ConnectionResult_ACCEPTED);
    set_conn_res_code(ConnectionResultReason_NO_ERROR);
    set_server_agrhbt(pkt_hdr->row_1.clihbt.hbtsec);
    sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    getpeername(socket_, (sockaddr *)&saddr, &len);
    if(!(cdrs_res = peer_.new_incoming_connection_accept(*this))) {
        if((cdrs_res = server_send_connect_res())) {
            set_internal_error(cdrs_res);
            IFLOG(err(TH_ID,
                      LS_CON"[error responding to peer: sockid:%d, host:%s, port:%d]",
                      socket_,
                      inet_ntoa(saddr.sin_addr),
                      ntohs(saddr.sin_port)))
        } else {
            set_proto_connected();
            on_connect(ConnectivityEventResult_OK,
                       ConnectivityEventType_PROTOCOL);
            IFLOG(inf(TH_ID,
                      LS_CON"[peer: sockid:%d, host:%s, port:%d is now connected with connid:%d]",
                      socket_,
                      inet_ntoa(saddr.sin_addr),
                      ntohs(saddr.sin_port),
                      connid_))
        }
    } else {
        set_disconnecting();
        set_conn_response(ConnectionResult_REFUSED);
        set_conn_res_code(ConnectionResultReason_APPLICATIVE_REFUSAL);
        server_send_connect_res();
        IFLOG(inf(TH_ID,
                  LS_CON"[peer: sockid:%d, host:%s, port:%d peer applicatively refused new connection]",
                  socket_,
                  inet_ntoa(saddr.sin_addr),
                  ntohs(saddr.sin_port),
                  connid_))
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(sockid:%d)", __func__, get_socket()))
    return cdrs_res;
}

blaze::RetCode connection_int::recv_connection_response(
    const blz_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(pkt_hdr:%p)", __func__, pkt_hdr))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    ConnectivityEventResult con_evt_res = ConnectivityEventResult_OK;
    set_conn_response(pkt_hdr->row_1.srvcrs.conres);
    set_conn_res_code(pkt_hdr->row_1.srvcrs.errcod);
    set_server_agrhbt(pkt_hdr->row_1.srvcrs.agrhbt);
    set_connid(pkt_hdr->row_2.connid.connid);
    selector_event *evt = new selector_event(BLZ_SELECTOR_Evt_Undef,
                                             this);
    switch(conn_response()) {
        case ConnectionResult_ACCEPTED:
            IFLOG(inf(TH_ID, LS_CON"[connection accepted by peer][connid:%d][socket:%d]",
                      connid_, socket_))
            set_proto_connected();
            evt->evt_ = BLZ_SELECTOR_Evt_ConnReqAccepted;
            break;
        case ConnectionResult_CONDITIONALLY_ACCEPTED:
            IFLOG(inf(TH_ID,
                      LS_CON"[connection accepted by peer - with reserve][connid:%d][socket:%d]",
                      connid_, socket_))
            set_proto_connected();
            evt->evt_ = BLZ_SELECTOR_Evt_ConnReqAccepted;
            break;
        case ConnectionResult_REFUSED:
            IFLOG(wrn(TH_ID, LS_CON"[connection refused by peer]"))
            evt->evt_ = BLZ_SELECTOR_Evt_ConnReqRefused;
            con_evt_res = ConnectivityEventResult_KO;
            break;
        default:
            IFLOG(err(TH_ID, LS_CON"[protocol error]"))
            evt->evt_ = BLZ_SELECTOR_Evt_ConnReqRefused;
            con_evt_res = ConnectivityEventResult_KO;
            break;
    }
    if((cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt))) {
        set_internal_error(cdrs_res);
        return cdrs_res;
    }
    on_connect(con_evt_res, ConnectivityEventType_PROTOCOL);
    notify_for_connectivity_result(con_evt_res,
                                   ConnectivityEventType_PROTOCOL);
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::recv_disconnection(const blz_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(pkt_hdr:%p)", __func__, pkt_hdr))
    disconrescode_ = pkt_hdr->row_1.diswrd.disres;
    IFLOG(inf(TH_ID,
              LS_CON"[connid:%d][socket:%d][received disconnection - disconrescode:%d]",
              connid_, socket_,
              disconrescode_))
    set_disconnecting();
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::recv_test_request(const blz_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(pkt_hdr:%p)", __func__, pkt_hdr))
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_UNSP;
}

// TRANSACTIONAL

blaze::RetCode connection_int::next_tx_id(tx_id &txid)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    txid.txplid = peer_.peer_plid_;
    txid.txsvid = peer_.peer_svid_;
    txid.txcnid = connid();
    txid.txprid = next_prid();
    IFLOG(trc(TH_ID, LS_CLO "%s() generated txid: [%08x%08x%08x%08x]",
              __func__,
              txid.txplid,
              txid.txsvid,
              txid.txcnid,
              txid.txprid))
    return blaze::RetCode_OK;
}

transaction_int *connection_int::blz_tx_factory_default_func(
    connection_int &connection, void *ud)
{
    transaction_int *new_transaction = new transaction_int(
        connection);
    return new_transaction;
}

blaze::RetCode connection_int::new_transaction(transaction_int
                                               **new_transaction,
                                               blz_tx_factory_func blz_tx_factory_f,
                                               bool compute_txid,
                                               void *ud)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(new_transaction:%p)", __func__, new_transaction))
    if(!new_transaction) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADARG;
    }
    if(!blz_tx_factory_f) {
        *new_transaction = blz_tx_factory_default_func(*this, NULL);
    } else {
        *new_transaction = blz_tx_factory_f(*this, ud);
    }
    if((*new_transaction)) {
        (*new_transaction)->init();
        if(compute_txid) {
            next_tx_id((*new_transaction)->txid());
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(new_transaction:%p)", __func__, *new_transaction))
    return blaze::RetCode_OK;
}

blaze::RetCode connection_int::release_transaction(transaction_int *transaction)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(transaction:%p)", __func__, transaction))
    if(&transaction->conn_ != this) {
        IFLOG(err(TH_ID, LS_CLO "%s(conn:%p, connid:%d) [transaction:%p is not mine]",
                  __func__, this, connid_, transaction))
        return blaze::RetCode_KO;
    }
    if(transaction->status() == TransactionStatus_FLYING) {
        IFLOG(err(TH_ID, LS_CLO
                  "%s(conn:%p, connid:%d) [transaction:%p is not in a valid state:%d]", __func__,
                  this, connid_,
                  transaction, transaction->status()))
        return blaze::RetCode_BADSTTS;
    }
    if(con_type_ == ConnectionType_INGOING) {
        /************************
        RELEASE_ID: TRX_SRV_01
        ************************/
        blaze::collector &c = transaction->get_collector();
        c.release(transaction);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return blaze::RetCode_OK;
}

/*only server*/
blaze::RetCode connection_int::recv_tx_req(const blz_hdr_rec *pkt_hdr,
                                           blaze::grow_byte_buffer *pkt_body)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, connid:%d, pkt_hdr:%p, pkt_body:%p)",
              __func__, get_socket(), connid(),
              pkt_hdr,
              pkt_body))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    bool skip_appl_mng = false;
    bool aborted = false;
    transaction_int *trans = NULL;
    new_transaction(&trans, tx_factory_, false, tx_factory_ud_);
    //server side adoption of new tx.
    /************************
    RETAIN_ID: TRX_SRV_01
    ************************/
    blaze::collector &c = trans->get_collector();
    c.retain(trans);

    trans->set_tx_req_type(pkt_hdr->row_1.txreqw.txtype);
    trans->set_tx_act(pkt_hdr->row_1.txreqw.txactn);
    trans->set_result_class_req(pkt_hdr->row_1.txreqw.rsclrq);
    trans->set_tx_id_PLID(pkt_hdr->row_2.txplid.txplid);
    trans->set_tx_id_SVID(pkt_hdr->row_3.txsvid.txsvid);
    trans->set_tx_id_CNID(pkt_hdr->row_4.txcnid.txcnid);
    trans->set_tx_id_PRID(pkt_hdr->row_5.txprid.txprid);
    blaze::rt_mark_time(&trans->start_mark_tim_);
    IFLOG(inf(TH_ID, LS_TRX"[%08x%08x%08x%08x][TXTYPE:%d, TXACT:%d, RSCLREQ:%d]",
              pkt_hdr->row_2.txplid.txplid,
              pkt_hdr->row_3.txsvid.txsvid,
              pkt_hdr->row_4.txcnid.txcnid,
              pkt_hdr->row_5.txprid.txprid,
              pkt_hdr->row_1.txresw.txresl,
              pkt_hdr->row_1.txresw.blzcod,
              pkt_hdr->row_1.txresw.rescls))
    if(!srv_flytx_repo_.contains_key(trans->tx_id_ptr())) {
        trans->set_tx_res(TransactionResult_FAILED);
        trans->set_tx_result_code(ProtocolCode_TRANSACTION_ALREADY_FLYING);
        trans->set_result_class_set(false);
        aborted = true;
        IFLOG(err(TH_ID, LS_TXT"[same tx already flying]"))
    } else {
        if((cdrs_res = srv_flytx_repo_.put(trans->tx_id_ptr(), &trans))) {
            trans->set_tx_res(TransactionResult_FAILED);
            trans->set_tx_result_code(ProtocolCode_SERVER_ERROR);
            trans->set_result_class_set(false);
            aborted = true;
            IFLOG(cri(TH_ID, LS_TXT"[error putting tx into flying map - res:%d]", cdrs_res))
        } else {
            IFLOG(trc(TH_ID, LS_TXT"[tx put into flying map]"))
        }
    }
    if(!aborted) {
        trans->set_flying();
        if(pkt_hdr->phdr.hdrlen == 9) {
            trans->set_tx_req_class_id(pkt_hdr->row_7.clsenc.nclsid);
            trans->set_tx_req_class_encode(pkt_hdr->row_7.clsenc.enctyp);
            trans->set_tx_res_class_encode(pkt_hdr->row_7.clsenc.enctyp);
            nclass *req_obj = NULL;
            if((cdrs_res = peer_.bem_.new_class_instance(trans->tx_req_class_id(),
                                                         &req_obj))) {
                trans->set_tx_res(TransactionResult_FAILED);
                trans->set_tx_result_code(ProtocolCode_MALFORMED_REQUEST);
                trans->set_result_class_set(false);
                skip_appl_mng = true;
                IFLOG(err(TH_ID,
                          LS_TXT"[tx request receive failed - new class fail:%d, nclass_id:%d]", cdrs_res,
                          trans->tx_req_class_id()))
            } else {
                trans->set_request_obj_on_request(req_obj);
                if((cdrs_res = req_obj->restore(&peer_.bem_, trans->tx_req_class_encode(),
                                                pkt_body))) {
                    trans->set_tx_res(TransactionResult_FAILED);
                    trans->set_tx_result_code(ProtocolCode_MALFORMED_REQUEST);
                    trans->set_result_class_set(false);
                    skip_appl_mng = true;
                    IFLOG(err(TH_ID,
                              LS_TXT"[tx request receive failed - class restore fail:%d, nclass_id:%d]",
                              cdrs_res,
                              trans->tx_req_class_id()))
                }
            }
        }
        if(!skip_appl_mng) {
            trans->on_request();
        }
    }
    if((cdrs_res = trans->send_response())) {
        IFLOG(err(TH_ID, LS_TXT"[tx response sending failed res:%d]", cdrs_res))
    }
    if((cdrs_res = srv_flytx_repo_.remove(trans->tx_id_ptr(), NULL))) {
        IFLOG(cri(TH_ID, LS_TXT"[error removing tx from flying map - res:%d]",
                  cdrs_res))
    } else {
        IFLOG(trc(TH_ID, LS_TXT"[tx removed from flying map]"))
    }
    if(aborted) {
        trans->set_aborted();
    } else {
        trans->set_closed();
    }
    release_transaction(trans);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

blaze::RetCode connection_int::recv_tx_res(const blz_hdr_rec *pkt_hdr,
                                           blaze::grow_byte_buffer *pkt_body)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, connid:%d, pkt_hdr:%p, pkt_body:%p)",
              __func__, get_socket(), connid(),
              pkt_hdr,
              pkt_body))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    bool aborted = false;
    tx_id txid;
    txid.txplid = pkt_hdr->row_2.txplid.txplid;
    txid.txsvid = pkt_hdr->row_3.txsvid.txsvid;
    txid.txcnid = pkt_hdr->row_4.txcnid.txcnid;
    txid.txprid = pkt_hdr->row_5.txprid.txprid;
    IFLOG(dbg(TH_ID, LS_INC"[%08x%08x%08x%08x][TXRES:%d, TXRESCODE:%d, RESCLS:%d] ",
              txid.txplid,
              txid.txsvid,
              txid.txcnid,
              txid.txprid,
              pkt_hdr->row_1.txresw.txresl,
              pkt_hdr->row_1.txresw.blzcod,
              pkt_hdr->row_1.txresw.rescls))
    transaction_int *trans = NULL;
    if((cdrs_res = cli_flytx_repo_.get(&txid, &trans))) {
        IFLOG(err(TH_ID, LS_TXT"[tx not found, aborting.]"))
        IFLOG(err(TH_ID, LS_CLO "%s(aborted)", __func__))
        return blaze::RetCode_ABORT;
    }
    trans->set_tx_res(pkt_hdr->row_1.txresw.txresl);
    trans->set_tx_result_code(pkt_hdr->row_1.txresw.blzcod);
    trans->set_result_class_set(pkt_hdr->row_1.txresw.rescls);
    if(pkt_hdr->phdr.hdrlen == 8) {
        trans->set_tx_res_class_id(pkt_hdr->row_7.clsenc.nclsid);
        trans->set_tx_res_class_encode(pkt_hdr->row_7.clsenc.enctyp);
        nclass *nobj = NULL;
        if((cdrs_res = peer_.bem_.new_class_instance(trans->tx_res_class_id(),
                                                     &nobj))) {
            IFLOG(err(TH_ID,
                      LS_TXT"[tx response receive failed - new class fail:%d, nclass_id:%d]",
                      cdrs_res,
                      trans->tx_res_class_id()))
            aborted = true;
        }
        trans->set_result_obj_on_response(nobj);
        if((cdrs_res = nobj->restore(&peer_.bem_, trans->tx_res_class_encode(),
                                     pkt_body))) {
            IFLOG(err(TH_ID,
                      LS_TXT"[tx response receive failed - class restore fail:%d, nclass_id:%d]",
                      cdrs_res,
                      trans->tx_res_class_id()))
            aborted = true;
        }
    }
    if((cdrs_res = cli_flytx_repo_.remove(trans->tx_id_ptr(), NULL))) {
        IFLOG(cri(TH_ID, LS_TXT"[error removing tx from flying map - res:%d]",
                  cdrs_res))
    } else {
        IFLOG(trc(TH_ID, LS_TXT"[tx removed from flying map]"))
    }
    if(aborted) {
        trans->set_aborted();
    } else {
        trans->set_closed();
    }
    //client side release.
    /************************
    RELEASE_ID: TRX_CLI_01
    ************************/
    blaze::collector &c = trans->get_collector();
    c.release(trans);

    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

// SUBSCRIPTION

subscription_int *connection_int::blz_sbs_factory_default_func(
    connection_int &connection, void *ud)
{
    subscription_int *new_subscription = new subscription_int(
        connection);
    return new_subscription;
}

blaze::RetCode connection_int::new_subscription(subscription_int
                                                **new_subscription,
                                                blz_sbs_factory_func  blz_sbs_factory_f,
                                                void *ud)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(new_subscription:%p)", __func__, new_subscription))
    if(!new_subscription) {
        IFLOG(err(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_BADARG;
    }
    if(!blz_sbs_factory_f) {
        *new_subscription = blz_sbs_factory_default_func(*this, NULL);
    } else {
        *new_subscription = blz_sbs_factory_f(*this, ud);
    }
    if((*new_subscription)) {
        (*new_subscription)->init();
    } else {
        IFLOG(cri(TH_ID, LS_CLO "%s() - new subscription fail.", __func__))
        return blaze::RetCode_MEMERR;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(new_subscription:%p)", __func__, *new_subscription))
    return blaze::RetCode_OK;
}

//client only
blaze::RetCode connection_int::detach_subscription(subscription_int
                                                   *subscription)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(subscription:%p)", __func__, subscription))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(&subscription->conn_ != this) {
        IFLOG(err(TH_ID, LS_CLO "%s(connid:%d) [subscription:%d is not mine]", __func__,
                  connid_,
                  subscription->sbsid()))
        return blaze::RetCode_KO;
    }
    if(subscription->status() == SubscriptionStatus_REQUEST_SENT ||
            subscription->status() == SubscriptionStatus_STARTED ||
            subscription->status() == SubscriptionStatus_RELEASED) {
        IFLOG(wrn(TH_ID, LS_CLO
                  "%s(connid:%d) [subscription:%d is not in a valid state for detaching:%d]",
                  __func__,
                  connid_,
                  subscription->sbsid(),
                  subscription->status()))
        return blaze::RetCode_BADSTTS;
    }
    if(con_type_ == ConnectionType_INGOING) {
        IFLOG(err(TH_ID, LS_CLO "%s(connid:%d) [invalid subscription type]", __func__,
                  connid_,
                  subscription->sbsid()))
        return blaze::RetCode_KO;
    } else {
        /*for additional safety we try to remove also from cli_reqid_sbs_repo_*/
        cli_reqid_sbs_repo_.remove(&subscription->reqid_, NULL);
        cli_sbsid_sbs_repo_.remove(&subscription->sbsid_, NULL);
        /************************
        RELEASE_ID: SBS_CLI_01
        ************************/
        blaze::collector &c = subscription->get_collector();
        c.release(subscription);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

blaze::RetCode connection_int::release_subscription(subscription_int
                                                    *subscription)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(subscription:%p)", __func__, subscription))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    if(&subscription->conn_ != this) {
        IFLOG(err(TH_ID, LS_CLO "%s(connid:%d) [subscription:%d is not mine]", __func__,
                  connid_,
                  subscription->sbsid()))
        return blaze::RetCode_KO;
    }
    if(subscription->status() == SubscriptionStatus_REQUEST_SENT ||
            subscription->status() == SubscriptionStatus_STARTED ||
            subscription->status() == SubscriptionStatus_RELEASED) {
        IFLOG(wrn(TH_ID, LS_CLO
                  "%s(connid:%d) [subscription:%d is not in a valid state for releasing:%d]",
                  __func__,
                  connid_,
                  subscription->sbsid(),
                  subscription->status()))
        return blaze::RetCode_BADSTTS;
    }
    if(con_type_ == ConnectionType_INGOING) {
        subscription->release_initial_query();
        peer_.remove_subscriber(subscription);
        if((cdrs_res = srv_sbsid_sbs_repo_.remove(&subscription->sbsid_, NULL))) {
            IFLOG(cri(TH_ID,
                      LS_SBT"%s(connid:%d) [error releasing sbs:%d from srv sbsid map]",__func__,
                      connid_,
                      subscription->sbsid()))
        }
        if((cdrs_res = srv_classid_sbs_repo_.remove(&subscription->nclassid_, NULL))) {
            IFLOG(cri(TH_ID,
                      LS_SBT"%s(connid:%d) [error releasing sbs:%d from srv nclass_id map]", __func__,
                      connid_,
                      subscription->sbsid()))
        }
        //server side subscription is autoreleasing.
        /************************
        RELEASE_ID: SBS_SRV_01
        ************************/
        subscription->set_released();
        blaze::collector &c = subscription->get_collector();
        c.release(subscription);
    } else {
        /*for additional safety we try to remove also from cli_reqid_sbs_repo_*/
        cli_reqid_sbs_repo_.remove(&subscription->reqid_, NULL);
        cli_sbsid_sbs_repo_.remove(&subscription->sbsid_, NULL);
        /************************
        RELEASE_ID: SBS_CLI_01
        ************************/
        subscription->set_released();
        blaze::collector &c = subscription->get_collector();
        c.release(subscription);
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

/*server only*/
blaze::RetCode connection_int::recv_sbs_start_req(const blz_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, connid:%d, pkt_hdr:%p)", __func__,
              get_socket(), connid(), pkt_hdr))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    unsigned int sbsid = next_sbsid();
    subscription_int *inc_sbs = NULL;
    new_subscription(&inc_sbs, sbs_factory_, sbs_factory_ud_);
    //server side adoption for autorelease.
    /************************
    RETAIN_ID: SBS_SRV_01
    ************************/
    blaze::collector &c = inc_sbs->get_collector();
    c.retain(inc_sbs);

    inc_sbs->sbstyp_ = pkt_hdr->row_1.sbreqw.sbstyp;
    inc_sbs->sbsmod_ = pkt_hdr->row_1.sbreqw.sbsmod;
    inc_sbs->flotyp_ = pkt_hdr->row_1.sbreqw.flotyp;
    inc_sbs->dwltyp_ = pkt_hdr->row_1.sbreqw.dwltyp;
    inc_sbs->enctyp_ = pkt_hdr->row_2.clsenc.enctyp;
    inc_sbs->nclassid_ = pkt_hdr->row_2.clsenc.nclsid;
    inc_sbs->reqid_ = pkt_hdr->row_4.rqstid.rqstid;
    if(pkt_hdr->phdr.hdrlen == 7) {
        inc_sbs->open_tmstp0_ = pkt_hdr->row_5.tmstmp.tmstmp;
        inc_sbs->open_tmstp1_ = pkt_hdr->row_6.tmstmp.tmstmp;
    }
    IFLOG(inf(TH_ID,
              LS_SBI"[CONNID:%010u-REQID:%010u][SBSTYP:%d, SBSMOD:%d, FLOTYP:%d, DWLTYP:%d, ENCTYP:%d, NCLSSID:%d, TMSTP0:%u, TMSTP1:%u]",
              connid_,
              inc_sbs->reqid_,
              inc_sbs->sbstyp_,
              inc_sbs->sbsmod_,
              inc_sbs->flotyp_,
              inc_sbs->dwltyp_,
              inc_sbs->enctyp_,
              inc_sbs->nclassid_,
              inc_sbs->open_tmstp0_,
              inc_sbs->open_tmstp1_
             ))
    entity_desc const *edesc = NULL;
    if((cdrs_res = peer_.get_em().get_entity_descriptor(inc_sbs->nclassid_,
                                                        &edesc))) {
        inc_sbs->sbresl_ = SubscriptionResponse_KO;
        inc_sbs->last_blzcod_ = ProtocolCode_UNSUPPORTED_REQUEST;
        IFLOG(err(TH_ID, LS_SBT"[unk. nclass_id requested in subscription: %u]",
                  inc_sbs->nclassid_))
    } else {
        if(!(cdrs_res = srv_classid_sbs_repo_.contains_key(&inc_sbs->nclassid_))) {
            inc_sbs->sbresl_ = SubscriptionResponse_KO;
            inc_sbs->last_blzcod_ = ProtocolCode_SUBSCRIPTION_ALREADY_STARTED;
            IFLOG(err(TH_ID,
                      LS_SBT"[sbs on this connection:%d already started for class:%d]", connid_,
                      inc_sbs->nclassid_))
        } else {
            if((cdrs_res = srv_sbsid_sbs_repo_.put(&sbsid, &inc_sbs))) {
                inc_sbs->sbresl_ = SubscriptionResponse_KO;
                inc_sbs->last_blzcod_ = ProtocolCode_SERVER_ERROR;
                IFLOG(cri(TH_ID, LS_SBT"[error putting sbs into sbsid map - res:%d]", cdrs_res))
            }
            if((cdrs_res = srv_classid_sbs_repo_.put(&inc_sbs->nclassid_, &inc_sbs))) {
                inc_sbs->sbresl_ = SubscriptionResponse_KO;
                inc_sbs->last_blzcod_ = ProtocolCode_SERVER_ERROR;
                IFLOG(cri(TH_ID, LS_SBT"[error putting sbs into nclass_id map - res:%d]",
                          cdrs_res))
            }
            inc_sbs->sbsid_ = sbsid;
            inc_sbs->sbresl_ = SubscriptionResponse_OK;
            inc_sbs->last_blzcod_ = ProtocolCode_SUCCESS;
        }
    }
    if((cdrs_res = inc_sbs->send_start_response())) {
        IFLOG(err(TH_ID, LS_SBT"[sbs response sending failed res:%d]", cdrs_res))
    }
    if(inc_sbs->sbresl_ == SubscriptionResponse_OK ||
            inc_sbs->sbresl_ == SubscriptionResponse_PARTIAL) {
        if((cdrs_res = peer_.add_subscriber(inc_sbs))) {
            IFLOG(cri(TH_ID, LS_SBT"[error adding sbs into peer res:%d]", cdrs_res))
            inc_sbs->set_error();
        } else {
            if(inc_sbs->get_sbsmod() == SubscriptionMode_ALL ||
                    inc_sbs->get_sbsmod() == SubscriptionMode_DOWNLOAD) {
                //do initial query.
                if(!(cdrs_res = inc_sbs->execute_initial_query())) {
                    inc_sbs->set_initial_query_ended(false);
                } else {
                    inc_sbs->set_initial_query_ended(true);
                }
            }
            inc_sbs->set_started();
            if(!(inc_sbs->is_initial_query_ended())) {
                inc_sbs->safe_submit_dwnl_event();
            }
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return cdrs_res;
}

/*client only*/
blaze::RetCode connection_int::recv_sbs_start_res(const blz_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, connid:%d, pkt_hdr:%p)", __func__,
              get_socket(), connid(), pkt_hdr))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    unsigned int sbsid = ((pkt_hdr->row_1.sbresw.sbresl ==
                           SubscriptionResponse_OK) ||
                          (pkt_hdr->row_1.sbresw.sbresl == SubscriptionResponse_PARTIAL)) ?
                         pkt_hdr->row_3.sbsrid.sbsrid :
                         0;
    IFLOG(inf(TH_ID,
              LS_SBI"[CONNID:%010u-REQID:%010u][SBSRES:%d, BLZCOD:%d, SBSID:%u]",
              connid_,
              pkt_hdr->row_2.rqstid.rqstid,
              pkt_hdr->row_1.sbresw.sbresl,
              pkt_hdr->row_1.sbresw.blzcod,
              sbsid))
    subscription_int *sbs = NULL;
    if((cdrs_res = cli_reqid_sbs_repo_.remove(&pkt_hdr->row_2.rqstid.rqstid,
                                              &sbs))) {
        IFLOG(err(TH_ID, LS_SBT"[sbs not found, aborting.]"))
        return blaze::RetCode_KO;
    }
    sbs->sbsid_ = sbsid;
    sbs->sbresl_ = pkt_hdr->row_1.sbresw.sbresl;
    sbs->last_blzcod_ = pkt_hdr->row_1.sbresw.blzcod;
    if((cdrs_res = cli_sbsid_sbs_repo_.put(&sbsid, &sbs))) {
        IFLOG(cri(TH_ID, LS_SBT"[error putting sbs into sbsid map - res:%d]", cdrs_res))
        sbs->set_status(SubscriptionStatus_ERROR);
        return cdrs_res;
    }
    if(!sbsid) {
        sbs->set_stopped();
    } else {
        sbs->set_started();
    }
    sbs->notify_for_start_stop_result();
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return cdrs_res;
}

blaze::RetCode connection_int::recv_sbs_evt(const blz_hdr_rec *pkt_hdr,
                                            blaze::grow_byte_buffer *pkt_body)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, connid:%d, pkt_hdr:%p, pkt_body:%p)",
              __func__,
              get_socket(),
              connid(),
              pkt_hdr,
              pkt_body))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    bool mng = true;
    subscription_event_int *new_sbs_event = NULL;
    if((subscription_event_int::new_instance(&new_sbs_event))) {
        IFLOG(cri(TH_ID, LS_CLO "%s", __func__))
        return blaze::RetCode_MEMERR;
    }

    blaze::collector &c = new_sbs_event->get_collector();
    /*adopted until AckEvent is called on related sbs*/
    /************************
    RETAIN_ID: SBE_CLI_01
    ************************/
    c.retain(new_sbs_event);

    (new_sbs_event)->set_sbsid(pkt_hdr->row_1.sbsrid.sbsrid);
    (new_sbs_event)->set_evtid(pkt_hdr->row_3.sevtid.sevtid);
    (new_sbs_event)->set_evttype(pkt_hdr->row_2.sevttp.sevttp);
    (new_sbs_event)->set_protocode(pkt_hdr->row_2.sevttp.blzcod);
    (new_sbs_event)->set_tmstp0(pkt_hdr->row_4.tmstmp.tmstmp);
    (new_sbs_event)->set_tmstp1(pkt_hdr->row_5.tmstmp.tmstmp);
    (new_sbs_event)->set_act(pkt_hdr->row_2.sevttp.sbeact);
    IFLOG(dbg(TH_ID, LS_SBS
              "[CONNID:%010u-SBSID:%010u][EVTID:%d, EVTTYP:%d, PRTCOD:%d, TMSTMP[0]:%d, TMSTMP[1]:%d, ACT:%d]",
              connid_,
              pkt_hdr->row_1.sbsrid.sbsrid,
              pkt_hdr->row_3.sevtid.sevtid,
              pkt_hdr->row_2.sevttp.sevttp,
              pkt_hdr->row_2.sevttp.blzcod,
              pkt_hdr->row_4.tmstmp.tmstmp,
              pkt_hdr->row_5.tmstmp.tmstmp,
              pkt_hdr->row_2.sevttp.sbeact))
    subscription_int *sbs = NULL;
    if((cdrs_res = cli_sbsid_sbs_repo_.get(&(new_sbs_event)->sbs_sbsid_, &sbs))) {
        IFLOG(cri(TH_ID, LS_SBT"[error getting sbs from sbsid map - res:%d]", cdrs_res))
        mng = false;
        /************************
        RELEASE_ID: SBE_CLI_01
        ************************/
        c.release(new_sbs_event);
    } else {
        if(mng && (cdrs_res = sbs->receive_event(pkt_hdr, pkt_body, new_sbs_event))) {
            IFLOG(wrn(TH_ID, LS_SBT"[sbs event:%d management failed res:%d]",
                      (new_sbs_event)->sbs_evtid_, cdrs_res))
            /************************
            RELEASE_ID: SBE_CLI_01
            ************************/
            c.release(new_sbs_event);
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return cdrs_res;
}

blaze::RetCode connection_int::recv_sbs_evt_ack(const blz_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, connid:%d, pkt_hdr:%p)", __func__,
              get_socket(), connid(), pkt_hdr))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    subscription_int *sbs = NULL;
    if((cdrs_res = srv_sbsid_sbs_repo_.get(&pkt_hdr->row_1.sbsrid.sbsrid, &sbs))) {
        IFLOG(cri(TH_ID,
                  LS_SBT"[sbs event ack - error getting sbs from sbsid map - res:%d]", cdrs_res))
    } else {
        if((cdrs_res = sbs->receive_event_ack(pkt_hdr))) {
            IFLOG(wrn(TH_ID, LS_SBT"[sbs event ack - management failed res:%d]", cdrs_res))
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
    return cdrs_res;
}

blaze::RetCode connection_int::recv_sbs_stop_req(const blz_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, connid:%d, pkt_hdr:%p)", __func__,
              get_socket(), connid(), pkt_hdr))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    unsigned int sbsid = pkt_hdr->row_1.sbsrid.sbsrid;
    IFLOG(inf(TH_ID, LS_SBS"[CONNID:%010u-SBSID:%010u][STOP REQUEST]",
              connid_,
              sbsid))
    SubscriptionResponse sbresl = SubscriptionResponse_OK;
    ProtocolCode protocode = ProtocolCode_SUCCESS;
    subscription_int *sbs = NULL;
    if((cdrs_res = srv_sbsid_sbs_repo_.get(&sbsid, &sbs))) {
        IFLOG(err(TH_ID, LS_SBT"[sbs stop - error getting sbs from sbsid map - res:%d]",
                  cdrs_res))
        sbresl = SubscriptionResponse_KO;
        protocode = ProtocolCode_SUBSCRIPTION_NOT_FOUND;
    } else {
        sbs->set_stopped();
        release_subscription(sbs);
    }
    DECLINITH_GBB(gbb, BLZ_BUFF_DEF_SZ)
    build_PKT_SBSSPR(sbresl,
                     protocode,
                     sbsid,
                     gbb);
    gbb->flip();
    RETURN_IF_NOT_OK(pkt_snd_q().put(&gbb))
    selector_event *evt = new selector_event(
        BLZ_SELECTOR_Evt_SendPacket, this);
    cdrs_res = peer_.get_selector().evt_enqueue_and_notify(evt);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

blaze::RetCode connection_int::recv_sbs_stop_res(const blz_hdr_rec *pkt_hdr)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(sockid:%d, connid:%d, pkt_hdr:%p)", __func__,
              get_socket(), connid(), pkt_hdr))
    blaze::RetCode cdrs_res = blaze::RetCode_OK;
    IFLOG(inf(TH_ID, LS_INC
              LS_SBS"[CONNID:%010u-SBSID:%010u][SBSRES:%d, BLZCOD:%d]",
              connid_,
              pkt_hdr->row_2.sbsrid.sbsrid,
              pkt_hdr->row_1.sbresw.sbresl,
              pkt_hdr->row_1.sbresw.blzcod))
    subscription_int *sbs = NULL;
    if((cdrs_res = cli_sbsid_sbs_repo_.get(&pkt_hdr->row_2.sbsrid.sbsrid, &sbs))) {
        IFLOG(cri(TH_ID, LS_SBT"[error getting sbs from sbsid map - res:%d]", cdrs_res))
    } else {
        sbs->notify_for_start_stop_result();
        if(pkt_hdr->row_1.sbresw.sbresl == SubscriptionResponse_OK) {
            sbs->set_stopped();
            detach_subscription(sbs);
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, cdrs_res))
    return cdrs_res;
}

void connection_int::on_connect(ConnectivityEventResult con_evt_res,
                                ConnectivityEventType connectivity_evt_type)
{}

void connection_int::on_disconnect(ConnectivityEventResult con_evt_res,
                                   ConnectivityEventType connectivity_evt_type)
{}

}
