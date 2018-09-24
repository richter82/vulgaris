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

extern "C" {
    RetCode set_logger_cfg_file_dir(const char *dir)
    {
        return logger::set_logger_cfg_file_dir(dir);
    }

    RetCode set_logger_cfg_file_path_name(const char *file_path)
    {
        return logger::set_logger_cfg_file_path_name(file_path);
    }

    RetCode load_logger_cfg()
    {
        return logger::load_logger_config();
    }
}

extern "C" {
    typedef struct own_peer own_peer;
    typedef struct shr_incoming_connection shr_incoming_connection;
    typedef struct own_incoming_connection own_incoming_connection;
    typedef struct shr_incoming_transaction shr_incoming_transaction;
    typedef struct own_incoming_transaction own_incoming_transaction;
    typedef struct shr_incoming_subscription shr_incoming_subscription;
    typedef struct own_incoming_subscription own_incoming_subscription;

    //peer

    typedef void (*peer_on_status_change)(peer *p, PeerStatus status, void *ud, void *ud2);
    typedef const char *(*peer_name)(peer *p, void *ud, void *ud2);
    typedef const unsigned int *(*peer_version)(peer *p, void *ud, void *ud2);
    typedef RetCode(*peer_on_load_config)(peer *p, int pnum, const char *param, const char *value, void *ud, void *ud2);
    typedef RetCode(*peer_on_init)(peer *p, void *ud, void *ud2);
    typedef RetCode(*peer_on_starting)(peer *p, void *ud, void *ud2);
    typedef RetCode(*peer_on_stopping)(peer *p, void *ud, void *ud2);
    typedef RetCode(*peer_on_move_running)(peer *p, void *ud, void *ud2);
    typedef void(*peer_on_dying_breath)(peer *p, void *ud, void *ud2);
    typedef RetCode(*peer_on_incoming_connection)(peer *p, shr_incoming_connection *ic, void *ud, void *ud2);

    //incoming connection

    typedef void(*inco_connection_on_status_change)(incoming_connection *conn,
                                                    ConnectionStatus status,
                                                    void *ud,
                                                    void *ud2);

    typedef void(*inco_connection_on_disconnect)(incoming_connection *conn,
                                                 ConnectivityEventResult con_evt_res,
                                                 ConnectivityEventType c_evt_type,
                                                 void *ud,
                                                 void *ud2);

    typedef RetCode(*inco_connection_on_incoming_transaction)(incoming_connection *conn,
                                                              shr_incoming_transaction *itx,
                                                              void *ud,
                                                              void *ud2);

    typedef RetCode(*inco_connection_on_incoming_subscription)(incoming_connection *conn,
                                                               shr_incoming_subscription *isbs,
                                                               void *ud,
                                                               void *ud2);

    typedef void(*inco_connection_on_releaseable)(incoming_connection *conn,
                                                  void *ud,
                                                  void *ud2);


    //incoming tx

    typedef void(*inco_transaction_on_status_change)(incoming_transaction *tx,
                                                     TransactionStatus status,
                                                     void *ud,
                                                     void *ud2);

    typedef void(*inco_transaction_on_request)(incoming_transaction *tx,
                                               void *ud,
                                               void *ud2);

    typedef void(*inco_transaction_on_closure)(incoming_transaction *tx,
                                               void *ud,
                                               void *ud2);

    typedef void(*inco_transaction_on_releaseable)(incoming_transaction *tx,
                                                   void *ud,
                                                   void *ud2);


    //incoming sbs

    typedef void(*inco_subscription_on_status_change)(incoming_subscription *isbs,
                                                      SubscriptionStatus status,
                                                      void *ud,
                                                      void *ud2);

    typedef RetCode(*inco_subscription_on_accept_distribution)(incoming_subscription *isbs,
                                                               const subscription_event *sbs_evt,
                                                               void *ud,
                                                               void *ud2);

    typedef void(*inco_subscription_on_stop)(incoming_subscription *isbs,
                                             void *ud,
                                             void *ud2);

    typedef void(*inco_subscription_on_releaseable)(incoming_subscription *isbs,
                                                    void *ud,
                                                    void *ud2);

}

