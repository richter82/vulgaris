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

#include "vlg_c_model.h"
#include "vlg_logger.h"
#include "vlg_model.h"
#include "vlg_globint.h"

namespace vlg {
extern "C" {
    // VLG_MEMBER_DESC

    unsigned short member_desc_get_member_id(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_member_id();
    }

    MemberType member_desc_get_member_type(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_member_type();
    }

    const char *member_desc_get_member_name(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_member_name();
    }

    const char *member_desc_get_member_description(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_member_description();
    }

    Type member_desc_get_field_vlg_type(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_field_vlg_type();
    }

    size_t member_desc_get_field_offset(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_field_offset();
    }

    size_t member_desc_get_field_type_size(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_field_type_size();
    }

    size_t member_desc_get_field_nmemb(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_field_nmemb();
    }

    unsigned int member_desc_get_field_class_id(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_field_nclass_id();
    }

    const char *member_desc_get_field_user_type(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_field_user_type();
    }

    NEntityType member_desc_get_field_entity_type(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_field_nentity_type();
    }

    long member_desc_get_enum_value(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_nenum_value();
    }

    // VLG_KEY_DESC

    unsigned short key_desc_get_key_id(key_desc_wr kdesc)
    {
        return static_cast<key_desc *>(kdesc)->get_key_id();
    }

    int key_desc_is_primary(key_desc_wr kdesc)
    {
        return static_cast<key_desc *>(kdesc)->is_primary() ? 1 : 0;
    }

}

struct enum_member_desc_wr_f_ud {
    enum_member_desc_wr c_f;
    void *ud;
};

void enum_member_desc_wr_f(const member_desc &member_descriptor, void *ud,
                           bool &stop)
{
    enum_member_desc_wr_f_ud *f_ud = static_cast<enum_member_desc_wr_f_ud *>(ud);
    int brk_c = 0;
    f_ud->c_f((const member_desc_wr)&member_descriptor, f_ud->ud, &brk_c);
    stop = brk_c ? true : false;
}

extern "C" {
    // VLG_ENTITY_DESC

    unsigned int entity_desc_get_class_id(entity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nclass_id();
    }

    size_t entity_desc_get_entity_size(entity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nclass_size();
    }

    size_t entity_desc_get_entity_max_align(entity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nclass_max_align();
    }

    NEntityType entity_desc_get_entity_type(entity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nentity_type();
    }

    const char *entity_desc_get_entity_namespace(entity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nentity_namespace();
    }

    const char *entity_desc_get_entity_name(entity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nentity_name();
    }

    alloc_func entity_desc_get_entity_allocation_function(entity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nclass_allocation_function();
    }

    unsigned int entity_desc_get_entity_member_num(entity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nentity_member_num();
    }

    int entity_desc_is_persistent(entity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->is_persistent() ? 1 : 0;
    }

    const member_desc_wr entity_desc_get_member_desc_by_id(entity_desc_wr edesc,
                                                           unsigned int member_id)
    {
        return (const member_desc_wr)static_cast<nentity_desc *>
               (edesc)->get_member_desc_by_id(member_id);
    }

    const member_desc_wr entity_desc_get_member_desc_by_name(entity_desc_wr edesc,
                                                             const char *member_name)
    {
        return (const member_desc_wr)static_cast<nentity_desc *>
               (edesc)->get_member_desc_by_name(member_name);
    }

    void entity_desc_enum_member_descriptors(entity_desc_wr edesc,
                                             enum_member_desc_wr emd_f,
                                             void *ud)
    {
        enum_member_desc_wr_f_ud f_ud = { emd_f, ud };
        static_cast<nentity_desc *>(edesc)->enum_member_descriptors(
            enum_member_desc_wr_f, &f_ud);
    }

    // nclass

    void net_class_delete(net_class_wr obj)
    {
        delete static_cast<nclass *>(obj);
    }

    void net_class_retain(net_class_wr obj)
    {
        nclass *bc = static_cast<nclass *>(obj);
        bc->get_collector().retain(bc);
    }

    int net_class_is_collected(net_class_wr obj)
    {
        nclass *bc = static_cast<nclass *>(obj);
        return bc->get_collector().is_instance_collected(bc) ? 1 : 0;
    }

    void net_class_release(net_class_wr obj)
    {
        nclass *bc = static_cast<nclass *>(obj);
        bc->get_collector().release(bc);
    }

    unsigned int net_class_get_class_id(net_class_wr obj)
    {
        return static_cast<nclass *>(obj)->get_nclass_id();
    }

    unsigned int net_class_get_compiler_version(net_class_wr obj)
    {
        return static_cast<nclass *>(obj)->get_compiler_version();
    }

    size_t net_class_get_entity_size(net_class_wr obj)
    {
        return static_cast<nclass *>(obj)->get_size();
    }

    const net_class_wr net_class_get_zero_object(net_class_wr obj)
    {
        return (const net_class_wr)static_cast<nclass *>(obj)->get_zero_object();
    }

    void net_class_copy_to(net_class_wr obj, net_class_wr to_obj)
    {
        static_cast<nclass *>(obj)->copy_to(static_cast<nclass *>(to_obj));
    }

    net_class_wr net_class_clone(net_class_wr obj)
    {
        return (net_class_wr)static_cast<nclass *>(obj)->clone();
    }

    int net_class_is_zero(net_class_wr obj)
    {
        return static_cast<nclass *>(obj)->is_zero() ? 1 : 0;
    }

    void net_class_set_zero(net_class_wr obj)
    {
        static_cast<nclass *>(obj)->set_zero();
    }

    void net_class_set_from(net_class_wr obj, const net_class_wr from_obj)
    {
        static_cast<nclass *>(obj)->set_from(static_cast<nclass *>(from_obj));
    }

    size_t net_class_get_field_size_by_id(net_class_wr obj,
                                          unsigned int field_id)
    {
        return static_cast<nclass *>(obj)->get_field_size_by_id(field_id);
    }

    size_t net_class_get_field_size_by_name(net_class_wr obj,
                                            const char *field_name)
    {
        return static_cast<nclass *>(obj)->get_field_size_by_name(field_name);
    }

    void *net_class_get_field_by_id(net_class_wr obj,
                                    unsigned int field_id)
    {
        return static_cast<nclass *>(obj)->get_field_by_id(field_id);
    }

    void *net_class_get_field_by_name(net_class_wr obj,
                                      const char *field_name)
    {
        return static_cast<nclass *>(obj)->get_field_by_name(field_name);
    }

    void *net_class_get_field_by_id_index(net_class_wr obj,
                                          unsigned int field_id,
                                          unsigned int index)
    {
        return static_cast<nclass *>(obj)->get_field_by_id_index(field_id,
                                                                 index);
    }

    void *net_class_get_field_by_name_index(net_class_wr obj,
                                            const char *field_name,
                                            unsigned int index)
    {
        return static_cast<nclass *>(obj)->get_field_by_name_index(field_name,
                                                                   index);
    }

    RetCode net_class_set_field_by_id(net_class_wr obj,
                                      unsigned int field_id,
                                      const void *ptr,
                                      size_t maxlen)
    {
        return static_cast<nclass *>(obj)->set_field_by_id(field_id,
                                                           ptr,
                                                           maxlen);
    }

    RetCode net_class_set_field_by_name(net_class_wr obj,
                                        const char *field_name,
                                        const void *ptr,
                                        size_t maxlen)
    {
        return static_cast<nclass *>(obj)->set_field_by_name(field_name, ptr,
                                                             maxlen);
    }

    RetCode net_class_set_field_by_id_index(net_class_wr obj,
                                            unsigned int field_id,
                                            const void *ptr, unsigned int index,
                                            size_t maxlen)
    {
        return static_cast<nclass *>(obj)->set_field_by_id_index(field_id,
                                                                 ptr,
                                                                 index,
                                                                 maxlen);
    }

    RetCode net_class_set_field_by_name_index(net_class_wr obj,
                                              const char *field_name,
                                              const void *ptr,
                                              unsigned int index,
                                              size_t maxlen)
    {
        return static_cast<nclass *>(obj)->set_field_by_name_index(field_name,
                                                                   ptr,
                                                                   index,
                                                                   maxlen);
    }

    RetCode net_class_is_field_zero_by_id(net_class_wr obj,
                                          unsigned int field_id,
                                          int *res)
    {
        bool b_res = false;
        RetCode r_res = static_cast<nclass *>(obj)->is_field_zero_by_id(field_id,
                                                                        b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode net_class_is_field_zero_by_name(net_class_wr obj,
                                            const char *field_name,
                                            int *res)
    {
        bool b_res = false;
        RetCode r_res = static_cast<nclass *>(obj)->is_field_zero_by_name(
                            field_name,
                            b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode net_class_is_field_zero_by_id_index(net_class_wr obj,
                                                unsigned int field_id,
                                                unsigned int index,
                                                unsigned int nmenb,
                                                int *res)
    {
        bool b_res = false;
        RetCode r_res = static_cast<nclass *>(obj)->is_field_zero_by_id_index(field_id,
                                                                              index,
                                                                              nmenb,
                                                                              b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode net_class_is_field_zero_by_name_index(net_class_wr obj,
                                                  const char *field_name,
                                                  unsigned int index,
                                                  unsigned int nmenb,
                                                  int *res)
    {
        bool b_res = false;
        RetCode r_res = static_cast<nclass *>(obj)->is_field_zero_by_name_index(
                            field_name, index, nmenb, b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode net_class_set_field_zero_by_id(net_class_wr obj,
                                           unsigned int field_id)
    {
        return static_cast<nclass *>(obj)->set_field_zero_by_id(field_id);
    }

    RetCode net_class_set_field_zero_by_name(net_class_wr obj,
                                             const char *field_name)
    {
        return static_cast<nclass *>(obj)->set_field_zero_by_name(field_name);
    }

    RetCode net_class_set_field_zero_by_id_index(net_class_wr obj,
                                                 unsigned int field_id,
                                                 unsigned int index,
                                                 unsigned int nmenb)
    {
        return static_cast<nclass *>(obj)->set_field_zero_by_id_index(field_id,
                                                                      index,
                                                                      nmenb);
    }

    RetCode net_class_set_field_zero_by_name_index(net_class_wr obj,
                                                   const char *field_name,
                                                   unsigned int index,
                                                   unsigned int nmenb)
    {
        return static_cast<nclass *>(obj)->set_field_zero_by_name_index(field_name,
                                                                        index,
                                                                        nmenb);
    }

    char *net_class_get_term_field_ref_by_plain_idx(net_class_wr obj,
                                                    unsigned int plain_idx,
                                                    const entity_manager_wr nem,
                                                    const member_desc_wr *member_descriptor)
    {
        return static_cast<nclass *>(obj)->get_term_field_ref_by_plain_idx(plain_idx,
                                                                           *static_cast<const nentity_manager *>(nem),
                                                                           (const member_desc **)member_descriptor);
    }

    const entity_desc_wr net_class_get_entity_descriptor(net_class_wr obj)
    {
        return (entity_desc_wr)static_cast<nclass *>(obj)->get_nentity_descriptor();
    }

    size_t net_class_pretty_dump_to_buffer(net_class_wr obj,
                                           char *buffer,
                                           int print_class_name)
    {
        return static_cast<nclass *>(obj)->pretty_dump_to_buffer(buffer,
                                                                 print_class_name ? true : false);
    }

    size_t net_class_pretty_dump_to_file(net_class_wr obj,
                                         FILE *file,
                                         int print_class_name)
    {
        return static_cast<nclass *>(obj)->pretty_dump_to_file(file,
                                                               print_class_name ? true : false);
    }

    RetCode net_class_primary_key_string_value(net_class_wr obj,
                                               ascii_str_wr out_str)
    {
        return static_cast<nclass *>(obj)->primary_key_string_value(
                   static_cast<ascii_string *>(out_str));
    }
}

struct enum_entity_desc_wr_f_ud {
    enum_entity_desc_wr c_f;
    void *ud;
};

void enum_entity_desc_wr_f(const nentity_desc &entity_descriptor,
                           void *ud,
                           bool &stop)
{
    enum_entity_desc_wr_f_ud *f_ud = static_cast<enum_entity_desc_wr_f_ud *>(ud);
    int brk_c = 0;
    f_ud->c_f((const entity_desc_wr)&entity_descriptor, f_ud->ud, &brk_c);
    stop = brk_c ? true : false;
}

extern "C" {

    // VLG_ENTITY_MANAGER
    RetCode entity_manager_get_entity_descriptor_by_classid(entity_manager_wr emng,
                                                            unsigned int nclass_id,
                                                            entity_desc_wr const *edesc)
    {
        return static_cast<nentity_manager *>(emng)->get_nentity_descriptor(nclass_id,
                                                                            (nentity_desc const **)edesc);
    }

    RetCode entity_manager_get_entity_descriptor_by_name(entity_manager_wr emng,
                                                         const char *entity_name,
                                                         entity_desc_wr const *edesc)
    {
        return static_cast<nentity_manager *>(emng)->get_nentity_descriptor(entity_name,
                                                                            (nentity_desc const **)edesc);
    }

    void entity_manager_enum_entity_descriptors(entity_manager_wr emng,
                                                enum_entity_desc_wr eedf,
                                                void *ud)
    {
        enum_entity_desc_wr_f_ud f_ud = { eedf, ud };
        static_cast<nentity_manager *>(emng)->enum_nentity_descriptors(
            enum_entity_desc_wr_f, &f_ud);
    }

    RetCode entity_manager_extend_with_model_name(entity_manager_wr emng,
                                                  const char *model_name)
    {
        return static_cast<nentity_manager *>(emng)->extend(model_name);
    }

    void entity_manager_enum_enum_descriptors(entity_manager_wr emng,
                                              enum_entity_desc_wr eedf,
                                              void *ud)
    {
        enum_entity_desc_wr_f_ud f_ud = { eedf, ud };
        static_cast<nentity_manager *>(emng)->enum_nenum_descriptors(
            enum_entity_desc_wr_f, &f_ud);
    }

    void entity_manager_enum_class_descriptors(entity_manager_wr emng,
                                               enum_entity_desc_wr eedf,
                                               void *ud)
    {
        enum_entity_desc_wr_f_ud f_ud = { eedf, ud };
        static_cast<nentity_manager *>(emng)->enum_nclass_descriptors(
            enum_entity_desc_wr_f, &f_ud);
    }

    RetCode entity_manager_new_class_instance(entity_manager_wr emng,
                                              unsigned int nclass_id,
                                              net_class_wr *new_class_obj)
    {
        return static_cast<nentity_manager *>(emng)->new_nclass_instance(nclass_id,
                                                                         (nclass **)new_class_obj);
    }

    unsigned int entity_manager_entity_count(entity_manager_wr emng)
    {
        return static_cast<nentity_manager *>(emng)->nentity_count();
    }

    unsigned int entity_manager_enum_count(entity_manager_wr emng)
    {
        return static_cast<nentity_manager *>(emng)->nenum_count();
    }

    unsigned int entity_manager_class_count(entity_manager_wr emng)
    {
        return static_cast<nentity_manager *>(emng)->nclass_count();
    }

    const char *entity_manager_get_class_name(entity_manager_wr emng,
                                              unsigned int nclass_id)
    {
        return static_cast<nentity_manager *>(emng)->get_nclass_name(nclass_id);
    }

    RetCode entity_manager_extend_with_entity_desc(entity_manager_wr emng,
                                                   const entity_desc_wr edesc)
    {
        return static_cast<nentity_manager *>(emng)->extend((const nentity_desc *)edesc);
    }

    RetCode entity_manager_extend_with_entity_manager(entity_manager_wr emng,
                                                      entity_manager_wr nem)
    {
        return static_cast<nentity_manager *>(emng)->extend((nentity_manager *)nem);
    }

    // VLG_LOGGER

    size_t log_pln(vlg_logger_wr l, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->pln(fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_trc(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->trc(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_dbg(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->dbg(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_inf(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->inf(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_wrn(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->wrn(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_err(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->err(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_cri(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->cri(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_fat(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->fat(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_trc_class(vlg_logger_wr l, uint32_t id, const net_class_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->trc_class(id,
                                                               (const nclass *)obj, print_class_name ? true : false,
                                                               fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_dbg_class(vlg_logger_wr l, uint32_t id, const net_class_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->dbg_class(id,
                                                               (const nclass *)obj, print_class_name ? true : false,
                                                               fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_inf_class(vlg_logger_wr l, uint32_t id, const net_class_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->inf_class(id,
                                                               (const nclass *)obj, print_class_name ? true : false,
                                                               fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_wrn_class(vlg_logger_wr l, uint32_t id, const net_class_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->wrn_class(id,
                                                               (const nclass *)obj, print_class_name ? true : false,
                                                               fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_err_class(vlg_logger_wr l, uint32_t id, const net_class_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->err_class(id,
                                                               (const nclass *)obj, print_class_name ? true : false,
                                                               fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_cri_class(vlg_logger_wr l, uint32_t id, const net_class_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->cri_class(id,
                                                               (const nclass *)obj, print_class_name ? true : false,
                                                               fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_fat_class(vlg_logger_wr l, uint32_t id, const net_class_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<nclass_logger *>(l)->fat_class(id,
                                                               (const nclass *)obj, print_class_name ? true : false,
                                                               fmt, args);
        va_end(args);
        return sz;
    }

    vlg_logger_wr get_vlg_logger_wr(const char *logger_name)
    {
        return (vlg_logger_wr)vlg::get_nclass_logger(logger_name);
    }

}
}
