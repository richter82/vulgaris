/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#import "vlg_objc.h"

/************************************************************************
 PEER
 ************************************************************************/

void peer_on_status_change_c(peer *p, PeerStatus status, void *ud, void *ud2)
{
    return ((__bridge peer_on_status_change_oc)ud)(p, status, ud2);
}

const char *peer_name_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_name_oc)ud)(p, ud2);
}

const unsigned int *peer_version_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_version_oc)ud)(p, ud2);
}

RetCode peer_on_load_config_c(peer *p,
                              int pnum,
                              const char *param,
                              const char *value,
                              void *ud, void *ud2)
{
    return ((__bridge peer_on_load_config_oc)ud)(p, pnum, param, value, ud2);
}

RetCode peer_on_init_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_on_init_oc)ud)(p, ud2);
}

RetCode peer_on_starting_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_on_starting_oc)ud)(p, ud2);
}

RetCode peer_on_stopping_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_on_stopping_oc)ud)(p, ud2);
}

RetCode peer_on_move_running_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_on_move_running_oc)ud)(p, ud2);
}

void peer_on_dying_breath_c(peer *p, void *ud, void *ud2)
{
    ((__bridge peer_on_dying_breath_oc)ud)(p, ud2);
}

RetCode peer_on_incoming_connection_c(peer *p,
                                      shr_incoming_connection *ic,
                                      void *ud, void *ud2)
{
    return ((__bridge peer_on_incoming_connection_oc)ud)(p, ic, ud2);
}

void peer_set_name_oc(peer *p, peer_name_oc hndl, void *ud)
{
    peer_set_name(p,
                  peer_name_c,
                  (__bridge_retained void *)hndl,
                  ud);
}

void peer_set_version_oc(peer *p, peer_version_oc hndl, void *ud)
{
    peer_set_version(p,
                     peer_version_c,
                     (__bridge_retained void *)hndl,
                     ud);
}

void peer_set_on_load_config_oc(peer *p,
                                peer_on_load_config_oc hndl,
                                void *ud)
{
    peer_set_on_load_config(p,
                            peer_on_load_config_c,
                            (__bridge_retained void *)hndl,
                            ud);
}

void peer_set_on_init_oc(peer *p,
                         peer_on_init_oc hndl,
                         void *ud)
{
    peer_set_on_init(p,
                     peer_on_init_c,
                     (__bridge_retained void *)hndl,
                     ud);
}

void peer_set_on_starting_oc(peer *p,
                             peer_on_starting_oc hndl,
                             void *ud)
{
    peer_set_on_starting(p,
                         peer_on_starting_c,
                         (__bridge_retained void *)hndl,
                         ud);
}

void peer_set_on_stopping_oc(peer *p,
                             peer_on_stopping_oc hndl,
                             void *ud)
{
    peer_set_on_stopping(p,
                         peer_on_stopping_c,
                         (__bridge_retained void *)hndl,
                         ud);
}

void peer_set_on_move_running_oc(peer *p,
                                 peer_on_move_running_oc hndl,
                                 void *ud)
{
    peer_set_on_move_running(p,
                             peer_on_move_running_c,
                             (__bridge_retained void *)hndl,
                             ud);
}

void peer_set_on_dying_breath_oc(peer *p,
                                 peer_on_dying_breath_oc hndl,
                                 void *ud)
{
    peer_set_on_dying_breath(p,
                             peer_on_dying_breath_c,
                             (__bridge_retained void *)hndl,
                             ud);
}

void peer_set_on_status_change_oc(peer *p,
                                  peer_on_status_change_oc hndl,
                                  void *ud)
{
    peer_set_on_status_change(p,
                              peer_on_status_change_c,
                              (__bridge_retained void *)hndl,
                              ud);
}

void peer_set_on_incoming_connection_oc(peer *p,
                                        peer_on_incoming_connection_oc hndl,
                                        void *ud)
{
    peer_set_on_incoming_connection(p,
                                    peer_on_incoming_connection_c,
                                    (__bridge_retained void *)hndl,
                                    ud);
}

/************************************************************************
 INCOMING CONNECTION
 ************************************************************************/

void inco_connection_status_change_c(incoming_connection *conn,
                                     ConnectionStatus status,
                                     void *ud, void *ud2)
{
    inco_connection_on_status_change_oc icscs = (__bridge id)ud;
    icscs(conn, status, NULL);
}

