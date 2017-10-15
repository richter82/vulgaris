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

#include "blz_compiler.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//--C++
#define BLZ_COMP_CPP_TYPE_UNSIGN    "unsigned"
#define BLZ_COMP_CPP_TYPE_BOOL      "bool"
#define BLZ_COMP_CPP_TYPE_INT       "int"
#define BLZ_COMP_CPP_TYPE_SHORT     "short"
#define BLZ_COMP_CPP_TYPE_LONG      "long"
#define BLZ_COMP_CPP_TYPE_INT64     "int64_t"
#define BLZ_COMP_CPP_TYPE_UINT64    "uint64_t"
#define BLZ_COMP_CPP_TYPE_FLOAT     "float"
#define BLZ_COMP_CPP_TYPE_DOUBLE    "double"
#define BLZ_COMP_CPP_TYPE_CHAR      "char"
#define BLZ_COMP_CPP_TYPE_WCHAR     "wchar_t"

//--Java
#define BLZ_COMP_JAVA_TYPE_BYTE      "Byte"
#define BLZ_COMP_JAVA_TYPE_BOOL      "Boolean"
#define BLZ_COMP_JAVA_TYPE_INT       "Integer"
#define BLZ_COMP_JAVA_TYPE_SHORT     "Short"
#define BLZ_COMP_JAVA_TYPE_LONG      "Long"
#define BLZ_COMP_JAVA_TYPE_FLOAT     "Float"
#define BLZ_COMP_JAVA_TYPE_DOUBLE    "Double"
#define BLZ_COMP_JAVA_TYPE_CHAR      "Character"
#define BLZ_COMP_JAVA_TYPE_STRING    "String"

#define NOTFNDINHASH "key not found in hash"

namespace blaze {

//-----------------------------
// BLZ_COMP_ARCH: x86_64
// BLZ_COMP_OS: unix
// BLZ_COMP_LANG: CPP
// BLZ_COMP_TCOMP: GCC
//-----------------------------
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
        default:
            return 0;
    }
    return 0;
}

//-----------------------------
// BLZ_COMP_ARCH: x86_64
// BLZ_COMP_OS: win
// BLZ_COMP_LANG: CPP
// BLZ_COMP_TCOMP: MSVC
//-----------------------------
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
        default:
            return 0;
    }
    return 0;
}

