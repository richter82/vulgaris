/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "glob.h"

using namespace vlg;

extern "C" {
    typedef struct own_broker own_broker;
    typedef struct shr_incoming_connection shr_incoming_connection;
    typedef struct own_incoming_connection own_incoming_connection;
    typedef struct shr_incoming_transaction shr_incoming_transaction;
    typedef struct own_incoming_transaction own_incoming_transaction;
    typedef struct shr_incoming_subscription shr_incoming_subscription;
    typedef struct own_incoming_subscription own_incoming_subscription;

    //broker

    typedef void (*broker_on_status_change)(broker *p, BrokerStatus status, void *ud, void *ud2);
    typedef const char *(*broker_name)(broker *p, void *ud, void *ud2);
    typedef const unsigned int *(*broker_version)(broker *p, void *ud, void *ud2);
    typedef RetCode(*broker_on_load_config)(broker *p, int pnum, const char *param, const char *value, void *ud, void *ud2);
    typedef RetCode(*broker_on_init)(broker *p, void *ud, void *ud2);
    typedef RetCode(*broker_on_starting)(broker *p, void *ud, void *ud2);
    typedef RetCode(*broker_on_stopping)(broker *p, void *ud, void *ud2);
    typedef RetCode(*broker_on_move_running)(broker *p, void *ud, void *ud2);
    typedef void(*broker_on_error)(broker *p, void *ud, void *ud2);
    typedef RetCode(*broker_on_incoming_connection)(broker *p, shr_incoming_connection *ic, void *ud, void *ud2);

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

#define IT_REQ_UD_IDX 0
#define IT_CLO_UD_IDX 1
#define IT_STC_UD_IDX 2
#define IT_REL_UD_IDX 3

struct c_inco_tx : public incoming_transaction {
    c_inco_tx(std::shared_ptr<incoming_connection> &c);

    inco_transaction_on_request tr_;
    inco_transaction_on_closure tc_;
    inco_transaction_on_status_change tsc_;
    inco_transaction_on_releaseable tod_;

    void *ud_[4];
    void *ud2_[4];
};

struct c_inco_tx_listener : public incoming_transaction_listener {
    virtual void on_status_change(incoming_transaction &it, TransactionStatus status) override {
        if(((c_inco_tx &)it).tsc_) {
            ((c_inco_tx &)it).tsc_(&it, status,
                                   ((c_inco_tx &)it).ud_[IT_STC_UD_IDX],
                                   ((c_inco_tx &)it).ud2_[IT_STC_UD_IDX]);
        }
    };
    virtual void on_request(incoming_transaction &it) override {
        if(((c_inco_tx &)it).tr_) {
            ((c_inco_tx &)it).tr_(&it,
                                  ((c_inco_tx &)it).ud_[IT_REQ_UD_IDX],
                                  ((c_inco_tx &)it).ud2_[IT_REQ_UD_IDX]);
        }
    };
    virtual void on_close(incoming_transaction &it) override {
        if(((c_inco_tx &)it).tc_) {
            ((c_inco_tx &)it).tc_(&it,
                                  ((c_inco_tx &)it).ud_[IT_CLO_UD_IDX],
                                  ((c_inco_tx &)it).ud2_[IT_CLO_UD_IDX]);
        }
    };
    virtual void on_releaseable(incoming_transaction &it) override {
        if(((c_inco_tx &)it).tod_) {
            ((c_inco_tx &)it).tod_(&it,
                                   ((c_inco_tx &)it).ud_[IT_REL_UD_IDX],
                                   ((c_inco_tx &)it).ud2_[IT_REL_UD_IDX]);
        }
    }
};

static c_inco_tx_listener citl;

c_inco_tx::c_inco_tx(std::shared_ptr<incoming_connection> &c) :
    incoming_transaction(c, citl),
    tr_(nullptr),
    tc_(nullptr),
    tsc_(nullptr),
    tod_(nullptr)
{
    memset(ud_, 0, sizeof(ud_));
    memset(ud2_, 0, sizeof(ud2_));
}

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
        return (own_incoming_transaction *)new std::shared_ptr<incoming_transaction>(*(std::shared_ptr<incoming_transaction> *)
                                                                                     tx);
    }

    incoming_transaction *inco_transaction_get_ptr(own_incoming_transaction *tx)
    {
        return ((std::shared_ptr<incoming_transaction> *)tx)->get();
    }

    void inco_transaction_set_on_status_change(incoming_transaction *tx,
                                               inco_transaction_on_status_change hndl,
                                               void *ud, void *ud2)
    {
        static_cast<c_inco_tx *>(tx)->tsc_ = hndl;
        static_cast<c_inco_tx *>(tx)->ud_[IT_STC_UD_IDX] = ud;
        static_cast<c_inco_tx *>(tx)->ud2_[IT_STC_UD_IDX] = ud2;
    }

    void inco_transaction_set_on_closure(incoming_transaction *tx,
                                         inco_transaction_on_closure hndl,
                                         void *ud, void *ud2)
    {
        static_cast<c_inco_tx *>(tx)->tc_ = hndl;
        static_cast<c_inco_tx *>(tx)->ud_[IT_CLO_UD_IDX] = ud;
        static_cast<c_inco_tx *>(tx)->ud2_[IT_CLO_UD_IDX] = ud2;
    }

    void inco_transaction_set_on_request(incoming_transaction *tx,
                                         inco_transaction_on_request hndl,
                                         void *ud, void *ud2)
    {
        static_cast<c_inco_tx *>(tx)->tr_ = hndl;
        static_cast<c_inco_tx *>(tx)->ud_[IT_REQ_UD_IDX] = ud;
        static_cast<c_inco_tx *>(tx)->ud2_[IT_REQ_UD_IDX] = ud2;
    }

    void inco_transaction_set_on_releaseable(incoming_transaction *tx,
                                             inco_transaction_on_releaseable hndl,
                                             void *ud, void *ud2)
    {
        static_cast<c_inco_tx *>(tx)->tod_ = hndl;
        static_cast<c_inco_tx *>(tx)->ud_[IT_REL_UD_IDX] = ud;
        static_cast<c_inco_tx *>(tx)->ud2_[IT_REL_UD_IDX] = ud2;
    }
}

