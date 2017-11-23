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

#include "vlg_peer_impl.h"
#include "vlg_connection_impl.h"
#include "vlg_subscription_impl.h"

namespace vlg {

#define NOT_PERS_ENBL_PEER " peer is not persistence enabled."

//-----------------------------
// PERSISTENCE
//-----------------------------

struct SPC_REC {
    peer_impl *peer;
    PersistenceAlteringMode mode;
    vlg::RetCode res;
};

void peer_enum_em_classes_create_schema(const nentity_desc &nentity_desc,
                                        void *ud, bool &stop)
{
    SPC_REC *pud = static_cast<SPC_REC *>(ud);
    if(nentity_desc.is_persistent()) {
        persistence_driver_impl *driv = NULL;
        if((driv = pud->peer->get_pers_mng().available_driver(
                       nentity_desc.get_nclass_id()))) {
            persistence_connection_impl *conn = NULL;
            if((conn = driv->available_connection(nentity_desc.get_nclass_id()))) {
                pud->res = conn->create_entity_schema(pud->mode, pud->peer->get_em(),
                                                      nentity_desc);
            } else {
                IFLOG2(pud->peer->logger(), wrn(TH_ID,
                                                LS_TRL "%s() - no available persistence connection for nclass_id:%d", __func__,
                                                nentity_desc.get_nclass_id()))
                pud->res = vlg::RetCode_KO;
            }
        } else {
            IFLOG2(pud->peer->logger(), wrn(TH_ID,
                                            LS_TRL "%s() - no available persistence driver for nclass_id:%d", __func__,
                                            nentity_desc.get_nclass_id()))
            pud->res = vlg::RetCode_KO;
        }
        if(pud->res) {
            IFLOG2(pud->peer->logger(), wrn(TH_ID,
                                            LS_TRL "%s() failed to create persistence schema for nclass_id:%d, res:%d",
                                            __func__,
                                            nentity_desc.get_nclass_id(),
                                            pud->res))
            if(pud->res != vlg::RetCode_DBOPFAIL) {
                //if it is worst than RetCode_DBOPFAIL we break;
                stop = true;
            }
        }
    }
}

vlg::RetCode peer_impl::pers_schema_create(PersistenceAlteringMode mode)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    SPC_REC ud;
    ud.peer = this;
    ud.mode = mode;
    ud.res = vlg::RetCode_OK;
    nem_.enum_nclass_descriptors(peer_enum_em_classes_create_schema, &ud);
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, ud.res))
    return ud.res;
}

