/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "glob.h"
using namespace vlg;

extern "C" {
    typedef struct shr_nclass shr_nclass;
    typedef struct own_nclass own_nclass;
}

extern "C" {

    unsigned short member_desc_get_member_id(member_desc *mdesc)
    {
        return mdesc->get_id();
    }

    MemberType member_desc_get_member_type(member_desc *mdesc)
    {
        return mdesc->get_member_type();
    }

    const char *member_desc_get_member_name(member_desc *mdesc)
    {
        return mdesc->get_member_name();
    }

    const char *member_desc_get_member_description(member_desc *mdesc)
    {
        return mdesc->get_member_description();
    }

    Type member_desc_get_field_vlg_type(member_desc *mdesc)
    {
        return mdesc->get_field_vlg_type();
    }

    size_t member_desc_get_field_offset(member_desc *mdesc)
    {
        return mdesc->get_field_offset();
    }

    size_t member_desc_get_field_type_size(member_desc *mdesc)
    {
        return mdesc->get_field_type_size();
    }

    size_t member_desc_get_field_nmemb(member_desc *mdesc)
    {
        return mdesc->get_field_nmemb();
    }

    unsigned int member_desc_get_field_nclass_id(member_desc *mdesc)
    {
        return mdesc->get_field_nclass_id();
    }

    const char *member_desc_get_field_user_type(member_desc *mdesc)
    {
        return mdesc->get_field_user_type();
    }

    NEntityType member_desc_get_field_nentity_type(member_desc *mdesc)
    {
        return mdesc->get_field_nentity_type();
    }

    long member_desc_get_nenum_value(member_desc *mdesc)
    {
        return mdesc->get_nenum_value();
    }

    unsigned short key_desc_get_key_id(key_desc *kdesc)
    {
        return kdesc->get_id();
    }

    int key_desc_is_primary(key_desc *kdesc)
    {
        return kdesc->is_primary() ? 1 : 0;
    }

    unsigned int nentity_desc_get_nclass_id(nentity_desc *edesc)
    {
        return edesc->get_nclass_id();
    }

    size_t nentity_desc_get_nentity_size(nentity_desc *edesc)
    {
        return edesc->get_nclass_size();
    }

    NEntityType nentity_desc_get_nentity_type(nentity_desc *edesc)
    {
        return edesc->get_nentity_type();
    }

    const char *nentity_desc_get_nentity_namespace(nentity_desc *edesc)
    {
        return edesc->get_nentity_namespace();
    }

    const char *nentity_desc_get_nentity_name(nentity_desc *edesc)
    {
        return edesc->get_nentity_name();
    }

    nclass_alloc nentity_desc_get_nentity_allocation_function(nentity_desc *edesc)
    {
        return edesc->get_nclass_allocation_function();
    }

    unsigned int nentity_desc_get_nentity_member_num(nentity_desc *edesc)
    {
        return edesc->get_nentity_member_count();
    }

    int nentity_desc_is_persistent(nentity_desc *edesc)
    {
        return edesc->is_persistent() ? 1 : 0;
    }

    const member_desc *nentity_desc_get_member_desc_by_id(nentity_desc *edesc,
                                                          unsigned int member_id)
    {
        return edesc->get_member_desc_by_id(member_id);
    }

    const member_desc *nentity_desc_get_member_desc_by_name(nentity_desc *edesc,
                                                            const char *member_name)
    {
        return edesc->get_member_desc_by_name(member_name);
    }

    void nentity_desc_enum_member_descriptors(nentity_desc *edesc,
                                              enum_member_desc emd_f,
                                              void *ud)
    {
        edesc->enum_member_descriptors(emd_f, ud);
    }

    unsigned int nclass_get_nclass_id(nclass *obj)
    {
        return obj->get_id();
    }

    unsigned int nclass_get_compiler_version(nclass *obj)
    {
        return obj->get_compiler_version();
    }

    size_t nclass_get_nentity_size(nclass *obj)
    {
        return obj->get_size();
    }

    const nclass *nclass_get_zero_object(nclass *obj)
    {
        return &obj->get_zero_object();
    }

    void nclass_copy_to(nclass *obj, nclass *to_obj)
    {
        obj->copy_to(*to_obj);
    }

    nclass *own_nclass_get_ptr(own_nclass *obj)
    {
        return ((std::unique_ptr<nclass> *)obj)->get();
    }

    void own_nclass_release(own_nclass *obj)
    {
        delete(std::unique_ptr<nclass> *)obj;
    }

    own_nclass *nclass_clone(nclass *obj)
    {
        return (own_nclass *) new std::unique_ptr<nclass>(obj->clone());
    }

    int nclass_is_zero(nclass *obj)
    {
        return obj->is_zero() ? 1 : 0;
    }

    void nclass_set_zero(nclass *obj)
    {
        obj->set_zero();
    }

    void nclass_set_from(nclass *obj, const nclass *from_obj)
    {
        obj->set_from(*from_obj);
    }

    size_t nclass_get_field_size_by_id(nclass *obj,
                                       unsigned int field_id)
    {
        return obj->get_field_size_by_id(field_id);
    }

    size_t nclass_get_field_size_by_name(nclass *obj,
                                         const char *field_name)
    {
        return obj->get_field_size_by_name(field_name);
    }

    void *nclass_get_field_by_id(nclass *obj,
                                 unsigned int field_id)
    {
        return obj->get_field_address_by_id(field_id);
    }

    void *nclass_get_field_by_name(nclass *obj,
                                   const char *field_name)
    {
        return obj->get_field_address_by_name(field_name);
    }

    void *nclass_get_field_by_id_index(nclass *obj,
                                       unsigned int field_id,
                                       unsigned int index)
    {
        return obj->get_field_address_by_id_and_index(field_id, index);
    }

    void *nclass_get_field_by_name_index(nclass *obj,
                                         const char *field_name,
                                         unsigned int index)
    {
        return obj->get_field_address_by_name_and_index(field_name, index);
    }

    RetCode nclass_set_field_by_id(nclass *obj,
                                   unsigned int field_id,
                                   const void *ptr,
                                   size_t maxlen)
    {
        return obj->set_field_by_id(field_id, ptr, maxlen);
    }

    RetCode nclass_set_field_by_name(nclass *obj,
                                     const char *field_name,
                                     const void *ptr,
                                     size_t maxlen)
    {
        return obj->set_field_by_name(field_name, ptr, maxlen);
    }

    RetCode nclass_set_field_by_id_index(nclass *obj,
                                         unsigned int field_id,
                                         const void *ptr, unsigned int index,
                                         size_t maxlen)
    {
        return obj->set_field_by_id_index(field_id,
                                          ptr,
                                          index,
                                          maxlen);
    }

    RetCode nclass_set_field_by_name_index(nclass *obj,
                                           const char *field_name,
                                           const void *ptr,
                                           unsigned int index,
                                           size_t maxlen)
    {
        return obj->set_field_by_name_index(field_name,
                                            ptr,
                                            index,
                                            maxlen);
    }

    RetCode nclass_is_field_zero_by_id(nclass *obj,
                                       unsigned int field_id,
                                       int *res)
    {
        bool b_res = false;
        RetCode r_res = obj->is_field_zero_by_id(field_id, b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode nclass_is_field_zero_by_name(nclass *obj,
                                         const char *field_name,
                                         int *res)
    {
        bool b_res = false;
        RetCode r_res = obj->is_field_zero_by_name(field_name, b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode nclass_is_field_zero_by_id_index(nclass *obj,
                                             unsigned int field_id,
                                             unsigned int index,
                                             unsigned int nmenb,
                                             int *res)
    {
        bool b_res = false;
        RetCode r_res = obj->is_field_zero_by_id_index(field_id,
                                                       index,
                                                       nmenb,
                                                       b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode nclass_is_field_zero_by_name_index(nclass *obj,
                                               const char *field_name,
                                               unsigned int index,
                                               unsigned int nmenb,
                                               int *res)
    {
        bool b_res = false;
        RetCode r_res = obj->is_field_zero_by_name_index(
                            field_name,
                            index,
                            nmenb,
                            b_res);
        *res = b_res ? 1 : 0;
        return r_res;
    }

    RetCode nclass_set_field_zero_by_id(nclass *obj,
                                        unsigned int field_id)
    {
        return obj->set_field_zero_by_id(field_id);
    }

    RetCode nclass_set_field_zero_by_name(nclass *obj,
                                          const char *field_name)
    {
        return obj->set_field_zero_by_name(field_name);
    }

    RetCode nclass_set_field_zero_by_id_index(nclass *obj,
                                              unsigned int field_id,
                                              unsigned int index,
                                              unsigned int nmenb)
    {
        return obj->set_field_zero_by_id_index(field_id,
                                               index,
                                               nmenb);
    }

    RetCode nclass_set_field_zero_by_name_index(nclass *obj,
                                                const char *field_name,
                                                unsigned int index,
                                                unsigned int nmenb)
    {
        return obj->set_field_zero_by_name_index(field_name,
                                                 index,
                                                 nmenb);
    }

    char *nclass_get_field_address_by_column_number(nclass *obj,
                                                    unsigned int plain_idx,
                                                    const nentity_manager *nem,
                                                    const member_desc **member_descriptor)
    {
        return obj->get_field_address_by_column_number(plain_idx,
                                                       *nem,
                                                       member_descriptor);
    }

    const nentity_desc *nclass_get_nentity_descriptor(nclass *obj)
    {
        return &obj->get_nentity_descriptor();
    }

    size_t nclass_pretty_dump_to_buffer(nclass *obj,
                                        char *buffer,
                                        int print_nclass_name)
    {
        return obj->pretty_dump_to_buffer(buffer, print_nclass_name ? true : false);
    }

    size_t nclass_pretty_dump_to_file(nclass *obj,
                                      FILE *file,
                                      int print_nclass_name)
    {
        return obj->pretty_dump_to_file(file, print_nclass_name ? true : false);
    }

    RetCode nclass_get_primary_key_value_as_string(nclass *obj,
                                                   char **newly_alloc_out_pkey)
    {
        std::unique_ptr<char> pkey;
        RetCode r_res = obj->get_primary_key_value_as_string(pkey);
        if(newly_alloc_out_pkey && !r_res) {
            *newly_alloc_out_pkey = strdup(pkey.get());
        }
        return r_res;
    }

    const nentity_desc *nentity_manager_get_nentity_descriptor_by_nclassid(nentity_manager *nem, unsigned int nclass_id)
    {
        return nem->get_nentity_descriptor(nclass_id);
    }

    const nentity_desc *nentity_manager_get_nentity_descriptor_by_name(nentity_manager *nem, const char *nentity_name)
    {
        return nem->get_nentity_descriptor(nentity_name);
    }

    void nentity_manager_enum_nentity_descriptors(nentity_manager *nem, enum_nentity_desc eedf, void *ud)
    {
        nem->enum_nentity_descriptors(eedf, ud);
    }

    RetCode nentity_manager_extend_with_model_name(nentity_manager *nem, const char *model_name)
    {
        return nem->extend(model_name);
    }

    void nentity_manager_enum_nenum_descriptors(nentity_manager *nem, enum_nentity_desc eedf, void *ud)
    {
        nem->enum_nenum_descriptors(eedf, ud);
    }

    void nentity_manager_enum_nclass_descriptors(nentity_manager *nem, enum_nentity_desc eedf, void *ud)
    {
        nem->enum_nclass_descriptors(eedf, ud);
    }

    RetCode nentity_manager_new_nclass_instance(nentity_manager *nem,
                                                unsigned int nclass_id,
                                                own_nclass **new_nclass_obj)
    {
        std::unique_ptr<nclass> *nnclss_obj = new std::unique_ptr<nclass>();
        RetCode r_res = nem->new_nclass_instance(nclass_id, *nnclss_obj);
        if(r_res) {
            delete nnclss_obj;
        }
        *new_nclass_obj = (own_nclass *)nnclss_obj;
        return r_res;
    }

    unsigned int nentity_manager_nentity_count(nentity_manager *nem)
    {
        return nem->nentity_count();
    }

    unsigned int nentity_manager_nenum_count(nentity_manager *nem)
    {
        return nem->nenum_count();
    }

    unsigned int nentity_manager_nclass_count(nentity_manager *nem)
    {
        return nem->nclass_count();
    }

    RetCode nentity_manager_extend_with_nentity_desc(nentity_manager *nem,
                                                     const nentity_desc *edesc)
    {
        return nem->extend(*edesc);
    }

    RetCode nentity_manager_extend_with_nentity_manager(nentity_manager *nem1,
                                                        const nentity_manager *nem2)
    {
        return nem1->extend(*nem2);
    }
}
