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

#pragma once
#include "glob.h"

//VPTR SIZE x86_64
#define NCLASS_VPTR_SIZE_x86_64    8

//starting offset for nclass derived classes x86_64.
#define NCLASS_DERREP_STRT_OFFST_x86_64   (NCLASS_VPTR_SIZE_x86_64)

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
#define VLG_RWRD_PFX                "@"

/***********************************
RECOGNIZED RES. WORDS
***********************************/
#define VLG_RWRD_INCLUDE            "include"
#define VLG_RWRD_DEFINE             "define"
#define VLG_RWRD_ENUM               "enum"
#define VLG_RWRD_NCLASS             "nclass"
#define VLG_RWRD_ID                 "id"
#define VLG_RWRD_PRIMARY            "primary"
#define VLG_RWRD_PERSISTENT         "persistent"
#define VLG_RWRD_KEY                "key"
#define VLG_RWRD_KEY_END            "key_end"
#define VLG_RWRD_FILDSET            "fldset"
#define VLG_RWRD_NAMESPACE          "namespace"
#define VLG_RWRD_NAMESPACE_END      "namespace_end"
#define VLG_RWRD_MODLNAME           "model_name"
#define VLG_RWRD_MODLVER            "model_version"

/***********************************
RECOGNIZED TYPES
***********************************/
#define VLG_RWORD_T_BOOL            "bool"
#define VLG_RWORD_T_INT_16          "int_16"
#define VLG_RWORD_T_UINT_16         "uint_16"
#define VLG_RWORD_T_INT_32          "int_32"
#define VLG_RWORD_T_UINT_32         "uint_32"
#define VLG_RWORD_T_INT_64          "int_64"
#define VLG_RWORD_T_UINT_64         "uint_64"
#define VLG_RWORD_T_FLOAT_32        "float_32"
#define VLG_RWORD_T_FLOAT_64        "float_64"
#define VLG_RWORD_T_ASCII           "ascii"
#define VLG_RWORD_T_BYTE            "byte"

/***********************************
COMPILER STRINGS
***********************************/
#define VLG_COMP_SYMB_NAME               "symbol"
#define VLG_COMP_RSRV_WORD               "reserved word"
#define VLG_COMP_KSYMB_NAME              "key-symbol"
#define VLG_COMP_TYPE                    "type"
#define VLG_COMP_STRING                  "string"

/***********************************
GLOBAL DEFINES
***********************************/
#define VLG_COMP_START_PAR_VRBLVL       "verblevel"
#define VLG_COMP_START_PAR_INCLUDE      "I"
#define VLG_COMP_START_PAR_OUTPT_DIR    "O"
#define VLG_COMP_START_PAR_FILES        "F"
#define VLG_COMP_START_PAR_ARCH         "arch"
#define VLG_COMP_START_PAR_LANG         "lang"
#define VLG_COMP_START_PAR_OS           "os"
#define VLG_COMP_START_PAR_TCOMP        "tcomp"
#define VLG_COMP_START_PAR_ALIGN        "align"
#define VLG_COMP_START_PAR_PACKING      "packing"

#define VLG_COMP_ARCH_TK_X86_64        "x86_64"

#define VLG_COMP_OS_TK_WIN              "win"
#define VLG_COMP_OS_TK_UNIX             "unix"

#define VLG_COMP_LANG_TK_C             "C"
#define VLG_COMP_LANG_TK_CPP           "CPP"
#define VLG_COMP_LANG_TK_JAVA          "JAVA"
#define VLG_COMP_LANG_TK_OBJC          "OBJC"
#define VLG_COMP_LANG_TK_SWIFT         "SWIFT"

#define VLG_COMP_TCOMP_TK_MSVC          "msvc"
#define VLG_COMP_TCOMP_TK_GCC           "gcc"

#define VLG_COMP_DFLT_DIR           "."
#define VLG_COMP_DFLT_FILE_SEP      "\\"
#define VLG_COMP_DOT                "."

#define OFFSET_ZERO 0x0

