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

#ifndef BLZ_COMPILER_H_
#define BLZ_COMPILER_H_
#include "blz_glob_int.h"

//VPTR SIZE x86_64
#define NCLASS_VPTR_SIZE_x86_64    8

//starting offset for nclass derived classes x86_64.
#define NCLASS_DERREP_STRT_OFFST_x86_64   (NCLASS_VPTR_SIZE_x86_64)

//starting usable memberid for nclass derived classes.
#define NCLASS_DERREP_STRT_MMRID   1

//enum length [byte size] assumption 
#define ENUM_B_SZ 4 

#define TCOMP_DEP_GEN_KEY(arch, os, lang, tcomp) ((arch*1000000)+(os*10000)+(lang*100)+tcomp)

#define EXIT_ACTION_STDOUT(msg)\
{\
    fprintf(stdout, "exit message: %s\n", msg);\
    exit(1);\
}

/***********************************
RECOGNIZED TOKENS
***********************************/
#define BLZ_RWRD_PFX                "@"

/***********************************
RECOGNIZED RES. WORDS
***********************************/
#define BLZ_RWRD_INCLUDE            "include"
#define BLZ_RWRD_DEFINE             "define"
#define BLZ_RWRD_ENUM               "enum"
#define BLZ_RWRD_NCLASS             "nclass"
#define BLZ_RWRD_ID                 "id"
#define BLZ_RWRD_PRIMARY            "primary"
#define BLZ_RWRD_PERSISTENT         "persistent"
#define BLZ_RWRD_KEY                "key"
#define BLZ_RWRD_KEY_END            "key_end"
#define BLZ_RWRD_FILDSET            "fldset"
#define BLZ_RWRD_NAMESPACE          "namespace"
#define BLZ_RWRD_NAMESPACE_END      "namespace_end"
#define BLZ_RWRD_MODLNAME           "model_name"
#define BLZ_RWRD_MODLVER            "model_version"

/***********************************
RECOGNIZED TYPES
***********************************/
#define BLZ_RWORD_T_BOOL            "bool"
#define BLZ_RWORD_T_INT_16          "int_16"
#define BLZ_RWORD_T_UINT_16         "uint_16"
#define BLZ_RWORD_T_INT_32          "int_32"
#define BLZ_RWORD_T_UINT_32         "uint_32"
#define BLZ_RWORD_T_INT_64          "int_64"
#define BLZ_RWORD_T_UINT_64         "uint_64"
#define BLZ_RWORD_T_FLOAT_32        "float_32"
#define BLZ_RWORD_T_FLOAT_64        "float_64"
#define BLZ_RWORD_T_ASCII           "ascii"

/***********************************
COMPILER STRINGS
***********************************/
#define BLZ_COMP_SYMB_NAME               "symbol"
#define BLZ_COMP_RSRV_WORD               "reserved word"
#define BLZ_COMP_KSYMB_NAME              "key-symbol"
#define BLZ_COMP_TYPE                    "type"
#define BLZ_COMP_STRING                  "string"

/***********************************
GLOBAL DEFINES
***********************************/
#define BLZ_COMP_START_PAR_VRBLVL       "verblevel"
#define BLZ_COMP_START_PAR_INCLUDE      "I"
#define BLZ_COMP_START_PAR_OUTPT_DIR    "O"
#define BLZ_COMP_START_PAR_FILES        "F"
#define BLZ_COMP_START_PAR_ARCH         "arch"
#define BLZ_COMP_START_PAR_LANG         "lang"
#define BLZ_COMP_START_PAR_OS           "os"
#define BLZ_COMP_START_PAR_TCOMP        "tcomp"
#define BLZ_COMP_START_PAR_ALIGN        "align"
#define BLZ_COMP_START_PAR_PACKING      "packing"

#define BLZ_COMP_ARCH_TK_X86_64        "x86_64"

#define BLZ_COMP_OS_TK_WIN              "win"
#define BLZ_COMP_OS_TK_UNIX             "unix"

#define BLZ_COMP_LANG_TK_C             "C"
#define BLZ_COMP_LANG_TK_CPP           "CPP"
#define BLZ_COMP_LANG_TK_JAVA          "JAVA"
#define BLZ_COMP_LANG_TK_OBJC          "OBJC"
#define BLZ_COMP_LANG_TK_SWIFT         "SWIFT"

#define BLZ_COMP_TCOMP_TK_MSVC          "msvc"
#define BLZ_COMP_TCOMP_TK_GCC           "gcc"

#define BLZ_COMP_DFLT_DIR           "."
#define BLZ_COMP_DFLT_FILE_SEP      "\\"
#define BLZ_COMP_DOT                "."

