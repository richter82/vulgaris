/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "vlg_peer.h"
#include "vlg_connection.h"
#include "vlg_transaction.h"
#include "vlg_subscription.h"
#include "vlg_logger.h"

using namespace vlg;

namespace vlg{
    RetCode load_vlg_logger();
}

extern "C" {
    RetCode set_logger_cfg_file_dir(const char *dir)
    {
        return logger::set_logger_cfg_file_dir(dir);
    }

    RetCode set_logger_cfg_file_path_name(const char *file_path)
    {
        return logger::set_logger_cfg_file_path_name(file_path);
    }

    RetCode load_logger_config()
    {
        return logger::load_logger_config();
    }

    RetCode load_logger_config_file_path_name(const char *file_path)
    {
        return logger::set_logger_cfg_file_path_name(file_path);
    }
    
    RetCode load_vlg_logger()
    {
        return vlg::load_vlg_logger();
    }
}

extern "C" {
    typedef struct own_peer own_peer;
    typedef struct shr_incoming_connection shr_incoming_connection;
    typedef struct shr_incoming_transaction shr_incoming_transaction;
    typedef struct shr_incoming_subscription shr_incoming_subscription;

    //peer

    typedef void (*peer_status_change)(peer *p, PeerStatus status, void *usr_data, void *usr_data2);
    typedef const char *(*peer_name_handler)(peer *p, void *usr_data, void *usr_data2);
    typedef const unsigned int *(*peer_version_handler)(peer *p, void *usr_data, void *usr_data2);
    typedef RetCode(*peer_load_config_handler)(peer *p, int pnum, const char *param, const char *value, void *usr_data, void *usr_data2);
    typedef RetCode(*peer_init_handler)(peer *p, void *usr_data, void *usr_data2);
    typedef RetCode(*peer_starting_handler)(peer *p, void *usr_data, void *usr_data2);
    typedef RetCode(*peer_stopping_handler)(peer *p, void *usr_data, void *usr_data2);
    typedef RetCode(*peer_on_move_running_handler)(peer *p, void *usr_data, void *usr_data2);
    typedef void(*peer_dying_breath_handler)(peer *p, void *usr_data, void *usr_data2);
    typedef RetCode(*peer_on_incoming_connection_handler)(peer *p, shr_incoming_connection *ic, void *usr_data, void *usr_data2);

    //incoming connection

    typedef void(*inco_connection_status_change)(incoming_connection *conn,
                                                 ConnectionStatus status,
                                                 void *usr_data);

    typedef void(*inco_connection_on_disconnect_handler)(incoming_connection *conn,
                                                         ConnectivityEventResult con_evt_res,
                                                         ConnectivityEventType c_evt_type,
                                                         void *usr_data);

    typedef RetCode(*inco_connection_on_incoming_transaction_handler)(incoming_connection *conn,
                                                                      shr_incoming_transaction *itx,
                                                                      void *usr_data);

    typedef RetCode(*inco_connection_on_incoming_subscription_handler)(incoming_connection *conn,
                                                                       shr_incoming_subscription *isbs,
                                                                       void *usr_data);

    //incoming tx

    typedef void(*inco_transaction_status_change)(incoming_transaction *tx,
                                                  TransactionStatus status,
                                                  void *usr_data);

    typedef void(*inco_transaction_request)(incoming_transaction *tx,
                                            void *usr_data);

    typedef void(*inco_transaction_closure)(incoming_transaction *tx,
                                            void *usr_data);

    //incoming sbs

    typedef void(*inco_subscription_status_change)(incoming_subscription *isbs,
                                                   SubscriptionStatus status,
                                                   void *usr_data);

    typedef RetCode(*inco_subscription_accept_distribution)(incoming_subscription *isbs,
                                                            const subscription_event *sbs_evt,
                                                            void *usr_data);

    typedef void(*inco_subscription_on_stop)(incoming_subscription *isbs,
                                             void *usr_data);
}

