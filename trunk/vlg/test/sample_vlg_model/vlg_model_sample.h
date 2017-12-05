/********************************************************
Model:              smplmdl
Model ver:          0.0.0
Source:             vlg_model_sample.h
Compiled on:        04-12-2017
Lang:               C++
********************************************************/

#ifndef VLG_GEN_H_VLG_MODEL_SAMPLE
#define VLG_GEN_H_VLG_MODEL_SAMPLE
#if defined(__cplusplus)
#include "vlg_model.h"
#ifdef WIN32
#ifdef VLG_DMODEL_EXPORTS
#define EXP_SYM __declspec(dllexport)
#else
#ifdef VLG_DMODEL_IMPORTS
#define EXP_SYM __declspec(dllimport)
#else
#define EXP_SYM
#endif
#endif
#else
#ifdef __linux
#define EXP_SYM
#endif
#if defined (__MACH__) || defined (__APPLE__)
#define EXP_SYM __attribute__((visibility("default")))
#endif
#endif

#define MID_MYVALUE 57
#define MID_ANOTH_DEFINE 107
#define MYVALUE 5
#define ANOTH_DEFINE 10
#define NAME_LEN 128
#define EXTRA_ROLE_PER_USER 3

/*-----------------------------------------------------------------------------
vlg_enum: SIMPLE_ENUM_MID
-----------------------------------------------------------------------------*/
enum SIMPLE_ENUM_MID{
    SIMPLE_ENUM_MID_Zero,
    SIMPLE_ENUM_MID_One,
    SIMPLE_ENUM_MID_Two,
    SIMPLE_ENUM_MID_OneHundred=100,
    SIMPLE_ENUM_MID_TwoHundredAndOne=201,
};

/*-----------------------------------------------------------------------------
vlg_enum: SIMPLE_ENUM_A
-----------------------------------------------------------------------------*/
enum SIMPLE_ENUM_A{
    SIMPLE_ENUM_A_Zero,
    SIMPLE_ENUM_A_One,
    SIMPLE_ENUM_A_Two,
    SIMPLE_ENUM_A_OneHundred=100,
    SIMPLE_ENUM_A_TwoHundredAndOne=201,
};

/*-----------------------------------------------------------------------------
vlg_enum: SIMPLE_ENUM_B
-----------------------------------------------------------------------------*/
enum SIMPLE_ENUM_B{
    SIMPLE_ENUM_B_ONE=-12,
    SIMPLE_ENUM_B_TWO,
    SIMPLE_ENUM_B_THREE=43,
    SIMPLE_ENUM_B_FOUR,
    SIMPLE_ENUM_B_FIVE,
};

/*-----------------------------------------------------------------------------
vlg_enum: SEX
-----------------------------------------------------------------------------*/
enum SEX{
    SEX_Undef,
    SEX_Male,
    SEX_Female,
};

/*-----------------------------------------------------------------------------
nclass: ROLE - ID: 700
-----------------------------------------------------------------------------*/
namespace smplmdl{

#define ROLE_ENTITY_ID 700

class ROLE : public vlg::nclass{
friend class vlg::nentity_manager;
friend void* ROLE_alloc_func(size_t type_size, const void *copy);
public:
EXP_SYM ROLE();
EXP_SYM virtual ~ROLE();

public:
/*-----------------------------------------------------------------------------
virtual methods
-----------------------------------------------------------------------------*/
EXP_SYM virtual unsigned int get_nclass_id() const;
EXP_SYM virtual unsigned int get_compiler_version() const;
EXP_SYM virtual size_t get_size() const;
EXP_SYM virtual const ROLE* get_zero_object() const;
EXP_SYM virtual void copy_to(vlg::nclass *obj) const;
EXP_SYM virtual ROLE* clone() const;
EXP_SYM virtual bool is_zero() const;
EXP_SYM virtual void set_zero();
EXP_SYM virtual void set_from(const vlg::nclass *obj);
EXP_SYM virtual const vlg::nentity_desc* get_nentity_descriptor() const;
EXP_SYM virtual size_t pretty_dump_to_buffer(char *buff, bool print_cname = true) const;
EXP_SYM virtual size_t pretty_dump_to_file(FILE *f, bool print_cname = true) const;
EXP_SYM virtual int serialize(vlg::Encode enctyp, const vlg::nclass *prev_image, vlg::grow_byte_buffer *obb) const;

/*-----------------------------------------------------------------------------
getter(s) / setter(s) / is_zero(s)
-----------------------------------------------------------------------------*/
EXP_SYM uint64_t get_role_id();
EXP_SYM void set_role_id(uint64_t val);
EXP_SYM bool is_zero_role_id() const;
EXP_SYM char* get_role_name();
EXP_SYM char get_role_name_idx(size_t idx);
EXP_SYM void set_role_name(const char *val);
EXP_SYM void set_role_name_idx(size_t idx, char val);
EXP_SYM bool is_zero_role_name() const;
EXP_SYM bool is_zero_role_name_idx(size_t idx) const;
EXP_SYM bool get_can_act_as_admin();
EXP_SYM void set_can_act_as_admin(bool val);
EXP_SYM bool is_zero_can_act_as_admin() const;

/*-----------------------------------------------------------------------------
representation
-----------------------------------------------------------------------------*/
protected:
uint64_t role_id;
char role_name[34];
bool can_act_as_admin;

public:
static const ROLE ZERO_OBJ;
};
}