static c_inco_tx_factory citf;

//c_inco_sbs

#define IS_DIS_UD_IDX 0
#define IS_STC_UD_IDX 1
#define IS_STO_UD_IDX 2
#define IS_REL_UD_IDX 3

struct c_inco_sbs : public incoming_subscription {
    c_inco_sbs(std::shared_ptr<incoming_connection> &c);

    inco_subscription_on_accept_distribution isad_;
    inco_subscription_on_status_change issc_;
    inco_subscription_on_stop isos_;
    inco_subscription_on_releaseable isod_;

    void *ud_[4];
    void *ud2_[4];
};

struct c_inco_sbs_listener : public incoming_subscription_listener {
    virtual void on_status_change(incoming_subscription &is, SubscriptionStatus status) override {
        if(((c_inco_sbs &)is).issc_) {
            ((c_inco_sbs &)is).issc_(&is, status,
                                     ((c_inco_sbs &)is).ud_[IS_STC_UD_IDX],
                                     ((c_inco_sbs &)is).ud2_[IS_STC_UD_IDX]);
        }
    }
    virtual void on_stop(incoming_subscription &is) override {
        if(((c_inco_sbs &)is).isos_) {
            ((c_inco_sbs &)is).isos_(&is,
                                     ((c_inco_sbs &)is).ud_[IS_STO_UD_IDX],
                                     ((c_inco_sbs &)is).ud2_[IS_STO_UD_IDX]);
        }
    }
    virtual RetCode on_accept_event(incoming_subscription &is, const subscription_event &se) override {
        if(((c_inco_sbs &)is).isad_) {
            return ((c_inco_sbs &)is).isad_(&is, &se,
                                            ((c_inco_sbs &)is).ud_[IS_DIS_UD_IDX],
                                            ((c_inco_sbs &)is).ud2_[IS_DIS_UD_IDX]);
        }
        return RetCode_OK;
    }
    virtual void on_releaseable(incoming_subscription &is) override {
        if(((c_inco_sbs &)is).isod_) {
            ((c_inco_sbs &)is).isod_(&is,
                                     ((c_inco_sbs &)is).ud_[IS_REL_UD_IDX],
                                     ((c_inco_sbs &)is).ud2_[IS_REL_UD_IDX]);
        }
    }
};