//c_inco_tx

struct c_inco_tx : public incoming_transaction {
    c_inco_tx(std::shared_ptr<incoming_connection> &c);

    inco_transaction_request tr_wr_;
    inco_transaction_closure tc_wr_;
    inco_transaction_status_change tsc_wr_;
    void *tr_ud_;
    void *tc_ud_;
    void *tsc_ud_;
};

struct c_inco_tx_listener : public incoming_transaction_listener {
    virtual void on_status_change(incoming_transaction &it, TransactionStatus status) override {
        ((c_inco_tx &)it).tsc_wr_(&it, status, ((c_inco_tx &)it).tsc_ud_);
    };
    virtual void on_request(incoming_transaction &it) override {
        ((c_inco_tx &)it).tr_wr_(&it, ((c_inco_tx &)it).tr_ud_);
    };
    virtual void on_close(incoming_transaction &it) override {
        ((c_inco_tx &)it).tc_wr_(&it, ((c_inco_tx &)it).tc_ud_);
    };
};

static c_inco_tx_listener citl;

c_inco_tx::c_inco_tx(std::shared_ptr<incoming_connection> &c) :
    incoming_transaction(c, citl),
    tr_wr_(nullptr),
    tc_wr_(nullptr),
    tsc_wr_(nullptr),
    tr_ud_(nullptr),
    tc_ud_(nullptr),
    tsc_ud_(nullptr) {}

//c_inco_tx_factory

struct c_inco_tx_factory : public incoming_transaction_factory {
    virtual c_inco_tx &make_incoming_transaction(std::shared_ptr<incoming_connection> &ic) override {
        return *new c_inco_tx(ic);
    }
};

extern "C" {
    void inco_transaction_release(shr_incoming_transaction *tx)
    {
        delete(std::shared_ptr<incoming_connection> *)tx;
    }

    incoming_transaction *inco_transaction_get_ptr(shr_incoming_transaction *tx)
    {
        return ((std::shared_ptr<incoming_transaction> *)tx)->get();
    }

    void inco_transaction_set_transaction_status_change_handler(incoming_transaction *tx,
                                                                inco_transaction_status_change handler,
                                                                void *usr_data)
    {
        static_cast<c_inco_tx *>(tx)->tsc_wr_ = handler;
        static_cast<c_inco_tx *>(tx)->tsc_ud_ = usr_data;
    }

    void inco_transaction_set_transaction_closure_handler(incoming_transaction *tx,
                                                          inco_transaction_closure handler,
                                                          void *usr_data)
    {
        static_cast<c_inco_tx *>(tx)->tc_wr_ = handler;
        static_cast<c_inco_tx *>(tx)->tc_ud_ = usr_data;
    }

    void inco_transaction_set_inco_transaction_request_handler(incoming_transaction *tx,
                                                               inco_transaction_request handler,
                                                               void *usr_data)
    {
        static_cast<c_inco_tx *>(tx)->tr_wr_ = handler;
        static_cast<c_inco_tx *>(tx)->tr_ud_ = usr_data;
    }
}

static c_inco_tx_factory citf;

//c_inco_sbs

struct c_inco_sbs : public incoming_subscription {
    c_inco_sbs(std::shared_ptr<incoming_connection> &c);

    inco_subscription_accept_distribution isad_wr_;
    inco_subscription_status_change issc_wr_;
    inco_subscription_on_stop isos_;
    void *issc_ud_;
    void *isad_ud_;
    void *isos_ud_;
};

struct c_inco_sbs_listener : public incoming_subscription_listener {
    virtual void on_status_change(incoming_subscription &is, SubscriptionStatus status) override {
        ((c_inco_sbs &)is).issc_wr_(&is, status, ((c_inco_sbs &)is).issc_ud_);
    }
    virtual void on_stop(incoming_subscription &is) override {
        ((c_inco_sbs &)is).isos_(&is, ((c_inco_sbs &)is).isos_ud_);
    }
    virtual RetCode on_accept_event(incoming_subscription &is, const subscription_event &se) override {
        return ((c_inco_sbs &)is).isad_wr_(&is, &se, ((c_inco_sbs &)is).isad_ud_);
    }
};