/*-----------------------------------------------------------------------------
nclass: USER - ID: 800
-----------------------------------------------------------------------------*/
namespace smplmdl{

#define USER_ENTITY_ID 800

class USER : public vlg::nclass{
friend class vlg::nentity_manager;
friend void* USER_alloc_func(size_t type_size, const void *copy);
public:
EXP_SYM USER();
EXP_SYM virtual ~USER();

public:
/*-----------------------------------------------------------------------------
virtual methods
-----------------------------------------------------------------------------*/
EXP_SYM virtual unsigned int get_nclass_id() const;
EXP_SYM virtual unsigned int get_compiler_version() const;
EXP_SYM virtual size_t get_size() const;
EXP_SYM virtual const USER* get_zero_object() const;
EXP_SYM virtual void copy_to(vlg::nclass *obj) const;
EXP_SYM virtual USER* clone() const;
EXP_SYM virtual bool is_zero() const;
EXP_SYM virtual void set_zero();
EXP_SYM virtual void set_from(const vlg::nclass *obj);
EXP_SYM virtual const vlg::nentity_desc* get_nentity_descriptor() const;
EXP_SYM virtual size_t pretty_dump_to_buffer(char *buff, bool print_cname = true) const;
EXP_SYM virtual size_t pretty_dump_to_file(FILE *f, bool print_cname = true) const;
EXP_SYM virtual int serialize(vlg::Encode enctyp, const vlg::nclass *prev_image, vlg::grow_byte_buffer *obb) const;

/*-----------------------------------------------------------------------------
getter(s) / setter(s) / is_zero(s)
-----------------------------------------------------------------------------*/
EXP_SYM unsigned int get_user_id();
EXP_SYM void set_user_id(unsigned int val);
EXP_SYM bool is_zero_user_id() const;
EXP_SYM char* get_name();
EXP_SYM char get_name_idx(size_t idx);
EXP_SYM void set_name(const char *val);
EXP_SYM void set_name_idx(size_t idx, char val);
EXP_SYM bool is_zero_name() const;
EXP_SYM bool is_zero_name_idx(size_t idx) const;
EXP_SYM char* get_surname();
EXP_SYM char get_surname_idx(size_t idx);
EXP_SYM void set_surname(const char *val);
EXP_SYM void set_surname_idx(size_t idx, char val);
EXP_SYM bool is_zero_surname() const;
EXP_SYM bool is_zero_surname_idx(size_t idx) const;
EXP_SYM SEX get_sex();
EXP_SYM void set_sex(SEX val);
EXP_SYM bool is_zero_sex() const;
EXP_SYM char* get_email();
EXP_SYM char get_email_idx(size_t idx);
EXP_SYM void set_email(const char *val);
EXP_SYM void set_email_idx(size_t idx, char val);
EXP_SYM bool is_zero_email() const;
EXP_SYM bool is_zero_email_idx(size_t idx) const;
EXP_SYM float get_height();
EXP_SYM void set_height(float val);
EXP_SYM bool is_zero_height() const;
EXP_SYM float get_weight();
EXP_SYM void set_weight(float val);
EXP_SYM bool is_zero_weight() const;
EXP_SYM bool get_active();
EXP_SYM void set_active(bool val);
EXP_SYM bool is_zero_active() const;
EXP_SYM short get_cap();
EXP_SYM void set_cap(short val);
EXP_SYM bool is_zero_cap() const;
EXP_SYM char get_type();
EXP_SYM void set_type(char val);
EXP_SYM bool is_zero_type() const;
EXP_SYM ROLE* get_main_role();
EXP_SYM void set_main_role(const ROLE *val);
EXP_SYM bool is_zero_main_role() const;
EXP_SYM ROLE* get_extra_roles();
EXP_SYM ROLE* get_extra_roles_idx(size_t idx);
EXP_SYM void set_extra_roles(const ROLE *val);
EXP_SYM void set_extra_roles_idx(size_t idx, const ROLE *val);
EXP_SYM bool is_zero_extra_roles() const;
EXP_SYM bool is_zero_extra_roles_idx(size_t idx) const;

/*-----------------------------------------------------------------------------
representation
-----------------------------------------------------------------------------*/
protected:
unsigned int user_id;
char name[130];
char surname[130];
SEX sex;
char email[65];
float height;
float weight;
bool active;
short cap;
char type;
ROLE main_role;
ROLE extra_roles[3];

public:
static const USER ZERO_OBJ;
};
}

/*-----------------------------------------------------------------------------
model version
-----------------------------------------------------------------------------*/
extern "C"{
EXP_SYM const char* get_mdl_ver_smplmdl();
}

/*-----------------------------------------------------------------------------
NEM entry point
-----------------------------------------------------------------------------*/
extern "C"{
EXP_SYM vlg::nentity_manager* get_em_smplmdl();
}

#else
/*-----------------------------------------------------------------------------
NEM entry point C
-----------------------------------------------------------------------------*/
nentity_manager_wr get_c_em_smplmdl();
#endif
#endif