#define OFFSET_ZERO 0x0

#define BLZ_COMP_INF_START              ">>"
#define BLZ_COMP_INF_END                "<<"
#define BLZ_COMP_INF_PARS_FILE          "parse"
#define BLZ_COMP_INF_COMPL_FILE         "compile"
#define BLZ_COMP_INF_JOB_DONE           "compile done"

/***********************************
CPP get/set prfx
***********************************/
#define BLZ_COMP_CPP_GETTER_PFX         "get"
#define BLZ_COMP_CPP_SETTER_PFX         "set"

/***********************************
CPP EXP_SYM
***********************************/
#define EXPORT_SYMBOL "EXP_SYM "

#define EXPORT_SYMBOL_DECL                                  \
"#ifdef WIN32\n"                                            \
"#ifdef BLZ_DMODEL_EXPORTS\n"                               \
"#define EXP_SYM __declspec(dllexport)\n"                   \
"#else\n"                                                   \
"#ifdef BLZ_DMODEL_IMPORTS\n"                               \
"#define EXP_SYM __declspec(dllimport)\n"                   \
"#else\n"                                                   \
"#define EXP_SYM\n"                                         \
"#endif\n"                                                  \
"#endif\n"                                                  \
"#else\n"                                                   \
"#ifdef __linux\n"                                          \
"#define EXP_SYM\n"                                         \
"#endif\n"                                                  \
"#if defined (__MACH__) || defined (__APPLE__)\n"           \
"#define EXP_SYM __attribute__((visibility(\"default\")))\n"\
"#endif\n"                                                  \
"#endif\n"

/***********************************
CPP_ONLY_BEGIN
***********************************/
#define CPP_ONLY_BEGIN                                      \
"#if defined(__cplusplus)\n"

#define OPN_CMMNT_LN "/*-----------------------------------------------------------------------------\n"
#define CLS_CMMNT_LN "-----------------------------------------------------------------------------*/\n"

#define STG_FMT_0 "@%-10s %s: %s\n"
#define COMPILE_FAIL  " - compile [FAIL]\n"

/***********************************
GLOB DEFS
***********************************/
namespace vlg {

vlg::RetCode str_from_EntityType(EntityType etype,
                                    vlg::ascii_string &out);

enum BLZ_COMP_ARCH {
    BLZ_COMP_ARCH_Undef,
    BLZ_COMP_ARCH_x86_64,
};

BLZ_COMP_ARCH   arch_from_str(const char *str);
vlg::RetCode  str_from_arch(BLZ_COMP_ARCH arch, vlg::ascii_string &out);

enum BLZ_COMP_OS {
    BLZ_COMP_OS_Undef,
    BLZ_COMP_OS_win,
    BLZ_COMP_OS_unix,
};

enum BLZ_COMP_LANG {
    BLZ_COMP_LANG_Undef,
    BLZ_COMP_LANG_CPP,
    BLZ_COMP_LANG_C,
    BLZ_COMP_LANG_JAVA,
    BLZ_COMP_LANG_OBJC,
    BLZ_COMP_LANG_SWIFT,
};

BLZ_COMP_LANG   lang_from_str(const char *str);
vlg::RetCode  str_from_lang(BLZ_COMP_LANG lang, vlg::ascii_string &out);

enum BLZ_COMP_TCOMP {
    BLZ_COMP_TCOMP_Undef,
    BLZ_COMP_TCOMP_MSVC,
    BLZ_COMP_TCOMP_GCC,
};

BLZ_COMP_TCOMP  tcomp_from_str(const char *str);
vlg::RetCode  str_from_tcomp(BLZ_COMP_TCOMP tcomp, vlg::ascii_string &out);

/***********************************
compiler_config
***********************************/
struct compiler_config {
    compiler_config();
    vlg::RetCode init();
    int verblvl;
    BLZ_COMP_LANG lang;
    vlg::linked_list path_list;
    vlg::linked_list file_list;
    const char *out_dir;
};

extern compiler_config comp_cfg;

/***********************************
compile_unit
***********************************/
class compile_unit {
    public:
        compile_unit();
        ~compile_unit();

        vlg::RetCode init(const char *fname);

        vlg::RetCode parse();
        vlg::RetCode compile();

        const char *get_file_name();
        const char *model_name() const;
        const char *model_version() const;

        vlg::hash_map &get_define_map();
        vlg::hash_map &get_entity_map();

    private:
        char *fname_;
        char *model_name_;
        char *model_version_;
        vlg::hash_map define_map_;
        vlg::hash_map entity_map_;
};

/***********************************
member_desc_comp
***********************************/
struct member_desc_comp {
    //---ctors
    member_desc_comp(unsigned short mmbrid,
                     MemberType mmbr_type,
                     const char *mmbr_name,
                     const char *mmbr_desc,
                     Type fild_type,
                     size_t nmemb,
                     unsigned int fild_entityid,
                     const char *fild_usr_str_type,
                     EntityType fild_entitytype,
                     long enum_value);

