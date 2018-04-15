/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "glob.h"

namespace vlg {

const std_shared_ptr_obj_mng<nclass> ncls_std_shp_omng;

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
        case Type_BYTE:
            return "vlg::Type_BYTE";
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

// key_desc_impl

struct key_desc_impl {
    key_desc_impl(unsigned short keyid, bool primary) :
        keyid_(keyid),
        primary_(primary) {
    }

    RetCode add_member_desc(member_desc &mmbrdesc) {
        fieldset_.insert(&mmbrdesc);
        return RetCode_OK;
    }

    unsigned short keyid_;
    bool primary_;
    std::set<const member_desc *> fieldset_;
};

// key_desc

key_desc::key_desc(unsigned short keyid, bool primary) :
    impl_(new key_desc_impl(keyid, primary))
{}

key_desc::~key_desc()
{}

RetCode key_desc::add_member_desc(member_desc &mmbrdesc)
{
    return impl_->add_member_desc(mmbrdesc);
}

unsigned short key_desc::get_id() const
{
    return impl_->keyid_;
}

bool key_desc::is_primary() const
{
    return impl_->primary_;
}

void key_desc::enum_member_descriptors(enum_member_desc emd_f, void *ud)  const
{
    for(auto it = impl_->fieldset_.begin(); it != impl_->fieldset_.end(); it++) {
        if(!emd_f(**it, ud)) {
            break;
        }
    }
}

// member_desc_impl

struct member_desc_impl {
    member_desc_impl(unsigned short mmbrid,
                     MemberType mmbr_type,
                     const char *mmbr_name,
                     const char *mmbr_desc,
                     Type fild_type,
                     size_t fild_offset,
                     size_t fild_type_size,
                     size_t nmemb,
                     unsigned int fild_entityid,
                     const char *fild_usr_str_type,
                     NEntityType fild_entitytype,
                     long enum_value) :
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

    unsigned short mmbrid_;
    MemberType mmbr_type_;
    const char *mmbr_name_;
    const char *mmbr_desc_;
    Type fild_type_;
    size_t fild_offset_;
    size_t fild_type_size_;
    size_t nmemb_;
    unsigned int fild_nclassid_;
    const char *fild_usr_str_type_;
    NEntityType fild_entitytype_;
    long enum_value_;
};

// member_desc

member_desc::member_desc(unsigned short mmbrid,
                         MemberType mmbr_type,
                         const char *mmbr_name,
                         const char *mmbr_desc,
                         Type fild_type,
                         size_t fild_offset,
                         size_t fild_type_size,
                         size_t nmemb,
                         unsigned int fild_entityid,
                         const char *fild_usr_str_type,
                         NEntityType fild_entitytype,
                         long enum_value) :
    impl_(new member_desc_impl(mmbrid,
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
                               enum_value))
{}

member_desc::~member_desc()
{}

unsigned short member_desc::get_id() const
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

// entity_desc_impl

struct nentity_desc_impl {
    nentity_desc_impl(unsigned int entityid,
                      size_t entity_size,
                      size_t entity_max_align,
                      NEntityType entitytype,
                      const char *nmspace,
                      const char *nclassname,
                      nclass_alloc afun,
                      unsigned int fild_num,
                      bool persistent) :
        nentity_id_(entityid),
        nclass_size_(entity_size),
        nclass_max_align_(entity_max_align),
        entity_type_(entitytype),
        nmspace_(nmspace),
        entityname_(nclassname),
        afun_(afun),
        fild_num_(fild_num),
        persistent_(persistent) {
    }

    RetCode add_member_desc(const member_desc &mmbrdesc) {
        mmbrid_mdesc_[mmbrdesc.get_id()] = &mmbrdesc;
        mmbrnm_mdesc_[mmbrdesc.get_member_name()] = &mmbrdesc;
        mmbrof_mdesc_[mmbrdesc.get_field_offset()] = &mmbrdesc;
        return RetCode_OK;
    }

    RetCode add_key_desc(const key_desc &keydesc) {
        keyid_kdesc_[keydesc.get_id()] = &keydesc;
        return RetCode_OK;
    }

