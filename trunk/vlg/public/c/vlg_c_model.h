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
NEntityType      member_desc_get_field_entity_type(member_desc_wr mdesc);

/*
Enum specific
*/
long            member_desc_get_enum_value(member_desc_wr mdesc);

unsigned short  key_desc_get_key_id(key_desc_wr kdesc);
int             key_desc_is_primary(key_desc_wr kdesc);


typedef void(*enum_member_desc_wr)(const member_desc_wr member_descriptor,
                                   void *ud,
                                   int *stop);

unsigned int    entity_desc_get_class_id(entity_desc_wr edesc);
size_t          entity_desc_get_entity_size(entity_desc_wr edesc);
size_t          entity_desc_get_entity_max_align(entity_desc_wr edesc);
NEntityType      entity_desc_get_entity_type(entity_desc_wr edesc);
const char      *entity_desc_get_entity_namespace(entity_desc_wr edesc);
const char      *entity_desc_get_entity_name(entity_desc_wr edesc);

alloc_func entity_desc_get_entity_allocation_function(entity_desc_wr edesc);

unsigned int    entity_desc_get_entity_member_num(entity_desc_wr edesc);
int             entity_desc_is_persistent(entity_desc_wr edesc);

const member_desc_wr
entity_desc_get_member_desc_by_id(entity_desc_wr edesc,
                                  unsigned int member_id);

const member_desc_wr
entity_desc_get_member_desc_by_name(entity_desc_wr edesc,
                                    const char *member_name);

void
entity_desc_enum_member_descriptors(entity_desc_wr edesc,
                                    enum_member_desc_wr emd_f,
                                    void *ud);

//---delete
void            net_class_delete(net_class_wr obj);

//---memory
void            net_class_retain(net_class_wr obj);
int             net_class_is_collected(net_class_wr obj);
void            net_class_release(net_class_wr obj);

//---getters
unsigned int        net_class_get_class_id(net_class_wr obj);
unsigned int        net_class_get_compiler_version(net_class_wr obj);
size_t              net_class_get_entity_size(net_class_wr obj);
const net_class_wr  net_class_get_zero_object(net_class_wr obj);

void            net_class_copy_to(net_class_wr obj, net_class_wr to_obj);
net_class_wr    net_class_clone(net_class_wr obj);
int             net_class_is_zero(net_class_wr obj);
void            net_class_set_zero(net_class_wr obj);
void            net_class_set_from(net_class_wr obj,
                                   const net_class_wr from_obj);

size_t  net_class_get_field_size_by_id(net_class_wr obj,
                                       unsigned int field_id);

size_t  net_class_get_field_size_by_name(net_class_wr obj,
                                         const char *field_name);

void   *net_class_get_field_by_id(net_class_wr obj,
                                  unsigned int field_id);

void   *net_class_get_field_by_name(net_class_wr obj,
                                    const char *field_name);

void   *net_class_get_field_by_id_index(net_class_wr obj,
                                        unsigned int field_id,
                                        unsigned int index);

void   *net_class_get_field_by_name_index(net_class_wr obj,
                                          const char *field_name,
                                          unsigned int index);

RetCode    net_class_set_field_by_id(net_class_wr obj,
                                     unsigned int field_id,
                                     const void *ptr,
                                     size_t maxlen);

RetCode    net_class_set_field_by_name(net_class_wr obj,
                                       const char *field_name,
                                       const void *ptr, size_t maxlen);

RetCode    net_class_set_field_by_id_index(net_class_wr obj,
                                           unsigned int field_id,
                                           const void *ptr,
                                           unsigned int index,
                                           size_t maxlen);

RetCode    net_class_set_field_by_name_index(net_class_wr obj,
                                             const char *field_name,
                                             const void *ptr,
                                             unsigned int index,
                                             size_t maxlen);

RetCode    net_class_is_field_zero_by_id(net_class_wr obj,
                                         unsigned int field_id,
                                         int *res);

RetCode    net_class_is_field_zero_by_name(net_class_wr obj,
                                           const char *field_name,
                                           int *res);

RetCode    net_class_is_field_zero_by_id_index(net_class_wr obj,
                                               unsigned int field_id,
                                               unsigned int index,
                                               unsigned int nmenb,
                                               int *res);

RetCode    net_class_is_field_zero_by_name_index(net_class_wr obj,
                                                 const char *field_name,
                                                 unsigned int index,
                                                 unsigned int nmenb,
                                                 int *res);

RetCode    net_class_set_field_zero_by_id(net_class_wr obj,
                                          unsigned int field_id);

RetCode    net_class_set_field_zero_by_name(net_class_wr obj,
                                            const char *field_name);

RetCode    net_class_set_field_zero_by_id_index(net_class_wr obj,
                                                unsigned int field_id,
                                                unsigned int index,
                                                unsigned int nmenb);

RetCode    net_class_set_field_zero_by_name_index(net_class_wr obj,
                                                  const char *field_name,
                                                  unsigned int index,
                                                  unsigned int nmenb);

char *net_class_get_term_field_ref_by_plain_idx(net_class_wr obj,
                                                unsigned int plain_idx,
                                                const entity_manager_wr nem,
                                                const member_desc_wr *member_descriptor);

const entity_desc_wr net_class_get_entity_descriptor(net_class_wr obj);

size_t net_class_pretty_dump_to_buffer(net_class_wr obj,
                                       char *buffer,
                                       int print_class_name);

size_t net_class_pretty_dump_to_file(net_class_wr obj,
                                     FILE *file,
                                     int print_class_name);

RetCode net_class_primary_key_string_value(net_class_wr obj,
                                           ascii_str_wr out_str);

typedef void(*enum_entity_desc_wr)(const entity_desc_wr edesc,
                                   void *ud,
                                   int *stop);

RetCode    entity_manager_get_entity_descriptor_by_classid(entity_manager_wr emng,
                                                           unsigned int nclass_id,
                                                           entity_desc_wr const *edesc);

RetCode    entity_manager_get_entity_descriptor_by_name(entity_manager_wr emng,
                                                        const char *entity_name,
                                                        entity_desc_wr const *edesc);

void    entity_manager_enum_entity_descriptors(entity_manager_wr emng,
                                               enum_entity_desc_wr eedf,
                                               void *ud);

void    entity_manager_enum_enum_descriptors(entity_manager_wr emng,
                                             enum_entity_desc_wr eedf,
                                             void *ud);

void    entity_manager_enum_struct_descriptors(entity_manager_wr emng,
                                               enum_entity_desc_wr eedf,
                                               void *ud);

void    entity_manager_enum_class_descriptors(entity_manager_wr emng,
                                              enum_entity_desc_wr eedf,
                                              void *ud);

RetCode    entity_manager_new_class_instance(entity_manager_wr emng,
                                             unsigned int nclass_id,
                                             net_class_wr *new_class_obj);

unsigned int     entity_manager_entity_count(entity_manager_wr emng);
unsigned int     entity_manager_enum_count(entity_manager_wr emng);
unsigned int     entity_manager_struct_count(entity_manager_wr emng);
unsigned int     entity_manager_class_count(entity_manager_wr emng);

const char      *entity_manager_get_class_name(entity_manager_wr emng,
                                               unsigned int nclass_id);

RetCode    entity_manager_extend_with_entity_desc(entity_manager_wr emng,
                                                  const entity_desc_wr edesc);

RetCode    entity_manager_extend_with_entity_manager(entity_manager_wr emng,
                                                     entity_manager_wr nem);

RetCode    entity_manager_extend_with_model_name(entity_manager_wr emng,
                                                 const char *model_name);

#if defined(__cplusplus)
}
#endif

#endif