static size_t  get_in_arch_type_size_x86_64_unix_CPP(Type type,
                                                     BLZ_COMP_TCOMP tcomp)
{
    switch(tcomp) {
        case BLZ_COMP_TCOMP_GCC:
            return get_in_arch_type_size_x86_64_unix_CPP_GCC(type);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

static size_t  get_in_arch_type_size_x86_64_win_CPP(Type type,
                                                    BLZ_COMP_TCOMP tcomp)
{
    switch(tcomp) {
        case BLZ_COMP_TCOMP_MSVC:
            return get_in_arch_type_size_x86_64_win_CPP_MSVC(type);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

static size_t  GetInArchTypeSize_x86_64_unix(Type type,
                                             BLZ_COMP_LANG lang,
                                             BLZ_COMP_TCOMP tcomp)
{
    switch(lang) {
        case BLZ_COMP_LANG_CPP:
            return get_in_arch_type_size_x86_64_unix_CPP(type, tcomp);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

static size_t  GetInArchTypeSize_x86_64_win(Type type,
                                            BLZ_COMP_LANG lang,
                                            BLZ_COMP_TCOMP tcomp)
{
    switch(lang) {
        case BLZ_COMP_LANG_CPP:
            return get_in_arch_type_size_x86_64_win_CPP(type, tcomp);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

static size_t  GetInArchTypeSize_x86_64(Type type,
                                        BLZ_COMP_OS os,
                                        BLZ_COMP_LANG lang,
                                        BLZ_COMP_TCOMP tcomp)
{
    switch(os) {
        case BLZ_COMP_OS_win:
            return GetInArchTypeSize_x86_64_win(type, lang, tcomp);
        case BLZ_COMP_OS_unix:
            return GetInArchTypeSize_x86_64_unix(type, lang, tcomp);
        default:
            EXIT_ACTION_STDOUT(__func__)
    }
    return 0;
}

size_t  get_in_arch_type_size(Type type,
                              BLZ_COMP_ARCH arch,
                              BLZ_COMP_OS os,
                              BLZ_COMP_LANG lang,
                              BLZ_COMP_TCOMP tcomp)
{
    switch(arch) {
        case BLZ_COMP_ARCH_x86_64:
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
        default:
            return 0;
    }
}

/***********************************
BLZ_MEMBER_DESC_COMP
***********************************/
member_desc_comp::member_desc_comp(unsigned short mmbrid,
                                   MemberType mmbr_type,
                                   const char *mmbr_name,
                                   const char *mmbr_desc,
                                   Type fild_type,
                                   size_t nmemb,
                                   unsigned int fild_entityid,
                                   const char *fild_usr_str_type,
                                   EntityType fild_entitytype,
                                   long enum_value) :
    mmbrid_(mmbrid),
    mmbr_type_(mmbr_type),
    mmbr_name_(mmbr_name),
    mmbr_desc_(mmbr_desc),
    fild_type_(fild_type),
    fild_offset_map_(sizeof(size_t), sizeof(unsigned int)),
    fild_type_size_map_(sizeof(size_t), sizeof(unsigned int)),
    nmemb_(nmemb),
    fild_nclassid_(fild_entityid),
    fild_usr_str_type_(fild_usr_str_type),
    fild_entitytype_(fild_entitytype),
    enum_value_(enum_value)
{}

blaze::RetCode member_desc_comp::init()
{
    RETURN_IF_NOT_OK(fild_offset_map_.init(HM_SIZE_NANO))
    RETURN_IF_NOT_OK(fild_type_size_map_.init(HM_SIZE_NANO))
    return blaze::RetCode_OK;
}

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

size_t member_desc_comp::get_field_offset(BLZ_COMP_ARCH arch,
                                          BLZ_COMP_OS os,
                                          BLZ_COMP_LANG lang,
                                          BLZ_COMP_TCOMP tcomp) const
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    char outmsg[256] = {0};
    sprintf(outmsg, "key not found(arch:%d, os:%d, lang:%d, tcomp:%d)", arch, os,
            lang, tcomp);
    size_t val = 0;
    COMMAND_IF_NOT_OK(fild_offset_map_.get(&key, &val), EXIT_ACTION(outmsg))
    return val;
}

size_t member_desc_comp::get_field_type_size(BLZ_COMP_ARCH arch,
                                             BLZ_COMP_OS os,
                                             BLZ_COMP_LANG lang,
                                             BLZ_COMP_TCOMP tcomp) const
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    char outmsg[256] = {0};
    sprintf(outmsg, "key not found(arch:%d, os:%d, lang:%d, tcomp:%d)", arch, os,
            lang, tcomp);
    size_t val = 0;
    COMMAND_IF_NOT_OK(fild_type_size_map_.get(&key, &val), EXIT_ACTION(outmsg))
    return val;
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

EntityType member_desc_comp::get_field_entity_type() const
{
    return fild_entitytype_;
}

long member_desc_comp::get_enum_value() const
{
    return enum_value_;
}

void member_desc_comp::set_field_offset(size_t val,
                                        BLZ_COMP_ARCH arch,
                                        BLZ_COMP_OS os,
                                        BLZ_COMP_LANG lang,
                                        BLZ_COMP_TCOMP tcomp)
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    COMMAND_IF_NOT_OK(fild_offset_map_.put(&key, &val), exit(1))
}

void member_desc_comp::set_field_type_size(size_t val,
                                           BLZ_COMP_ARCH arch,
                                           BLZ_COMP_OS os,
                                           BLZ_COMP_LANG lang,
                                           BLZ_COMP_TCOMP tcomp)
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    COMMAND_IF_NOT_OK(fild_type_size_map_.put(&key, &val), exit(1))
}


/***********************************
BLZ_ENTITY_DESC_COMP
***********************************/
entity_desc_comp::entity_desc_comp(unsigned int entityid,
                                   EntityType entitytype,
                                   const char *nmspace,
                                   const char *entityname,
                                   blaze::alloc_func afun,
                                   unsigned int fild_num,
                                   bool persistent) :
    entityid_(entityid),
    entity_size_map_(sizeof(size_t), sizeof(unsigned int)),
    entity_max_align_map_(sizeof(size_t), sizeof(unsigned int)),
    entitytype_(entitytype),
    nmspace_(nmspace),
    entityname_(entityname),
    afun_(afun),
    fild_num_(fild_num),
    mmbrid_mdesc_(blaze::sngl_ptr_obj_mng(), sizeof(unsigned short)),
    mmbrnm_mdesc_(blaze::sngl_ptr_obj_mng(), blaze::sngl_cstr_obj_mng()),
    persistent_(persistent),
    keyid_kdesc_(blaze::sngl_ptr_obj_mng(), sizeof(unsigned short))
{}

blaze::RetCode entity_desc_comp::init()
{
    RETURN_IF_NOT_OK(entity_size_map_.init(HM_SIZE_NANO))
    RETURN_IF_NOT_OK(entity_max_align_map_.init(HM_SIZE_NANO))
    RETURN_IF_NOT_OK(mmbrid_mdesc_.init(HM_SIZE_MINI))
    RETURN_IF_NOT_OK(mmbrnm_mdesc_.init(HM_SIZE_MINI))
    if(persistent_) {
        RETURN_IF_NOT_OK(keyid_kdesc_.init(HM_SIZE_NANO))
    }
    return blaze::RetCode_OK;
}

blaze::RetCode entity_desc_comp::extend(blaze::hash_map *mmbrmap,
                                        blaze::hash_map *keymap)
{
    member_desc_comp *mdesc = NULL;
    mmbrmap->start_iteration();
    while(!mmbrmap->next(NULL, &mdesc)) {
        unsigned long mmbrid = mdesc->get_member_id();
        RETURN_IF_NOT_OK(mmbrid_mdesc_.put(&mmbrid, &mdesc))
        const char *mmbrnm = mdesc->get_member_name();
        RETURN_IF_NOT_OK(mmbrnm_mdesc_.put(mmbrnm, &mdesc))
    }
    if(persistent_) {
        key_desc_comp *keydesc = NULL;
        unsigned short keyid = 0;
        keymap->start_iteration();
        while(!keymap->next(&keyid, &keydesc)) {
            RETURN_IF_NOT_OK(keyid_kdesc_.put(&keyid, &keydesc))
        }
    }
    return blaze::RetCode_OK;
}

blaze::RetCode entity_desc_comp::add_key_desc(const key_desc_comp *keydesc)
{
    unsigned short keyid = keydesc->get_key_id();
    RETURN_IF_NOT_OK(keyid_kdesc_.put(&keyid, &keydesc))
    return blaze::RetCode_OK;
}

unsigned int entity_desc_comp::get_entityid()  const
{
    return entityid_;
}

size_t entity_desc_comp::get_entity_size(BLZ_COMP_ARCH arch,
                                         BLZ_COMP_OS os,
                                         BLZ_COMP_LANG lang,
                                         BLZ_COMP_TCOMP tcomp) const
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    char outmsg[256] = {0};
    sprintf(outmsg, "key not found(arch:%d, os:%d, lang:%d, tcomp:%d)", arch, os,
            lang, tcomp);
    size_t val = 0;
    COMMAND_IF_NOT_OK(entity_size_map_.get(&key, &val), EXIT_ACTION(outmsg))
    return val;
}

size_t entity_desc_comp::get_entity_max_align(BLZ_COMP_ARCH arch,
                                              BLZ_COMP_OS os,
                                              BLZ_COMP_LANG lang,
                                              BLZ_COMP_TCOMP tcomp) const
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    char outmsg[256] = {0};
    sprintf(outmsg, "key not found(arch:%d, os:%d, lang:%d, tcomp:%d)", arch, os,
            lang, tcomp);
    size_t val = 0;
    COMMAND_IF_NOT_OK(entity_max_align_map_.get(&key, &val), EXIT_ACTION(outmsg))
    return val;
}

EntityType entity_desc_comp::get_entity_type()  const
{
    return entitytype_;
}

const char *entity_desc_comp::get_entity_namespace() const
{
    return nmspace_;
}

const char *entity_desc_comp::get_entity_name()  const
{
    return entityname_;
}

blaze::alloc_func entity_desc_comp::get_entity_alloc_f() const
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

blaze::hash_map &entity_desc_comp::get_map_id_MMBRDSC()
{
    return mmbrid_mdesc_;
}

const blaze::hash_map &entity_desc_comp::get_map_name_MMBRDSC() const
{
    return mmbrnm_mdesc_;
}

const blaze::hash_map &entity_desc_comp::get_map_keyid_KDESC() const
{
    return keyid_kdesc_;
}

blaze::hash_map &entity_desc_comp::get_map_keyid_KDESC_mod()
{
    return keyid_kdesc_;
}

const member_desc_comp  *entity_desc_comp::get_member_desc_by_id(
    unsigned int mmbrid) const
{
    const void *ptr = mmbrid_mdesc_.get(&mmbrid);
    return ptr ? *(const member_desc_comp **)ptr : NULL;
}

const member_desc_comp  *entity_desc_comp::get_member_desc_by_name(
    const char *name) const
{
    const void *ptr = mmbrnm_mdesc_.get(name);
    return ptr ? *(const member_desc_comp **)ptr : NULL;
}

void entity_desc_comp::enum_member_desc(enum_member_desc_comp_func func) const
{
    return ;
}

void entity_desc_comp::set_entity_size(size_t val,
                                       BLZ_COMP_ARCH arch,
                                       BLZ_COMP_OS os,
                                       BLZ_COMP_LANG lang,
                                       BLZ_COMP_TCOMP tcomp)
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    COMMAND_IF_NOT_OK(entity_size_map_.put(&key, &val), exit(1))
}

void entity_desc_comp::set_entity_max_align(size_t val,
                                            BLZ_COMP_ARCH arch,
                                            BLZ_COMP_OS os,
                                            BLZ_COMP_LANG lang,
                                            BLZ_COMP_TCOMP tcomp)
{
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    COMMAND_IF_NOT_OK(entity_max_align_map_.put(&key, &val), exit(1))
}

/***********************************
key_desc_comp
***********************************/
key_desc_comp::key_desc_comp(unsigned short keyid, bool primary) :
    keyid_(keyid),
    primary_(primary),
    fildset_(blaze::sngl_ptr_obj_mng())
{
}

blaze::RetCode key_desc_comp::init()
{
    RETURN_IF_NOT_OK(fildset_.init())
    return blaze::RetCode_OK;
}

blaze::RetCode key_desc_comp::init(blaze::linked_list *fldset)
{
    RETURN_IF_NOT_OK(fildset_.init())
    member_desc_comp *mmbrdesc = NULL;
    fldset->start_iteration();
    while(!fldset->next(&mmbrdesc)) {
        RETURN_IF_NOT_OK(fildset_.push_back(&mmbrdesc))
    }
    return blaze::RetCode_OK;
}

blaze::RetCode key_desc_comp::add_member_desc(const member_desc_comp  *mmbrdesc)
{
    RETURN_IF_NOT_OK(fildset_.push_back(&mmbrdesc))
    return blaze::RetCode_OK;
}

unsigned short key_desc_comp::get_key_id() const
{
    return keyid_;
}

bool key_desc_comp::is_primary() const
{
    return primary_;
}

const blaze::linked_list &key_desc_comp::get_key_member_set() const
{
    return fildset_;
}

blaze::linked_list &key_desc_comp::get_key_member_set_m()
{
    return fildset_;
}


blaze::RetCode get_zero_val_for_BLZ_TYPE(Type type,
                                         blaze::ascii_string &out)
{
    switch(comp_cfg.lang) {
        case BLZ_COMP_LANG_C:
            return blaze::RetCode_UNSP;
            break;
        case BLZ_COMP_LANG_CPP:
            switch(type) {
                case Type_BOOL:
                    RETURN_IF_NOT_OK(out.assign("false"))
                    return blaze::RetCode_OK;
                case Type_INT16:
                    RETURN_IF_NOT_OK(out.assign("0"))
                    return blaze::RetCode_OK;
                case Type_UINT16:
                    RETURN_IF_NOT_OK(out.assign("0"))
                    return blaze::RetCode_OK;
                case Type_INT32:
                    RETURN_IF_NOT_OK(out.assign("0"))
                    return blaze::RetCode_OK;
                case Type_UINT32:
                    RETURN_IF_NOT_OK(out.assign("0"))
                    return blaze::RetCode_OK;
                case Type_INT64:
                    RETURN_IF_NOT_OK(out.assign("0"))
                    return blaze::RetCode_OK;
                case Type_UINT64:
                    RETURN_IF_NOT_OK(out.assign("0"))
                    return blaze::RetCode_OK;
                case Type_FLOAT32:
                    RETURN_IF_NOT_OK(out.assign("0"))
                    return blaze::RetCode_OK;
                case Type_FLOAT64:
                    RETURN_IF_NOT_OK(out.assign("0.0"))
                    return blaze::RetCode_OK;
                case Type_ASCII:
                    RETURN_IF_NOT_OK(out.assign("\'\\0\'"))
                    return blaze::RetCode_OK;
                default:
                    return blaze::RetCode_KO;
            }
            break;
        case BLZ_COMP_LANG_JAVA:
            switch(type) {
                case Type_BOOL:
                    RETURN_IF_NOT_OK(out.assign("false"))
                    return blaze::RetCode_OK;
                case Type_INT16:
                case Type_UINT16:
                    RETURN_IF_NOT_OK(out.assign("(short)0"))
                    return blaze::RetCode_OK;
                case Type_INT32:
                case Type_UINT32:
                    RETURN_IF_NOT_OK(out.assign("0"))
                    return blaze::RetCode_OK;
                case Type_INT64:
                case Type_UINT64:
                    RETURN_IF_NOT_OK(out.assign("0L"))
                    return blaze::RetCode_OK;
                case Type_FLOAT32:
                    RETURN_IF_NOT_OK(out.assign("0f"))
                    return blaze::RetCode_OK;
                case Type_FLOAT64:
                    RETURN_IF_NOT_OK(out.assign("0.0"))
                    return blaze::RetCode_OK;
                case Type_ASCII:
                    RETURN_IF_NOT_OK(out.assign("(char)0x0000"))
                    return blaze::RetCode_OK;
                default:
                    return blaze::RetCode_KO;
            }
            break;
        default:
            return blaze::RetCode_KO;
    }
}

blaze::RetCode target_type_from_builtin_BLZ_TYPE(member_desc_comp &mdsc,
                                                 blaze::ascii_string &out)
{
    switch(comp_cfg.lang) {
        case BLZ_COMP_LANG_C:
            return blaze::RetCode_UNSP;
            break;
        case BLZ_COMP_LANG_CPP:
            switch(mdsc.get_field_type()) {
                case Type_BOOL:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_BOOL))
                    return blaze::RetCode_OK;
                case Type_INT16:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_SHORT))
                    return blaze::RetCode_OK;
                case Type_UINT16:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_UNSIGN
                                                CR_TK_SP
                                                BLZ_COMP_CPP_TYPE_SHORT))
                    return blaze::RetCode_OK;
                case Type_INT32:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_INT))
                    return blaze::RetCode_OK;
                case Type_UINT32:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_UNSIGN
                                                CR_TK_SP
                                                BLZ_COMP_CPP_TYPE_INT))
                    return blaze::RetCode_OK;
                case Type_INT64:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_INT64))
                    return blaze::RetCode_OK;
                case Type_UINT64:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_UINT64))
                    return blaze::RetCode_OK;
                case Type_FLOAT32:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_FLOAT))
                    return blaze::RetCode_OK;
                case Type_FLOAT64:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_DOUBLE))
                    return blaze::RetCode_OK;
                case Type_ASCII:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_CPP_TYPE_CHAR))
                    return blaze::RetCode_OK;
                default:
                    return blaze::RetCode_KO;
            }
            break;
        case BLZ_COMP_LANG_JAVA:
            switch(mdsc.get_field_type()) {
                case Type_BOOL:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_JAVA_TYPE_BOOL))
                    return blaze::RetCode_OK;
                case Type_INT16:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_JAVA_TYPE_SHORT))
                    return blaze::RetCode_OK;
                case Type_UINT16:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_JAVA_TYPE_SHORT))
                    return blaze::RetCode_OK;
                case Type_INT32:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_JAVA_TYPE_INT))
                    return blaze::RetCode_OK;
                case Type_UINT32:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_JAVA_TYPE_INT))
                    return blaze::RetCode_OK;
                case Type_INT64:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_JAVA_TYPE_LONG))
                    return blaze::RetCode_OK;
                case Type_UINT64:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_JAVA_TYPE_LONG))
                    return blaze::RetCode_OK;
                case Type_FLOAT32:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_JAVA_TYPE_FLOAT))
                    return blaze::RetCode_OK;
                case Type_FLOAT64:
                    RETURN_IF_NOT_OK(out.assign(BLZ_COMP_JAVA_TYPE_DOUBLE))
                    return blaze::RetCode_OK;
                case Type_ASCII:
                    RETURN_IF_NOT_OK(out.assign((mdsc.get_nmemb() > 1) ?
                                                BLZ_COMP_JAVA_TYPE_STRING :
                                                BLZ_COMP_JAVA_TYPE_CHAR))
                    return blaze::RetCode_OK;
                default:
                    return blaze::RetCode_KO;
            }
            break;
        default:
            return blaze::RetCode_KO;
    }
}

