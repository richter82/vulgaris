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

#define LS_TST "TT|"
#define TEST_TMOUT 4

int save_nclass_position(const char *filename,
                         unsigned int ts0,
                         unsigned int ts1)
{
    FILE *f;
    if((f = fopen(filename,"w+"))) {
        fprintf(f, "%u\n%u\n", ts0, ts1);
        fclose(f);
        return 0;
    } else {
        return -1;
    }
}

int load_nclass_position(const char *filename,
                         unsigned int &ts0,
                         unsigned int &ts1)
{
    FILE *f;
    if((f = fopen(filename,"r"))) {
        fscanf(f, "%u%u", &ts0, &ts1);
        fclose(f);
    } else {
        ts0 = ts1 = 0;
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
    smplmdl::ROLE r0, r1, r4;
    return 0;
}

// outg_conn

struct outg_conn : public vlg::outgoing_connection {
    virtual void on_connect(vlg::ConnectivityEventResult con_evt_res,
                            vlg::ConnectivityEventType connectivity_evt_type) override {
        IFLOG(inf(TH_ID, LS_TST"[called outg_conn on_connect][%d, %d]",
                  con_evt_res,
                  connectivity_evt_type))
    }
    virtual void on_disconnect(vlg::ConnectivityEventResult con_evt_res,
                               vlg::ConnectivityEventType connectivity_evt_type) override {
        IFLOG(inf(TH_ID, LS_TST"[called outg_conn on_disconnect][%d, %d]",
                  con_evt_res,
                  connectivity_evt_type))
    }
};

// inco_conn

struct inco_conn : public vlg::incoming_connection {
    inco_conn(vlg::peer &p) : incoming_connection(p) {}

    virtual void on_disconnect(vlg::ConnectivityEventResult con_evt_res,
                               vlg::ConnectivityEventType connectivity_evt_type) override {
        IFLOG(inf(TH_ID, LS_TST"[called inco_conn on_disconnect]"))
    }
};

// incoming_connection_factory

struct incoming_connection_factory : public vlg::incoming_connection_factory {
    virtual ~incoming_connection_factory() {
    }
    virtual inco_conn &make_incoming_connection(vlg::peer &p) override {
        return *new inco_conn(p);
    }
};

// incoming_transaction

struct inco_tx : public vlg::incoming_transaction {
    inco_tx(std::shared_ptr<vlg::incoming_connection> &conn) : vlg::incoming_transaction(conn) {}
    virtual void on_request() override {
        IFLOG(inf(TH_ID, LS_TST"[called incoming_transaction on_request]"))
        const vlg::nclass *sending_obj = get_request_obj();
        if(sending_obj) {
            switch(sending_obj->get_id()) {
                case USER_ENTITY_ID:
                    IFLOG(dbg(TH_ID, LS_TST"[applicative-tx mng for USER]"))
                    if(!get_connection().get_peer().obj_update_or_save_and_distribute(1, *sending_obj)) {
                        set_result(vlg::TransactionResult_COMMITTED);
                        set_result_code(vlg::ProtocolCode_SUCCESS);
                    } else {
                        set_result(vlg::TransactionResult_FAILED);
                        set_result_code(vlg::ProtocolCode_APPLICATIVE_ERROR);
                    }
                    set_result_obj(*sending_obj);
                    break;
                default:
                    break;
            }
        }
    }

    virtual void on_close() override {
        IFLOG(dbg(TH_ID, LS_TST"[called incoming_transaction on_close]"))
    }
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

struct outg_tx : public vlg::outgoing_transaction {
    virtual void on_close() override {
        IFLOG(dbg(TH_ID, LS_TST"[called outgoing_transaction on_close]"))
    }
};

void applicative_on_sbs_event(vlg::subscription_event &sbs_evt)
{}

// outgoing_subscription

struct outg_sbs : public vlg::outgoing_subscription {
        outg_sbs() : recv_items(0) {}

        virtual void on_start() override {
            IFLOG(inf(TH_ID, LS_TST"[called outgoing_subscription on_start]"))
        }

        virtual void on_stop() override {
            IFLOG(inf(TH_ID, LS_TST"[called outgoing_subscription on_stop]"))
        }

        virtual void on_incoming_event(std::unique_ptr<vlg::subscription_event> &sbs_evt) override {
            IFLOG(dbg(TH_ID, LS_TST"[called outgoing_subscription on_event]"))
            switch(sbs_evt.get()->get_event_type()) {
                case vlg::SubscriptionEventType_DOWNLOAD_END:
                    IFLOG(inf(TH_ID, LS_TST"[download end]"))
                    break;
                case vlg::SubscriptionEventType_DOWNLOAD:
                    IFLOG(inf(TH_ID, LS_TST"[download event]")) {
                        get_connection().get_peer().obj_update_or_save(1, *sbs_evt->get_data());
                        switch(get_nclass_id()) {
                            case USER_ENTITY_ID:
                                save_nclass_position("user.pos",
                                                     sbs_evt.get()->get_timestamp_0(),
                                                     sbs_evt.get()->get_timestamp_1());
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case vlg::SubscriptionEventType_LIVE:
                    IFLOG(dbg(TH_ID, LS_TST"[live event]"))
                    break;
                default:
                    IFLOG(inf(TH_ID, LS_TST"[event not managed]"))
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

// incoming_subscription

struct inco_sbs : public vlg::incoming_subscription {
    inco_sbs(std::shared_ptr<vlg::incoming_connection> &conn) : vlg::incoming_subscription(conn) {}

    virtual void on_stop() override {
        IFLOG(inf(TH_ID, LS_TST"[called incoming_subscription on_stop]"))
    }

    virtual vlg::RetCode accept_distribution(const vlg::subscription_event &sbs_evt) override {
        IFLOG(dbg(TH_ID, LS_TST"[called incoming_subscription on_event_accept]"))
        return vlg::RetCode_OK;
    }
};

// incoming_transaction_factory

struct incoming_subscription_factory : public vlg::incoming_subscription_factory {
    virtual ~incoming_subscription_factory() {
    }

    virtual inco_sbs &make_incoming_subscription(std::shared_ptr<vlg::incoming_connection> &conn) override {
        return *new inco_sbs(conn);
    }
};

// both_peer

struct both_peer : public vlg::peer {

        both_peer() {
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

        virtual vlg::RetCode on_load_config(int pnum,
                                            const char *param,
                                            const char *value) override {
            IFLOG(trc(TH_ID, LS_TST"[called both_peer on_load_config]"))
            return vlg::RetCode_OK;
        }

        virtual vlg::RetCode on_init() override {
            IFLOG(inf(TH_ID, LS_TST"[called both_peer on_init]"))
            return vlg::RetCode_OK;
        }

        virtual vlg::RetCode on_starting() override {
            IFLOG(inf(TH_ID, LS_TST"[called both_peer on_starting]"))
            return vlg::RetCode_OK;
        }

        virtual vlg::RetCode on_stopping() override {
            IFLOG(inf(TH_ID, LS_TST"[called both_peer on_stopping]"))
            return vlg::RetCode_OK;
        }

        virtual vlg::RetCode on_move_running() override {
            IFLOG(inf(TH_ID, LS_TST"[called both_peer on_move_running]"))
            return vlg::RetCode_OK;
        }

        virtual vlg::RetCode on_error() override {
            IFLOG(inf(TH_ID, LS_TST"[called both_peer on_error]"))
            return vlg::RetCode_OK;
        }

        virtual void on_dying_breath() override {
            IFLOG(inf(TH_ID, LS_TST"[called both_peer on_dying_breath]"))
        }

        virtual vlg::RetCode on_incoming_connection(std::shared_ptr<vlg::incoming_connection> &iconn) override {
            IFLOG(inf(TH_ID, LS_TST"[called both_peer on_new_incoming_connection]"))
            iconn->set_incoming_transaction_factory(inco_tx_fctry_);
            iconn->set_incoming_subscription_factory(inco_sbs_fctry_);
            return vlg::RetCode_OK;
        }

    private:
        unsigned int peer_both_ver_[4];
        incoming_subscription_factory inco_sbs_fctry_;
        incoming_transaction_factory inco_tx_fctry_;
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
            vlg::persistence_driver *sqlite_dri = vlg::get_pers_driv_sqlite();
            vlg::persistence_manager::load_driver(&sqlite_dri, 1);
            tpeer_.extend_model(*get_nem_smplmdl());
#endif
            return vlg::RetCode_OK;
        }

        vlg::RetCode start_peer(int argc, char *argv[], bool spawn_thread) {
            vlg::PeerStatus p_status = vlg::PeerStatus_ZERO;
            CMD_ON_KO(tpeer_.start(argc, argv, spawn_thread), exit(1))
            return tpeer_.await_for_status_reached(vlg::PeerStatus_RUNNING,
                                                   p_status, TEST_TMOUT);
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
            unsigned int ts0 = 0, ts1 = 0;
            load_nclass_position("user.pos", ts0, ts1);

            out_sbs_.bind(out_conn_);
            out_sbs_.start(vlg::SubscriptionType_SNAPSHOT,
                           vlg::SubscriptionMode_ALL,
                           vlg::SubscriptionFlowType_LAST,
                           vlg::SubscriptionDownloadType_PARTIAL,
                           vlg::Encode_INDEXED_NOT_ZERO,
                           USER_ENTITY_ID,
                           ts0,
                           ts1);

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

    private:
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
                        vlg::persistence_query p_qry(ep_.tpeer_.get_entity_manager());
                        smplmdl::USER qry_obj;
                        rcode = p_qry.execute(USER_ENTITY_ID, "select * from USER");

                        if(!rcode) {
                            do_qry_distr(p_qry, qry_obj);
                        }

                        vlg::mssleep(30000);
                        IFLOG(dbg(TH_ID, LS_TST"subscription dist cycle %d.", cycl_count_++))
                    }
                    return 0;
                }

            private:
                void do_qry_distr(vlg::persistence_query &p_qry,
                                  vlg::nclass &qry_obj) {
                    vlg::RetCode rcode = vlg::RetCode_OK;
                    unsigned int ts0 = 0, ts1 = 0;
                    while((rcode = p_qry.next_obj(ts0, ts1, qry_obj)) == vlg::RetCode_DBROW) {
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

    int count = 0;
    while(true) {
        entry_point::wait_for_enter();
        smplmdl::USER user;
        fill_user(user, ++count);
        ep.send_user_tx(user);
        vlg::mssleep(50);
    }

    IFLOG(inf(TH_ID, LS_TST"MAIN WAITS"))
    vlg::mx mon;
    mon.lock();
    mon.wait();
    mon.unlock();
    return 0;
}