//c_inco_tx

struct c_inco_tx : public incoming_transaction {
    c_inco_tx(std::shared_ptr<incoming_connection> &c);

    inco_transaction_on_request tr_;
    inco_transaction_on_closure tc_;
    inco_transaction_on_status_change tsc_;
    inco_transaction_on_releaseable tod_;

    void *tr_ud_;
    void *tc_ud_;
    void *tsc_ud_;
    void *tod_ud_;
    void *tr_ud2_;
    void *tc_ud2_;
    void *tsc_ud2_;
    void *tod_ud2_;
};

struct c_inco_tx_listener : public incoming_transaction_listener {
    virtual void on_status_change(incoming_transaction &it, TransactionStatus status) override {
        if(((c_inco_tx &)it).tsc_) {
            ((c_inco_tx &)it).tsc_(&it, status, ((c_inco_tx &)it).tsc_ud_, ((c_inco_tx &)it).tsc_ud2_);
        }
    };
    virtual void on_request(incoming_transaction &it) override {
        if(((c_inco_tx &)it).tr_) {
            ((c_inco_tx &)it).tr_(&it, ((c_inco_tx &)it).tr_ud_, ((c_inco_tx &)it).tr_ud2_);
        }
    };
    virtual void on_close(incoming_transaction &it) override {
        if(((c_inco_tx &)it).tc_) {
            ((c_inco_tx &)it).tc_(&it, ((c_inco_tx &)it).tc_ud_, ((c_inco_tx &)it).tc_ud2_);
        }
    };
};

static c_inco_tx_listener citl;

c_inco_tx::c_inco_tx(std::shared_ptr<incoming_connection> &c) :
    incoming_transaction(c, citl),
    tr_(nullptr),
    tc_(nullptr),
    tsc_(nullptr),
    tod_(nullptr),
    tr_ud_(nullptr),
    tc_ud_(nullptr),
    tsc_ud_(nullptr),
    tod_ud_(nullptr),
    tr_ud2_(nullptr),
    tc_ud2_(nullptr),
    tsc_ud2_(nullptr),
    tod_ud2_(nullptr) {}

//c_inco_tx_factory

struct c_inco_tx_factory : public incoming_transaction_factory {
    virtual c_inco_tx &make_incoming_transaction(std::shared_ptr<incoming_connection> &ic) override {
        return *new c_inco_tx(ic);
    }
};

extern "C" {
    void inco_transaction_release(own_incoming_transaction *tx)
    {
        delete(std::shared_ptr<incoming_connection> *)tx;
    }

    own_incoming_transaction *inco_transaction_get_own_ptr(shr_incoming_transaction *tx)
    {
        return (own_incoming_transaction *)new std::shared_ptr<incoming_transaction>(*(std::shared_ptr<incoming_transaction> *)tx);
    }

    incoming_transaction *inco_transaction_get_ptr(own_incoming_transaction *tx)
    {
        return ((std::shared_ptr<incoming_transaction> *)tx)->get();
    }

    void inco_transaction_set_on_status_change(incoming_transaction *tx,
                                               inco_transaction_on_status_change hndl,
                                               void *ud,
                                               void *ud2)
    {
        static_cast<c_inco_tx *>(tx)->tsc_ = hndl;
        static_cast<c_inco_tx *>(tx)->tsc_ud_ = ud;
        static_cast<c_inco_tx *>(tx)->tsc_ud2_ = ud2;
    }

    void inco_transaction_set_on_closure(incoming_transaction *tx,
                                         inco_transaction_on_closure hndl,
                                         void *ud,
                                         void *ud2)
    {
        static_cast<c_inco_tx *>(tx)->tc_ = hndl;
        static_cast<c_inco_tx *>(tx)->tc_ud_ = ud;
        static_cast<c_inco_tx *>(tx)->tc_ud2_ = ud2;
    }

    void inco_transaction_set_on_request(incoming_transaction *tx,
                                         inco_transaction_on_request hndl,
                                         void *ud,
                                         void *ud2)
    {
        static_cast<c_inco_tx *>(tx)->tr_ = hndl;
        static_cast<c_inco_tx *>(tx)->tr_ud_ = ud;
        static_cast<c_inco_tx *>(tx)->tr_ud2_ = ud2;
    }

    void inco_transaction_set_on_releaseable(incoming_transaction *tx,
                                             inco_transaction_on_releaseable hndl,
                                             void *ud,
                                             void *ud2)
    {
        static_cast<c_inco_tx *>(tx)->tod_ = hndl;
        static_cast<c_inco_tx *>(tx)->tod_ud_ = ud;
        static_cast<c_inco_tx *>(tx)->tod_ud2_ = ud2;
    }
}