blaze::RetCode target_type_from_BLZ_TYPE(member_desc_comp &mdsc,
                                         blaze::hash_map &entitymap,
                                         blaze::ascii_string &out)
{
    switch(mdsc.get_field_type()) {
        case Type_UNDEFINED:
            return blaze::RetCode_KO;
        case Type_ENTITY:
            RETURN_IF_NOT_OK(out.assign(mdsc.get_field_usr_str_type()))
            return blaze::RetCode_OK;
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
            RETURN_IF_NOT_OK(target_type_from_builtin_BLZ_TYPE(mdsc, out))
            return blaze::RetCode_OK;
        default:
            return blaze::RetCode_KO;
    }
}

blaze::RetCode printf_percent_from_BLZ_TYPE(member_desc_comp &mdsc,
                                            blaze::ascii_string &out,
                                            bool strict_linux)
{
    switch(mdsc.get_field_type()) {
        case Type_BOOL:
            RETURN_IF_NOT_OK(out.assign("d")) return blaze::RetCode_OK;
        case Type_INT16:
            RETURN_IF_NOT_OK(out.assign("d")) return blaze::RetCode_OK;
        case Type_UINT16:
            RETURN_IF_NOT_OK(out.assign("u")) return blaze::RetCode_OK;
        case Type_INT32:
            RETURN_IF_NOT_OK(out.assign("d")) return blaze::RetCode_OK;
        case Type_UINT32:
            RETURN_IF_NOT_OK(out.assign("u")) return blaze::RetCode_OK;
        case Type_INT64:
            if(strict_linux) {
                RETURN_IF_NOT_OK(out.assign("ld")) return blaze::RetCode_OK;
            } else {
                RETURN_IF_NOT_OK(out.assign("lld")) return blaze::RetCode_OK;
            }
        case Type_UINT64:
            if(strict_linux) {
                RETURN_IF_NOT_OK(out.assign("lu")) return blaze::RetCode_OK;
            } else {
                RETURN_IF_NOT_OK(out.assign("llu")) return blaze::RetCode_OK;
            }
        case Type_FLOAT32:
            RETURN_IF_NOT_OK(out.assign("f")) return blaze::RetCode_OK;
        case Type_FLOAT64:
            RETURN_IF_NOT_OK(out.assign("f")) return blaze::RetCode_OK;
        case Type_ASCII:
            RETURN_IF_NOT_OK(out.assign("c")) return blaze::RetCode_OK;
        default:
            return blaze::RetCode_KO;
    }
}

