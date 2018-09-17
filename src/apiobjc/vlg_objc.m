/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#import "vlg_objc.h"

/************************************************************************
 PEER
 ************************************************************************/

void peer_status_change_c(peer *p, PeerStatus status, void *ud, void *ud2)
{
    return ((__bridge peer_status_change_swf)ud)(p, status, ud2);
}

const char *peer_name_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_name_swf)ud)(p, ud2);
}

const unsigned int *peer_version_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_version_swf)ud)(p, ud2);
}

RetCode peer_load_config_c(peer *p, int pnum, const char *param, const char *value, void *ud, void *ud2)
{
    return ((__bridge peer_load_config_swf)ud)(p, pnum, param, value, ud2);
}

RetCode peer_init_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_init_swf)ud)(p, ud2);
}

RetCode peer_starting_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_starting_swf)ud)(p, ud2);
}

RetCode peer_stopping_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_stopping_swf)ud)(p, ud2);
}

RetCode peer_on_move_running_c(peer *p, void *ud, void *ud2)
{
    return ((__bridge peer_on_move_running_swf)ud)(p, ud2);
}

void peer_dying_breath_c(peer *p, void *ud, void *ud2)
{
    ((__bridge peer_dying_breath_swf)ud)(p, ud2);
}

RetCode peer_on_incoming_connection_c(peer *p, shr_incoming_connection *ic, void *ud, void *ud2)
{
    return ((__bridge peer_on_incoming_connection_swf)ud)(p, ic, ud2);
}

void peer_set_name_swf(peer *p, peer_name_swf hndl, void *ud)
{
    peer_set_name(p, peer_name_c, (__bridge_retained void *)hndl, ud);
}

void peer_set_version_swf(peer *p, peer_version_swf hndl, void *ud)
{
    peer_set_version(p, peer_version_c, (__bridge_retained void *)hndl, ud);
}

void peer_set_load_config_swf(peer *p, peer_load_config_swf hndl, void *ud)
{
    peer_set_load_config(p, peer_load_config_c, (__bridge_retained void *)hndl, ud);
}

void peer_set_init_swf(peer *p, peer_init_swf hndl, void *ud)
{
    peer_set_init(p, peer_init_c, (__bridge_retained void *)hndl, ud);
}

void peer_set_starting_swf(peer *p, peer_starting_swf hndl, void *ud)
{
    peer_set_starting(p, peer_starting_c, (__bridge_retained void *)hndl, ud);
}

void peer_set_stopping_swf(peer *p, peer_stopping_swf hndl, void *ud)
{
    peer_set_stopping(p, peer_stopping_c, (__bridge_retained void *)hndl, ud);
}

void peer_set_on_move_running_swf(peer *p, peer_on_move_running_swf hndl, void *ud)
{
    peer_set_on_move_running(p, peer_on_move_running_c, (__bridge_retained void *)hndl, ud);
}

void peer_set_dying_breath_swf(peer *p, peer_dying_breath_swf hndl, void *ud)
{
    peer_set_dying_breath(p, peer_dying_breath_c, (__bridge_retained void *)hndl, ud);
}

void peer_set_status_change_swf(peer *p, peer_status_change_swf hndl, void *ud)
{
    peer_set_status_change(p, peer_status_change_c, (__bridge_retained void *)hndl, ud);
}

void peer_set_peer_on_incoming_connection_swf(peer *p, peer_on_incoming_connection_swf hndl, void *ud)
{
    peer_set_peer_on_incoming_connection(p, peer_on_incoming_connection_c, (__bridge_retained void *)hndl, ud);
}

/************************************************************************
 INCOMING CONNECTION
 ************************************************************************/

void inco_connection_status_change_c(incoming_connection *conn, ConnectionStatus status, void *ud, void *ud2)
{
    inco_connection_status_change_swf icscs = (__bridge id)ud;
    icscs(conn, status, NULL);
}

