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

#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg/vlg_connection_impl.h"
#include "vlg/vlg_transaction_impl.h"

namespace vlg {

extern vlg::synch_hash_map &int_publ_peer_map();
extern vlg::synch_hash_map &int_publ_srv_conn_map();

vlg::synch_hash_map *int_publ_srv_tx_map_ =
    NULL;  //transaction_impl --> transaction
vlg::synch_hash_map &int_publ_srv_tx_map()
{
    if(int_publ_srv_tx_map_) {
        return *int_publ_srv_tx_map_;
    }
    if(!(int_publ_srv_tx_map_ = new vlg::synch_hash_map(
        vlg::sngl_ptr_obj_mng(),
        vlg::sngl_ptr_obj_mng()))) {
        EXIT_ACTION("failed creating int_publ_srv_tx_map_\n")
    }
    int_publ_srv_tx_map_->init(HM_SIZE_NORM);
    return *int_publ_srv_tx_map_;
}

//-----------------------------
// CLASS transaction_impl
//-----------------------------
class transaction_impl_pub {
    private:
        //-----------------------------
        // CLASS timpl_transaction_impl_client
        //-----------------------------
        class timpl_transaction_impl_client : public transaction_impl {
            public:
                timpl_transaction_impl_client(connection_impl &conn,
                                              transaction &publ) : transaction_impl(conn), publ_(publ) {}
            public:
                virtual void on_request() {
                    publ_.on_request();
                }
                virtual void on_close() {
                    publ_.on_close();
                }
            private:
                transaction &publ_;
        };

        //-----------------------------
        // callbacks
        //-----------------------------
        static transaction_impl *vlg_client_tx_factory_timpl(connection_impl
                                                             &connection,
                                                             void *ud) {
            transaction *publ = static_cast<transaction *>(ud);
            return new timpl_transaction_impl_client(connection, *publ);
        }

        static void transaction_status_change_hndlr_timpl(transaction_impl &trans,
                                                          TransactionStatus status, void *ud) {
            transaction_impl_pub *timpl = static_cast<transaction_impl_pub *>(ud);
            if(timpl->tsh_) {
                timpl->tsh_(timpl->publ_, status, timpl->tsh_ud_);
            }
        }

        //-----------------------------
        // transaction_impl meths
        //-----------------------------
    public:
        transaction_impl_pub(transaction &publ) :
            publ_(publ),
            conn_(NULL),
            int_(NULL),
            tsh_(NULL),
            tsh_ud_(NULL),
            clh_(NULL),
            clh_ud_(NULL) {}
        ~transaction_impl_pub() {
            if(int_ && int_->get_connection().conn_type() == ConnectionType_OUTGOING) {
                vlg::collector &c = int_->get_collector();
                c.release(int_);
            }
        }

        transaction_impl *get_tx() const {
            return int_;
        }
        void set_tx(transaction_impl *val) {
            int_ = val;
        }

        connection *get_conn() const {
            return conn_;
        }
        void set_conn(connection &val) {
            conn_ = &val;
        }

        transaction::transaction_status_change get_tsh() const {
            return tsh_;
        }
        void set_tsh(transaction::transaction_status_change val) {
            tsh_ = val;
        }

        void *get_tsh_ud() const {
            return tsh_ud_;
        }
        void set_tsh_ud(void *val) {
            tsh_ud_ = val;
        }

        transaction::transaction_closure get_clh() const {
            return clh_;
        }
        void set_clh(transaction::transaction_closure val) {
            clh_ = val;
        }

        void *get_clh_ud() const {
            return clh_ud_;
        }
        void set_clh_ud(void *val) {
            clh_ud_ = val;
        }

        vlg::RetCode bind_implernal(connection &conn) {
            vlg::RetCode cdrs_res = vlg::RetCode_OK;
            if(conn.get_connection_type() == ConnectionType_OUTGOING) {
                transaction_impl *t_impl = NULL;
                if((cdrs_res = conn.get_implernal()->new_transaction(&t_impl,
                                                                     vlg_client_tx_factory_timpl,
                                                                     true,
                                                                     &publ_)) == vlg::RetCode_OK) {
                    int_ = t_impl;
                    vlg::collector &c = int_->get_collector();
                    c.retain(int_);
                }
            }
            int_->set_transaction_status_change_handler(
                transaction_status_change_hndlr_timpl, this);
            return cdrs_res;
        }