static c_inco_sbs_listener cisl;

c_inco_sbs::c_inco_sbs(std::shared_ptr<incoming_connection> &c) :
    incoming_subscription(c, cisl),
    isad_wr_(nullptr),
    issc_wr_(nullptr),
    isos_(nullptr),
    issc_ud_(nullptr),
    isad_ud_(nullptr),
    isos_ud_(nullptr) {}

//c_inco_sbs_factory

struct c_inco_sbs_factory : public incoming_subscription_factory {
    virtual c_inco_sbs &make_incoming_subscription(std::shared_ptr<incoming_connection> &ic) override {
        return *new c_inco_sbs(ic);
    }
};

static c_inco_sbs_factory cisf;

extern "C" {
    void inco_subscription_release(shr_incoming_subscription *sbs)
    {
        delete(std::shared_ptr<shr_incoming_subscription> *)sbs;
    }

    incoming_subscription *inco_subscription_get_ptr(shr_incoming_subscription *sbs)
    {
        return ((std::shared_ptr<incoming_subscription> *)sbs)->get();
    }

    void inco_subscription_set_status_change_handler(incoming_subscription *subscription,
                                                     inco_subscription_status_change handler,
                                                     void *usr_data)
    {
        static_cast<c_inco_sbs *>(subscription)->issc_wr_ = handler;
        static_cast<c_inco_sbs *>(subscription)->issc_ud_ = usr_data;
    }

    void inco_subscription_set_accept_distribution_handler(incoming_subscription *subscription,
                                                           inco_subscription_accept_distribution handler,
                                                           void *usr_data)
    {
        static_cast<c_inco_sbs *>(subscription)->isad_wr_ = handler;
        static_cast<c_inco_sbs *>(subscription)->isad_ud_ = usr_data;
    }

    void inco_subscription_set_on_stop_handler(incoming_subscription *sbs,
                                               inco_subscription_on_stop handler,
                                               void *usr_data)
    {
        static_cast<c_inco_sbs *>(sbs)->isos_ = handler;
        static_cast<c_inco_sbs *>(sbs)->isos_ud_ = usr_data;
    }
}

//c_inco_conn

struct c_inco_conn : public incoming_connection {
    c_inco_conn(peer &p);

    inco_connection_status_change icsc_;
    inco_connection_on_disconnect_handler icodh_;
    inco_connection_on_incoming_transaction_handler icoith_;
    inco_connection_on_incoming_subscription_handler icoish_;

    void *icsc_ud_;
    void *icodh_ud_;
    void *icoith_ud_;
    void *icoish_ud_;
};

struct c_inco_conn_listener : public incoming_connection_listener {
    virtual void on_status_change(incoming_connection &ic,
                                  ConnectionStatus current) override {
        ((c_inco_conn &)ic).icsc_(&ic, current, ((c_inco_conn &)ic).icsc_ud_);
    }

    virtual void on_disconnect(incoming_connection &ic,
                               ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type) override {
        ((c_inco_conn &)ic).icodh_(&ic, con_evt_res, c_evt_type, ((c_inco_conn &)ic).icodh_ud_);
    }

    virtual RetCode on_incoming_transaction(incoming_connection &ic,
                                            std::shared_ptr<incoming_transaction> &it) override {
        return ((c_inco_conn &)ic).icoith_(&ic, (shr_incoming_transaction *)new std::shared_ptr<incoming_transaction>(it), ((c_inco_conn &)ic).icoith_ud_);
    }

    virtual RetCode on_incoming_subscription(incoming_connection &ic,
                                             std::shared_ptr<incoming_subscription> &is) override {
        return ((c_inco_conn &)ic).icoish_(&ic, (shr_incoming_subscription *)new std::shared_ptr<incoming_subscription>(is), ((c_inco_conn &)ic).icoish_ud_);
    }
};

