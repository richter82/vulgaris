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

#ifndef BLZ_CPP_MDL_H_
#define BLZ_CPP_MDL_H_
#include "blaze_memory.h"
#include "blaze_byte_buffer.h"
#include "blaze_ascii_string.h"

namespace vlg {

/** @brief member_desc describes a member of a vlg entity type.

    member_desc can describe both a field of a nclass type or a value of a
    blaze_enum type.
 */
class member_desc_impl;
class member_desc {
    public:
        explicit member_desc(unsigned short member_id,
                             MemberType     member_type,
                             const char     *member_name,
                             const char     *member_description,
                             Type           field_blz_type,
                             size_t         field_offset,
                             size_t         field_type_size,
                             size_t         field_nmemb,
                             unsigned int   field_class_id,
                             const char     *field_user_type,
                             EntityType     field_entity_type,
                             long           enum_value);

        ~member_desc();

    public:
        /*
        Member section
        */
        unsigned short  get_member_id()             const;
        MemberType      get_member_type()           const;
        const char      *get_member_name()          const;
        const char      *get_member_description()   const;

        /*
        Field section
        */
        Type            get_field_blz_type()        const;
        size_t          get_field_offset()          const;
        size_t          get_field_type_size()       const;
        size_t          get_field_nmemb()           const;
        unsigned int    get_field_class_id()        const;
        const char      *get_field_user_type()      const;
        EntityType      get_field_entity_type()     const;

        void            set_field_offset(size_t field_offset);
        void            set_field_type_size(size_t field_type_size);

        /*
        Enum specific
        */
        long            get_enum_value()            const;

    public:
        const member_desc_impl *get_opaque()        const;

    private:
        member_desc_impl *impl_;
};

/** @brief key_desc describes a key of a nclass type.
 */
class key_desc_impl;
class key_desc {
    public:
        explicit key_desc(unsigned short keyid,
                          bool primary);

        ~key_desc();

    public:
        vlg::RetCode  init();
        vlg::RetCode  add_member_desc(const member_desc *member_descriptor);

        unsigned short  get_key_id()        const;
        bool            is_primary()        const;

    public:
        const key_desc_impl *get_opaque()   const;

    private:
        key_desc_impl *impl_;
};


typedef void (*enum_member_desc)(const member_desc &member_descriptor,
                                 void *ud,
                                 bool &stop);

/** @brief entity_desc describes vlg entity type.

    entity_desc can describe both a nclass type or a blaze_enum type.
 */
class entity_desc_impl;
class entity_desc {
    public:
        explicit entity_desc(unsigned int       nclass_id,
                             size_t             entity_size,
                             size_t             entity_max_align,
                             EntityType         entity_type,
                             const char         *entity_namespace,
                             const char         *entity_name,
                             vlg::alloc_func  entity_allocation_function,
                             unsigned int       entity_member_num,
                             bool               persistent);

        ~entity_desc();

    public:
        vlg::RetCode      init();
        vlg::RetCode      add_member_desc(const member_desc *member_descriptor);
        vlg::RetCode      add_key_desc(const key_desc *key_descriptor);

        unsigned int        get_nclass_id()                     const;
        size_t              get_entity_size()                   const;
        size_t              get_entity_max_align()              const;
        EntityType          get_entity_type()                   const;
        const char          *get_entity_namespace()             const;
        const char          *get_entity_name()                  const;
        vlg::alloc_func   get_entity_allocation_function()    const;
        unsigned int        get_entity_member_num()             const;
        bool                is_persistent()                     const;

        const member_desc *
        get_member_desc_by_id(unsigned int member_id)           const;

        const member_desc *
        get_member_desc_by_name(const char *member_name)        const;

        const member_desc *
        get_member_desc_by_offset(size_t member_offset)         const;

        void
        enum_member_descriptors(enum_member_desc emd_f,
                                void *ud)                       const;

    public:
        const entity_desc_impl *get_opaque()                    const;

    private:
        entity_desc_impl *impl_;
};

/** @brief nclass
 */
class nclass : public vlg::collectable {
        friend class entity_manager;

    public:
        explicit nclass();
        virtual ~nclass();

        virtual vlg::collector &get_collector();

    public:
        virtual unsigned int    get_nclass_id()             const = 0;
        virtual unsigned int    get_compiler_version()      const = 0;
        virtual size_t          get_entity_size()           const = 0;
        virtual const nclass    *get_zero_object()          const = 0;

        /*************************************************************
        -Class level manipulation
        **************************************************************/
    public:
        virtual void            copy_to(nclass *obj)        const = 0;
        virtual nclass          *clone()                    const = 0;
        virtual bool            is_zero()                   const = 0;
        virtual void            set_zero() = 0;
        virtual void            set_from(const nclass *obj) = 0;

        /*************************************************************
        -Fields manipulation
        **************************************************************/
    public:
        size_t          get_field_size_by_id(unsigned int field_id)     const;
        size_t          get_field_size_by_name(const char *field_name)  const;
        void            *get_field_by_id(unsigned int field_id);
        void            *get_field_by_name(const char *field_name);

        void            *get_field_by_id_index(unsigned int field_id,
                                               unsigned int index);

        void            *get_field_by_name_index(const char *field_name,
                                                 unsigned int index);

