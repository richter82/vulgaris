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

#include "vlg_logger.h"
#include "vlg_model.h"
#include "vlg_globint.h"

namespace vlg {

const char *string_from_Type(Type bt)
{
    switch(bt) {
        case Type_UNDEFINED:
            return "vlg::Type_UNDEFINED";
        case Type_ENTITY:
            return "vlg::Type_ENTITY";
        case Type_BOOL:
            return "vlg::Type_BOOL";
        case Type_INT16:
            return "vlg::Type_INT16";
        case Type_UINT16:
            return "vlg::Type_UINT16";
        case Type_INT32:
            return "vlg::Type_INT32";
        case Type_UINT32:
            return "vlg::Type_UINT32";
        case Type_INT64:
            return "vlg::Type_INT64";
        case Type_UINT64:
            return "vlg::Type_UINT64";
        case Type_FLOAT32:
            return "vlg::Type_FLOAT32";
        case Type_FLOAT64:
            return "vlg::Type_FLOAT64";
        case Type_ASCII:
            return "vlg::Type_ASCII";
    }
    return "";
}

const char *string_from_NEntityType(NEntityType bet)
{
    switch(bet) {
        case NEntityType_UNDEFINED:
            return "vlg::NEntityType_UNDEFINED";
        case NEntityType_NENUM:
            return "vlg::NEntityType_NENUM";
        case NEntityType_NCLASS:
            return "vlg::NEntityType_NCLASS";
        default:
            return "";
    }
}

const char *string_from_MemberType(MemberType bmt)
{
    switch(bmt) {
        case MemberType_UNDEFINED:
            return "vlg::MemberType_UNDEFINED";
        case MemberType_FIELD:
            return "vlg::MemberType_FIELD";
        case MemberType_NENUM_VALUE:
            return "vlg::MemberType_NENUM_VALUE";
        default:
            return "";
    }
}

//-----------------------------
// key_desc_impl
//-----------------------------
class key_desc_impl {
    public:
        key_desc_impl(unsigned short keyid, bool primary) :
            keyid_(keyid),
            primary_(primary),
            fieldset_(vlg::sngl_ptr_obj_mng()) {
        }

        vlg::RetCode init() {
            RETURN_IF_NOT_OK(fieldset_.init())
            return vlg::RetCode_OK;
        }

        vlg::RetCode init(vlg::linked_list *fldset) {
            RETURN_IF_NOT_OK(fieldset_.init())
            member_desc *mmbrdesc = NULL;
            fldset->start_iteration();
            while(!fldset->next(&mmbrdesc)) {
                RETURN_IF_NOT_OK(fieldset_.push_back(&mmbrdesc))
            }
            return vlg::RetCode_OK;
        }

        vlg::RetCode add_member_desc(const member_desc  *mmbrdesc) {
            RETURN_IF_NOT_OK(fieldset_.push_back(&mmbrdesc))
            return vlg::RetCode_OK;
        }

        unsigned short get_key_id() const {
            return keyid_;
        }

        bool is_primary() const {
            return primary_;
        }

        const vlg::linked_list &NOINLINE get_key_field_set() const {
            return fieldset_;
        }

        vlg::linked_list &get_key_field_set_m() {
            return fieldset_;
        }

    private:
        unsigned short  keyid_;
        bool            primary_;

        //members being part of this key.
        vlg::linked_list  fieldset_;
};

//-----------------------------
// key_desc
//-----------------------------
key_desc::key_desc(unsigned short keyid, bool primary) : impl_(NULL)
{
    impl_ = new key_desc_impl(keyid, primary);
    impl_->init();
}

key_desc::~key_desc()
{
    if(impl_) {
        delete impl_;
    }
}

vlg::RetCode key_desc::add_member_desc(const member_desc  *mmbrdesc)
{
    return impl_->add_member_desc(mmbrdesc);
}

unsigned short key_desc::get_key_id() const
{
    return impl_->get_key_id();
}

bool key_desc::is_primary() const
{
    return impl_->is_primary();
}

const key_desc_impl *key_desc::get_opaque() const
{
    return impl_;
}

//-----------------------------
// member_desc_impl
//-----------------------------
class member_desc_impl {
    public:
        member_desc_impl(unsigned short mmbrid,
                         MemberType     mmbr_type,
                         const char     *mmbr_name,
                         const char     *mmbr_desc,
                         Type           fild_type,
                         size_t         fild_offset,
                         size_t         fild_type_size,
                         size_t         nmemb,
                         unsigned int   fild_entityid,
                         const char     *fild_usr_str_type,
                         NEntityType    fild_entitytype,
                         long           enum_value) :
            mmbrid_(mmbrid),
            mmbr_type_(mmbr_type),
            mmbr_name_(mmbr_name),
            mmbr_desc_(mmbr_desc),
            fild_type_(fild_type),
            fild_offset_(fild_offset),
            fild_type_size_(fild_type_size),
            nmemb_(nmemb),
            fild_nclassid_(fild_entityid),
            fild_usr_str_type_(fild_usr_str_type),
            fild_entitytype_(fild_entitytype),
            enum_value_(enum_value) {
        }