static c_inco_sbs_listener cisl;

c_inco_sbs::c_inco_sbs(std::shared_ptr<incoming_connection> &c) :
    incoming_subscription(c, cisl),
    isad_(nullptr),
    issc_(nullptr),
    isos_(nullptr),
    isod_(nullptr)
{
    memset(ud_, 0, sizeof(ud_));
    memset(ud2_, 0, sizeof(ud2_));
}

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
        return (own_incoming_subscription *)new std::shared_ptr<incoming_subscription>(*(std::shared_ptr<incoming_subscription>
                                                                                         *)sbs);
    }

    incoming_subscription *inco_subscription_get_ptr(own_incoming_subscription *sbs)
    {
        return ((std::shared_ptr<incoming_subscription> *)sbs)->get();
    }

    void inco_subscription_set_on_status_change(incoming_subscription *sbs,
                                                inco_subscription_on_status_change hndl,
                                                void *ud, void *ud2)
    {
        static_cast<c_inco_sbs *>(sbs)->issc_ = hndl;
        static_cast<c_inco_sbs *>(sbs)->ud_[IS_STC_UD_IDX] = ud;
        static_cast<c_inco_sbs *>(sbs)->ud2_[IS_STC_UD_IDX] = ud2;
    }

    void inco_subscription_set_on_accept_distribution(incoming_subscription *sbs,
                                                      inco_subscription_on_accept_distribution hndl,
                                                      void *ud, void *ud2)
    {
        static_cast<c_inco_sbs *>(sbs)->isad_ = hndl;
        static_cast<c_inco_sbs *>(sbs)->ud_[IS_DIS_UD_IDX] = ud;
        static_cast<c_inco_sbs *>(sbs)->ud2_[IS_DIS_UD_IDX] = ud2;
    }

    void inco_subscription_set_on_stop(incoming_subscription *sbs,
                                       inco_subscription_on_stop hndl,
                                       void *ud, void *ud2)
    {
        static_cast<c_inco_sbs *>(sbs)->isos_ = hndl;
        static_cast<c_inco_sbs *>(sbs)->ud_[IS_STO_UD_IDX] = ud;
        static_cast<c_inco_sbs *>(sbs)->ud2_[IS_STO_UD_IDX] = ud2;
    }

    void inco_subscription_set_on_releaseable(incoming_subscription *sbs,
                                              inco_subscription_on_releaseable hndl,
                                              void *ud, void *ud2)
    {
        static_cast<c_inco_sbs *>(sbs)->isod_ = hndl;
        static_cast<c_inco_sbs *>(sbs)->ud_[IS_REL_UD_IDX] = ud;
        static_cast<c_inco_sbs *>(sbs)->ud2_[IS_REL_UD_IDX] = ud2;
    }
}

//c_inco_conn

#define IC_STC_UD_IDX 0
#define IC_DSC_UD_IDX 1
#define IC_ITX_UD_IDX 2
#define IC_ISB_UD_IDX 3
#define IC_REL_UD_IDX 4

struct c_inco_conn : public incoming_connection {
    c_inco_conn(broker &p);