static c_inco_tx_factory citf;

//c_inco_sbs

struct c_inco_sbs : public incoming_subscription {
    c_inco_sbs(std::shared_ptr<incoming_connection> &c);

    inco_subscription_on_accept_distribution isad_;
    inco_subscription_on_status_change issc_;
    inco_subscription_on_stop isos_;
    inco_subscription_on_releaseable isod_;

    void *issc_ud_;
    void *isad_ud_;
    void *isos_ud_;
    void *isod_ud_;
    void *issc_ud2_;
    void *isad_ud2_;
    void *isos_ud2_;
    void *isod_ud2_;
};

struct c_inco_sbs_listener : public incoming_subscription_listener {
    virtual void on_status_change(incoming_subscription &is, SubscriptionStatus status) override {
        if(((c_inco_sbs &)is).issc_) {
            ((c_inco_sbs &)is).issc_(&is, status, ((c_inco_sbs &)is).issc_ud_, ((c_inco_sbs &)is).issc_ud2_);
        }
    }
    virtual void on_stop(incoming_subscription &is) override {
        if(((c_inco_sbs &)is).isos_) {
            ((c_inco_sbs &)is).isos_(&is, ((c_inco_sbs &)is).isos_ud_, ((c_inco_sbs &)is).isos_ud2_);
        }
    }
    virtual RetCode on_accept_event(incoming_subscription &is, const subscription_event &se) override {
        if(((c_inco_sbs &)is).isad_) {
            return ((c_inco_sbs &)is).isad_(&is, &se, ((c_inco_sbs &)is).isad_ud_, ((c_inco_sbs &)is).isad_ud2_);
        }
        return RetCode_OK;
    }
};

static c_inco_sbs_listener cisl;

c_inco_sbs::c_inco_sbs(std::shared_ptr<incoming_connection> &c) :
    incoming_subscription(c, cisl),
    isad_(nullptr),
    issc_(nullptr),
    isos_(nullptr),
    isod_(nullptr),
    issc_ud_(nullptr),
    isad_ud_(nullptr),
    isos_ud_(nullptr),
    isod_ud_(nullptr),
    issc_ud2_(nullptr),
    isad_ud2_(nullptr),
    isos_ud2_(nullptr),
    isod_ud2_(nullptr) {}

//c_inco_sbs_factory

struct c_inco_sbs_factory : public incoming_subscription_factory {
    virtual c_inco_sbs &make_incoming_subscription(std::shared_ptr<incoming_connection> &ic) override {
        return *new c_inco_sbs(ic);
    }
};

static c_inco_sbs_factory cisf;