    //rep
    unsigned int nentity_id_;
    const size_t nclass_size_;
    const size_t nclass_max_align_;
    NEntityType entity_type_;
    const char *nmspace_;
    const char *entityname_;
    nclass_alloc afun_;
    unsigned int fild_num_;
    std::map<unsigned short, const member_desc *> mmbrid_mdesc_;
    std::map<std::string, const member_desc *> mmbrnm_mdesc_;
    std::map<size_t, const member_desc *> mmbrof_mdesc_;
    bool persistent_;
    std::map<unsigned short, const key_desc *> keyid_kdesc_;
};

// nentity_desc

nentity_desc::nentity_desc(unsigned int nentity_id,
                           size_t nclass_size,
                           size_t nclass_max_align,
                           NEntityType nentity_type,
                           const char *nmspace,
                           const char *nentity_name,
                           nclass_alloc afun,
                           unsigned int field_num,
                           bool persistent) :
    impl_(new nentity_desc_impl(nentity_id,
                                nclass_size,
                                nclass_max_align,
                                nentity_type,
                                nmspace,
                                nentity_name,
                                afun,
                                field_num,
                                persistent))
{}

nentity_desc::~nentity_desc()
{}

RetCode nentity_desc::add_member_desc(const member_desc &mmbrdesc)
{
    return impl_->add_member_desc(mmbrdesc);
}

RetCode nentity_desc::add_key_desc(const key_desc &keydesc)
{
    return impl_->add_key_desc(keydesc);
}

unsigned int nentity_desc::get_nclass_id()  const
{
    return impl_->nentity_id_;
}

size_t nentity_desc::get_nclass_size()  const
{
    return impl_->nclass_size_;
}

NEntityType nentity_desc::get_nentity_type()  const
{
    return impl_->entity_type_;
}

const char *nentity_desc::get_nentity_namespace() const
{
    return impl_->nmspace_;
}

const char *nentity_desc::get_nentity_name()  const
{
    return impl_->entityname_;
}

nclass_alloc nentity_desc::get_nclass_allocation_function() const
{
    return impl_->afun_;
}

unsigned int nentity_desc::get_nentity_member_count()  const
{
    return impl_->fild_num_;
}

bool nentity_desc::is_persistent()  const
{
    return impl_->persistent_;
}

const member_desc *nentity_desc::get_member_desc_by_id(unsigned int mmbrid) const
{
    auto it = impl_->mmbrid_mdesc_.find(mmbrid);
    if(it != impl_->mmbrid_mdesc_.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

const member_desc *nentity_desc::get_member_desc_by_name(const char *name) const
{
    auto it = impl_->mmbrnm_mdesc_.find(name);
    if(it != impl_->mmbrnm_mdesc_.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

const member_desc *nentity_desc::get_member_desc_by_offset(size_t fldoffst) const
{
    auto it = impl_->mmbrof_mdesc_.find(fldoffst);
    if(it != impl_->mmbrof_mdesc_.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

void nentity_desc::enum_member_descriptors(enum_member_desc emdf, void *ud) const
{
    for(auto it = impl_->mmbrid_mdesc_.begin(); it != impl_->mmbrid_mdesc_.end(); it++) {
        if(!emdf(*it->second, ud)) {
            break;
        }
    }
}

const key_desc *nentity_desc::get_key_desc_by_id(unsigned short keyid) const
{
    auto it = impl_->keyid_kdesc_.find(keyid);
    if(it != impl_->keyid_kdesc_.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

void nentity_desc::enum_key_descriptors(enum_key_desc ekd_f, void *ud) const
{
    for(auto it = impl_->keyid_kdesc_.begin(); it != impl_->keyid_kdesc_.end(); it++) {
        if(!ekd_f(*it->second, ud)) {
            break;
        }
    }
}

// entity_manager_impl

struct nentity_manager_impl {
    nentity_manager_impl() {}

    RetCode extend(const nentity_desc &nent_desc) {
        if(nent_desc.get_nentity_type() == NEntityType_NCLASS) {
            entid_edesc_[nent_desc.get_nclass_id()] = &nent_desc;
        }
        entnm_edesc_[nent_desc.get_nentity_name()] = &nent_desc;
        IFLOG(trc(TH_ID, LS_CLO"[num_nenum:%d, num_nclass:%d, num_nentity:%d]", __func__,
                  entnm_edesc_.size() - entid_edesc_.size(),
                  entid_edesc_.size(),
                  entnm_edesc_.size()))
        return RetCode_OK;
    }

    void extend(const nentity_manager_impl &nem) {
        entnm_edesc_ = nem.entnm_edesc_;
        entid_edesc_ = nem.entid_edesc_;
    }

    RetCode extend(const char *model_name) {
        IFLOG(trc(TH_ID, LS_OPN "[model_name:%s]", __func__, model_name))
        if(!model_name || !strlen(model_name)) {
            IFLOG(err(TH_ID, LS_CLO, __func__))
            return RetCode_BADARG;
        }
#if defined WIN32 && defined _MSC_VER
        wchar_t w_model_name[VLG_MDL_NAME_LEN] = {0};
        swprintf(w_model_name, VLG_MDL_NAME_LEN, L"%hs", model_name);
        void *dynalib = dynamic_lib_open(w_model_name);
#endif
#ifdef __linux
        char slib_name[VLG_MDL_NAME_LEN] = {0};
        sprintf(slib_name, "lib%s.so", model_name);
        void *dynalib = dynamic_lib_open(slib_name);
#endif
#if defined (__MACH__) || defined (__APPLE__)
        char slib_name[VLG_MDL_NAME_LEN] = {0};
        sprintf(slib_name, "lib%s.dylib", model_name);
        void *dynalib = dynamic_lib_open(slib_name);
#endif
        if(!dynalib) {
            IFLOG(err(TH_ID, LS_CLO "[failed loading so-lib for model:%s]", __func__, model_name))
            return RetCode_KO;
        }
        char nem_ep_f[VLG_MDL_NAME_LEN] = {0};
        sprintf(nem_ep_f, "get_nem_%s", model_name);
        nentity_manager_func nem_f = (nentity_manager_func)dynamic_lib_load_symbol(dynalib, nem_ep_f);
        if(!nem_f) {
            IFLOG(err(TH_ID, LS_CLO "[failed to locate nem entrypoint in so-lib for model:%s]", __func__, model_name))
            return RetCode_KO;
        }
        extend(*nem_f()->impl_);
        char mdlv_f_n[VLG_MDL_NAME_LEN] = { 0 };
        sprintf(mdlv_f_n, "get_mdl_ver_%s", model_name);
        model_version_func mdlv_f = (model_version_func)dynamic_lib_load_symbol(dynalib, mdlv_f_n);
        IFLOG(inf(TH_ID, LS_MDL"model:%s [loaded]", __func__, mdlv_f()))
        return RetCode_OK;
    }

    std::map<std::string, const nentity_desc *> entnm_edesc_;
    std::map<unsigned int, const nentity_desc *> entid_edesc_;
};

// nentity_manager

nentity_manager::nentity_manager() : impl_(new nentity_manager_impl())
{}

nentity_manager::~nentity_manager()
{}

const nentity_desc *nentity_manager::get_nentity_descriptor(unsigned int nclass_id) const
{
    auto it = impl_->entid_edesc_.find(nclass_id);
    if(it != impl_->entid_edesc_.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

const nentity_desc *nentity_manager::get_nentity_descriptor(const char *nclassname) const
{
    auto it = impl_->entnm_edesc_.find(nclassname);
    if(it != impl_->entnm_edesc_.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

void nentity_manager::enum_nentity_descriptors(enum_nentity_desc eedf,
                                               void *ud) const
{
    for(auto it = impl_->entnm_edesc_.begin(); it != impl_->entnm_edesc_.end(); it++) {
        if(!eedf(*it->second, ud)) {
            break;
        }
    }
}

void nentity_manager::enum_nenum_descriptors(enum_nentity_desc eedf,
                                             void *ud) const
{
    for(auto it = impl_->entnm_edesc_.begin(); it != impl_->entnm_edesc_.end(); it++) {
        if(it->second->get_nentity_type() == NEntityType_NENUM) {
            if(!eedf(*it->second, ud)) {
                break;
            }
        }
    }
}

void nentity_manager::enum_nclass_descriptors(enum_nentity_desc eedf,
                                              void *ud) const
{
    for(auto it = impl_->entid_edesc_.begin(); it != impl_->entid_edesc_.end(); it++) {
        if(!eedf(*it->second, ud)) {
            break;
        }
    }
}

unsigned int nentity_manager::nentity_count() const
{
    return (unsigned int)impl_->entnm_edesc_.size();
}

unsigned int nentity_manager::nenum_count() const
{
    return (unsigned int)impl_->entnm_edesc_.size() - (unsigned int)impl_->entid_edesc_.size();
}

unsigned int nentity_manager::nclass_count() const
{
    return (unsigned int)impl_->entid_edesc_.size();
}

RetCode nentity_manager::extend(const nentity_desc &nentity_desc)
{
    return impl_->extend(nentity_desc);
}

RetCode nentity_manager::extend(const nentity_manager &nem)
{
    impl_->extend(*nem.impl_);
    return RetCode_OK;
}

RetCode nentity_manager::extend(const char *model_name)
{
    return impl_->extend(model_name);
}

RetCode nentity_manager::new_nclass_instance(unsigned int nclass_id, nclass **new_nclass_obj) const
{
    auto it = impl_->entid_edesc_.find(nclass_id);
    if(it != impl_->entid_edesc_.end()) {
        *new_nclass_obj = (nclass *)it->second->get_nclass_allocation_function()();
        return RetCode_OK;
    } else {
        IFLOG(wrn(TH_ID, LS_CLO "[nclass_id:%u][unknown nclass_id]", __func__, nclass_id))
        return RetCode_BADARG;
    }
}

nclass::nclass()
{}

nclass::~nclass()
{}

size_t nclass::get_field_size_by_id(unsigned int fldid) const
{
    const nentity_desc *ed = &get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_id(fldid);
    if(md) {
        return md->get_field_type_size();
    }
    return 0;
}

size_t nclass::get_field_size_by_name(const char *fldname) const
{
    const nentity_desc *ed = &get_nentity_descriptor();
    const member_desc *md = ed->get_member_desc_by_name(fldname);
    if(md) {
        return md->get_field_type_size();
    }
    return 0;
}

char *nclass::get_field_address_by_id(unsigned int fldid)
{
    const member_desc *md = get_nentity_descriptor().get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        return cptr + md->get_field_offset();
    }
    return nullptr;
}

char *nclass::get_field_address_by_name(const char *fldname)
{
    const member_desc *md = get_nentity_descriptor().get_member_desc_by_name(
                                fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        return cptr + md->get_field_offset();
    }
    return nullptr;
}

char *nclass::get_field_address_by_id_and_index(unsigned int fldid, unsigned int index)
{
    const member_desc *md = get_nentity_descriptor().get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this) + md->get_field_offset() + md->get_field_type_size()*index;
        return cptr;
    }
    return nullptr;
}

char *nclass::get_field_address_by_name_and_index(const char *fldname, unsigned int index)
{
    const member_desc *md = get_nentity_descriptor().get_member_desc_by_name(fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this) + md->get_field_offset() + md->get_field_type_size()*index;
        return cptr;
    }
    return nullptr;
}

RetCode nclass::set_field_by_id(unsigned int fldid, const void *ptr, size_t maxlen)
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += md->get_field_offset();
        memcpy(cptr, ptr, maxlen ? min(maxlen, (md->get_field_type_size()*md->get_field_nmemb())) :
               (md->get_field_type_size()*md->get_field_nmemb()));
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::set_field_by_name(const char *fldname, const void *ptr, size_t maxlen)
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_name(fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += md->get_field_offset();
        memcpy(cptr, ptr, maxlen ? min(maxlen, (md->get_field_type_size()*md->get_field_nmemb())) :
               (md->get_field_type_size()*md->get_field_nmemb()));
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::set_field_by_id_index(unsigned int fldid,
                                      const void *ptr,
                                      unsigned int index,
                                      size_t maxlen)
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        memcpy(cptr, ptr, maxlen ? min(maxlen, (md->get_field_type_size()*md->get_field_nmemb())) :
               (md->get_field_type_size()*md->get_field_nmemb()));
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::set_field_by_name_index(const char *fldname,
                                        const void *ptr,
                                        unsigned int index,
                                        size_t maxlen)
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_name(fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        memcpy(cptr, ptr, maxlen ? min(maxlen, (md->get_field_type_size()*md->get_field_nmemb())) :
               (md->get_field_type_size()*md->get_field_nmemb()));
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::is_field_zero_by_id(unsigned int fldid, bool &res) const
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_id(fldid);
    if(md) {
        const char *cptr = reinterpret_cast<const char *>(this);
        cptr += md->get_field_offset();
        const char *zcptr = reinterpret_cast<const char *>(&get_zero_object());
        zcptr += md->get_field_offset();
        res = (memcmp(cptr, zcptr, (md->get_field_type_size()*md->get_field_nmemb())) == 0);
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::is_field_zero_by_name(const char *fldname, bool &res) const
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_name(fldname);
    if(md) {
        const char *cptr = reinterpret_cast<const char *>(this);
        cptr += md->get_field_offset();
        const char *zcptr = reinterpret_cast<const char *>(&get_zero_object());
        zcptr += md->get_field_offset();
        res = (memcmp(cptr, zcptr, (md->get_field_type_size()*md->get_field_nmemb())) == 0);
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::is_field_zero_by_id_index(unsigned int fldid,
                                          unsigned int index,
                                          unsigned int nmenb,
                                          bool &res) const
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_id(fldid);
    if(md) {
        const char *cptr = reinterpret_cast<const char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        const char *zcptr = reinterpret_cast<const char *>(&get_zero_object());
        zcptr += (md->get_field_offset() + md->get_field_type_size()*index);
        res = (memcmp(cptr, zcptr, (md->get_field_type_size()*nmenb)) == 0);
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::is_field_zero_by_name_index(const char *fldname,
                                            unsigned int index,
                                            unsigned int nmenb,
                                            bool &res) const
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_name(fldname);
    if(md) {
        const char *cptr = reinterpret_cast<const char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        const char *zcptr = reinterpret_cast<const char *>(&get_zero_object());
        zcptr += (md->get_field_offset() + md->get_field_type_size()*index);
        res = (memcmp(cptr, zcptr, (md->get_field_type_size()*nmenb)) == 0);
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::set_field_zero_by_id(unsigned int fldid)
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += md->get_field_offset();
        const char *zcptr = reinterpret_cast<const char *>(&get_zero_object());
        zcptr += md->get_field_offset();
        memcpy(cptr, zcptr, md->get_field_type_size()*md->get_field_nmemb());
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::set_field_zero_by_name(const char *fldname)
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_name(fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += md->get_field_offset();
        const char *zcptr = reinterpret_cast<const char *>(&get_zero_object());
        zcptr += md->get_field_offset();
        memcpy(cptr, zcptr, md->get_field_type_size()*md->get_field_nmemb());
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::set_field_zero_by_name_index(const char *fldname,
                                             unsigned int index,
                                             unsigned int nmenb)
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_name(fldname);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        const char *zcptr = reinterpret_cast<const char *>(&get_zero_object());
        zcptr += (md->get_field_offset() + md->get_field_type_size()*index);
        memcpy(cptr, zcptr, md->get_field_type_size()*nmenb);
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
}

RetCode nclass::set_field_zero_by_id_index(unsigned int fldid,
                                           unsigned int index,
                                           unsigned int nmenb)
{
    const nentity_desc &ed = get_nentity_descriptor();
    const member_desc *md = ed.get_member_desc_by_id(fldid);
    if(md) {
        char *cptr = reinterpret_cast<char *>(this);
        cptr += (md->get_field_offset() + md->get_field_type_size()*index);
        const char *zcptr = reinterpret_cast<const char *>(&get_zero_object());
        zcptr += (md->get_field_offset() + md->get_field_type_size()*index);
        memcpy(cptr, zcptr, md->get_field_type_size()*nmenb);
        return RetCode_OK;
    }
    return RetCode_NOTFOUND;
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
        fld_mmbrd_(nullptr),
        col_num_(col_num),
        current_col_num_(current_col_num),
        obj_fld_ptr_(obj_fld_ptr),
        res_valid_(res_valid) {
    }

    const nentity_manager &nem_;
    char *obj_ptr_;
    const member_desc *fld_mmbrd_;
    unsigned int col_num_;
    unsigned int *current_col_num_;
    char **obj_fld_ptr_;
    bool *res_valid_;
};

bool enum_edesc_fnd_idx(const member_desc &mmbrd, void *ptr)
{
    ENM_FND_IDX_REC_UD *rud = (ENM_FND_IDX_REC_UD *)ptr;
    if(mmbrd.get_field_vlg_type() == Type_ENTITY) {
        if(mmbrd.get_field_nentity_type() == NEntityType_NENUM) {
            //treat enum as number
            if(mmbrd.get_field_nmemb() > 1) {
                for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                    if(rud->col_num_ == *(rud->current_col_num_)) {
                        *(rud->obj_fld_ptr_) = rud->obj_ptr_ +
                                               mmbrd.get_field_offset() +
                                               mmbrd.get_field_type_size()*i;
                        rud->fld_mmbrd_ = &mmbrd;
                        *(rud->res_valid_) = true;
                        return false;
                    } else {
                        (*(rud->current_col_num_))++;
                    }
                }
            } else {
                if(rud->col_num_ == *(rud->current_col_num_)) {
                    *(rud->obj_fld_ptr_) = rud->obj_ptr_ + mmbrd.get_field_offset();
                    rud->fld_mmbrd_ = &mmbrd;
                    *(rud->res_valid_) = true;
                    return false;
                } else {
                    (*(rud->current_col_num_))++;
                }
            }
        } else {
            //class, struct is a recursive step.
            ENM_FND_IDX_REC_UD rrud = *rud;
            const nentity_desc *edsc = nullptr;

            if((edsc = rud->nem_.get_nentity_descriptor(mmbrd.get_field_user_type()))) {
                if(mmbrd.get_field_nmemb() > 1) {
                    for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                        rrud.obj_ptr_ = rud->obj_ptr_ +
                                        mmbrd.get_field_offset() +
                                        mmbrd.get_field_type_size()*i;
                        edsc->enum_member_descriptors(enum_edesc_fnd_idx, &rrud);
                        if(*(rud->res_valid_)) {
                            rud->fld_mmbrd_ = rrud.fld_mmbrd_;
                            return false;
                        }
                    }
                } else {
                    rrud.obj_ptr_ = rud->obj_ptr_ + mmbrd.get_field_offset();
                    edsc->enum_member_descriptors(enum_edesc_fnd_idx, &rrud);
                    if(*(rud->res_valid_)) {
                        rud->fld_mmbrd_ = rrud.fld_mmbrd_;
                        return false;
                    }
                }
            } else {
                //ERROR
            }
        }
    } else {
        //primitive type
        if(mmbrd.get_field_vlg_type() == Type_ASCII || mmbrd.get_field_vlg_type() == Type_BYTE) {
            if(rud->col_num_ == *(rud->current_col_num_)) {
                *(rud->obj_fld_ptr_) = rud->obj_ptr_ + mmbrd.get_field_offset();
                rud->fld_mmbrd_ = &mmbrd;
                *(rud->res_valid_) = true;
                return false;
            } else {
                (*(rud->current_col_num_))++;
            }
        } else if(mmbrd.get_field_nmemb() > 1) {
            for(unsigned int i = 0; i<mmbrd.get_field_nmemb(); i++) {
                if(rud->col_num_ == *(rud->current_col_num_)) {
                    *(rud->obj_fld_ptr_) = rud->obj_ptr_ +
                                           mmbrd.get_field_offset() +
                                           mmbrd.get_field_type_size()*i;
                    rud->fld_mmbrd_ = &mmbrd;
                    *(rud->res_valid_) = true;
                    return false;
                } else {
                    (*(rud->current_col_num_))++;
                }
            }
        } else {
            if(rud->col_num_ == *(rud->current_col_num_)) {
                *(rud->obj_fld_ptr_) = rud->obj_ptr_ + mmbrd.get_field_offset();
                rud->fld_mmbrd_ = &mmbrd;
                *(rud->res_valid_) = true;
                return false;
            } else {
                (*(rud->current_col_num_))++;
            }
        }
    }
    return true;
}

struct offst_m_v {
    size_t foffst;
    const member_desc *fmdesc;
};

char *nclass::get_field_address_by_column_number(unsigned int col_num,
                                                 const nentity_manager &nem,
                                                 const member_desc **mdesc)
{
    static pthread_rwlock_t offst_m_l = PTHREAD_RWLOCK_INITIALIZER;
    static std::unordered_map<std::string, offst_m_v> offst_m;
    if(!mdesc) {
        return nullptr;
    }

    std::stringstream ss;
    ss << get_id() << '_' << col_num;
    {
        scoped_rd_lock rl(offst_m_l);
        auto it = offst_m.find(ss.str());
        if(it != offst_m.end()) {
            *mdesc = it->second.fmdesc;
            return reinterpret_cast<char *>(this) + it->second.foffst;
        }
    }

    bool res_valid = false;
    unsigned int current_plain_idx = 0;
    char *obj_fld_ptr = nullptr;
    ENM_FND_IDX_REC_UD fnd_idx_rud((char *)this,
                                   nem,
                                   col_num,
                                   &current_plain_idx,
                                   &obj_fld_ptr,
                                   &res_valid);

    get_nentity_descriptor().enum_member_descriptors(enum_edesc_fnd_idx, &fnd_idx_rud);
    if(fnd_idx_rud.res_valid_) {
        {
            scoped_wr_lock wl(offst_m_l);
            offst_m.insert(std::pair<std::string, offst_m_v>(ss.str(), {(size_t)(obj_fld_ptr - (char *)this), fnd_idx_rud.fld_mmbrd_}));
        }
        *mdesc = fnd_idx_rud.fld_mmbrd_;
        return obj_fld_ptr;
    }
    return nullptr;
}

/*************************************************************
-Class Persistence meths BEG
**************************************************************/

struct prim_key_buff_value_rec_ud {
    prim_key_buff_value_rec_ud(const char *self,
                               unsigned int max_out_len,
                               char *out) :
        obj_ptr_(self),
        max_out_len_(max_out_len),
        cur_idx_(0),
        out_(out),
        res(RetCode_OK) {}

    const char *obj_ptr_;
    unsigned int max_out_len_;
    int cur_idx_;
    char *out_;
    RetCode res;
};

inline void FillStr_FldValue(const void *fld_ptr,
                             Type btype,
                             size_t nmemb,
                             std::string *out)
{
    std::stringstream ss;
    switch(btype) {
        case Type_BOOL:
            ss << *(bool *)fld_ptr;
            break;
        case Type_INT16:
            ss << *(short *)fld_ptr;
            break;
        case Type_UINT16:
            ss << *(unsigned short *)fld_ptr;
            break;
        case Type_INT32:
            ss << *(int *)fld_ptr;
            break;
        case Type_UINT32:
            ss << *(unsigned int *)fld_ptr;
            break;
        case Type_INT64:
            ss << *(int64_t *)fld_ptr;
        case Type_UINT64:
            ss << *(uint64_t *)fld_ptr;
        case Type_FLOAT32:
            ss << *(float *)fld_ptr;
            break;
        case Type_FLOAT64:
            ss << *(double *)fld_ptr;
            break;
        case Type_ASCII:
            if(nmemb > 1) {
                out->append((char *)fld_ptr);
                return;
            } else {
                *out += *(char *)fld_ptr;
                return;
            }
        case Type_BYTE:
            append_hex_str((char *)fld_ptr, nmemb, *out);
            break;
        default:
            return;
    }
    out->append(ss.str());
}

struct prim_key_str_value_rec_ud {
    prim_key_str_value_rec_ud(const char *self, std::string *out) :
        obj_ptr_(self),
        out_(out),
        res(RetCode_OK) {}

    const char *obj_ptr_;
    std::string *out_;
    RetCode res;
};

bool enum_prim_key_str_value(const key_desc &kdsc,
                             void *ud)
{
    prim_key_str_value_rec_ud *rud = static_cast<prim_key_str_value_rec_ud *>(ud);
    if(kdsc.is_primary()) {
        std::for_each(kdsc.impl_->fieldset_.begin(), kdsc.impl_->fieldset_.end(), [&](auto mdsc) {
            const char *obj_f_ptr = nullptr;
            obj_f_ptr = rud->obj_ptr_ + mdsc->get_field_offset();
            FillStr_FldValue(obj_f_ptr, mdsc->get_field_vlg_type(), mdsc->get_field_nmemb(), rud->out_);
        });
        return false;
    } else {
        return true;
    }
}

RetCode nclass::get_primary_key_value_as_string(std::unique_ptr<char> &out_str)
{
    std::string str;
    prim_key_str_value_rec_ud rud((const char *)this, &str);
    get_nentity_descriptor().enum_key_descriptors(enum_prim_key_str_value, &rud);
    if(!rud.res) {
        out_str = std::unique_ptr<char>(strdup(str.c_str()));
    }
    return rud.res;
}

}
