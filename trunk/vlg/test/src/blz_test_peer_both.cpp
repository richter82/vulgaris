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

#include "blaze_logger.h"
#include "blaze_peer.h"
#include "blaze_connection.h"
#include "blaze_transaction.h"
#include "blaze_subscription.h"
#include "blz_model_sample.h"
#include "blz_glob_int.h"
//needed only if statically linked
#include "blz_drv_sqlite.h"

#define LS_TST "TST|"
#define TEST_TMOUT 4

blaze::nclass_logger *blog = NULL;

int save_class_position(const char *filename,
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

int load_class_position(const char *filename,
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

//-----------------------------
//  fill_user
//-----------------------------
int fill_user(blz_model_sample::USER &usr, int count)
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
    blz_model_sample::ROLE r0, r1, r4;
    return 0;
}

//-----------------------------
// outgoing_connection
//-----------------------------
class outgoing_connection : public blaze::connection {
    public:
        virtual void on_connect(blaze::ConnectivityEventResult con_evt_res,
                                blaze::ConnectivityEventType connectivity_evt_type) {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED outgoing_connection on_connect][%d, %d]",
                             con_evt_res,
                             connectivity_evt_type))
        }
        virtual void on_disconnect(blaze::ConnectivityEventResult con_evt_res,
                                   blaze::ConnectivityEventType connectivity_evt_type) {
            IFLOG2(blog, inf(TH_ID,
                             LS_TST"[CALLED outgoing_connection on_disconnect][%d, %d]", con_evt_res,
                             connectivity_evt_type))
        }
};

//-----------------------------
// incoming_connection
//-----------------------------
class incoming_connection : public blaze::connection {
    public:
        virtual void on_connect(blaze::ConnectivityEventResult con_evt_res,
                                blaze::ConnectivityEventType connectivity_evt_type) {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED incoming_connection on_connect]"))
        }
        virtual void on_disconnect(blaze::ConnectivityEventResult con_evt_res,
                                   blaze::ConnectivityEventType connectivity_evt_type) {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED incoming_connection on_disconnect]"))
        }
};

//-----------------------------
// incoming_connection_factory
//-----------------------------
class incoming_connection_factory : public blaze::connection_factory {
    public:
        virtual ~incoming_connection_factory() {
        }
    public:
        virtual blaze::connection *new_connection(blaze::peer &p) {
            return new incoming_connection();
        }
};

