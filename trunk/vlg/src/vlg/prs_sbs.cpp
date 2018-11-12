/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "pr_impl.h"
#include "conn_impl.h"
#include "sbs_impl.h"

namespace vlg {

#define NOT_PERS_ENBL_PEER "[peer is not persistence enabled]"

// PERSISTENCE

struct SPC_REC {
    peer_impl *peer;
    PersistenceAlteringMode mode;
    RetCode res;
};

bool peer_enum_nem_nclasses_create_schema(const nentity_desc &nentity_desc,
                                          void *ud)
{
    SPC_REC *pud = static_cast<SPC_REC *>(ud);
    if(nentity_desc.is_persistent()) {
        persistence_driver *driv = nullptr;
        if((driv = pud->peer->pers_mng_.available_driver(nentity_desc.get_nclass_id()))) {
            persistence_connection_impl *conn = nullptr;
            if((conn = driv->available_connection(nentity_desc.get_nclass_id()))) {
                pud->res = conn->create_entity_schema(pud->mode,
                                                      pud->peer->nem_,
                                                      nentity_desc);
            } else {
                IFLOG(pud->peer->log_, wrn(TH_ID, LS_TRL "[no available persistence connection for nclass_id:%d]", __func__,
                                           nentity_desc.get_nclass_id()))
                pud->res = RetCode_KO;
            }
        } else {
            IFLOG(pud->peer->log_, wrn(TH_ID, LS_TRL "[no available persistence driver for nclass_id:%d]", __func__,
                                       nentity_desc.get_nclass_id()))
            pud->res = RetCode_KO;
        }
        if(pud->res) {
            IFLOG(pud->peer->log_, wrn(TH_ID, LS_TRL "[failed to create persistence schema for nclass_id:%d][res:%d]",
                                       __func__,
                                       nentity_desc.get_nclass_id(),
                                       pud->res))
            if(pud->res != RetCode_DBOPFAIL) {
                //if it is worst than RetCode_DBOPFAIL we break;
                return false;
            }
        }
    }
    return true;
}

RetCode peer_impl::create_persistent_schema(PersistenceAlteringMode mode)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    SPC_REC ud;
    ud.peer = this;
    ud.mode = mode;
    ud.res = RetCode_OK;
    nem_.enum_nclass_descriptors(peer_enum_nem_nclasses_create_schema, &ud);
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, ud.res))
    return ud.res;
}

RetCode peer_impl::nclass_create_persistent_schema(PersistenceAlteringMode mode,
                                                   unsigned int nclass_id)
{
    IFLOG(log_, trc(TH_ID, LS_OPN "[mode:%d, nclass_id:%d]", __func__, mode, nclass_id))
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = conn->create_entity_schema(mode, nem_, *nclass_desc))) {
                        IFLOG(log_, err(TH_ID, LS_TRL "[create-schema failed for nclass_id:%d][res:%d]", __func__, nclass_id, rcode))
                    }
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::obj_load(unsigned short key,
                            unsigned int &ts0_out,
                            unsigned int &ts1_out,
                            nclass &in_out)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    unsigned int nclass_id = in_out.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    rcode = conn->load_entity(key, nem_, ts0_out, ts1_out, in_out);
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    return rcode;
}

RetCode peer_impl::obj_save(const nclass &in)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts_0 = 0, ts_1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(log_, cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts_0, ts_1);
                        rcode = conn->save_entity(nem_, ts_0, ts_1, in);
                    }
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    return rcode;
}

RetCode peer_impl::obj_update(unsigned short key,
                              const nclass &in)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts_0 = 0, ts_1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(log_, cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts_0, ts_1);
                        rcode = conn->update_entity(key, nem_, ts_0, ts_1, in);
                    }
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    return rcode;
}

RetCode peer_impl::obj_update_or_save(unsigned short key,
                                      const nclass &in)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    unsigned int ts_0 = 0, ts_1 = 0;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(log_, cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts_0, ts_1);
                        rcode = conn->save_or_update_entity(key, nem_, ts_0, ts_1, in);
                    }
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    return rcode;
}