BLZ_COMP_ARCH arch_from_str(const char *str)
{
    if(!strcmp(str, BLZ_COMP_ARCH_TK_X86_64)) {
        return BLZ_COMP_ARCH_x86_64;
    } else {
        EXIT_ACTION_STDOUT(__func__)
    }
}

blaze::RetCode str_from_arch(BLZ_COMP_ARCH arch, blaze::ascii_string &out)
{
    switch(arch) {
        case BLZ_COMP_ARCH_x86_64:
            out.assign("x86_64");
            break;
        default:
            out.assign("");
            break;
    }
    return blaze::RetCode_OK;
}

BLZ_COMP_LANG lang_from_str(const char *str)
{
    if(!strcmp(str, BLZ_COMP_LANG_TK_C)) {
        return BLZ_COMP_LANG_C;
    } else if(!strcmp(str, BLZ_COMP_LANG_TK_CPP)) {
        return BLZ_COMP_LANG_CPP;
    } else if(!strcmp(str, BLZ_COMP_LANG_TK_JAVA)) {
        return BLZ_COMP_LANG_JAVA;
    } else if(!strcmp(str, BLZ_COMP_LANG_TK_OBJC)) {
        return BLZ_COMP_LANG_OBJC;
    } else {
        EXIT_ACTION_STDOUT(__func__)
    }
}

