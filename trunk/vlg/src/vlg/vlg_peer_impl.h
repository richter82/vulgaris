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
#include "vlg_peer_automa.h"
#include "vlg_selector.h"
#include "vlg_pers_impl.h"
#include "vlg_subscription_impl.h"

namespace vlg {

class connection_impl;

/***********************************
an helper class used to server peer to generate a progressive - per nclass_id -
subscription event id.
It is also used to generate ts0, ts1.
***********************************/
class per_nclassid_helper_rec {
    public:
        per_nclassid_helper_rec();
        ~per_nclassid_helper_rec();

        RetCode init();

        vlg::synch_hash_map &get_srv_connid_condesc_set();

        unsigned int next_sbs_evt_id();

        void         next_time_stamp(unsigned int &ts0,
                                     unsigned int &ts1);
        //--rep
    private:
        unsigned int sbsevtid_;
        unsigned int ts0_;
        unsigned int ts1_;
        vlg::synch_hash_map srv_connid_condesc_set_;  //[connid --> condesc]
        mutable vlg::synch_monitor mon_;
};

#define VLG_DEF_SRV_SBS_EXEC_NO     1
#define VLG_DEF_SRV_SBS_EXEC_Q_LEN  0

typedef connection_impl *(*connection_impl_factory)(peer_impl &peer,
                                                    ConnectionType con_type,
                                                    unsigned int connid,
                                                    void *ud);
// peer_impl
class peer_impl : public peer_automa {
        friend class selector;
        friend class peer_recv_task;
        friend class connection_impl;
        friend class transaction_impl;
        friend class subscription_impl;

    public:
        peer_impl(peer &publ,
                  unsigned int id);
        ~peer_impl();

    public:
        peer &get_public();

        RetCode set_params_file_dir(const char *dir);

        // GETTERS
    public:
        selector                &get_selector();
        const nentity_manager       &get_nem() const;
        nentity_manager             &get_nem_not_const();
        persistence_manager_impl    &get_persistence_manager();
        vlg::synch_hash_map         &get_srv_nclassid_condesc_set();

        // CONFIG GETTERS
        PeerPersonality         get_cfg_personality();
        sockaddr_in             get_cfg_srv_sock_addr();
        unsigned int            get_cfg_srv_pkt_q_len();
        unsigned int            get_cfg_srv_exectrs();
        unsigned int            get_cfg_cli_pkt_q_len();
        unsigned int            get_cfg_cli_exectrs();
        unsigned int            get_cfg_srv_sbs_evt_q_len();
        unsigned int            get_cfg_srv_sbs_exectrs();
        bool                    get_cfg_pers_enabled();
        bool                    get_cfg_pers_schema_create();
        bool                    get_cfg_drop_existing_schema();

        // CONFIG SETTERS
        void set_cfg_personality(PeerPersonality personality);
        void set_cfg_load_model(const char *model);
        void set_cfg_srv_sin_addr(const char *addr);
        void set_cfg_srv_sin_port(int port);
        void set_cfg_srv_pkt_q_len(unsigned int srv_pkt_q_len);
        void set_cfg_srv_exectrs(unsigned int srv_exectrs);
        void set_cfg_cli_pkt_q_len(unsigned int cli_pkt_q_len);
        void set_cfg_cli_exectrs(unsigned int cli_exectrs);
        void set_cfg_srv_sbs_evt_q_len(unsigned int srv_sbs_evt_q_len);
        void set_cfg_srv_sbs_exectrs(unsigned int srv_sbs_exectrs);
        void set_cfg_pers_enabled(bool pers_enabled);
        void set_cfg_pers_schema_create(bool pers_schema_create);
        void set_cfg_drop_existing_schema(bool drop_existing_schema);
        void set_cfg_load_pers_driv(const char *driv);

        // MODEL
        RetCode    extend_model(nentity_manager *nem);
        RetCode    extend_model(const char *model_name);

        // VLG_PEER_LFCYC HANDLERS
    protected:
        virtual RetCode    load_config_handler(int pnum,
                                               const char *param,
                                               const char *value);

        virtual RetCode    on_early_init()      override;
        virtual RetCode    on_init()            override;
        virtual RetCode    on_start()           override;
        virtual RetCode    on_stop()            override;
        virtual RetCode    on_move_running()    override;
        virtual RetCode    on_error()           override;
        virtual RetCode    on_dying_breath()    override;

        // CONNECTIVITY
    public:
        RetCode    next_connid(unsigned int &connid);

    public:
        RetCode    new_connection(connection_impl **new_connection,
                                  connection_impl_factory vlg_conn_factory_f = nullptr,
                                  ConnectionType con_type = ConnectionType_OUTGOING,
                                  unsigned int connid = 0,
                                  void *ud = nullptr);

        RetCode    release_connection(connection_impl *connection);

        // SERVER SPECIFIC CONNECTIVITY HANDLER
    public:
        virtual RetCode on_incoming_connection(connection_impl
                                               &incoming_connection);

        connection_impl_factory get_connection_impl_factory() const;
        void set_connection_impl_factory(connection_impl_factory conn_f);

