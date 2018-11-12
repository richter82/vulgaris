/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#ifndef VLG_C_MDL_H_
#define VLG_C_MDL_H_
#include "vlg.h"


/************************************************************************
MEMBER DESC
************************************************************************/

unsigned short member_desc_get_member_id(member_desc *mdesc);
MemberType member_desc_get_member_type(member_desc *mdesc);
const char *member_desc_get_member_name(member_desc *mdesc);
const char *member_desc_get_member_description(member_desc *mdesc);
Type member_desc_get_field_vlg_type(member_desc *mdesc);
size_t member_desc_get_field_offset(member_desc *mdesc);
size_t member_desc_get_field_type_size(member_desc *mdesc);
size_t member_desc_get_field_nmemb(member_desc *mdesc);
unsigned int member_desc_get_field_nclass_id(member_desc *mdesc);
const char *member_desc_get_field_user_type(member_desc *mdesc);
NEntityType member_desc_get_field_nentity_type(member_desc *mdesc);
long member_desc_get_nenum_value(member_desc *mdesc);

/************************************************************************
KEY DESC
************************************************************************/

unsigned short key_desc_get_key_id(key_desc *kdesc);
int key_desc_is_primary(key_desc *kdesc);

/************************************************************************
NENTITY DESC
************************************************************************/

typedef int(*enum_member_desc)(const member_desc *mdesc, void *ud);

unsigned int nentity_desc_get_nclass_id(nentity_desc *edesc);
size_t nentity_desc_get_nentity_size(nentity_desc *edesc);
NEntityType nentity_desc_get_nentity_type(nentity_desc *edesc);
const char *nentity_desc_get_nentity_namespace(nentity_desc *edesc);
const char *nentity_desc_get_nentity_name(nentity_desc *edesc);
nclass_alloc nentity_desc_get_nentity_allocation_function(nentity_desc *edesc);
unsigned int nentity_desc_get_nentity_member_num(nentity_desc *edesc);
int nentity_desc_is_persistent(nentity_desc *edesc);
const member_desc *nentity_desc_get_member_desc_by_id(nentity_desc *edesc, unsigned int member_id);
const member_desc *nentity_desc_get_member_desc_by_name(nentity_desc *edesc, const char *member_name);
void nentity_desc_enum_member_descriptors(nentity_desc *edesc, enum_member_desc emd_f, void *ud);

/************************************************************************
NCLASS
************************************************************************/

nclass *own_nclass_get_ptr(own_nclass *obj);
void own_nclass_release(own_nclass *obj);

unsigned int nclass_get_nclass_id(nclass *obj);
unsigned int nclass_get_compiler_version(nclass *obj);
size_t nclass_get_nentity_size(nclass *obj);
const nclass *nclass_get_zero_object(nclass *obj);
void nclass_copy_to(nclass *obj, nclass *to_obj);
own_nclass *nclass_clone(nclass *obj);
int nclass_is_zero(nclass *obj);
void nclass_set_zero(nclass *obj);
void nclass_set_from(nclass *obj, const nclass *from_obj);
size_t nclass_get_field_size_by_id(nclass *obj, unsigned int field_id);
size_t nclass_get_field_size_by_name(nclass *obj, const char *field_name);
char *nclass_get_field_addr_by_id(nclass *obj, unsigned int field_id);
char *nclass_get_field_addr_by_name(nclass *obj, const char *field_name);
char *nclass_get_field_addr_by_id_index(nclass *obj, unsigned int field_id, unsigned int index);
char *nclass_get_field_addr_by_name_index(nclass *obj, const char *field_name, unsigned int index);
char *nclass_get_field_addr_by_column_number(nclass *obj, unsigned int plain_idx, const nentity_manager *nem, const member_desc **member_descriptor);
RetCode nclass_set_field_by_id(nclass *obj, unsigned int field_id, const void *ptr, size_t maxlen);
RetCode nclass_set_field_by_name(nclass *obj, const char *field_name, const void *ptr, size_t maxlen);
RetCode nclass_set_field_by_id_index(nclass *obj, unsigned int field_id, const void *ptr, unsigned int index, size_t maxlen);
RetCode nclass_set_field_by_name_index(nclass *obj, const char *field_name, const void *ptr, unsigned int index, size_t maxlen);
RetCode nclass_is_field_zero_by_id(nclass *obj, unsigned int field_id, int *res);
RetCode nclass_is_field_zero_by_name(nclass *obj, const char *field_name, int *res);
RetCode nclass_is_field_zero_by_id_index(nclass *obj, unsigned int field_id, unsigned int index, unsigned int nmenb, int *res);
RetCode nclass_is_field_zero_by_name_index(nclass *obj, const char *field_name, unsigned int index, unsigned int nmenb, int *res);
RetCode nclass_set_field_zero_by_id(nclass *obj, unsigned int field_id);
RetCode nclass_set_field_zero_by_name(nclass *obj, const char *field_name);
RetCode nclass_set_field_zero_by_id_index(nclass *obj, unsigned int field_id, unsigned int index, unsigned int nmenb);
RetCode nclass_set_field_zero_by_name_index(nclass *obj, const char *field_name, unsigned int index, unsigned int nmenb);
const nentity_desc *nclass_get_nentity_descriptor(nclass *obj);
size_t nclass_pretty_dump_to_buffer(nclass *obj, char *buffer, int print_nclass_name);
size_t nclass_pretty_dump_to_file(nclass *obj, FILE *file, int print_nclass_name);
RetCode nclass_get_primary_key_value_as_string(nclass *obj, char **newly_alloc_out_pkey);

/************************************************************************
NENTITY_MANAGER
************************************************************************/

typedef int(*enum_nentity_desc)(const nentity_desc *edesc, void *ud);

const nentity_desc *nentity_manager_get_nentity_descriptor_by_nclassid(nentity_manager *nem, unsigned int nclass_id);
const nentity_desc *nentity_manager_get_nentity_descriptor_by_name(nentity_manager *nem, const char *nentity_name);
void nentity_manager_enum_nentity_descriptors(nentity_manager *nem, enum_nentity_desc eedf, void *ud);
void nentity_manager_enum_nenum_descriptors(nentity_manager *nem, enum_nentity_desc eedf, void *ud);
void nentity_manager_enum_nclass_descriptors(nentity_manager *nem, enum_nentity_desc eedf, void *ud);
RetCode nentity_manager_new_nclass_instance(nentity_manager *nem, unsigned int nclass_id, own_nclass **new_nclass_obj);
unsigned int nentity_manager_nentity_count(nentity_manager *nem);
unsigned int nentity_manager_nenum_count(nentity_manager *nem);
unsigned int nentity_manager_nclass_count(nentity_manager *nem);
RetCode nentity_manager_extend_with_nentity_desc(nentity_manager *nem, const nentity_desc *edesc);
RetCode nentity_manager_extend_with_nentity_manager(nentity_manager *nem1, const nentity_manager *nem2);
RetCode nentity_manager_extend_with_model_name(nentity_manager *nem, const char *model_name);

#endif
