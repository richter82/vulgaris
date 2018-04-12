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

#include "compiler.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//--C++
#define VLG_COMP_CPP_TYPE_UNSIGN    "unsigned"
#define VLG_COMP_CPP_TYPE_BOOL      "bool"
#define VLG_COMP_CPP_TYPE_INT       "int"
#define VLG_COMP_CPP_TYPE_SHORT     "short"
#define VLG_COMP_CPP_TYPE_LONG      "long"
#define VLG_COMP_CPP_TYPE_INT64     "int64_t"
#define VLG_COMP_CPP_TYPE_UINT64    "uint64_t"
#define VLG_COMP_CPP_TYPE_FLOAT     "float"
#define VLG_COMP_CPP_TYPE_DOUBLE    "double"
#define VLG_COMP_CPP_TYPE_CHAR      "char"
#define VLG_COMP_CPP_TYPE_WCHAR     "wchar_t"

//--Java
#define VLG_COMP_JAVA_TYPE_BYTE      "Byte"
#define VLG_COMP_JAVA_TYPE_BOOL      "Boolean"
#define VLG_COMP_JAVA_TYPE_INT       "Integer"
#define VLG_COMP_JAVA_TYPE_SHORT     "Short"
#define VLG_COMP_JAVA_TYPE_LONG      "Long"
#define VLG_COMP_JAVA_TYPE_FLOAT     "Float"
#define VLG_COMP_JAVA_TYPE_DOUBLE    "Double"
#define VLG_COMP_JAVA_TYPE_CHAR      "Character"
#define VLG_COMP_JAVA_TYPE_STRING    "String"

#define NOTFNDINHASH "key not found in hash"