        void *get_connection_impl_factory_ud() const;
        void set_connection_impl_factory_ud(void *ud);

        // PROTOCOL RCVNG INTERFACE
    private:
        /*
        this method is called by an executor
        */
        RetCode recv_and_route_pkt(connection_impl &conn,
                                   vlg_hdr_rec *hdr,
                                   vlg::grow_byte_buffer *body);

        // RCV TASK
    private:
        /*
        this method is called by selector thread.
        */
        vlg::p_task  *new_peer_recv_task(connection_impl &conn,
                                         vlg_hdr_rec *pkt_hdr,
                                         vlg::grow_byte_buffer *pkt_body);

        // SBS TASK
        /*
        this method is called by a client/server executor thread.
        */
        vlg::p_task  *new_sbs_evt_task(subscription_event_impl &sbs_evt,
                                       vlg::synch_hash_map &srv_connid_connection_map);

        // SUBSCRIPTION
    private:
        RetCode    build_sbs_event(unsigned int evt_id,
                                   SubscriptionEventType sbs_evttype,
                                   ProtocolCode sbs_protocode,
                                   unsigned int sbs_tmstp0,
                                   unsigned int sbs_tmstp1,
                                   Action sbs_act,
                                   const nclass *sbs_data,
                                   subscription_event_impl **new_sbs_event);

        RetCode    add_subscriber(subscription_impl *sbsdesc);

        RetCode    remove_subscriber(subscription_impl *sbsdesc);

        RetCode    get_per_nclassid_helper_rec(unsigned int nclass_id,
                                               per_nclassid_helper_rec **out);

        RetCode    submit_sbs_evt_task(subscription_event_impl &sbs_evt,
                                       vlg::synch_hash_map &srv_connid_condesc_set);

        // PERSISTENCE
    public:
        RetCode create_persistent_schema(PersistenceAlteringMode mode);

        RetCode nclass_create_persistent_schema(PersistenceAlteringMode mode,
                                                unsigned int entity_id);

        RetCode obj_load(unsigned short key,
                         unsigned int &ts0_out,
                         unsigned int &ts1_out,
                         nclass &in_out_obj);

        RetCode obj_save(const nclass &in_obj);

        RetCode obj_update(unsigned short key,
                           const nclass &in_obj);

        RetCode obj_update_or_save(unsigned short key,
                                   const nclass &in_obj);

        RetCode obj_remove(unsigned short key,
                           PersistenceDeletionMode mode,
                           const nclass &in_obj);

        // DISTRIBUTION
    public:
        RetCode obj_distribute(SubscriptionEventType evt_type,
                               ProtocolCode proto_code,
                               Action act,
                               const nclass &obj);

        // PERSISTENCE + DISTRIBUTION
    public:
        RetCode obj_save_and_distribute(const nclass &in_obj);

        RetCode obj_update_and_distribute(unsigned short key,
                                          const nclass &in_obj);

        RetCode obj_update_or_save_and_distribute(unsigned short key,
                                                  const nclass &in_obj);

        RetCode obj_remove_and_distribute(unsigned short key,
                                          PersistenceDeletionMode mode,
                                          const nclass &in_obj);

        // REP INIT
    private:
        RetCode            init();
        RetCode            init_dyna();

        // REP
    protected:
        PeerPersonality         personality_;
        unsigned int            srv_pkt_q_len_;
        unsigned int            srv_exectrs_;
        unsigned int            cli_pkt_q_len_;
        unsigned int            cli_exectrs_;
        unsigned int            tskid_;
        //sbs
        unsigned int            srv_sbs_evt_q_len_;
        unsigned int            srv_sbs_exectrs_;

    protected:
        selector                    selector_;
        mutable vlg::synch_monitor  mon_;
        unsigned int                prgr_conn_id_;
        nentity_manager              nem_;
        vlg::hash_map               model_map_;

        //persistence
    protected:
        bool                        pers_enabled_;
        persistence_manager_impl    &pers_mng_;
        vlg::hash_map               pers_dri_load_;
        bool                        pers_schema_create_;
        bool                        drop_existing_schema_;

        //sbs srv rep
    protected:
        /***********************************
        NOTE:
        Responsability of srv_sbs_exec_serv_ executor service is limited to
        the synchro-management of BRAND-NEW-SBS-EVENTS for all started sbs.
        This means that actions such as: management of SBS-EVT-ACK and subsequent
        actions; management of triggering of initial-download for ALL-TYPE request
        is responsability of srv_exec_serv_ executor service.
        ***********************************/
        //srv sbs executor service.
        vlg::p_executor_service   srv_sbs_exec_serv_;
        //nclassid --> condesc_set
        vlg::synch_hash_map       srv_sbs_nclassid_condesc_set_;

        connection_impl_factory     conn_impl_factory_;      //factory for incoming connections
        void                        *conn_impl_factory_ud_;  //factory for incoming connections ud

    private:
        peer &publ_;

    public:
        static nclass_logger *logger();

    protected:
        static nclass_logger *log_;
};

}

#endif