#define VLG_COMP_INF_START              ">>"
#define VLG_COMP_INF_END                "<<"
#define VLG_COMP_INF_PARS_FILE          "parse"
#define VLG_COMP_INF_COMPL_FILE         "compile"
#define VLG_COMP_INF_JOB_DONE           "compile done"

/***********************************
CPP get/set prfx
***********************************/
#define VLG_COMP_CPP_GETTER_PFX         "get"
#define VLG_COMP_CPP_SETTER_PFX         "set"

/***********************************
CPP EXP_SYM
***********************************/
#define EXPORT_SYMBOL "EXP_SYM "

#define EXPORT_SYMBOL_DECL \
"#if defined WIN32 && defined _MSC_VER\n"\
"#ifdef VLG_DMODEL_EXPORTS\n"\
"#define EXP_SYM __declspec(dllexport)\n"\
"#else\n"\
"#ifdef VLG_DMODEL_IMPORTS\n"\
"#define EXP_SYM __declspec(dllimport)\n"\
"#else\n"\
"#define EXP_SYM\n"\
"#endif\n"\
"#endif\n"\
"#else\n"\
"#ifdef __linux\n"\
"#define EXP_SYM\n"\
"#endif\n"\
"#if defined (__MACH__) || defined (__APPLE__)\n"\
"#define EXP_SYM __attribute__((visibility(\"default\")))\n"\
"#endif\n"\
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

enum VLG_COMP_ARCH {
    VLG_COMP_ARCH_Undef,
    VLG_COMP_ARCH_x86_64,
};

enum VLG_COMP_OS {
    VLG_COMP_OS_Undef,
    VLG_COMP_OS_win,
    VLG_COMP_OS_unix,
};

enum VLG_COMP_LANG {
    VLG_COMP_LANG_Undef,
    VLG_COMP_LANG_CPP,
    VLG_COMP_LANG_C,
    VLG_COMP_LANG_JAVA,
    VLG_COMP_LANG_OBJC,
    VLG_COMP_LANG_SWIFT,
};

VLG_COMP_LANG   lang_from_str(const char *str);
RetCode  str_from_lang(VLG_COMP_LANG lang, std::string &out);

enum VLG_COMP_TCOMP {
    VLG_COMP_TCOMP_Undef,
    VLG_COMP_TCOMP_MSVC,
    VLG_COMP_TCOMP_GCC,
};

VLG_COMP_TCOMP  tcomp_from_str(const char *str);
RetCode  str_from_tcomp(VLG_COMP_TCOMP tcomp, std::string &out);

/***********************************
compiler_config
***********************************/
struct compiler_config {
    compiler_config();
    int verblvl;
    VLG_COMP_LANG lang;
    std::list<std::string> path_list;
    std::list<std::string> file_list;
    const char *out_dir;
};

extern compiler_config comp_cfg;

/***********************************
member_desc_comp
***********************************/
struct member_desc_comp {
    member_desc_comp(unsigned short mmbrid,
                     MemberType mmbr_type,
                     const char *mmbr_name,
                     const char *mmbr_desc,
                     Type fild_type,
                     size_t nmemb,
                     unsigned int fild_entityid,
                     const char *fild_usr_str_type,
                     NEntityType fild_entitytype,
                     long enum_value);
    /*
    Member section
    */
    unsigned short get_member_id() const;
    MemberType get_member_type() const;
    const char *get_member_name() const;
    const char *get_member_desc() const;

    /*
    Field section
    */
    Type get_field_type() const;

    size_t get_field_offset(VLG_COMP_ARCH arch,
                            VLG_COMP_OS os,
                            VLG_COMP_LANG lang,
                            VLG_COMP_TCOMP tcomp) const;

    size_t get_field_type_size(VLG_COMP_ARCH arch,
                               VLG_COMP_OS os,
                               VLG_COMP_LANG lang,
                               VLG_COMP_TCOMP tcomp) const;

    size_t get_nmemb() const;
    unsigned int get_field_nclassid() const;
    const char *get_field_usr_str_type() const;
    NEntityType get_field_entity_type() const;


