/********************************************************
Model:              smplmdl
Model ver:          0.0.0
Source:             vlg_model_sample.cpp
Compiled on:        13-03-2018
Lang:               C++
********************************************************/

#include "vlg_model_sample.h"

#define ENTLN_B_SZ 4
#define FLDLN_B_SZ 4
#define ARRAY_B_SZ 4
#define ARIDX_B_SZ 2
#define MMBID_B_SZ 2
#define ENUM_B_SZ 4

#define COMMAND_IF_NOT_OK(fun, cmd)\
{\
    int res;\
    if((res = fun)){\
        cmd;\
    }\
}

#define COMMAND_IF_NULL(ptr, cmd)\
{\
    if(!(ptr)){\
        cmd;\
    }\
}

/*-----------------------------------------------------------------------------
Dynamic Byte Buffer
-----------------------------------------------------------------------------*/
namespace vlg { class g_bbuf {
public:
    explicit g_bbuf();
    ~g_bbuf();
    RetCode init(size_t initial_capacity);
    void reset();
    void flip();
    RetCode grow(size_t);
    RetCode ensure_capacity(size_t);
    RetCode append(const void*, size_t, size_t);
    RetCode append_ushort(unsigned short);
    RetCode append_uint(unsigned int);
    RetCode put(const void*, size_t, size_t);
    size_t position() const;
    size_t limit() const;
    size_t mark() const;
    size_t capacity() const;
    size_t remaining() const;
    unsigned char *buffer();
    char *buffer_as_char();
    unsigned int *buffer_as_uint();
    RetCode advance_pos_write(size_t);
    RetCode set_pos_write(size_t);
    void move_pos_write(size_t);
    RetCode advance_pos_read(size_t);
    RetCode set_pos_read(size_t);
    void set_mark();
    RetCode set_mark(size_t);
    size_t from_mark() const;
    size_t available_read();
    RetCode read(size_t, void*);
    RetCode read_ushort(unsigned short*);
    RetCode read_uint(unsigned int*);
    RetCode read_uint_to_sizet(size_t*);
}; }

/*-----------------------------------------------------------------------------
CLASS ALLOCATORS
-----------------------------------------------------------------------------*/
namespace smplmdl{
void* ROLE_alloc_func()
{
    return new smplmdl::ROLE();
}
}
namespace smplmdl{
void* USER_alloc_func()
{
    return new smplmdl::USER();
}
}