//-----------------------------
// incoming_transaction
//-----------------------------
class incoming_transaction : public blaze::transaction {
    public:
        virtual void on_request() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED incoming_transaction on_request]"))
            const blaze::nclass *sending_obj = get_request_obj();
            if(sending_obj) {
                switch(sending_obj->get_nclass_id()) {
                    case USER_ENTITY_ID:
                        IFLOG2(blog, dbg(TH_ID, LS_TST"[applicative-tx mng for USER]"))
                        if(!get_connection()->get_peer()->class_persistent_update_or_save_and_distribute(
                                    1,
                                    *sending_obj)) {
                            set_transaction_result(blaze::TransactionResult_COMMITTED);
                            set_transaction_result_code(blaze::ProtocolCode_SUCCESS);
                        } else {
                            set_transaction_result(blaze::TransactionResult_FAILED);
                            set_transaction_result_code(blaze::ProtocolCode_APPLICATIVE_ERROR);
                        }
                        {
                            blaze::nclass *result_obj = sending_obj->clone();
                            set_result_obj(result_obj);
                            delete result_obj;
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        virtual void on_close() {
            IFLOG2(blog, dbg(TH_ID, LS_TST"[CALLED incoming_transaction on_close]"))
        }
};

//-----------------------------
// incoming_transaction_factory
//-----------------------------
class incoming_transaction_factory : public blaze::transaction_factory {
    public:
        virtual ~incoming_transaction_factory() {
        }
    public:
        virtual blaze::transaction *new_transaction(blaze::connection &conn) {
            return new incoming_transaction();
        }
};

//-----------------------------
// outgoing_transaction
//-----------------------------
class outgoing_transaction : public blaze::transaction {
    public:
        virtual void on_request() {
            IFLOG2(blog, dbg(TH_ID, LS_TST"[CALLED outgoing_transaction on_request]"))
        }

        virtual void on_close() {
            IFLOG2(blog, dbg(TH_ID, LS_TST"[CALLED outgoing_transaction on_close]"))
        }
};

void applicative_on_sbs_event(blaze::subscription_event &sbs_evt)
{}

//-----------------------------
// outgoing_subscription
//-----------------------------
class outgoing_subscription : public blaze::subscription {
    public:
        outgoing_subscription() : recv_items(0) {}
    public:
        virtual void on_start() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED outgoing_subscription on_start]"))
        }
        virtual void on_stop() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED outgoing_subscription on_stop]"))
        }
        virtual void on_event(blaze::subscription_event &sbs_evt) {
            IFLOG2(blog, dbg(TH_ID, LS_TST"[CALLED outgoing_subscription on_event]"))
            switch(sbs_evt.get_event_type()) {
                case blaze::SubscriptionEventType_DOWNLOAD_END:
                    IFLOG2(blog, inf(TH_ID, LS_TST"[download end]"))
                    break;
                case blaze::SubscriptionEventType_DOWNLOAD:
                    IFLOG2(blog, inf(TH_ID, LS_TST"[download event]")) {
                        blaze::nclass *obj = sbs_evt.get_object();
                        if(obj) {
                            get_connection()->get_peer()->class_persistent_update_or_save(1, *obj);
                        }
                        switch(get_subscription_class_id()) {
                            case USER_ENTITY_ID:
                                save_class_position("user.pos",
                                                    sbs_evt.get_timestamp_0(),
                                                    sbs_evt.get_timestamp_1());
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case blaze::SubscriptionEventType_LIVE:
                    IFLOG2(blog, dbg(TH_ID, LS_TST"[live event]"))
                    break;
                default:
                    IFLOG2(blog, inf(TH_ID, LS_TST"[event not managed]"))
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

//-----------------------------
// incoming_subscription
//-----------------------------
class incoming_subscription : public blaze::subscription {
    public:
        virtual void on_start() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED incoming_subscription on_start]"))
        }
        virtual void on_stop() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED incoming_subscription on_stop]"))
        }
        virtual blaze::RetCode
        on_event_accept(const blaze::subscription_event &sbs_evt) {
            IFLOG2(blog, dbg(TH_ID, LS_TST"[CALLED incoming_subscription on_event_accept]"))
            return blaze::RetCode_OK;
        }
};

//-----------------------------
// incoming_transaction_factory
//-----------------------------
class incoming_subscription_factory : public blaze::subscription_factory {
    public:
        virtual ~incoming_subscription_factory() {
        }
    public:
        virtual blaze::subscription *new_subscription(blaze::connection &conn) {
            return new incoming_subscription();
        }
};

//-----------------------------
// both_peer
//-----------------------------
class both_peer : public blaze::peer {
    public:
        both_peer() {
            peer_both_ver_[0] = 0;
            peer_both_ver_[1] = 0;
            peer_both_ver_[2] = 0;
            peer_both_ver_[3] = 0;
        }

        virtual const char *name_handler() {
#if STA_L
            return "static_peer[" __DATE__"]";
#else
            return "dyna_peer[" __DATE__"]";
#endif
        }

        virtual const unsigned int *version_handler() {
            return peer_both_ver_;
        }

        virtual blaze::RetCode on_load_config(int pnum,
                                              const char *param,
                                              const char *value) {
            IFLOG2(blog, trc(TH_ID, LS_TST"[CALLED both_peer on_load_config]"))
            return blaze::RetCode_OK;
        }