    void set_field_offset(size_t,
                          VLG_COMP_ARCH arch,
                          VLG_COMP_OS os,
                          VLG_COMP_LANG lang,
                          VLG_COMP_TCOMP tcomp);

    void set_field_type_size(size_t,
                             VLG_COMP_ARCH arch,
                             VLG_COMP_OS os,
                             VLG_COMP_LANG lang,
                             VLG_COMP_TCOMP tcomp);

    //Enum specific
    long get_enum_value() const;


    unsigned short mmbrid_;
    MemberType mmbr_type_;
    const char *mmbr_name_;
    const char *mmbr_desc_;

    //field type when applicable
    Type fild_type_;

    std::map<unsigned int, size_t> fild_offset_map_;
    std::map<unsigned int, size_t> fild_type_size_map_;

    //1 for single element
    //N for arrays, as calloc()
    size_t nmemb_;

    // valid only if fild_type
    unsigned int fild_nclassid_;

    // is set to VLG_TYPE_Entity &&
    // fild_entitytype_ == VLG_ENTITY_TYPE_Class
    // equals to nclassname when
    const char *fild_usr_str_type_;

    // fild_type_ == VLG_TYPE_Entity
    // is set to VLG_TYPE_Entity
    // valid only if fild_type
    // is set to VLG_TYPE_Entity
    NEntityType fild_entitytype_;

    //enum specific
    //value assumed by this enum
    long enum_value_;
};

/***********************************
entity_desc_comp
***********************************/
class key_desc_comp {
    public:
        explicit key_desc_comp(unsigned short keyid,
                               bool primary);

    public:
        RetCode init(std::set<member_desc_comp *> &member_set);
        RetCode add_member_desc(member_desc_comp *const member_descriptor);

        unsigned short get_key_id() const;
        bool is_primary() const;
        const std::set<member_desc_comp *> &get_key_member_set() const;
        std::set<member_desc_comp *> &get_key_member_set_m();


    private:
        unsigned short keyid_;
        bool primary_;
        std::set<member_desc_comp *> fildset_;  //members being part of this key.
};

/***********************************
entity_desc_comp
***********************************/
typedef void (*enum_member_desc_comp_func)(const member_desc_comp  &desc);

struct entity_desc_comp {
        explicit entity_desc_comp(unsigned int entityid,
                                  NEntityType entitytype,
                                  const char *nmspace,
                                  const char *nclassname,
                                  vlg::alloc_func afun,
                                  unsigned int fild_num,
                                  bool persistent);

        RetCode extend(std::map<std::string, member_desc_comp *> &mmbrmap,
                       std::map<unsigned short, key_desc_comp *> &keymap);

        RetCode extend(std::map<std::string, member_desc_comp *> &mmbrmap);

        RetCode add_key_desc(key_desc_comp *keydesc);

        unsigned int get_entityid()  const;

        size_t get_size(VLG_COMP_ARCH arch,
                        VLG_COMP_OS os,
                        VLG_COMP_LANG lang,
                        VLG_COMP_TCOMP tcomp)  const;

        size_t get_entity_max_align(VLG_COMP_ARCH arch,
                                    VLG_COMP_OS os,
                                    VLG_COMP_LANG lang,
                                    VLG_COMP_TCOMP tcomp)  const;

        NEntityType get_nentity_type() const;
        const char *get_entity_namespace() const;
        const char *get_nentity_name() const;
        vlg::alloc_func get_entity_alloc_f() const;
        unsigned int get_field_num() const;
        bool is_persistent() const;

        std::map<unsigned short, member_desc_comp *> &get_map_id_MMBRDSC();
        const std::map<std::string, member_desc_comp *> &get_map_name_MMBRDSC() const;

        const std::map<unsigned short, key_desc_comp *> &get_map_keyid_KDESC() const;
        std::map<unsigned short, key_desc_comp *> &get_map_keyid_KDESC_mod();

