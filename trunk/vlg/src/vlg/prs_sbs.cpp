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

bool peer_enum_em_classes_create_schema(const nentity_desc &nentity_desc,
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
                IFLOG(wrn(TH_ID,LS_TRL "[no available persistence connection for nclass_id:%d]", __func__,
                          nentity_desc.get_nclass_id()))
                pud->res = vlg::RetCode_KO;
            }
        } else {
            IFLOG(wrn(TH_ID,LS_TRL "[no available persistence driver for nclass_id:%d]", __func__,
                      nentity_desc.get_nclass_id()))
            pud->res = vlg::RetCode_KO;
        }
        if(pud->res) {
            IFLOG(wrn(TH_ID,LS_TRL "[failed to create persistence schema for nclass_id:%d][res:%d]",
                      __func__,
                      nentity_desc.get_nclass_id(),
                      pud->res))
            if(pud->res != vlg::RetCode_DBOPFAIL) {
                //if it is worst than RetCode_DBOPFAIL we break;
                return false;
            }
        }
    }
    return true;
}

RetCode peer_impl::create_persistent_schema(PersistenceAlteringMode mode)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    SPC_REC ud;
    ud.peer = this;
    ud.mode = mode;
    ud.res = vlg::RetCode_OK;
    nem_.enum_nclass_descriptors(peer_enum_em_classes_create_schema, &ud);
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, ud.res))
    return ud.res;
}

RetCode peer_impl::nclass_create_persistent_schema(PersistenceAlteringMode mode,
                                                   unsigned int nclass_id)
{
    IFLOG(trc(TH_ID, LS_OPN "[mode:%d, nclass_id:%d]", __func__, mode, nclass_id))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = conn->create_entity_schema(mode, nem_, *nclass_desc))) {
                        IFLOG(err(TH_ID, LS_TRL "[create-schema failed for nclass_id:%d][res:%d]", __func__, nclass_id, rcode))
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::obj_load(unsigned short key,
                            unsigned int &ts0_out,
                            unsigned int &ts1_out,
                            nclass &in_out)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
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
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::obj_save(const nclass &in)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->save_entity(nem_, ts0, ts1, in);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::obj_update(unsigned short key,
                              const nclass &in)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->update_entity(key, nem_, ts0, ts1, in);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::obj_update_or_save(unsigned short key,
                                      const nclass &in)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
    unsigned int ts0 = 0, ts1 = 0;
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
                        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->save_or_update_entity(key, nem_, ts0, ts1, in);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::obj_remove(unsigned short key,
                              PersistenceDeletionMode mode,
                              const nclass &in)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->remove_entity(key, nem_, ts0, ts1, mode, in);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

// DISTRIBUTION

RetCode peer_impl::obj_distribute(SubscriptionEventType evt_type,
                                  ProtocolCode proto_code,
                                  Action act,
                                  const nclass &in)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    RetCode rcode = vlg::RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts0 = 0, ts1 = 0;
    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]", __func__, rcode))
    } else {
        sdr->next_time_stamp(ts0, ts1);
        if(sdr->connid_condesc_set_.size()) {
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           evt_type,
                                                                           proto_code,
                                                                           ts0,
                                                                           ts1,
                                                                           act,
                                                                           in);
            if((rcode = submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_))) {
                IFLOG(err(TH_ID, LS_TRL "%s() - SubmitSbsEvtTask failed with res:%d", __func__, rcode))
            }
        }
    }
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

// PERSISTENCE + DISTRIBUTION