blaze::RetCode str_from_lang(BLZ_COMP_LANG lang, blaze::ascii_string &out)
{
    switch(lang) {
        case BLZ_COMP_LANG_C:
            out.assign("C");
            break;
        case BLZ_COMP_LANG_CPP:
            out.assign("C++");
            break;
        case BLZ_COMP_LANG_JAVA:
            out.assign("Java");
            break;
        case BLZ_COMP_LANG_OBJC:
            out.assign("Objective C");
            break;
        default:
            out.assign("");
            break;
    }
    return blaze::RetCode_OK;
}

BLZ_COMP_TCOMP tcomp_from_str(const char *str)
{
    if(!strcmp(str, BLZ_COMP_TCOMP_TK_MSVC)) {
        return BLZ_COMP_TCOMP_MSVC;
    } else if(!strcmp(str, BLZ_COMP_TCOMP_TK_GCC)) {
        return BLZ_COMP_TCOMP_GCC;
    } else {
        EXIT_ACTION_STDOUT(__func__)
    }
}

blaze::RetCode str_from_tcomp(BLZ_COMP_TCOMP tcomp, blaze::ascii_string &out)
{
    switch(tcomp) {
        case BLZ_COMP_TCOMP_MSVC:
            out.assign("msvc");
            break;
        case BLZ_COMP_TCOMP_GCC:
            out.assign("gcc");
            break;
        default:
            out.assign("");
            break;
    }
    return blaze::RetCode_OK;
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
                          BLZ_COMP_ARCH arch,
                          BLZ_COMP_OS os,
                          BLZ_COMP_LANG lang,
                          BLZ_COMP_TCOMP tcomp)
{
    size_t rem = cur_offset % max_align;
    return rem ? (cur_offset + (max_align - rem)) : cur_offset;
}