void inco_connection_on_disconnect_c(incoming_connection *conn,
                                     ConnectivityEventResult con_evt_res,
                                     ConnectivityEventType c_evt_type,
                                     void *ud,
                                     void *ud2)
{
    ((__bridge inco_connection_on_disconnect_oc)ud)(conn,
                                                    con_evt_res,
                                                    c_evt_type,
                                                    ud2);
}

RetCode inco_connection_on_incoming_transaction_c(incoming_connection *conn,
                                                  shr_incoming_transaction *itx,
                                                  void *ud,
                                                  void *ud2)
{
    return ((__bridge inco_connection_on_incoming_transaction_oc)ud)(conn,
                                                                     itx,
                                                                     ud2);
}

RetCode inco_connection_on_incoming_subscription_c(incoming_connection *conn,
                                                   shr_incoming_subscription *isbs,
                                                   void *ud,
                                                   void *ud2)
{
    return ((__bridge inco_connection_on_incoming_subscription_oc)ud)(conn,
                                                                      isbs,
                                                                      ud2);
}

void inco_connection_on_releaseable_c(incoming_connection *conn,
                                      void *ud,
                                      void *ud2)
{
    return ((__bridge inco_connection_on_releaseable_oc)ud)(conn, ud2);
}

void inco_connection_set_on_status_change_oc(incoming_connection *ic,
                                             inco_connection_on_status_change_oc hndl,
                                             void *ud)
{

    inco_connection_set_on_status_change(ic,
                                         inco_connection_status_change_c,
                                         (__bridge_retained void *)hndl,
                                         ud);
}

void inco_connection_set_on_disconnect_oc(incoming_connection *ic,
                                          inco_connection_on_disconnect_oc hndl,
                                          void *ud)
{
    inco_connection_set_on_disconnect(ic,
                                      inco_connection_on_disconnect_c,
                                      (__bridge_retained void *)hndl,
                                      ud);
}

void inco_connection_set_on_incoming_transaction_oc(incoming_connection *ic,
                                                    inco_connection_on_incoming_transaction_oc hndl,
                                                    void *ud)
{
    inco_connection_set_on_incoming_transaction(ic,
                                                inco_connection_on_incoming_transaction_c,
                                                (__bridge_retained void *)hndl,
                                                ud);
}

void inco_connection_set_on_incoming_subscription_oc(incoming_connection *ic,
                                                     inco_connection_on_incoming_subscription_oc hndl,
                                                     void *ud)
{
    inco_connection_set_on_incoming_subscription(ic,
                                                 inco_connection_on_incoming_subscription_c,
                                                 (__bridge_retained void *)hndl,
                                                 ud);
}

void inco_connection_set_on_releaseable_oc(incoming_connection *ic,
                                           inco_connection_on_releaseable_oc hndl,
                                           void *ud)
{
    inco_connection_set_on_releaseable(ic,
                                       inco_connection_on_releaseable_c,
                                       (__bridge_retained void *)hndl,
                                       ud);
}


/************************************************************************
 OUTGOING CONNECTION
 ************************************************************************/

void outg_connection_status_change_c(outgoing_connection *oc,
                                     ConnectionStatus status,
                                     void *ud,
                                     void *ud2)
{
    ((__bridge outg_connection_on_status_change_oc)ud)(oc, status, ud2);
}

void outg_connection_on_connect_c(outgoing_connection *oc,
                                  ConnectivityEventResult con_evt_res,
                                  ConnectivityEventType c_evt_type,
                                  void *ud,
                                  void *ud2)
{
    ((__bridge outg_connection_on_connect_oc)ud)(oc,
                                                 con_evt_res,
                                                 c_evt_type,
                                                 ud2);
}

void outg_connection_on_disconnect_c(outgoing_connection *oc,
                                     ConnectivityEventResult con_evt_res,
                                     ConnectivityEventType c_evt_type,
                                     void *ud,
                                     void *ud2)
{
    ((__bridge outg_connection_on_disconnect_oc)ud)(oc,
                                                    con_evt_res,
                                                    c_evt_type,
                                                    ud2);
}

void outg_connection_set_on_status_change_oc(outgoing_connection *oc,
                                             outg_connection_on_status_change_oc hndl,
                                             void *ud)
{
    outg_connection_set_on_status_change(oc,
                                         outg_connection_status_change_c,
                                         (__bridge_retained void *)hndl,
                                         ud);
}