RetCode peer_impl::obj_remove(unsigned short key,
                              PersistenceDeletionMode mode,
                              const nclass &in)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts_0 = 0, ts_1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(log_, cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts_0, ts_1);
                        rcode = conn->remove_entity(key, nem_, ts_0, ts_1, mode, in);
                    }
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    return rcode;
}

// DISTRIBUTION

RetCode peer_impl::obj_distribute(SubscriptionEventType evt_type,
                                  ProtocolCode proto_code,
                                  Action act,
                                  const nclass &in)
{
    RetCode rcode = RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts_0 = 0, ts_1 = 0;
    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
        IFLOG(log_, cri(TH_ID, LS_TRL "failed get per-nclass_id helper class [res:%d]", __func__, rcode))
    } else {
        sdr->next_time_stamp(ts_0, ts_1);
        if(sdr->connid_condesc_set_.size()) {
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           evt_type,
                                                                           proto_code,
                                                                           ts_0,
                                                                           ts_1,
                                                                           act,
                                                                           in);
            if((rcode = submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_))) {
                IFLOG(log_, err(TH_ID, LS_TRL "submit_sbs_evt_task failed with res:%d", __func__, rcode))
            }
        }
    }
    IFLOG(log_, trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

// PERSISTENCE + DISTRIBUTION

RetCode peer_impl::obj_save_and_distribute(const nclass &in)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts_0 = 0, ts_1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(log_, cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts_0, ts_1);
                        rcode = conn->save_entity(nem_, ts_0, ts_1, in);
                    }
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->connid_condesc_set_.size()) {
            unsigned int ts_1 = 0;
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           SubscriptionEventType_LIVE,
                                                                           ProtocolCode_SUCCESS,
                                                                           ts_0,
                                                                           ts_1,
                                                                           Action_INSERT,
                                                                           in);
            rcode = submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_);
        }
    }
    //**** SBS MNG END
    return rcode;
}

RetCode peer_impl::obj_update_and_distribute(unsigned short key,
                                             const nclass &in)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts_0 = 0, ts_1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(log_, cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts_0, ts_1);
                        rcode = conn->update_entity(key, nem_, ts_0, ts_1, in);
                    }
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->connid_condesc_set_.size()) {
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           SubscriptionEventType_LIVE,
                                                                           ProtocolCode_SUCCESS,
                                                                           ts_0,
                                                                           ts_1,
                                                                           Action_UPDATE,
                                                                           in);
            rcode = submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_);
        }
    }
    //**** SBS MNG END
    return rcode;
}

RetCode peer_impl::obj_update_or_save_and_distribute(unsigned short key,
                                                     const nclass &in)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    unsigned int ts_0 = 0, ts_1 = 0;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(log_, cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts_0, ts_1);
                        rcode = conn->save_or_update_entity(key, nem_, ts_0, ts_1, in);
                    }
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->connid_condesc_set_.size()) {
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           SubscriptionEventType_LIVE,
                                                                           ProtocolCode_SUCCESS,
                                                                           ts_0,
                                                                           ts_1,
                                                                           Action_UPDATE,
                                                                           in);
            rcode = submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_);
        }
    }
    //**** SBS MNG END
    return rcode;
}

RetCode peer_impl::obj_remove_and_distribute(unsigned short key,
                                             PersistenceDeletionMode mode,
                                             const nclass &in)
{
    if(!pers_enabled_) {
        IFLOG(log_, err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return RetCode_KO;
    }
    RetCode rcode = RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts_0 = 0, ts_1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(log_, cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts_0, ts_1);
                        rcode = conn->remove_entity(key, nem_, ts_0, ts_1, mode, in);
                    }
                } else {
                    IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = RetCode_KO;
                }
            } else {
                IFLOG(log_, err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = RetCode_KO;
            }
        } else {
            IFLOG(log_, err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = RetCode_KO;
        }
    } else {
        IFLOG(log_, err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->connid_condesc_set_.size()) {
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           SubscriptionEventType_LIVE,
                                                                           ProtocolCode_SUCCESS,
                                                                           ts_0,
                                                                           ts_1,
                                                                           (mode == PersistenceDeletionMode_LOGICAL) ? Action_DELETE : Action_REMOVE,
                                                                           in);
            rcode = submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_);
        }
    }
    //**** SBS MNG END
    return rcode;
}

}
