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

#ifndef VLG_CPP_MDL_H_
#define VLG_CPP_MDL_H_
#include "vlg.h"

namespace vlg {

/** @brief member_desc describes a member of a vlg entity type.
    member_desc can describe both a field of a nclass type or a value of a
    nenum type.
 */
struct member_desc_impl;
struct member_desc {
    explicit member_desc(unsigned short member_id,
                         MemberType member_type,
                         const char *member_name,
                         const char *member_description,
                         Type field_vlg_type,
                         size_t field_offset,
                         size_t field_type_size,
                         size_t field_nmemb,
                         unsigned int field_nclass_id,
                         const char *field_user_type,
                         NEntityType field_entity_type,
                         long enum_value);

    ~member_desc();

    unsigned short get_id() const;
    MemberType get_member_type() const;
    const char *get_member_name() const;
    const char *get_member_description() const;

    Type get_field_vlg_type() const;
    size_t get_field_offset() const;
    size_t get_field_type_size() const;
    size_t get_field_nmemb() const;
    unsigned int get_field_nclass_id() const;
    const char *get_field_user_type() const;
    NEntityType get_field_nentity_type() const;

    long get_nenum_value() const;

    std::unique_ptr<member_desc_impl> impl_;
};

typedef bool(*enum_member_desc)(const member_desc &member_descriptor,
                                void *ud);

/** @brief key_desc describes a key of a nclass type.
 */
struct key_desc_impl;
struct key_desc {
    explicit key_desc(unsigned short keyid,
                      bool primary);

    ~key_desc();

    RetCode add_member_desc(member_desc &member_descriptor);

    unsigned short get_id() const;
    bool is_primary() const;

    void enum_member_descriptors(enum_member_desc emd_f,
                                 void *ud) const;

    std::unique_ptr<key_desc_impl> impl_;
};

typedef bool(*enum_key_desc)(const key_desc &key_descriptor,
                             void *ud);

/** @brief nentity_desc describes nentity types.
    nentity_desc can describe both a nclass type or a nenum type.
 */
struct nentity_desc_impl;
struct nentity_desc {
    explicit nentity_desc(unsigned int nclass_id,
                          size_t nclass_size,
                          size_t nclass_max_align,
                          NEntityType nentity_type,
                          const char *entity_namespace,
                          const char *entity_name,
                          nclass_alloc nclass_allocation_function,
                          unsigned int nentity_member_num,
                          bool persistent);
    ~nentity_desc();

    RetCode add_member_desc(const member_desc &);
    RetCode add_key_desc(const key_desc &);

    unsigned int get_nclass_id() const;
    size_t get_nclass_size() const;
    NEntityType get_nentity_type() const;
    const char *get_nentity_namespace() const;
    const char *get_nentity_name() const;
    nclass_alloc get_nclass_allocation_function() const;
    unsigned int get_nentity_member_count() const;
    bool is_persistent() const;

    const member_desc *get_member_desc_by_id(unsigned int) const;
    const member_desc *get_member_desc_by_name(const char *) const;
    const member_desc *get_member_desc_by_offset(size_t) const;

    void enum_member_descriptors(enum_member_desc emd_f,
                                 void *ud) const;

    const key_desc *get_key_desc_by_id(unsigned short) const;

    void enum_key_descriptors(enum_key_desc ekd_f,
                              void *ud) const;

    std::unique_ptr<nentity_desc_impl> impl_;
};

/** @brief nclass
 */
struct nclass {
    explicit nclass();
    virtual ~nclass();

    virtual unsigned int get_id() const = 0;
    virtual unsigned int get_compiler_version() const = 0;
    virtual size_t get_size() const = 0;
    virtual const nclass &get_zero_object() const = 0;

    virtual void copy_to(nclass &) const = 0;
    virtual std::unique_ptr<nclass> clone() const = 0;
    virtual bool is_zero() const = 0;
    virtual void set_zero() = 0;
    virtual void set_from(const nclass &) = 0;