/***********************************
OPEN- BLZ_COMP_OpenInputFile
***********************************/
blaze::RetCode open_input_file(const char *fname, FILE **fdesc)
{
    comp_cfg.path_list.start_iteration();
    blaze::ascii_string ffname;
    char path[CR_MAX_SRC_PATH_LEN];
    while(!comp_cfg.path_list.next(path)) {
        COMMAND_IF_NOT_OK(ffname.assign(path), exit(1))
        if(ffname.char_at(ffname.length()-1) != CR_FS_SEP_C) {
            COMMAND_IF_NOT_OK(ffname.append(CR_FS_SEP), exit(1))
        }
        COMMAND_IF_NOT_OK(ffname.append(fname), exit(1))
        if((*fdesc = fopen(ffname.internal_buff(), "r"))) {
            break;
        }
    }
    if(!(*fdesc)) {
        fprintf(stderr, "ERROR opening input file %s\n", ffname.internal_buff());
        EXIT_ACTION_STDOUT("")
    }
    return blaze::RetCode_OK;
}

/***********************************
OPEN- BLZ_COMP_OpenOutputFile
***********************************/
blaze::RetCode open_output_file(const char *fname, FILE **fdesc)
{
    blaze::ascii_string ffname;
    COMMAND_IF_NOT_OK(ffname.assign(comp_cfg.out_dir), exit(1))
    if(ffname.char_at(ffname.length()-1) != CR_FS_SEP_C) {
        COMMAND_IF_NOT_OK(ffname.append(CR_FS_SEP), exit(1))
    }
    COMMAND_IF_NOT_OK(ffname.append(fname), exit(1))
    if(!(*fdesc = fopen(ffname.internal_buff(),"w+"))) {
        fprintf(stderr, "ERROR opening output file %s\n", ffname.internal_buff());
        EXIT_ACTION_STDOUT("")
    }
    return blaze::RetCode_OK;
}