        virtual blaze::RetCode on_init() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED both_peer on_init]"))
            return blaze::RetCode_OK;
        }

        virtual blaze::RetCode on_starting() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED both_peer on_starting]"))
            return blaze::RetCode_OK;
        }

        virtual blaze::RetCode on_stopping() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED both_peer on_stopping]"))
            return blaze::RetCode_OK;
        }

        virtual blaze::RetCode on_transit_on_air() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED both_peer on_transit_on_air]"))
            return blaze::RetCode_OK;
        }

        virtual void on_error() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED both_peer on_error]"))
        }

        virtual void on_dying_breath() {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED both_peer on_dying_breath]"))
        }

        virtual blaze::RetCode
        on_new_incoming_connection(blaze::connection &incoming_connection) {
            IFLOG2(blog, inf(TH_ID, LS_TST"[CALLED both_peer on_new_incoming_connection]"))
            incoming_connection.set_transaction_factory(inco_tx_fctry_);
            incoming_connection.set_subscription_factory(inco_sbs_fctry_);
            return blaze::RetCode_OK;
        }
    private:
        unsigned int                    peer_both_ver_[4];
        incoming_subscription_factory   inco_sbs_fctry_;
        incoming_transaction_factory    inco_tx_fctry_;
};

//-----------------------------
// entry_point class
//-----------------------------
class entry_point {
    public:
        static void wait_for_enter() {
            printf("+++WAIT FOR ENTER");
            char c;
            scanf("%c", &c);
            printf("+++ENTER");
        }

    public:
        entry_point() : first_tx_send_(true), gen_evt_th_(*this) {
            tpeer_.set_connection_factory(inco_conn_fctry_);
            memset(&out_conn_params_, 0, sizeof(out_conn_params_));
            out_conn_params_.sin_family = AF_INET;
            load_client_params_by_file();
        }

        virtual ~entry_point() {
            blaze::PeerStatus p_status = blaze::PeerStatus_ZERO;
            tpeer_.stop(true);
            tpeer_.await_for_status_reached_or_outdated(blaze::PeerStatus_STOPPED,
                                                        p_status,
                                                        TEST_TMOUT);
        }

        blaze::RetCode init() {
#if STA_L
            blaze::persistence_driver_int *sqlite_dri = blaze::get_pers_driv_sqlite();
            blaze::persistence_manager::load_persistence_driver(&sqlite_dri, 1);
            tpeer_.extend_model(get_em_blz_model_sample());
#endif
            return blaze::RetCode_OK;
        }

        blaze::RetCode start_peer(int argc, char *argv[], bool spawn_thread) {
            blaze::PeerStatus p_status = blaze::PeerStatus_ZERO;
            COMMAND_IF_NOT_OK(tpeer_.start(argc,
                                           argv,
                                           spawn_thread),
                              exit(1))
            return tpeer_.await_for_status_reached_or_outdated(
                       blaze::PeerStatus_RUNNING,
                       p_status, TEST_TMOUT);
        }

        blaze::RetCode out_connect() {
            blaze::ConnectivityEventResult con_res = blaze::ConnectivityEventResult_OK;
            blaze::ConnectivityEventType con_evt_type =
                blaze::ConnectivityEventType_UNDEFINED;
            out_conn_.bind(tpeer_);
            out_conn_.connect(out_conn_params_);
            return out_conn_.await_for_connection_result(con_res,
                                                         con_evt_type,
                                                         TEST_TMOUT);
        }

        blaze::RetCode out_disconnect() {
            blaze::ConnectivityEventResult con_res = blaze::ConnectivityEventResult_OK;
            blaze::ConnectivityEventType con_evt_type =
                blaze::ConnectivityEventType_UNDEFINED;
            out_conn_.disconnect(blaze::DisconnectionResultReason_UNSPECIFIED);
            return out_conn_.await_for_disconnection_result(con_res,
                                                            con_evt_type,
                                                            TEST_TMOUT);
        }