extern "C" {
    void inco_subscription_release(own_incoming_subscription *sbs)
    {
        delete(std::shared_ptr<shr_incoming_subscription> *)sbs;
    }

    own_incoming_subscription *inco_subscription_get_own_ptr(shr_incoming_subscription *sbs)
    {
        return (own_incoming_subscription *)new std::shared_ptr<incoming_subscription>(*(std::shared_ptr<incoming_subscription> *)sbs);
    }

    incoming_subscription *inco_subscription_get_ptr(own_incoming_subscription *sbs)
    {
        return ((std::shared_ptr<incoming_subscription> *)sbs)->get();
    }

    void inco_subscription_set_on_status_change(incoming_subscription *subscription,
                                                inco_subscription_on_status_change hndl,
                                                void *ud,
                                                void *ud2)
    {
        static_cast<c_inco_sbs *>(subscription)->issc_ = hndl;
        static_cast<c_inco_sbs *>(subscription)->issc_ud_ = ud;
        static_cast<c_inco_sbs *>(subscription)->issc_ud2_ = ud2;
    }

    void inco_subscription_set_on_accept_distribution(incoming_subscription *subscription,
                                                      inco_subscription_on_accept_distribution hndl,
                                                      void *ud,
                                                      void *ud2)
    {
        static_cast<c_inco_sbs *>(subscription)->isad_ = hndl;
        static_cast<c_inco_sbs *>(subscription)->isad_ud_ = ud;
        static_cast<c_inco_sbs *>(subscription)->isad_ud2_ = ud2;
    }

    void inco_subscription_set_on_stop(incoming_subscription *sbs,
                                       inco_subscription_on_stop hndl,
                                       void *ud,
                                       void *ud2)
    {
        static_cast<c_inco_sbs *>(sbs)->isos_ = hndl;
        static_cast<c_inco_sbs *>(sbs)->isos_ud_ = ud;
        static_cast<c_inco_sbs *>(sbs)->isos_ud2_ = ud2;
    }

    void inco_subscription_set_on_releaseable(incoming_subscription *sbs,
                                              inco_subscription_on_releaseable hndl,
                                              void *ud,
                                              void *ud2)
    {
        static_cast<c_inco_sbs *>(sbs)->isod_ = hndl;
        static_cast<c_inco_sbs *>(sbs)->isod_ud_ = ud;
        static_cast<c_inco_sbs *>(sbs)->isod_ud2_ = ud2;
    }

}

//c_inco_conn

struct c_inco_conn : public incoming_connection {
    c_inco_conn(peer &p);

    inco_connection_on_status_change icsc_;
    inco_connection_on_disconnect icodh_;
    inco_connection_on_incoming_transaction icoith_;
    inco_connection_on_incoming_subscription icoish_;
    inco_connection_on_releaseable icod_;

    void *icsc_ud_;
    void *icodh_ud_;
    void *icoith_ud_;
    void *icoish_ud_;
    void *icod_ud_;
    void *icsc_ud2_;
    void *icodh_ud2_;
    void *icoith_ud2_;
    void *icoish_ud2_;
    void *icod_ud2_;
};

struct c_inco_conn_listener : public incoming_connection_listener {
    virtual void on_status_change(incoming_connection &ic,
                                  ConnectionStatus current) override {
        if(((c_inco_conn &)ic).icsc_) {
            ((c_inco_conn &)ic).icsc_(&ic, current, ((c_inco_conn &)ic).icsc_ud_, ((c_inco_conn &)ic).icsc_ud2_);
        }
    }

    virtual void on_disconnect(incoming_connection &ic,
                               ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type) override {
        if(((c_inco_conn &)ic).icodh_) {
            ((c_inco_conn &)ic).icodh_(&ic, con_evt_res, c_evt_type, ((c_inco_conn &)ic).icodh_ud_, ((c_inco_conn &)ic).icodh_ud2_);
        }
    }

    virtual void on_releaseable(vlg::incoming_connection &ic) override {
        if(((c_inco_conn &)ic).icod_) {
            ((c_inco_conn &)ic).icod_(&ic, ((c_inco_conn &)ic).icod_ud_, ((c_inco_conn &)ic).icod_ud2_);
        }
    }

    virtual RetCode on_incoming_transaction(incoming_connection &ic,
                                            std::shared_ptr<incoming_transaction> &it) override {
        if(((c_inco_conn &)ic).icoith_) {
            return ((c_inco_conn &)ic).icoith_(&ic,
                                               (shr_incoming_transaction *)&it,
                                               ((c_inco_conn &)ic).icoith_ud_,
                                               ((c_inco_conn &)ic).icoith_ud2_);
        }
        return RetCode_OK;
    }