    //---meths
    vlg::RetCode  init();
    /*
    Member section
    */
    unsigned short  get_member_id()         const;
    MemberType      get_member_type()       const;
    const char      *get_member_name()      const;
    const char      *get_member_desc()      const;

    /*
    Field section
    */
    Type            get_field_type()        const;

    size_t          get_field_offset(BLZ_COMP_ARCH arch,
                                         BLZ_COMP_OS os,
                                         BLZ_COMP_LANG lang,
                                         BLZ_COMP_TCOMP tcomp)  const;

    size_t          get_field_type_size(BLZ_COMP_ARCH arch,
                                            BLZ_COMP_OS os,
                                            BLZ_COMP_LANG lang,
                                            BLZ_COMP_TCOMP tcomp)   const;

    size_t          get_nmemb()                 const;
    unsigned int    get_field_nclassid()         const;
    const char      *get_field_usr_str_type()   const;
    EntityType      get_field_entity_type()     const;


    void                set_field_offset(size_t val,
                                         BLZ_COMP_ARCH arch,
                                         BLZ_COMP_OS os,
                                         BLZ_COMP_LANG lang,
                                         BLZ_COMP_TCOMP tcomp);

    void                set_field_type_size(size_t val,
                                            BLZ_COMP_ARCH arch,
                                            BLZ_COMP_OS os,
                                            BLZ_COMP_LANG lang,
                                            BLZ_COMP_TCOMP tcomp);

    //Enum specific
    long                get_enum_value()          const;

    
    unsigned short  mmbrid_;
    MemberType      mmbr_type_;
    const char      *mmbr_name_;
    const char      *mmbr_desc_;

    //field type when applicable
    Type            fild_type_;

    vlg::hash_map fild_offset_map_;
    vlg::hash_map fild_type_size_map_;

    //1 for single element
    //N for arrays, as calloc()
    size_t          nmemb_;

    // valid only if fild_type
    unsigned int    fild_nclassid_;

    // is set to BLZ_TYPE_Entity &&
    // fild_entitytype_ == BLZ_ENTITY_TYPE_Class
    // equals to entityname when
    const char      *fild_usr_str_type_;

    // fild_type_ == BLZ_TYPE_Entity
    // is set to BLZ_TYPE_Entity
    // valid only if fild_type
    // is set to BLZ_TYPE_Entity
    EntityType      fild_entitytype_;

    //enum specific
    //value assumed by this enum
    long            enum_value_;
};

/***********************************
entity_desc_comp
***********************************/
class key_desc_comp {
public:
    //---ctors
    explicit key_desc_comp(unsigned short keyid,
        bool primary);

    ~key_desc_comp();

public:
    //---meths
    vlg::RetCode      init();
    vlg::RetCode      init(vlg::linked_list *member_set);
    vlg::RetCode      add_member_desc(const member_desc_comp *member_descriptor);

    unsigned short      get_key_id() const;
    bool                is_primary() const;
    const vlg::linked_list   &get_key_member_set() const;
    vlg::linked_list         &get_key_member_set_m();

    
private:
    unsigned short      keyid_;
    bool                primary_;
    vlg::linked_list  fildset_;   //members being part of this key.
};

/***********************************
entity_desc_comp
***********************************/
typedef void (*enum_member_desc_comp_func)(const member_desc_comp  &desc);

struct entity_desc_comp {
        //---ctors
        entity_desc_comp(unsigned int entityid,
                         EntityType entitytype,
                         const char *nmspace,
                         const char *entityname,
                         vlg::alloc_func afun,
                         unsigned int fild_num,
                         bool persistent);
        //---meths
        vlg::RetCode  init();
        vlg::RetCode  extend(vlg::hash_map *mmbrmap,
                                             vlg::hash_map *keymap);
        vlg::RetCode  add_key_desc(const key_desc_comp *keydesc);

        unsigned int    get_entityid()  const;

        size_t          get_entity_size(BLZ_COMP_ARCH arch,
                                                BLZ_COMP_OS os,
                                                BLZ_COMP_LANG lang,
                                                BLZ_COMP_TCOMP tcomp)  const;

        size_t          get_entity_max_align(BLZ_COMP_ARCH arch,
                                                     BLZ_COMP_OS os,
                                                     BLZ_COMP_LANG lang,
                                                     BLZ_COMP_TCOMP tcomp)  const;

