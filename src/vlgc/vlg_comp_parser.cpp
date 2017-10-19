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

#include "vlg_compiler.h"

namespace vlg {

/***********************************
glob
***********************************/
vlg::ascii_string unit_nmspace;

/***********************************
HMAP: tcomp_packing_map
***********************************/
static vlg::hash_map *tcomp_packing_map = NULL;

vlg::RetCode LoadTCompPackingMap(vlg::hash_map &map)
{
    size_t packing_value = 8;
    unsigned int key = 0;
    packing_value = 8;
    key = TCOMP_DEP_GEN_KEY(VLG_COMP_ARCH_x86_64,
                            VLG_COMP_OS_win,
                            VLG_COMP_LANG_CPP,
                            VLG_COMP_TCOMP_MSVC);
    RETURN_IF_NOT_OK(map.put(&key, &packing_value))
    packing_value = 8;
    key = TCOMP_DEP_GEN_KEY(VLG_COMP_ARCH_x86_64,
                            VLG_COMP_OS_unix,
                            VLG_COMP_LANG_CPP,
                            VLG_COMP_TCOMP_GCC);
    RETURN_IF_NOT_OK(map.put(&key, &packing_value))
    return vlg::RetCode_OK;
}

vlg::hash_map &GetTCompPackingMap()
{
    if(!tcomp_packing_map) {
        tcomp_packing_map = new vlg::hash_map(sizeof(size_t),
                                              sizeof(unsigned int));
        if(!tcomp_packing_map) {
            EXIT_ACTION("failed creating tcomp_packing_map")
        }
        COMMAND_IF_NOT_OK(tcomp_packing_map->init(HM_SIZE_MINI),
                          EXIT_ACTION("init tcomp_packing_map"))
        COMMAND_IF_NOT_OK(LoadTCompPackingMap(*tcomp_packing_map),
                          EXIT_ACTION("populating tcomp_packing_map"))
    }
    return *tcomp_packing_map;
}


/***********************************
HMAP: rword_map
***********************************/

static vlg::hash_map *rword_map = NULL;

vlg::RetCode LoadResWordsMap(vlg::hash_map &map)
{
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_ENUM, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_NCLASS, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_ID, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_PRIMARY, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_PERSISTENT, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_KEY, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_KEY_END, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_FILDSET, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_NAMESPACE, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_NAMESPACE_END, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_MODLNAME, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWRD_PFX VLG_RWRD_MODLVER, ""))
    return vlg::RetCode_OK;
}

vlg::hash_map &GetResWordsMap()
{
    if(!rword_map) {
        rword_map = new vlg::hash_map(vlg::sngl_cstr_obj_mng(),
                                      vlg::sngl_cstr_obj_mng());
        if(!rword_map) {
            EXIT_ACTION_STDOUT("failed creating rword_map")
        }
        COMMAND_IF_NOT_OK(rword_map->init(HM_SIZE_MINI), EXIT_ACTION("init rword_map"))
        COMMAND_IF_NOT_OK(LoadResWordsMap(*rword_map), EXIT_ACTION("loading rword_map"))
    }
    return *rword_map;
}

/***********************************
HMAP: types_map
***********************************/

static vlg::hash_map *types_map = NULL;

vlg::RetCode LoadTypesMap(vlg::hash_map &map)
{
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_BOOL, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_INT_16, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_UINT_16, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_INT_32, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_UINT_32, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_INT_64, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_UINT_64, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_FLOAT_32, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_FLOAT_64, ""))
    RETURN_IF_NOT_OK(map.put(VLG_RWORD_T_ASCII, ""))
    return vlg::RetCode_OK;
}

vlg::hash_map &GetTypesMap()
{
    if(!types_map) {
        types_map = new vlg::hash_map(vlg::sngl_cstr_obj_mng(),
                                      vlg::sngl_cstr_obj_mng());
        if(!types_map) {
            EXIT_ACTION_STDOUT("failed creating types_map")
        }
        COMMAND_IF_NOT_OK(types_map->init(HM_SIZE_MINI), EXIT_ACTION("init types_map"))
        COMMAND_IF_NOT_OK(LoadTypesMap(*types_map), EXIT_ACTION("loading types_map"))
    }
    return *types_map;
}

Type VLG_COMP_StrToTYPE(const vlg::ascii_string &str)
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
vlg::RetCode VLG_COMP_CheckSymbol(unsigned long &lnum,
                                  vlg::ascii_string &tkn,
                                  vlg::hash_map *definemap,
                                  vlg::hash_map *entitymap,
                                  vlg::hash_map *mmbrmap,
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
    if(!GetResWordsMap().contains_key(tkn.internal_buff())) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.internal_buff(), EXP_CTG);
        return vlg::RetCode_KO;
    }
    //check if this is a built-in compiler type
    if(!GetTypesMap().contains_key(tkn.internal_buff())) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.internal_buff(), EXP_CTG);
        return vlg::RetCode_KO;
    }
    //if we have already seen this symbol (define), we return with error
    if(definemap && !definemap->contains_key(tkn.internal_buff())) {
        VLG_COMP_PARSE_SYMB_ALRDY_DECL(lnum, tkn.internal_buff());
        return vlg::RetCode_KO;
    }
    //if we have already seen this symbol (entity), we return with error
    if(entitymap && !entitymap->contains_key(tkn.internal_buff())) {
        VLG_COMP_PARSE_SYMB_ALRDY_DECL(lnum, tkn.internal_buff());
        return vlg::RetCode_KO;
    }
    //if we have already seen this symbol (member), we return with error
    if(mmbrmap && !mmbrmap->contains_key(tkn.internal_buff())) {
        VLG_COMP_PARSE_SYMB_ALRDY_DECL(lnum, tkn.internal_buff());
        return vlg::RetCode_KO;
    }
    return vlg::RetCode_OK;
}