    inco_connection_on_status_change icsc_;
    inco_connection_on_disconnect icodh_;
    inco_connection_on_incoming_transaction icoith_;
    inco_connection_on_incoming_subscription icoish_;
    inco_connection_on_releaseable icod_;

    void *ud_[5];
    void *ud2_[5];
};

struct c_inco_conn_listener : public incoming_connection_listener {
    virtual void on_status_change(incoming_connection &ic,
                                  ConnectionStatus current) override {
        if(((c_inco_conn &)ic).icsc_) {
            ((c_inco_conn &)ic).icsc_(&ic, current,
                                      ((c_inco_conn &)ic).ud_[IC_STC_UD_IDX],
                                      ((c_inco_conn &)ic).ud2_[IC_STC_UD_IDX]);
        }
    }

    virtual void on_disconnect(incoming_connection &ic,
                               ConnectivityEventResult con_evt_res,
                               ConnectivityEventType c_evt_type) override {
        if(((c_inco_conn &)ic).icodh_) {
            ((c_inco_conn &)ic).icodh_(&ic, con_evt_res, c_evt_type,
                                       ((c_inco_conn &)ic).ud_[IC_DSC_UD_IDX],
                                       ((c_inco_conn &)ic).ud2_[IC_DSC_UD_IDX]);
        }
    }

    virtual void on_releaseable(vlg::incoming_connection &ic) override {
        if(((c_inco_conn &)ic).icod_) {
            ((c_inco_conn &)ic).icod_(&ic,
                                      ((c_inco_conn &)ic).ud_[IC_REL_UD_IDX],
                                      ((c_inco_conn &)ic).ud2_[IC_REL_UD_IDX]);
        }
    }

    virtual RetCode on_incoming_transaction(incoming_connection &ic,
                                            std::shared_ptr<incoming_transaction> &it) override {
        if(((c_inco_conn &)ic).icoith_) {
            return ((c_inco_conn &)ic).icoith_(&ic,
                                               (shr_incoming_transaction *)&it,
                                               ((c_inco_conn &)ic).ud_[IC_ITX_UD_IDX],
                                               ((c_inco_conn &)ic).ud2_[IC_ITX_UD_IDX]);
        }
        return RetCode_OK;
    }

    virtual RetCode on_incoming_subscription(incoming_connection &ic,
                                             std::shared_ptr<incoming_subscription> &is) override {
        if(((c_inco_conn &)ic).icoish_) {
            return ((c_inco_conn &)ic).icoish_(&ic,
                                               (shr_incoming_subscription *)&is,
                                               ((c_inco_conn &)ic).ud_[IC_ISB_UD_IDX],
                                               ((c_inco_conn &)ic).ud2_[IC_ISB_UD_IDX]);
        }
        return RetCode_OK;
    }
};

static c_inco_conn_listener cicl;

c_inco_conn::c_inco_conn(broker &p) : incoming_connection(p, cicl),
    icsc_(nullptr),
    icodh_(nullptr),
    icoith_(nullptr),
    icoish_(nullptr),
    icod_(nullptr)
{
    set_incoming_transaction_factory(citf);
    set_incoming_subscription_factory(cisf);
    memset(ud_, 0, sizeof(ud_));
    memset(ud2_, 0, sizeof(ud2_));
}

//c_inco_conn_factory

struct c_inco_conn_factory : public incoming_connection_factory {
    virtual incoming_connection &make_incoming_connection(broker &p) override {
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
        static_cast<c_inco_conn *>(ic)->ud_[IC_STC_UD_IDX] = ud;
        static_cast<c_inco_conn *>(ic)->ud2_[IC_STC_UD_IDX] = ud2;
    }

    void inco_connection_set_on_disconnect(incoming_connection *ic,
                                           inco_connection_on_disconnect hndl,
                                           void *ud,
                                           void *ud2)
    {
        static_cast<c_inco_conn *>(ic)->icodh_ = hndl;
        static_cast<c_inco_conn *>(ic)->ud_[IC_DSC_UD_IDX] = ud;
        static_cast<c_inco_conn *>(ic)->ud2_[IC_DSC_UD_IDX] = ud2;
    }