void inco_connection_on_disconnect_c(incoming_connection *conn,
                                     ConnectivityEventResult con_evt_res,
                                     ConnectivityEventType c_evt_type,
                                     void *ud,
                                     void *ud2)
{
    ((__bridge inco_connection_on_disconnect_swf)ud)(conn, con_evt_res, c_evt_type, ud2);
}

RetCode inco_connection_on_incoming_transaction_c(incoming_connection *conn,
                                                  shr_incoming_transaction *itx,
                                                  void *ud,
                                                  void *ud2)
{
    return ((__bridge inco_connection_on_incoming_transaction_swf)ud)(conn, itx, ud2);
}

RetCode inco_connection_on_incoming_subscription_c(incoming_connection *conn,
                                                   shr_incoming_subscription *isbs,
                                                   void *ud,
                                                   void *ud2)
{
    return ((__bridge inco_connection_on_incoming_subscription_swf)ud)(conn, isbs, ud2);
}

void inco_connection_on_destroy_c(incoming_connection *conn,
                                  void *ud,
                                  void *ud2)
{
    return ((__bridge inco_connection_on_destroy_swf)ud)(conn, ud2);
}

void inco_connection_set_status_change_swf(incoming_connection *ic,
                                           inco_connection_status_change_swf hndl,
                                           void *ud)
{

    inco_connection_set_status_change(ic, inco_connection_status_change_c, (__bridge_retained void *)hndl, ud);
}

void inco_connection_set_on_disconnect_swf(incoming_connection *ic,
                                           inco_connection_on_disconnect_swf hndl,
                                           void *ud)
{
    inco_connection_set_on_disconnect(ic, inco_connection_on_disconnect_c, (__bridge_retained void *)hndl, ud);
}

void inco_connection_set_on_incoming_transaction_swf(incoming_connection *ic,
                                                     inco_connection_on_incoming_transaction_swf hndl,
                                                     void *ud)
{
    inco_connection_set_on_incoming_transaction(ic,
                                                inco_connection_on_incoming_transaction_c,
                                                (__bridge_retained void *)hndl,
                                                ud);
}

void inco_connection_set_on_incoming_subscription_swf(incoming_connection *ic,
                                                      inco_connection_on_incoming_subscription_swf hndl,
                                                      void *ud)
{
    inco_connection_set_on_incoming_subscription(ic,
                                                 inco_connection_on_incoming_subscription_c,
                                                 (__bridge_retained void *)hndl,
                                                 ud);
}