    size_t get_field_size_by_id(unsigned int) const;
    size_t get_field_size_by_name(const char *) const;
    char *get_field_address_by_id(unsigned int);
    char *get_field_address_by_name(const char *);

    char *get_field_address_by_id_and_index(unsigned int field_id,
                                            unsigned int index);

    char *get_field_address_by_name_and_index(const char *field_name,
                                              unsigned int index);

    char *get_field_address_by_column_number(unsigned int column_number,
                                             const nentity_manager &nem,
                                             const member_desc **member_descriptor);

    RetCode set_field_by_id(unsigned int field_id,
                            const void *ptr,
                            size_t maxlen = 0);

    RetCode set_field_by_name(const char *field_name,
                              const void *ptr,
                              size_t maxlen = 0);

    RetCode set_field_by_id_index(unsigned int field_id,
                                  const void *ptr,
                                  unsigned int index,
                                  size_t maxlen = 0);

    RetCode set_field_by_name_index(const char *field_name,
                                    const void *ptr,
                                    unsigned int index,
                                    size_t maxlen = 0);

    RetCode is_field_zero_by_id(unsigned int field_id,
                                bool &res) const;

    RetCode is_field_zero_by_name(const char *field_name,
                                  bool &res) const;

    RetCode is_field_zero_by_id_index(unsigned int field_id,
                                      unsigned int index,
                                      unsigned int nmenb,
                                      bool &res) const;

    RetCode is_field_zero_by_name_index(const char *field_name,
                                        unsigned int index,
                                        unsigned int nmenb,
                                        bool &res) const;

    RetCode set_field_zero_by_id(unsigned int field_id);
    RetCode set_field_zero_by_name(const char *field_name);

    RetCode set_field_zero_by_id_index(unsigned int field_id,
                                       unsigned int index,
                                       unsigned int nmenb);

    RetCode set_field_zero_by_name_index(const char *field_name,
                                         unsigned int index,
                                         unsigned int nmenb);

    virtual const nentity_desc &get_nentity_descriptor() const = 0;

    virtual size_t pretty_dump_to_buffer(char *buffer,
                                         bool print_nclass_name = true) const = 0;

    virtual size_t pretty_dump_to_file(FILE *file,
                                       bool print_nclass_name = true) const = 0;

    /*
    serialize / restore
    */
    virtual int serialize(Encode encode,
                          const nclass *previous_image,
                          g_bbuf *obb) const = 0;

    RetCode restore(const nentity_manager *nem,
                    Encode encode,
                    g_bbuf *ibb);

    virtual RetCode get_primary_key_value_as_string(std::unique_ptr<char> &);
};

typedef const char *(*model_version_func)();
typedef nentity_manager *(*nentity_manager_func)();

typedef bool (*enum_nentity_desc)(const nentity_desc &nentity_descriptor,
                                  void *ud);

/** @brief nentity_manager holds nentity_desc objects.
 */
struct nentity_manager_impl;
struct nentity_manager {

    explicit nentity_manager();
    ~nentity_manager();

    const nentity_desc *get_nentity_descriptor(unsigned int nclass_id) const;
    const nentity_desc *get_nentity_descriptor(const char *entity_name) const;

    void enum_nentity_descriptors(enum_nentity_desc eedf,
                                  void *ud) const;

    void enum_nenum_descriptors(enum_nentity_desc eedf,
                                void *ud)   const;

    void enum_nclass_descriptors(enum_nentity_desc eedf,
                                 void *ud)  const;

    RetCode new_nclass_instance(unsigned int nclass_id,
                                nclass **new_nclass_obj) const;

    unsigned int nentity_count() const;
    unsigned int nenum_count() const;
    unsigned int nclass_count() const;

    RetCode extend(const nentity_desc &nentity_descriptor);
    RetCode extend(const nentity_manager &nem);
    RetCode extend(const char *model_name);

    std::unique_ptr<nentity_manager_impl> impl_;
};

}

#endif
