/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "pr_aut.h"
#include "selector.h"
#include "prs_impl.h"
#include "sbs_impl.h"

namespace vlg {

struct connection_impl;

struct peer_recv_task_inco_conn : public p_tsk {
    peer_recv_task_inco_conn(std::shared_ptr<incoming_connection> &conn_sh,
                             vlg_hdr_rec &pkt_hdr,
                             std::unique_ptr<g_bbuf> &pkt_body);

    ~peer_recv_task_inco_conn();
    virtual RetCode execute() override;

    std::shared_ptr<incoming_connection> conn_sh_;
    vlg_hdr_rec pkt_hdr_;
    std::unique_ptr<g_bbuf> pkt_body_;
};

struct peer_recv_task_outg_conn : public p_tsk {
    peer_recv_task_outg_conn(outgoing_connection &conn,
                             vlg_hdr_rec &pkt_hdr,
                             std::unique_ptr<g_bbuf> &pkt_body);

    ~peer_recv_task_outg_conn();
    virtual RetCode execute() override;

    outgoing_connection &conn_;
    vlg_hdr_rec pkt_hdr_;
    std::unique_ptr<g_bbuf> pkt_body_;
};

/***********************************
an helper class used to server peer
to generate a progressive
- per nclass_id - subscription event id.
It is also used to generate ts_0, ts_1.
***********************************/
struct per_nclass_id_conn_set {
    explicit per_nclass_id_conn_set();

    unsigned int next_sbs_evt_id();

    void next_time_stamp(unsigned int &ts_0,
                         unsigned int &ts_1);

    unsigned int sbsevtid_;
    unsigned int ts0_;
    unsigned int ts1_;
    s_hm connid_condesc_set_;  //[connid --> condesc]
    mutable mx mon_;
};

// peer_impl
struct peer_impl : public peer_automa {
        explicit peer_impl(peer &, peer_listener &);

        RetCode set_params_file_dir(const char *dir);

        void set_cfg_load_model(const char *model);
        void set_cfg_srv_sin_addr(const char *addr);
        void set_cfg_srv_sin_port(int port);
        void set_cfg_load_pers_driv(const char *driv);

        RetCode extend_model(nentity_manager &nem);
        RetCode extend_model(const char *model_name);

        unsigned int next_connid() {
            scoped_mx smx(mon_);
            return ++prgr_conn_id_;
        }

        RetCode new_incoming_connection(std::shared_ptr<incoming_connection> &new_connection,
                                        unsigned int connid = 0);

        incoming_connection_factory &get_incoming_connection_factory() const;
        void set_incoming_connection_factory(incoming_connection_factory &conn_f);

        RetCode recv_and_route_pkt(outgoing_connection &conn,
                                   vlg_hdr_rec *hdr,
                                   g_bbuf *body);

        RetCode recv_and_route_pkt(std::shared_ptr<incoming_connection> &inco_conn,
                                   vlg_hdr_rec *hdr,
                                   g_bbuf *body);

        RetCode add_subscriber(incoming_subscription_impl *sbsdesc);
        RetCode remove_subscriber(incoming_subscription_impl *sbsdesc);

        RetCode get_per_nclassid_helper_rec(unsigned int nclass_id,
                                            per_nclass_id_conn_set **out);

        RetCode submit_sbs_evt_task(subscription_event_impl &sbs_evt,
                                    s_hm &connid_condesc_set);

        RetCode create_persistent_schema(PersistenceAlteringMode mode);

        RetCode nclass_create_persistent_schema(PersistenceAlteringMode mode,
                                                unsigned int entity_id);

        RetCode obj_load(unsigned short key,
                         unsigned int &ts0_out,
                         unsigned int &ts1_out,
                         nclass &in_out);

        RetCode obj_save(const nclass &in);

        RetCode obj_update(unsigned short key,
                           const nclass &in);

        RetCode obj_update_or_save(unsigned short key,
                                   const nclass &in);

        RetCode obj_remove(unsigned short key,
                           PersistenceDeletionMode mode,
                           const nclass &in);

        RetCode obj_distribute(SubscriptionEventType evt_type,
                               ProtocolCode proto_code,
                               Action act,
                               const nclass &in);

        RetCode obj_save_and_distribute(const nclass &in);

        RetCode obj_update_and_distribute(unsigned short key,
                                          const nclass &in);

        RetCode obj_update_or_save_and_distribute(unsigned short key,
                                                  const nclass &in);

        RetCode obj_remove_and_distribute(unsigned short key,
                                          PersistenceDeletionMode mode,
                                          const nclass &in);
    private:
        RetCode init();
        RetCode init_dyna();

    private:
        virtual const char *get_automa_name() override;
        virtual const unsigned int *get_automa_version() override;

        virtual RetCode on_automa_load_config(int pnum,
                                              const char *param,
                                              const char *value) override;

        virtual RetCode on_automa_early_init() override;
        virtual RetCode on_automa_init() override;
        virtual RetCode on_automa_start() override;
        virtual RetCode on_automa_stop() override;
        virtual RetCode on_automa_move_running() override;
        virtual void on_automa_error() override;

    public:
        PeerPersonality personality_;
        unsigned int srv_exectrs_;
        unsigned int cli_exectrs_;
        unsigned int srv_sbs_exectrs_;

        selector selector_;
        mutable mx mon_;
        unsigned int prgr_conn_id_;
        nentity_manager nem_;
        std::set<std::string> model_map_;

        bool pers_enabled_;
        persistence_manager_impl &pers_mng_;
        std::set<std::string> pers_dri_load_;
        bool pers_schema_create_;
        bool drop_existing_schema_;

    protected:
        //srv subscription executor service.
        p_exec_srv srv_sbs_exec_serv_;

        //nclassid --> condesc_set
        s_hm srv_sbs_nclassid_condesc_set_;

        //factory for incoming connections
        //factories cannot be references
        incoming_connection_factory *inco_conn_factory_;
};

}
