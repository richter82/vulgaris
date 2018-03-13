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

#ifndef VLG_C_MDL_H_
#define VLG_C_MDL_H_
#include "vlg.h"

#if defined(__cplusplus)
using namespace vlg;
extern "C" {
#endif

/*
Member section
*/
unsigned short  member_desc_get_member_id(member_desc_wr mdesc);
MemberType      member_desc_get_member_type(member_desc_wr mdesc);
const char      *member_desc_get_member_name(member_desc_wr mdesc);
const char      *member_desc_get_member_description(member_desc_wr mdesc);

/*
Field section
*/
Type            member_desc_get_field_vlg_type(member_desc_wr mdesc);
size_t          member_desc_get_field_offset(member_desc_wr mdesc);
size_t          member_desc_get_field_type_size(member_desc_wr mdesc);
size_t          member_desc_get_field_nmemb(member_desc_wr mdesc);
unsigned int    member_desc_get_field_class_id(member_desc_wr mdesc);
const char      *member_desc_get_field_user_type(member_desc_wr mdesc);
NEntityType     member_desc_get_field_entity_type(member_desc_wr mdesc);

/*
Enum specific
*/
long            member_desc_get_enum_value(member_desc_wr mdesc);

unsigned short  key_desc_get_key_id(key_desc_wr kdesc);
int             key_desc_is_primary(key_desc_wr kdesc);


typedef int(*enum_member_desc_wr)(const member_desc_wr member_descriptor,
                                  void *ud);

unsigned int    entity_desc_get_class_id(nentity_desc_wr edesc);
size_t          entity_desc_get_entity_size(nentity_desc_wr edesc);
NEntityType     entity_desc_get_entity_type(nentity_desc_wr edesc);
const char      *entity_desc_get_entity_namespace(nentity_desc_wr edesc);
const char      *entity_desc_get_entity_name(nentity_desc_wr edesc);

nclass_alloc entity_desc_get_entity_allocation_function(nentity_desc_wr edesc);

unsigned int    entity_desc_get_entity_member_num(nentity_desc_wr edesc);
int             entity_desc_is_persistent(nentity_desc_wr edesc);

const member_desc_wr entity_desc_get_member_desc_by_id(nentity_desc_wr edesc,
                                                       unsigned int member_id);

const member_desc_wr entity_desc_get_member_desc_by_name(nentity_desc_wr edesc,
                                                         const char *member_name);

void entity_desc_enum_member_descriptors(nentity_desc_wr edesc,
                                         enum_member_desc_wr emd_f,
                                         void *ud);

//---delete
void            nclass_delete(nclass_wr obj);

//---memory
void            nclass_retain(nclass_wr obj);
int             nclass_is_collected(nclass_wr obj);
void            nclass_release(nclass_wr obj);

//---getters
unsigned int        nclass_get_class_id(nclass_wr obj);
unsigned int        nclass_get_compiler_version(nclass_wr obj);
size_t              nclass_get_entity_size(nclass_wr obj);
const nclass_wr     nclass_get_zero_object(nclass_wr obj);

void            nclass_copy_to(nclass_wr obj, nclass_wr to_obj);
nclass_wr       nclass_clone(nclass_wr obj);
int             nclass_is_zero(nclass_wr obj);
void            nclass_set_zero(nclass_wr obj);
void            nclass_set_from(nclass_wr obj,
                                const nclass_wr from_obj);

size_t  nclass_get_field_size_by_id(nclass_wr obj,
                                    unsigned int field_id);

size_t  nclass_get_field_size_by_name(nclass_wr obj,
                                      const char *field_name);

void   *nclass_get_field_by_id(nclass_wr obj,
                               unsigned int field_id);

void   *nclass_get_field_by_name(nclass_wr obj,
                                 const char *field_name);

void   *nclass_get_field_by_id_index(nclass_wr obj,
                                     unsigned int field_id,
                                     unsigned int index);

void   *nclass_get_field_by_name_index(nclass_wr obj,
                                       const char *field_name,
                                       unsigned int index);

RetCode    nclass_set_field_by_id(nclass_wr obj,
                                  unsigned int field_id,
                                  const void *ptr,
                                  size_t maxlen);

RetCode    nclass_set_field_by_name(nclass_wr obj,
                                    const char *field_name,
                                    const void *ptr, size_t maxlen);

RetCode    nclass_set_field_by_id_index(nclass_wr obj,
                                        unsigned int field_id,
                                        const void *ptr,
                                        unsigned int index,
                                        size_t maxlen);

RetCode    nclass_set_field_by_name_index(nclass_wr obj,
                                          const char *field_name,
                                          const void *ptr,
                                          unsigned int index,
                                          size_t maxlen);

RetCode    nclass_is_field_zero_by_id(nclass_wr obj,
                                      unsigned int field_id,
                                      int *res);

RetCode    nclass_is_field_zero_by_name(nclass_wr obj,
                                        const char *field_name,
                                        int *res);

RetCode    nclass_is_field_zero_by_id_index(nclass_wr obj,
                                            unsigned int field_id,
                                            unsigned int index,
                                            unsigned int nmenb,
                                            int *res);

RetCode    nclass_is_field_zero_by_name_index(nclass_wr obj,
                                              const char *field_name,
                                              unsigned int index,
                                              unsigned int nmenb,
                                              int *res);

RetCode    nclass_set_field_zero_by_id(nclass_wr obj,
                                       unsigned int field_id);

RetCode    nclass_set_field_zero_by_name(nclass_wr obj,
                                         const char *field_name);

RetCode    nclass_set_field_zero_by_id_index(nclass_wr obj,
                                             unsigned int field_id,
                                             unsigned int index,
                                             unsigned int nmenb);

RetCode    nclass_set_field_zero_by_name_index(nclass_wr obj,
                                               const char *field_name,
                                               unsigned int index,
                                               unsigned int nmenb);

char *nclass_get_term_field_ref_by_plain_idx(nclass_wr obj,
                                             unsigned int plain_idx,
                                             const nentity_manager_wr nem,
                                             const member_desc_wr *member_descriptor);

const nentity_desc_wr nclass_get_entity_descriptor(nclass_wr obj);

size_t nclass_pretty_dump_to_buffer(nclass_wr obj,
                                    char *buffer,
                                    int print_class_name);

size_t nclass_pretty_dump_to_file(nclass_wr obj,
                                  FILE *file,
                                  int print_class_name);

RetCode nclass_primary_key_string_value(nclass_wr obj,
                                        char **newly_alloc_out_pkey);

typedef int(*enum_nentity_desc_wr)(const nentity_desc_wr edesc,
                                   void *ud);

RetCode    entity_manager_get_nentity_descriptor_by_nclassid(nentity_manager_wr nem,
                                                             unsigned int nclass_id,
                                                             nentity_desc_wr const *edesc);

RetCode    entity_manager_get_nentity_descriptor_by_name(nentity_manager_wr nem,
                                                         const char *entity_name,
                                                         nentity_desc_wr const *edesc);

void    entity_manager_enum_nentity_descriptors(nentity_manager_wr nem,
                                                enum_nentity_desc_wr eedf,
                                                void *ud);

void    entity_manager_enum_nenum_descriptors(nentity_manager_wr nem,
                                              enum_nentity_desc_wr eedf,
                                              void *ud);

void    entity_manager_enum_nclass_descriptors(nentity_manager_wr nem,
                                               enum_nentity_desc_wr eedf,
                                               void *ud);

RetCode    entity_manager_new_nclass_instance(nentity_manager_wr nem,
                                              unsigned int nclass_id,
                                              nclass_wr *new_class_obj);

unsigned int     entity_manager_nentity_count(nentity_manager_wr nem);
unsigned int     entity_manager_nenum_count(nentity_manager_wr nem);
unsigned int     entity_manager_nclass_count(nentity_manager_wr nem);

RetCode    entity_manager_extend_with_entity_desc(nentity_manager_wr nem,
                                                  const nentity_desc_wr edesc);

RetCode    entity_manager_extend_with_entity_manager(nentity_manager_wr nem1,
                                                     const nentity_manager_wr nem2);

RetCode    entity_manager_extend_with_model_name(nentity_manager_wr nem,
                                                 const char *model_name);

#if defined(__cplusplus)
}
#endif

#endif