vlg::RetCode peer_impl::class_pers_schema_create(PersistenceAlteringMode
                                                 mode,
                                                 unsigned int nclass_id)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(mode:%d, nclass_id:%d)", __func__, nclass_id))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = conn->create_entity_schema(mode, nem_, *class_desc))) {
                        IFLOG(err(TH_ID, LS_TRL "%s() - create-schema failed for nclass_id:%d [res:%d]",
                                  __func__, nclass_id, rcode))
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::class_pers_load(unsigned short key,
                                        unsigned int  &ts0_out,
                                        unsigned int  &ts1_out,
                                        nclass &in_out_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%u)", __func__, key))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    unsigned int nclass_id = in_out_obj.get_nclass_id();
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    rcode = conn->load_entity(key, nem_, ts0_out, ts1_out, in_out_obj);
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::class_pers_save(const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    per_nclassid_helper_rec *sdr = NULL;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in_obj.get_nclass_id();
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_classid_helper_class(in_obj.get_nclass_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_CLO "%s() - failed get per-nclass_id helper class [res:%d]",
                                  __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->save_entity(nem_, ts0, ts1, in_obj);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::class_pers_update(unsigned short key,
                                          const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%u)", __func__, key))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    per_nclassid_helper_rec *sdr = NULL;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in_obj.get_nclass_id();
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_classid_helper_class(in_obj.get_nclass_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_CLO
                                  "%s() - failed get per-nclass_id helper class [res:%d]",
                                  __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->update_entity(key, nem_, ts0, ts1, in_obj);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::class_pers_update_or_save(unsigned short key,
                                                  const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%u)", __func__, key))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    unsigned int ts0 = 0, ts1 = 0;
    per_nclassid_helper_rec *sdr = NULL;
    unsigned int nclass_id = in_obj.get_nclass_id();
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_classid_helper_class(in_obj.get_nclass_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]",
                                  __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->save_or_update_entity(key, nem_, ts0, ts1, in_obj);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::class_pers_remove(unsigned short key,
                                          PersistenceDeletionMode mode,
                                          const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%u)", __func__, key))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    per_nclassid_helper_rec *sdr = NULL;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in_obj.get_nclass_id();
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_classid_helper_class(in_obj.get_nclass_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]",
                                  __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->remove_entity(key, nem_, ts0, ts1, mode, in_obj);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//-----------------------------
// DISTRIBUTION
//-----------------------------

vlg::RetCode peer_impl::class_distribute(SubscriptionEventType evt_type,
                                         ProtocolCode proto_code,
                                         Action act,
                                         const nclass &obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    vlg::RetCode rcode = vlg::RetCode_OK;
    per_nclassid_helper_rec *sdr = NULL;
    unsigned int ts0 = 0, ts1 = 0;
    if((rcode = get_per_classid_helper_class(obj.get_nclass_id(), &sdr))) {
        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]",
                  __func__, rcode))
    } else {
        sdr->next_time_stamp(ts0, ts1);
        if(sdr->get_srv_connid_condesc_set().size()) {
            subscription_event_impl *new_sbs_event = NULL;
            if(!(rcode = build_sbs_event(sdr->next_sbs_evt_id(),
                                         evt_type,
                                         proto_code,
                                         ts0,
                                         ts1,
                                         act,
                                         &obj,
                                         &new_sbs_event))) {
                if((rcode = submit_sbs_evt_task(*new_sbs_event,
                                                sdr->get_srv_connid_condesc_set()))) {
                    IFLOG(err(TH_ID, LS_TRL "%s() - SubmitSbsEvtTask failed with res:%d", __func__,
                              rcode))
                }
            } else {
                IFLOG(cri(TH_ID, LS_TRL "%s() - BuildSbsEvent failed with res:%d", __func__,
                          rcode))
            }
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

//-----------------------------
// PERSISTENCE + DISTRIBUTION
//-----------------------------

vlg::RetCode peer_impl::class_pers_save_and_distribute(
    const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    per_nclassid_helper_rec *sdr = NULL;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in_obj.get_nclass_id();
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_classid_helper_class(in_obj.get_nclass_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_CLO "%s() - failed get per-nclass_id helper class [res:%d]",
                                  __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->save_entity(nem_, ts0, ts1, in_obj);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->get_srv_connid_condesc_set().size()) {
            unsigned int ts1 = 0;
            subscription_event_impl *new_sbs_event = NULL;
            RETURN_IF_NOT_OK(build_sbs_event(sdr->next_sbs_evt_id(),
                                             SubscriptionEventType_LIVE,
                                             ProtocolCode_SUCCESS,
                                             ts0,
                                             ts1,
                                             Action_INSERT,
                                             &in_obj,
                                             &new_sbs_event))
            RETURN_IF_NOT_OK(submit_sbs_evt_task(*new_sbs_event,
                                                 sdr->get_srv_connid_condesc_set()))
        }
    }
    //**** SBS MNG END
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::class_pers_update_and_distribute(unsigned short key,
                                                         const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%u)", __func__, key))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    per_nclassid_helper_rec *sdr = NULL;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in_obj.get_nclass_id();
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_classid_helper_class(in_obj.get_nclass_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_CLO "%s() - failed get per-nclass_id helper class [res:%d]",
                                  __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->update_entity(key, nem_, ts0, ts1, in_obj);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->get_srv_connid_condesc_set().size()) {
            subscription_event_impl *new_sbs_event = NULL;
            RETURN_IF_NOT_OK(build_sbs_event(sdr->next_sbs_evt_id(),
                                             SubscriptionEventType_LIVE,
                                             ProtocolCode_SUCCESS,
                                             ts0,
                                             ts1,
                                             Action_UPDATE,
                                             &in_obj,
                                             &new_sbs_event))
            RETURN_IF_NOT_OK(submit_sbs_evt_task(*new_sbs_event,
                                                 sdr->get_srv_connid_condesc_set()))
        }
    }
    //**** SBS MNG END
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::class_pers_update_or_save_and_distribute(
    unsigned short key,
    const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%u)", __func__, key))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    unsigned int ts0 = 0, ts1 = 0;
    per_nclassid_helper_rec *sdr = NULL;
    unsigned int nclass_id = in_obj.get_nclass_id();
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_classid_helper_class(in_obj.get_nclass_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]",
                                  __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->save_or_update_entity(key, nem_, ts0, ts1, in_obj);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->get_srv_connid_condesc_set().size()) {
            subscription_event_impl *new_sbs_event = NULL;
            RETURN_IF_NOT_OK(build_sbs_event(sdr->next_sbs_evt_id(),
                                             SubscriptionEventType_LIVE,
                                             ProtocolCode_SUCCESS,
                                             ts0,
                                             ts1,
                                             Action_UPDATE,
                                             &in_obj,
                                             &new_sbs_event))
            RETURN_IF_NOT_OK(submit_sbs_evt_task(*new_sbs_event,
                                                 sdr->get_srv_connid_condesc_set()))
        }
    }
    //**** SBS MNG END
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

vlg::RetCode peer_impl::class_pers_remove_and_distribute(unsigned short key,
                                                         PersistenceDeletionMode mode,
                                                         const nclass &in_obj)
{
    IFLOG(trc(TH_ID, LS_OPN "%s(key:%u)", __func__, key))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO "%s() -" NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    vlg::RetCode rcode = vlg::RetCode_OK;
    per_nclassid_helper_rec *sdr = NULL;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in_obj.get_nclass_id();
    const nentity_desc *class_desc = NULL;
    if(!(rcode = nem_.get_nentity_descriptor(nclass_id, &class_desc))) {
        if(class_desc->is_persistent()) {
            persistence_driver_impl *driv = NULL;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = NULL;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_classid_helper_class(in_obj.get_nclass_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]",
                                  __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->remove_entity(key, nem_, ts0, ts1, mode, in_obj);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-connection for nclass_id:%d",
                              __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "%s() - no available pers-driver for nclass_id:%d",
                          __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "%s() - class is not persistable. [nclass_id:%u]",
                      __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "%s() - class descriptor not found. [nclass_id:%u]",
                  __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->get_srv_connid_condesc_set().size()) {
            subscription_event_impl *new_sbs_event = NULL;
            RETURN_IF_NOT_OK(build_sbs_event(sdr->next_sbs_evt_id(),
                                             SubscriptionEventType_LIVE,
                                             ProtocolCode_SUCCESS,
                                             ts0,
                                             ts1,
                                             (mode == PersistenceDeletionMode_LOGICAL) ? Action_DELETE :
                                             Action_REMOVE,
                                             &in_obj,
                                             &new_sbs_event))
            RETURN_IF_NOT_OK(submit_sbs_evt_task(*new_sbs_event,
                                                 sdr->get_srv_connid_condesc_set()))
        }
    }
    //**** SBS MNG END
    IFLOG(trc(TH_ID, LS_CLO "%s(res:%d)", __func__, rcode))
    return rcode;
}

}