        unsigned short  mmbrid_;
        MemberType      mmbr_type_;
        const char      *mmbr_name_;
        const char      *mmbr_desc_;
        Type            fild_type_;
        size_t          fild_offset_;
        size_t          fild_type_size_;
        size_t          nmemb_;
        unsigned int    fild_nclassid_;
        const char      *fild_usr_str_type_;
        NEntityType     fild_entitytype_;
        long            enum_value_;
};

//-----------------------------
// member_desc
//-----------------------------
member_desc::member_desc(unsigned short mmbrid,
                         MemberType     mmbr_type,
                         const char     *mmbr_name,
                         const char     *mmbr_desc,
                         Type           fild_type,
                         size_t         fild_offset,
                         size_t         fild_type_size,
                         size_t         nmemb,
                         unsigned int   fild_entityid,
                         const char     *fild_usr_str_type,
                         NEntityType    fild_entitytype,
                         long           enum_value) : impl_(NULL)
{
    impl_ = new member_desc_impl(mmbrid,
                                 mmbr_type,
                                 mmbr_name,
                                 mmbr_desc,
                                 fild_type,
                                 fild_offset,
                                 fild_type_size,
                                 nmemb,
                                 fild_entityid,
                                 fild_usr_str_type,
                                 fild_entitytype,
                                 enum_value);
}

member_desc::~member_desc()
{
    if(impl_) {
        delete impl_;
    }
}

unsigned short member_desc::get_member_id() const
{
    return impl_->mmbrid_;
}

MemberType member_desc::get_member_type() const
{
    return impl_->mmbr_type_;
}

const char *member_desc::get_member_name() const
{
    return impl_->mmbr_name_;
}

const char *member_desc::get_member_description() const
{
    return impl_->mmbr_desc_;
}

Type member_desc::get_field_vlg_type() const
{
    return impl_->fild_type_;
}

size_t member_desc::get_field_offset() const
{
    return impl_->fild_offset_;
}

size_t member_desc::get_field_type_size() const
{
    return impl_->fild_type_size_;
}

size_t member_desc::get_field_nmemb() const
{
    return impl_->nmemb_;
}

unsigned int member_desc::get_field_nclass_id() const
{
    return impl_->fild_nclassid_;
}

const char *member_desc::get_field_user_type() const
{
    return impl_->fild_usr_str_type_;
}

NEntityType member_desc::get_field_nentity_type() const
{
    return impl_->fild_entitytype_;
}

long member_desc::get_nenum_value() const
{
    return impl_->enum_value_;
}

void member_desc::set_field_offset(size_t val)
{
    impl_->fild_offset_ = val;
}

void member_desc::set_field_type_size(size_t val)
{
    impl_->fild_type_size_ = val;
}

//-----------------------------
// entity_desc_impl
//-----------------------------
struct ent_enum_mdesc_rec {
    enum_member_desc cllbk;
    void *ud;
};

void ent_enum_MembDesc(const vlg::hash_map &map,
                       const void *key,
                       const void *ptr,
                       void *ud,
                       bool &brk)
{
    ent_enum_mdesc_rec *pud = static_cast<ent_enum_mdesc_rec *>(ud);
    pud->cllbk(*(*(member_desc **)ptr), pud->ud, brk);
}

class nentity_desc_impl {
    public:
        nentity_desc_impl(unsigned int entityid,
                          size_t entity_size,
                          size_t entity_max_align,
                          NEntityType entitytype,
                          const char *nmspace,
                          const char *entityname,
                          vlg::alloc_func afun,
                          unsigned int fild_num,
                          bool persistent) :
            nentity_id_(entityid),
            nclass_size_(entity_size),
            nclass_max_align_(entity_max_align),
            entity_type_(entitytype),
            nmspace_(nmspace),
            entityname_(entityname),
            afun_(afun),
            fild_num_(fild_num),
            mmbrid_mdesc_(vlg::sngl_ptr_obj_mng(), sizeof(unsigned short)),
            mmbrnm_mdesc_(vlg::sngl_ptr_obj_mng(), vlg::sngl_cstr_obj_mng()),
            mmbrof_mdesc_(vlg::sngl_ptr_obj_mng(), sizeof(size_t)),
            persistent_(persistent),
            keyid_kdesc_(vlg::sngl_ptr_obj_mng(), sizeof(unsigned short)) {
        }

        vlg::RetCode init() {
            RETURN_IF_NOT_OK(mmbrid_mdesc_.init(HM_SIZE_MINI))
            RETURN_IF_NOT_OK(mmbrnm_mdesc_.init(HM_SIZE_MINI))
            RETURN_IF_NOT_OK(mmbrof_mdesc_.init(HM_SIZE_MINI))
            if(persistent_) {
                RETURN_IF_NOT_OK(keyid_kdesc_.init(HM_SIZE_NANO))
            }
            return vlg::RetCode_OK;
        }

        vlg::RetCode init(vlg::hash_map *mmbrmap, vlg::hash_map *keymap) {
            RETURN_IF_NOT_OK(init())
            member_desc *mdesc = NULL;
            unsigned long mmbrid = 0;
            unsigned long fldofst = 0;
            mmbrmap->start_iteration();
            while(!mmbrmap->next(NULL, &mdesc)) {
                mmbrid = mdesc->get_member_id();
                RETURN_IF_NOT_OK(mmbrid_mdesc_.put(&mmbrid, &mdesc))
                const char *mmbrnm = mdesc->get_member_name();
                RETURN_IF_NOT_OK(mmbrnm_mdesc_.put(mmbrnm, &mdesc))
                if(mdesc->get_member_type() == MemberType_FIELD) {
                    fldofst = (unsigned long)mdesc->get_field_offset();
                    RETURN_IF_NOT_OK(mmbrof_mdesc_.put(&fldofst, &mdesc))
                }
            }
            if(persistent_) {
                key_desc *keydesc = NULL;
                unsigned short keyid = 0;
                keymap->start_iteration();
                while(!keymap->next(&keyid, &keydesc)) {
                    RETURN_IF_NOT_OK(keyid_kdesc_.put(&keyid, &keydesc))
                }
            }
            return vlg::RetCode_OK;
        }

        vlg::RetCode add_member_desc(const member_desc *mmbrdesc) {
            unsigned long mmbrid = mmbrdesc->get_member_id();
            const char *mmbrnm = mmbrdesc->get_member_name();
            unsigned long fldofst = (unsigned long)mmbrdesc->get_field_offset();
            RETURN_IF_NOT_OK(mmbrid_mdesc_.put(&mmbrid, &mmbrdesc))
            RETURN_IF_NOT_OK(mmbrnm_mdesc_.put(mmbrnm, &mmbrdesc))
            RETURN_IF_NOT_OK(mmbrof_mdesc_.put(&fldofst, &mmbrdesc))
            return vlg::RetCode_OK;
        }

        vlg::RetCode add_key_desc(const key_desc *keydesc) {
            unsigned short keyid = keydesc->get_key_id();
            RETURN_IF_NOT_OK(keyid_kdesc_.put(&keyid, &keydesc))
            return vlg::RetCode_OK;
        }

        unsigned int get_nentity_id()  const {
            return nentity_id_;
        }

        size_t get_nclass_size()  const {
            return nclass_size_;
        }

        size_t get_nclass_max_align() const {
            return nclass_max_align_;
        }

        NEntityType get_nentity_type()  const {
            return entity_type_;
        }

        const char *get_nentity_namespace() const {
            return nmspace_;
        }

        const char *get_nentity_name()  const {
            return entityname_;
        }

        vlg::alloc_func get_nentity_alloc_f() const {
            return afun_;
        }

        unsigned int get_field_num()  const {
            return fild_num_;
        }

        bool is_persistent()  const {
            return persistent_;
        }

        vlg::hash_map &NOINLINE GetMap_ID_MMBRDSC() {
            return mmbrid_mdesc_;
        }

        const vlg::hash_map &NOINLINE GetMap_NM_MMBRDSC() const {
            return mmbrnm_mdesc_;
        }

        const vlg::hash_map &NOINLINE GetMap_KEYID_KDESC() const {
            return keyid_kdesc_;
        }

        const member_desc *GetMemberDescById(unsigned int mmbrid) const {
            const void *ptr = mmbrid_mdesc_.get(&mmbrid);
            return ptr ? *(const member_desc **)ptr : NULL;
        }