void outg_connection_set_on_connect_oc(outgoing_connection *oc,
                                       outg_connection_on_connect_oc hndl,
                                       void *ud)
{
    outg_connection_set_on_connect(oc,
                                   outg_connection_on_connect_c,
                                   (__bridge_retained void *)hndl,
                                   ud);
}

void outg_connection_set_on_disconnect_oc(outgoing_connection *oc,
                                          outg_connection_on_disconnect_oc hndl,
                                          void *ud)
{
    outg_connection_set_on_disconnect(oc,
                                      outg_connection_on_disconnect_c,
                                      (__bridge_retained void *)hndl,
                                      ud);
}

/************************************************************************
 INCOMING TRANSACTION
 ************************************************************************/

void inco_transaction_status_change_c(incoming_transaction *tx,
                                      TransactionStatus status,
                                      void *ud, void *ud2)
{
    ((__bridge inco_transaction_on_status_change_oc)ud)(tx, status, ud2);
}

void inco_transaction_request_c(incoming_transaction *tx, void *ud, void *ud2)
{
    ((__bridge inco_transaction_on_request_oc)ud)(tx, ud2);
}

void inco_transaction_closure_c(incoming_transaction *tx, void *ud, void *ud2)
{
    ((__bridge inco_transaction_on_closure_oc)ud)(tx, ud2);
}

void inco_transaction_on_releaseable_c(incoming_transaction *tx,
                                       void *ud,
                                       void *ud2)
{
    ((__bridge inco_transaction_on_releaseable_oc)ud)(tx, ud2);
}

void inco_transaction_set_on_status_change_oc(incoming_transaction *tx,
                                              inco_transaction_on_status_change_oc hndl,
                                              void *ud)
{
    inco_transaction_set_on_status_change(tx,
                                          inco_transaction_status_change_c,
                                          (__bridge_retained void *)hndl,
                                          ud);
}

void inco_transaction_set_on_transaction_closure_oc(incoming_transaction *tx,
                                                    inco_transaction_on_closure_oc hndl,
                                                    void *ud)
{
    inco_transaction_set_on_closure(tx,
                                    inco_transaction_closure_c,
                                    (__bridge_retained void *)hndl,
                                    ud);
}

void inco_transaction_set_on_request_oc(incoming_transaction *tx,
                                        inco_transaction_on_request_oc hndl,
                                        void *ud)
{
    inco_transaction_set_on_request(tx,
                                    inco_transaction_request_c,
                                    (__bridge_retained void *)hndl,
                                    ud);
}

void inco_transaction_set_on_releaseable_oc(incoming_transaction *tx,
                                            inco_transaction_on_releaseable_oc hndl,
                                            void *ud)
{
    inco_transaction_set_on_releaseable(tx,
                                        inco_transaction_on_releaseable_c,
                                        (__bridge_retained void *)hndl,
                                        ud);

}


/************************************************************************
 OUTGOING TRANSACTION
 ************************************************************************/

void outg_transaction_status_change_c(outgoing_transaction *tx,
                                      TransactionStatus status,
                                      void *ud, void *ud2)
{
    ((__bridge outg_transaction_on_status_change_oc)ud)(tx, status, ud2);
}

void outg_transaction_closure_c(outgoing_transaction *tx,
                                void *ud, void *ud2)
{
    ((__bridge outg_transaction_on_closure_oc)ud)(tx, ud2);
}

void outg_transaction_set_on_status_change_oc(outgoing_transaction *tx,
                                              outg_transaction_on_status_change_oc hndl,
                                              void *ud)
{
    outg_transaction_set_on_status_change(tx,
                                          outg_transaction_status_change_c,
                                          (__bridge_retained void *)hndl,
                                          ud);
}

void outg_transaction_set_on_closure_oc(outgoing_transaction *tx,
                                        outg_transaction_on_closure_oc hndl,
                                        void *ud)
{
    outg_transaction_set_on_closure(tx,
                                    outg_transaction_closure_c,
                                    (__bridge_retained void *)hndl,
                                    ud);
}

/************************************************************************
 INCOMING SUBSCRIPTION
 ************************************************************************/

void inco_subscription_status_change_c(incoming_subscription *isbs,
                                       SubscriptionStatus status,
                                       void *ud,
                                       void *ud2)
{
    ((__bridge inco_subscription_on_status_change_oc)ud)(isbs, status, ud2);
}