/***********************************
CONSISTENCY- VLG_COMP_CheckKeySymbol
***********************************/
//@fixme: add separator check (,)
vlg::RetCode VLG_COMP_CheckKeySymbol(unsigned long &lnum,
                                     vlg::ascii_string &tkn,
                                     vlg::hash_map *mmbrmap,
                                     const char *FND_CGT,    //found category
                                     const char *EXP_CTG     //expected category
                                    )
{
    vlg::ascii_string hint;
    COMMAND_IF_NOT_OK(hint.assign(FND_CGT), exit(1))
    COMMAND_IF_NOT_OK(hint.append(CR_TK_SP), exit(1))
    COMMAND_IF_NOT_OK(hint.append(EXP_CTG), exit(1))
    //we expect symb name, so we return with error if newline found.
    if(is_new_line(tkn)) {
        VLG_COMP_PARSE_EXP(lnum, EXP_CTG);
        return vlg::RetCode_KO;
    }
    //check if this is a reserved word
    if(!GetResWordsMap().contains_key(tkn.internal_buff())) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.internal_buff(), hint.internal_buff());
        return vlg::RetCode_KO;
    }
    //check if this is a built-in compiler type
    if(!GetTypesMap().contains_key(tkn.internal_buff())) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.internal_buff(), hint.internal_buff());
        return vlg::RetCode_KO;
    }
    //this must be a member field
    if(mmbrmap->contains_key(tkn.internal_buff())) {
        VLG_COMP_PARSE_FND_EXP(lnum, tkn.internal_buff(), hint.internal_buff());
        return vlg::RetCode_KO;
    }
    return vlg::RetCode_OK;
}

/***********************************
READ- VLG_COMP_ReadOpeningCurlyBrace
***********************************/
vlg::RetCode VLG_COMP_ReadOpeningCurlyBrace(unsigned long &lnum,
                                            vlg::ascii_string_tok &tknz)
{
    vlg::ascii_string tkn;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_CBL, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_SKIP_NEWLINE(tkn)
        if(tkn != CR_TK_CBL) {
            VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
            return vlg::RetCode_KO;
        }
        break;
    }
    return vlg::RetCode_OK;
}

/***********************************
READ- VLG_COMP_ReadInteger
***********************************/
vlg::RetCode VLG_COMP_ReadInteger(unsigned long &lnum,
                                  vlg::ascii_string_tok &tknz,
                                  long &along)
{
    vlg::ascii_string tkn;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA, true)) {
        CR_SKIP_SP_TABS(tkn)
        //we expect an int, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn,
                             VLG_COMP_PARSE_EXP(lnum, VLG_COMP_SYMB_NAME);
                             return vlg::RetCode_KO)
        if(vlg::string_is_int_number(tkn.internal_buff())) {
            along = atol(tkn.internal_buff());
        } else {
            VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
            return vlg::RetCode_KO;
        }
        break;
    }
    return vlg::RetCode_OK;
}