        const member_desc *GetMemberDescByName(const char *name) const {
            const void *ptr = mmbrnm_mdesc_.get(name);
            return ptr ? *(const member_desc **)ptr : NULL;
        }

        const member_desc *GetMemberDescByOffset(size_t fldoffst) const {
            const void *ptr = mmbrof_mdesc_.get(&fldoffst);
            return ptr ? *(const member_desc **)ptr : NULL;
        }

        void EnumMemberDesc(enum_member_desc emdf, void *ud) const {
            ent_enum_mdesc_rec e_ud;
            e_ud.cllbk = emdf;
            e_ud.ud = ud;
            mmbrid_mdesc_.enum_elements_breakable(ent_enum_MembDesc, &e_ud);
        }


    private:
        unsigned int    nentity_id_;
        const size_t    nclass_size_;
        const size_t    nclass_max_align_;
        NEntityType     entity_type_;
        const char      *nmspace_;
        const char      *entityname_;
        vlg::alloc_func afun_;
        unsigned int    fild_num_;
        vlg::hash_map   mmbrid_mdesc_;
        vlg::hash_map   mmbrnm_mdesc_;
        vlg::hash_map   mmbrof_mdesc_;
        bool            persistent_;
        vlg::hash_map   keyid_kdesc_;
};

//-----------------------------
// nentity_desc
//-----------------------------
nentity_desc::nentity_desc(unsigned int nentity_id,
                           size_t nclass_size,
                           size_t nclass_max_align,
                           NEntityType nentity_type,
                           const char *nmspace,
                           const char *nentity_name,
                           vlg::alloc_func afun,
                           unsigned int field_num,
                           bool persistent) : impl_(NULL)
{
    impl_ = new nentity_desc_impl(nentity_id,
                                  nclass_size,
                                  nclass_max_align,
                                  nentity_type,
                                  nmspace,
                                  nentity_name,
                                  afun,
                                  field_num,
                                  persistent);
    impl_->init();
}

nentity_desc::~nentity_desc()
{
    if(impl_) {
        delete impl_;
    }
}

vlg::RetCode nentity_desc::add_member_desc(const member_desc *mmbrdesc)
{
    return impl_->add_member_desc(mmbrdesc);
}

vlg::RetCode nentity_desc::add_key_desc(const key_desc *keydesc)
{
    return impl_->add_key_desc(keydesc);
}

unsigned int nentity_desc::get_nclass_id()  const
{
    return impl_->get_nentity_id();
}

size_t nentity_desc::get_nclass_size()  const
{
    return impl_->get_nclass_size();
}

size_t nentity_desc::get_nclass_max_align() const
{
    return impl_->get_nclass_max_align();
}

NEntityType nentity_desc::get_nentity_type()  const
{
    return impl_->get_nentity_type();
}

const char *nentity_desc::get_nentity_namespace() const
{
    return impl_->get_nentity_namespace();
}

const char *nentity_desc::get_nentity_name()  const
{
    return impl_->get_nentity_name();
}

vlg::alloc_func nentity_desc::get_nclass_allocation_function() const
{
    return impl_->get_nentity_alloc_f();
}

unsigned int nentity_desc::get_nentity_member_num()  const
{
    return impl_->get_field_num();
}

bool nentity_desc::is_persistent()  const
{
    return impl_->is_persistent();
}

const nentity_desc_impl *nentity_desc::get_opaque() const
{
    return impl_;
}

const member_desc  *nentity_desc::get_member_desc_by_id(unsigned int mmbrid)
const
{
    return impl_->GetMemberDescById(mmbrid);
}

const member_desc  *nentity_desc::get_member_desc_by_name(const char *name) const
{
    return impl_->GetMemberDescByName(name);
}

const member_desc  *nentity_desc::get_member_desc_by_offset(
    size_t fldoffst) const
{
    return impl_->GetMemberDescByOffset(fldoffst);
}

void nentity_desc::enum_member_descriptors(enum_member_desc emdf, void *ud) const
{
    impl_->EnumMemberDesc(emdf, ud);
}

//-----------------------------
// entity_manager_impl
//-----------------------------
struct nem_enum_nedesc_rec {
    enum_nentity_desc cllbk;
    void *ud;
};

//for all entities
void nem_enum_nedesc(const vlg::hash_map &map,
                     const void *key,
                     const void *ptr,
                     void *ud,
                     bool &brk)
{
    nem_enum_nedesc_rec *pud = static_cast<nem_enum_nedesc_rec *>(ud);
    pud->cllbk(*(*(nentity_desc **)ptr), pud->ud, brk);
}

//for enums
void nem_enum_nedesc_nenum(const vlg::hash_map &map,
                           const void *key,
                           const void *ptr,
                           void *ud,
                           bool &brk)
{
    nem_enum_nedesc_rec *pud = static_cast<nem_enum_nedesc_rec *>(ud);
    const nentity_desc *edsc = *(const nentity_desc **)ptr;
    if(edsc->get_nentity_type() == NEntityType_NENUM) {
        pud->cllbk(*(*(nentity_desc **)ptr), pud->ud, brk);
    }
}

//for classes
void nem_enum_nedesc_nclass(const vlg::hash_map &map,
                            const void *key,
                            const void *ptr,
                            void *ud,
                            bool &brk)
{
    nem_enum_nedesc_rec *pud = static_cast<nem_enum_nedesc_rec *>(ud);
    const nentity_desc *edsc = *(const nentity_desc **)ptr;
    if(edsc->get_nentity_type() == NEntityType_NCLASS) {
        pud->cllbk(*(*(nentity_desc **)ptr), pud->ud, brk);
    }
}

class nentity_manager_impl {
    public:
        nentity_manager_impl() :
            id_(sid_++),
            num_nentity_(0),
            num_nenum_(0),
            num_nclass_(0),
            fakeid_edesc_(vlg::sngl_ptr_obj_mng(), sizeof(int)),
            entnm_edesc_(vlg::sngl_ptr_obj_mng(), vlg::sngl_cstr_obj_mng()),
            entid_edesc_(vlg::sngl_ptr_obj_mng(), sizeof(unsigned int)),
            fake_id_(-1) {
            log_ = get_nclass_logger("entity_manager_impl");
            IFLOG(trc(TH_ID, LS_CTR "%s", __func__))
        }

        ~nentity_manager_impl() {
            IFLOG(trc(TH_ID, LS_DTR "%s", __func__))
        }

        vlg::RetCode init() {
            IFLOG(trc(TH_ID, LS_OPN "%s", __func__))
            RETURN_IF_NOT_OK(fakeid_edesc_.init(HM_SIZE_SMALL))
            RETURN_IF_NOT_OK(entnm_edesc_.init(HM_SIZE_SMALL))
            RETURN_IF_NOT_OK(entid_edesc_.init(HM_SIZE_SMALL))
            IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
            return vlg::RetCode_OK;
        }

