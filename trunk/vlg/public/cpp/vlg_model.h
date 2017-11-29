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
#include "vlg_memory.h"

namespace vlg {

/** @brief member_desc describes a member of a vlg entity type.
    member_desc can describe both a field of a nclass type or a value of a
    nenum type.
 */
class member_desc_impl;
class member_desc {
    public:
        explicit member_desc(unsigned short member_id,
                             MemberType     member_type,
                             const char     *member_name,
                             const char     *member_description,
                             Type           field_vlg_type,
                             size_t         field_offset,
                             size_t         field_type_size,
                             size_t         field_nmemb,
                             unsigned int   field_class_id,
                             const char     *field_user_type,
                             NEntityType    field_entity_type,
                             long           enum_value);

        ~member_desc();

    public:
        /*
        member section
        */
        unsigned short  get_member_id()             const;
        MemberType      get_member_type()           const;
        const char      *get_member_name()          const;
        const char      *get_member_description()   const;

        /*
        field section
        */
        Type            get_field_vlg_type()        const;
        size_t          get_field_offset()          const;
        size_t          get_field_type_size()       const;
        size_t          get_field_nmemb()           const;
        unsigned int    get_field_nclass_id()       const;
        const char      *get_field_user_type()      const;
        NEntityType      get_field_nentity_type()   const;

        void            set_field_offset(size_t field_offset);
        void            set_field_type_size(size_t field_type_size);

        /*
        nenum specific
        */
        long            get_nenum_value()           const;

    public:
        const member_desc_impl *get_opaque()    const;

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

/** @brief nentity_desc describes nentity types.
    nentity_desc can describe both a nclass type or a nenum type.
 */
class nentity_desc_impl;
class nentity_desc {
    public:
        explicit nentity_desc(unsigned int      nclass_id,
                              size_t            nclass_size,
                              size_t            nclass_max_align,
                              NEntityType       nentity_type,
                              const char        *entity_namespace,
                              const char        *entity_name,
                              vlg::alloc_func   nentity_allocation_function,
                              unsigned int      nentity_member_num,
                              bool              persistent);
        ~nentity_desc();

    public:
        vlg::RetCode      add_member_desc(const member_desc *member_descriptor);
        vlg::RetCode      add_key_desc(const key_desc *key_descriptor);

        unsigned int        get_nclass_id()                     const;
        size_t              get_nclass_size()                   const;
        size_t              get_nclass_max_align()              const;
        NEntityType         get_nentity_type()                  const;
        const char          *get_nentity_namespace()            const;
        const char          *get_nentity_name()                 const;
        vlg::alloc_func     get_nclass_allocation_function()    const;
        unsigned int        get_nentity_member_num()            const;
        bool                is_persistent()                     const;

        const member_desc *get_member_desc_by_id(unsigned int member_id)    const;
        const member_desc *get_member_desc_by_name(const char *member_name) const;
        const member_desc *get_member_desc_by_offset(size_t member_offset)  const;
        void enum_member_descriptors(enum_member_desc emd_f, void *ud)      const;

    public:
        const nentity_desc_impl *get_opaque() const;

    private:
        nentity_desc_impl *impl_;
};

/** @brief nclass
 */
class nclass : public vlg::collectable {
        friend class nentity_manager;

    public:
        explicit nclass();
        virtual ~nclass();

        virtual vlg::collector &get_collector();

    public:
        virtual unsigned int    get_nclass_id()             const = 0;
        virtual unsigned int    get_compiler_version()      const = 0;
        virtual size_t          get_size()                  const = 0;
        virtual const nclass    *get_zero_object()          const = 0;

        /*
        nclass manipulation
        */
    public:
        virtual void            copy_to(nclass *obj)        const = 0;
        virtual nclass          *clone()                    const = 0;
        virtual bool            is_zero()                   const = 0;
        virtual void            set_zero() = 0;
        virtual void            set_from(const nclass *obj) = 0;

        /*
        nclass field manipulation
        */
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

        char *get_field_by_column_number(unsigned int column_number,
                                         const nentity_manager &nem,
                                         const member_desc **member_descriptor);

    public:
        virtual const nentity_desc *get_nentity_descriptor()  const = 0;

        virtual size_t pretty_dump_to_buffer(char *buffer,
                                             bool print_nclass_name = true) const = 0;

        virtual size_t pretty_dump_to_file(FILE *file,
                                           bool print_nclass_name = true)   const = 0;

        /*
        serialize / restore
        */
    public:
        virtual int serialize(Encode encode,
                              const nclass *previous_image,
                              vlg::grow_byte_buffer *obb) const = 0;

        vlg::RetCode restore(const nentity_manager *nem,
                             Encode encode,
                             vlg::grow_byte_buffer *ibb);

        /*
        persistence related
        */
    public:
        virtual vlg::RetCode get_primary_key_value_as_string(vlg::shared_pointer<char> &out_str);

    protected:
        static nclass_logger *log_;
};

typedef const char *(*model_version_func)();
typedef nentity_manager *(*nentity_manager_func)();

typedef void (*enum_nentity_desc)(const nentity_desc &nentity_descriptor,
                                  void *ud,
                                  bool &stop);

/** @brief nentity_manager holds nentity_desc objects.
 */
class nentity_manager_impl;
class nentity_manager {
        friend class nentity_manager_impl;
    public:
        explicit nentity_manager();
        ~nentity_manager();

        vlg::RetCode get_nentity_descriptor(unsigned int nclass_id,
                                            nentity_desc const **entity_descriptor) const;

        vlg::RetCode get_nentity_descriptor(const char *entity_name,
                                            nentity_desc const **entity_descriptor) const;

        void enum_nentity_descriptors(enum_nentity_desc eedf,
                                      void *ud) const;

        void enum_nenum_descriptors(enum_nentity_desc eedf,
                                    void *ud)   const;

        void enum_nclass_descriptors(enum_nentity_desc eedf,
                                     void *ud)  const;

        vlg::RetCode new_nclass_instance(unsigned int nclass_id,
                                         nclass **new_nclass_obj) const;

        unsigned int    nentity_count() const;
        unsigned int    nenum_count()   const;
        unsigned int    nclass_count()  const;

        const char      *get_nclass_name(unsigned int nclass_id) const;

        vlg::RetCode  extend(const nentity_desc *nentity_descriptor);
        vlg::RetCode  extend(nentity_manager *nem);
        vlg::RetCode  extend(const char *model_name);

    public:
        const nentity_manager_impl *get_opaque() const;

    private:
        nentity_manager_impl *impl_;
};

}

#endif