/***********************************
PUT- BLZ_COMP_Put_NewLine
***********************************/
blaze::RetCode put_newline(FILE *file)
{
    fprintf(file,  "\n");
    return blaze::RetCode_OK;
}

/***********************************
GET- BLZ_COMP_Get_LocalDate
***********************************/
blaze::RetCode get_local_date(char *out)
{
#ifdef WIN32
    SYSTEMTIME time;
    GetLocalTime(&time);
    sprintf(out, "%02d-%02d-%d", time.wDay, time.wMonth, time.wYear);
#else
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(out, "%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
#endif
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_Hdr
***********************************/
blaze::RetCode render_hdr(compile_unit &cunit, blaze::ascii_string &fname,
                          FILE *file)
{
    fprintf(file,  "/********************************************************");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file, "%-20s%s", "Model:", cunit.model_name());
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file, "%-20s%s", "Model ver:", cunit.model_version());
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file, "%-20s%s", "Source:", fname.internal_buff());
    RETURN_IF_NOT_OK(put_newline(file))
    char cur_date[11];
    get_local_date(cur_date);
    fprintf(file, "%-20s%s", "Compiled on:", cur_date);
    RETURN_IF_NOT_OK(put_newline(file))
    blaze::ascii_string tmp;
    RETURN_IF_NOT_OK(str_from_lang(comp_cfg.lang, tmp))
    fprintf(file, "%-20s%s", "Lang:", tmp.internal_buff());
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file,  "********************************************************/");
    RETURN_IF_NOT_OK(put_newline(file))
    return blaze::RetCode_OK;
}

