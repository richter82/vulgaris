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

#ifndef BLZ_PEER_H_
#define BLZ_PEER_H_
#include "blz_peer_automa.h"
#include "blz_selector.h"
#include "blz_persistence_int.h"
#include "blz_subscription_int.h"


namespace blaze {

class connection_int;

/***********************************
an helper class used to server peer to generate a progressive - per nclass_id -
subscription event id.
It is also used to generate ts0, ts1.
***********************************/
class per_nclassid_helper_rec {
    public:
        per_nclassid_helper_rec();
        ~per_nclassid_helper_rec();

        blaze::RetCode init();

        blaze::synch_hash_map &get_srv_connid_condesc_set();

        unsigned int next_sbs_evt_id();

        void         next_time_stamp(unsigned int &ts0,
                                     unsigned int &ts1);

        //--rep
    private:
        unsigned int sbsevtid_;
        unsigned int ts0_;
        unsigned int ts1_;
        blaze::synch_hash_map srv_connid_condesc_set_;  //[connid --> condesc]
        mutable blaze::synch_monitor mon_;
};

#define BLZ_DEF_SRV_SBS_EXEC_NO     1
#define BLZ_DEF_SRV_SBS_EXEC_Q_LEN  0

//-----------------------------
// BLZ_PEER
//-----------------------------

typedef connection_int *(*blz_conn_factory)(peer_int &peer,
                                            ConnectionType con_type,
                                            unsigned int connid,
                                            void *ud);

class peer_int : public peer_automa {
        friend class selector;
        friend class peer_recv_task;
        friend class connection_int;
        friend class transaction_int;
        friend class subscription_int;

        /*************************************************************
        -Factory function types
        *************************************************************/

        static connection_int *blz_conn_factory_default(peer_int &peer,
                                                        ConnectionType con_type,
                                                        unsigned int connid,
                                                        void *ud);

    public:
        //---ctors
        peer_int(unsigned int id);
        ~peer_int();

        blaze::RetCode set_params_file_dir(const char *dir);

        //-----------------------------
        // GETTERS
        //-----------------------------
    public:
        selector                &get_selector();
        const entity_manager    &get_em() const;
        entity_manager          &get_em_m();
        persistence_manager_int &get_pers_mng();
        bool                    persistent();
        bool                    pers_schema_create();
        blaze::synch_hash_map   &get_srv_classid_condesc_set();

        //-----------------------------
        // CONFIG GETTERS
        //-----------------------------

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

        //-----------------------------
        // CONFIG SETTERS
        //-----------------------------

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

        //-----------------------------
        // MODEL
        //-----------------------------
        blaze::RetCode    extend_model(entity_manager *emng);
        blaze::RetCode    extend_model(const char *model_name);

        //-----------------------------
        // BLZ_PEER_LFCYC HANDLERS
        //-----------------------------
    protected:
        virtual blaze::RetCode    peer_load_cfg_usr(int pnum,
                                                    const char *param,
                                                    const char *value);

        virtual blaze::RetCode    peer_early_init_usr();
        virtual blaze::RetCode    peer_init_usr();
        virtual blaze::RetCode    peer_start_usr();
        virtual blaze::RetCode    peer_stop_usr();
        virtual blaze::RetCode    peer_move_running_usr();
        virtual blaze::RetCode    peer_dying_breath_handler();

        //-----------------------------
        // CONNECTIVITY
        //-----------------------------

    public:
        blaze::RetCode    next_connid(unsigned int &connid);

    public:
        blaze::RetCode    new_connection(connection_int     **new_connection,
                                         blz_conn_factory   blz_conn_factory_f = NULL,
                                         ConnectionType     con_type = ConnectionType_OUTGOING,
                                         unsigned int       connid = 0,
                                         void               *ud = NULL);

        blaze::RetCode    release_connection(connection_int *connection);

        //-----------------------------
        // SERVER SPECIFIC CONNECTIVITY HANDLER
        //-----------------------------
    public:
        virtual blaze::RetCode new_incoming_connection_accept(connection_int
                                                              &incoming_connection);

        blz_conn_factory conn_factory() const;
        void set_conn_factory(blz_conn_factory val);

        void *conn_factory_ud() const;
        void set_conn_factory_ud(void *val);

        //-----------------------------
        // PROTOCOL RCVNG INTERFACE
        //-----------------------------
    private:
        /*
        this method is called by an executor
        */
        blaze::RetCode recv_and_route_pkt(connection_int &conn,
                                          blz_hdr_rec *hdr,
                                          blaze::grow_byte_buffer *body);