void inco_connection_set_on_destroy_swf(incoming_connection *ic,
                                        inco_connection_on_destroy_swf hndl,
                                        void *ud)
{
    inco_connection_set_on_destroy(ic,
                                   inco_connection_on_destroy_c,
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
    ((__bridge outg_connection_status_change_swf)ud)(oc, status, ud2);
}

void outg_connection_on_connect_c(outgoing_connection *oc,
                                  ConnectivityEventResult con_evt_res,
                                  ConnectivityEventType c_evt_type,
                                  void *ud,
                                  void *ud2)
{
    ((__bridge outg_connection_on_connect_swf)ud)(oc, con_evt_res, c_evt_type, ud2);
}

void outg_connection_on_disconnect_c(outgoing_connection *oc,
                                     ConnectivityEventResult con_evt_res,
                                     ConnectivityEventType c_evt_type,
                                     void *ud,
                                     void *ud2)
{
    ((__bridge outg_connection_on_disconnect_swf)ud)(oc, con_evt_res, c_evt_type, ud2);
}

void outg_connection_set_status_change_swf(outgoing_connection *oc,
                                           outg_connection_status_change_swf hndl,
                                           void *ud)
{
    outg_connection_set_status_change(oc, outg_connection_status_change_c, (__bridge_retained void *)hndl, ud);
}

void outg_connection_set_on_connect_swf(outgoing_connection *oc,
                                        outg_connection_on_connect_swf hndl,
                                        void *ud)
{
    outg_connection_set_on_connect(oc, outg_connection_on_connect_c, (__bridge_retained void *)hndl, ud);
}

void outg_connection_set_on_disconnect_swf(outgoing_connection *oc,
                                           outg_connection_on_disconnect_swf hndl,
                                           void *ud)
{
    outg_connection_set_on_disconnect(oc, outg_connection_on_disconnect_c, (__bridge_retained void *)hndl, ud);
}

/************************************************************************
 INCOMING TRANSACTION
 ************************************************************************/

void inco_transaction_status_change_c(incoming_transaction *tx, TransactionStatus status, void *ud, void *ud2)
{
    ((__bridge inco_transaction_status_change_swf)ud)(tx, status, ud2);
}

void inco_transaction_request_c(incoming_transaction *tx, void *ud, void *ud2)
{
    ((__bridge inco_transaction_request_swf)ud)(tx, ud2);
}

void inco_transaction_closure_c(incoming_transaction *tx, void *ud, void *ud2)
{
    ((__bridge inco_transaction_closure_swf)ud)(tx, ud2);
}

void inco_transaction_on_destroy_c(incoming_transaction *tx,
                                   void *ud,
                                   void *ud2)
{
    ((__bridge inco_transaction_on_destroy_swf)ud)(tx, ud2);
}

void inco_transaction_set_transaction_status_change_swf(incoming_transaction *tx,
                                                        inco_transaction_status_change_swf hndl,
                                                        void *ud)
{
    inco_transaction_set_transaction_status_change(tx,
                                                   inco_transaction_status_change_c,
                                                   (__bridge_retained void *)hndl,
                                                   ud);
}

void inco_transaction_set_transaction_closure_swf(incoming_transaction *tx,
                                                  inco_transaction_closure_swf hndl,
                                                  void *ud)
{
    inco_transaction_set_transaction_closure(tx,
                                             inco_transaction_closure_c,
                                             (__bridge_retained void *)hndl,
                                             ud);
}

void inco_transaction_set_inco_transaction_request_swf(incoming_transaction *tx,
                                                       inco_transaction_request_swf hndl,
                                                       void *ud)
{
    inco_transaction_set_inco_transaction_request(tx,
                                                  inco_transaction_request_c,
                                                  (__bridge_retained void *)hndl,
                                                  ud);
}

void inco_transaction_set_on_destroy_swf(incoming_transaction *tx,
                                         inco_transaction_on_destroy_swf hndl,
                                         void *ud)
{
    inco_transaction_set_on_destroy(tx,
                                    inco_transaction_on_destroy_c,
                                    (__bridge_retained void *)hndl,
                                    ud);

}


/************************************************************************
 OUTGOING TRANSACTION
 ************************************************************************/

void outg_transaction_status_change_c(outgoing_transaction *tx, TransactionStatus status, void *ud, void *ud2)
{
    ((__bridge outg_transaction_status_change_swf)ud)(tx, status, ud2);
}

void outg_transaction_closure_c(outgoing_transaction *tx, void *ud, void *ud2)
{
    ((__bridge outg_transaction_closure_swf)ud)(tx, ud2);
}

void outg_transaction_set_transaction_status_change_swf(outgoing_transaction *tx,
                                                        outg_transaction_status_change_swf hndl,
                                                        void *ud)
{
    outg_transaction_set_transaction_status_change(tx,
                                                   outg_transaction_status_change_c,
                                                   (__bridge_retained void *)hndl,
                                                   ud);
}

void outg_transaction_set_transaction_closure_swf(outgoing_transaction *tx, outg_transaction_closure_swf hndl,
                                                  void *ud)
{
    outg_transaction_set_transaction_closure(tx, outg_transaction_closure_c, (__bridge_retained void *)hndl, ud);
}

/************************************************************************
 INCOMING SUBSCRIPTION
 ************************************************************************/

void inco_subscription_status_change_c(incoming_subscription *isbs,
                                       SubscriptionStatus status,
                                       void *ud,
                                       void *ud2)
{
    ((__bridge inco_subscription_status_change_swf)ud)(isbs, status, ud2);
}

RetCode inco_subscription_accept_distribution_c(incoming_subscription *isbs,
                                                const subscription_event *sbs_evt,
                                                void *ud,
                                                void *ud2)
{
    return ((__bridge inco_subscription_accept_distribution_swf)ud)(isbs, sbs_evt, ud2);
}

void inco_subscription_on_stop_c(incoming_subscription *isbs, void *ud, void *ud2)
{
    ((__bridge inco_subscription_on_stop_swf)ud)(isbs, ud2);
}

void inco_subscription_on_destroy_c(incoming_subscription *isbs, void *ud, void *ud2)
{
    ((__bridge inco_subscription_on_destroy_swf)ud)(isbs, ud2);
}

void inco_subscription_set_status_change_swf(incoming_subscription *sbs,
                                             inco_subscription_status_change_swf hndl,
                                             void *ud)
{
    inco_subscription_set_status_change(sbs,
                                        inco_subscription_status_change_c,
                                        (__bridge_retained void *)hndl,
                                        ud);
}

void inco_subscription_set_accept_distribution_swf(incoming_subscription *sbs,
                                                   inco_subscription_accept_distribution_swf hndl,
                                                   void *ud)
{
    inco_subscription_set_accept_distribution(sbs,
                                              inco_subscription_accept_distribution_c,
                                              (__bridge_retained void *)hndl,
                                              ud);
}

void inco_subscription_set_on_stop_swf(incoming_subscription *sbs,
                                       inco_subscription_on_stop_swf hndl,
                                       void *ud)
{
    inco_subscription_set_on_stop(sbs,
                                  inco_subscription_on_stop_c,
                                  (__bridge_retained void *)hndl,
                                  ud);
}

void inco_subscription_set_on_destroy_swf(incoming_subscription *sbs,
                                          inco_subscription_on_destroy_swf hndl,
                                          void *ud)
{
    inco_subscription_set_on_destroy(sbs,
                                     inco_subscription_on_destroy_c,
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
    ((__bridge outg_subscription_status_change_swf)ud)(osbs, status, ud2);
}

void outg_subscription_notify_event_c(outgoing_subscription *osbs,
                                      const subscription_event *sbs_evt,
                                      void *ud,
                                      void *ud2)
{
    ((__bridge outg_subscription_notify_event_swf)ud)(osbs, sbs_evt, ud2);
}

void outg_subscription_on_start_c(outgoing_subscription *osbs, void *ud, void *ud2)
{
    ((__bridge outg_subscription_on_start_swf)ud)(osbs, ud2);
}

void outg_subscription_on_stop_c(outgoing_subscription *osbs, void *ud, void *ud2)
{
    ((__bridge outg_subscription_on_stop_swf)ud)(osbs, ud2);
}

void outg_subscription_set_status_change_swf(outgoing_subscription *sbs,
                                             outg_subscription_status_change_swf hndl,
                                             void *ud)
{
    outg_subscription_set_status_change(sbs,
                                        outg_subscription_status_change_c,
                                        (__bridge_retained void *)hndl,
                                        ud);
}

void outg_subscription_set_event_notify_swf(outgoing_subscription *sbs,
                                            outg_subscription_notify_event_swf hndl,
                                            void *ud)
{
    outg_subscription_set_event_notify(sbs,
                                       outg_subscription_notify_event_c,
                                       (__bridge_retained void *)hndl,
                                       ud);
}

void outg_subscription_set_on_start_swf(outgoing_subscription *sbs,
                                        outg_subscription_on_start_swf hndl,
                                        void *ud)
{
    outg_subscription_set_on_start(sbs, outg_subscription_on_start_c, (__bridge_retained void *)hndl, ud);
}

void outg_subscription_set_on_stop_swf(outgoing_subscription *sbs,
                                       outg_subscription_on_stop_swf hndl,
                                       void *ud)
{
    outg_subscription_set_on_stop(sbs, outg_subscription_on_stop_c, (__bridge_retained void *)hndl, ud);
}
