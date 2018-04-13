/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "compiler.h"

namespace vlg {

/***********************************
glob
***********************************/
std::string unit_nmspace;

/***********************************
HMAP: tcomp_packing_map
***********************************/
static std::map<unsigned int, size_t> tcomp_packing_map;

RetCode LoadTCompPackingMap(std::map<unsigned int, size_t> &map)
{
    size_t packing_value = 8;
    unsigned int key = 0;
    packing_value = 8;
    key = TCOMP_DEP_GEN_KEY(VLG_COMP_ARCH_x86_64,
                            VLG_COMP_OS_win,
                            VLG_COMP_LANG_CPP,
                            VLG_COMP_TCOMP_MSVC);
    map[key] = packing_value;

    packing_value = 8;
    key = TCOMP_DEP_GEN_KEY(VLG_COMP_ARCH_x86_64,
                            VLG_COMP_OS_unix,
                            VLG_COMP_LANG_CPP,
                            VLG_COMP_TCOMP_GCC);
    map[key] = packing_value;

    return vlg::RetCode_OK;
}

std::map<unsigned int, size_t> &GetTCompPackingMap()
{
    if(tcomp_packing_map.empty()) {
        LoadTCompPackingMap(tcomp_packing_map);
    }
    return tcomp_packing_map;
}

/***********************************
HMAP: rword_map
***********************************/

static std::unordered_set<std::string> rword_map;

RetCode LoadResWordsMap(std::unordered_set<std::string> &map)
{
    map.insert(VLG_RWRD_PFX VLG_RWRD_ENUM);
    map.insert(VLG_RWRD_PFX VLG_RWRD_NCLASS);
    map.insert(VLG_RWRD_PFX VLG_RWRD_ID);
    map.insert(VLG_RWRD_PFX VLG_RWRD_PRIMARY);
    map.insert(VLG_RWRD_PFX VLG_RWRD_PERSISTENT);
    map.insert(VLG_RWRD_PFX VLG_RWRD_KEY);
    map.insert(VLG_RWRD_PFX VLG_RWRD_KEY_END);
    map.insert(VLG_RWRD_PFX VLG_RWRD_FILDSET);
    map.insert(VLG_RWRD_PFX VLG_RWRD_NAMESPACE);
    map.insert(VLG_RWRD_PFX VLG_RWRD_NAMESPACE_END);
    map.insert(VLG_RWRD_PFX VLG_RWRD_MODLNAME);
    map.insert(VLG_RWRD_PFX VLG_RWRD_MODLVER);
    return vlg::RetCode_OK;
}

std::unordered_set<std::string> &GetResWordsMap()
{
    if(rword_map.empty()) {
        LoadResWordsMap(rword_map);
    }
    return rword_map;
}

/***********************************
HMAP: types_map
***********************************/

static std::unordered_set<std::string> types_map;

RetCode LoadTypesMap(std::unordered_set<std::string> &map)
{
    map.insert(VLG_RWORD_T_BOOL);
    map.insert(VLG_RWORD_T_INT_16);
    map.insert(VLG_RWORD_T_UINT_16);
    map.insert(VLG_RWORD_T_INT_32);
    map.insert(VLG_RWORD_T_UINT_32);
    map.insert(VLG_RWORD_T_INT_64);
    map.insert(VLG_RWORD_T_UINT_64);
    map.insert(VLG_RWORD_T_FLOAT_32);
    map.insert(VLG_RWORD_T_FLOAT_64);
    map.insert(VLG_RWORD_T_ASCII);
    map.insert(VLG_RWORD_T_BYTE);
    return vlg::RetCode_OK;
}

std::unordered_set<std::string> &GetTypesMap()
{
    if(types_map.empty()) {
        CMD_ON_KO(LoadTypesMap(types_map), EXIT_ACTION)
    }
    return types_map;
}

Type VLG_COMP_StrToTYPE(const std::string &str)
{
    if(str == VLG_RWORD_T_BOOL) {
        return  Type_BOOL;
    }
    if(str == VLG_RWORD_T_INT_16) {
        return  Type_INT16;
    }
    if(str == VLG_RWORD_T_UINT_16) {
        return  Type_UINT16;
    }
    if(str == VLG_RWORD_T_INT_32) {
        return  Type_INT32;
    }
    if(str == VLG_RWORD_T_UINT_32) {
        return  Type_UINT32;
    }
    if(str == VLG_RWORD_T_INT_64) {
        return  Type_INT64;
    }
    if(str == VLG_RWORD_T_UINT_64) {
        return  Type_UINT64;
    }
    if(str == VLG_RWORD_T_FLOAT_32) {
        return  Type_FLOAT32;
    }
    if(str == VLG_RWORD_T_FLOAT_64) {
        return  Type_FLOAT64;
    }
    if(str == VLG_RWORD_T_ASCII) {
        return  Type_ASCII;
    }
    if(str == VLG_RWORD_T_BYTE) {
        return  Type_BYTE;
    }
    return Type_UNDEFINED;
}

#define PARSE_ERR "\nERROR at line: %lu - "

void VLG_COMP_PARSE_FND_EXP(unsigned long line,
                            const char *fnd,
                            const char *exp)
{
    fprintf(stderr, PARSE_ERR"found: %s, expected: %s.\n", line, fnd, exp);
}

void VLG_COMP_PARSE_EXP(unsigned long line,
                        const char *exp)
{
    fprintf(stderr, PARSE_ERR"%s was expected.\n", line, exp);
}

void VLG_COMP_PARSE_UNEXP(unsigned long line,
                          const char *unexp)
{
    fprintf(stderr, PARSE_ERR"unexpected token: %s.\n", line, unexp);
}

void VLG_COMP_PARSE_SYMB_ALRDY_DECL(unsigned long line,
                                    const char *symb)
{
    fprintf(stderr, PARSE_ERR"symbol already declared: %s.\n", line, symb);
}

void VLG_COMP_PARSE_TYPE_UNRECOGN(unsigned long line,
                                  const char *symb)
{
    fprintf(stderr, PARSE_ERR"unrecognized type: %s.\n", line, symb);
}

void VLG_COMP_PARSE_EXP_INVALID(unsigned long line)
{
    fprintf(stderr, PARSE_ERR"invalid expression.\n", line);
}

void VLG_COMP_PARSE_CLASS_ID_NOT_DECL(unsigned long line,
                                      const char *symb)
{
    fprintf(stderr, PARSE_ERR"undeclared @id for class: %s.\n", line, symb);
}

void VLG_COMP_PARSE_CLASS_ID_ALRDY_DECL(unsigned long line,
                                        const char *symb)
{
    fprintf(stderr, PARSE_ERR"@id already declared for class: %s.\n", line, symb);
}

void VLG_COMP_PARSE_CLASS_NOT_PERSISTENT(unsigned long line,
                                         const char *symb)
{
    fprintf(stderr, PARSE_ERR"class not persistent, found: %s.\n", line, symb);
}

void VLG_COMP_PARSE_CLASS_ALRDY_PERSISTENT(unsigned long line,
                                           const char *symb)
{
    fprintf(stderr, PARSE_ERR"class already tag as persistent, found: %s.\n", line,
            symb);
}

void VLG_COMP_PARSE_KEYSET_INVALID(unsigned long line)
{
    fprintf(stderr, PARSE_ERR"invalid class keyset.\n", line);
}

/***********************************
CONSISTENCY- VLG_COMP_CheckSymbol
***********************************/

//@fixme: add separator check (,)
RetCode VLG_COMP_CheckSymbol(unsigned long &lnum,
                             std::string &tkn,
                             std::map<std::string, std::string> &definemap,
                             std::map<std::string, entity_desc_comp *> &entitymap,
                             std::map<std::string, member_desc_comp *> *mmbrmap,
                             const char *FND_CGT,    //found category
                             const char *EXP_CTG     //expected category
                            )
{
    //we expect symb name, so we return with error if newline found.
    if(is_new_line(tkn)) {
        VLG_COMP_PARSE_EXP(lnum, EXP_CTG);
        return vlg::RetCode_KO;
    }
    //check if this is a reserved word
    if(GetResWordsMap().find(tkn) != GetResWordsMap().end()) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.c_str(), EXP_CTG);
        return vlg::RetCode_KO;
    }
    //check if this is a built-in compiler type
    if(GetTypesMap().find(tkn) != GetTypesMap().end()) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.c_str(), EXP_CTG);
        return vlg::RetCode_KO;
    }
    //if we have already seen this symbol (define), we return with error
    if(definemap.find(tkn) != definemap.end()) {
        VLG_COMP_PARSE_SYMB_ALRDY_DECL(lnum, tkn.c_str());
        return vlg::RetCode_KO;
    }
    //if we have already seen this symbol (entity), we return with error
    if(entitymap.find(tkn) != entitymap.end()) {
        VLG_COMP_PARSE_SYMB_ALRDY_DECL(lnum, tkn.c_str());
        return vlg::RetCode_KO;
    }
    //if we have already seen this symbol (member), we return with error
    if(mmbrmap && mmbrmap->find(tkn) != mmbrmap->end()) {
        VLG_COMP_PARSE_SYMB_ALRDY_DECL(lnum, tkn.c_str());
        return vlg::RetCode_KO;
    }
    return vlg::RetCode_OK;
}