        vlg::RetCode  set_field_by_id(unsigned int field_id,
                                        const void *ptr,
                                        size_t maxlen = 0);

        vlg::RetCode  set_field_by_name(const char *field_name,
                                          const void *ptr,
                                          size_t maxlen = 0);

        vlg::RetCode  set_field_by_id_index(unsigned int field_id,
                                              const void *ptr,
                                              unsigned int index,
                                              size_t maxlen = 0);

        vlg::RetCode  set_field_by_name_index(const char *field_name,
                                                const void *ptr,
                                                unsigned int index,
                                                size_t maxlen = 0);

        vlg::RetCode  is_field_zero_by_id(unsigned int field_id,
                                            bool &res) const;

        vlg::RetCode  is_field_zero_by_name(const char *field_name,
                                              bool &res) const;

        vlg::RetCode  is_field_zero_by_id_index(unsigned int field_id,
                                                  unsigned int index,
                                                  unsigned int nmenb,
                                                  bool &res) const;

        vlg::RetCode  is_field_zero_by_name_index(const char *field_name,
                                                    unsigned int index,
                                                    unsigned int nmenb,
                                                    bool &res) const;

        vlg::RetCode  set_field_zero_by_id(unsigned int field_id);
        vlg::RetCode  set_field_zero_by_name(const char *field_name);

        vlg::RetCode  set_field_zero_by_id_index(unsigned int field_id,
                                                   unsigned int index,
                                                   unsigned int nmenb);

        vlg::RetCode  set_field_zero_by_name_index(const char *field_name,
                                                     unsigned int index,
                                                     unsigned int nmenb);

        /**
        This method returns the memory address to the LEAF
        field of this nclass object, denoted by plain_idx.
        If plain_idx exceeds the logical number of available item-fields
        for this nclass object, this function returns NULL.
        In case of success *member_descriptor points to the member_desc linked
        to the leaf-field-pointer returned.
        Significant fields of member_desc struct are: get_field_blz_type() and
        get_field_type_size().
        *member_descriptor can never point to a descriptor for a
        EntityType field; instead it will always point to a primitive
        BLZ_TYPE field.
        The use of this function is *DISCOURAGED*
        because it is inherently inefficient.
        Anyway in some very peculiar cases it is necessary.

        @param plain_idx
        @param em
        @param member_descriptor
        @return the address [byte aligned] to the TERMINAL-LEAF field of this
                nclass object, denoted by plain_idx;
                NULL if plain_idx exceeds the logical number of available
                item-fields for this nclass object.
        */
        char *
        get_term_field_ref_by_plain_idx(unsigned int plain_idx,
                                        const entity_manager &em,
                                        const member_desc **member_descriptor);

        /*************************************************************
        -Class Description
        **************************************************************/
    public:
        virtual const entity_desc *get_entity_descriptor()  const = 0;

        virtual size_t
        pretty_dump_to_buffer(char *buffer,
                              bool print_nclass_name = true) const = 0;

        virtual size_t
        pretty_dump_to_file(FILE *file,
                            bool print_nclass_name = true)   const = 0;

        /*************************************************************
        -Class Serialization
        **************************************************************/
    public:
        virtual int serialize(Encode class_encode_type,
                              const nclass *previous_image,
                              vlg::grow_byte_buffer *obb) const = 0;

        /*************************************************************
        -Class Restore
        **************************************************************/
    public:
        vlg::RetCode restore(const entity_manager *em,
                               Encode class_encode_type,
                               vlg::grow_byte_buffer *ibb);

        /*************************************************************
        -Class Persistence
        **************************************************************/
    public:
        virtual vlg::RetCode
        primary_key_string_value(vlg::ascii_string *out_str);

    protected:
        static nclass_logger *log_;
};

typedef const char *(*model_version_func)();
typedef entity_manager *(*entity_manager_func)();

typedef void (*enum_entity_desc)(const entity_desc &entity_descriptor,
                                 void *ud,
                                 bool &stop);

/** @brief entity_manager holds entity_desc objects.
 */
class entity_manager_impl;
class entity_manager {
        friend class entity_manager_impl;
    public:
        explicit entity_manager();
        ~entity_manager();

        vlg::RetCode init();

        vlg::RetCode
        get_entity_descriptor(unsigned int nclass_id,
                              entity_desc const **entity_descriptor) const;

        vlg::RetCode
        get_entity_descriptor(const char *entity_name,
                              entity_desc const **entity_descriptor) const;

        void enum_entity_descriptors(enum_entity_desc eedf,
                                     void *ud)                      const;

        void enum_enum_descriptors(enum_entity_desc eedf,
                                   void *ud)                        const;

        void enum_nclass_descriptors(enum_entity_desc eedf,
                                     void *ud)                      const;

        vlg::RetCode new_class_instance(unsigned int nclass_id,
                                          nclass **new_class_obj) const;

        unsigned int    entity_count()  const;
        unsigned int    enum_count()    const;
        unsigned int    nclass_count()  const;

        const char      *get_class_name(unsigned int nclass_id) const;

        vlg::RetCode  extend(const entity_desc *entity_descriptor);
        vlg::RetCode  extend(entity_manager *em);
        vlg::RetCode  extend(const char *model_name);

    public:
        const entity_manager_impl *get_opaque()                 const;

    private:
        entity_manager_impl *impl_;
};

}

#endif
