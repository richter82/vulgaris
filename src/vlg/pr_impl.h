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

#ifndef VLG_PEER_H_
#define VLG_PEER_H_
#include "pr_aut.h"
#include "selector.h"
#include "prs_impl.h"
#include "sbs_impl.h"

namespace vlg {

struct connection_impl;

class peer_recv_task_inco_conn : public vlg::p_tsk {
    public:
        peer_recv_task_inco_conn(std::shared_ptr<vlg::connection> &conn_sh,
                                 std::unique_ptr<vlg_hdr_rec> &pkt_hdr,
                                 std::unique_ptr<g_bbuf> &pkt_body);

        ~peer_recv_task_inco_conn();

        virtual RetCode execute();

    private:
        std::shared_ptr<vlg::connection> conn_sh_;
        std::unique_ptr<vlg_hdr_rec> pkt_hdr_;
        std::unique_ptr<g_bbuf> pkt_body_;
};

class peer_recv_task_outg_conn : public vlg::p_tsk {
    public:
        peer_recv_task_outg_conn(vlg::connection &conn,
                                 std::unique_ptr<vlg_hdr_rec> &pkt_hdr,
                                 std::unique_ptr<g_bbuf> &pkt_body);

        ~peer_recv_task_outg_conn();

        virtual RetCode execute();

    private:
        vlg::connection &conn_;
        std::unique_ptr<vlg_hdr_rec> pkt_hdr_;
        std::unique_ptr<g_bbuf> pkt_body_;
};

/***********************************
an helper class used to server peer
to generate a progressive
- per nclass_id - subscription event id.
It is also used to generate ts0, ts1.
***********************************/
struct per_nclass_id_conn_set {
    explicit per_nclass_id_conn_set();

    unsigned int next_sbs_evt_id();

    void next_time_stamp(unsigned int &ts0,
                         unsigned int &ts1);

    unsigned int sbsevtid_;
    unsigned int ts0_;
    unsigned int ts1_;
    vlg::s_hm connid_condesc_set_;  //[connid --> condesc]
    mutable vlg::mx mon_;
};

#define VLG_DEF_SRV_SBS_EXEC_NO     1
#define VLG_DEF_SRV_SBS_EXEC_Q_LEN  0

// peer_impl
struct peer_impl : public peer_automa {
        explicit peer_impl(peer &);

        RetCode set_params_file_dir(const char *dir);

        // CONFIG SETTERS
        void set_cfg_load_model(const char *model);
        void set_cfg_srv_sin_addr(const char *addr);
        void set_cfg_srv_sin_port(int port);
        void set_cfg_load_pers_driv(const char *driv);

        // MODEL
        RetCode extend_model(nentity_manager &nem);
        RetCode extend_model(const char *model_name);

        // CONNECTIVITY
        RetCode next_connid(unsigned int &connid);

        RetCode new_incoming_connection(std::shared_ptr<connection> &new_connection,
                                        unsigned int connid = 0);

        // SERVER SPECIFIC CONNECTIVITY HANDLER
        incoming_connection_factory &get_incoming_connection_factory() const;
        void set_incoming_connection_factory(incoming_connection_factory &conn_f);

        // PROTOCOL RCVNG INTERFACE
        /*
        this method is called by an executor
        */
        RetCode recv_and_route_pkt(vlg::connection &conn,
                                   vlg_hdr_rec *hdr,
                                   vlg::g_bbuf *body);

        RetCode recv_and_route_pkt(std::shared_ptr<vlg::connection> &inco_conn,
                                   vlg_hdr_rec *hdr,
                                   vlg::g_bbuf *body);

        // SUBSCRIPTION
        RetCode add_subscriber(subscription_impl *sbsdesc);
        RetCode remove_subscriber(subscription_impl *sbsdesc);

        RetCode get_per_nclassid_helper_rec(unsigned int nclass_id,
                                            per_nclass_id_conn_set **out);

        RetCode submit_sbs_evt_task(subscription_event_impl &sbs_evt,
                                    vlg::s_hm &connid_condesc_set);

        // PERSISTENCE
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

        // DISTRIBUTION
        RetCode obj_distribute(SubscriptionEventType evt_type,
                               ProtocolCode proto_code,
                               Action act,
                               const nclass &in);

        // PERSISTENCE + DISTRIBUTION
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

        // VLG_PEER_LFCYC HANDLERS
    private:
        virtual const char *get_automa_name()                       override;
        virtual const unsigned int *get_automa_version()            override;
        virtual RetCode on_automa_load_config(int pnum,
                                              const char *param,
                                              const char *value)    override;

        virtual RetCode    on_automa_early_init()      override;
        virtual RetCode    on_automa_init()            override;
        virtual RetCode    on_automa_start()           override;
        virtual RetCode    on_automa_stop()            override;
        virtual RetCode    on_automa_move_running()    override;
        virtual RetCode    on_automa_error()           override;
        virtual void       on_automa_dying_breath()    override;

    public:
        PeerPersonality personality_;
        unsigned int srv_exectrs_;
        unsigned int cli_exectrs_;
        unsigned int srv_sbs_exectrs_;

        selector selector_;
        mutable vlg::mx mon_;
        unsigned int prgr_conn_id_;
        nentity_manager nem_;
        std::set<std::string> model_map_;

        bool pers_enabled_;
        persistence_manager_impl &pers_mng_;
        std::set<std::string> pers_dri_load_;
        bool pers_schema_create_;
        bool drop_existing_schema_;

    protected:
        /***********************************
        NOTE:
        Responsability of srv_sbs_exec_serv_ executor service is limited to
        the synchro-management of BRAND-NEW-SBS-EVENTS for all started subscription.
        This means that actions such as: management of SBS-EVT-ACK and subsequent
        actions; management of triggering of initial-download for ALL-TYPE request
        is responsability of srv_exec_serv_ executor service.
        ***********************************/
        //srv subscription executor service.
        vlg::p_exec_srv srv_sbs_exec_serv_;
        //nclassid --> condesc_set
        vlg::s_hm srv_sbs_nclassid_condesc_set_;

        //factory for incoming connections
        //factories cannot be references
        incoming_connection_factory *inco_conn_factory_;
};

}

#endif