        EntityType          get_entity_type()       const;
        const char          *get_entity_namespace() const;
        const char          *get_entity_name()      const;
        vlg::alloc_func   get_entity_alloc_f()    const;
        unsigned int        get_field_num()         const;
        bool                is_persistent()         const;

        vlg::hash_map         &get_map_id_MMBRDSC();
        const vlg::hash_map   &get_map_name_MMBRDSC() const;
        const vlg::hash_map   &get_map_keyid_KDESC()  const;


        vlg::hash_map         &get_map_keyid_KDESC_mod();

        const member_desc_comp  *get_member_desc_by_id(unsigned int mmbrid) const;
        const member_desc_comp  *get_member_desc_by_name(const char *name)  const;

        void                enum_member_desc(enum_member_desc_comp_func func)
        const;

        void                    set_entity_size(size_t val,
                                                BLZ_COMP_ARCH arch,
                                                BLZ_COMP_OS os,
                                                BLZ_COMP_LANG lang,
                                                BLZ_COMP_TCOMP tcomp);

        void                    set_entity_max_align(size_t val,
                                                     BLZ_COMP_ARCH arch,
                                                     BLZ_COMP_OS os,
                                                     BLZ_COMP_LANG lang,
                                                     BLZ_COMP_TCOMP tcomp);
        
    public:
        unsigned int        entityid_;

        vlg::hash_map     entity_size_map_;
        vlg::hash_map     entity_max_align_map_;

        EntityType          entitytype_;
        const char          *nmspace_;
        const char          *entityname_;
        vlg::alloc_func   afun_;
        unsigned int        fild_num_;
        vlg::hash_map     mmbrid_mdesc_;  //mmbrid --> mmbrdesc
        vlg::hash_map     mmbrnm_mdesc_;  //mmbrname --> mmbrdesc

        //persistence
        bool                persistent_;
        vlg::hash_map     keyid_kdesc_;  //keyid --> keydesc
};

/***********************************
GLOB FUNCTIONS
***********************************/
vlg::RetCode    open_input_file(const char *fname,
                              FILE **fdesc);

vlg::RetCode    open_output_file(const char *fname,
                               FILE **fdesc);

vlg::RetCode    parse_data(const char *fname,
                         vlg::ascii_string &data,
                         vlg::hash_map &definemap,
                         vlg::hash_map &entitymap,
                         char **modname,
                         char **modver);

/*
Returns the potential next valid offset (to be used in next iteration).
Set in cur_offset the offset that must be used for the current field.
*/
size_t  get_next_valid_offset(size_t &cur_offset,
                              size_t type_align,
                              size_t type_size,
                              size_t nmemb,
                              size_t max_align,
                              size_t packing,
                              bool scalar);

/*
Return the size of the field adjusted to align/paking value.
cur_offset must contain last value returned by get_next_valid_offset.
*/
size_t  adjust_entity_size(size_t cur_offset,
                           size_t max_align,
                           BLZ_COMP_ARCH arch,
                           BLZ_COMP_OS os,
                           BLZ_COMP_LANG lang,
                           BLZ_COMP_TCOMP tcomp);

/***********************************
GLOB FUNCTIONS / COMPILING
***********************************/

/*
On machine-architecture dependant type size (in bytes)
*/
size_t  get_in_arch_type_size(Type type,
                              BLZ_COMP_ARCH arch,
                              BLZ_COMP_OS os,
                              BLZ_COMP_LANG lang,
                              BLZ_COMP_TCOMP tcomp);

/*
On Network-protocol dependant type size (in bytes)
*/
size_t  get_network_type_size(Type type);

vlg::RetCode    compile_C(compile_unit &cunit);
vlg::RetCode    compile_CPP(compile_unit &cunit);
vlg::RetCode    compile_Java(compile_unit &cunit);
vlg::RetCode    compile_ObjC(compile_unit &cunit);

vlg::RetCode    get_zero_val_for_BLZ_TYPE(Type type, vlg::ascii_string &out);

vlg::RetCode    target_type_from_builtin_BLZ_TYPE(member_desc_comp &mdsc,
                                                vlg::ascii_string &out);

vlg::RetCode    target_type_from_BLZ_TYPE(member_desc_comp &mdsc,
                                        vlg::hash_map &entitymap,
                                        vlg::ascii_string &out);

vlg::RetCode    printf_percent_from_BLZ_TYPE(member_desc_comp &mdsc,
                                           vlg::ascii_string &out,
                                           bool strict_linux = false);

vlg::RetCode get_local_date(char *out);
vlg::RetCode put_newline(FILE *file);

vlg::RetCode render_hdr(compile_unit &cunit,
                      vlg::ascii_string &fname,
                      FILE *file);

}
#endif