    virtual RetCode on_incoming_subscription(incoming_connection &ic,
                                             std::shared_ptr<incoming_subscription> &is) override {
        if(((c_inco_conn &)ic).icoish_) {
            return ((c_inco_conn &)ic).icoish_(&ic,
                                               (shr_incoming_subscription *)&is,
                                               ((c_inco_conn &)ic).icoish_ud_,
                                               ((c_inco_conn &)ic).icoish_ud2_);
        }
        return RetCode_OK;
    }
};

static c_inco_conn_listener cicl;

c_inco_conn::c_inco_conn(peer &p) : incoming_connection(p, cicl),
    icsc_(nullptr),
    icodh_(nullptr),
    icoith_(nullptr),
    icoish_(nullptr),
    icod_(nullptr),
    icsc_ud_(nullptr),
    icodh_ud_(nullptr),
    icoith_ud_(nullptr),
    icoish_ud_(nullptr),
    icod_ud_(nullptr),
    icsc_ud2_(nullptr),
    icodh_ud2_(nullptr),
    icoith_ud2_(nullptr),
    icoish_ud2_(nullptr),
    icod_ud2_(nullptr)
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
    void inco_connection_release(own_incoming_connection *ic)
    {
        delete(std::shared_ptr<incoming_connection> *)ic;
    }

    own_incoming_connection *inco_connection_get_own_ptr(shr_incoming_connection *ic)
    {
        return (own_incoming_connection *)new std::shared_ptr<incoming_connection>(*(std::shared_ptr<incoming_connection> *)ic);
    }

    incoming_connection *inco_connection_get_ptr(own_incoming_connection *ic)
    {
        return ((std::shared_ptr<incoming_connection> *)ic)->get();
    }

    void inco_connection_set_on_status_change(incoming_connection *ic,
                                              inco_connection_on_status_change hndl,
                                              void *ud,
                                              void *ud2)
    {
        static_cast<c_inco_conn *>(ic)->icsc_ = hndl;
        static_cast<c_inco_conn *>(ic)->icsc_ud_ = ud;
        static_cast<c_inco_conn *>(ic)->icsc_ud2_ = ud2;
    }

    void inco_connection_set_on_disconnect(incoming_connection *ic,
                                           inco_connection_on_disconnect hndl,
                                           void *ud,
                                           void *ud2)
    {
        static_cast<c_inco_conn *>(ic)->icodh_ = hndl;
        static_cast<c_inco_conn *>(ic)->icodh_ud_ = ud;
        static_cast<c_inco_conn *>(ic)->icodh_ud2_ = ud2;
    }

    void inco_connection_set_on_incoming_transaction(incoming_connection *ic,
                                                     inco_connection_on_incoming_transaction hndl,
                                                     void *ud,
                                                     void *ud2)
    {
        static_cast<c_inco_conn *>(ic)->icoith_ = hndl;
        static_cast<c_inco_conn *>(ic)->icoith_ud_ = ud;
        static_cast<c_inco_conn *>(ic)->icoith_ud2_ = ud2;
    }

    void inco_connection_set_on_incoming_subscription(incoming_connection *ic,
                                                      inco_connection_on_incoming_subscription hndl,
                                                      void *ud,
                                                      void *ud2)
    {
        static_cast<c_inco_conn *>(ic)->icoish_ = hndl;
        static_cast<c_inco_conn *>(ic)->icoish_ud_ = ud;
        static_cast<c_inco_conn *>(ic)->icoish_ud2_ = ud2;
    }

    void inco_connection_set_on_destroy(incoming_connection *ic,
                                        inco_connection_on_releaseable hndl,
                                        void *ud,
                                        void *ud2)
    {
        static_cast<c_inco_conn *>(ic)->icod_ = hndl;
        static_cast<c_inco_conn *>(ic)->icod_ud_ = ud;
        static_cast<c_inco_conn *>(ic)->icod_ud2_ = ud2;
    }

}

//c_peer

struct c_peer : public peer {
    c_peer();

    virtual const char *get_name() override {
        if(pnh_) {
            return pnh_(this, pnh_ud_, pnh_ud2_);
        }
        return nullptr;
    }

    virtual const unsigned int *get_version() override {
        if(pvh_) {
            return pvh_(this, pvh_ud_, pvh_ud2_);
        }
        return nullptr;
    }