namespace vlg {


// VLG_COMP_ARCH: x86_64
// VLG_COMP_OS: unix
// VLG_COMP_LANG: CPP
// VLG_COMP_TCOMP: GCC

static size_t  get_in_arch_type_size_x86_64_unix_CPP_GCC(Type type)
{
    switch(type) {
        case Type_BOOL:
            return 1;
        case Type_INT16:
            return 2;
        case Type_UINT16:
            return 2;
        case Type_INT32:
            return 4;
        case Type_UINT32:
            return 4;
        case Type_INT64:
            return 8;
        case Type_UINT64:
            return 8;
        case Type_FLOAT32:
            return 4;
        case Type_FLOAT64:
            return 8;
        case Type_ASCII:
            return 1;
        case Type_BYTE:
            return 1;
        default:
            return 0;
    }
    return 0;
}


// VLG_COMP_ARCH: x86_64
// VLG_COMP_OS: win
// VLG_COMP_LANG: CPP
// VLG_COMP_TCOMP: MSVC

static size_t  get_in_arch_type_size_x86_64_win_CPP_MSVC(Type type)
{
    switch(type) {
        case Type_BOOL:
            return 1;
        case Type_INT16:
            return 2;
        case Type_UINT16:
            return 2;
        case Type_INT32:
            return 4;
        case Type_UINT32:
            return 4;
        case Type_INT64:
            return 8;
        case Type_UINT64:
            return 8;
        case Type_FLOAT32:
            return 4;
        case Type_FLOAT64:
            return 8;
        case Type_ASCII:
            return 1;
        case Type_BYTE:
            return 1;
        default:
            return 0;
    }
    return 0;
}

static size_t  get_in_arch_type_size_x86_64_unix_CPP(Type type,
                                                     VLG_COMP_TCOMP tcomp)
{
    switch(tcomp) {
        case VLG_COMP_TCOMP_GCC:
            return get_in_arch_type_size_x86_64_unix_CPP_GCC(type);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

static size_t  get_in_arch_type_size_x86_64_win_CPP(Type type,
                                                    VLG_COMP_TCOMP tcomp)
{
    switch(tcomp) {
        case VLG_COMP_TCOMP_MSVC:
            return get_in_arch_type_size_x86_64_win_CPP_MSVC(type);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

static size_t  GetInArchTypeSize_x86_64_unix(Type type,
                                             VLG_COMP_LANG lang,
                                             VLG_COMP_TCOMP tcomp)
{
    switch(lang) {
        case VLG_COMP_LANG_CPP:
            return get_in_arch_type_size_x86_64_unix_CPP(type, tcomp);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

static size_t  GetInArchTypeSize_x86_64_win(Type type,
                                            VLG_COMP_LANG lang,
                                            VLG_COMP_TCOMP tcomp)
{
    switch(lang) {
        case VLG_COMP_LANG_CPP:
            return get_in_arch_type_size_x86_64_win_CPP(type, tcomp);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

static size_t  GetInArchTypeSize_x86_64(Type type,
                                        VLG_COMP_OS os,
                                        VLG_COMP_LANG lang,
                                        VLG_COMP_TCOMP tcomp)
{
    switch(os) {
        case VLG_COMP_OS_win:
            return GetInArchTypeSize_x86_64_win(type, lang, tcomp);
        case VLG_COMP_OS_unix:
            return GetInArchTypeSize_x86_64_unix(type, lang, tcomp);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

size_t  get_in_arch_type_size(Type type,
                              VLG_COMP_ARCH arch,
                              VLG_COMP_OS os,
                              VLG_COMP_LANG lang,
                              VLG_COMP_TCOMP tcomp)
{
    switch(arch) {
        case VLG_COMP_ARCH_x86_64:
            return GetInArchTypeSize_x86_64(type, os, lang, tcomp);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

/*
On Network-protocol dependant type size
*/
size_t  get_network_type_size(Type type)
{
    switch(type) {
        case Type_UNDEFINED:
        case Type_ENTITY:
            return 0;
        case Type_BOOL:
            return 1;
        case Type_INT16:
            return 2;
        case Type_UINT16:
            return 2;
        case Type_INT32:
            return 4;
        case Type_UINT32:
            return 4;
        case Type_INT64:
            return 8;
        case Type_UINT64:
            return 8;
        case Type_FLOAT32:
            return 4;
        case Type_FLOAT64:
            return 8;
        case Type_ASCII:
            return 1;
        case Type_BYTE:
            return 1;
        default:
            return 0;
    }
}

/***********************************
VLG_MEMBER_DESC_COMP
***********************************/
member_desc_comp::member_desc_comp(unsigned short mmbrid,
                                   MemberType mmbr_type,
                                   const char *mmbr_name,
                                   const char *mmbr_desc,
                                   Type fild_type,
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
    nmemb_(nmemb),
    fild_nclassid_(fild_entityid),
    fild_usr_str_type_(fild_usr_str_type),
    fild_entitytype_(fild_entitytype),
    enum_value_(enum_value)
{}

unsigned short member_desc_comp::get_member_id() const
{
    return mmbrid_;
}

MemberType member_desc_comp::get_member_type() const
{
    return mmbr_type_;
}

const char *member_desc_comp::get_member_name() const
{
    return mmbr_name_;
}

const char *member_desc_comp::get_member_desc() const
{
    return mmbr_desc_;
}

/*
Field section
*/
Type member_desc_comp::get_field_type() const
{
    return fild_type_;
}

size_t member_desc_comp::get_field_offset(VLG_COMP_ARCH arch,
                                          VLG_COMP_OS os,
                                          VLG_COMP_LANG lang,
                                          VLG_COMP_TCOMP tcomp) const
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    auto it = fild_offset_map_.find(key);
    return it->second;
}

size_t member_desc_comp::get_field_type_size(VLG_COMP_ARCH arch,
                                             VLG_COMP_OS os,
                                             VLG_COMP_LANG lang,
                                             VLG_COMP_TCOMP tcomp) const
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    auto it = fild_type_size_map_.find(key);
    return it->second;
}

size_t member_desc_comp::get_nmemb() const
{
    return nmemb_;
}

unsigned int member_desc_comp::get_field_nclassid() const
{
    return fild_nclassid_;
}

const char *member_desc_comp::get_field_usr_str_type() const
{
    return fild_usr_str_type_;
}

NEntityType member_desc_comp::get_field_entity_type() const
{
    return fild_entitytype_;
}

long member_desc_comp::get_enum_value() const
{
    return enum_value_;
}

void member_desc_comp::set_field_offset(size_t val,
                                        VLG_COMP_ARCH arch,
                                        VLG_COMP_OS os,
                                        VLG_COMP_LANG lang,
                                        VLG_COMP_TCOMP tcomp)
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    fild_offset_map_[key] = val;
}

void member_desc_comp::set_field_type_size(size_t val,
                                           VLG_COMP_ARCH arch,
                                           VLG_COMP_OS os,
                                           VLG_COMP_LANG lang,
                                           VLG_COMP_TCOMP tcomp)
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    fild_type_size_map_[key] = val;
}


/***********************************
VLG_ENTITY_DESC_COMP
***********************************/
entity_desc_comp::entity_desc_comp(unsigned int entityid,
                                   NEntityType entitytype,
                                   const char *nmspace,
                                   const char *nclassname,
                                   vlg::alloc_func afun,
                                   unsigned int fild_num,
                                   bool persistent) :
    entityid_(entityid),
    entitytype_(entitytype),
    nmspace_(nmspace),
    entityname_(nclassname),
    afun_(afun),
    fild_num_(fild_num),
    persistent_(persistent)
{}

RetCode entity_desc_comp::extend(std::map<std::string, member_desc_comp *> &mmbrmap)
{
    mmbrnm_mdesc_ = mmbrmap;
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        mmbrid_mdesc_[mdesc->second->get_member_id()] = mdesc->second;
    }
    return vlg::RetCode_OK;
}

RetCode entity_desc_comp::extend(std::map<std::string, member_desc_comp *> &mmbrmap,
                                 std::map<unsigned short, key_desc_comp *> &keymap)
{
    extend(mmbrmap);
    if(persistent_) {
        keyid_kdesc_ = keymap;
    }
    return vlg::RetCode_OK;
}

RetCode entity_desc_comp::add_key_desc(key_desc_comp *keydesc)
{
    unsigned short keyid = keydesc->get_key_id();
    keyid_kdesc_[keyid] = keydesc;
    return vlg::RetCode_OK;
}

unsigned int entity_desc_comp::get_entityid()  const
{
    return entityid_;
}

size_t entity_desc_comp::get_size(VLG_COMP_ARCH arch,
                                  VLG_COMP_OS os,
                                  VLG_COMP_LANG lang,
                                  VLG_COMP_TCOMP tcomp) const
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    return entity_size_map_.find(key)->second;
}

size_t entity_desc_comp::get_entity_max_align(VLG_COMP_ARCH arch,
                                              VLG_COMP_OS os,
                                              VLG_COMP_LANG lang,
                                              VLG_COMP_TCOMP tcomp) const
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    return entity_max_align_map_.find(key)->second;
}

NEntityType entity_desc_comp::get_nentity_type()  const
{
    return entitytype_;
}

const char *entity_desc_comp::get_entity_namespace() const
{
    return nmspace_;
}

const char *entity_desc_comp::get_nentity_name()  const
{
    return entityname_;
}

vlg::alloc_func entity_desc_comp::get_entity_alloc_f() const
{
    return afun_;
}

unsigned int entity_desc_comp::get_field_num()  const
{
    return fild_num_;
}

bool entity_desc_comp::is_persistent()  const
{
    return persistent_;
}

std::map<unsigned short, member_desc_comp *> &entity_desc_comp::get_map_id_MMBRDSC()
{
    return mmbrid_mdesc_;
}

const std::map<std::string, member_desc_comp *> &entity_desc_comp::get_map_name_MMBRDSC() const
{
    return mmbrnm_mdesc_;
}

const std::map<unsigned short, key_desc_comp *> &entity_desc_comp::get_map_keyid_KDESC() const
{
    return keyid_kdesc_;
}

std::map<unsigned short, key_desc_comp *> &entity_desc_comp::get_map_keyid_KDESC_mod()
{
    return keyid_kdesc_;
}

const member_desc_comp *entity_desc_comp::get_member_desc_by_id(unsigned int mmbrid) const
{
    return mmbrid_mdesc_.find(mmbrid)->second;
}

const member_desc_comp *entity_desc_comp::get_member_desc_by_name(const char *name) const
{
    return mmbrnm_mdesc_.find(name)->second;
}

void entity_desc_comp::enum_member_desc(enum_member_desc_comp_func func) const
{
    return ;
}

void entity_desc_comp::set_entity_size(size_t val,
                                       VLG_COMP_ARCH arch,
                                       VLG_COMP_OS os,
                                       VLG_COMP_LANG lang,
                                       VLG_COMP_TCOMP tcomp)
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    entity_size_map_[key] = val;
}

void entity_desc_comp::set_entity_max_align(size_t val,
                                            VLG_COMP_ARCH arch,
                                            VLG_COMP_OS os,
                                            VLG_COMP_LANG lang,
                                            VLG_COMP_TCOMP tcomp)
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    entity_max_align_map_[key] = val;
}

/***********************************
key_desc_comp
***********************************/
key_desc_comp::key_desc_comp(unsigned short keyid, bool primary) :
    keyid_(keyid),
    primary_(primary)
{}

RetCode key_desc_comp::init(std::set<member_desc_comp *> &fldset)
{
    fildset_ = fldset;
    return vlg::RetCode_OK;
}

RetCode key_desc_comp::add_member_desc(member_desc_comp *const mmbrdesc)
{
    fildset_.insert(mmbrdesc);
    return vlg::RetCode_OK;
}

unsigned short key_desc_comp::get_key_id() const
{
    return keyid_;
}

bool key_desc_comp::is_primary() const
{
    return primary_;
}

const std::set<member_desc_comp *> &key_desc_comp::get_key_member_set() const
{
    return fildset_;
}

std::set<member_desc_comp *> &key_desc_comp::get_key_member_set_m()
{
    return fildset_;
}


RetCode get_zero_val_for_VLG_TYPE(Type type, std::string &out)
{
    switch(comp_cfg.lang) {
        case VLG_COMP_LANG_C:
            return vlg::RetCode_UNSP;
            break;
        case VLG_COMP_LANG_CPP:
            switch(type) {
                case Type_BOOL:
                    out.assign("false");
                    return vlg::RetCode_OK;
                case Type_INT16:
                    out.assign("0");
                    return vlg::RetCode_OK;
                case Type_UINT16:
                    out.assign("0");
                    return vlg::RetCode_OK;
                case Type_INT32:
                    out.assign("0");
                    return vlg::RetCode_OK;
                case Type_UINT32:
                    out.assign("0");
                    return vlg::RetCode_OK;
                case Type_INT64:
                    out.assign("0");
                    return vlg::RetCode_OK;
                case Type_UINT64:
                    out.assign("0");
                    return vlg::RetCode_OK;
                case Type_FLOAT32:
                    out.assign("0");
                    return vlg::RetCode_OK;
                case Type_FLOAT64:
                    out.assign("0.0");
                    return vlg::RetCode_OK;
                case Type_ASCII:
                    out.assign("\'\\0\'");
                    return vlg::RetCode_OK;
                case Type_BYTE:
                    out.assign("0");
                    return vlg::RetCode_OK;
                default:
                    return vlg::RetCode_KO;
            }
            break;
        case VLG_COMP_LANG_JAVA:
            switch(type) {
                case Type_BOOL:
                    out.assign("false");
                    return vlg::RetCode_OK;
                case Type_INT16:
                case Type_UINT16:
                    out.assign("(short)0");
                    return vlg::RetCode_OK;
                case Type_INT32:
                case Type_UINT32:
                    out.assign("0");
                    return vlg::RetCode_OK;
                case Type_INT64:
                case Type_UINT64:
                    out.assign("0L");
                    return vlg::RetCode_OK;
                case Type_FLOAT32:
                    out.assign("0f");
                    return vlg::RetCode_OK;
                case Type_FLOAT64:
                    out.assign("0.0");
                    return vlg::RetCode_OK;
                case Type_ASCII:
                    out.assign("(char)0x0000");
                    return vlg::RetCode_OK;
                case Type_BYTE:
                    out.assign("0");
                    return vlg::RetCode_OK;
                default:
                    return vlg::RetCode_KO;
            }
            break;
        default:
            return vlg::RetCode_KO;
    }
}

RetCode target_type_from_builtin_VLG_TYPE(member_desc_comp &mdsc,
                                          std::string &out)
{
    switch(comp_cfg.lang) {
        case VLG_COMP_LANG_C:
            return vlg::RetCode_UNSP;
            break;
        case VLG_COMP_LANG_CPP:
            switch(mdsc.get_field_type()) {
                case Type_BOOL:
                    out.assign(VLG_COMP_CPP_TYPE_BOOL);
                    return vlg::RetCode_OK;
                case Type_INT16:
                    out.assign(VLG_COMP_CPP_TYPE_SHORT);
                    return vlg::RetCode_OK;
                case Type_UINT16:
                    out.assign(VLG_COMP_CPP_TYPE_UNSIGN
                               CR_TK_SP
                               VLG_COMP_CPP_TYPE_SHORT);
                    return vlg::RetCode_OK;
                case Type_INT32:
                    out.assign(VLG_COMP_CPP_TYPE_INT);
                    return vlg::RetCode_OK;
                case Type_UINT32:
                    out.assign(VLG_COMP_CPP_TYPE_UNSIGN
                               CR_TK_SP
                               VLG_COMP_CPP_TYPE_INT);
                    return vlg::RetCode_OK;
                case Type_INT64:
                    out.assign(VLG_COMP_CPP_TYPE_INT64);
                    return vlg::RetCode_OK;
                case Type_UINT64:
                    out.assign(VLG_COMP_CPP_TYPE_UINT64);
                    return vlg::RetCode_OK;
                case Type_FLOAT32:
                    out.assign(VLG_COMP_CPP_TYPE_FLOAT);
                    return vlg::RetCode_OK;
                case Type_FLOAT64:
                    out.assign(VLG_COMP_CPP_TYPE_DOUBLE);
                    return vlg::RetCode_OK;
                case Type_ASCII:
                    out.assign(VLG_COMP_CPP_TYPE_CHAR);
                    return vlg::RetCode_OK;
                case Type_BYTE:
                    out.assign(VLG_COMP_CPP_TYPE_UNSIGN
                               CR_TK_SP
                               VLG_COMP_CPP_TYPE_CHAR);
                    return vlg::RetCode_OK;
                default:
                    return vlg::RetCode_KO;
            }
            break;
        case VLG_COMP_LANG_JAVA:
            switch(mdsc.get_field_type()) {
                case Type_BOOL:
                    out.assign(VLG_COMP_JAVA_TYPE_BOOL);
                    return vlg::RetCode_OK;
                case Type_INT16:
                    out.assign(VLG_COMP_JAVA_TYPE_SHORT);
                    return vlg::RetCode_OK;
                case Type_UINT16:
                    out.assign(VLG_COMP_JAVA_TYPE_SHORT);
                    return vlg::RetCode_OK;
                case Type_INT32:
                    out.assign(VLG_COMP_JAVA_TYPE_INT);
                    return vlg::RetCode_OK;
                case Type_UINT32:
                    out.assign(VLG_COMP_JAVA_TYPE_INT);
                    return vlg::RetCode_OK;
                case Type_INT64:
                    out.assign(VLG_COMP_JAVA_TYPE_LONG);
                    return vlg::RetCode_OK;
                case Type_UINT64:
                    out.assign(VLG_COMP_JAVA_TYPE_LONG);
                    return vlg::RetCode_OK;
                case Type_FLOAT32:
                    out.assign(VLG_COMP_JAVA_TYPE_FLOAT);
                    return vlg::RetCode_OK;
                case Type_FLOAT64:
                    out.assign(VLG_COMP_JAVA_TYPE_DOUBLE);
                    return vlg::RetCode_OK;
                case Type_ASCII:
                    out.assign((mdsc.get_nmemb() > 1) ?
                               VLG_COMP_JAVA_TYPE_STRING :
                               VLG_COMP_JAVA_TYPE_CHAR);
                    return vlg::RetCode_OK;
                case Type_BYTE:
                    out.assign(VLG_COMP_JAVA_TYPE_BYTE);
                    return vlg::RetCode_OK;
                default:
                    return vlg::RetCode_KO;
            }
            break;
        default:
            return vlg::RetCode_KO;
    }
}

RetCode target_type_from_VLG_TYPE(member_desc_comp &mdsc,
                                  std::map<std::string, entity_desc_comp *> &entitymap,
                                  std::string &out)
{
    switch(mdsc.get_field_type()) {
        case Type_UNDEFINED:
            return vlg::RetCode_KO;
        case Type_ENTITY:
            out.assign(mdsc.get_field_usr_str_type());
            return vlg::RetCode_OK;
        case Type_BOOL:
        case Type_INT16:
        case Type_UINT16:
        case Type_INT32:
        case Type_UINT32:
        case Type_INT64:
        case Type_UINT64:
        case Type_FLOAT32:
        case Type_FLOAT64:
        case Type_ASCII:
        case Type_BYTE:
            RET_ON_KO(target_type_from_builtin_VLG_TYPE(mdsc, out))
            return vlg::RetCode_OK;
        default:
            return vlg::RetCode_KO;
    }
}

RetCode printf_percent_from_VLG_TYPE(member_desc_comp &mdsc,
                                     std::string &out,
                                     bool strict_linux)
{
    switch(mdsc.get_field_type()) {
        case Type_BOOL:
            out.assign("d");
            return vlg::RetCode_OK;
        case Type_INT16:
            out.assign("d");
            return vlg::RetCode_OK;
        case Type_UINT16:
            out.assign("u");
            return vlg::RetCode_OK;
        case Type_INT32:
            out.assign("d");
            return vlg::RetCode_OK;
        case Type_UINT32:
            out.assign("u");
            return vlg::RetCode_OK;
        case Type_INT64:
            if(strict_linux) {
                out.assign("ld");
                return vlg::RetCode_OK;
            } else {
                out.assign("lld");
                return vlg::RetCode_OK;
            }
        case Type_UINT64:
            if(strict_linux) {
                out.assign("lu");
                return vlg::RetCode_OK;
            } else {
                out.assign("llu");
                return vlg::RetCode_OK;
            }
        case Type_FLOAT32:
            out.assign("f");
            return vlg::RetCode_OK;
        case Type_FLOAT64:
            out.assign("f");
            return vlg::RetCode_OK;
        case Type_ASCII:
            out.assign("c");
            return vlg::RetCode_OK;
        case Type_BYTE:
            out.assign("x");
            return vlg::RetCode_OK;
        default:
            return vlg::RetCode_KO;
    }
}

VLG_COMP_LANG lang_from_str(const char *str)
{
    if(!strcmp(str, VLG_COMP_LANG_TK_C)) {
        return VLG_COMP_LANG_C;
    } else if(!strcmp(str, VLG_COMP_LANG_TK_CPP)) {
        return VLG_COMP_LANG_CPP;
    } else if(!strcmp(str, VLG_COMP_LANG_TK_JAVA)) {
        return VLG_COMP_LANG_JAVA;
    } else if(!strcmp(str, VLG_COMP_LANG_TK_OBJC)) {
        return VLG_COMP_LANG_OBJC;
    } else {
        EXIT_ACTION_STDOUT(__func__)
    }
}

RetCode str_from_lang(VLG_COMP_LANG lang, std::string &out)
{
    switch(lang) {
        case VLG_COMP_LANG_C:
            out.assign("C");
            break;
        case VLG_COMP_LANG_CPP:
            out.assign("C++");
            break;
        case VLG_COMP_LANG_JAVA:
            out.assign("Java");
            break;
        case VLG_COMP_LANG_OBJC:
            out.assign("Objective C");
            break;
        default:
            out.assign("");
            break;
    }
    return vlg::RetCode_OK;
}

VLG_COMP_TCOMP tcomp_from_str(const char *str)
{
    if(!strcmp(str, VLG_COMP_TCOMP_TK_MSVC)) {
        return VLG_COMP_TCOMP_MSVC;
    } else if(!strcmp(str, VLG_COMP_TCOMP_TK_GCC)) {
        return VLG_COMP_TCOMP_GCC;
    } else {
        EXIT_ACTION_STDOUT(__func__)
    }
}

RetCode str_from_tcomp(VLG_COMP_TCOMP tcomp, std::string &out)
{
    switch(tcomp) {
        case VLG_COMP_TCOMP_MSVC:
            out.assign("msvc");
            break;
        case VLG_COMP_TCOMP_GCC:
            out.assign("gcc");
            break;
        default:
            out.assign("");
            break;
    }
    return vlg::RetCode_OK;
}

size_t get_next_valid_offset(size_t &cur_offset,
                             size_t type_align,
                             size_t type_size,
                             size_t nmemb,
                             size_t max_align,
                             size_t packing,
                             bool   scalar)
{
    size_t align = scalar ? min(type_align, packing) : max_align;
    size_t rem = 0;
    if((rem = (cur_offset % align))) {
        cur_offset += (align - rem);
    }
    size_t fldsize = (type_size * nmemb);
    return cur_offset + fldsize;
}

size_t adjust_entity_size(size_t cur_offset,
                          size_t max_align,
                          VLG_COMP_ARCH arch,
                          VLG_COMP_OS os,
                          VLG_COMP_LANG lang,
                          VLG_COMP_TCOMP tcomp)
{
    size_t rem = cur_offset % max_align;
    return rem ? (cur_offset + (max_align - rem)) : cur_offset;
}

/***********************************
OPEN- VLG_COMP_OpenInputFile
***********************************/
RetCode open_input_file(const char *fname, FILE **fdesc)
{
    std::string ffname;
    for(auto it = vlg::comp_cfg.path_list.begin(); it != vlg::comp_cfg.path_list.end(); it++) {
        ffname = it->c_str();
        if(ffname[(ffname.length() - 1)] != CR_FS_SEP_C) {
            ffname.append(CR_FS_SEP);
        }
        ffname.append(fname);
        if((*fdesc = fopen(ffname.c_str(), "r"))) {
            break;
        }
    }
    if(!(*fdesc)) {
        fprintf(stderr, "ERROR opening input file %s\n", ffname.c_str());
        EXIT_ACTION_STDOUT("")
    }
    return vlg::RetCode_OK;
}

/***********************************
OPEN- VLG_COMP_OpenOutputFile
***********************************/
RetCode open_output_file(const char *fname, FILE **fdesc)
{
    std::string ffname;
    ffname.assign(comp_cfg.out_dir);
    if(ffname[ffname.length()-1] != CR_FS_SEP_C) {
        ffname.append(CR_FS_SEP);
    }
    ffname.append(fname);
    if(!(*fdesc = fopen(ffname.c_str(),"w+"))) {
        fprintf(stderr, "ERROR opening output file %s\n", ffname.c_str());
        EXIT_ACTION_STDOUT("")
    }
    return vlg::RetCode_OK;
}

/***********************************
PUT- VLG_COMP_Put_NewLine
***********************************/
RetCode put_newline(FILE *file)
{
    fprintf(file,  "\n");
    return vlg::RetCode_OK;
}

/***********************************
GET- VLG_COMP_Get_LocalDate
***********************************/
RetCode get_local_date(char *out)
{
#if defined WIN32 && defined _MSC_VER
    SYSTEMTIME time;
    GetLocalTime(&time);
    sprintf(out, "%02d-%02d-%d", time.wDay, time.wMonth, time.wYear);
#else
    time_t t = time(nullptr);
    struct tm tm = *localtime(&t);
    sprintf(out, "%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
#endif
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_Hdr
***********************************/
RetCode render_hdr(compile_unit &cunit, std::string &fname,
                   FILE *file)
{
    fprintf(file,  "/********************************************************");
    RET_ON_KO(put_newline(file))
    fprintf(file, "%-20s%s", "Model:", cunit.model_name());
    RET_ON_KO(put_newline(file))
    fprintf(file, "%-20s%s", "Model ver:", cunit.model_version());
    RET_ON_KO(put_newline(file))
    fprintf(file, "%-20s%s", "Source:", fname.c_str());
    RET_ON_KO(put_newline(file))
    char cur_date[11];
    get_local_date(cur_date);
    fprintf(file, "%-20s%s", "Compiled on:", cur_date);
    RET_ON_KO(put_newline(file))
    std::string tmp;
    RET_ON_KO(str_from_lang(comp_cfg.lang, tmp))
    fprintf(file, "%-20s%s", "Lang:", tmp.c_str());
    RET_ON_KO(put_newline(file))
    fprintf(file,  "********************************************************/");
    RET_ON_KO(put_newline(file))
    return vlg::RetCode_OK;
}

compiler_config::compiler_config() :
    verblvl(0),
    lang(VLG_COMP_LANG_Undef),
    out_dir(VLG_COMP_DFLT_DIR) {}

compile_unit::compile_unit() :
    fname_(nullptr),
    model_name_(nullptr),
    model_version_(nullptr)
{}

compile_unit::~compile_unit()
{
    if(fname_) {
        free(fname_);
    }
}

RetCode compile_unit::init(const char *fname)
{
    if(!fname) {
        return vlg::RetCode_KO;
    }
    CMD_ON_NUL(fname_ = strdup(fname), exit(1))
    return vlg::RetCode_OK;
}

RetCode compile_unit::parse()
{
    FILE *fdesc = nullptr;
    std::string data; //file content loaded on data
    RET_ON_KO(open_input_file(fname_, &fdesc))
    RET_ON_KO(load_file(fdesc, data))
    RET_ON_KO(parse_data(fname_,
                         data,
                         define_map_,
                         entity_map_,
                         &model_name_,
                         &model_version_))
    return vlg::RetCode_OK;
}

RetCode compile_unit::compile()
{
    switch(comp_cfg.lang) {
        case VLG_COMP_LANG_C:
            return vlg::RetCode_UNSP;
        case VLG_COMP_LANG_CPP:
            RET_ON_KO(compile_CPP(*this)) break;
        case VLG_COMP_LANG_JAVA:
            RET_ON_KO(compile_Java(*this)) break;
        default:
            return vlg::RetCode_UNSP;
    }
    return vlg::RetCode_OK;
}

const char *compile_unit::model_name() const
{
    return model_name_;
}

const char *compile_unit::model_version() const
{
    return model_version_;
}

const char *compile_unit::get_file_name()
{
    return fname_;
}

std::map<std::string, std::string> &compile_unit::get_define_map()
{
    return define_map_;
}

std::map<std::string, entity_desc_comp *> &compile_unit::get_entity_map()
{
    return entity_map_;
}

}