static c_inco_conn_listener cicl;

c_inco_conn::c_inco_conn(peer &p) : incoming_connection(p, cicl),
    icsc_(nullptr),
    icodh_(nullptr),
    icoith_(nullptr),
    icoish_(nullptr),
    icsc_ud_(nullptr),
    icodh_ud_(nullptr),
    icoith_ud_(nullptr),
    icoish_ud_(nullptr)
{
    set_incoming_transaction_factory(citf);
    set_incoming_subscription_factory(cisf);
}

//c_inco_conn_factory

struct c_inco_conn_factory : public incoming_connection_factory {
    virtual incoming_connection &make_incoming_connection(peer &p) override {
        return *new c_inco_conn(p);
    }
};

static c_inco_conn_factory cicf;

extern "C" {
    void inco_connection_release(shr_incoming_connection *ic)
    {
        delete(std::shared_ptr<incoming_connection> *)ic;
    }

    void inco_connection_set_status_change_handler(incoming_connection *ic,
                                                   inco_connection_status_change hndl,
                                                   void *usr_data)
    {
        static_cast<c_inco_conn *>(ic)->icsc_ = hndl;
        static_cast<c_inco_conn *>(ic)->icsc_ud_ = usr_data;
    }

    void inco_connection_set_on_disconnect_handler(incoming_connection *ic,
                                                   inco_connection_on_disconnect_handler hndl,
                                                   void *usr_data)
    {
        static_cast<c_inco_conn *>(ic)->icodh_ = hndl;
        static_cast<c_inco_conn *>(ic)->icodh_ud_ = usr_data;
    }

    void inco_connection_set_on_incoming_transaction_handler(incoming_connection *ic,
                                                             inco_connection_on_incoming_transaction_handler hndl,
                                                             void *usr_data)
    {
        static_cast<c_inco_conn *>(ic)->icoith_ = hndl;
        static_cast<c_inco_conn *>(ic)->icoith_ud_ = usr_data;
    }

    void inco_connection_set_on_incoming_subscription_handler(incoming_connection *ic,
                                                              inco_connection_on_incoming_subscription_handler hndl,
                                                              void *usr_data)
    {
        static_cast<c_inco_conn *>(ic)->icoish_ = hndl;
        static_cast<c_inco_conn *>(ic)->icoish_ud_ = usr_data;
    }
}

//c_peer

struct c_peer : public peer {
    c_peer();

    virtual const char *get_name() override {
        if(pnh_wr_) {
            return pnh_wr_(this, pnh_wr_ud_, pnh_wr_ud2_);
        }
        return nullptr;
    }

    virtual const unsigned int *get_version() override {
        if(pvh_wr_) {
            return pvh_wr_(this, pvh_wr_ud_, pvh_wr_ud2_);
        }
        return nullptr;
    }

    peer_name_handler pnh_wr_;
    peer_version_handler pvh_wr_;
    peer_load_config_handler plch_wr_;
    peer_init_handler pih_wr_;
    peer_starting_handler pstarth_wr_;
    peer_stopping_handler pstoph_wr_;
    peer_on_move_running_handler ptoah_wr_;
    peer_dying_breath_handler pdbh_wr_;
    peer_status_change psc_wr_;
    peer_on_incoming_connection_handler sic_wr_;

    void *psc_ud_;
    void *pnh_wr_ud_;
    void *pvh_wr_ud_;
    void *plch_wr_ud_;
    void *pih_wr_ud_;
    void *pstarth_wr_ud_;
    void *pstoph_wr_ud_;
    void *ptoah_wr_ud_;
    void *pdbh_wr_ud_;
    void *sic_wr_ud_;
    