/***********************************
CONSISTENCY- VLG_COMP_CheckKeySymbol
***********************************/
//@fixme: add separator check (,)
RetCode VLG_COMP_CheckKeySymbol(unsigned long &lnum,
                                std::string &tkn,
                                std::map<std::string, member_desc_comp *> &mmbrmap,
                                const char *FND_CGT,    //found category
                                const char *EXP_CTG)    //expected category

{
    std::string hint;
    hint.assign(FND_CGT);
    hint.append(CR_TK_SP);
    hint.append(EXP_CTG);
    //we expect symb name, so we return with error if newline found.
    if(is_new_line(tkn)) {
        VLG_COMP_PARSE_EXP(lnum, EXP_CTG);
        return vlg::RetCode_KO;
    }
    //check if this is a reserved word
    if(GetResWordsMap().find(tkn) != GetResWordsMap().end()) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.c_str(), hint.c_str());
        return vlg::RetCode_KO;
    }
    //check if this is a built-in compiler type
    if(GetTypesMap().find(tkn) != GetTypesMap().end()) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.c_str(), hint.c_str());
        return vlg::RetCode_KO;
    }
    //this must be a member field
    if(mmbrmap.find(tkn) == mmbrmap.end()) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.c_str(), hint.c_str());
        return vlg::RetCode_KO;
    }
    return vlg::RetCode_OK;
}

/***********************************
READ- VLG_COMP_ReadOpeningCurlyBrace
***********************************/
RetCode VLG_COMP_ReadOpeningCurlyBrace(unsigned long &lnum,
                                       vlg::str_tok &tknz)
{
    std::string tkn;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_TK_CBL, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_SKIP_NEWLINE(tkn)
        if(tkn != CR_TK_CBL) {
            VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
            return vlg::RetCode_KO;
        }
        break;
    }
    return vlg::RetCode_OK;
}

/***********************************
READ- VLG_COMP_ReadInteger
***********************************/
RetCode VLG_COMP_ReadInteger(unsigned long &lnum,
                             vlg::str_tok &tknz,
                             long &along)
{
    std::string tkn;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA, true)) {
        CR_SKIP_SP_TABS(tkn)
        //we expect an int, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, VLG_COMP_PARSE_EXP(lnum, VLG_COMP_SYMB_NAME); return vlg::RetCode_KO)
        if(vlg::string_is_number(tkn.c_str())) {
            along = atol(tkn.c_str());
        } else {
            VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
            return vlg::RetCode_KO;
        }
        break;
    }
    return vlg::RetCode_OK;
}