        //-----------------------------
        // RCV TASK
        //-----------------------------
    private:
        /*
        this method is called by selector thread.
        */
        blaze::p_task  *new_peer_recv_task(connection_int &conn,
                                           blz_hdr_rec *pkt_hdr,
                                           blaze::grow_byte_buffer *pkt_body);

        //-----------------------------
        // SBS TASK
        //-----------------------------
        /*
        this method is called by a client/server executor thread.
        */
        blaze::p_task  *new_sbs_evt_task(subscription_event_int &sbs_evt,
                                         blaze::synch_hash_map &srv_connid_connection_map);

        //-----------------------------
        // SUBSCRIPTION
        //-----------------------------
    private:
        blaze::RetCode    build_sbs_event(unsigned int evt_id,
                                          SubscriptionEventType sbs_evttype,
                                          ProtocolCode sbs_protocode,
                                          unsigned int sbs_tmstp0,
                                          unsigned int sbs_tmstp1,
                                          Action sbs_act,
                                          const nclass *sbs_data,
                                          subscription_event_int **new_sbs_event);

        blaze::RetCode    add_subscriber(subscription_int *sbsdesc);

        blaze::RetCode    remove_subscriber(subscription_int *sbsdesc);

        blaze::RetCode    get_per_classid_helper_class(unsigned int nclass_id,
                                                       per_nclassid_helper_rec **out);

        blaze::RetCode    submit_sbs_evt_task(subscription_event_int &sbs_evt,
                                              blaze::synch_hash_map &srv_connid_condesc_set);


        //-----------------------------
        // PERSISTENCE
        //-----------------------------
    public:
        blaze::RetCode pers_schema_create(PersistenceAlteringMode mode);

        blaze::RetCode class_pers_schema_create(PersistenceAlteringMode mode,
                                                unsigned int entity_id);

        blaze::RetCode class_pers_load(unsigned short key,
                                       unsigned int &ts0_out,
                                       unsigned int &ts1_out,
                                       nclass &in_out_obj);

        blaze::RetCode class_pers_save(const nclass &in_obj);

        blaze::RetCode class_pers_update(unsigned short key,
                                         const nclass &in_obj);

        blaze::RetCode class_pers_update_or_save(unsigned short key,
                                                 const nclass &in_obj);

        blaze::RetCode class_pers_remove(unsigned short key,
                                         PersistenceDeletionMode mode,
                                         const nclass &in_obj);


        //-----------------------------
        // DISTRIBUTION
        //-----------------------------
    public:
        blaze::RetCode class_distribute(SubscriptionEventType evt_type,
                                        ProtocolCode proto_code,
                                        Action act,
                                        const nclass &obj);

        //-----------------------------
        // PERSISTENCE + DISTRIBUTION
        //-----------------------------
    public:
        blaze::RetCode class_pers_save_and_distribute(const nclass &in_obj);

        blaze::RetCode class_pers_update_and_distribute(unsigned short key,
                                                        const nclass &in_obj);

        blaze::RetCode class_pers_update_or_save_and_distribute(unsigned short key,
                                                                const nclass &in_obj);

        blaze::RetCode class_pers_remove_and_distribute(unsigned short key,
                                                        PersistenceDeletionMode mode,
                                                        const nclass &in_obj);

        //-----------------------------
        // REP INIT
        //-----------------------------
    private:
        blaze::RetCode            init_peer();
        blaze::RetCode            init_peer_dyna();

        //-----------------------------
        // REP
        //-----------------------------
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
        selector                        selector_;
        mutable blaze::synch_monitor    mon_;
        unsigned int                    prgr_conn_id_;
        entity_manager                  bem_;
        blaze::hash_map                 model_map_;

        //persistence
    protected:
        bool                    pers_enabled_;
        persistence_manager_int &pers_mng_;
        blaze::hash_map         pers_dri_load_;
        bool                    pers_schema_create_;
        /*use this only when develop*/
        bool                    drop_existing_schema_;

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
        blaze::p_executor_service   srv_sbs_exec_serv_;
        //nclassid --> condesc_set
        blaze::synch_hash_map       srv_sbs_nclassid_condesc_set_;

        blz_conn_factory    conn_factory_;      //factory for incoming connections
        void                *conn_factory_ud_;  //factory for incoming connections ud
    public:
        static nclass_logger *logger();

    protected:
        static nclass_logger *log_;
};

}

#endif