        int next_fake_id() {
            return ++fake_id_;
        }

        int current_fake_id() {
            return fake_id_;
        }

        const vlg::hash_map &get_fake_id_entity_desc_map() {
            return fakeid_edesc_;
        }

        const vlg::hash_map &get_entity_id_entity_desc_map() {
            return entid_edesc_;
        }

        vlg::RetCode get_entity_desc(unsigned int nclass_id,
                                     nentity_desc const **edesc) const {
            const nentity_desc **ptr = (const nentity_desc **)entid_edesc_.get(&nclass_id);
            if(ptr) {
                *edesc = *ptr;
                return vlg::RetCode_OK;
            }
            IFLOG(wrn(TH_ID, LS_CLO "%s(nclass_id:%d) - not found in nem.", __func__,
                      nclass_id))
            return vlg::RetCode_KO;
        }

        vlg::RetCode get_entity_desc(const char *entityname,
                                     nentity_desc const **edesc) const {
            const nentity_desc **ptr = (const nentity_desc **)entnm_edesc_.get(entityname);
            if(ptr) {
                *edesc = *ptr;
                return vlg::RetCode_OK;
            }
            IFLOG(wrn(TH_ID, LS_CLO "%s(classname:%s) - not found in nem.", __func__,
                      entityname))
            return vlg::RetCode_KO;
        }

        void enum_entity_descs(enum_nentity_desc eedf, void *ud) const {
            IFLOG(trc(TH_ID, LS_OPN "%s(cllbk:%p)", __func__, eedf))
            nem_enum_nedesc_rec e_ud;
            e_ud.cllbk = eedf;
            e_ud.ud = ud;
            fakeid_edesc_.enum_elements_breakable(nem_enum_nedesc, &e_ud);
            IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        }

        void enum_enum_descs(enum_nentity_desc eedf, void *ud) const {
            IFLOG(trc(TH_ID, LS_OPN "%s(cllbk:%p)", __func__, eedf))
            nem_enum_nedesc_rec e_ud;
            e_ud.cllbk = eedf;
            e_ud.ud = ud;
            fakeid_edesc_.enum_elements_breakable(nem_enum_nedesc_nenum, &e_ud);
            IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        }

        void enum_class_descs(enum_nentity_desc eedf, void *ud) const {
            IFLOG(trc(TH_ID, LS_OPN "%s(cllbk:%p)", __func__, eedf))
            nem_enum_nedesc_rec e_ud;
            e_ud.cllbk = eedf;
            e_ud.ud = ud;
            fakeid_edesc_.enum_elements_breakable(nem_enum_nedesc_nclass, &e_ud);
            IFLOG(trc(TH_ID, LS_CLO "%s", __func__))
        }

        unsigned int entity_count() const {
            return num_nentity_;
        }

        unsigned int enum_count() const {
            return num_nenum_;
        }

        unsigned int class_count() const {
            return num_nclass_;
        }

        const char *get_class_name(unsigned int nclass_id) const {
            const nentity_desc **ptr = (const nentity_desc **)entid_edesc_.get(&nclass_id);
            return ptr ? (*ptr)->get_nentity_name() : NULL;
        }

        vlg::RetCode extend(const nentity_desc *nentity_desc) {
            IFLOG(trc(TH_ID, LS_OPN "%s(edsc:%p)", __func__, nentity_desc))
            unsigned int eid = 0;
            switch(nentity_desc->get_nentity_type()) {
                case NEntityType_NENUM:
                    num_nenum_++;
                    break;
                case NEntityType_NCLASS:
                    num_nclass_++;
                    eid = nentity_desc->get_nclass_id();
                    if(entid_edesc_.put(&eid, &nentity_desc)) {
                        IFLOG(cri(TH_ID, LS_CLO, "%s", __func__))
                        return vlg::RetCode_GENERR;
                    }
                    break;
                default:
                    IFLOG(err(TH_ID, LS_CLO, "%s", __func__))
                    return vlg::RetCode_KO;
            }
            int fake_id = next_fake_id();
            const char *enm = nentity_desc->get_nentity_name();
            if(fakeid_edesc_.put(&fake_id, &nentity_desc)) {
                IFLOG(cri(TH_ID, LS_CLO, "%s", __func__))
                return vlg::RetCode_GENERR;
            }
            if(entnm_edesc_.put(enm, &nentity_desc)) {
                IFLOG(cri(TH_ID, LS_CLO, "%s", __func__))
                return vlg::RetCode_GENERR;
            }
            num_nentity_++;
            IFLOG(trc(TH_ID, LS_CLO
                      "%s(num_enum:%d, num_nclass:%d, num_entity:%d)", __func__,
                      num_nenum_, num_nclass_, num_nentity_))
            return vlg::RetCode_OK;
        }

        vlg::RetCode extend(nentity_manager_impl *nem) {
            IFLOG(trc(TH_ID, LS_OPN "%s(nem:%p)", __func__, nem))
            nem->entnm_edesc_.start_iteration();
            nentity_desc *nentity_desc = NULL;
            while(!nem->entnm_edesc_.next(NULL, &nentity_desc)) {
                int fake_id = next_fake_id();
                const char *enm = nentity_desc->get_nentity_name();
                if(fakeid_edesc_.put(&fake_id, &nentity_desc)) {
                    IFLOG(cri(TH_ID, LS_CLO, "%s", __func__))
                    return vlg::RetCode_GENERR;
                }
                if(entnm_edesc_.put(enm, &nentity_desc)) {
                    IFLOG(cri(TH_ID, LS_CLO, "%s", __func__))
                    return vlg::RetCode_GENERR;
                }
                if(nentity_desc->get_nentity_type() == NEntityType_NCLASS) {
                    unsigned int eid = nentity_desc->get_nclass_id();
                    if(entid_edesc_.put(&eid, &nentity_desc)) {
                        IFLOG(cri(TH_ID, LS_CLO, "%s", __func__))
                        return vlg::RetCode_GENERR;
                    }
                }
            }
            num_nentity_ += nem->num_nentity_;
            num_nenum_   += nem->num_nenum_;
            num_nclass_  += nem->num_nclass_;
            IFLOG(trc(TH_ID, LS_CLO
                      "%s(num_nenum:%d, num_nclass:%d, num_nentity:%d)", __func__,
                      num_nenum_, num_nclass_, num_nentity_))
            return vlg::RetCode_OK;
        }