    void inco_connection_set_on_incoming_transaction(incoming_connection *ic,
                                                     inco_connection_on_incoming_transaction hndl,
                                                     void *ud,
                                                     void *ud2)
    {
        static_cast<c_inco_conn *>(ic)->icoith_ = hndl;
        static_cast<c_inco_conn *>(ic)->ud_[IC_ITX_UD_IDX] = ud;
        static_cast<c_inco_conn *>(ic)->ud2_[IC_ITX_UD_IDX] = ud2;
    }

    void inco_connection_set_on_incoming_subscription(incoming_connection *ic,
                                                      inco_connection_on_incoming_subscription hndl,
                                                      void *ud,
                                                      void *ud2)
    {
        static_cast<c_inco_conn *>(ic)->icoish_ = hndl;
        static_cast<c_inco_conn *>(ic)->ud_[IC_ISB_UD_IDX] = ud;
        static_cast<c_inco_conn *>(ic)->ud2_[IC_ISB_UD_IDX] = ud2;
    }

    void inco_connection_set_on_releaseable(incoming_connection *ic,
                                            inco_connection_on_releaseable hndl,
                                            void *ud,
                                            void *ud2)
    {
        static_cast<c_inco_conn *>(ic)->icod_ = hndl;
        static_cast<c_inco_conn *>(ic)->ud_[IC_REL_UD_IDX] = ud;
        static_cast<c_inco_conn *>(ic)->ud2_[IC_REL_UD_IDX] = ud2;
    }

}

//c_broker

#define PR_NME_UD_IDX 0
#define PR_VER_UD_IDX 1
#define PR_LDC_UD_IDX 2
#define PR_INI_UD_IDX 3
#define PR_STA_UD_IDX 4
#define PR_STO_UD_IDX 5
#define PR_RUN_UD_IDX 6
#define PR_DYN_UD_IDX 7
#define PR_STC_UD_IDX 8
#define PR_ICO_UD_IDX 9

struct c_broker : public broker {
    c_broker();

    virtual const char *get_name() override {
        if(pnh_) {
            return pnh_(this, ud_[PR_NME_UD_IDX], ud2_[PR_NME_UD_IDX]);
        }
        return nullptr;
    }

    virtual const unsigned int *get_version() override {
        if(pvh_) {
            return pvh_(this, ud_[PR_VER_UD_IDX], ud2_[PR_VER_UD_IDX]);
        }
        return nullptr;
    }

    broker_name pnh_;
    broker_version pvh_;
    broker_on_load_config plch_;
    broker_on_init pih_;
    broker_on_starting pstarth_;
    broker_on_stopping pstoph_;
    broker_on_move_running ptoah_;
    broker_on_error pdbh_;
    broker_on_status_change psc_;
    broker_on_incoming_connection sic_;

    void *ud_[10];
    void *ud2_[10];
};