RetCode inco_subscription_accept_distribution_c(incoming_subscription *isbs,
                                                const subscription_event *sbs_evt,
                                                void *ud,
                                                void *ud2)
{
    return ((__bridge inco_subscription_on_accept_distribution_oc)ud)(isbs,
                                                                      sbs_evt,
                                                                      ud2);
}

void inco_subscription_on_stop_c(incoming_subscription *isbs,
                                 void *ud,
                                 void *ud2)
{
    ((__bridge inco_subscription_on_stop_oc)ud)(isbs, ud2);
}

void inco_subscription_on_releaseable_c(incoming_subscription *isbs,
                                        void *ud,
                                        void *ud2)
{
    ((__bridge inco_subscription_on_releaseable_oc)ud)(isbs, ud2);
}

void inco_subscription_set_on_status_change_oc(incoming_subscription *sbs,
                                               inco_subscription_on_status_change_oc hndl,
                                               void *ud)
{
    inco_subscription_set_on_status_change(sbs,
                                           inco_subscription_status_change_c,
                                           (__bridge_retained void *)hndl,
                                           ud);
}

void inco_subscription_set_on_accept_distribution_oc(incoming_subscription *sbs,
                                                     inco_subscription_on_accept_distribution_oc hndl,
                                                     void *ud)
{
    inco_subscription_set_on_accept_distribution(sbs,
                                                 inco_subscription_accept_distribution_c,
                                                 (__bridge_retained void *)hndl,
                                                 ud);
}

void inco_subscription_set_on_stop_oc(incoming_subscription *sbs,
                                      inco_subscription_on_stop_oc hndl,
                                      void *ud)
{
    inco_subscription_set_on_stop(sbs,
                                  inco_subscription_on_stop_c,
                                  (__bridge_retained void *)hndl,
                                  ud);
}

void inco_subscription_set_on_releaseable_oc(incoming_subscription *sbs,
                                             inco_subscription_on_releaseable_oc hndl,
                                             void *ud)
{
    inco_subscription_set_on_releaseable(sbs,
                                         inco_subscription_on_releaseable_c,
                                         (__bridge_retained void *)hndl,
                                         ud);
}

/************************************************************************
 OUTGOING SUBSCRIPTION
 ************************************************************************/

void outg_subscription_status_change_c(outgoing_subscription *osbs,
                                       SubscriptionStatus status,
                                       void *ud,
                                       void *ud2)
{
    ((__bridge outg_subscription_on_status_change_oc)ud)(osbs, status, ud2);
}

void outg_subscription_notify_event_c(outgoing_subscription *osbs,
                                      const subscription_event *sbs_evt,
                                      void *ud,
                                      void *ud2)
{
    ((__bridge outg_subscription_on_notify_event_oc)ud)(osbs, sbs_evt, ud2);
}

void outg_subscription_on_start_c(outgoing_subscription *osbs,
                                  void *ud,
                                  void *ud2)
{
    ((__bridge outg_subscription_on_start_oc)ud)(osbs, ud2);
}

void outg_subscription_on_stop_c(outgoing_subscription *osbs,
                                 void *ud,
                                 void *ud2)
{
    ((__bridge outg_subscription_on_stop_oc)ud)(osbs, ud2);
}

void outg_subscription_set_on_status_change_oc(outgoing_subscription *sbs,
                                               outg_subscription_on_status_change_oc hndl,
                                               void *ud)
{
    outg_subscription_set_on_status_change(sbs,
                                           outg_subscription_status_change_c,
                                           (__bridge_retained void *)hndl,
                                           ud);
}

void outg_subscription_set_on_event_notify_oc(outgoing_subscription *sbs,
                                              outg_subscription_on_notify_event_oc hndl,
                                              void *ud)
{
    outg_subscription_set_on_event_notify(sbs,
                                          outg_subscription_notify_event_c,
                                          (__bridge_retained void *)hndl,
                                          ud);
}

void outg_subscription_set_on_start_oc(outgoing_subscription *sbs,
                                       outg_subscription_on_start_oc hndl,
                                       void *ud)
{
    outg_subscription_set_on_start(sbs,
                                   outg_subscription_on_start_c,
                                   (__bridge_retained void *)hndl,
                                   ud);
}

void outg_subscription_set_on_stop_oc(outgoing_subscription *sbs,
                                      outg_subscription_on_stop_oc hndl,
                                      void *ud)
{
    outg_subscription_set_on_stop(sbs,
                                  outg_subscription_on_stop_c,
                                  (__bridge_retained void *)hndl,
                                  ud);
}
