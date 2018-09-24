/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "vlg_model_sample.h"
#include "glob.h"

#include "vlg_persistence.h"
//needed only if statically linked
#include "vlg_drv_sqlite.h"

#define LS_TST "TT|%s"
#define TEST_TMOUT 4

int save_nclass_position(const char *filename,
                         unsigned int ts_0,
                         unsigned int ts_1)
{
    FILE *f;
    if((f = fopen(filename,"w+"))) {
        fprintf(f, "%u\n%u\n", ts_0, ts_1);
        fclose(f);
        return 0;
    } else {
        return -1;
    }
}

int load_nclass_position(const char *filename,
                         unsigned int &ts_0,
                         unsigned int &ts_1)
{
    FILE *f;
    if((f = fopen(filename,"r"))) {
        fscanf(f, "%u%u", &ts_0, &ts_1);
        fclose(f);
    } else {
        ts_0 = ts_1 = 0;
    }
    return 0;
}

//  fill_user

int fill_user(smplmdl::USER &usr, int count)
{
    usr.set_user_id(count);
    usr.set_name("John");
    usr.set_surname("Richman");
    usr.set_sex(SEX_Male);
    usr.set_email("john.richman@gmail.com");
    usr.set_weight(76.9f);
    usr.set_height(183.2f);
    usr.set_active(true);
    usr.set_cap(10001);
    usr.set_type('K');
    usr.set_raw_txt((unsigned char *)"testtesttesttesttesttesttesttest");
    smplmdl::ROLE r0, r1, r4;
    return 0;
}

// outg_conn