struct c_broker_listener : public broker_listener {
    virtual RetCode on_load_config(broker &p,
                                   int pnum,
                                   const char *param,
                                   const char *value) override {
        if(((c_broker &)p).plch_) {
            return ((c_broker &)p).plch_(&p, pnum, param, value,
                                         ((c_broker &)p).ud_[PR_LDC_UD_IDX],
                                         ((c_broker &)p).ud2_[PR_LDC_UD_IDX]);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_init(broker &p) override {
        if(((c_broker &)p).pih_) {
            return ((c_broker &)p).pih_(&p,
                                        ((c_broker &)p).ud_[PR_INI_UD_IDX],
                                        ((c_broker &)p).ud2_[PR_INI_UD_IDX]);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_starting(broker &p) override {
        if(((c_broker &)p).pstarth_) {
            return ((c_broker &)p).pstarth_(&p,
                                            ((c_broker &)p).ud_[PR_STA_UD_IDX],
                                            ((c_broker &)p).ud2_[PR_STA_UD_IDX]);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_stopping(broker &p) override {
        if(((c_broker &)p).pstoph_) {
            return ((c_broker &)p).pstoph_(&p,
                                           ((c_broker &)p).ud_[PR_STO_UD_IDX],
                                           ((c_broker &)p).ud2_[PR_STO_UD_IDX]);
        } else {
            return RetCode_OK;
        }
    }

    virtual RetCode on_move_running(broker &p) override {
        if(((c_broker &)p).ptoah_) {
            return ((c_broker &)p).ptoah_(&p,
                                          ((c_broker &)p).ud_[PR_RUN_UD_IDX],
                                          ((c_broker &)p).ud2_[PR_RUN_UD_IDX]);
        } else {
            return RetCode_OK;
        }
    }

    virtual void on_error(broker &p) override {
        if(((c_broker &)p).pdbh_) {
            ((c_broker &)p).pdbh_(&p,
                                  ((c_broker &)p).ud_[PR_DYN_UD_IDX],
                                  ((c_broker &)p).ud2_[PR_DYN_UD_IDX]);
        }
    }

    virtual void on_status_change(broker &p, BrokerStatus status) override {
        if(((c_broker &)p).psc_) {
            ((c_broker &)p).psc_(&p, status,
                                 ((c_broker &)p).ud_[PR_STC_UD_IDX],
                                 ((c_broker &)p).ud2_[PR_STC_UD_IDX]);
        }
    }

    virtual RetCode on_incoming_connection(broker &p, std::shared_ptr<incoming_connection> &ic) override {
        if(((c_broker &)p).sic_) {
            return ((c_broker &)p).sic_(&p, (shr_incoming_connection *) &ic,
                                        ((c_broker &)p).ud_[PR_ICO_UD_IDX],
                                        ((c_broker &)p).ud2_[PR_ICO_UD_IDX]);
        }
        return RetCode_OK;
    }
};

static c_broker_listener cpl;

c_broker::c_broker() :
    broker(cpl),
    pnh_(nullptr),
    pvh_(nullptr),
    plch_(nullptr),
    pih_(nullptr),
    pstarth_(nullptr),
    pstoph_(nullptr),
    ptoah_(nullptr),
    pdbh_(nullptr),
    psc_(nullptr),
    sic_(nullptr)
{
    set_incoming_connection_factory(cicf);
    memset(ud_, 0, sizeof(ud_));
    memset(ud2_, 0, sizeof(ud2_));
}

extern "C" {
    own_broker *broker_create()
    {
        return (own_broker *) new c_broker();
    }

    void broker_destroy(own_broker *p)
    {
        delete(c_broker *)p;
    }

    broker *own_broker_get_ptr(own_broker *p)
    {
        return (c_broker *)p;
    }

    RetCode broker_set_params_file_dir(broker *p, const char *dir)
    {
        return p->set_params_file_dir(dir);
    }

    RetCode broker_set_params_file_path_name(broker *p, const char *file_path)
    {
        return p->set_params_file_path_name(file_path);
    }

    const char *broker_get_name(broker *p)
    {
        return p->get_name();
    }

    const unsigned int *broker_get_version(broker *p)
    {
        return p->get_version();
    }

    unsigned int broker_get_version_major(broker *p)
    {
        return p->get_version_major();
    }

    unsigned int broker_get_version_minor(broker *p)
    {
        return p->get_version_minor();
    }

    unsigned int broker_get_version_maintenance(broker *p)
    {
        return p->get_version_maintenance();
    }

    unsigned int broker_get_version_architecture(broker *p)
    {
        return p->get_version_architecture();
    }

    int broker_is_configured(broker *p)
    {
        return p->is_configured();
    }

    const nentity_manager *broker_get_nentity_manager(broker *p)
    {
        return &p->get_nentity_manager();
    }

    int broker_is_persistent(broker *p)
    {
        return p->is_persistent() ? 1 : 0;
    }

    int broker_is_persistent_schema_creating(broker *p)
    {
        return p->is_create_persistent_schema() ? 1 : 0;
    }

    int broker_is_dropping_existing_schema(broker *p)
    {
        return p->is_drop_existing_persistent_schema() ? 1 : 0;
    }

    BrokerPersonality broker_get_personality(broker *p)
    {
        return p->get_personality();
    }

    sockaddr_in broker_get_server_sockaddr(broker *p)
    {
        return p->get_server_sockaddr();
    }

    unsigned int broker_server_executor_count(broker *p)
    {
        return p->get_server_transaction_service_executor_size();
    }

    unsigned int broker_client_executor_count(broker *p)
    {
        return p->get_client_transaction_service_executor_size();
    }

    unsigned int broker_server_sbs_executor_count(broker *p)
    {
        return p->get_server_subscription_service_executor_size();
    }

    void broker_set_personality(broker *p, BrokerPersonality personality)
    {
        p->set_personality(personality);
    }

    void broker_add_load_model(broker *p, const char *model)
    {
        p->add_load_model(model);
    }

    void broker_set_srv_sin_addr(broker *p, const char *address)
    {
        p->set_server_address(address);
    }

    void broker_set_sin_port(broker *p, int port)
    {
        p->set_server_port(port);
    }

    void broker_set_srv_executors(broker *p, unsigned int server_executors)
    {
        p->set_server_transaction_service_executor_size(server_executors);
    }

    void broker_set_cli_executors(broker *p, unsigned int client_executors)
    {
        p->set_client_transaction_service_executor_size(client_executors);
    }

    void broker_set_srv_sbs_executors(broker *p, unsigned int srv_sbs_executors)
    {
        p->set_server_subscription_service_executor_size(srv_sbs_executors);
    }

    void broker_set_persistent(broker *p, int persistent)
    {
        p->set_persistent(persistent ? true : false);
    }

    void broker_set_persistent_schema_creating(broker *p, int persistent_schema_create)
    {
        p->set_create_persistent_schema(persistent_schema_create ? true : false);
    }

    void broker_set_dropping_existing_schema(broker *p, int drop_existing_schema)
    {
        p->set_drop_existing_persistent_schema(drop_existing_schema ? true : false);
    }

    void broker_add_load_persistent_driver(broker *p, const char *driver)
    {
        p->add_load_persistent_driver(driver);
    }

    RetCode broker_extend_model_with_nem(broker *p, nentity_manager *nem)
    {
        return p->extend_model(*static_cast<nentity_manager *>(nem));
    }

    RetCode broker_extend_model_with_model_name(broker *p, const char *model_name)
    {
        return p->extend_model(model_name);
    }

    void broker_set_name(broker *p, broker_name hndl, void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->pnh_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_NME_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_NME_UD_IDX] = ud2;
    }

    void broker_set_version(broker *p, broker_version hndl, void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->pvh_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_VER_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_VER_UD_IDX] = ud2;
    }

    void broker_set_on_load_config(broker *p, broker_on_load_config hndl, void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->plch_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_LDC_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_LDC_UD_IDX] = ud2;
    }

    void broker_set_on_init(broker *p, broker_on_init hndl, void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->pih_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_INI_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_INI_UD_IDX] = ud2;
    }

    void broker_set_on_starting(broker *p, broker_on_starting hndl, void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->pstarth_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_STA_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_STA_UD_IDX] = ud2;
    }

    void broker_set_on_stopping(broker *p, broker_on_stopping hndl, void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->pstoph_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_STO_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_STO_UD_IDX] = ud2;
    }