    private:
        transaction &publ_;
        connection *conn_;
        transaction_impl *int_;
        transaction::transaction_status_change tsh_;
        void *tsh_ud_;
        transaction::transaction_closure clh_;
        void *clh_ud_;
};

//*************************************
//transaction MEMORY MNGMENT BEGIN
//*************************************

class transaction_collector : public vlg::collector {
    public:
        transaction_collector() : vlg::collector("transaction") {}
};

vlg::collector *inst_transaction_collector = NULL;
vlg::collector &get_inst_transaction_collector()
{
    if(inst_transaction_collector) {
        return *inst_transaction_collector;
    }
    if(!(inst_transaction_collector = new transaction_collector())) {
        EXIT_ACTION("failed creating inst_transaction_collector\n")
    }
    return *inst_transaction_collector;
}

vlg::collector &transaction::get_collector()
{
    return get_inst_transaction_collector();
}

//-----------------------------
// CLASS transaction
//-----------------------------

nclass_logger *transaction::log_ = NULL;

transaction::transaction()
{
    log_ = get_nclass_logger("transaction");
    impl_ = new transaction_impl_pub(*this);
    IFLOG(trc(TH_ID, LS_CTR "%s(ptr:%p)", __func__, this))
}

transaction::~transaction()
{
    vlg::collector &c = get_collector();
    if((c.is_instance_collected(this))) {
        IFLOG(cri(TH_ID, LS_DTR "%s(ptr:%p)" D_W_R_COLL LS_EXUNX, __func__, this))
    }
    if(impl_) {
        delete impl_;
    }
    IFLOG(trc(TH_ID, LS_DTR "%s(ptr:%p)", __func__, this))
}

vlg::RetCode transaction::bind(connection &conn)
{
    impl_->set_conn(conn);
    return impl_->bind_implernal(conn);
}

connection *transaction::get_connection()
{
    return impl_->get_conn();
}

TransactionResult transaction::get_transaction_result()
{
    return impl_->get_tx()->tx_res();
}

ProtocolCode transaction::get_transaction_result_code()
{
    return impl_->get_tx()->tx_result_code();
}

TransactionRequestType transaction::get_transaction_request_type()
{
    return impl_->get_tx()->tx_req_type();
}

Action transaction::get_transaction_action()
{
    return impl_->get_tx()->tx_act();
}

unsigned int transaction::get_transaction_request_class_id()
{
    return impl_->get_tx()->tx_req_class_id();
}

Encode transaction::get_transaction_request_class_encode()
{
    return impl_->get_tx()->tx_req_class_encode();
}

unsigned int transaction::get_transaction_result_class_id()
{
    return impl_->get_tx()->tx_res_class_id();
}

Encode transaction::get_transaction_result_class_encode()
{
    return impl_->get_tx()->tx_res_class_encode();
}

bool transaction::is_transaction_result_class_required()
{
    return (impl_->get_tx()->is_result_class_req());
}

bool transaction::is_transaction_result_class_set()
{
    return (impl_->get_tx()->is_result_class_set());
}

const nclass *transaction::get_request_obj()
{
    return impl_->get_tx()->request_obj();
}

const nclass *transaction::get_current_obj()
{
    return impl_->get_tx()->current_obj();
}

const nclass *transaction::get_result_obj()
{
    return impl_->get_tx()->result_obj();
}

void transaction::set_transaction_result(TransactionResult tx_res)
{
    impl_->get_tx()->set_tx_res(tx_res);
}

void transaction::set_transaction_result_code(ProtocolCode tx_res_code)
{
    impl_->get_tx()->set_tx_result_code(tx_res_code);
}

void transaction::set_transaction_request_type(TransactionRequestType
                                               tx_req_type)
{
    impl_->get_tx()->set_tx_req_type(tx_req_type);
}

void transaction::set_transaction_action(Action tx_act)
{
    impl_->get_tx()->set_tx_act(tx_act);
}

void transaction::set_transaction_request_class_id(unsigned int nclass_id)
{
    impl_->get_tx()->set_tx_req_class_id(nclass_id);
}

void transaction::set_transaction_request_class_encode(Encode class_encode)
{
    impl_->get_tx()->set_tx_req_class_encode(class_encode);
}

void transaction::set_transaction_result_class_id(unsigned int nclass_id)
{
    impl_->get_tx()->set_tx_res_class_id(nclass_id);
}

void transaction::set_transaction_result_class_encode(Encode class_encode)
{
    impl_->get_tx()->set_tx_res_class_encode(class_encode);
}

void transaction::set_transaction_result_class_required(bool res_class_req)
{
    impl_->get_tx()->set_result_class_req(res_class_req);
}

void transaction::set_request_obj(const nclass *obj)
{
    impl_->get_tx()->set_request_obj(obj);
}

void transaction::set_current_obj(const nclass *obj)
{
    impl_->get_tx()->set_current_obj(obj);
}

void transaction::set_result_obj(const nclass *obj)
{
    impl_->get_tx()->set_result_obj(obj);
}

TransactionStatus transaction::get_status()
{
    return impl_->get_tx()->status();
}

vlg::RetCode transaction::await_for_status_reached_or_outdated(
    TransactionStatus
    test,
    TransactionStatus &current,
    time_t sec,
    long nsec)
{
    return impl_->get_tx()->await_for_status_reached_or_outdated(test, current, sec,
                                                                 nsec);
}

vlg::RetCode transaction::await_for_closure(time_t sec, long nsec)
{
    return  impl_->get_tx()->await_for_closure(sec, nsec);
}

void transaction::set_transaction_status_change_handler(
    transaction_status_change handler, void *ud)
{
    impl_->set_tsh(handler);
    impl_->set_tsh_ud(ud);
}

void transaction::set_transaction_closure_handler(transaction_closure handler,
                                                  void *ud)
{
    impl_->set_clh(handler);
    impl_->set_clh_ud(ud);
}

tx_id &transaction::get_transaction_id()
{
    return  impl_->get_tx()->txid();
}

void transaction::set_transaction_id(tx_id &txid)
{
    impl_->get_tx()->set_tx_id(txid);
}

unsigned int transaction::get_transaction_id_PLID()
{
    return  impl_->get_tx()->tx_id_PLID();
}

unsigned int transaction::get_transaction_id_SVID()
{
    return  impl_->get_tx()->tx_id_SVID();
}

unsigned int transaction::get_transaction_id_CNID()
{
    return  impl_->get_tx()->tx_id_CNID();
}

unsigned int transaction::get_transaction_id_PRID()
{
    return  impl_->get_tx()->tx_id_PRID();
}

void transaction::set_transaction_id_PLID(unsigned int plid)
{
    impl_->get_tx()->set_tx_id_PLID(plid);
}

void transaction::set_transaction_id_SVID(unsigned int svid)
{
    impl_->get_tx()->set_tx_id_SVID(svid);
}

void transaction::set_transaction_id_CNID(unsigned int cnid)
{
    impl_->get_tx()->set_tx_id_CNID(cnid);
}

void transaction::set_transaction_id_PRID(unsigned int prid)
{
    impl_->get_tx()->set_tx_id_PRID(prid);
}

vlg::RetCode transaction::renew()
{
    return impl_->get_tx()->re_new();
}

vlg::RetCode transaction::prepare()
{
    return impl_->get_tx()->prepare();
}

vlg::RetCode transaction::prepare(TransactionRequestType
                                  tx_request_type,
                                  Action tx_action,
                                  const nclass *sending_obj,
                                  const nclass *current_obj)
{
    return impl_->get_tx()->prepare(tx_request_type,
                                    tx_action,
                                    Encode_INDEXED_NOT_ZERO,
                                    impl_->get_tx()->is_result_class_req(),
                                    sending_obj,
                                    current_obj);
}

vlg::RetCode transaction::send()
{
    return impl_->get_tx()->send();
}

void transaction::on_request()
{
}

void transaction::on_close()
{
}

transaction_impl *transaction::get_implernal()
{
    return impl_->get_tx();
}

void transaction::set_implernal(transaction_impl *tx)
{
    impl_->set_tx(tx);
}

//-----------------------------
// CLASS timpl_transaction_impl_server
//-----------------------------
class timpl_transaction_impl_server : public transaction_impl {
    public:
        timpl_transaction_impl_server(connection_impl &conn,
                                      transaction &publ) : transaction_impl(conn), publ_(publ) {}
    public:
        virtual ~timpl_transaction_impl_server() {
            void *self = this;
            int_publ_srv_tx_map().remove(&self, NULL);
            /************************
            RELEASE_ID: TPB_SRV_01
            ************************/
            vlg::collector &c = publ_.get_collector();
            c.release(&publ_);
        }
        virtual void on_request() {
            publ_.on_request();
        }
        virtual void on_close() {
            publ_.on_close();
        }
    private:
        transaction &publ_;
};

//-----------------------------
// CLASS transaction_factory
//-----------------------------
transaction_factory *default_tx_factory = NULL;
transaction_factory *transaction_factory::default_transaction_factory()
{
    if(default_tx_factory  == NULL) {
        default_tx_factory  = new transaction_factory();
        if(!default_tx_factory) {
            EXIT_ACTION("failed creating default_tx_factory\n")
        }
    }
    return default_tx_factory;
}

transaction_impl *transaction_factory::tx_factory_impl_f(connection_impl &conn,
                                                         void *ud)
{
    transaction_factory *tsf = static_cast<transaction_factory *>(ud);
    connection_impl *connection_impl_ptr = &conn;
    connection *c_publ = NULL;
    if(int_publ_srv_conn_map().get(&connection_impl_ptr, &c_publ)) {
        EXIT_ACTION("failed get instance from int_publ_srv_conn_map\n")
    }
    transaction *publ = tsf->new_transaction(*c_publ);
    vlg::collector &c = publ->get_collector();
    /************************
    RETAIN_ID: TPB_SRV_01
    ************************/
    c.retain(publ);

    transaction_impl *int_tx =
        new timpl_transaction_impl_server(conn, *publ);

    publ->set_implernal(int_tx);
    publ->bind(*c_publ);
    int_publ_srv_tx_map().put(&int_tx, &publ);
    return int_tx;
}

transaction_factory::transaction_factory()
{}

transaction_factory::~transaction_factory()
{}

transaction *transaction_factory::new_transaction(connection &conn)
{
    return new transaction();
}


}