RetCode peer_impl::obj_save_and_distribute(const nclass &in)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->save_entity(nem_, ts0, ts1, in);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->connid_condesc_set_.size()) {
            unsigned int ts1 = 0;
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           SubscriptionEventType_LIVE,
                                                                           ProtocolCode_SUCCESS,
                                                                           ts0,
                                                                           ts1,
                                                                           Action_INSERT,
                                                                           in);
            RET_ON_KO(submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_))
        }
    }
    //**** SBS MNG END
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::obj_update_and_distribute(unsigned short key,
                                             const nclass &in)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_CLO "[failed get per-nclass_id helper class][res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->update_entity(key, nem_, ts0, ts1, in);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->connid_condesc_set_.size()) {
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           SubscriptionEventType_LIVE,
                                                                           ProtocolCode_SUCCESS,
                                                                           ts0,
                                                                           ts1,
                                                                           Action_UPDATE,
                                                                           in);
            RET_ON_KO(submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_))
        }
    }
    //**** SBS MNG END
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::obj_update_or_save_and_distribute(unsigned short key,
                                                     const nclass &in)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
    unsigned int ts0 = 0, ts1 = 0;
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
                        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->save_or_update_entity(key, nem_, ts0, ts1, in);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->connid_condesc_set_.size()) {
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           SubscriptionEventType_LIVE,
                                                                           ProtocolCode_SUCCESS,
                                                                           ts0,
                                                                           ts1,
                                                                           Action_UPDATE,
                                                                           in);
            RET_ON_KO(submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_))
        }
    }
    //**** SBS MNG END
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

RetCode peer_impl::obj_remove_and_distribute(unsigned short key,
                                             PersistenceDeletionMode mode,
                                             const nclass &in)
{
    IFLOG(trc(TH_ID, LS_OPN, __func__))
    if(!pers_enabled_) {
        IFLOG(err(TH_ID, LS_CLO NOT_PERS_ENBL_PEER, __func__))
        return vlg::RetCode_KO;
    }
    RetCode rcode = vlg::RetCode_OK;
    per_nclass_id_conn_set *sdr = nullptr;
    unsigned int ts0 = 0, ts1 = 0;
    unsigned int nclass_id = in.get_id();
    const nentity_desc *nclass_desc = nem_.get_nentity_descriptor(nclass_id);
    if(nclass_desc) {
        if(nclass_desc->is_persistent()) {
            persistence_driver *driv = nullptr;
            if((driv = pers_mng_.available_driver(nclass_id))) {
                persistence_connection_impl *conn = nullptr;
                if((conn = driv->available_connection(nclass_id))) {
                    if((rcode = get_per_nclassid_helper_rec(in.get_id(), &sdr))) {
                        IFLOG(cri(TH_ID, LS_TRL "%s() - failed get per-nclass_id helper class [res:%d]", __func__, rcode))
                    } else {
                        sdr->next_time_stamp(ts0, ts1);
                        rcode = conn->remove_entity(key, nem_, ts0, ts1, mode, in);
                    }
                } else {
                    IFLOG(err(TH_ID, LS_TRL "[no available persistence-connection for nclass_id:%d]", __func__, nclass_id))
                    rcode = vlg::RetCode_KO;
                }
            } else {
                IFLOG(err(TH_ID, LS_TRL "[no available persistence-driver for nclass_id:%d]", __func__, nclass_id))
                rcode = vlg::RetCode_KO;
            }
        } else {
            IFLOG(err(TH_ID, LS_TRL "[nclass is not persistent][nclass_id:%u]", __func__, nclass_id))
            rcode = vlg::RetCode_KO;
        }
    } else {
        IFLOG(err(TH_ID, LS_TRL "[nclass descriptor not found][nclass_id:%u]", __func__, nclass_id))
        rcode = vlg::RetCode_KO;
    }
    //**** SBS MNG BG
    if(!rcode) {
        if(sdr->connid_condesc_set_.size()) {
            subscription_event_impl *sbs_evt = new subscription_event_impl(0,
                                                                           sdr->next_sbs_evt_id(),
                                                                           SubscriptionEventType_LIVE,
                                                                           ProtocolCode_SUCCESS,
                                                                           ts0,
                                                                           ts1,
                                                                           (mode == PersistenceDeletionMode_LOGICAL) ? Action_DELETE : Action_REMOVE,
                                                                           in);
            RET_ON_KO(submit_sbs_evt_task(*sbs_evt, sdr->connid_condesc_set_))
        }
    }
    //**** SBS MNG END
    IFLOG(trc(TH_ID, LS_CLO "[res:%d]", __func__, rcode))
    return rcode;
}

}