compiler_config::compiler_config() :    verblvl(0),
    lang(BLZ_COMP_LANG_Undef),
    path_list(blaze::sngl_cstr_obj_mng()),
    file_list(blaze::sngl_cstr_obj_mng()),
    out_dir(BLZ_COMP_DFLT_DIR) {}

blaze::RetCode compiler_config::init()
{
    RETURN_IF_NOT_OK(path_list.init())
    RETURN_IF_NOT_OK(file_list.init())
    return blaze::RetCode_OK;
}


compile_unit::compile_unit() :
    fname_(NULL),
    model_name_(NULL),
    model_version_(NULL),
    define_map_(blaze::sngl_cstr_obj_mng(),
                blaze::sngl_cstr_obj_mng()),
    entity_map_(blaze::sngl_ptr_obj_mng(),
                blaze::sngl_cstr_obj_mng())
{}

compile_unit::~compile_unit()
{
    if(fname_) {
        free(fname_);
    }
}

blaze::RetCode compile_unit::init(const char *fname)
{
    if(!fname) {
        return blaze::RetCode_KO;
    }
#ifdef WIN32
    COMMAND_IF_NULL(fname_ = _strdup(fname), exit(1))
#else
    COMMAND_IF_NULL(fname_ = strdup(fname), exit(1))
#endif
    COMMAND_IF_NOT_OK(define_map_.init(HM_SIZE_MINI), exit(1))
    COMMAND_IF_NOT_OK(entity_map_.init(HM_SIZE_NORM), exit(1))
    return blaze::RetCode_OK;
}

blaze::RetCode compile_unit::parse()
{
    FILE *fdesc = NULL;
    blaze::ascii_string data; //file content loaded on data
    RETURN_IF_NOT_OK(open_input_file(fname_, &fdesc))
    RETURN_IF_NOT_OK(load_file(fdesc, data))
    RETURN_IF_NOT_OK(parse_data(fname_,
                                data,
                                define_map_,
                                entity_map_,
                                &model_name_,
                                &model_version_))
    return blaze::RetCode_OK;
}

blaze::RetCode compile_unit::compile()
{
    switch(comp_cfg.lang) {
        case BLZ_COMP_LANG_C:
            return blaze::RetCode_UNSP;
        case BLZ_COMP_LANG_CPP:
            RETURN_IF_NOT_OK(compile_CPP(*this)) break;
        case BLZ_COMP_LANG_JAVA:
            RETURN_IF_NOT_OK(compile_Java(*this)) break;
        default:
            return blaze::RetCode_UNSP;
    }
    return blaze::RetCode_OK;
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

blaze::hash_map &compile_unit::get_define_map()
{
    return define_map_;
}

blaze::hash_map &compile_unit::get_entity_map()
{
    return entity_map_;
}

}