        blaze::RetCode start_user_sbs() {
            //READ LAST RECEIVED POSITION TS0, TS1
            unsigned int ts0 = 0, ts1 = 0;
            load_class_position("user.pos", ts0, ts1);
            out_sbs_.bind(out_conn_);
            out_sbs_.start(blaze::SubscriptionType_SNAPSHOT,
                           blaze::SubscriptionMode_ALL,
                           blaze::SubscriptionFlowType_LAST,
                           blaze::SubscriptionDownloadType_PARTIAL,
                           blaze::Encode_INDEXED_NOT_ZERO,
                           USER_ENTITY_ID,
                           ts0,
                           ts1);
            blaze::ProtocolCode pcode = blaze::ProtocolCode_SUCCESS;
            blaze::SubscriptionResponse resp =
                blaze::SubscriptionResponse_UNDEFINED;
            return out_sbs_.await_for_start_result(resp, pcode, TEST_TMOUT);
        }

        blaze::RetCode send_user_tx(blz_model_sample::USER &user) {
            if(first_tx_send_) {
                out_tx_.bind(out_conn_);
                first_tx_send_ = false;
            } else {
                out_tx_.renew();
            }
            out_tx_.prepare(blaze::TransactionRequestType_OBJECT,
                            blaze::Action_INSERT,
                            &user,
                            NULL);
            out_tx_.send();
            return out_tx_.await_for_closure(TEST_TMOUT);
        }

        blaze::RetCode start_sbs_dist_th() {
            gen_evt_th_.init();
            gen_evt_th_.start();
            return blaze::RetCode_OK;
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
        sockaddr_in                 out_conn_params_;
        both_peer                   tpeer_;
        outgoing_connection         out_conn_;
        outgoing_subscription       out_sbs_;
        outgoing_transaction        out_tx_;
        bool                        first_tx_send_;
        incoming_connection_factory inco_conn_fctry_;

    private:
        class sbs_rr_gen_evts : public blaze::p_thread {
            public:
                sbs_rr_gen_evts(entry_point &ep) : ep_(ep), cycl_count_(1) {}
            public:
                blaze::RetCode init() {
                    return blaze::RetCode_OK;
                }
            public:
                virtual void *run() {
                    while(true) {
                        blaze::RetCode cdrs_res = blaze::RetCode_OK;
                        blaze::persistence_query p_qry(ep_.tpeer_.get_entity_manager());
                        blz_model_sample::USER qry_obj;
                        cdrs_res = p_qry.bind(USER_ENTITY_ID, "select * from USER");
                        if(!cdrs_res) {
                            do_qry_distr(p_qry, qry_obj);
                        }
                        blaze::mssleep(30000);
                        IFLOG2(blog, dbg(TH_ID, LS_TST"sbs dist cycle %d.", cycl_count_++))
                    }
                    return 0;
                }

            private:
                void do_qry_distr(blaze::persistence_query &p_qry,
                                  blaze::nclass &qry_obj) {
                    blaze::RetCode cdrs_res = blaze::RetCode_OK;
                    unsigned int ts0 = 0, ts1 = 0;
                    while((cdrs_res = p_qry.next_entity(ts0,
                                                        ts1,
                                                        qry_obj)) == blaze::RetCode_DBROW) {
                        ep_.tpeer_.class_distribute(blaze::SubscriptionEventType_LIVE,
                                                    blaze::Action_UPDATE,
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

//-----------------------------
// MAIN
//-----------------------------
int main(int argc, char *argv[])
{
    blog = blaze::get_nclass_logger("root");

    entry_point ep;
    ep.init();
    ep.start_peer(argc, argv, true);
    ep.start_sbs_dist_th();

    blaze::collector_stat::get_instance().start_monitoring(10, blaze::TL_WRN);

    entry_point::wait_for_enter();
    ep.out_connect();
    ep.start_user_sbs();

    int count = 0;
    while(true) {
        entry_point::wait_for_enter();
        blz_model_sample::USER user;
        fill_user(user, ++count);
        ep.send_user_tx(user);
        blaze::mssleep(50);
    }

    IFLOG2(blog, inf(TH_ID, LS_TST"MAIN WAITS"))
    blaze::synch_monitor mon;
    mon.lock();
    mon.wait();
    mon.unlock();
    return 0;
}