        const member_desc_comp *get_member_desc_by_id(unsigned int mmbrid) const;
        const member_desc_comp *get_member_desc_by_name(const char *name)  const;

        void enum_member_desc(enum_member_desc_comp_func func) const;

        void set_entity_size(size_t,
                             VLG_COMP_ARCH arch,
                             VLG_COMP_OS os,
                             VLG_COMP_LANG lang,
                             VLG_COMP_TCOMP tcomp);

        void set_entity_max_align(size_t,
                                  VLG_COMP_ARCH arch,
                                  VLG_COMP_OS os,
                                  VLG_COMP_LANG lang,
                                  VLG_COMP_TCOMP tcomp);

    public:
        unsigned int entityid_;

        std::map<unsigned int, size_t> entity_size_map_;
        std::map<unsigned int, size_t> entity_max_align_map_;

        NEntityType entitytype_;
        const char *nmspace_;
        const char *entityname_;
        vlg::alloc_func afun_;
        unsigned int fild_num_;

        std::map<unsigned short, member_desc_comp *> mmbrid_mdesc_;
        std::map<std::string, member_desc_comp *> mmbrnm_mdesc_;  //mmbrname --> mmbrdesc

        //persistence
        bool persistent_;
        std::map<unsigned short, key_desc_comp *> keyid_kdesc_;
};

/***********************************
compile_unit
***********************************/
class compile_unit {
    public:
        compile_unit();
        ~compile_unit();

        RetCode init(const char *fname);

        RetCode parse();
        RetCode compile();

        const char *get_file_name();
        const char *model_name() const;
        const char *model_version() const;

        std::map<std::string, std::string> &get_define_map();
        std::map<std::string, entity_desc_comp *> &get_entity_map();

    private:
        char *fname_;
        char *model_name_;
        char *model_version_;
        std::map<std::string, std::string> define_map_;
        std::map<std::string, entity_desc_comp *> entity_map_;
};

/***********************************
GLOB FUNCTIONS
***********************************/
RetCode open_input_file(const char *fname,
                        FILE **fdesc);

RetCode open_output_file(const char *fname,
                         FILE **fdesc);

RetCode parse_data(const char *fname,
                   std::string &data,
                   std::map<std::string, std::string> &definemap,
                   std::map<std::string, entity_desc_comp *> &entitymap,
                   char **modname,
                   char **modver);

/*
Returns the potential next valid offset (to be used in next iteration).
Set in cur_offset the offset that must be used for the current field.
*/
size_t get_next_valid_offset(size_t &cur_offset,
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
size_t adjust_entity_size(size_t cur_offset,
                          size_t max_align,
                          VLG_COMP_ARCH arch,
                          VLG_COMP_OS os,
                          VLG_COMP_LANG lang,
                          VLG_COMP_TCOMP tcomp);

/***********************************
GLOB FUNCTIONS / COMPILING
***********************************/

/*
On machine-architecture dependant type size (in bytes)
*/
size_t get_in_arch_type_size(Type type,
                             VLG_COMP_ARCH arch,
                             VLG_COMP_OS os,
                             VLG_COMP_LANG lang,
                             VLG_COMP_TCOMP tcomp);

/*
On Network-protocol dependant type size (in bytes)
*/
size_t get_network_type_size(Type type);

RetCode compile_CPP(compile_unit &cunit);
RetCode compile_Java(compile_unit &cunit);

RetCode get_zero_val_for_VLG_TYPE(Type type, std::string &out);

RetCode target_type_from_builtin_VLG_TYPE(member_desc_comp &mdsc,
                                          std::string &out);

RetCode target_type_from_VLG_TYPE(member_desc_comp &mdsc,
                                  std::map<std::string, entity_desc_comp *> &entitymap,
                                  std::string &out);

RetCode printf_percent_from_VLG_TYPE(member_desc_comp &mdsc,
                                     std::string &out,
                                     bool strict_linux = false);

RetCode get_local_date(char *out);
RetCode put_newline(FILE *file);

RetCode render_hdr(compile_unit &cunit,
                   std::string &fname,
                   FILE *file);

}