/***********************************
READ- VLG_COMP_ReadString
***********************************/
RetCode VLG_COMP_ReadString(unsigned long &lnum,
                            vlg::str_tok &tknz,
                            char **newstr, bool begin_quote_read = false,
                            bool opt = false)
{
    std::string tkn;
    if(!begin_quote_read) {
        while(tknz.next_token(tkn, CR_DF_DLMT CR_TK_QT CR_TK_COMA, true)) {
            CR_SKIP_SP_TABS(tkn)
            //we expect a string, so we return with error if newline found.
            if(opt) {
                CR_DO_CMD_ON_NEWLINE(tkn, lnum++; return vlg::RetCode_OK)
            } else {
                CR_DO_CMD_ON_NEWLINE(tkn, VLG_COMP_PARSE_EXP(lnum, VLG_COMP_STRING);
                                     return vlg::RetCode_KO)
            }
            if(tkn == CR_TK_QT) {
                //ok we have read the beginning quote of the string
                break;
            } else {
                VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
        }
    }
    while(tknz.next_token(tkn, CR_NL_DLMT CR_TK_QT, true)) {
        //we expect a string, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, VLG_COMP_PARSE_EXP(lnum, VLG_COMP_STRING);
                             return vlg::RetCode_KO)
        if(!*newstr) {
            *newstr = strdup(tkn.c_str());
        } else {
            if(tkn == CR_TK_QT) {
                //ok we have read the ending quote of the string
                break;
            } else {
                VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseVal
***********************************/
RetCode VLG_COMP_ParseVal(unsigned long &lnum,
                          unsigned short &mmbrid,
                          std::string &symb_name,
                          long &last_enum_val,
                          vlg::str_tok &tknz,
                          std::map<std::string, std::string> &definemap,
                          std::map<std::string, entity_desc_comp *> &entitymap,
                          std::map<std::string, member_desc_comp *> &mmbrmap)
{
    RET_ON_KO(VLG_COMP_CheckSymbol(lnum,
                                   symb_name,
                                   definemap,
                                   entitymap,
                                   &mmbrmap,
                                   VLG_COMP_RSRV_WORD,
                                   VLG_COMP_SYMB_NAME))
    mmbrid++;

    std::string tkn;
    long enum_val = last_enum_val + 1;
    bool enum_val_read = false;
    char *desc = nullptr;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_TK_EQUAL CR_TK_QT, true)) {
        CR_SKIP_SP_TABS(tkn)
        if(tkn == CR_TK_EQUAL) {
            //ok now we expect an integer value
            if(enum_val_read) {
                //we have already read it..
                VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
            RET_ON_KO(VLG_COMP_ReadInteger(lnum, tknz, enum_val))
            enum_val_read = true;
        } else if(is_new_line(tkn)) {
            lnum++;
            if(enum_val_read) {
                break;
            }
            //legal, we have read enum name and we can assign to it last_value + 1
            break;
        } else if(tkn == CR_TK_QT) {
            //ok we read a description
            RET_ON_KO(VLG_COMP_ReadString(lnum, tknz, &desc, true))
            break;
        }
    }
    last_enum_val = enum_val;
    //at this point we can create a VLG_MEMBER_DESC_COMP for this enum member
    member_desc_comp *mmbrdesc = new member_desc_comp(mmbrid,
                                                      MemberType_NENUM_VALUE,
                                                      strdup(symb_name.c_str()),
                                                      desc,
                                                      Type_INT32,
                                                      1,
                                                      0,
                                                      nullptr,
                                                      NEntityType_NENUM,
                                                      last_enum_val);
    //1
    mmbrdesc->set_field_offset(0,
                               VLG_COMP_ARCH_x86_64,
                               VLG_COMP_OS_win,
                               VLG_COMP_LANG_CPP,
                               VLG_COMP_TCOMP_MSVC);
    mmbrdesc->set_field_type_size(4,
                                  VLG_COMP_ARCH_x86_64,
                                  VLG_COMP_OS_win,
                                  VLG_COMP_LANG_CPP,
                                  VLG_COMP_TCOMP_MSVC);
    //2
    mmbrdesc->set_field_offset(0,
                               VLG_COMP_ARCH_x86_64,
                               VLG_COMP_OS_unix,
                               VLG_COMP_LANG_CPP,
                               VLG_COMP_TCOMP_GCC);
    mmbrdesc->set_field_type_size(4,
                                  VLG_COMP_ARCH_x86_64,
                                  VLG_COMP_OS_unix,
                                  VLG_COMP_LANG_CPP,
                                  VLG_COMP_TCOMP_GCC);
    mmbrmap[symb_name] = mmbrdesc;
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseNMemb
we expect an integer value or an expression (a + b + c ..)
or a combination of (a + DEFINE_0 + b + DEFINE_1...)
***********************************/
RetCode VLG_COMP_ParseNMemb(unsigned long &lnum,
                            vlg::str_tok &tknz,
                            std::map<std::string, std::string> &definemap,
                            size_t &nmemb)
{
    std::string tkn;
    bool exp_valid = false, plus_allwd = false;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_TK_PLUS CR_TK_RBR, true)) {
        CR_SKIP_SP_TABS(tkn)
        auto it = definemap.end();
        if(tkn == CR_TK_PLUS) {
            if(plus_allwd) {
                exp_valid = plus_allwd = false;
            } else {
                VLG_COMP_PARSE_EXP_INVALID(lnum);
                return vlg::RetCode_KO;
            }
        } else if(tkn == CR_TK_RBR) {
            //expression end
            if(exp_valid) {
                //ok we got the final result of expression
                break;
            } else {
                VLG_COMP_PARSE_EXP_INVALID(lnum);
                return vlg::RetCode_KO;
            }
        } else if(vlg::string_is_number(tkn.c_str())) {
            //this is a number
            nmemb += atol(tkn.c_str());
            exp_valid = plus_allwd = true;
        } else if((it = definemap.find(tkn)) != definemap.end()) {
            //this is a @define
            if(vlg::string_is_number(it->second.c_str())) {
                nmemb += atol(it->second.c_str());
                exp_valid = plus_allwd = true;
            } else {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
        } else {
            //unexpected token
            VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
            return vlg::RetCode_KO;
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseType
***********************************/
RetCode VLG_COMP_ParseType(unsigned long &lnum,
                           std::string &str_vlg_type,
                           vlg::str_tok &tknz,
                           std::map<std::string, std::string> &definemap,
                           std::map<std::string, entity_desc_comp *> &entitymap,
                           Type &vlg_type,
                           size_t &nmemb,
                           entity_desc_comp **desc,
                           std::string &symb_name)
{
    auto tmpdesc = entitymap.end();
    if(GetTypesMap().find(str_vlg_type) != GetTypesMap().end()) {
        //build-in type
        vlg_type = VLG_COMP_StrToTYPE(str_vlg_type);
        //ok we have type
    } else if((tmpdesc = entitymap.find(str_vlg_type)) != entitymap.end()) {
        //user defined type
        vlg_type = Type_ENTITY;
        *desc = tmpdesc->second;
        //ok we have type
    } else {
        VLG_COMP_PARSE_TYPE_UNRECOGN(lnum, str_vlg_type.c_str());
        return vlg::RetCode_KO;
    }

    std::string tkn;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA CR_RB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        //we expect to read nmemb or symbol, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, VLG_COMP_PARSE_EXP(lnum, VLG_COMP_SYMB_NAME);
                             return vlg::RetCode_KO)
        if(tkn == CR_TK_RBL) {
            RET_ON_KO(VLG_COMP_ParseNMemb(lnum, tknz, definemap, nmemb))
            break;
        } else {
            //we have read symb here.
            symb_name.assign(tkn);
            break;
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
CALC- VLG_COMP_CalcFieldOffset
***********************************/
RetCode VLG_COMP_CalcFieldOffset(size_t max_align,
                                 size_t &mmbr_offset,
                                 std::map<std::string, entity_desc_comp *> &entitymap,
                                 std::vector<member_desc_comp *> &mmbrmap,
                                 VLG_COMP_ARCH arch,
                                 VLG_COMP_OS os,
                                 VLG_COMP_LANG lang,
                                 VLG_COMP_TCOMP tcomp)
{
    //get cur_offset and compute next
    size_t cur_offst = mmbr_offset;
    size_t type_align = 0;
    size_t packing = 0;
    unsigned int key = TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp);
    packing = GetTCompPackingMap().at(key);

    //enumerate all members
    std::for_each(mmbrmap.begin(), mmbrmap.end(), [&](auto &mdesc) {
        //it is scalar if it is a primitive type or an enum
        bool scalar = (mdesc->get_field_type() != Type_ENTITY) ||
                      (mdesc->get_field_entity_type() == NEntityType_NENUM);
        if(mdesc->get_field_type() == Type_ENTITY &&
                mdesc->get_field_entity_type() != NEntityType_NENUM) {
            //we get max align if it is a struct/class
            auto edesc = entitymap.end();
            if((edesc = entitymap.find(mdesc->get_field_usr_str_type())) == entitymap.end()) {
                EXIT_ACTION
            }
            type_align = edesc->second->get_entity_max_align(arch, os, lang, tcomp);
        } else if(mdesc->get_field_entity_type() == NEntityType_NENUM) {
            //we treat enum with int align.
            type_align = 4;
        } else {
            //we treat primitive types with their own align.
            type_align = get_in_arch_type_size(mdesc->get_field_type(),
                                               arch,
                                               os,
                                               lang,
                                               tcomp);
        }
        mmbr_offset = get_next_valid_offset(cur_offst,
                                            type_align,
                                            mdesc->get_field_type_size(arch,
                                                                       os,
                                                                       lang,
                                                                       tcomp),
                                            mdesc->get_nmemb(),
                                            max_align,
                                            packing,
                                            scalar);
        mdesc->set_field_offset(cur_offst, arch, os, lang, tcomp);
        cur_offst = mmbr_offset;
    });
    return vlg::RetCode_OK;
}

/***********************************
CALC- VLG_COMP_CalcFieldsSizeAndEntityMaxAlign
***********************************/
RetCode VLG_COMP_CalcFieldsSizeAndEntityMaxAlign(size_t &max_align,
                                                 std::map<std::string, entity_desc_comp *> &entitymap,
                                                 std::vector<member_desc_comp *> &mmbrmap,
                                                 VLG_COMP_ARCH arch,
                                                 VLG_COMP_OS os,
                                                 VLG_COMP_LANG lang,
                                                 VLG_COMP_TCOMP tcomp)
{
    size_t type_size = 0;
    //enumerate all members
    std::for_each(mmbrmap.begin(), mmbrmap.end(), [&](auto &mdesc) {
        if(mdesc->get_field_type() == Type_ENTITY) {
            //user-defined type
            auto fld_entity_desc = entitymap.end();
            if((fld_entity_desc = entitymap.find(mdesc->get_field_usr_str_type())) != entitymap.end()) {
                type_size = fld_entity_desc->second->get_size(arch,
                                                              os,
                                                              lang,
                                                              tcomp);
                max_align = (fld_entity_desc->second->get_entity_max_align(arch,
                                                                           os,
                                                                           lang,
                                                                           tcomp) > max_align) ?
                            fld_entity_desc->second->get_entity_max_align(arch,
                                                                          os,
                                                                          lang,
                                                                          tcomp) : max_align;
            } else {
                EXIT_ACTION
            }
        } else {
            //built-in type
            type_size = get_in_arch_type_size(mdesc->get_field_type(),
                                              arch,
                                              os,
                                              lang,
                                              tcomp);
            max_align = (type_size > max_align) ? type_size : max_align;
        }
        mdesc->set_field_type_size(type_size, arch, os, lang, tcomp);
    });
    return vlg::RetCode_OK;
}


/***********************************
CALC- VLG_COMP_CalcTCompDependantValues
***********************************/
RetCode VLG_COMP_CalcTCompDependantValues(size_t max_align,
                                          size_t field_offset,
                                          entity_desc_comp &entitydesc,
                                          std::map<std::string, entity_desc_comp *> &entitymap,
                                          std::vector<member_desc_comp *> &mmbrmap,
                                          VLG_COMP_ARCH arch,
                                          VLG_COMP_OS os,
                                          VLG_COMP_LANG lang,
                                          VLG_COMP_TCOMP tcomp)
{
    RET_ON_KO(VLG_COMP_CalcFieldsSizeAndEntityMaxAlign(max_align,
                                                       entitymap,
                                                       mmbrmap,
                                                       arch,
                                                       os,
                                                       lang,
                                                       tcomp))
    entitydesc.set_entity_max_align(max_align,
                                    arch,
                                    os,
                                    lang,
                                    tcomp);
    size_t fsize = 0;
    //calculate class fields offsets
    RET_ON_KO(VLG_COMP_CalcFieldOffset(entitydesc.get_entity_max_align(arch,
                                                                       os,
                                                                       lang,
                                                                       tcomp),
                                       field_offset,
                                       entitymap,
                                       mmbrmap,
                                       arch,
                                       os,
                                       lang,
                                       tcomp))
    //adjust entity size
    fsize = adjust_entity_size(field_offset,
                               entitydesc.get_entity_max_align(arch,
                                                               os,
                                                               lang,
                                                               tcomp),
                               arch,
                               os,
                               lang,
                               tcomp);

    entitydesc.set_entity_size(fsize,
                               arch,
                               os,
                               lang,
                               tcomp);
    return vlg::RetCode_OK;
}


/***********************************
PARSE- VLG_COMP_ParseFild
***********************************/
RetCode VLG_COMP_ParseFild(unsigned long &lnum,
                           unsigned short &mmbrid,
                           std::string &str_vlg_type,
                           vlg::str_tok &tknz,
                           std::map<std::string, std::string> &definemap,
                           std::map<std::string, entity_desc_comp *> &entitymap,
                           std::map<std::string, member_desc_comp *> &mmbrmap)
{
    std::string tkn, symb_name;
    Type fld_type = Type_UNDEFINED;
    size_t fld_nmemb = 0;
    entity_desc_comp *fld_entity_desc = nullptr;
    char *fld_desc = nullptr;

    RET_ON_KO(VLG_COMP_ParseType(lnum,
                                 str_vlg_type,
                                 tknz,
                                 definemap,
                                 entitymap,
                                 fld_type,
                                 fld_nmemb,
                                 &fld_entity_desc,
                                 symb_name))

    if(!symb_name.length()) {
        while(tknz.next_token(tkn, CR_DF_DLMT, true)) {
            CR_SKIP_SP_TABS(tkn)
            // ok we got symb name.
            symb_name.assign(tkn);
            break;
        }
    }

    RET_ON_KO(VLG_COMP_CheckSymbol(lnum,
                                   symb_name,
                                   definemap,
                                   entitymap,
                                   &mmbrmap,
                                   VLG_COMP_RSRV_WORD,
                                   VLG_COMP_SYMB_NAME))

    //we read an opt description
    RET_ON_KO(VLG_COMP_ReadString(lnum, tknz, &fld_desc, false, true))

    mmbrid++;
    fld_nmemb = fld_nmemb ? fld_nmemb : 1;
    NEntityType etype = NEntityType_UNDEFINED;
    if(fld_type == Type_ENTITY) {
        //user-defined type
        etype = fld_entity_desc->get_nentity_type();
    }
    if(fld_type == Type_ASCII && fld_nmemb > 1) {
        //adjust size +1 for string null terminator
        fld_nmemb++;
    }

    //built-in types default to zero
    unsigned int fld_entityid = 0;
    if(fld_type == Type_ENTITY) {
        //user-defined type id
        fld_entityid = fld_entity_desc->get_entityid();
    }
    //at this point we can create a VLG_MEMBER_DESC_COMP for this field
    member_desc_comp *mmbrdesc = new member_desc_comp(mmbrid,
                                                      MemberType_FIELD,
                                                      strdup(symb_name.c_str()),
                                                      fld_desc,
                                                      fld_type,
                                                      fld_nmemb,
                                                      fld_entityid,
                                                      (fld_type == Type_ENTITY) ? fld_entity_desc->get_nentity_name() : nullptr,
                                                      etype,
                                                      0);
    mmbrmap[symb_name] = mmbrdesc;
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseId
***********************************/
RetCode VLG_COMP_ParseId(unsigned long &lnum,
                         vlg::str_tok &tknz,
                         std::map<std::string, std::string> &definemap,
                         unsigned int &id)
{
    std::string tkn;
    while(tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        auto it = definemap.end();
        if(vlg::string_is_number(tkn.c_str())) {
            //this is a number
            id = atoi(tkn.c_str());
            break;
        } else if((it = definemap.find(tkn)) != definemap.end()) {
            //this is a @define
            if(vlg::string_is_number(it->second.c_str())) {
                id = atoi(it->second.c_str());
                break;
            } else {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
        } else {
            //unexpected token
            VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
            return vlg::RetCode_KO;
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseFildSet
***********************************/
RetCode VLG_COMP_ParseFildSet(unsigned long &lnum,
                              vlg::str_tok &tknz,
                              std::map<std::string, std::string> &definemap,
                              std::map<std::string, entity_desc_comp *> &entitymap,
                              std::map<std::string, member_desc_comp *> &mmbrmap,
                              std::map<unsigned short, key_desc_comp *> &keymap,
                              std::set<member_desc_comp *> &mmbrset)
{
    std::string tkn;
    bool fildset_valid = false,
         cbl_read = false,
         fild_reading_allwd = false;
    member_desc_comp *mmbrptr = nullptr;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA CR_TK_CBL CR_TK_CBR, true)) {
        CR_SKIP_SP_TABS(tkn)
        if(tkn == CR_TK_CBL) {
            if(cbl_read) {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
            fild_reading_allwd = true;
            cbl_read = true;
        } else if(tkn == CR_TK_CBR) {
            if(fildset_valid) {
                break;
            } else {
                VLG_COMP_PARSE_KEYSET_INVALID(lnum);
                return vlg::RetCode_KO;
            }
        } else if(tkn == CR_TK_COMA) {
            if(fild_reading_allwd) {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            } else {
                fild_reading_allwd = true;
                fildset_valid = false;
            }
        } else {
            if(fild_reading_allwd) {
                //read key member ...
                RET_ON_KO(VLG_COMP_CheckKeySymbol(lnum,
                                                  tkn,
                                                  mmbrmap,
                                                  VLG_COMP_RSRV_WORD,
                                                  VLG_COMP_KSYMB_NAME))
                mmbrptr = mmbrmap.at(tkn);
                // ok we got key field.
                mmbrset.insert(mmbrptr);
                fildset_valid = true;
                fild_reading_allwd = false;
            } else {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseKey
***********************************/
RetCode VLG_COMP_ParseKey(unsigned long &lnum,
                          vlg::str_tok &tknz,
                          std::map<std::string, std::string> &definemap,
                          std::map<std::string, entity_desc_comp *> &entitymap,
                          std::map<std::string, member_desc_comp *> &mmbrmap,
                          std::map<unsigned short, key_desc_comp *> &keymap)
{
    std::string tkn;
    unsigned int k_id = 0;
    bool primary = false, mmbrset_read = false, id_read = false;
    std::set<member_desc_comp *> mmbrset;
    //check if primary
    while(tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; break)
        if(tkn == VLG_RWRD_PRIMARY) {
            primary = true;
            break;
        } else {
            //unexpected token
            VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
            return vlg::RetCode_KO;
        }
    }
    while(tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; continue)
        if(tkn == VLG_RWRD_PFX VLG_RWRD_ID) {
            //parse @id
            //@fixme if id already read...
            RET_ON_KO(VLG_COMP_ParseId(lnum,
                                       tknz,
                                       definemap,
                                       k_id))
            id_read = true;
        } else if(tkn == VLG_RWRD_PFX VLG_RWRD_FILDSET) {
            //parse @fildset
            //@fixme if fildset already read...
            RET_ON_KO(VLG_COMP_ParseFildSet(lnum,
                                            tknz,
                                            definemap,
                                            entitymap,
                                            mmbrmap,
                                            keymap,
                                            mmbrset))
            mmbrset_read = true;
        } else if(tkn == VLG_RWRD_PFX VLG_RWRD_KEY_END) {
            //parse @key_end
            if(id_read && mmbrset_read) {
                break;
            } else {
                return vlg::RetCode_KO;
            }
        } else {
            //unexpected token
            VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
            return vlg::RetCode_KO;
        }
    }
    //at this point we can create a VLG_KEY_DESC for this key
    key_desc_comp *keydesc = nullptr;
    CMD_ON_NUL(keydesc = new key_desc_comp((unsigned short)k_id, primary), exit(1))
    CMD_ON_KO(keydesc->init(mmbrset), exit(1))
    keymap[k_id] = keydesc;
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseEnum
***********************************/
RetCode VLG_COMP_ParseEnum(unsigned long &lnum,
                           vlg::str_tok &tknz,
                           std::map<std::string, std::string> &definemap,
                           std::map<std::string, entity_desc_comp *> &entitymap)
{
    std::string tkn, symb_name;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_CB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        RET_ON_KO(VLG_COMP_CheckSymbol(lnum,
                                       tkn,
                                       definemap,
                                       entitymap,
                                       nullptr,
                                       VLG_COMP_RSRV_WORD,
                                       VLG_COMP_SYMB_NAME))
        symb_name.assign(tkn);
        // ok we got symb name.
        break;
    }

    RET_ON_KO(VLG_COMP_ReadOpeningCurlyBrace(lnum, tknz))

    long last_enum_val = -1; //last value of enum
    std::map<std::string, member_desc_comp *> mmbrmap; //map symb -> mmbrdesc
    unsigned short mmbrid = 0;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_CB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; continue)
        CR_BREAK_ON_TKN(tkn, CR_TK_CBR)

        RET_ON_KO(VLG_COMP_ParseVal(lnum,
                                    mmbrid,
                                    tkn,
                                    last_enum_val,
                                    tknz,
                                    definemap,
                                    entitymap,
                                    mmbrmap))
    }
    size_t field_num = mmbrmap.size();
    //at this point we can create a VLG_ENTITY_DESC_COMP for this ENUM
    entity_desc_comp *entitydesc = new entity_desc_comp(0,  //0 for enum
                                                        NEntityType_NENUM,
                                                        strdup(unit_nmspace.c_str()),
                                                        strdup(symb_name.c_str()),
                                                        0, //alloc f
                                                        (uint32_t)field_num, //field num
                                                        false);

    // VLG_COMP_ARCH: x86_64
    // VLG_COMP_OS: win
    // VLG_COMP_LANG: CPP
    // VLG_COMP_TCOMP: MSVC

    entitydesc->set_entity_size(4,
                                VLG_COMP_ARCH_x86_64,
                                VLG_COMP_OS_win,
                                VLG_COMP_LANG_CPP,
                                VLG_COMP_TCOMP_MSVC);
    entitydesc->set_entity_max_align(4,
                                     VLG_COMP_ARCH_x86_64,
                                     VLG_COMP_OS_win,
                                     VLG_COMP_LANG_CPP,
                                     VLG_COMP_TCOMP_MSVC);

    // VLG_COMP_ARCH: x86_64
    // VLG_COMP_OS: unix
    // VLG_COMP_LANG: CPP
    // VLG_COMP_TCOMP: gcc

    entitydesc->set_entity_size(4,
                                VLG_COMP_ARCH_x86_64,
                                VLG_COMP_OS_unix,
                                VLG_COMP_LANG_CPP,
                                VLG_COMP_TCOMP_GCC);
    entitydesc->set_entity_max_align(4,
                                     VLG_COMP_ARCH_x86_64,
                                     VLG_COMP_OS_unix,
                                     VLG_COMP_LANG_CPP,
                                     VLG_COMP_TCOMP_GCC);
    entitydesc->extend(mmbrmap);
    entitymap[entitydesc->get_nentity_name()] = entitydesc;
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseClass
***********************************/
RetCode VLG_COMP_ParseEntity(unsigned long &lnum,
                             vlg::str_tok &tknz,
                             std::map<std::string, std::string> &definemap,
                             std::map<std::string, entity_desc_comp *> &entitymap)
{
    bool id_decl = false;
    std::string tkn, symb_name;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_CB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        RET_ON_KO(VLG_COMP_CheckSymbol(lnum,
                                       tkn,
                                       definemap,
                                       entitymap,
                                       nullptr,
                                       VLG_COMP_RSRV_WORD,
                                       VLG_COMP_SYMB_NAME))
        symb_name.assign(tkn);
        // ok we got symb name.
        break;
    }

    RET_ON_KO(VLG_COMP_ReadOpeningCurlyBrace(lnum, tknz))

    std::map<std::string, member_desc_comp *> mmbrmap;  //map symb -> mmbrdesc
    std::map<unsigned short, key_desc_comp *> keymap;

    unsigned int nclass_id = 0;
    unsigned short mmbrid = 0;
    bool persistent = false;
    while(tknz.next_token(tkn, CR_DF_DLMT CR_CB_DLMT CR_RB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; continue)
        if(tkn == VLG_RWRD_PFX VLG_RWRD_ID) {
            //parse @id
            if(!id_decl) {
                RET_ON_KO(VLG_COMP_ParseId(lnum,
                                           tknz,
                                           definemap,
                                           nclass_id))
                id_decl = true;
            } else {
                VLG_COMP_PARSE_CLASS_ID_ALRDY_DECL(lnum, symb_name.c_str());
                return vlg::RetCode_KO;
            }
        } else if(tkn == VLG_RWRD_PFX VLG_RWRD_PERSISTENT) {
            //parse @persistent
            if(persistent) {
                VLG_COMP_PARSE_CLASS_ALRDY_PERSISTENT(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
            persistent = true;
        } else if(tkn == VLG_RWRD_PFX VLG_RWRD_KEY) {
            //parse @key
            if(!persistent) {
                VLG_COMP_PARSE_CLASS_NOT_PERSISTENT(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
            RET_ON_KO(VLG_COMP_ParseKey(lnum,
                                        tknz,
                                        definemap,
                                        entitymap,
                                        mmbrmap,
                                        keymap))
        } else if(tkn == CR_TK_CBR) {
            //parse @class_end
            break;
        } else {
            //parse @fild
            RET_ON_KO(VLG_COMP_ParseFild(lnum,
                                         mmbrid,
                                         tkn,
                                         tknz,
                                         definemap,
                                         entitymap,
                                         mmbrmap))

        }
    }

    if(!id_decl) {
        VLG_COMP_PARSE_CLASS_ID_NOT_DECL(lnum, symb_name.c_str());
        return vlg::RetCode_KO;
    }

    size_t field_num = mmbrmap.size();
    //at this point we can create a VLG_ENTITY_DESC_COMP for this Class
    entity_desc_comp *entitydesc = new entity_desc_comp(nclass_id,
                                                        NEntityType_NCLASS,
                                                        strdup(unit_nmspace.c_str()),
                                                        strdup(symb_name.c_str()),
                                                        0, //alloc f
                                                        (uint32_t)field_num, //field num
                                                        persistent);

    std::vector<member_desc_comp *> id_mmbrvec(mmbrmap.size());
    std::for_each(mmbrmap.begin(), mmbrmap.end(), [&](std::pair<const std::string, member_desc_comp *> &it) {
        id_mmbrvec[it.second->get_member_id()-1] = it.second;
    });

    size_t max_align = 1;
    size_t field_offset = NCLASS_DERREP_STRT_OFFST_x86_64;

    // VLG_COMP_ARCH: x86_64
    // VLG_COMP_OS: win
    // VLG_COMP_LANG: CPP
    // VLG_COMP_TCOMP: MSVC
    RET_ON_KO(VLG_COMP_CalcTCompDependantValues(max_align,
                                                field_offset,
                                                *entitydesc,
                                                entitymap,
                                                id_mmbrvec,
                                                VLG_COMP_ARCH_x86_64,
                                                VLG_COMP_OS_win,
                                                VLG_COMP_LANG_CPP,
                                                VLG_COMP_TCOMP_MSVC))

    // VLG_COMP_ARCH: x86_64
    // VLG_COMP_OS: unix
    // VLG_COMP_LANG: CPP
    // VLG_COMP_TCOMP: gcc
    max_align = 1;
    field_offset = NCLASS_DERREP_STRT_OFFST_x86_64;
    RET_ON_KO(VLG_COMP_CalcTCompDependantValues(max_align,
                                                field_offset,
                                                *entitydesc,
                                                entitymap,
                                                id_mmbrvec,
                                                VLG_COMP_ARCH_x86_64,
                                                VLG_COMP_OS_unix,
                                                VLG_COMP_LANG_CPP,
                                                VLG_COMP_TCOMP_GCC))

    CMD_ON_KO(entitydesc->extend(mmbrmap, keymap), exit(1))
    entitymap[entitydesc->get_nentity_name()] = entitydesc;
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseDefine
***********************************/
RetCode VLG_COMP_ParseDefine(unsigned long &lnum,
                             vlg::str_tok &tknz,
                             std::map<std::string, std::string> &definemap,
                             std::map<std::string, entity_desc_comp *> &entitymap)
{
    std::string tkn, define_name, define_val;
    //@fixme add separators and special chars
    while(tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        RET_ON_KO(VLG_COMP_CheckSymbol(lnum,
                                       tkn,
                                       definemap,
                                       entitymap,
                                       nullptr,
                                       VLG_COMP_RSRV_WORD,
                                       VLG_COMP_SYMB_NAME))
        define_name.assign(tkn);
        // ok we got define name.
        break;
    }
    //@fixme add separators and special chars
    while(tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        //we expect a define name, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, VLG_COMP_PARSE_EXP(lnum, VLG_COMP_SYMB_NAME); return vlg::RetCode_KO)
        define_val.assign(tkn);
        // ok we got define val.
        break;
    }
    definemap[define_name] = define_val;
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseInclude
***********************************/

RetCode VLG_COMP_ParseInclude(unsigned long &lnum,
                              vlg::str_tok &tknz,
                              std::map<std::string, std::string> &definemap,
                              std::map<std::string, entity_desc_comp *> &entitymap,
                              char **modname,
                              char **modver)
{
    char *incl_fname = nullptr;
    RET_ON_KO(VLG_COMP_ReadString(lnum, tknz, &incl_fname))
    FILE *fdesc = nullptr;
    std::string data; //file content loaded on data
    RET_ON_KO(open_input_file(incl_fname, &fdesc))
    RET_ON_KO(load_file(fdesc, data))
    printf(STG_FMT_0,
           VLG_COMP_INF_START,
           VLG_COMP_INF_PARS_FILE,
           incl_fname);
    RET_ON_KO(parse_data(incl_fname,
                         data,
                         definemap,
                         entitymap,
                         modname,
                         modver))
    printf(STG_FMT_0,
           VLG_COMP_INF_END,
           VLG_COMP_INF_PARS_FILE,
           incl_fname);
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseNamespace
***********************************/
RetCode VLG_COMP_ParseNamespace(unsigned long &lnum,
                                vlg::str_tok &tknz,
                                std::map<std::string, std::string> &definemap,
                                std::map<std::string, entity_desc_comp *> &entitymap)
{
    char *n_space = nullptr;
    RET_ON_KO(VLG_COMP_ReadString(lnum, tknz, &n_space))
    unit_nmspace.assign(n_space);
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseNamespace_end
***********************************/
RetCode VLG_COMP_ParseNamespace_end(unsigned long &lnum,
                                    vlg::str_tok &tknz,
                                    std::map<std::string, std::string> &definemap,
                                    std::map<std::string, entity_desc_comp *> &entitymap)
{
    unit_nmspace.assign("");
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseModelName
***********************************/
RetCode VLG_COMP_ParseModelName(unsigned long &lnum,
                                vlg::str_tok &tknz,
                                std::map<std::string, std::string> &definemap,
                                std::map<std::string, entity_desc_comp *> &entitymap,
                                char **modname)
{
    RET_ON_KO(VLG_COMP_ReadString(lnum, tknz, modname))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseModelName
***********************************/
RetCode VLG_COMP_ParseModelVer(unsigned long &lnum,
                               vlg::str_tok &tknz,
                               std::map<std::string, std::string> &definemap,
                               std::map<std::string, entity_desc_comp *> &entitymap,
                               char **modver)
{
    RET_ON_KO(VLG_COMP_ReadString(lnum, tknz, modver))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseData
***********************************/
RetCode parse_data(const char *fname,
                   std::string &data,
                   std::map<std::string, std::string> &definemap,
                   std::map<std::string, entity_desc_comp *> &entitymap,
                   char **modname,
                   char **modver)
{
    unsigned long lnum = 1;
    bool parsing_comment = false;
    std::string tkn;
    vlg::str_tok tknz(data);
    unit_nmspace.assign("");
    while(tknz.next_token(tkn, CR_DF_DLMT VLG_TK_COMMENT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; parsing_comment = false; continue)
        if(!parsing_comment) {
            if(tkn == VLG_RWRD_PFX VLG_RWRD_INCLUDE) {
                //parse @include
                RET_ON_KO(VLG_COMP_ParseInclude(lnum,
                                                tknz,
                                                definemap,
                                                entitymap,
                                                modname,
                                                modver))
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_MODLNAME) {
                //parse @model_name
                if(*modname) {
                    VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                    return vlg::RetCode_KO;
                } else {
                    RET_ON_KO(VLG_COMP_ParseModelName(lnum,
                                                      tknz,
                                                      definemap,
                                                      entitymap,
                                                      modname))
                }
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_MODLVER) {
                //parse @model_version
                if(*modver) {
                    VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                    return vlg::RetCode_KO;
                } else {
                    RET_ON_KO(VLG_COMP_ParseModelVer(lnum,
                                                     tknz,
                                                     definemap,
                                                     entitymap,
                                                     modver))
                }
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_DEFINE) {
                //parse @define
                RET_ON_KO(VLG_COMP_ParseDefine(lnum,
                                               tknz,
                                               definemap,
                                               entitymap))
            } else if(tkn == VLG_RWRD_ENUM) {
                //parse @enum
                RET_ON_KO(VLG_COMP_ParseEnum(lnum,
                                             tknz,
                                             definemap,
                                             entitymap))
            } else if(tkn == VLG_RWRD_NCLASS) {
                //parse @class
                RET_ON_KO(VLG_COMP_ParseEntity(lnum,
                                               tknz,
                                               definemap,
                                               entitymap))
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_NAMESPACE) {
                //parse @namespace
                RET_ON_KO(VLG_COMP_ParseNamespace(lnum,
                                                  tknz,
                                                  definemap,
                                                  entitymap))
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_NAMESPACE_END) {
                //parse @namespace_end
                RET_ON_KO(VLG_COMP_ParseNamespace_end(lnum,
                                                      tknz,
                                                      definemap,
                                                      entitymap))
            } else if(tkn == VLG_TK_COMMENT) {
                //comment begin
                parsing_comment = true;
            } else {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.c_str());
                return vlg::RetCode_KO;
            }
        }
    }
    return vlg::RetCode_OK;
}

}