struct outg_conn_listener : public vlg::outgoing_connection_listener {
    virtual void on_status_change(vlg::outgoing_connection &oc,
                                  vlg::ConnectionStatus current) override {
        IFLOG(oc.get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
    }

    virtual void on_connect(vlg::outgoing_connection &oc,
                            vlg::ConnectivityEventResult con_evt_res,
                            vlg::ConnectivityEventType c_evt_type) override {
        IFLOG(oc.get_peer().get_logger(), inf(TH_ID, LS_TST"[%d, %d]", __func__,
                                              con_evt_res,
                                              c_evt_type))
    }

    virtual void on_disconnect(vlg::outgoing_connection &oc,
                               vlg::ConnectivityEventResult con_evt_res,
                               vlg::ConnectivityEventType c_evt_type) override {
        IFLOG(oc.get_peer().get_logger(), inf(TH_ID, LS_TST"[%d, %d]", __func__,
                                              con_evt_res,
                                              c_evt_type))
    }
};

static outg_conn_listener ocl;

struct outg_conn : public vlg::outgoing_connection {
    outg_conn() : vlg::outgoing_connection(ocl) {}
};

// inco_conn

struct inco_conn_listener : public vlg::incoming_connection_listener {
    virtual void on_status_change(vlg::incoming_connection &ic,
                                  vlg::ConnectionStatus current) override {
        IFLOG(ic.get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
    }

    virtual void on_disconnect(vlg::incoming_connection &ic,
                               vlg::ConnectivityEventResult con_evt_res,
                               vlg::ConnectivityEventType c_evt_type) override {
        IFLOG(ic.get_peer().get_logger(), inf(TH_ID, LS_TST"[%d, %d]", __func__, con_evt_res, c_evt_type))
    }

    virtual void on_releaseable(vlg::incoming_connection &ic) override {
        IFLOG(ic.get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
    }

    virtual vlg::RetCode on_incoming_transaction(vlg::incoming_connection &ic,
                                                 std::shared_ptr<vlg::incoming_transaction> &) override {
        IFLOG(ic.get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
        return vlg::RetCode_OK;
    }

    virtual vlg::RetCode on_incoming_subscription(vlg::incoming_connection &ic,
                                                  std::shared_ptr<vlg::incoming_subscription> &) override {
        IFLOG(ic.get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
        return vlg::RetCode_OK;
    }
};

static inco_conn_listener icl;

// inco_conn

struct inco_conn : public vlg::incoming_connection {
    inco_conn(vlg::peer &p) : incoming_connection(p, icl) {}
};

// incoming_connection_factory

struct incoming_connection_factory : public vlg::incoming_connection_factory {
    virtual ~incoming_connection_factory() {
    }
    virtual inco_conn &make_incoming_connection(vlg::peer &p) override {
        return *new inco_conn(p);
    }
};

struct inco_tx_listener : public vlg::incoming_transaction_listener {
    virtual void on_status_change(vlg::incoming_transaction &, vlg::TransactionStatus) override {};
    virtual void on_request(vlg::incoming_transaction &it) override {
        IFLOG(it.get_connection().get_peer().get_logger(), inf(TH_ID, LS_TST, __func__))
        const vlg::nclass *sending_obj = it.get_request_obj();
        if(sending_obj) {
            switch(sending_obj->get_id()) {
                case USER_ENTITY_ID:
                    IFLOG(it.get_connection().get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__, "[applicative-tx mng for USER]"))
                    if(!it.get_connection().get_peer().obj_update_or_save_and_distribute(1, *sending_obj)) {
                        it.set_result(vlg::TransactionResult_COMMITTED);
                        it.set_result_code(vlg::ProtocolCode_SUCCESS);
                    } else {
                        it.set_result(vlg::TransactionResult_FAILED);
                        it.set_result_code(vlg::ProtocolCode_APPLICATIVE_ERROR);
                    }
                    it.set_result_obj(*sending_obj);
                    break;
                default:
                    break;
            }
        }
    }

    virtual void on_close(vlg::incoming_transaction &it) override {
        IFLOG(it.get_connection().get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
    }

    virtual void on_releaseable(vlg::incoming_transaction &it) override {
        IFLOG(it.get_connection().get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
    }
};

static inco_tx_listener itl;

// incoming_transaction

struct inco_tx : public vlg::incoming_transaction {
    inco_tx(std::shared_ptr<vlg::incoming_connection> &conn) : vlg::incoming_transaction(conn, itl) {}
};

// incoming_transaction_factory

struct incoming_transaction_factory : public vlg::incoming_transaction_factory {
    virtual ~incoming_transaction_factory() {
    }

    virtual inco_tx &make_incoming_transaction(std::shared_ptr<vlg::incoming_connection> &conn) override {
        return *new inco_tx(conn);
    }
};

// outgoing_transaction

struct outg_tx_listener : public vlg::outgoing_transaction_listener {
    virtual void on_status_change(vlg::outgoing_transaction &, vlg::TransactionStatus) override {}
    virtual void on_close(vlg::outgoing_transaction &ot) override {
        IFLOG(ot.get_connection().get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
    }
};

static outg_tx_listener otl;

struct outg_tx : public vlg::outgoing_transaction {
    outg_tx() : vlg::outgoing_transaction(otl) {}
};

void applicative_on_sbs_event(vlg::subscription_event &sbs_evt)
{}

// outgoing_subscription

struct outg_sbs_listener : public vlg::outgoing_subscription_listener {
        virtual void on_status_change(vlg::outgoing_subscription &, vlg::SubscriptionStatus) override {}
        virtual void on_start(vlg::outgoing_subscription &os) override {
            IFLOG(os.get_connection().get_peer().get_logger(), inf(TH_ID, LS_TST, __func__))
        }
        virtual void on_stop(vlg::outgoing_subscription &os) override {
            IFLOG(os.get_connection().get_peer().get_logger(), inf(TH_ID, LS_TST, __func__))
        }
        virtual void on_incoming_event(vlg::outgoing_subscription &os, std::unique_ptr<vlg::subscription_event> &se) override {
            IFLOG(os.get_connection().get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
            switch(se.get()->get_event_type()) {
                case vlg::SubscriptionEventType_DOWNLOAD_END:
                    IFLOG(os.get_connection().get_peer().get_logger(), inf(TH_ID, LS_TST"[download end]", __func__))
                    break;
                case vlg::SubscriptionEventType_DOWNLOAD:
                    IFLOG(os.get_connection().get_peer().get_logger(), inf(TH_ID, LS_TST"[download event]", __func__)) {
                        os.get_connection().get_peer().obj_update_or_save(1, *se->get_data());
                        switch(os.get_nclass_id()) {
                            case USER_ENTITY_ID:
                                save_nclass_position("user.pos",
                                                     se.get()->get_timestamp_0(),
                                                     se.get()->get_timestamp_1());
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case vlg::SubscriptionEventType_LIVE:
                    IFLOG(os.get_connection().get_peer().get_logger(), dbg(TH_ID, LS_TST"[live event]", __func__))
                    break;
                default:
                    IFLOG(os.get_connection().get_peer().get_logger(), dbg(TH_ID, LS_TST"[event not managed]", __func__))
                    break;
            }
            recv_items++;
        }

        int get_recv_items() const {
            return recv_items;
        }

        void set_recv_items(int val) {
            recv_items = val;
        }
    private:
        int recv_items;
};

static outg_sbs_listener osl;

struct outg_sbs : public vlg::outgoing_subscription {
    outg_sbs() : vlg::outgoing_subscription(osl) {}
};

// incoming_subscription

struct inco_sbs_listener : public vlg::incoming_subscription_listener {
    virtual void on_status_change(vlg::incoming_subscription &is, vlg::SubscriptionStatus) override {
        IFLOG(is.get_connection().get_peer().get_logger(), trc(TH_ID, LS_TST, __func__))
    }
    virtual void on_stop(vlg::incoming_subscription &is) override {
        IFLOG(is.get_connection().get_peer().get_logger(), inf(TH_ID, LS_TST, __func__))
    }
    virtual vlg::RetCode on_accept_event(vlg::incoming_subscription &is, const vlg::subscription_event &) override {
        IFLOG(is.get_connection().get_peer().get_logger(), dbg(TH_ID, LS_TST, __func__))
        return vlg::RetCode_OK;
    }
    virtual void on_releaseable(vlg::incoming_subscription &is) override {
        IFLOG(is.get_connection().get_peer().get_logger(), inf(TH_ID, LS_TST, __func__))
    }
};

static inco_sbs_listener isl;

struct inco_sbs : public vlg::incoming_subscription {
    inco_sbs(std::shared_ptr<vlg::incoming_connection> &conn) : vlg::incoming_subscription(conn, isl) {}
};

// incoming_transaction_factory

struct incoming_subscription_factory : public vlg::incoming_subscription_factory {
    virtual ~incoming_subscription_factory() {
    }

    virtual inco_sbs &make_incoming_subscription(std::shared_ptr<vlg::incoming_connection> &conn) override {
        return *new inco_sbs(conn);
    }
};

struct both_peer_listener : public vlg::peer_listener {

    virtual void on_status_change(vlg::peer &p, vlg::PeerStatus) override {
        IFLOG(p.get_logger(), trc(TH_ID, LS_TST, __func__))
    }

    virtual vlg::RetCode on_load_config(vlg::peer &p,
                                        int pnum,
                                        const char *param,
                                        const char *value) override {
        IFLOG(p.get_logger(), trc(TH_ID, LS_TST, __func__))
        return vlg::RetCode_OK;
    }

    virtual vlg::RetCode on_init(vlg::peer &p) override {
        IFLOG(p.get_logger(), trc(TH_ID, LS_TST, __func__))
        return vlg::RetCode_OK;
    }

    virtual vlg::RetCode on_starting(vlg::peer &p) override {
        IFLOG(p.get_logger(), trc(TH_ID, LS_TST, __func__))
        return vlg::RetCode_OK;
    }

    virtual vlg::RetCode on_stopping(vlg::peer &p) override {
        IFLOG(p.get_logger(), trc(TH_ID, LS_TST, __func__))
        return vlg::RetCode_OK;
    }

    virtual vlg::RetCode on_move_running(vlg::peer &p) override {
        IFLOG(p.get_logger(), trc(TH_ID, LS_TST, __func__))
        return vlg::RetCode_OK;
    }

    virtual void on_dying_breath(vlg::peer &p) override {
        IFLOG(p.get_logger(), trc(TH_ID, LS_TST, __func__))
    }

    virtual vlg::RetCode on_incoming_connection(vlg::peer &p, std::shared_ptr<vlg::incoming_connection> &iconn) override {
        IFLOG(p.get_logger(), trc(TH_ID, LS_TST, __func__))
        iconn->set_incoming_transaction_factory(inco_tx_fctry_);
        iconn->set_incoming_subscription_factory(inco_sbs_fctry_);
        return vlg::RetCode_OK;
    }

    incoming_subscription_factory inco_sbs_fctry_;
    incoming_transaction_factory inco_tx_fctry_;
};

static both_peer_listener bpl;

// both_peer

struct both_peer : public vlg::peer {
        both_peer() : vlg::peer(bpl) {
            peer_both_ver_[0] = 0;
            peer_both_ver_[1] = 0;
            peer_both_ver_[2] = 0;
            peer_both_ver_[3] = 0;
        }

        virtual const char *get_name() override {
#if STA_L
            return "static_peer[" __DATE__"]";
#else
            return "dyna_peer[" __DATE__"]";
#endif
        }

        virtual const unsigned int *get_version() override {
            return peer_both_ver_;
        }

    private:
        unsigned int peer_both_ver_[4];
};

// entry_point class

struct entry_point {
        static void wait_for_enter() {
            printf("+++WAIT FOR ENTER");
            char c;
            scanf("%c", &c);
            printf("+++ENTER");
        }

        entry_point() : first_tx_send_(true), gen_evt_th_(*this) {
            tpeer_.set_incoming_connection_factory(inco_conn_fctry_);
            memset(&out_conn_params_, 0, sizeof(out_conn_params_));
            out_conn_params_.sin_family = AF_INET;
            load_client_params_by_file();
        }

        virtual ~entry_point() {
            vlg::PeerStatus p_status = vlg::PeerStatus_ZERO;
            tpeer_.stop(true);
            tpeer_.await_for_status_reached(vlg::PeerStatus_STOPPED,
                                            p_status,
                                            TEST_TMOUT);
        }

        vlg::RetCode init() {
#if STA_L
            vlg::persistence_driver *sqlite_dri = vlg::get_pers_driv_sqlite(tpeer_.get_logger());
            vlg::persistence_manager::load_driver(&sqlite_dri, 1);
            tpeer_.extend_model(*get_nem_smplmdl(tpeer_.get_logger()));
#endif
            return vlg::RetCode_OK;
        }

        vlg::RetCode start_peer(int argc, char *argv[], bool spawn_thread) {
            vlg::PeerStatus p_status = vlg::PeerStatus_ZERO;
            CMD_ON_KO(tpeer_.start(argc, argv, spawn_thread), exit(1))
            return tpeer_.await_for_status_reached(vlg::PeerStatus_RUNNING, p_status, TEST_TMOUT);
        }

        vlg::RetCode out_connect() {
            vlg::ConnectivityEventResult con_res = vlg::ConnectivityEventResult_OK;
            vlg::ConnectivityEventType con_evt_type = vlg::ConnectivityEventType_UNDEFINED;
            out_conn_.bind(tpeer_);
            out_conn_.connect(out_conn_params_);
            return out_conn_.await_for_connection_result(con_res,
                                                         con_evt_type,
                                                         TEST_TMOUT);
        }

        vlg::RetCode out_disconnect() {
            vlg::ConnectivityEventResult con_res = vlg::ConnectivityEventResult_OK;
            vlg::ConnectivityEventType con_evt_type = vlg::ConnectivityEventType_UNDEFINED;
            out_conn_.disconnect(vlg::ProtocolCode_UNSPECIFIED);
            return out_conn_.await_for_disconnection_result(con_res,
                                                            con_evt_type,
                                                            TEST_TMOUT);
        }

        vlg::RetCode start_user_sbs() {
            //READ LAST RECEIVED POSITION TS0, TS1
            unsigned int ts_0 = 0, ts_1 = 0;

            //load_nclass_position("user.pos", ts_0, ts_1);

            out_sbs_.bind(out_conn_);
            out_sbs_.start(vlg::SubscriptionType_SNAPSHOT,
                           vlg::SubscriptionMode_ALL,
                           vlg::SubscriptionFlowType_LAST,
                           vlg::SubscriptionDownloadType_PARTIAL,
                           vlg::Encode_INDEXED_NOT_ZERO,
                           USER_ENTITY_ID,
                           ts_0,
                           ts_1);

            vlg::ProtocolCode pcode = vlg::ProtocolCode_SUCCESS;
            vlg::SubscriptionResponse resp = vlg::SubscriptionResponse_UNDEFINED;
            return out_sbs_.await_for_start_result(resp, pcode, TEST_TMOUT);
        }

        vlg::RetCode send_user_tx(smplmdl::USER &user) {
            if(first_tx_send_) {
                out_tx_.bind(out_conn_);
                first_tx_send_ = false;
            } else {
                out_tx_.renew();
            }

            out_tx_.set_request_obj(user);
            out_tx_.set_request_action(vlg::Action_INSERT);

            out_tx_.send();
            return out_tx_.await_for_close(TEST_TMOUT);
        }

        vlg::RetCode start_sbs_dist_th() {
            gen_evt_th_.init();
            gen_evt_th_.start();
            return vlg::RetCode_OK;
        }

    private:
        int load_client_params_by_file() {
            unsigned int port = 0;
            char host[256] = {0};
            FILE *f = fopen("lastconn", "r");
            if(f) {
                fscanf(f, "%s%u", host, &port);
                fclose(f);
            } else {
                strcpy(host, "127.0.0.1");
                port = 12345;
            }
            printf("***CLI--------------\n");
            printf("host: %s \n", host);
            printf("port: %u \n", port);
            printf("***CLI--------------\n");
            out_conn_params_.sin_addr.s_addr = inet_addr(host);
            out_conn_params_.sin_port = htons(port);
            return 0;
        }

    public:
        sockaddr_in out_conn_params_;
        both_peer tpeer_;
        outg_conn out_conn_;
        outg_sbs out_sbs_;
        outg_tx out_tx_;
        bool first_tx_send_;
        incoming_connection_factory inco_conn_fctry_;

    private:
        class sbs_rr_gen_evts : public vlg::p_th {
            public:
                sbs_rr_gen_evts(entry_point &ep) : ep_(ep), cycl_count_(1) {}
            public:
                vlg::RetCode init() {
                    return vlg::RetCode_OK;
                }
            public:
                virtual void *run() {
                    while(true) {
                        vlg::RetCode rcode = vlg::RetCode_OK;
                        vlg::persistence_query p_qry(ep_.tpeer_.get_nentity_manager());
                        smplmdl::USER qry_obj;
                        rcode = p_qry.execute(USER_ENTITY_ID, "select * from USER");

                        if(!rcode) {
                            do_qry_distr(p_qry, qry_obj);
                        }

                        vlg::mssleep(30000);
                        IFLOG(ep_.tpeer_.get_logger(), dbg(TH_ID, LS_TST"subscription dist cycle %d.", __func__, cycl_count_++))
                    }
                    return 0;
                }

            private:
                void do_qry_distr(vlg::persistence_query &p_qry,
                                  vlg::nclass &qry_obj) {
                    vlg::RetCode rcode = vlg::RetCode_OK;
                    unsigned int ts_0 = 0, ts_1 = 0;
                    while((rcode = p_qry.next_obj(ts_0, ts_1, qry_obj)) == vlg::RetCode_DBROW) {
                        ep_.tpeer_.obj_distribute(vlg::SubscriptionEventType_LIVE,
                                                  vlg::Action_UPDATE,
                                                  qry_obj);
                    }
                }

            private:
                entry_point &ep_;
                int cycl_count_;
        };

    private:
        sbs_rr_gen_evts gen_evt_th_;
};

// MAIN

int main(int argc, char *argv[])
{
    entry_point ep;
    ep.init();
    ep.start_peer(argc, argv, true);
    //ep.start_sbs_dist_th();

    entry_point::wait_for_enter();
    ep.out_connect();
    ep.start_user_sbs();

    //int count = 0;
    //while(true) {
    //    //entry_point::wait_for_enter();
    //    smplmdl::USER user;
    //    fill_user(user, ++count);
    //    ep.send_user_tx(user);
    //    vlg::mssleep(50);
    //}

    IFLOG(ep.tpeer_.get_logger(), inf(TH_ID, LS_TST"MAIN WAITS", __func__))
    vlg::mx mon;
    mon.lock();
    mon.wait();
    mon.unlock();
    return 0;
}