/***********************************
READ- VLG_COMP_ReadString
***********************************/
vlg::RetCode VLG_COMP_ReadString(unsigned long &lnum,
                                 vlg::ascii_string_tok &tknz,
                                 char **newstr, bool begin_quote_read = false,
                                 bool opt = false)
{
    vlg::ascii_string tkn;
    if(!begin_quote_read) {
        while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_QT CR_TK_COMA, true)) {
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
                VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            }
        }
    }
    while(!tknz.next_token(tkn, CR_NL_DLMT CR_TK_QT, true)) {
        //we expect a string, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, VLG_COMP_PARSE_EXP(lnum, VLG_COMP_STRING);
                             return vlg::RetCode_KO)
        if(!*newstr) {
            COMMAND_IF_NULL(*newstr = tkn.new_buffer(), exit(1))
        } else {
            if(tkn == CR_TK_QT) {
                //ok we have read the ending quote of the string
                break;
            } else {
                VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseVal
***********************************/
vlg::RetCode VLG_COMP_ParseVal(unsigned long &lnum,
                               unsigned short &mmbrid,
                               vlg::ascii_string &symb_name,
                               long &last_enum_val,
                               vlg::ascii_string_tok &tknz,
                               vlg::hash_map &definemap,
                               vlg::hash_map &entitymap,
                               vlg::hash_map &mmbrmap)
{
    RETURN_IF_NOT_OK(VLG_COMP_CheckSymbol(lnum,
                                          symb_name,
                                          &definemap,
                                          &entitymap,
                                          &mmbrmap,
                                          VLG_COMP_RSRV_WORD,
                                          VLG_COMP_SYMB_NAME))
    mmbrid++;

    vlg::ascii_string tkn;
    long enum_val = last_enum_val + 1;
    bool enum_val_read = false;
    char *desc = NULL;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_EQUAL CR_TK_QT, true)) {
        CR_SKIP_SP_TABS(tkn)
        if(tkn == CR_TK_EQUAL) {
            //ok now we expect an integer value
            if(enum_val_read) {
                //we have already read it..
                VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            }
            RETURN_IF_NOT_OK(VLG_COMP_ReadInteger(lnum, tknz, enum_val))
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
            RETURN_IF_NOT_OK(VLG_COMP_ReadString(lnum, tknz, &desc, true))
            break;
        }
    }
    last_enum_val = enum_val;
    //at this point we can create a VLG_MEMBER_DESC_COMP for this enum member
    member_desc_comp *mmbrdesc = NULL;
    COMMAND_IF_NULL(
        mmbrdesc = new member_desc_comp(mmbrid,
                                        MemberType_ENUM_VALUE,
                                        symb_name.new_buffer(),
                                        desc,
                                        Type_INT32,
                                        1,
                                        0,
                                        NULL,
                                        EntityType_ENUM,
                                        last_enum_val), exit(1))
    COMMAND_IF_NOT_OK(mmbrdesc->init(), exit(1))
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
    COMMAND_IF_NOT_OK(mmbrmap.put(symb_name.internal_buff(), &mmbrdesc), exit(1))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseNMemb
we expect an integer value or an expression (a + b + c ..)
or a combination of (a + DEFINE_0 + b + DEFINE_1...)
***********************************/
vlg::RetCode VLG_COMP_ParseNMemb(unsigned long &lnum,
                                 vlg::ascii_string_tok &tknz,
                                 vlg::hash_map &definemap,
                                 size_t &nmemb)
{
    vlg::ascii_string tkn;
    const char *define_val = NULL;
    bool exp_valid = false, plus_allwd = false;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_PLUS CR_TK_RBR, true)) {
        CR_SKIP_SP_TABS(tkn)
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
        } else if(vlg::string_is_int_number(tkn.internal_buff())) {
            //this is a number
            nmemb += atol(tkn.internal_buff());
            exp_valid = plus_allwd = true;
        } else if((define_val = (const char *)definemap.get(tkn.internal_buff()))) {
            //this is a @define
            if(vlg::string_is_int_number(define_val)) {
                nmemb += atol(define_val);
                exp_valid = plus_allwd = true;
            } else {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            }
        } else {
            //unexpected token
            VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
            return vlg::RetCode_KO;
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseType
***********************************/
vlg::RetCode VLG_COMP_ParseType(unsigned long &lnum,
                                vlg::ascii_string &str_vlg_type,
                                vlg::ascii_string_tok &tknz,
                                vlg::hash_map &definemap,
                                vlg::hash_map &entitymap,
                                Type &vlg_type,
                                size_t &nmemb,
                                entity_desc_comp **desc,
                                vlg::ascii_string &symb_name)
{
    entity_desc_comp *tmpdesc = NULL;
    if(!GetTypesMap().contains_key(str_vlg_type.internal_buff())) {
        //build-in type
        vlg_type = VLG_COMP_StrToTYPE(str_vlg_type);
        //ok we have type
    } else if(!entitymap.get(str_vlg_type.internal_buff(), &tmpdesc)) {
        //user defined type
        vlg_type = Type_ENTITY;
        *desc = tmpdesc;
        //ok we have type
    } else {
        VLG_COMP_PARSE_TYPE_UNRECOGN(lnum, str_vlg_type.internal_buff());
        return vlg::RetCode_KO;
    }

    vlg::ascii_string tkn;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA CR_RB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        //we expect to read nmemb or symbol, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, VLG_COMP_PARSE_EXP(lnum, VLG_COMP_SYMB_NAME);
                             return vlg::RetCode_KO)
        if(tkn == CR_TK_RBL) {
            RETURN_IF_NOT_OK(VLG_COMP_ParseNMemb(lnum, tknz, definemap, nmemb))
            break;
        } else {
            //we have read symb here.
            RETURN_IF_NOT_OK(symb_name.assign(tkn))
            break;
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
CALC- VLG_COMP_CalcFieldOffset
***********************************/
vlg::RetCode VLG_COMP_CalcFieldOffset(size_t max_align,
                                      size_t &mmbr_offset,
                                      vlg::hash_map &entitymap,
                                      vlg::hash_map &mmbrmap,
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
    char outmsg[256] = {0};
    sprintf(outmsg,
            "TCompPackingMap get failed. (arch:%d, os:%d, lang:%d, tcomp:%d)", arch, os,
            lang, tcomp);
    COMMAND_IF_NOT_OK(GetTCompPackingMap().get(&key, &packing), EXIT_ACTION(outmsg))
    mmbrmap.start_iteration();
    member_desc_comp *mdesc = NULL;
    //enumerate all members
    while(!mmbrmap.next(NULL, &mdesc)) {
        //it is scalar if it is a primitive type or an enum
        bool scalar = (mdesc->get_field_type() != Type_ENTITY) ||
                      (mdesc->get_field_entity_type() == EntityType_ENUM);
        if(mdesc->get_field_type() == Type_ENTITY &&
                mdesc->get_field_entity_type() != EntityType_ENUM) {
            //we get max align if it is a struct/class
            entity_desc_comp *edesc = NULL;
            if(entitymap.get(mdesc->get_field_usr_str_type(), &edesc)) {
                return vlg::RetCode_KO;
            }
            type_align = edesc->get_entity_max_align(arch, os, lang, tcomp);
        } else if(mdesc->get_field_entity_type() == EntityType_ENUM) {
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
    }
    return vlg::RetCode_OK;
}

/***********************************
CALC- VLG_COMP_CalcFieldsSizeAndEntityMaxAlign
***********************************/
vlg::RetCode VLG_COMP_CalcFieldsSizeAndEntityMaxAlign(size_t &max_align,
                                                      vlg::hash_map &entitymap,
                                                      vlg::hash_map &mmbrmap,
                                                      VLG_COMP_ARCH arch,
                                                      VLG_COMP_OS os,
                                                      VLG_COMP_LANG lang,
                                                      VLG_COMP_TCOMP tcomp)
{
    size_t type_size = 0;
    mmbrmap.start_iteration();
    member_desc_comp *mdesc = NULL;
    //enumerate all members
    while(!mmbrmap.next(NULL, &mdesc)) {
        if(mdesc->get_field_type() == Type_ENTITY) {
            //user-defined type
            entity_desc_comp *fld_entity_desc = NULL;
            if(!entitymap.get(mdesc->get_field_usr_str_type(), &fld_entity_desc)) {
                type_size = fld_entity_desc->get_entity_size(arch,
                                                             os,
                                                             lang,
                                                             tcomp);
                max_align = (fld_entity_desc->get_entity_max_align(arch,
                                                                   os,
                                                                   lang,
                                                                   tcomp) > max_align) ?
                            fld_entity_desc->get_entity_max_align(arch,
                                                                  os,
                                                                  lang,
                                                                  tcomp) : max_align;
            } else {
                return vlg::RetCode_KO;
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
    }
    return vlg::RetCode_OK;
}


/***********************************
CALC- VLG_COMP_CalcTCompDependantValues
***********************************/
vlg::RetCode VLG_COMP_CalcTCompDependantValues(size_t max_align,
                                               size_t field_offset,
                                               entity_desc_comp &entitydesc,
                                               vlg::hash_map &entitymap,
                                               vlg::hash_map &mmbrmap,
                                               VLG_COMP_ARCH arch,
                                               VLG_COMP_OS os,
                                               VLG_COMP_LANG lang,
                                               VLG_COMP_TCOMP tcomp)
{
    RETURN_IF_NOT_OK(VLG_COMP_CalcFieldsSizeAndEntityMaxAlign(max_align,
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
    RETURN_IF_NOT_OK(VLG_COMP_CalcFieldOffset(entitydesc.get_entity_max_align(arch,
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
vlg::RetCode VLG_COMP_ParseFild(unsigned long &lnum,
                                unsigned short &mmbrid,
                                vlg::ascii_string &str_vlg_type,
                                vlg::ascii_string_tok &tknz,
                                vlg::hash_map &definemap,
                                vlg::hash_map &entitymap,
                                vlg::hash_map &mmbrmap)
{
    vlg::ascii_string tkn, symb_name;
    Type fld_type = Type_UNDEFINED;
    size_t fld_nmemb = 0;
    entity_desc_comp *fld_entity_desc = NULL;
    char *fld_desc = NULL;

    RETURN_IF_NOT_OK(VLG_COMP_ParseType(lnum,
                                        str_vlg_type,
                                        tknz,
                                        definemap,
                                        entitymap,
                                        fld_type,
                                        fld_nmemb,
                                        &fld_entity_desc,
                                        symb_name))

    if(!symb_name.length()) {
        while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
            CR_SKIP_SP_TABS(tkn)
            // ok we got symb name.
            RETURN_IF_NOT_OK(symb_name.assign(tkn))
            break;
        }
    }

    RETURN_IF_NOT_OK(VLG_COMP_CheckSymbol(lnum,
                                          symb_name,
                                          &definemap,
                                          &entitymap,
                                          &mmbrmap,
                                          VLG_COMP_RSRV_WORD,
                                          VLG_COMP_SYMB_NAME))

    //we read an opt description
    RETURN_IF_NOT_OK(VLG_COMP_ReadString(lnum, tknz, &fld_desc, false, true))

    mmbrid++;
    fld_nmemb = fld_nmemb ? fld_nmemb : 1;
    EntityType etype = EntityType_UNDEFINED;
    if(fld_type == Type_ENTITY) {
        //user-defined type
        etype = fld_entity_desc->get_entity_type();
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
    member_desc_comp *mmbrdesc = NULL;
    COMMAND_IF_NULL(mmbrdesc = new member_desc_comp(mmbrid,
                                                    MemberType_FIELD,
                                                    symb_name.new_buffer(),
                                                    fld_desc,
                                                    fld_type,
                                                    fld_nmemb,
                                                    fld_entityid,
                                                    (fld_type == Type_ENTITY) ? fld_entity_desc->get_entity_name() : NULL,
                                                    etype,
                                                    0   //not sign. for fields
                                                   ), exit(1))
    COMMAND_IF_NOT_OK(mmbrdesc->init(), exit(1))
    COMMAND_IF_NOT_OK(mmbrmap.put(symb_name.internal_buff(), &mmbrdesc), exit(1))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseId
***********************************/
vlg::RetCode VLG_COMP_ParseId(unsigned long &lnum,
                              vlg::ascii_string_tok &tknz,
                              vlg::hash_map &definemap,
                              unsigned int &id)
{
    vlg::ascii_string tkn;
    const char *define_val = NULL;
    while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        if(vlg::string_is_int_number(tkn.internal_buff())) {
            //this is a number
            id = atoi(tkn.internal_buff());
            break;
        } else if((define_val = (const char *)definemap.get(tkn.internal_buff()))) {
            //this is a @define
            if(vlg::string_is_int_number(define_val)) {
                id = atoi(define_val);
                break;
            } else {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            }
        } else {
            //unexpected token
            VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
            return vlg::RetCode_KO;
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseFildSet
***********************************/
vlg::RetCode VLG_COMP_ParseFildSet(unsigned long &lnum,
                                   vlg::ascii_string_tok &tknz,
                                   vlg::hash_map &definemap,
                                   vlg::hash_map &entitymap,
                                   vlg::hash_map &mmbrmap,
                                   vlg::hash_map &keymap,
                                   vlg::linked_list  &mmbrset)
{
    vlg::ascii_string tkn;
    bool    fildset_valid = false,
            cbl_read = false,
            fild_reading_allwd = false;
    void *mmbrptr = NULL;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA CR_TK_CBL CR_TK_CBR, true)) {
        CR_SKIP_SP_TABS(tkn)
        if(tkn == CR_TK_CBL) {
            if(cbl_read) {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
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
                VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            } else {
                fild_reading_allwd = true;
                fildset_valid = false;
            }
        } else {
            if(fild_reading_allwd) {
                //read key member ...
                RETURN_IF_NOT_OK(VLG_COMP_CheckKeySymbol(lnum,
                                                         tkn,
                                                         &mmbrmap,
                                                         VLG_COMP_RSRV_WORD,
                                                         VLG_COMP_KSYMB_NAME))
                RETURN_IF_NOT_OK(mmbrmap.get(tkn.internal_buff(), &mmbrptr))
                // ok we got key field.
                RETURN_IF_NOT_OK(mmbrset.push_back(&mmbrptr))
                fildset_valid = true;
                fild_reading_allwd = false;
            } else {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseKey
***********************************/
vlg::RetCode VLG_COMP_ParseKey(unsigned long &lnum,
                               vlg::ascii_string_tok &tknz,
                               vlg::hash_map &definemap,
                               vlg::hash_map &entitymap,
                               vlg::hash_map &mmbrmap,
                               vlg::hash_map &keymap)
{
    vlg::ascii_string tkn;
    unsigned int k_id = 0;
    bool primary = false, mmbrset_read = false, id_read = false;
    vlg::linked_list mmbrset(vlg::sngl_ptr_obj_mng());
    COMMAND_IF_NOT_OK(mmbrset.init(), exit(1))
    //check if primary
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; break)
        if(tkn == VLG_RWRD_PRIMARY) {
            primary = true;
            break;
        } else {
            //unexpected token
            VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
            return vlg::RetCode_KO;
        }
    }
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_TK_COMA, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; continue)
        if(tkn == VLG_RWRD_PFX VLG_RWRD_ID) {
            //parse @id
            //@fixme if id already read...
            RETURN_IF_NOT_OK(VLG_COMP_ParseId(lnum,
                                              tknz,
                                              definemap,
                                              k_id))
            id_read = true;
        } else if(tkn == VLG_RWRD_PFX VLG_RWRD_FILDSET) {
            //parse @fildset
            //@fixme if fildset already read...
            RETURN_IF_NOT_OK(VLG_COMP_ParseFildSet(lnum,
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
            VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
            return vlg::RetCode_KO;
        }
    }
    //at this point we can create a VLG_KEY_DESC for this key
    key_desc_comp *keydesc = NULL;
    COMMAND_IF_NULL(keydesc = new key_desc_comp((unsigned short)k_id,
                                                primary), exit(1))
    COMMAND_IF_NOT_OK(keydesc->init(&mmbrset), exit(1))
    COMMAND_IF_NOT_OK(keymap.put(&k_id, &keydesc), exit(1))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseEnum
***********************************/
vlg::RetCode VLG_COMP_ParseEnum(unsigned long &lnum,
                                vlg::ascii_string_tok &tknz,
                                vlg::hash_map &definemap,
                                vlg::hash_map &entitymap)
{
    vlg::ascii_string tkn, symb_name;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_CB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        RETURN_IF_NOT_OK(VLG_COMP_CheckSymbol(lnum,
                                              tkn,
                                              &definemap,
                                              &entitymap,
                                              NULL,
                                              VLG_COMP_RSRV_WORD,
                                              VLG_COMP_SYMB_NAME)
                        )
        RETURN_IF_NOT_OK(symb_name.assign(tkn))
        // ok we got symb name.
        break;
    }

    RETURN_IF_NOT_OK(VLG_COMP_ReadOpeningCurlyBrace(lnum, tknz))

    long last_enum_val = -1;             //last value of enum
    vlg::hash_map mmbrmap(vlg::sngl_ptr_obj_mng(),
                          vlg::sngl_cstr_obj_mng());  //map symb -> mmbrdesc
    COMMAND_IF_NOT_OK(mmbrmap.init(HM_SIZE_NORM), exit(1))
    unsigned short mmbrid = 0;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_CB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; continue)
        CR_BREAK_ON_TKN(tkn, CR_TK_CBR)

        RETURN_IF_NOT_OK(VLG_COMP_ParseVal(lnum,
                                           mmbrid,
                                           tkn,
                                           last_enum_val,
                                           tknz,
                                           definemap,
                                           entitymap,
                                           mmbrmap))
    }
    uint32_t field_num = 0;
    mmbrmap.size(field_num);
    //at this point we can create a VLG_ENTITY_DESC_COMP for this ENUM
    entity_desc_comp *entitydesc = NULL;
    COMMAND_IF_NULL(entitydesc = new entity_desc_comp(0,  //0 for enum
                                                      EntityType_ENUM,
                                                      unit_nmspace.new_buffer(),
                                                      symb_name.new_buffer(),
                                                      0, //alloc f
                                                      field_num, //field num
                                                      false),exit(1))
    COMMAND_IF_NOT_OK(entitydesc->init(), exit(1))
    //-----------------------------
    // VLG_COMP_ARCH: x86_64
    // VLG_COMP_OS: win
    // VLG_COMP_LANG: CPP
    // VLG_COMP_TCOMP: MSVC
    //-----------------------------
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
    //-----------------------------
    // VLG_COMP_ARCH: x86_64
    // VLG_COMP_OS: unix
    // VLG_COMP_LANG: CPP
    // VLG_COMP_TCOMP: gcc
    //-----------------------------
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
    COMMAND_IF_NOT_OK(entitydesc->extend(&mmbrmap, NULL), exit(1))
    COMMAND_IF_NOT_OK(entitymap.put(entitydesc->get_entity_name(), &entitydesc),
                      exit(1))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseClass
***********************************/
vlg::RetCode VLG_COMP_ParseEntity(unsigned long &lnum,
                                  vlg::ascii_string_tok &tknz,
                                  vlg::hash_map &definemap,
                                  vlg::hash_map &entitymap)
{
    bool id_decl = false;
    vlg::ascii_string tkn, symb_name;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_CB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        RETURN_IF_NOT_OK(VLG_COMP_CheckSymbol(lnum,
                                              tkn,
                                              &definemap,
                                              &entitymap,
                                              NULL,
                                              VLG_COMP_RSRV_WORD,
                                              VLG_COMP_SYMB_NAME)
                        )
        RETURN_IF_NOT_OK(symb_name.assign(tkn))
        // ok we got symb name.
        break;
    }

    RETURN_IF_NOT_OK(VLG_COMP_ReadOpeningCurlyBrace(lnum, tknz))

    vlg::hash_map mmbrmap(vlg::sngl_ptr_obj_mng(),
                          vlg::sngl_cstr_obj_mng());  //map symb -> mmbrdesc
    vlg::hash_map keymap(vlg::sngl_ptr_obj_mng(),
                         sizeof(unsigned short));  //keyid -> keydesc
    COMMAND_IF_NOT_OK(mmbrmap.init(HM_SIZE_NORM),exit(1))
    COMMAND_IF_NOT_OK(keymap.init(HM_SIZE_NORM),exit(1))
    unsigned int   nclass_id = 0;
    unsigned short mmbrid = NCLASS_DERREP_STRT_MMRID;
    bool persistent = false;
    while(!tknz.next_token(tkn, CR_DF_DLMT CR_CB_DLMT CR_RB_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; continue)
        if(tkn == VLG_RWRD_PFX VLG_RWRD_ID) {
            //parse @id
            if(!id_decl) {
                RETURN_IF_NOT_OK(VLG_COMP_ParseId(lnum,
                                                  tknz,
                                                  definemap,
                                                  nclass_id))
                id_decl = true;
            } else {
                VLG_COMP_PARSE_CLASS_ID_ALRDY_DECL(lnum, symb_name.internal_buff());
                return vlg::RetCode_KO;
            }
        } else if(tkn == VLG_RWRD_PFX VLG_RWRD_PERSISTENT) {
            //parse @persistent
            if(persistent) {
                VLG_COMP_PARSE_CLASS_ALRDY_PERSISTENT(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            }
            persistent = true;
        } else if(tkn == VLG_RWRD_PFX VLG_RWRD_KEY) {
            //parse @key
            if(!persistent) {
                VLG_COMP_PARSE_CLASS_NOT_PERSISTENT(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            }
            RETURN_IF_NOT_OK(VLG_COMP_ParseKey(lnum,
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
            RETURN_IF_NOT_OK(VLG_COMP_ParseFild(lnum,
                                                mmbrid,
                                                tkn,
                                                tknz,
                                                definemap,
                                                entitymap,
                                                mmbrmap))

        }
    }

    if(!id_decl) {
        VLG_COMP_PARSE_CLASS_ID_NOT_DECL(lnum, symb_name.internal_buff());
        return vlg::RetCode_KO;
    }

    uint32_t field_num = 0;
    mmbrmap.size(field_num);
    //at this point we can create a VLG_ENTITY_DESC_COMP for this Class
    entity_desc_comp *entitydesc = NULL;
    COMMAND_IF_NULL(entitydesc = new entity_desc_comp(nclass_id,
                                                      EntityType_NCLASS,
                                                      unit_nmspace.new_buffer(),
                                                      symb_name.new_buffer(),
                                                      0, //alloc f
                                                      field_num, //field num
                                                      persistent),exit(1))
    COMMAND_IF_NOT_OK(entitydesc->init(), exit(1))
    size_t max_align = 1;
    size_t field_offset = NCLASS_DERREP_STRT_OFFST_x86_64;
    //-----------------------------
    // VLG_COMP_ARCH: x86_64
    // VLG_COMP_OS: win
    // VLG_COMP_LANG: CPP
    // VLG_COMP_TCOMP: MSVC
    //-----------------------------
    RETURN_IF_NOT_OK(VLG_COMP_CalcTCompDependantValues(max_align,
                                                       field_offset,
                                                       *entitydesc,
                                                       entitymap,
                                                       mmbrmap,
                                                       VLG_COMP_ARCH_x86_64,
                                                       VLG_COMP_OS_win,
                                                       VLG_COMP_LANG_CPP,
                                                       VLG_COMP_TCOMP_MSVC))
    //-----------------------------
    // VLG_COMP_ARCH: x86_64
    // VLG_COMP_OS: unix
    // VLG_COMP_LANG: CPP
    // VLG_COMP_TCOMP: gcc
    //-----------------------------
    max_align = 1;
    field_offset = NCLASS_DERREP_STRT_OFFST_x86_64;
    RETURN_IF_NOT_OK(VLG_COMP_CalcTCompDependantValues(max_align,
                                                       field_offset,
                                                       *entitydesc,
                                                       entitymap,
                                                       mmbrmap,
                                                       VLG_COMP_ARCH_x86_64,
                                                       VLG_COMP_OS_unix,
                                                       VLG_COMP_LANG_CPP,
                                                       VLG_COMP_TCOMP_GCC))
    COMMAND_IF_NOT_OK(entitydesc->extend(&mmbrmap, &keymap),exit(1))
    COMMAND_IF_NOT_OK(entitymap.put(entitydesc->get_entity_name(), &entitydesc),
                      exit(1))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseDefine
***********************************/
vlg::RetCode VLG_COMP_ParseDefine(unsigned long &lnum,
                                  vlg::ascii_string_tok &tknz,
                                  vlg::hash_map &definemap,
                                  vlg::hash_map &entitymap)
{
    vlg::ascii_string tkn, define_name, define_val;
    //@fixme add separators and special chars
    while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        RETURN_IF_NOT_OK(VLG_COMP_CheckSymbol(lnum,
                                              tkn,
                                              &definemap,
                                              &entitymap,
                                              NULL,
                                              VLG_COMP_RSRV_WORD,
                                              VLG_COMP_SYMB_NAME)
                        )
        RETURN_IF_NOT_OK(define_name.assign(tkn))
        // ok we got define name.
        break;
    }
    //@fixme add separators and special chars
    while(!tknz.next_token(tkn, CR_DF_DLMT, true)) {
        CR_SKIP_SP_TABS(tkn)
        //we expect a define name, so we return with error if newline found.
        CR_DO_CMD_ON_NEWLINE(tkn, VLG_COMP_PARSE_EXP(lnum, VLG_COMP_SYMB_NAME);
                             return vlg::RetCode_KO)
        RETURN_IF_NOT_OK(define_val.assign(tkn))
        // ok we got define val.
        break;
    }
    COMMAND_IF_NOT_OK(definemap.put(define_name.internal_buff(),
                                    define_val.internal_buff()),exit(1))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseInclude
***********************************/

vlg::RetCode VLG_COMP_ParseInclude(unsigned long &lnum,
                                   vlg::ascii_string_tok &tknz,
                                   vlg::hash_map &definemap,
                                   vlg::hash_map &entitymap,
                                   char **modname,
                                   char **modver)
{
    char *incl_fname = NULL;
    RETURN_IF_NOT_OK(VLG_COMP_ReadString(lnum, tknz, &incl_fname))
    FILE *fdesc = NULL;
    vlg::ascii_string data; //file content loaded on data
    RETURN_IF_NOT_OK(open_input_file(incl_fname, &fdesc))
    RETURN_IF_NOT_OK(load_file(fdesc, data))
    printf(STG_FMT_0,
           VLG_COMP_INF_START,
           VLG_COMP_INF_PARS_FILE,
           incl_fname);
    RETURN_IF_NOT_OK(parse_data(incl_fname,
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
vlg::RetCode VLG_COMP_ParseNamespace(unsigned long &lnum,
                                     vlg::ascii_string_tok &tknz,
                                     vlg::hash_map &definemap,
                                     vlg::hash_map &entitymap)
{
    char *n_space = NULL;
    RETURN_IF_NOT_OK(VLG_COMP_ReadString(lnum, tknz, &n_space))
    RETURN_IF_NOT_OK(unit_nmspace.assign(n_space))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseNamespace_end
***********************************/
vlg::RetCode VLG_COMP_ParseNamespace_end(unsigned long &lnum,
                                         vlg::ascii_string_tok &tknz,
                                         vlg::hash_map &definemap,
                                         vlg::hash_map &entitymap)
{
    RETURN_IF_NOT_OK(unit_nmspace.assign(""))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseModelName
***********************************/
vlg::RetCode VLG_COMP_ParseModelName(unsigned long &lnum,
                                     vlg::ascii_string_tok &tknz,
                                     vlg::hash_map &definemap,
                                     vlg::hash_map &entitymap,
                                     char **modname)
{
    RETURN_IF_NOT_OK(VLG_COMP_ReadString(lnum, tknz, modname))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseModelName
***********************************/
vlg::RetCode VLG_COMP_ParseModelVer(unsigned long &lnum,
                                    vlg::ascii_string_tok &tknz,
                                    vlg::hash_map &definemap,
                                    vlg::hash_map &entitymap,
                                    char **modver)
{
    RETURN_IF_NOT_OK(VLG_COMP_ReadString(lnum, tknz, modver))
    return vlg::RetCode_OK;
}

/***********************************
PARSE- VLG_COMP_ParseData
***********************************/
vlg::RetCode parse_data(const char *fname,
                        vlg::ascii_string &data,
                        vlg::hash_map &definemap,
                        vlg::hash_map &entitymap,
                        char **modname,
                        char **modver)
{
    unsigned long lnum = 1;
    bool parsing_comment = false;
    vlg::ascii_string tkn;
    vlg::ascii_string_tok tknz;
    RETURN_IF_NOT_OK(tknz.init(data))
    RETURN_IF_NOT_OK(unit_nmspace.assign(""))
    while(!tknz.next_token(tkn, CR_DF_DLMT VLG_TK_COMMENT, true)) {
        CR_SKIP_SP_TABS(tkn)
        CR_DO_CMD_ON_NEWLINE(tkn, lnum++; parsing_comment = false;
                             continue)
        if(!parsing_comment) {
            if(tkn == VLG_RWRD_PFX VLG_RWRD_INCLUDE) {
                //parse @include
                RETURN_IF_NOT_OK(VLG_COMP_ParseInclude(lnum,
                                                       tknz,
                                                       definemap,
                                                       entitymap,
                                                       modname,
                                                       modver))
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_MODLNAME) {
                //parse @model_name
                if(*modname) {
                    VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                    return vlg::RetCode_KO;
                } else {
                    RETURN_IF_NOT_OK(VLG_COMP_ParseModelName(lnum,
                                                             tknz,
                                                             definemap,
                                                             entitymap,
                                                             modname))
                }
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_MODLVER) {
                //parse @model_version
                if(*modver) {
                    VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                    return vlg::RetCode_KO;
                } else {
                    RETURN_IF_NOT_OK(VLG_COMP_ParseModelVer(lnum,
                                                            tknz,
                                                            definemap,
                                                            entitymap,
                                                            modver))
                }
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_DEFINE) {
                //parse @define
                RETURN_IF_NOT_OK(VLG_COMP_ParseDefine(lnum,
                                                      tknz,
                                                      definemap,
                                                      entitymap))
            } else if(tkn == VLG_RWRD_ENUM) {
                //parse @enum
                RETURN_IF_NOT_OK(VLG_COMP_ParseEnum(lnum,
                                                    tknz,
                                                    definemap,
                                                    entitymap))
            } else if(tkn == VLG_RWRD_NCLASS) {
                //parse @class
                RETURN_IF_NOT_OK(VLG_COMP_ParseEntity(lnum,
                                                      tknz,
                                                      definemap,
                                                      entitymap))
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_NAMESPACE) {
                //parse @namespace
                RETURN_IF_NOT_OK(VLG_COMP_ParseNamespace(lnum,
                                                         tknz,
                                                         definemap,
                                                         entitymap))
            } else if(tkn == VLG_RWRD_PFX VLG_RWRD_NAMESPACE_END) {
                //parse @namespace_end
                RETURN_IF_NOT_OK(VLG_COMP_ParseNamespace_end(lnum,
                                                             tknz,
                                                             definemap,
                                                             entitymap))
            } else if(tkn == VLG_TK_COMMENT) {
                //comment begin
                parsing_comment = true;
            } else {
                //unexpected token
                VLG_COMP_PARSE_UNEXP(lnum, tkn.internal_buff());
                return vlg::RetCode_KO;
            }
        }
    }
    return vlg::RetCode_OK;
}

}
