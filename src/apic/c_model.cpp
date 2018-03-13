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
#include "glob.h"

namespace vlg {
extern "C" {
    // VLG_MEMBER_DESC

    unsigned short member_desc_get_member_id(member_desc_wr mdesc)
    {
        return static_cast<member_desc *>(mdesc)->get_id();
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
        return static_cast<key_desc *>(kdesc)->get_id();
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

bool enum_member_desc_wr_f(const member_desc &member_descriptor, void *ud)
{
    enum_member_desc_wr_f_ud *f_ud = static_cast<enum_member_desc_wr_f_ud *>(ud);
    int brk_c = f_ud->c_f((const member_desc_wr)&member_descriptor, f_ud->ud);
    return brk_c ? true : false;
}

extern "C" {
    // VLG_ENTITY_DESC

    unsigned int entity_desc_get_class_id(nentity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nclass_id();
    }

    size_t entity_desc_get_entity_size(nentity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nclass_size();
    }

    NEntityType entity_desc_get_entity_type(nentity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nentity_type();
    }

    const char *entity_desc_get_entity_namespace(nentity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nentity_namespace();
    }

    const char *entity_desc_get_entity_name(nentity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nentity_name();
    }

    nclass_alloc entity_desc_get_entity_allocation_function(nentity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nclass_allocation_function();
    }

    unsigned int entity_desc_get_entity_member_num(nentity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->get_nentity_member_count();
    }

    int entity_desc_is_persistent(nentity_desc_wr edesc)
    {
        return static_cast<nentity_desc *>(edesc)->is_persistent() ? 1 : 0;
    }

    const member_desc_wr entity_desc_get_member_desc_by_id(nentity_desc_wr edesc,
                                                           unsigned int member_id)
    {
        return (const member_desc_wr)static_cast<nentity_desc *>
               (edesc)->get_member_desc_by_id(member_id);
    }

    const member_desc_wr entity_desc_get_member_desc_by_name(nentity_desc_wr edesc,
                                                             const char *member_name)
    {
        return (const member_desc_wr)static_cast<nentity_desc *>
               (edesc)->get_member_desc_by_name(member_name);
    }

    void entity_desc_enum_member_descriptors(nentity_desc_wr edesc,
                                             enum_member_desc_wr emd_f,
                                             void *ud)
    {
        enum_member_desc_wr_f_ud f_ud = { emd_f, ud };
        static_cast<nentity_desc *>(edesc)->enum_member_descriptors(enum_member_desc_wr_f, &f_ud);
    }

    // nclass

    void nclass_delete(nclass_wr obj)
    {
        delete static_cast<nclass *>(obj);
    }

    void nclass_retain(nclass_wr obj)
    {
        nclass *bc = static_cast<nclass *>(obj);
        //bc->get_collector().retain(bc);
    }

    //int nclass_is_collected(nclass_wr obj)
    //{
    //    nclass *bc = static_cast<nclass *>(obj);
    //    //return bc->get_collector().is_instance_collected(bc) ? 1 : 0;
    //}

    void nclass_release(nclass_wr obj)
    {
        nclass *bc = static_cast<nclass *>(obj);
        //bc->get_collector().release(bc);
    }

    unsigned int nclass_get_class_id(nclass_wr obj)
    {
        return static_cast<nclass *>(obj)->get_id();
    }

    unsigned int nclass_get_compiler_version(nclass_wr obj)
    {
        return static_cast<nclass *>(obj)->get_compiler_version();
    }

    size_t nclass_get_entity_size(nclass_wr obj)
    {
        return static_cast<nclass *>(obj)->get_size();
    }

    const nclass_wr nclass_get_zero_object(nclass_wr obj)
    {
        return (const nclass_wr)&static_cast<nclass *>(obj)->get_zero_object();
    }

    void nclass_copy_to(nclass_wr obj, nclass_wr to_obj)
    {
        static_cast<nclass *>(obj)->copy_to(*static_cast<nclass *>(to_obj));
    }

    nclass_wr nclass_clone(nclass_wr obj)
    {
        //return (nclass_wr)static_cast<nclass *>(obj)->clone();
        return nullptr;
    }

    int nclass_is_zero(nclass_wr obj)
    {
        return static_cast<nclass *>(obj)->is_zero() ? 1 : 0;
    }

    void nclass_set_zero(nclass_wr obj)
    {
        static_cast<nclass *>(obj)->set_zero();
    }

    void nclass_set_from(nclass_wr obj, const nclass_wr from_obj)
    {
        static_cast<nclass *>(obj)->set_from(*static_cast<nclass *>(from_obj));
    }

    size_t nclass_get_field_size_by_id(nclass_wr obj,
                                       unsigned int field_id)
    {
        return static_cast<nclass *>(obj)->get_field_size_by_id(field_id);
    }

    size_t nclass_get_field_size_by_name(nclass_wr obj,
                                         const char *field_name)
    {
        return static_cast<nclass *>(obj)->get_field_size_by_name(field_name);
    }

    void *nclass_get_field_by_id(nclass_wr obj,
                                 unsigned int field_id)
    {
        return static_cast<nclass *>(obj)->get_field_address_by_id(field_id);
    }

    void *nclass_get_field_by_name(nclass_wr obj,
                                   const char *field_name)
    {
        return static_cast<nclass *>(obj)->get_field_address_by_name(field_name);
    }

    void *nclass_get_field_by_id_index(nclass_wr obj,
                                       unsigned int field_id,
                                       unsigned int index)
    {
        return static_cast<nclass *>(obj)->get_field_address_by_id_and_index(field_id,
                                                                             index);
    }

    void *nclass_get_field_by_name_index(nclass_wr obj,
                                         const char *field_name,
                                         unsigned int index)
    {
        return static_cast<nclass *>(obj)->get_field_address_by_name_and_index(field_name,
                                                                               index);
    }

    RetCode nclass_set_field_by_id(nclass_wr obj,
                                   unsigned int field_id,
                                   const void *ptr,
                                   size_t maxlen)
    {
        return static_cast<nclass *>(obj)->set_field_by_id(field_id,
                                                           ptr,
                                                           maxlen);
    }

    RetCode nclass_set_field_by_name(nclass_wr obj,
                                     const char *field_name,
                                     const void *ptr,
                                     size_t maxlen)
    {
        return static_cast<nclass *>(obj)->set_field_by_name(field_name, ptr,
                                                             maxlen);
    }

    RetCode nclass_set_field_by_id_index(nclass_wr obj,
                                         unsigned int field_id,
                                         const void *ptr, unsigned int index,
                                         size_t maxlen)
    {
        return static_cast<nclass *>(obj)->set_field_by_id_index(field_id,
                                                                 ptr,
                                                                 index,
                                                                 maxlen);
    }

    RetCode nclass_set_field_by_name_index(nclass_wr obj,
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

    RetCode nclass_is_field_zero_by_id(nclass_wr obj,
                                       unsigned int field_id,
                                       int *res)
    {
        bool b_res = false;
        RetCode r_res = static_cast<nclass *>(obj)->is_field_zero_by_id(field_id,
                                                                        b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode nclass_is_field_zero_by_name(nclass_wr obj,
                                         const char *field_name,
                                         int *res)
    {
        bool b_res = false;
        RetCode r_res = static_cast<nclass *>(obj)->is_field_zero_by_name(field_name,
                                                                          b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode nclass_is_field_zero_by_id_index(nclass_wr obj,
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

    RetCode nclass_is_field_zero_by_name_index(nclass_wr obj,
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

    RetCode nclass_set_field_zero_by_id(nclass_wr obj,
                                        unsigned int field_id)
    {
        return static_cast<nclass *>(obj)->set_field_zero_by_id(field_id);
    }

    RetCode nclass_set_field_zero_by_name(nclass_wr obj,
                                          const char *field_name)
    {
        return static_cast<nclass *>(obj)->set_field_zero_by_name(field_name);
    }

    RetCode nclass_set_field_zero_by_id_index(nclass_wr obj,
                                              unsigned int field_id,
                                              unsigned int index,
                                              unsigned int nmenb)
    {
        return static_cast<nclass *>(obj)->set_field_zero_by_id_index(field_id,
                                                                      index,
                                                                      nmenb);
    }

    RetCode nclass_set_field_zero_by_name_index(nclass_wr obj,
                                                const char *field_name,
                                                unsigned int index,
                                                unsigned int nmenb)
    {
        return static_cast<nclass *>(obj)->set_field_zero_by_name_index(field_name,
                                                                        index,
                                                                        nmenb);
    }

    char *nclass_get_term_field_ref_by_plain_idx(nclass_wr obj,
                                                 unsigned int plain_idx,
                                                 const nentity_manager_wr nem,
                                                 const member_desc_wr *member_descriptor)
    {
        return static_cast<nclass *>(obj)->get_field_address_by_column_number(plain_idx,
                                                                              *static_cast<const nentity_manager *>(nem),
                                                                              (const member_desc **)member_descriptor);
    }

    const nentity_desc_wr nclass_get_entity_descriptor(nclass_wr obj)
    {
        return (nentity_desc_wr)&static_cast<nclass *>(obj)->get_nentity_descriptor();
    }

    size_t nclass_pretty_dump_to_buffer(nclass_wr obj,
                                        char *buffer,
                                        int print_class_name)
    {
        return static_cast<nclass *>(obj)->pretty_dump_to_buffer(buffer,
                                                                 print_class_name ? true : false);
    }

    size_t nclass_pretty_dump_to_file(nclass_wr obj,
                                      FILE *file,
                                      int print_class_name)
    {
        return static_cast<nclass *>(obj)->pretty_dump_to_file(file,
                                                               print_class_name ? true : false);
    }

    RetCode nclass_primary_key_string_value(nclass_wr obj,
                                            char **newly_alloc_out_pkey)
    {
        std::unique_ptr<char> pkey;
        RetCode r_res = static_cast<nclass *>(obj)->get_primary_key_value_as_string(pkey);
        if(newly_alloc_out_pkey && !r_res) {
            *newly_alloc_out_pkey = strdup(pkey.get());
        }
        return r_res;
    }
}

struct enum_entity_desc_wr_f_ud {
    enum_nentity_desc_wr c_f;
    void *ud;
};

bool enum_nentity_desc_wr_f(const nentity_desc &entity_descriptor, void *ud)
{
    enum_entity_desc_wr_f_ud *f_ud = static_cast<enum_entity_desc_wr_f_ud *>(ud);
    int brk_c = 0;
    return f_ud->c_f((const nentity_desc_wr)&entity_descriptor, f_ud->ud) ? true : false;
}

extern "C" {

    // VLG_ENTITY_MANAGER
    const nentity_desc_wr entity_manager_get_entity_descriptor_by_classid(nentity_manager_wr nem, unsigned int nclass_id)
    {
        return (nentity_desc_wr)static_cast<nentity_manager *>(nem)->get_nentity_descriptor(nclass_id);
    }

    const nentity_desc_wr entity_manager_get_entity_descriptor_by_name(nentity_manager_wr nem, const char *entity_name)
    {
        return (nentity_desc_wr)static_cast<nentity_manager *>(nem)->get_nentity_descriptor(entity_name);
    }

    void entity_manager_enum_entity_descriptors(nentity_manager_wr nem, enum_nentity_desc_wr eedf, void *ud)
    {
        enum_entity_desc_wr_f_ud f_ud = { eedf, ud };
        static_cast<nentity_manager *>(nem)->enum_nentity_descriptors(enum_nentity_desc_wr_f, &f_ud);
    }

    RetCode entity_manager_extend_with_model_name(nentity_manager_wr nem, const char *model_name)
    {
        return static_cast<nentity_manager *>(nem)->extend(model_name);
    }

    void entity_manager_enum_enum_descriptors(nentity_manager_wr nem, enum_nentity_desc_wr eedf, void *ud)
    {
        enum_entity_desc_wr_f_ud f_ud = { eedf, ud };
        static_cast<nentity_manager *>(nem)->enum_nenum_descriptors(enum_nentity_desc_wr_f, &f_ud);
    }

    void entity_manager_enum_class_descriptors(nentity_manager_wr nem, enum_nentity_desc_wr eedf, void *ud)
    {
        enum_entity_desc_wr_f_ud f_ud = { eedf, ud };
        static_cast<nentity_manager *>(nem)->enum_nclass_descriptors(enum_nentity_desc_wr_f, &f_ud);
    }

    RetCode entity_manager_new_class_instance(nentity_manager_wr nem,
                                              unsigned int nclass_id,
                                              nclass_wr *new_class_obj)
    {
        return static_cast<nentity_manager *>(nem)->new_nclass_instance(nclass_id,
                                                                        (nclass **)new_class_obj);
    }

    unsigned int entity_manager_entity_count(nentity_manager_wr nem)
    {
        return static_cast<nentity_manager *>(nem)->nentity_count();
    }

    unsigned int entity_manager_enum_count(nentity_manager_wr nem)
    {
        return static_cast<nentity_manager *>(nem)->nenum_count();
    }

    unsigned int entity_manager_class_count(nentity_manager_wr nem)
    {
        return static_cast<nentity_manager *>(nem)->nclass_count();
    }

    RetCode entity_manager_extend_with_entity_desc(nentity_manager_wr nem,
                                                   const nentity_desc_wr edesc)
    {
        return static_cast<nentity_manager *>(nem)->extend(*(const nentity_desc *)edesc);
    }

    RetCode entity_manager_extend_with_entity_manager(nentity_manager_wr nem1,
                                                      const nentity_manager_wr nem2)
    {
        return static_cast<nentity_manager *>(nem1)->extend(*(nentity_manager *)nem2);
    }

    // VLG_LOGGER

    size_t log_pln(vlg_logger_wr l, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->pln(fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_trc(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->trc(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_dbg(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->dbg(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_inf(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->inf(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_wrn(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->wrn(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_err(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->err(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_cri(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->cri(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_fat(vlg_logger_wr l, uint32_t id, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->fat(id, fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_trc_class(vlg_logger_wr l, uint32_t id, const nclass_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->trc_nclass(id,
                                                         (const nclass *)obj, print_class_name ? true : false,
                                                         fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_dbg_class(vlg_logger_wr l, uint32_t id, const nclass_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->dbg_nclass(id,
                                                         (const nclass *)obj, print_class_name ? true : false,
                                                         fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_inf_class(vlg_logger_wr l, uint32_t id, const nclass_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->inf_nclass(id,
                                                         (const nclass *)obj, print_class_name ? true : false,
                                                         fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_wrn_class(vlg_logger_wr l, uint32_t id, const nclass_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->wrn_nclass(id,
                                                         (const nclass *)obj, print_class_name ? true : false,
                                                         fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_err_class(vlg_logger_wr l, uint32_t id, const nclass_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->err_nclass(id,
                                                         (const nclass *)obj, print_class_name ? true : false,
                                                         fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_cri_class(vlg_logger_wr l, uint32_t id, const nclass_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->cri_nclass(id,
                                                         (const nclass *)obj, print_class_name ? true : false,
                                                         fmt, args);
        va_end(args);
        return sz;
    }

    size_t log_fat_class(vlg_logger_wr l, uint32_t id, const nclass_wr obj,
                         int print_class_name,
                         const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t sz = static_cast<logger *>(l)->fat_nclass(id,
                                                         (const nclass *)obj, print_class_name ? true : false,
                                                         fmt, args);
        va_end(args);
        return sz;
    }

    vlg_logger_wr get_vlg_logger_wr(const char *logger_name)
    {
        return (vlg_logger_wr)vlg::logger::get_logger(logger_name);
    }

}
}