/*-----------------------------------------------------------------------------
CLASS ROLE DESC
-----------------------------------------------------------------------------*/
#if defined WIN32 && defined _MSC_VER
vlg::nentity_desc ROLE_EntityDesc
(
    700,
    56,
    8,
    vlg::NEntityType_NCLASS,
    "smplmdl",
    "ROLE",
    smplmdl::ROLE_alloc_func,
    3,
    false
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::nentity_desc ROLE_EntityDesc
(
    700,
    56,
    8,
    vlg::NEntityType_NCLASS,
    "smplmdl",
    "ROLE",
    smplmdl::ROLE_alloc_func,
    3,
    false
);
#endif

#if defined WIN32 && defined _MSC_VER
vlg::member_desc ROLE_role_id
(
    1,
    vlg::MemberType_FIELD,
    "role_id",
    "",
    vlg::Type_UINT64,
    0x8,
    8,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc ROLE_role_id
(
    1,
    vlg::MemberType_FIELD,
    "role_id",
    "",
    vlg::Type_UINT64,
    0x8,
    8,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc ROLE_role_name
(
    2,
    vlg::MemberType_FIELD,
    "role_name",
    "",
    vlg::Type_ASCII,
    0x10,
    1,
    34,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc ROLE_role_name
(
    2,
    vlg::MemberType_FIELD,
    "role_name",
    "",
    vlg::Type_ASCII,
    0x10,
    1,
    34,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc ROLE_can_act_as_admin
(
    3,
    vlg::MemberType_FIELD,
    "can_act_as_admin",
    "can be admin",
    vlg::Type_BOOL,
    0x32,
    1,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc ROLE_can_act_as_admin
(
    3,
    vlg::MemberType_FIELD,
    "can_act_as_admin",
    "can be admin",
    vlg::Type_BOOL,
    0x32,
    1,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
/*-----------------------------------------------------------------------------
ENUM SEX DESC
-----------------------------------------------------------------------------*/
vlg::nentity_desc SEX_EntityDesc
(
    0,
    0,
    0,
    vlg::NEntityType_NENUM,
    "smplmdl",
    "SEX",
    0,
    0,
    false
);

#if defined WIN32 && defined _MSC_VER
vlg::member_desc SEX_SEX_Undef
(
    1,
    vlg::MemberType_NENUM_VALUE,
    "SEX_Undef",
    "Undef",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SEX_SEX_Undef
(
    1,
    vlg::MemberType_NENUM_VALUE,
    "SEX_Undef",
    "Undef",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SEX_SEX_Male
(
    2,
    vlg::MemberType_NENUM_VALUE,
    "SEX_Male",
    "Male",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    1
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SEX_SEX_Male
(
    2,
    vlg::MemberType_NENUM_VALUE,
    "SEX_Male",
    "Male",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    1
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SEX_SEX_Female
(
    3,
    vlg::MemberType_NENUM_VALUE,
    "SEX_Female",
    "Female",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    2
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SEX_SEX_Female
(
    3,
    vlg::MemberType_NENUM_VALUE,
    "SEX_Female",
    "Female",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    2
);
#endif
/*-----------------------------------------------------------------------------
ENUM SIMPLE_ENUM_A DESC
-----------------------------------------------------------------------------*/
vlg::nentity_desc SIMPLE_ENUM_A_EntityDesc
(
    0,
    0,
    0,
    vlg::NEntityType_NENUM,
    "",
    "SIMPLE_ENUM_A",
    0,
    0,
    false
);

#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_Zero
(
    1,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_Zero",
    "Zero",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_Zero
(
    1,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_Zero",
    "Zero",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_One
(
    2,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_One",
    "One",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    1
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_One
(
    2,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_One",
    "One",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    1
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_Two
(
    3,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_Two",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    2
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_Two
(
    3,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_Two",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    2
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_OneHundred
(
    4,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_OneHundred",
    "OneHundred",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    100
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_OneHundred
(
    4,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_OneHundred",
    "OneHundred",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    100
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_TwoHundredAndOne
(
    5,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_TwoHundredAndOne",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    201
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_A_SIMPLE_ENUM_A_TwoHundredAndOne
(
    5,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_A_TwoHundredAndOne",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    201
);
#endif
/*-----------------------------------------------------------------------------
ENUM SIMPLE_ENUM_B DESC
-----------------------------------------------------------------------------*/
vlg::nentity_desc SIMPLE_ENUM_B_EntityDesc
(
    0,
    0,
    0,
    vlg::NEntityType_NENUM,
    "",
    "SIMPLE_ENUM_B",
    0,
    0,
    false
);

#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_ONE
(
    1,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_ONE",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    -12
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_ONE
(
    1,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_ONE",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    -12
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_TWO
(
    2,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_TWO",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    -11
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_TWO
(
    2,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_TWO",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    -11
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_THREE
(
    3,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_THREE",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    43
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_THREE
(
    3,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_THREE",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    43
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_FOUR
(
    4,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_FOUR",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    44
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_FOUR
(
    4,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_FOUR",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    44
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_FIVE
(
    5,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_FIVE",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    45
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_B_SIMPLE_ENUM_B_FIVE
(
    5,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_B_FIVE",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    45
);
#endif
/*-----------------------------------------------------------------------------
ENUM SIMPLE_ENUM_MID DESC
-----------------------------------------------------------------------------*/
vlg::nentity_desc SIMPLE_ENUM_MID_EntityDesc
(
    0,
    0,
    0,
    vlg::NEntityType_NENUM,
    "",
    "SIMPLE_ENUM_MID",
    0,
    0,
    false
);

#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_Zero
(
    1,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_Zero",
    "Zero",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_Zero
(
    1,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_Zero",
    "Zero",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_One
(
    2,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_One",
    "One",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    1
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_One
(
    2,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_One",
    "One",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    1
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_Two
(
    3,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_Two",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    2
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_Two
(
    3,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_Two",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    2
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_OneHundred
(
    4,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_OneHundred",
    "OneHundred",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    100
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_OneHundred
(
    4,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_OneHundred",
    "OneHundred",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    100
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_TwoHundredAndOne
(
    5,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_TwoHundredAndOne",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    201
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc SIMPLE_ENUM_MID_SIMPLE_ENUM_MID_TwoHundredAndOne
(
    5,
    vlg::MemberType_NENUM_VALUE,
    "SIMPLE_ENUM_MID_TwoHundredAndOne",
    "",
    vlg::Type_INT32,
    0x0,
    4,
    1,
    0,
    "",
    vlg::NEntityType_NENUM,
    201
);
#endif
/*-----------------------------------------------------------------------------
CLASS USER DESC
-----------------------------------------------------------------------------*/
#if defined WIN32 && defined _MSC_VER
vlg::nentity_desc USER_EntityDesc
(
    800,
    584,
    8,
    vlg::NEntityType_NCLASS,
    "smplmdl",
    "USER",
    smplmdl::USER_alloc_func,
    12,
    true
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::nentity_desc USER_EntityDesc
(
    800,
    584,
    8,
    vlg::NEntityType_NCLASS,
    "smplmdl",
    "USER",
    smplmdl::USER_alloc_func,
    12,
    true
);
#endif

#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_user_id
(
    1,
    vlg::MemberType_FIELD,
    "user_id",
    "id",
    vlg::Type_UINT32,
    0x8,
    4,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_user_id
(
    1,
    vlg::MemberType_FIELD,
    "user_id",
    "id",
    vlg::Type_UINT32,
    0x8,
    4,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_name
(
    2,
    vlg::MemberType_FIELD,
    "name",
    "name",
    vlg::Type_ASCII,
    0xc,
    1,
    130,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_name
(
    2,
    vlg::MemberType_FIELD,
    "name",
    "name",
    vlg::Type_ASCII,
    0xc,
    1,
    130,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_surname
(
    3,
    vlg::MemberType_FIELD,
    "surname",
    "surname",
    vlg::Type_ASCII,
    0x8e,
    1,
    130,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_surname
(
    3,
    vlg::MemberType_FIELD,
    "surname",
    "surname",
    vlg::Type_ASCII,
    0x8e,
    1,
    130,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_sex
(
    4,
    vlg::MemberType_FIELD,
    "sex",
    "sex",
    vlg::Type_ENTITY,
    0x110,
    4,
    1,
    0,
    "SEX",
    vlg::NEntityType_NENUM,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_sex
(
    4,
    vlg::MemberType_FIELD,
    "sex",
    "sex",
    vlg::Type_ENTITY,
    0x110,
    4,
    1,
    0,
    "SEX",
    vlg::NEntityType_NENUM,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_email
(
    5,
    vlg::MemberType_FIELD,
    "email",
    "surname",
    vlg::Type_ASCII,
    0x114,
    1,
    65,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_email
(
    5,
    vlg::MemberType_FIELD,
    "email",
    "surname",
    vlg::Type_ASCII,
    0x114,
    1,
    65,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_height
(
    6,
    vlg::MemberType_FIELD,
    "height",
    "height",
    vlg::Type_FLOAT32,
    0x158,
    4,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_height
(
    6,
    vlg::MemberType_FIELD,
    "height",
    "height",
    vlg::Type_FLOAT32,
    0x158,
    4,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_weight
(
    7,
    vlg::MemberType_FIELD,
    "weight",
    "weight",
    vlg::Type_FLOAT32,
    0x15c,
    4,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_weight
(
    7,
    vlg::MemberType_FIELD,
    "weight",
    "weight",
    vlg::Type_FLOAT32,
    0x15c,
    4,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_active
(
    8,
    vlg::MemberType_FIELD,
    "active",
    "active",
    vlg::Type_BOOL,
    0x160,
    1,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_active
(
    8,
    vlg::MemberType_FIELD,
    "active",
    "active",
    vlg::Type_BOOL,
    0x160,
    1,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_cap
(
    9,
    vlg::MemberType_FIELD,
    "cap",
    "CAP",
    vlg::Type_INT16,
    0x162,
    2,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_cap
(
    9,
    vlg::MemberType_FIELD,
    "cap",
    "CAP",
    vlg::Type_INT16,
    0x162,
    2,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_type
(
    10,
    vlg::MemberType_FIELD,
    "type",
    "type",
    vlg::Type_ASCII,
    0x164,
    1,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_type
(
    10,
    vlg::MemberType_FIELD,
    "type",
    "type",
    vlg::Type_ASCII,
    0x164,
    1,
    1,
    0,
    "",
    vlg::NEntityType_UNDEFINED,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_main_role
(
    11,
    vlg::MemberType_FIELD,
    "main_role",
    "main_role",
    vlg::Type_ENTITY,
    0x168,
    56,
    1,
    700,
    "ROLE",
    vlg::NEntityType_NCLASS,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_main_role
(
    11,
    vlg::MemberType_FIELD,
    "main_role",
    "main_role",
    vlg::Type_ENTITY,
    0x168,
    56,
    1,
    700,
    "ROLE",
    vlg::NEntityType_NCLASS,
    0
);
#endif
#if defined WIN32 && defined _MSC_VER
vlg::member_desc USER_extra_roles
(
    12,
    vlg::MemberType_FIELD,
    "extra_roles",
    "extra_roles",
    vlg::Type_ENTITY,
    0x1a0,
    56,
    3,
    700,
    "ROLE",
    vlg::NEntityType_NCLASS,
    0
);
#endif
#if !defined WIN32 && defined __GNUG__
vlg::member_desc USER_extra_roles
(
    12,
    vlg::MemberType_FIELD,
    "extra_roles",
    "extra_roles",
    vlg::Type_ENTITY,
    0x1a0,
    56,
    3,
    700,
    "ROLE",
    vlg::NEntityType_NCLASS,
    0
);
#endif
vlg::key_desc KEY_USER_1
(
    1,
    true
);

vlg::key_desc KEY_USER_2
(
    2,
    false
);

vlg::key_desc KEY_USER_3
(
    3,
    false
);


namespace smplmdl{
/*-----------------------------------------------------------------------------
CLASS ROLE CTOR
-----------------------------------------------------------------------------*/
EXP_SYM ROLE::ROLE():
    role_id(0),
    can_act_as_admin(false)
{
    memset(&role_name[0], 0, sizeof(role_name));
}
EXP_SYM ROLE::~ROLE(){}

/*-----------------------------------------------------------------------------
ZERO ROLE OBJ
-----------------------------------------------------------------------------*/
const ROLE ROLE::ZERO_OBJ;

/*-----------------------------------------------------------------------------
Getter(s) / Setter(s) / is_zero(s)
-----------------------------------------------------------------------------*/
EXP_SYM uint64_t ROLE::get_role_id()
{
    return role_id;
}
EXP_SYM void ROLE::set_role_id(uint64_t val)
{
    role_id = val;
}
EXP_SYM bool ROLE::is_zero_role_id() const
{
    return !memcmp(&role_id, &ZERO_OBJ.role_id, sizeof(uint64_t)*1);
}
EXP_SYM char* ROLE::get_role_name()
{
    return &role_name[0];
}
EXP_SYM char ROLE::get_role_name_idx(size_t idx)
{
    return role_name[idx];
}
EXP_SYM void ROLE::set_role_name(const char *val)
{
    memcpy((void*)&role_name[0], (void*)val, sizeof(char)*34-1);
}
EXP_SYM void ROLE::set_role_name_idx(size_t idx, char val)
{
    role_name[idx] = val;
}
EXP_SYM bool ROLE::is_zero_role_name() const
{
    return !memcmp(&role_name[0], &ZERO_OBJ.role_name[0], sizeof(char)*34);
}
EXP_SYM bool ROLE::is_zero_role_name_idx(size_t idx) const
{
    return !memcmp(&role_name[idx], &ZERO_OBJ.role_name[0], sizeof(char));
}
EXP_SYM bool ROLE::get_can_act_as_admin()
{
    return can_act_as_admin;
}
EXP_SYM void ROLE::set_can_act_as_admin(bool val)
{
    can_act_as_admin = val;
}
EXP_SYM bool ROLE::is_zero_can_act_as_admin() const
{
    return !memcmp(&can_act_as_admin, &ZERO_OBJ.can_act_as_admin, sizeof(bool)*1);
}
/*-----------------------------------------------------------------------------
CLASS ROLE VIRTUAL METHS.
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
CLASS ROLE get_id.
-----------------------------------------------------------------------------*/
EXP_SYM unsigned int ROLE::get_id() const
{
    return 700;
}
/*-----------------------------------------------------------------------------
CLASS ROLE get_compiler_version.
-----------------------------------------------------------------------------*/
EXP_SYM unsigned int ROLE::get_compiler_version() const
{
    return 0;
}
/*-----------------------------------------------------------------------------
CLASS ROLE get_size.
-----------------------------------------------------------------------------*/
EXP_SYM size_t ROLE::get_size() const
{
    return sizeof(ROLE);
}
/*-----------------------------------------------------------------------------
CLASS ROLE get_zero_object.
-----------------------------------------------------------------------------*/
EXP_SYM const ROLE& ROLE::get_zero_object() const
{
    return ZERO_OBJ;
}
/*-----------------------------------------------------------------------------
CLASS ROLE copy_to.
-----------------------------------------------------------------------------*/
EXP_SYM void ROLE::copy_to(nclass &out) const
{
    memcpy((void*)(&out), (void*)(this), sizeof(ROLE));
}
/*-----------------------------------------------------------------------------
CLASS ROLE clone.
-----------------------------------------------------------------------------*/
EXP_SYM std::unique_ptr<vlg::nclass> ROLE::clone() const
{
    ROLE *newptr = nullptr;
    COMMAND_IF_NULL(newptr = new ROLE(), exit(1))
    copy_to(*newptr);
    return std::unique_ptr<ROLE>(newptr);
}
/*-----------------------------------------------------------------------------
CLASS ROLE is_zero.
-----------------------------------------------------------------------------*/
EXP_SYM bool ROLE::is_zero() const
{
    if(!is_zero_role_id())
        return false;
    if(!is_zero_role_name())
        return false;
    if(!is_zero_can_act_as_admin())
        return false;
    return true;
}
/*-----------------------------------------------------------------------------
CLASS ROLE set_zero.
-----------------------------------------------------------------------------*/
EXP_SYM void ROLE::set_zero()
{
    memcpy((void*)(this), (void*)(&ZERO_OBJ), sizeof(ROLE));
}
/*-----------------------------------------------------------------------------
CLASS ROLE set_from.
-----------------------------------------------------------------------------*/
EXP_SYM void ROLE::set_from(const nclass &obj)
{
    memcpy((void*)(this), (void*)(&obj), sizeof(ROLE));
}
/*-----------------------------------------------------------------------------
CLASS ROLE get_nentity_descriptor.
-----------------------------------------------------------------------------*/
EXP_SYM const vlg::nentity_desc& ROLE::get_nentity_descriptor() const
{
    return ROLE_EntityDesc;
}
/*-----------------------------------------------------------------------------
CLASS ROLE pretty_dump_to_buffer.
-----------------------------------------------------------------------------*/
EXP_SYM size_t ROLE::pretty_dump_to_buffer(char *buff, bool print_name) const
{
    size_t blen = 0;
    if(print_name) blen += sprintf(&buff[blen], "ROLE");
    blen += sprintf(&buff[blen], "{");
#if defined __GNUG__ && defined __linux
        if(!is_zero_role_id()) blen += sprintf(&buff[blen], "role_id=%lu|", role_id);
#else
        if(!is_zero_role_id()) blen += sprintf(&buff[blen], "role_id=%llu|", role_id);
#endif
        if(!is_zero_role_name()) blen += sprintf(&buff[blen], "role_name=[%s]", role_name);
        if(!is_zero_can_act_as_admin()) blen += sprintf(&buff[blen], "can_act_as_admin=%d|", can_act_as_admin);
    blen += sprintf(&buff[blen], "}");
    return blen;
}
/*-----------------------------------------------------------------------------
CLASS ROLE pretty_dump_to_file.
-----------------------------------------------------------------------------*/
EXP_SYM size_t ROLE::pretty_dump_to_file(FILE *f, bool print_name) const
{
    size_t blen = 0;
    if(print_name) blen += fprintf(f, "ROLE");
    blen += fprintf(f, "{");
#if defined __GNUG__ && defined __linux
        if(!is_zero_role_id()) blen += fprintf(f, "role_id=%lu|", role_id);
#else
        if(!is_zero_role_id()) blen += fprintf(f, "role_id=%llu|", role_id);
#endif
        if(!is_zero_role_name()) blen += fprintf(f, "role_name=[%s]", role_name);
        if(!is_zero_can_act_as_admin()) blen += fprintf(f, "can_act_as_admin=%d|", can_act_as_admin);
    blen += fprintf(f, "}");
    return blen;
}
/*-----------------------------------------------------------------------------
CLASS ROLE serialize.
-----------------------------------------------------------------------------*/
EXP_SYM int ROLE::serialize(vlg::Encode enctyp, const nclass *prev_image, vlg::g_bbuf *obb) const
{
    size_t tlen_offst = obb->position(), tlen = 0;
    switch(enctyp){
        case vlg::Encode_INDEXED_NOT_ZERO:
            obb->advance_pos_write(ENTLN_B_SZ); tlen = obb->position();
            if(!is_zero_role_id()){
                obb->grow(MMBID_B_SZ+8);
                obb->append_ushort(1);
                obb->append(&role_id, 0, 8);
            }
            if(!is_zero_role_name()){
                size_t fsize = strnlen(role_name, 34);
                obb->grow(MMBID_B_SZ+FLDLN_B_SZ+fsize);
                obb->append_ushort(2);
                obb->append_uint((unsigned int)fsize);
                obb->append(role_name, 0, fsize);
            }
            if(!is_zero_can_act_as_admin()){
                obb->grow(MMBID_B_SZ+1);
                obb->append_ushort(3);
                obb->append(&can_act_as_admin, 0, 1);
            }
            tlen = (obb->position() - tlen); obb->put(&tlen, tlen_offst, ENTLN_B_SZ);
            break;
        default: return -1;
    }
    return (int)obb->position();
}
}
namespace smplmdl{
/*-----------------------------------------------------------------------------
CLASS USER CTOR
-----------------------------------------------------------------------------*/
EXP_SYM USER::USER():
    user_id(0),
    height(0),
    weight(0),
    active(false),
    cap(0),
    type('\0')
{
    memset(&name[0], 0, sizeof(name));
    memset(&surname[0], 0, sizeof(surname));
    memset(&sex, 0, sizeof(ENUM_B_SZ));
    memset(&email[0], 0, sizeof(email));
}
EXP_SYM USER::~USER(){}

/*-----------------------------------------------------------------------------
ZERO USER OBJ
-----------------------------------------------------------------------------*/
const USER USER::ZERO_OBJ;

/*-----------------------------------------------------------------------------
Getter(s) / Setter(s) / is_zero(s)
-----------------------------------------------------------------------------*/
EXP_SYM unsigned int USER::get_user_id()
{
    return user_id;
}
EXP_SYM void USER::set_user_id(unsigned int val)
{
    user_id = val;
}
EXP_SYM bool USER::is_zero_user_id() const
{
    return !memcmp(&user_id, &ZERO_OBJ.user_id, sizeof(unsigned int)*1);
}
EXP_SYM char* USER::get_name()
{
    return &name[0];
}
EXP_SYM char USER::get_name_idx(size_t idx)
{
    return name[idx];
}
EXP_SYM void USER::set_name(const char *val)
{
    memcpy((void*)&name[0], (void*)val, sizeof(char)*130-1);
}
EXP_SYM void USER::set_name_idx(size_t idx, char val)
{
    name[idx] = val;
}
EXP_SYM bool USER::is_zero_name() const
{
    return !memcmp(&name[0], &ZERO_OBJ.name[0], sizeof(char)*130);
}
EXP_SYM bool USER::is_zero_name_idx(size_t idx) const
{
    return !memcmp(&name[idx], &ZERO_OBJ.name[0], sizeof(char));
}
EXP_SYM char* USER::get_surname()
{
    return &surname[0];
}
EXP_SYM char USER::get_surname_idx(size_t idx)
{
    return surname[idx];
}
EXP_SYM void USER::set_surname(const char *val)
{
    memcpy((void*)&surname[0], (void*)val, sizeof(char)*130-1);
}
EXP_SYM void USER::set_surname_idx(size_t idx, char val)
{
    surname[idx] = val;
}
EXP_SYM bool USER::is_zero_surname() const
{
    return !memcmp(&surname[0], &ZERO_OBJ.surname[0], sizeof(char)*130);
}
EXP_SYM bool USER::is_zero_surname_idx(size_t idx) const
{
    return !memcmp(&surname[idx], &ZERO_OBJ.surname[0], sizeof(char));
}
EXP_SYM SEX USER::get_sex()
{
    return sex;
}
EXP_SYM void USER::set_sex(SEX val)
{
    sex = val;
}
EXP_SYM bool USER::is_zero_sex() const
{
    return !memcmp(&sex, &ZERO_OBJ.sex, sizeof(SEX)*1);
}
EXP_SYM char* USER::get_email()
{
    return &email[0];
}
EXP_SYM char USER::get_email_idx(size_t idx)
{
    return email[idx];
}
EXP_SYM void USER::set_email(const char *val)
{
    memcpy((void*)&email[0], (void*)val, sizeof(char)*65-1);
}
EXP_SYM void USER::set_email_idx(size_t idx, char val)
{
    email[idx] = val;
}
EXP_SYM bool USER::is_zero_email() const
{
    return !memcmp(&email[0], &ZERO_OBJ.email[0], sizeof(char)*65);
}
EXP_SYM bool USER::is_zero_email_idx(size_t idx) const
{
    return !memcmp(&email[idx], &ZERO_OBJ.email[0], sizeof(char));
}
EXP_SYM float USER::get_height()
{
    return height;
}
EXP_SYM void USER::set_height(float val)
{
    height = val;
}
EXP_SYM bool USER::is_zero_height() const
{
    return !memcmp(&height, &ZERO_OBJ.height, sizeof(float)*1);
}
EXP_SYM float USER::get_weight()
{
    return weight;
}
EXP_SYM void USER::set_weight(float val)
{
    weight = val;
}
EXP_SYM bool USER::is_zero_weight() const
{
    return !memcmp(&weight, &ZERO_OBJ.weight, sizeof(float)*1);
}
EXP_SYM bool USER::get_active()
{
    return active;
}
EXP_SYM void USER::set_active(bool val)
{
    active = val;
}
EXP_SYM bool USER::is_zero_active() const
{
    return !memcmp(&active, &ZERO_OBJ.active, sizeof(bool)*1);
}
EXP_SYM short USER::get_cap()
{
    return cap;
}
EXP_SYM void USER::set_cap(short val)
{
    cap = val;
}
EXP_SYM bool USER::is_zero_cap() const
{
    return !memcmp(&cap, &ZERO_OBJ.cap, sizeof(short)*1);
}
EXP_SYM char USER::get_type()
{
    return type;
}
EXP_SYM void USER::set_type(char val)
{
    type = val;
}
EXP_SYM bool USER::is_zero_type() const
{
    return !memcmp(&type, &ZERO_OBJ.type, sizeof(char)*1);
}
EXP_SYM ROLE* USER::get_main_role()
{
    return &main_role;
}
EXP_SYM void USER::set_main_role(const ROLE *val)
{
    memcpy((void*)&main_role, (void*)val, sizeof(ROLE));
}
EXP_SYM bool USER::is_zero_main_role() const
{
    return main_role.is_zero();
}
EXP_SYM ROLE* USER::get_extra_roles()
{
    return &extra_roles[0];
}
EXP_SYM ROLE* USER::get_extra_roles_idx(size_t idx)
{
    return &extra_roles[idx];
}
EXP_SYM void USER::set_extra_roles(const ROLE *val)
{
    memcpy((void*)&extra_roles[0], (void*)val, sizeof(ROLE)*3);
}
EXP_SYM void USER::set_extra_roles_idx(size_t idx, const ROLE *val)
{
    memcpy((void*)&extra_roles[idx], (void*)val, sizeof(ROLE));
}
EXP_SYM bool USER::is_zero_extra_roles() const
{
    for(int i = 0; i<3; i++){
        if(!extra_roles[i].is_zero()) return false;
    }
    return true;
}
EXP_SYM bool USER::is_zero_extra_roles_idx(size_t idx) const
{
    return extra_roles[idx].is_zero();
}
/*-----------------------------------------------------------------------------
CLASS USER VIRTUAL METHS.
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
CLASS USER get_id.
-----------------------------------------------------------------------------*/
EXP_SYM unsigned int USER::get_id() const
{
    return 800;
}
/*-----------------------------------------------------------------------------
CLASS USER get_compiler_version.
-----------------------------------------------------------------------------*/
EXP_SYM unsigned int USER::get_compiler_version() const
{
    return 0;
}
/*-----------------------------------------------------------------------------
CLASS USER get_size.
-----------------------------------------------------------------------------*/
EXP_SYM size_t USER::get_size() const
{
    return sizeof(USER);
}
/*-----------------------------------------------------------------------------
CLASS USER get_zero_object.
-----------------------------------------------------------------------------*/
EXP_SYM const USER& USER::get_zero_object() const
{
    return ZERO_OBJ;
}
/*-----------------------------------------------------------------------------
CLASS USER copy_to.
-----------------------------------------------------------------------------*/
EXP_SYM void USER::copy_to(nclass &out) const
{
    memcpy((void*)(&out), (void*)(this), sizeof(USER));
}
/*-----------------------------------------------------------------------------
CLASS USER clone.
-----------------------------------------------------------------------------*/
EXP_SYM std::unique_ptr<vlg::nclass> USER::clone() const
{
    USER *newptr = nullptr;
    COMMAND_IF_NULL(newptr = new USER(), exit(1))
    copy_to(*newptr);
    return std::unique_ptr<USER>(newptr);
}
/*-----------------------------------------------------------------------------
CLASS USER is_zero.
-----------------------------------------------------------------------------*/
EXP_SYM bool USER::is_zero() const
{
    if(!is_zero_user_id())
        return false;
    if(!is_zero_name())
        return false;
    if(!is_zero_surname())
        return false;
    if(!is_zero_sex())
        return false;
    if(!is_zero_email())
        return false;
    if(!is_zero_height())
        return false;
    if(!is_zero_weight())
        return false;
    if(!is_zero_active())
        return false;
    if(!is_zero_cap())
        return false;
    if(!is_zero_type())
        return false;
    if(!is_zero_main_role())
        return false;
    if(!is_zero_extra_roles())
        return false;
    return true;
}
/*-----------------------------------------------------------------------------
CLASS USER set_zero.
-----------------------------------------------------------------------------*/
EXP_SYM void USER::set_zero()
{
    memcpy((void*)(this), (void*)(&ZERO_OBJ), sizeof(USER));
}
/*-----------------------------------------------------------------------------
CLASS USER set_from.
-----------------------------------------------------------------------------*/
EXP_SYM void USER::set_from(const nclass &obj)
{
    memcpy((void*)(this), (void*)(&obj), sizeof(USER));
}
/*-----------------------------------------------------------------------------
CLASS USER get_nentity_descriptor.
-----------------------------------------------------------------------------*/
EXP_SYM const vlg::nentity_desc& USER::get_nentity_descriptor() const
{
    return USER_EntityDesc;
}
/*-----------------------------------------------------------------------------
CLASS USER pretty_dump_to_buffer.
-----------------------------------------------------------------------------*/
EXP_SYM size_t USER::pretty_dump_to_buffer(char *buff, bool print_name) const
{
    size_t blen = 0;
    if(print_name) blen += sprintf(&buff[blen], "USER");
    blen += sprintf(&buff[blen], "{");
        if(!is_zero_user_id()) blen += sprintf(&buff[blen], "user_id=%u|", user_id);
        if(!is_zero_name()) blen += sprintf(&buff[blen], "name=[%s]", name);
        if(!is_zero_surname()) blen += sprintf(&buff[blen], "surname=[%s]", surname);
        if(!is_zero_sex()) blen += sprintf(&buff[blen], "sex=%d|", sex);
        if(!is_zero_email()) blen += sprintf(&buff[blen], "email=[%s]", email);
        if(!is_zero_height()) blen += sprintf(&buff[blen], "height=%f|", height);
        if(!is_zero_weight()) blen += sprintf(&buff[blen], "weight=%f|", weight);
        if(!is_zero_active()) blen += sprintf(&buff[blen], "active=%d|", active);
        if(!is_zero_cap()) blen += sprintf(&buff[blen], "cap=%d|", cap);
        if(!is_zero_type()) blen += sprintf(&buff[blen], "type=%c|", type);
        if(!is_zero_main_role()) blen += main_role.pretty_dump_to_buffer(&buff[blen], false);
        if(!is_zero_extra_roles()){
                blen += sprintf(&buff[blen], "extra_roles=[");
                for(int i=0; i<3; i++){
                        blen += extra_roles[i].pretty_dump_to_buffer(&buff[blen], false);
                        if(3>i+1) blen += sprintf(&buff[blen], ",");
                }
                blen += sprintf(&buff[blen], "]");
        }
    blen += sprintf(&buff[blen], "}");
    return blen;
}
/*-----------------------------------------------------------------------------
CLASS USER pretty_dump_to_file.
-----------------------------------------------------------------------------*/
EXP_SYM size_t USER::pretty_dump_to_file(FILE *f, bool print_name) const
{
    size_t blen = 0;
    if(print_name) blen += fprintf(f, "USER");
    blen += fprintf(f, "{");
        if(!is_zero_user_id()) blen += fprintf(f, "user_id=%u|", user_id);
        if(!is_zero_name()) blen += fprintf(f, "name=[%s]", name);
        if(!is_zero_surname()) blen += fprintf(f, "surname=[%s]", surname);
        if(!is_zero_sex()) blen += fprintf(f, "sex=%d|", sex);
        if(!is_zero_email()) blen += fprintf(f, "email=[%s]", email);
        if(!is_zero_height()) blen += fprintf(f, "height=%f|", height);
        if(!is_zero_weight()) blen += fprintf(f, "weight=%f|", weight);
        if(!is_zero_active()) blen += fprintf(f, "active=%d|", active);
        if(!is_zero_cap()) blen += fprintf(f, "cap=%d|", cap);
        if(!is_zero_type()) blen += fprintf(f, "type=%c|", type);
        if(!is_zero_main_role()) blen += main_role.pretty_dump_to_file(f, false);
        if(!is_zero_extra_roles()){
                blen += fprintf(f, "extra_roles=[");
                for(int i=0; i<3; i++){
                        blen += extra_roles[i].pretty_dump_to_file(f, false);
                        if(3>i+1) blen += fprintf(f, ",");
                }
                blen += fprintf(f, "]");
        }
    blen += fprintf(f, "}");
    return blen;
}
/*-----------------------------------------------------------------------------
CLASS USER serialize.
-----------------------------------------------------------------------------*/
EXP_SYM int USER::serialize(vlg::Encode enctyp, const nclass *prev_image, vlg::g_bbuf *obb) const
{
    size_t tlen_offst = obb->position(), tlen = 0;
    switch(enctyp){
        case vlg::Encode_INDEXED_NOT_ZERO:
            obb->advance_pos_write(ENTLN_B_SZ); tlen = obb->position();
            if(!is_zero_user_id()){
                obb->grow(MMBID_B_SZ+4);
                obb->append_ushort(1);
                obb->append(&user_id, 0, 4);
            }
            if(!is_zero_name()){
                size_t fsize = strnlen(name, 130);
                obb->grow(MMBID_B_SZ+FLDLN_B_SZ+fsize);
                obb->append_ushort(2);
                obb->append_uint((unsigned int)fsize);
                obb->append(name, 0, fsize);
            }
            if(!is_zero_surname()){
                size_t fsize = strnlen(surname, 130);
                obb->grow(MMBID_B_SZ+FLDLN_B_SZ+fsize);
                obb->append_ushort(3);
                obb->append_uint((unsigned int)fsize);
                obb->append(surname, 0, fsize);
            }
            if(!is_zero_sex()){
                obb->grow(MMBID_B_SZ+4);
                obb->append_ushort(4);
                obb->append(&sex, 0, 4);
            }
            if(!is_zero_email()){
                size_t fsize = strnlen(email, 65);
                obb->grow(MMBID_B_SZ+FLDLN_B_SZ+fsize);
                obb->append_ushort(5);
                obb->append_uint((unsigned int)fsize);
                obb->append(email, 0, fsize);
            }
            if(!is_zero_height()){
                obb->grow(MMBID_B_SZ+4);
                obb->append_ushort(6);
                obb->append(&height, 0, 4);
            }
            if(!is_zero_weight()){
                obb->grow(MMBID_B_SZ+4);
                obb->append_ushort(7);
                obb->append(&weight, 0, 4);
            }
            if(!is_zero_active()){
                obb->grow(MMBID_B_SZ+1);
                obb->append_ushort(8);
                obb->append(&active, 0, 1);
            }
            if(!is_zero_cap()){
                obb->grow(MMBID_B_SZ+2);
                obb->append_ushort(9);
                obb->append(&cap, 0, 2);
            }
            if(!is_zero_type()){
                obb->grow(MMBID_B_SZ+1);
                obb->append_ushort(10);
                obb->append(&type, 0, 1);
            }
            if(!is_zero_main_role()){
                obb->append_ushort(11);
                main_role.serialize(enctyp, nullptr, obb); }
            if(!is_zero_extra_roles()){
                obb->append_ushort(12);
                size_t alen_offst = obb->position();
                obb->advance_pos_write(ARRAY_B_SZ); size_t alen = obb->position();
                for(int i = 0; i<3; i++){
                    if(!is_zero_extra_roles_idx(i)){
                        obb->append_ushort(i);
                        extra_roles[i].serialize(enctyp, nullptr, obb); }
                    }
                    alen = (obb->position() - alen); obb->put(&alen, alen_offst, ARRAY_B_SZ);
                }
            tlen = (obb->position() - tlen); obb->put(&tlen, tlen_offst, ENTLN_B_SZ);
            break;
        default: return -1;
    }
    return (int)obb->position();
}
}

/*-----------------------------------------------------------------------------
MODEL:smplmdl NEM
-----------------------------------------------------------------------------*/
vlg::nentity_manager NEM_smplmdl;

/*-----------------------------------------------------------------------------
MODEL:smplmdl VERSION
-----------------------------------------------------------------------------*/
extern "C"{
const char* get_mdl_ver_smplmdl()
{
    return "model.smplmdl.ver.0.0.0.compiler.ver.0.0.0.0.date:Mar 13 2018";
}
}


/*-----------------------------------------------------------------------------
MODEL:smplmdl ENTRYPOINT
-----------------------------------------------------------------------------*/
extern "C"{
EXP_SYM vlg::nentity_manager* get_em_smplmdl()
{
    COMMAND_IF_NOT_OK(ROLE_EntityDesc.add_member_desc(ROLE_role_id), exit(1))
    COMMAND_IF_NOT_OK(ROLE_EntityDesc.add_member_desc(ROLE_role_name), exit(1))
    COMMAND_IF_NOT_OK(ROLE_EntityDesc.add_member_desc(ROLE_can_act_as_admin), exit(1))
    COMMAND_IF_NOT_OK(NEM_smplmdl.extend(ROLE_EntityDesc), exit(1))
    COMMAND_IF_NOT_OK(NEM_smplmdl.extend(SEX_EntityDesc), exit(1))
    COMMAND_IF_NOT_OK(NEM_smplmdl.extend(SIMPLE_ENUM_A_EntityDesc), exit(1))
    COMMAND_IF_NOT_OK(NEM_smplmdl.extend(SIMPLE_ENUM_B_EntityDesc), exit(1))
    COMMAND_IF_NOT_OK(NEM_smplmdl.extend(SIMPLE_ENUM_MID_EntityDesc), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_user_id), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_name), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_surname), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_sex), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_email), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_height), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_weight), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_active), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_cap), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_type), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_main_role), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_member_desc(USER_extra_roles), exit(1))
    COMMAND_IF_NOT_OK(KEY_USER_1.add_member_desc(USER_user_id), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_key_desc(KEY_USER_1), exit(1))
    COMMAND_IF_NOT_OK(KEY_USER_2.add_member_desc(USER_surname), exit(1))
    COMMAND_IF_NOT_OK(KEY_USER_2.add_member_desc(USER_name), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_key_desc(KEY_USER_2), exit(1))
    COMMAND_IF_NOT_OK(KEY_USER_3.add_member_desc(USER_email), exit(1))
    COMMAND_IF_NOT_OK(USER_EntityDesc.add_key_desc(KEY_USER_3), exit(1))
    COMMAND_IF_NOT_OK(NEM_smplmdl.extend(USER_EntityDesc), exit(1))
    return &NEM_smplmdl;
}
}


/*-----------------------------------------------------------------------------
MODEL:smplmdl C ENTRYPOINT
-----------------------------------------------------------------------------*/
typedef void *nentity_manager_wr;
extern "C"{
nentity_manager_wr get_c_em_smplmdl()
{
    return (nentity_manager_wr)get_em_smplmdl();
}
}