        vlg::RetCode extend(const char *model_name) {
            IFLOG(trc(TH_ID, LS_OPN "%s(model_name:%s)", __func__, model_name))
            if(!model_name || !strlen(model_name)) {
                IFLOG(err(TH_ID, LS_CLO "%s", __func__))
                return vlg::RetCode_BADARG;
            }
#ifdef WIN32
            wchar_t w_model_name[VLG_MDL_NAME_LEN] = {0};
            swprintf(w_model_name, VLG_MDL_NAME_LEN, L"%hs", model_name);
            void *dynalib = vlg::dynamic_lib_open(w_model_name);
#endif
#ifdef __linux
            char slib_name[VLG_MDL_NAME_LEN] = {0};
            sprintf(slib_name, "lib%s.so", model_name);
            void *dynalib = vlg::dynamic_lib_open(slib_name);
#endif
#if defined (__MACH__) || defined (__APPLE__)
            char slib_name[VLG_MDL_NAME_LEN] = {0};
            sprintf(slib_name, "lib%s.dylib", model_name);
            void *dynalib = vlg::dynamic_lib_open(slib_name);
#endif
            if(!dynalib) {
                IFLOG(err(TH_ID, LS_CLO "%s() - failed loading dynamic-lib for model:%s",
                          __func__, model_name))
                return vlg::RetCode_KO;
            }
            char nem_ep_f[VLG_MDL_NAME_LEN] = {0};
            sprintf(nem_ep_f, "get_em_%s", model_name);
            nentity_manager_func nem_f =
                (nentity_manager_func)vlg::dynamic_lib_load_symbol(dynalib, nem_ep_f);
            if(!nem_f) {
                IFLOG(err(TH_ID, LS_CLO
                          "%s() - failed to locate nem entrypoint in dynamic-lib for model:%s", __func__,
                          model_name))
                return vlg::RetCode_KO;
            }
            vlg::RetCode rcode = vlg::RetCode_OK;
            if((rcode = extend(nem_f()->impl_))) {
                IFLOG(err(TH_ID, LS_CLO "%s() - failed to extend nem for model:%s, res:%d",
                          __func__, model_name, rcode))
                return rcode;
            } else {
                char mdlv_f_n[VLG_MDL_NAME_LEN] = {0};
                sprintf(mdlv_f_n, "get_mdl_ver_%s", model_name);
                model_version_func mdlv_f =
                    (model_version_func)vlg::dynamic_lib_load_symbol(dynalib, mdlv_f_n);
                IFLOG(inf(TH_ID, LS_MDL"[DYNALOADED]%s", mdlv_f()))
                return vlg::RetCode_OK;
            }
        }

        vlg::RetCode new_nclass_instance(unsigned int nclass_id, nclass **ptr) const {
            IFLOG(trc(TH_ID, LS_OPN "%s(nclass_id:%u, ptr:%p)", __func__, nclass_id, ptr))
            const nentity_desc **edptr = (const nentity_desc **)entid_edesc_.get(&nclass_id);
            if(edptr) {
                const nentity_desc *edesc = *edptr;
                if(!(*ptr = (nclass *)edesc->get_nclass_allocation_function()(0,0))) {
                    IFLOG(cri(TH_ID, LS_CLO
                              "%s(nclass_id:%u, classname:%s, new_inst:%p) - new failed - ", __func__,
                              nclass_id,
                              edesc->get_nentity_name(), *ptr))
                    return vlg::RetCode_MEMERR;
                }
                IFLOG(trc(TH_ID, LS_CLO "%s(nclass_id:%u, classname:%s, new_inst:%p)", __func__,
                          nclass_id, edesc->get_nentity_name(), *ptr))
                return vlg::RetCode_OK;
            }
            IFLOG(wrn(TH_ID, LS_CLO "%s(nclass_id:%u) - KO -", __func__, nclass_id))
            return vlg::RetCode_KO;
        }


    private:
        unsigned int    id_;
        unsigned int    num_nentity_;
        unsigned int    num_nenum_;
        unsigned int    num_nclass_;
        vlg::hash_map   fakeid_edesc_;
        vlg::hash_map   entnm_edesc_;
        vlg::hash_map   entid_edesc_;
        int             fake_id_;

    protected:
        static unsigned int sid_;
        static nclass_logger *log_;
};

nclass_logger *nentity_manager_impl::log_ = NULL;
unsigned int nentity_manager_impl::sid_ = 0;

//-----------------------------
// nentity_manager
//-----------------------------
nentity_manager::nentity_manager() : impl_(NULL)
{
    impl_ = new nentity_manager_impl();
    impl_->init();
}

nentity_manager::~nentity_manager()
{
    if(impl_) {
        delete impl_;
    }
}

vlg::RetCode nentity_manager::get_nentity_descriptor(unsigned int nclass_id,
                                                     nentity_desc const **edesc) const
{
    return impl_->get_entity_desc(nclass_id, edesc);
}

vlg::RetCode nentity_manager::get_nentity_descriptor(const char *entityname,
                                                     nentity_desc const **edesc) const
{
    return impl_->get_entity_desc(entityname, edesc);
}

void nentity_manager::enum_nentity_descriptors(enum_nentity_desc eedf,
                                               void *ud) const
{
    impl_->enum_entity_descs(eedf, ud);
}

void nentity_manager::enum_nenum_descriptors(enum_nentity_desc eedf,
                                             void *ud) const
{
    impl_->enum_enum_descs(eedf, ud);
}

void nentity_manager::enum_nclass_descriptors(enum_nentity_desc eedf,
                                              void *ud) const
{
    impl_->enum_class_descs(eedf, ud);
}

unsigned int nentity_manager::nentity_count() const
{
    return impl_->entity_count();
}

unsigned int nentity_manager::nenum_count() const
{
    return impl_->enum_count();
}

unsigned int nentity_manager::nclass_count() const
{
    return impl_->class_count();
}

const char *nentity_manager::get_nclass_name(unsigned int nclass_id) const
{
    return impl_->get_class_name(nclass_id);
}

vlg::RetCode nentity_manager::extend(const nentity_desc *nentity_desc)
{
    return impl_->extend(nentity_desc);
}

vlg::RetCode nentity_manager::extend(nentity_manager *nem)
{
    return impl_->extend(nem->impl_);
}

vlg::RetCode nentity_manager::extend(const char *model_name)
{
    return impl_->extend(model_name);
}

vlg::RetCode nentity_manager::new_nclass_instance(unsigned int entityid,
                                                  nclass **ptr) const
{
    return impl_->new_nclass_instance(entityid, ptr);
}

//nclass MEMORY

class nclass_inst_collector : public vlg::collector {
    public:
        nclass_inst_collector() : vlg::collector("nclass") {}
};

vlg::collector *nclass_inst_coll_ = NULL;
vlg::collector &nclass_get_instance_collector()
{
    if(nclass_inst_coll_) {
        return *nclass_inst_coll_;
    }
    if(!(nclass_inst_coll_ = new nclass_inst_collector())) {
        EXIT_ACTION
    }
    return *nclass_inst_coll_;
}

vlg::collector &nclass::get_collector()
{
    return nclass_get_instance_collector();
}

//-----------------------------
// nclass
//-----------------------------
nclass_logger *nclass::log_ = NULL;

nclass::nclass()
{
    log_ = get_nclass_logger("nclass");
    IFLOG(trc(TH_ID, LS_CTR "%s(ptr:%p)", __func__, this))
}

nclass::~nclass()
{
    vlg::collector &c = get_collector();
    if((c.is_instance_collected(this))) {
        IFLOG(cri(TH_ID, LS_DTR "%s(ptr:%p)" D_W_R_COLL LS_EXUNX, __func__, this))
    }
    IFLOG(trc(TH_ID, LS_DTR "%s(ptr:%p)", __func__, this))
}

size_t nclass::get_field_size_by_id(unsigned int fldid) const
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_id(fldid);
    if(md) {
        return md->get_field_type_size();
    }
    return 0;
}

