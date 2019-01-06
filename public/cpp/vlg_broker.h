/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "vlg.h"

namespace vlg {

/** @brief broker listener.
*/
struct broker_listener {
    virtual RetCode on_load_config(broker &,
                                   int pnum,
                                   const char *param,
                                   const char *value) = 0;

    virtual RetCode on_init(broker &) = 0;
    virtual RetCode on_starting(broker &) = 0;
    virtual RetCode on_stopping(broker &) = 0;
    virtual RetCode on_move_running(broker &) = 0;
    virtual void on_error(broker &) = 0;
    virtual void on_status_change(broker &, PeerStatus) = 0;

    /**
    @return default implementation always returns RetCode_OK,
    so all incoming will be accepted.
    */
    virtual RetCode on_incoming_connection(broker &, std::shared_ptr<incoming_connection> &) = 0;

    static broker_listener &default_listener();
};

/** @brief broker.
*/
struct broker {
    explicit broker(broker_listener &listener = broker_listener::default_listener());
    virtual ~broker();

    RetCode set_params_file_dir(const char *);
    RetCode set_params_file_path_name(const char *);

    unsigned int get_version_major();
    unsigned int get_version_minor();
    unsigned int get_version_maintenance();
    unsigned int get_version_architecture();
    bool is_configured();
    const nentity_manager &get_nentity_manager() const;
    bool is_persistent();
    bool is_create_persistent_schema();
    bool is_drop_existing_persistent_schema();

    PeerPersonality get_personality();
    sockaddr_in get_server_sockaddr();
    unsigned int get_server_transaction_service_executor_size();
    unsigned int get_server_subscription_service_executor_size();
    unsigned int get_client_transaction_service_executor_size();

    void set_personality(PeerPersonality);
    void set_server_address(const char *);
    void set_server_port(int);
    void set_server_transaction_service_executor_size(unsigned int);
    void set_server_subscription_service_executor_size(unsigned int);
    void set_client_transaction_service_executor_size(unsigned int);
    void set_persistent(bool);
    void set_create_persistent_schema(bool);
    void set_drop_existing_persistent_schema(bool);
    void set_configured(bool);
    void add_load_model(const char *);
    void add_load_persistent_driver(const char *);

    RetCode extend_model(nentity_manager &nem);
    RetCode extend_model(const char *model_name);

    virtual const char *get_name() = 0;
    virtual const unsigned int *get_version() = 0;

    PeerStatus get_status();

    RetCode await_for_status_reached(PeerStatus test,
                                     PeerStatus &current,
                                     time_t sec = -1,
                                     long nsec = 0);

    RetCode await_for_status_change(PeerStatus &broker_status,
                                    time_t sec = -1,
                                    long nsec = 0);

    incoming_connection_factory &get_incoming_connection_factory();
    void set_incoming_connection_factory(incoming_connection_factory &);

    RetCode start(int argc,
                  char *argv[],
                  bool broker_automa_on_new_thread);

    RetCode stop(bool force_disconnect = false);

    RetCode create_persistent_schema(PersistenceAlteringMode);

    RetCode nclass_create_persistent_schema(PersistenceAlteringMode mode,
                                            unsigned int nclass_id);

    RetCode obj_load(unsigned short nclass_key,
                     unsigned int &ts_0_out,
                     unsigned int &ts_1_out,
                     nclass &in_out);

    RetCode obj_save(const nclass &in);

    RetCode obj_update(unsigned short nclass_key,
                       const nclass &in);

    RetCode obj_update_or_save(unsigned short nclass_key,
                               const nclass &in);

    RetCode obj_remove(unsigned short nclass_key,
                       PersistenceDeletionMode mode,
                       const nclass &in);

    RetCode obj_distribute(SubscriptionEventType event_type,
                           Action action,
                           const nclass &in);

    RetCode obj_save_and_distribute(const nclass &in);

    RetCode obj_update_and_distribute(unsigned short nclass_key,
                                      const nclass &in);

    RetCode obj_update_or_save_and_distribute(unsigned short nclass_key,
                                              const nclass &in);

    RetCode obj_remove_and_distribute(unsigned short nclass_key,
                                      PersistenceDeletionMode mode,
                                      const nclass &in);

    std::unique_ptr<broker_impl> impl_;
};

}