    void *psc_ud2_;
    void *pnh_wr_ud2_;
    void *pvh_wr_ud2_;
    void *plch_wr_ud2_;
    void *pih_wr_ud2_;
    void *pstarth_wr_ud2_;
    void *pstoph_wr_ud2_;
    void *ptoah_wr_ud2_;
    void *pdbh_wr_ud2_;
    void *sic_wr_ud2_;
};

struct c_peer_listener : public peer_listener {
    virtual RetCode on_load_config(peer &p,
                                   int pnum,
                                   const char *param,
                                   const char *value) override {
        if(((c_peer &)p).plch_wr_) {
            return ((c_peer &)p).plch_wr_(&p, pnum, param, value, ((c_peer &)p).plch_wr_ud_, ((c_peer &)p).plch_wr_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_init(peer &p) override {
        if(((c_peer &)p).pih_wr_) {
            return ((c_peer &)p).pih_wr_(&p, ((c_peer &)p).pih_wr_ud_, ((c_peer &)p).pih_wr_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_starting(peer &p) override {
        if(((c_peer &)p).pstarth_wr_) {
            return ((c_peer &)p).pstarth_wr_(&p, ((c_peer &)p).pstarth_wr_ud_, ((c_peer &)p).pstarth_wr_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_stopping(peer &p) override {
        if(((c_peer &)p).pstoph_wr_) {
            return ((c_peer &)p).pstoph_wr_(&p, ((c_peer &)p).pstoph_wr_ud_, ((c_peer &)p).pstoph_wr_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_move_running(peer &p) override {
        if(((c_peer &)p).ptoah_wr_) {
            return ((c_peer &)p).ptoah_wr_(&p, ((c_peer &)p).ptoah_wr_ud_, ((c_peer &)p).ptoah_wr_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual void on_dying_breath(peer &p) override {
        if(((c_peer &)p).pdbh_wr_) {
            ((c_peer &)p).pdbh_wr_(&p, ((c_peer &)p).pdbh_wr_ud_, ((c_peer &)p).pdbh_wr_ud2_);
        }
    }

    virtual void on_status_change(peer &p, PeerStatus status) override {
        if(((c_peer &)p).psc_wr_){
            ((c_peer &)p).psc_wr_(&p, status, ((c_peer &)p).psc_ud_, ((c_peer &)p).psc_ud2_);
        }
    }

    virtual RetCode on_incoming_connection(peer &p, std::shared_ptr<incoming_connection> &ic) override {
        if(((c_peer &)p).sic_wr_) {
            return ((c_peer &)p).sic_wr_(&p,
                                         (shr_incoming_connection *) new std::shared_ptr<incoming_connection>(ic),
                                         ((c_peer &)p).sic_wr_ud_,
                                         ((c_peer &)p).sic_wr_ud2_);
        }
        return RetCode_OK;
    }
};

static c_peer_listener cpl;

c_peer::c_peer() :
    peer(cpl),
    pnh_wr_(nullptr),
    pvh_wr_(nullptr),
    plch_wr_(nullptr),
    pih_wr_(nullptr),
    pstarth_wr_(nullptr),
    pstoph_wr_(nullptr),
    ptoah_wr_(nullptr),
    pdbh_wr_(nullptr),
    psc_wr_(nullptr),
    sic_wr_(nullptr),
    psc_ud_(nullptr),
    pnh_wr_ud_(nullptr),
    pvh_wr_ud_(nullptr),
    plch_wr_ud_(nullptr),
    pih_wr_ud_(nullptr),
    pstarth_wr_ud_(nullptr),
    pstoph_wr_ud_(nullptr),
    ptoah_wr_ud_(nullptr),
    pdbh_wr_ud_(nullptr),
    sic_wr_ud_(nullptr),
    psc_ud2_(nullptr),
    pnh_wr_ud2_(nullptr),
    pvh_wr_ud2_(nullptr),
    plch_wr_ud2_(nullptr),
    pih_wr_ud2_(nullptr),
    pstarth_wr_ud2_(nullptr),
    pstoph_wr_ud2_(nullptr),
    ptoah_wr_ud2_(nullptr),
    pdbh_wr_ud2_(nullptr),
    sic_wr_ud2_(nullptr)
{
    set_incoming_connection_factory(cicf);
}

extern "C" {
    own_peer *peer_create()
    {
        return (own_peer *) new c_peer();
    }

    void peer_destroy(own_peer *p)
    {
        delete(c_peer *)p;
    }

    peer *own_peer_get_ptr(own_peer *p)
    {
        return (c_peer *)p;
    }

    RetCode peer_set_params_file_dir(peer *p, const char *dir)
    {
        return p->set_params_file_dir(dir);
    }

    RetCode peer_set_params_file_path_name(peer *p, const char *file_path)
    {
        return p->set_params_file_path_name(file_path);
    }

    const char *peer_get_name(peer *p)
    {
        return p->get_name();
    }

    const unsigned int *peer_get_version(peer *p)
    {
        return p->get_version();
    }

    unsigned int peer_get_version_major(peer *p)
    {
        return p->get_version_major();
    }

    unsigned int peer_get_version_minor(peer *p)
    {
        return p->get_version_minor();
    }

    unsigned int peer_get_version_maintenance(peer *p)
    {
        return p->get_version_maintenance();
    }

    unsigned int peer_get_version_architecture(peer *p)
    {
        return p->get_version_architecture();
    }

    int peer_is_configured(peer *p)
    {
        return p->is_configured();
    }

    const nentity_manager *peer_get_nentity_manager(peer *p)
    {
        return &p->get_nentity_manager();
    }

    int peer_is_persistent(peer *p)
    {
        return p->is_persistent() ? 1 : 0;
    }

    int peer_is_persistent_schema_creating(peer *p)
    {
        return p->is_create_persistent_schema() ? 1 : 0;
    }

    int peer_is_dropping_existing_schema(peer *p)
    {
        return p->is_drop_existing_persistent_schema() ? 1 : 0;
    }

    PeerPersonality peer_get_personality(peer *p)
    {
        return p->get_personality();
    }

    sockaddr_in peer_get_server_sockaddr(peer *p)
    {
        return p->get_server_sockaddr();
    }

    unsigned int peer_server_executor_count(peer *p)
    {
        return p->get_server_transaction_service_executor_size();
    }

    unsigned int peer_client_executor_count(peer *p)
    {
        return p->get_client_transaction_service_executor_size();
    }

    unsigned int peer_server_sbs_executor_count(peer *p)
    {
        return p->get_server_subscription_service_executor_size();
    }

    void peer_set_personality(peer *p, PeerPersonality personality)
    {
        p->set_personality(personality);
    }

    void peer_add_load_model(peer *p, const char *model)
    {
        p->add_load_model(model);
    }

    void peer_set_srv_sin_addr(peer *p, const char *address)
    {
        p->set_server_address(address);
    }

    void peer_set_sin_port(peer *p, int port)
    {
        p->set_server_port(port);
    }

    void peer_set_srv_executors(peer *p, unsigned int server_executors)
    {
        p->set_server_transaction_service_executor_size(server_executors);
    }

    void peer_set_cli_executors(peer *p, unsigned int client_executors)
    {
        p->set_client_transaction_service_executor_size(client_executors);
    }

    void peer_set_srv_sbs_executors(peer *p, unsigned int srv_sbs_executors)
    {
        p->set_server_subscription_service_executor_size(srv_sbs_executors);
    }

    void peer_set_persistent(peer *p, int persistent)
    {
        p->set_persistent(persistent ? true : false);
    }

    void peer_set_persistent_schema_creating(peer *p, int persistent_schema_create)
    {
        p->set_create_persistent_schema(persistent_schema_create ? true : false);
    }

    void peer_set_dropping_existing_schema(peer *p, int drop_existing_schema)
    {
        p->set_drop_existing_persistent_schema(drop_existing_schema ? true : false);
    }

    void peer_add_load_persistent_driver(peer *p, const char *driver)
    {
        p->add_load_persistent_driver(driver);
    }

    RetCode peer_extend_model_with_nem(peer *p, nentity_manager *nem)
    {
        return p->extend_model(*static_cast<nentity_manager *>(nem));
    }

    RetCode peer_extend_model_with_model_name(peer *p, const char *model_name)
    {
        return p->extend_model(model_name);
    }

    void peer_set_name_handler(peer *p, peer_name_handler hndl, void *usr_data, void *usr_data2)
    {
        static_cast<c_peer *>(p)->pnh_wr_ = hndl;
        static_cast<c_peer *>(p)->pnh_wr_ud_ = usr_data;
        static_cast<c_peer *>(p)->pnh_wr_ud2_ = usr_data2;
    }

    void peer_set_version_handler(peer *p, peer_version_handler hndl, void *usr_data, void *usr_data2)
    {
        static_cast<c_peer *>(p)->pvh_wr_ = hndl;
        static_cast<c_peer *>(p)->pvh_wr_ud_ = usr_data;
        static_cast<c_peer *>(p)->pvh_wr_ud2_ = usr_data2;
    }

    void peer_set_load_config_handler(peer *p, peer_load_config_handler hndl, void *usr_data, void *usr_data2)
    {
        static_cast<c_peer *>(p)->plch_wr_ = hndl;
        static_cast<c_peer *>(p)->plch_wr_ud_ = usr_data;
        static_cast<c_peer *>(p)->plch_wr_ud2_ = usr_data2;
    }

    void peer_set_init_handler(peer *p, peer_init_handler hndl, void *usr_data, void *usr_data2)
    {
        static_cast<c_peer *>(p)->pih_wr_ = hndl;
        static_cast<c_peer *>(p)->pih_wr_ud_ = usr_data;
        static_cast<c_peer *>(p)->pih_wr_ud2_ = usr_data2;
    }

    void peer_set_starting_handler(peer *p, peer_starting_handler hndl, void *usr_data, void *usr_data2)
    {
        static_cast<c_peer *>(p)->pstarth_wr_ = hndl;
        static_cast<c_peer *>(p)->pstarth_wr_ud_ = usr_data;
        static_cast<c_peer *>(p)->pstarth_wr_ud2_ = usr_data2;
    }

    void peer_set_stopping_handler(peer *p, peer_stopping_handler hndl, void *usr_data, void *usr_data2)
    {
        static_cast<c_peer *>(p)->pstoph_wr_ = hndl;
        static_cast<c_peer *>(p)->pstoph_wr_ud_ = usr_data;
        static_cast<c_peer *>(p)->pstoph_wr_ud2_ = usr_data2;
    }

    void peer_set_on_move_running_handler(peer *p, peer_on_move_running_handler hndl, void *usr_data, void *usr_data2)
    {
        static_cast<c_peer *>(p)->ptoah_wr_ = hndl;
        static_cast<c_peer *>(p)->ptoah_wr_ud_ = usr_data;
        static_cast<c_peer *>(p)->ptoah_wr_ud2_ = usr_data2;
    }

    void peer_set_dying_breath_handler(peer *p, peer_dying_breath_handler hndl, void *usr_data, void *usr_data2)
    {
        static_cast<c_peer *>(p)->pdbh_wr_ = hndl;
        static_cast<c_peer *>(p)->pdbh_wr_ud_ = usr_data;
        static_cast<c_peer *>(p)->pdbh_wr_ud2_ = usr_data2;
    }

    void peer_set_configured(peer *p, int configured)
    {
        p->set_configured(configured ? true : false);
    }

    PeerStatus peer_get_status(peer *p)
    {
        return p->get_status();
    }

    void peer_set_status_change_handler(peer *p, peer_status_change handler, void *usr_data, void *usr_data2)
    {
        static_cast<c_peer *>(p)->psc_wr_ = handler;
        static_cast<c_peer *>(p)->psc_ud_ = usr_data;
        static_cast<c_peer *>(p)->psc_ud2_ = usr_data2;
    }

    void peer_set_peer_on_incoming_connection_handler(peer *p,
                                                      peer_on_incoming_connection_handler handler,
                                                      void *usr_data,
                                                      void *usr_data2)
    {
        static_cast<c_peer *>(p)->sic_wr_ = handler;
        static_cast<c_peer *>(p)->sic_wr_ud_ = usr_data;
        static_cast<c_peer *>(p)->sic_wr_ud2_ = usr_data2;
    }

    RetCode peer_await_for_status_reached(peer *p,
                                          PeerStatus test,
                                          PeerStatus *current,
                                          time_t sec,
                                          long nsec)
    {
        return p->await_for_status_reached(test, *current, sec, nsec);
    }

    RetCode peer_await_for_status_change(peer *p,
                                         PeerStatus *peer_status,
                                         time_t sec,
                                         long nsec)
    {
        return p->await_for_status_change(*peer_status, sec, nsec);
    }

    RetCode peer_start(peer *p, int argc, char *argv[], int spawn_new_thread)
    {
        return p->start(argc, argv, spawn_new_thread ? true : false);
    }

    RetCode peer_stop(peer *p, int force_disconnect)
    {
        return p->stop(force_disconnect ? true : false);
    }

    RetCode peer_persistence_schema_create(peer *p, PersistenceAlteringMode mode)
    {
        return p->create_persistent_schema(mode);
    }

    RetCode peer_nclass_persistence_schema_create(peer *p, PersistenceAlteringMode mode, unsigned int nclass_id)
    {
        return p->nclass_create_persistent_schema(mode, nclass_id);
    }

    RetCode peer_nclass_persistent_load(peer *p,
                                        unsigned short nclass_key,
                                        unsigned int *ts_0_out,
                                        unsigned int *ts_1_out,
                                        nclass *in_out)
    {
        return p->obj_load(nclass_key,
                           *ts_0_out,
                           *ts_1_out,
                           *in_out);
    }

    RetCode peer_nclass_persistent_save(peer *p, nclass *in)
    {
        return p->obj_save(*in);
    }

    RetCode peer_nclass_persistent_update(peer *p, unsigned short nclass_key, nclass *in)
    {
        return p->obj_update(nclass_key, *in);
    }

    RetCode peer_nclass_persistent_update_or_save(peer *p,
                                                  unsigned short nclass_key,
                                                  nclass *in)
    {
        return p->obj_update_or_save(nclass_key, *in);
    }

    RetCode peer_nclass_persistent_remove(peer *p,
                                          unsigned short nclass_key,
                                          PersistenceDeletionMode mode,
                                          nclass *in)
    {
        return p->obj_remove(nclass_key, mode, *in);
    }

    RetCode peer_nclass_distribute(peer *p,
                                   SubscriptionEventType event_type,
                                   Action action,
                                   nclass *in)
    {
        return p->obj_distribute(event_type, action, *in);
    }

    RetCode peer_nclass_persistent_save_and_distribute(peer *p, nclass *in)
    {
        return p->obj_save_and_distribute(*in);
    }

    RetCode peer_nclass_persistent_update_and_distribute(peer *p, unsigned short nclass_key, nclass *in)
    {
        return p->obj_update_and_distribute(nclass_key, *in);
    }

    RetCode peer_nclass_persistent_update_or_save_and_distribute(peer *p, unsigned short nclass_key, nclass *in)
    {
        return p->obj_update_or_save_and_distribute(nclass_key, *in);
    }

    RetCode peer_nclass_persistent_remove_and_distribute(peer *p,
                                                         unsigned short nclass_key,
                                                         PersistenceDeletionMode mode,
                                                         nclass *in)
    {
        return p->obj_remove_and_distribute(nclass_key, mode, *in);
    }
}