size_t nclass::get_field_size_by_name(const char *fldname) const
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_name(fldname);
    if(md) {
        return md->get_field_type_size();
    }
    return 0;
}

void *nclass::get_field_by_id(unsigned int fldid)
{
    const member_desc *md = get_nentity_descriptor()->get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        return cptr + md->get_field_offset();
    }
    return NULL;
}

void *nclass::get_field_by_name(const char *fldname)
{
    const member_desc *md = get_nentity_descriptor()->get_member_desc_by_name(
                                fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        return cptr + md->get_field_offset();
    }
    return NULL;
}

void *nclass::get_field_by_id_index(unsigned int fldid,
                                    unsigned int index)
{
    const member_desc *md = get_nentity_descriptor()->get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this) + md->get_field_offset() +
                     md->get_field_type_size()*index;
        return cptr;
    }
    return NULL;
}

void *nclass::get_field_by_name_index(const char *fldname,
                                      unsigned int index)
{
    const member_desc *md = get_nentity_descriptor()->get_member_desc_by_name(
                                fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this) + md->get_field_offset() +
                     md->get_field_type_size()*index;
        return cptr;
    }
    return NULL;
}

vlg::RetCode nclass::set_field_by_id(unsigned int fldid,
                                     const void *ptr,
                                     size_t maxlen)
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += md->get_field_offset();
        memcpy(cptr, ptr, maxlen ? min(maxlen,
                                       (md->get_field_type_size()*md->get_field_nmemb())) :
               (md->get_field_type_size()*md->get_field_nmemb()));
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::set_field_by_name(const char *fldname,
                                       const void *ptr,
                                       size_t maxlen)
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_name(fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += md->get_field_offset();
        memcpy(cptr, ptr, maxlen ? min(maxlen,
                                       (md->get_field_type_size()*md->get_field_nmemb())) :
               (md->get_field_type_size()*md->get_field_nmemb()));
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::set_field_by_id_index(unsigned int fldid,
                                           const void *ptr,
                                           unsigned int index,
                                           size_t maxlen)
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        memcpy(cptr, ptr, maxlen ? min(maxlen,
                                       (md->get_field_type_size()*md->get_field_nmemb())) :
               (md->get_field_type_size()*md->get_field_nmemb()));
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::set_field_by_name_index(const char *fldname,
                                             const void *ptr,
                                             unsigned int index,
                                             size_t maxlen)
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_name(fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        memcpy(cptr, ptr, maxlen ? min(maxlen,
                                       (md->get_field_type_size()*md->get_field_nmemb())) :
               (md->get_field_type_size()*md->get_field_nmemb()));
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::is_field_zero_by_id(unsigned int fldid,
                                         bool &res) const
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_id(fldid);
    if(md) {
        const char *cptr = reinterpret_cast<const char *>(this);
        cptr += md->get_field_offset();
        const char *zcptr = reinterpret_cast<const char *>(get_zero_object());
        zcptr += md->get_field_offset();
        res = (memcmp(cptr, zcptr,
                      (md->get_field_type_size()*md->get_field_nmemb())) == 0);
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::is_field_zero_by_name(const char *fldname,
                                           bool &res) const
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_name(fldname);
    if(md) {
        const char *cptr = reinterpret_cast<const char *>(this);
        cptr += md->get_field_offset();
        const char *zcptr = reinterpret_cast<const char *>(get_zero_object());
        zcptr += md->get_field_offset();
        res = (memcmp(cptr, zcptr,
                      (md->get_field_type_size()*md->get_field_nmemb())) == 0);
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::is_field_zero_by_id_index(unsigned int fldid,
                                               unsigned int index,
                                               unsigned int nmenb,
                                               bool &res) const
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_id(fldid);
    if(md) {
        const char *cptr = reinterpret_cast<const char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        const char *zcptr = reinterpret_cast<const char *>(get_zero_object());
        zcptr += (md->get_field_offset() + md->get_field_type_size()*index);
        res = (memcmp(cptr, zcptr, (md->get_field_type_size()*nmenb)) == 0);
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::is_field_zero_by_name_index(const char *fldname,
                                                 unsigned int index,
                                                 unsigned int nmenb,
                                                 bool &res) const
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_name(fldname);
    if(md) {
        const char *cptr = reinterpret_cast<const char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        const char *zcptr = reinterpret_cast<const char *>(get_zero_object());
        zcptr += (md->get_field_offset() + md->get_field_type_size()*index);
        res = (memcmp(cptr, zcptr, (md->get_field_type_size()*nmenb)) == 0);
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::set_field_zero_by_id(unsigned int fldid)
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += md->get_field_offset();
        const char *zcptr = reinterpret_cast<const char *>(get_zero_object());
        zcptr += md->get_field_offset();
        memcpy(cptr, zcptr, md->get_field_type_size()*md->get_field_nmemb());
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::set_field_zero_by_name(const char *fldname)
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_name(fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += md->get_field_offset();
        const char *zcptr = reinterpret_cast<const char *>(get_zero_object());
        zcptr += md->get_field_offset();
        memcpy(cptr, zcptr, md->get_field_type_size()*md->get_field_nmemb());
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::set_field_zero_by_name_index(const char *fldname,
                                                  unsigned int index,
                                                  unsigned int nmenb)
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_name(fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        const char *zcptr = reinterpret_cast<const char *>(get_zero_object());
        zcptr += (md->get_field_offset() + md->get_field_type_size()*index);
        memcpy(cptr, zcptr, md->get_field_type_size()*nmenb);
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

vlg::RetCode nclass::set_field_zero_by_id_index(unsigned int fldid,
                                                unsigned int index,
                                                unsigned int nmenb)
{
    const nentity_desc *ed = get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        const char *zcptr = reinterpret_cast<const char *>(get_zero_object());
        zcptr += (md->get_field_offset() + md->get_field_type_size()*index);
        memcpy(cptr, zcptr, md->get_field_type_size()*nmenb);
        return vlg::RetCode_OK;
    }
    return vlg::RetCode_NOTFOUND;
}

/*************************************************************
-get_field_address_by_column_number
**************************************************************/

struct ENM_FND_IDX_REC_UD {
    ENM_FND_IDX_REC_UD(char *obj_ptr,
                       const nentity_manager &nem,
                       unsigned int col_num,
                       unsigned int *current_col_num,
                       char **obj_fld_ptr,
                       bool *res_valid) :
        nem_(nem),
        obj_ptr_(obj_ptr),
        fld_mmbrd_(NULL),
        col_num_(col_num),
        current_col_num_(current_col_num),
        obj_fld_ptr_(obj_fld_ptr),
        res_valid_(res_valid) {
    }

    ~ENM_FND_IDX_REC_UD() {
    }

    const nentity_manager    &nem_;
    char                    *obj_ptr_;
    const member_desc       *fld_mmbrd_;
    unsigned int            col_num_;
    unsigned int            *current_col_num_;
    char                    **obj_fld_ptr_;
    bool                    *res_valid_;
};

void enum_edesc_fnd_idx(const vlg::hash_map &map,
                        const void *key,
                        const void *ptr,
                        void *ud,
                        bool &brk)
{
    ENM_FND_IDX_REC_UD *rud = static_cast<ENM_FND_IDX_REC_UD *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    if(mmbrd->get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd->get_field_nentity_type() == NEntityType_NENUM) {
            //treat enum as number
            if(mmbrd->get_field_nmemb() > 1) {
                for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                    if(rud->col_num_ == *(rud->current_col_num_)) {
                        *(rud->obj_fld_ptr_) = rud->obj_ptr_ +
                                               mmbrd->get_field_offset() +
                                               mmbrd->get_field_type_size()*i;
                        rud->fld_mmbrd_ = mmbrd;
                        brk = *(rud->res_valid_) = true;
                        break;
                    } else {
                        (*(rud->current_col_num_))++;
                    }
                }
            } else {
                if(rud->col_num_ == *(rud->current_col_num_)) {
                    *(rud->obj_fld_ptr_) = rud->obj_ptr_ + mmbrd->get_field_offset();
                    rud->fld_mmbrd_ = mmbrd;
                    brk = *(rud->res_valid_) = true;
                } else {
                    (*(rud->current_col_num_))++;
                }
            }
        } else {
            //class, struct is a recursive step.
            ENM_FND_IDX_REC_UD rrud = *rud;
            const nentity_desc *edsc = NULL;
            if(!rud->nem_.get_nentity_descriptor(mmbrd->get_field_user_type(), &edsc)) {
                const vlg::hash_map &nm_desc = edsc->get_opaque()->GetMap_NM_MMBRDSC();
                if(mmbrd->get_field_nmemb() > 1) {
                    for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                        rrud.obj_ptr_ = rud->obj_ptr_ +
                                        mmbrd->get_field_offset() +
                                        mmbrd->get_field_type_size()*i;
                        nm_desc.enum_elements_breakable(enum_edesc_fnd_idx, &rrud);
                        if(*(rud->res_valid_)) {
                            rud->fld_mmbrd_ = rrud.fld_mmbrd_;
                            brk = true;
                            break;
                        }
                    }
                } else {
                    rrud.obj_ptr_ = rud->obj_ptr_ + mmbrd->get_field_offset();
                    nm_desc.enum_elements_breakable(enum_edesc_fnd_idx, &rrud);
                    if(*(rud->res_valid_)) {
                        rud->fld_mmbrd_ = rrud.fld_mmbrd_;
                        brk = true;
                    }
                }
            } else {
                //ERROR
            }
        }
    } else {
        //primitive type
        if(mmbrd->get_field_vlg_type() == Type_ASCII) {
            if(rud->col_num_ == *(rud->current_col_num_)) {
                *(rud->obj_fld_ptr_) = rud->obj_ptr_ + mmbrd->get_field_offset();
                rud->fld_mmbrd_ = mmbrd;
                brk = *(rud->res_valid_) = true;
            } else {
                (*(rud->current_col_num_))++;
            }
        } else if(mmbrd->get_field_nmemb() > 1) {
            for(unsigned int i = 0; i<mmbrd->get_field_nmemb(); i++) {
                if(rud->col_num_ == *(rud->current_col_num_)) {
                    *(rud->obj_fld_ptr_) = rud->obj_ptr_ +
                                           mmbrd->get_field_offset() +
                                           mmbrd->get_field_type_size()*i;
                    rud->fld_mmbrd_ = mmbrd;
                    brk = *(rud->res_valid_) = true;
                    break;
                } else {
                    (*(rud->current_col_num_))++;
                }
            }
        } else {
            if(rud->col_num_ == *(rud->current_col_num_)) {
                *(rud->obj_fld_ptr_) = rud->obj_ptr_ + mmbrd->get_field_offset();
                rud->fld_mmbrd_ = mmbrd;
                brk = *(rud->res_valid_) = true;
            } else {
                (*(rud->current_col_num_))++;
            }
        }
    }
}

char *nclass::get_field_by_column_number(unsigned int col_num,
                                         const nentity_manager &nem,
                                         const member_desc **mdesc)
{
    if(!mdesc) {
        return NULL;
    }
    const vlg::hash_map &nm_mmdesc =
        get_nentity_descriptor()->get_opaque()->GetMap_NM_MMBRDSC();
    bool res_valid = false;
    unsigned int current_plain_idx = 0;
    char *obj_fld_ptr = NULL;
    ENM_FND_IDX_REC_UD fnd_idx_rud((char *)this,
                                   nem,
                                   col_num,
                                   &current_plain_idx,
                                   &obj_fld_ptr,
                                   &res_valid);
    nm_mmdesc.enum_elements_breakable(enum_edesc_fnd_idx, &fnd_idx_rud);
    if(fnd_idx_rud.res_valid_) {
        *mdesc = fnd_idx_rud.fld_mmbrd_;
        return obj_fld_ptr;
    } else {
        return NULL;
    }
}

/*************************************************************
-Class Persistence meths BEG
**************************************************************/
inline int FillBuff_FldValue(const void *fld_ptr,
                             Type btype,
                             size_t nmemb,
                             char *out,
                             int max_out_len)
{
    switch(btype) {
        case Type_BOOL:
            return snprintf(out, max_out_len, "%d", *(bool *)fld_ptr);
        case Type_INT16:
            return snprintf(out, max_out_len, "%d", *(short *)fld_ptr);
        case Type_UINT16:
            return snprintf(out, max_out_len, "%u", *(unsigned short *)fld_ptr);
        case Type_INT32:
            return snprintf(out, max_out_len, "%d", *(int *)fld_ptr);
        case Type_UINT32:
            return snprintf(out, max_out_len, "%u", *(unsigned int *)fld_ptr);
        case Type_INT64:
#if defined(__GNUG__) && defined(__linux)
            return snprintf(out, max_out_len, "%ld", *(int64_t *)fld_ptr);
#else
            return snprintf(out, max_out_len, "%lld", *(int64_t *)fld_ptr);
#endif
        case Type_UINT64:
#if defined(__GNUG__) && defined(__linux)
            return snprintf(out, max_out_len, "%lu", *(uint64_t *)fld_ptr);
#else
            return snprintf(out, max_out_len, "%llu", *(uint64_t *)fld_ptr);
#endif
        case Type_FLOAT32:
            return snprintf(out, max_out_len, "%f", *(float *)fld_ptr);
        case Type_FLOAT64:
            return snprintf(out, max_out_len, "%f", *(double *)fld_ptr);
        case Type_ASCII:
            if(nmemb > 1) {
                return snprintf(out, max_out_len, "%s", (char *)fld_ptr);
            } else {
                return snprintf(out, max_out_len, "%c", *(char *)fld_ptr);
            }
        default:
            return -1;
    }
}

struct prim_key_buff_value_rec_ud {
    prim_key_buff_value_rec_ud(const char *self,
                               unsigned int max_out_len,
                               char *out) :
        obj_ptr_(self),
        max_out_len_(max_out_len),
        cur_idx_(0),
        out_(out),
        res(vlg::RetCode_OK) {}

    const char *obj_ptr_;
    unsigned int max_out_len_;
    int cur_idx_;
    char *out_;
    vlg::RetCode res;
};

void enum_keyset_prim_key_buff_value(const vlg::linked_list &list,
                                     const void *ptr,
                                     void *ud)
{
    prim_key_buff_value_rec_ud *rud = static_cast<prim_key_buff_value_rec_ud *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    const char *obj_f_ptr = NULL;
    obj_f_ptr = rud->obj_ptr_ + mmbrd->get_field_offset();
    int sout = FillBuff_FldValue(obj_f_ptr, mmbrd->get_field_vlg_type(),
                                 mmbrd->get_field_nmemb(),
                                 &rud->out_[rud->cur_idx_],
                                 rud->max_out_len_);
    if(sout<0) {
        rud->res = vlg::RetCode_KO;
    } else {
        rud->cur_idx_ += sout;
        rud->max_out_len_ -= sout;
    }
}

void enum_prim_key_buff_value(const vlg::hash_map &map,
                              const void *key,
                              const void *ptr,
                              void *ud)
{
    prim_key_buff_value_rec_ud *rud = static_cast<prim_key_buff_value_rec_ud *>(ud);
    const key_desc *kdsc = *(const key_desc **)ptr;
    const vlg::linked_list &kset = kdsc->get_opaque()->get_key_field_set();
    if(kdsc->is_primary()) {
        kset.enum_elements(enum_keyset_prim_key_buff_value, rud);
    } else {
        return;
    }
}

inline void FillStr_FldValue(const void *fld_ptr,
                             Type btype,
                             size_t nmemb,
                             vlg::ascii_string *out)
{
    int max_out_len = 256;
    char buff[256] = { '\0' };
    switch(btype) {
        case Type_BOOL:
            snprintf(buff, max_out_len, "%d", *(bool *)fld_ptr);
            break;
        case Type_INT16:
            snprintf(buff, max_out_len, "%d", *(short *)fld_ptr);
            break;
        case Type_UINT16:
            snprintf(buff, max_out_len, "%u", *(unsigned short *)fld_ptr);
            break;
        case Type_INT32:
            snprintf(buff, max_out_len, "%d", *(int *)fld_ptr);
            break;
        case Type_UINT32:
            snprintf(buff, max_out_len, "%u", *(unsigned int *)fld_ptr);
            break;
        case Type_INT64:
#if defined(__GNUG__) && defined(__linux)
            snprintf(buff, max_out_len, "%ld", *(int64_t *)fld_ptr);
            break;
#else
            snprintf(buff, max_out_len, "%lld", *(int64_t *)fld_ptr);
            break;
#endif
        case Type_UINT64:
#if defined(__GNUG__) && defined(__linux)
            snprintf(buff, max_out_len, "%lu", *(uint64_t *)fld_ptr);
            break;
#else
            snprintf(buff, max_out_len, "%llu", *(uint64_t *)fld_ptr);
            break;
#endif
        case Type_FLOAT32:
            snprintf(buff, max_out_len, "%f", *(float *)fld_ptr);
            break;
        case Type_FLOAT64:
            snprintf(buff, max_out_len, "%f", *(double *)fld_ptr);
            break;
        case Type_ASCII:
            if(nmemb > 1) {
                out->append((char *)fld_ptr);
                return;
            } else {
                out->append(*(char *)fld_ptr);
                return;
            }
        default:
            return;
    }
    out->append(buff);
}

struct prim_key_str_value_rec_ud {
    prim_key_str_value_rec_ud(const char *self, vlg::ascii_string *out) :
        obj_ptr_(self),
        out_(out),
        res(vlg::RetCode_OK) {}

    const char *obj_ptr_;
    vlg::ascii_string *out_;
    vlg::RetCode res;
};

void enum_keyset_prim_key_str_value(const vlg::linked_list &list,
                                    const void *ptr,
                                    void *ud)
{
    prim_key_str_value_rec_ud *rud = static_cast<prim_key_str_value_rec_ud *>(ud);
    const member_desc *mmbrd = *(const member_desc **)ptr;
    const char *obj_f_ptr = NULL;
    obj_f_ptr = rud->obj_ptr_ + mmbrd->get_field_offset();
    FillStr_FldValue(obj_f_ptr, mmbrd->get_field_vlg_type(),
                     mmbrd->get_field_nmemb(), rud->out_);
}

void enum_prim_key_str_value(const vlg::hash_map &map,
                             const void *key,
                             const void *ptr,
                             void *ud)
{
    prim_key_str_value_rec_ud *rud = static_cast<prim_key_str_value_rec_ud *>(ud);
    const key_desc *kdsc = *(const key_desc **)ptr;
    const vlg::linked_list &kset = kdsc->get_opaque()->get_key_field_set();
    if(kdsc->is_primary()) {
        kset.enum_elements(enum_keyset_prim_key_str_value, rud);
    } else {
        return;
    }
}

vlg::RetCode nclass::get_primary_key_value_as_string(vlg::shared_pointer<char> &out_str)
{
    vlg::ascii_string str;
    const vlg::hash_map &idk_desc = get_nentity_descriptor()->get_opaque()->GetMap_KEYID_KDESC();
    prim_key_str_value_rec_ud rud((const char *)this, &str);
    idk_desc.enum_elements(enum_prim_key_str_value, &rud);
    if(!rud.res) {
        out_str.set_pointer(str.new_buffer());
    }
    return rud.res;
}



}