    void broker_set_on_move_running(broker *p, broker_on_move_running hndl, void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->ptoah_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_RUN_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_RUN_UD_IDX] = ud2;
    }

    void broker_set_on_error(broker *p, broker_on_error hndl, void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->pdbh_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_DYN_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_DYN_UD_IDX] = ud2;
    }

    void broker_set_configured(broker *p, int configured)
    {
        p->set_configured(configured ? true : false);
    }

    BrokerStatus broker_get_status(broker *p)
    {
        return p->get_status();
    }

    void broker_set_on_status_change(broker *p, broker_on_status_change hndl, void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->psc_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_STC_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_STC_UD_IDX] = ud2;
    }

    void broker_set_on_incoming_connection(broker *p,
                                           broker_on_incoming_connection hndl,
                                           void *ud, void *ud2)
    {
        static_cast<c_broker *>(p)->sic_ = hndl;
        static_cast<c_broker *>(p)->ud_[PR_ICO_UD_IDX] = ud;
        static_cast<c_broker *>(p)->ud2_[PR_ICO_UD_IDX] = ud2;
    }

    RetCode broker_await_for_status_reached(broker *p,
                                            BrokerStatus test,
                                            BrokerStatus *current,
                                            time_t sec,
                                            long nsec)
    {
        return p->await_for_status_reached(test, *current, sec, nsec);
    }

    RetCode broker_await_for_status_change(broker *p,
                                           BrokerStatus *broker_status,
                                           time_t sec,
                                           long nsec)
    {
        return p->await_for_status_change(*broker_status, sec, nsec);
    }

    RetCode broker_start(broker *p, int argc, char *argv[], int spawn_new_thread)
    {
        return p->start(argc, argv, spawn_new_thread ? true : false);
    }

    RetCode broker_stop(broker *p, int force_disconnect)
    {
        return p->stop(force_disconnect ? true : false);
    }

    RetCode broker_persistence_schema_create(broker *p, PersistenceAlteringMode mode)
    {
        return p->create_persistent_schema(mode);
    }

    RetCode broker_nclass_persistence_schema_create(broker *p, PersistenceAlteringMode mode, unsigned int nclass_id)
    {
        return p->nclass_create_persistent_schema(mode, nclass_id);
    }

    RetCode broker_nclass_persistent_load(broker *p,
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

    RetCode broker_nclass_persistent_save(broker *p, nclass *in)
    {
        return p->obj_save(*in);
    }

    RetCode broker_nclass_persistent_update(broker *p, unsigned short nclass_key, nclass *in)
    {
        return p->obj_update(nclass_key, *in);
    }

    RetCode broker_nclass_persistent_update_or_save(broker *p,
                                                    unsigned short nclass_key,
                                                    nclass *in)
    {
        return p->obj_update_or_save(nclass_key, *in);
    }

    RetCode broker_nclass_persistent_remove(broker *p,
                                            unsigned short nclass_key,
                                            PersistenceDeletionMode mode,
                                            nclass *in)
    {
        return p->obj_remove(nclass_key, mode, *in);
    }

    RetCode broker_nclass_distribute(broker *p,
                                     SubscriptionEventType event_type,
                                     Action action,
                                     nclass *in)
    {
        return p->obj_distribute(event_type, action, *in);
    }

    RetCode broker_nclass_persistent_save_and_distribute(broker *p, nclass *in)
    {
        return p->obj_save_and_distribute(*in);
    }

    RetCode broker_nclass_persistent_update_and_distribute(broker *p, unsigned short nclass_key, nclass *in)
    {
        return p->obj_update_and_distribute(nclass_key, *in);
    }

    RetCode broker_nclass_persistent_update_or_save_and_distribute(broker *p, unsigned short nclass_key, nclass *in)
    {
        return p->obj_update_or_save_and_distribute(nclass_key, *in);
    }

    RetCode broker_nclass_persistent_remove_and_distribute(broker *p,
                                                           unsigned short nclass_key,
                                                           PersistenceDeletionMode mode,
                                                           nclass *in)
    {
        return p->obj_remove_and_distribute(nclass_key, mode, *in);
    }
}