    peer_name pnh_;
    peer_version pvh_;
    peer_on_load_config plch_;
    peer_on_init pih_;
    peer_on_starting pstarth_;
    peer_on_stopping pstoph_;
    peer_on_move_running ptoah_;
    peer_on_dying_breath pdbh_;
    peer_on_status_change psc_;
    peer_on_incoming_connection sic_;

    void *psc_ud_;
    void *pnh_ud_;
    void *pvh_ud_;
    void *plch_ud_;
    void *pih_ud_;
    void *pstarth_ud_;
    void *pstoph_ud_;
    void *ptoah_ud_;
    void *pdbh_ud_;
    void *sic_ud_;
    void *psc_ud2_;
    void *pnh_ud2_;
    void *pvh_ud2_;
    void *plch_ud2_;
    void *pih_ud2_;
    void *pstarth_ud2_;
    void *pstoph_ud2_;
    void *ptoah_ud2_;
    void *pdbh_ud2_;
    void *sic_ud2_;
};

struct c_peer_listener : public peer_listener {
    virtual RetCode on_load_config(peer &p,
                                   int pnum,
                                   const char *param,
                                   const char *value) override {
        if(((c_peer &)p).plch_) {
            return ((c_peer &)p).plch_(&p, pnum, param, value, ((c_peer &)p).plch_ud_, ((c_peer &)p).plch_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_init(peer &p) override {
        if(((c_peer &)p).pih_) {
            return ((c_peer &)p).pih_(&p, ((c_peer &)p).pih_ud_, ((c_peer &)p).pih_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_starting(peer &p) override {
        if(((c_peer &)p).pstarth_) {
            return ((c_peer &)p).pstarth_(&p, ((c_peer &)p).pstarth_ud_, ((c_peer &)p).pstarth_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_stopping(peer &p) override {
        if(((c_peer &)p).pstoph_) {
            return ((c_peer &)p).pstoph_(&p, ((c_peer &)p).pstoph_ud_, ((c_peer &)p).pstoph_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_move_running(peer &p) override {
        if(((c_peer &)p).ptoah_) {
            return ((c_peer &)p).ptoah_(&p, ((c_peer &)p).ptoah_ud_, ((c_peer &)p).ptoah_ud2_);
        } else {
            return RetCode_OK;
        }
    }

    virtual void on_dying_breath(peer &p) override {
        if(((c_peer &)p).pdbh_) {
            ((c_peer &)p).pdbh_(&p, ((c_peer &)p).pdbh_ud_, ((c_peer &)p).pdbh_ud2_);
        }
    }

    virtual void on_status_change(peer &p, PeerStatus status) override {
        if(((c_peer &)p).psc_) {
            ((c_peer &)p).psc_(&p, status, ((c_peer &)p).psc_ud_, ((c_peer &)p).psc_ud2_);
        }
    }

    virtual RetCode on_incoming_connection(peer &p, std::shared_ptr<incoming_connection> &ic) override {
        if(((c_peer &)p).sic_) {
            return ((c_peer &)p).sic_(&p,
                                      (shr_incoming_connection *) &ic,
                                      ((c_peer &)p).sic_ud_,
                                      ((c_peer &)p).sic_ud2_);
        }
        return RetCode_OK;
    }
};

static c_peer_listener cpl;

c_peer::c_peer() :
    peer(cpl),
    pnh_(nullptr),
    pvh_(nullptr),
    plch_(nullptr),
    pih_(nullptr),
    pstarth_(nullptr),
    pstoph_(nullptr),
    ptoah_(nullptr),
    pdbh_(nullptr),
    psc_(nullptr),
    sic_(nullptr),
    psc_ud_(nullptr),
    pnh_ud_(nullptr),
    pvh_ud_(nullptr),
    plch_ud_(nullptr),
    pih_ud_(nullptr),
    pstarth_ud_(nullptr),
    pstoph_ud_(nullptr),
    ptoah_ud_(nullptr),
    pdbh_ud_(nullptr),
    sic_ud_(nullptr),
    psc_ud2_(nullptr),
    pnh_ud2_(nullptr),
    pvh_ud2_(nullptr),
    plch_ud2_(nullptr),
    pih_ud2_(nullptr),
    pstarth_ud2_(nullptr),
    pstoph_ud2_(nullptr),
    ptoah_ud2_(nullptr),
    pdbh_ud2_(nullptr),
    sic_ud2_(nullptr)
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

    logger *peer_get_logger(peer *p)
    {
        return p->get_logger();
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

    void peer_set_name(peer *p, peer_name hndl, void *ud, void *ud2)
    {
        static_cast<c_peer *>(p)->pnh_ = hndl;
        static_cast<c_peer *>(p)->pnh_ud_ = ud;
        static_cast<c_peer *>(p)->pnh_ud2_ = ud2;
    }

    void peer_set_version(peer *p, peer_version hndl, void *ud, void *ud2)
    {
        static_cast<c_peer *>(p)->pvh_ = hndl;
        static_cast<c_peer *>(p)->pvh_ud_ = ud;
        static_cast<c_peer *>(p)->pvh_ud2_ = ud2;
    }

    void peer_set_on_load_config(peer *p, peer_on_load_config hndl, void *ud, void *ud2)
    {
        static_cast<c_peer *>(p)->plch_ = hndl;
        static_cast<c_peer *>(p)->plch_ud_ = ud;
        static_cast<c_peer *>(p)->plch_ud2_ = ud2;
    }

    void peer_set_on_init(peer *p, peer_on_init hndl, void *ud, void *ud2)
    {
        static_cast<c_peer *>(p)->pih_ = hndl;
        static_cast<c_peer *>(p)->pih_ud_ = ud;
        static_cast<c_peer *>(p)->pih_ud2_ = ud2;
    }

    void peer_set_on_starting(peer *p, peer_on_starting hndl, void *ud, void *ud2)
    {
        static_cast<c_peer *>(p)->pstarth_ = hndl;
        static_cast<c_peer *>(p)->pstarth_ud_ = ud;
        static_cast<c_peer *>(p)->pstarth_ud2_ = ud2;
    }

    void peer_set_on_stopping(peer *p, peer_on_stopping hndl, void *ud, void *ud2)
    {
        static_cast<c_peer *>(p)->pstoph_ = hndl;
        static_cast<c_peer *>(p)->pstoph_ud_ = ud;
        static_cast<c_peer *>(p)->pstoph_ud2_ = ud2;
    }

    void peer_set_on_move_running(peer *p, peer_on_move_running hndl, void *ud, void *ud2)
    {
        static_cast<c_peer *>(p)->ptoah_ = hndl;
        static_cast<c_peer *>(p)->ptoah_ud_ = ud;
        static_cast<c_peer *>(p)->ptoah_ud2_ = ud2;
    }

    void peer_set_on_dying_breath(peer *p, peer_on_dying_breath hndl, void *ud, void *ud2)
    {
        static_cast<c_peer *>(p)->pdbh_ = hndl;
        static_cast<c_peer *>(p)->pdbh_ud_ = ud;
        static_cast<c_peer *>(p)->pdbh_ud2_ = ud2;
    }

    void peer_set_configured(peer *p, int configured)
    {
        p->set_configured(configured ? true : false);
    }

    PeerStatus peer_get_status(peer *p)
    {
        return p->get_status();
    }

    void peer_set_on_status_change(peer *p, peer_on_status_change hndl, void *ud, void *ud2)
    {
        static_cast<c_peer *>(p)->psc_ = hndl;
        static_cast<c_peer *>(p)->psc_ud_ = ud;
        static_cast<c_peer *>(p)->psc_ud2_ = ud2;
    }

    void peer_set_peer_on_incoming_connection(peer *p,
                                              peer_on_incoming_connection hndl,
                                              void *ud,
                                              void *ud2)
    {
        static_cast<c_peer *>(p)->sic_ = hndl;
        static_cast<c_peer *>(p)->sic_ud_ = ud;
        static_cast<c_peer *>(p)->sic_ud2_ = ud2;
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