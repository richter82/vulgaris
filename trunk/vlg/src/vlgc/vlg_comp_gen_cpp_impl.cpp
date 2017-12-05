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

#include "vlg_compiler.h"

#define ENT_DESC_NOT_FND "entity descriptor not found, exiting.."

#define VLG_COMP_DPND_x86_64_win_MSVC "#if defined(_M_X64) && defined(WIN32) && defined(_MSC_VER)\n"
#define VLG_COMP_DPND_x86_64_unix_GCC "#if defined(__amd64__) && !defined(WIN32) && defined(__GNUG__)\n"

#define VLG_COMP_DPND_strict_linux "#if defined(__GNUG__) && defined(__linux)\n"

namespace vlg {

extern RetCode VLG_COMP_CPP_Calc_NMspc(entity_desc_comp &edesc,
                                       vlg::ascii_string &out);

#define BBUF_DECL \
"namespace vlg { class grow_byte_buffer {\n"\
"public:\n"\
 CR_1IND"explicit grow_byte_buffer();\n"\
 CR_1IND"~grow_byte_buffer();\n"\
 CR_1IND"RetCode init(size_t initial_capacity);\n"\
 CR_1IND"void reset();\n"\
 CR_1IND"void flip();\n"\
 CR_1IND"RetCode grow(size_t);\n"\
 CR_1IND"RetCode ensure_capacity(size_t);\n"\
 CR_1IND"RetCode append(const void*, size_t, size_t);\n"\
 CR_1IND"RetCode append_ushort(unsigned short);\n"\
 CR_1IND"RetCode append_uint(unsigned int);\n"\
 CR_1IND"RetCode put(const void*, size_t, size_t);\n"\
 CR_1IND"size_t position() const;\n"\
 CR_1IND"size_t limit() const;\n"\
 CR_1IND"size_t mark() const;\n"\
 CR_1IND"size_t capacity() const;\n"\
 CR_1IND"size_t remaining() const;\n"\
 CR_1IND"unsigned char *buffer();\n"\
 CR_1IND"char *buffer_as_char();\n"\
 CR_1IND"unsigned int *buffer_as_uint();\n"\
 CR_1IND"RetCode advance_pos_write(size_t);\n"\
 CR_1IND"RetCode set_pos_write(size_t);\n"\
 CR_1IND"void move_pos_write(size_t);\n"\
 CR_1IND"RetCode advance_pos_read(size_t);\n"\
 CR_1IND"RetCode set_pos_read(size_t);\n"\
 CR_1IND"void set_mark();\n"\
 CR_1IND"RetCode set_mark(size_t);\n"\
 CR_1IND"size_t from_mark() const;\n"\
 CR_1IND"size_t available_read();\n"\
 CR_1IND"RetCode read(size_t, void*);\n"\
 CR_1IND"RetCode read_ushort(unsigned short*);\n"\
 CR_1IND"RetCode read_uint(unsigned int*);\n"\
 CR_1IND"RetCode read_uint_to_sizet(size_t*);\n"\
"}; }\n\n"

/***********************************
GEN- VLG_COMP_Gen_Grow_Byte_Buffer__CPP_
***********************************/
RetCode VLG_COMP_Gen_Grow_Byte_Buffer__CPP_(FILE *file)
{
    fprintf(file, OPN_CMMNT_LN "Dynamic Byte Buffer\n" CLS_CMMNT_LN);
    fprintf(file, BBUF_DECL);
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_Serialize_new__CPP_
***********************************/
RetCode VLG_COMP_Gen_Serialize_new__CPP_(vlg::hash_map &entitymap,
                                         entity_desc_comp &edesc,
                                         FILE *file)
{
    //fprintf(file, VLG_COMP_1IND"unsigned short fidx = 0;\n");
    fprintf(file, CR_1IND"size_t tlen_offst = obb->position(), tlen = 0;\n");
    fprintf(file, CR_1IND"switch(enctyp){\n");
    fprintf(file, CR_2IND"case vlg::Encode_INDEXED_NOT_ZERO:\n");
    fprintf(file, CR_3IND"obb->advance_pos_write(ENTLN_B_SZ); tlen = obb->position();\n");
    vlg::hash_map &mmbr_map = edesc.get_map_id_MMBRDSC();
    member_desc_comp *mdsc = nullptr;
    mmbr_map.start_iteration();
    while(!mmbr_map.next(nullptr, &mdsc)) {
        if(mdsc->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdsc->get_field_type() != Type_ENTITY) {
            //primitive type
            if(mdsc->get_field_type() == Type_ASCII && mdsc->get_nmemb() > 1) {
                //for strings we send only significative bytes
                fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdsc->get_member_name());
                fprintf(file, CR_4IND"size_t fsize = strnlen(%s, %lu);\n",
                        mdsc->get_member_name(),
                        get_network_type_size(mdsc->get_field_type())
                        * mdsc->get_nmemb());
                //allocating space for: field index, field length, field size
                fprintf(file, CR_4IND"obb->grow(MMBID_B_SZ+FLDLN_B_SZ+fsize);\n");
                fprintf(file, CR_4IND"obb->append_ushort(%d);\n",
                        mdsc->get_member_id());
                fprintf(file,
                        CR_4IND"obb->append_uint((unsigned int)fsize);\n");
                fprintf(file, CR_4IND"obb->append(%s, 0, fsize);\n"
                        CR_3IND"}\n", mdsc->get_member_name());
            } else {
                if(mdsc->get_nmemb() == 1) {
                    //primitive type nmemb == 1
                    fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdsc->get_member_name());
                    fprintf(file, CR_4IND"obb->grow(MMBID_B_SZ+%lu);\n",
                            get_network_type_size(mdsc->get_field_type()));
                    fprintf(file, CR_4IND"obb->append_ushort(%d);\n",
                            mdsc->get_member_id());
                    fprintf(file, CR_4IND"obb->append(&%s, 0, %lu);\n"
                            CR_3IND"}\n", mdsc->get_member_name(),
                            get_network_type_size(mdsc->get_field_type()));
                } else {
                    //primitive type nmemb > 1
                    fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdsc->get_member_name());
                    fprintf(file, CR_4IND"obb->append_ushort(%d);\n",
                            mdsc->get_member_id());
                    fprintf(file, CR_4IND"size_t alen_offst = obb->position();\n");
                    fprintf(file, CR_4IND"obb->advance_pos_write(ARRAY_B_SZ); size_t alen = obb->position();\n");
                    fprintf(file, CR_4IND"for(int i = 0; i<%lu; i++){\n", mdsc->get_nmemb());
                    fprintf(file, CR_5IND"if(!is_zero_%s_idx(i)){\n", mdsc->get_member_name());
                    fprintf(file, CR_6IND"obb->append_ushort(i);\n");
                    fprintf(file, CR_6IND"obb->append(&%s[i], 0, %lu); }\n",
                            mdsc->get_member_name(),
                            get_network_type_size(mdsc->get_field_type()));
                    fprintf(file, CR_5IND"}\n");
                    fprintf(file,
                            CR_5IND"alen = (obb->position() - alen); obb->put(&alen, alen_offst, ARRAY_B_SZ);\n");
                    fprintf(file, CR_4IND"}\n");
                }
            }
        } else {
            entity_desc_comp *fdesc = nullptr;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &fdesc)) {
                if(fdesc->get_nentity_type() == NEntityType_NENUM) {
                    //enum, treat this as primitive type
                    size_t fsize = ENUM_B_SZ;
                    if(mdsc->get_nmemb() == 1) {
                        //primitive type nmemb == 1
                        fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdsc->get_member_name());
                        fprintf(file, CR_4IND"obb->grow(MMBID_B_SZ+%lu);\n", fsize);
                        fprintf(file, CR_4IND"obb->append_ushort(%d);\n",
                                mdsc->get_member_id());
                        fprintf(file, CR_4IND"obb->append(&%s, 0, %lu);\n"
                                CR_3IND"}\n", mdsc->get_member_name(), fsize);
                    } else {
                        //primitive type nmemb > 1
                        fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdsc->get_member_name());
                        fprintf(file, CR_4IND"obb->append_ushort(%d); //put fidx.\n",
                                mdsc->get_member_id());
                        fprintf(file, CR_4IND"size_t alen_offst = obb->position();\n");
                        fprintf(file,
                                CR_4IND"obb->advance_pos_write(ARRAY_B_SZ); size_t alen = obb->position();\n");
                        fprintf(file, CR_4IND"for(int i = 0; i<%lu; i++){\n", mdsc->get_nmemb());
                        fprintf(file, CR_5IND"if(!is_zero_%s_idx(i)){\n", mdsc->get_member_name());
                        fprintf(file, CR_6IND"obb->append_ushort(i);\n");
                        fprintf(file, CR_6IND"obb->append(&%s[i], 0, %lu); }\n",
                                mdsc->get_member_name(), fsize);
                        fprintf(file, CR_5IND"}\n");
                        fprintf(file, CR_5IND"alen = (obb->position() - alen); obb->put(&alen, alen_offst, ARRAY_B_SZ);\n");
                        fprintf(file, CR_4IND"}\n");
                    }
                } else {
                    //struct, class
                    if(mdsc->get_nmemb() == 1) {
                        fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdsc->get_member_name());
                        fprintf(file, CR_4IND"obb->append_ushort(%d);\n",
                                mdsc->get_member_id());
                        fprintf(file, CR_4IND"%s.serialize(enctyp, nullptr, obb); }\n",
                                mdsc->get_member_name());
                    } else {
                        fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdsc->get_member_name());
                        fprintf(file, CR_4IND"obb->append_ushort(%d);\n",
                                mdsc->get_member_id());
                        fprintf(file, CR_4IND"size_t alen_offst = obb->position();\n");
                        fprintf(file, CR_4IND"obb->advance_pos_write(ARRAY_B_SZ); size_t alen = obb->position();\n");
                        fprintf(file, CR_4IND"for(int i = 0; i<%lu; i++){\n", mdsc->get_nmemb());
                        fprintf(file, CR_5IND"if(!is_zero_%s_idx(i)){\n", mdsc->get_member_name());
                        fprintf(file, CR_6IND"obb->append_ushort(i);\n");
                        fprintf(file, CR_6IND"%s[i].serialize(enctyp, nullptr, obb); }\n",
                                mdsc->get_member_name());
                        fprintf(file, CR_5IND"}\n");
                        fprintf(file, CR_5IND"alen = (obb->position() - alen); obb->put(&alen, alen_offst, ARRAY_B_SZ);\n");
                        fprintf(file, CR_4IND"}\n");
                    }
                }
            }
        }
    }
    fprintf(file, CR_3IND"tlen = (obb->position() - tlen); obb->put(&tlen, tlen_offst, ENTLN_B_SZ);\n");
    fprintf(file, CR_3IND"break;\n");
    fprintf(file, CR_2IND"default: return -1;\n");
    fprintf(file, CR_1IND"}\n");
    fprintf(file, CR_1IND "return (int)obb->position();\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_ScalarMember_Buff_CPP_
***********************************/
RetCode VLG_COMP_Gen_ScalarMember_Buff_CPP_(member_desc_comp *mdsc,
                                            vlg::ascii_string &tmp,
                                            FILE *file)
{
    if(mdsc->get_nmemb() > 1) {
        fprintf(file,  CR_2IND
                "if(!is_zero_%s()){\n"
                CR_2IND
                CR_2IND
                "blen += sprintf(&buff[blen], \"%s=[\");\n",
                mdsc->get_member_name(),
                mdsc->get_member_name());
        fprintf(file, CR_2IND
                CR_2IND
                "for(int i=0; i<%lu; i++){\n",
                mdsc->get_nmemb());
        if(mdsc->get_field_type() == Type_INT64 ||
                mdsc->get_field_type() == Type_UINT64) {
            vlg::ascii_string tmp_lnx;
            fprintf(file, "%s", VLG_COMP_DPND_strict_linux);
            RETURN_IF_NOT_OK(printf_percent_from_VLG_TYPE(*mdsc, tmp_lnx, true))
            fprintf(file,  CR_2IND
                    CR_2IND
                    CR_2IND
                    "blen += sprintf(&buff[blen], \"%%%s\", %s[i]);\n",
                    tmp_lnx.internal_buff(),
                    mdsc->get_member_name());
            fprintf(file, "#else\n");
            fprintf(file,  CR_2IND
                    CR_2IND
                    CR_2IND
                    "blen += sprintf(&buff[blen], \"%%%s\", %s[i]);\n",
                    tmp.internal_buff(),
                    mdsc->get_member_name());
            fprintf(file, "#endif\n");
        } else {
            fprintf(file,  CR_2IND
                    CR_2IND
                    CR_2IND
                    "blen += sprintf(&buff[blen], \"%%%s\", %s[i]);\n",
                    tmp.internal_buff(),
                    mdsc->get_member_name());
        }
        fprintf(file,  CR_2IND
                CR_2IND
                CR_2IND
                "if(%lu>i+1) blen += sprintf(&buff[blen], \",\");\n",
                mdsc->get_nmemb());
        fprintf(file, CR_2IND
                CR_2IND
                "}\n");
        fprintf(file,  CR_2IND
                CR_2IND
                "blen += sprintf(&buff[blen], \"]\");\n");
        fprintf(file,  CR_2IND"}\n");
    } else {
        if(mdsc->get_field_type() == Type_INT64 ||
                mdsc->get_field_type() == Type_UINT64) {
            vlg::ascii_string tmp_lnx;
            fprintf(file, "%s", VLG_COMP_DPND_strict_linux);
            RETURN_IF_NOT_OK(printf_percent_from_VLG_TYPE(*mdsc, tmp_lnx, true))
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += sprintf(&buff[blen], \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp_lnx.internal_buff(),
                    mdsc->get_member_name());
            fprintf(file, "#else\n");
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += sprintf(&buff[blen], \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp.internal_buff(),
                    mdsc->get_member_name());
            fprintf(file, "#endif\n");
        } else {
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += sprintf(&buff[blen], \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp.internal_buff(),
                    mdsc->get_member_name());
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntityMember_Buff_CPP_
***********************************/
RetCode VLG_COMP_Gen_EntityMember_Buff_CPP_(entity_desc_comp
                                            &class_desc,
                                            member_desc_comp *ent_mdsc,
                                            entity_desc_comp *ent_desc,
                                            vlg::hash_map &entitymap,
                                            FILE *file)
{
    if(ent_mdsc->get_nmemb() > 1) {
        fprintf(file, CR_2IND
                "if(!is_zero_%s()){\n"
                CR_2IND CR_2IND
                "blen += sprintf(&buff[blen], \"%s=[\");\n",
                ent_mdsc->get_member_name(),
                ent_mdsc->get_member_name());
        fprintf(file, CR_2IND CR_2IND
                "for(int i=0; i<%lu; i++){\n",
                ent_mdsc->get_nmemb());
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "blen += %s[i].pretty_dump_to_buffer(&buff[blen], false);\n",
                ent_mdsc->get_member_name());
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "if(%lu>i+1) blen += sprintf(&buff[blen], \",\");\n",
                ent_mdsc->get_nmemb());
        fprintf(file, CR_2IND CR_2IND
                "}\n");
        fprintf(file, CR_2IND CR_2IND
                "blen += sprintf(&buff[blen], \"]\");\n");
        fprintf(file, CR_2IND"}\n");
    } else {
        fprintf(file, CR_2IND
                "if(!is_zero_%s()) blen += %s.pretty_dump_to_buffer(&buff[blen], false);\n",
                ent_mdsc->get_member_name(),
                ent_mdsc->get_member_name());
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_ScalarMember_FILE__CPP_
***********************************/
RetCode VLG_COMP_Gen_ScalarMember_FILE__CPP_(member_desc_comp *mdsc,
                                             vlg::ascii_string &tmp,
                                             FILE *file)
{
    if(mdsc->get_nmemb() > 1) {
        fprintf(file, CR_2IND
                "if(!is_zero_%s()){\n"
                CR_2IND CR_2IND
                "blen += fprintf(f, \"%s=[\");\n",
                mdsc->get_member_name(),
                mdsc->get_member_name());
        fprintf(file, CR_2IND CR_2IND
                "for(int i=0; i<%lu; i++){\n",
                mdsc->get_nmemb());
        if(mdsc->get_field_type() == Type_INT64 ||
                mdsc->get_field_type() == Type_UINT64) {
            vlg::ascii_string tmp_lnx;
            fprintf(file, "%s", VLG_COMP_DPND_strict_linux);
            RETURN_IF_NOT_OK(printf_percent_from_VLG_TYPE(*mdsc, tmp_lnx, true))
            fprintf(file, CR_2IND CR_2IND CR_2IND
                    "blen += fprintf(f, \"%%%s\", %s[i]);\n",
                    tmp_lnx.internal_buff(),
                    mdsc->get_member_name());
            fprintf(file, "#else\n");
            fprintf(file, CR_2IND CR_2IND CR_2IND
                    "blen += fprintf(f, \"%%%s\", %s[i]);\n",
                    tmp.internal_buff(),
                    mdsc->get_member_name());
            fprintf(file, "#endif\n");
        } else {
            fprintf(file, CR_2IND CR_2IND CR_2IND
                    "blen += fprintf(f, \"%%%s\", %s[i]);\n",
                    tmp.internal_buff(),
                    mdsc->get_member_name());
        }
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "if(%lu>i+1) blen += fprintf(f, \",\");\n",
                mdsc->get_nmemb());
        fprintf(file, CR_2IND CR_2IND
                "}\n");
        fprintf(file, CR_2IND CR_2IND
                "blen += fprintf(f, \"]\");\n");
        fprintf(file, CR_2IND"}\n");
    } else {
        if(mdsc->get_field_type() == Type_INT64 ||
                mdsc->get_field_type() == Type_UINT64) {
            vlg::ascii_string tmp_lnx;
            fprintf(file, "%s", VLG_COMP_DPND_strict_linux);
            RETURN_IF_NOT_OK(printf_percent_from_VLG_TYPE(*mdsc, tmp_lnx, true))
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += fprintf(f, \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp_lnx.internal_buff(),
                    mdsc->get_member_name());
            fprintf(file, "#else\n");
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += fprintf(f, \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp.internal_buff(),
                    mdsc->get_member_name());
            fprintf(file, "#endif\n");
        } else {
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += fprintf(f, \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp.internal_buff(),
                    mdsc->get_member_name());
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntityMember_FILE__CPP_
***********************************/
RetCode VLG_COMP_Gen_EntityMember_FILE__CPP_(entity_desc_comp
                                             &class_desc,
                                             member_desc_comp *ent_mdsc,
                                             entity_desc_comp *ent_desc,
                                             vlg::hash_map &entitymap,
                                             FILE *file)
{
    if(ent_mdsc->get_nmemb() > 1) {
        fprintf(file, CR_2IND
                "if(!is_zero_%s()){\n"
                CR_2IND CR_2IND
                "blen += fprintf(f, \"%s=[\");\n",
                ent_mdsc->get_member_name(),
                ent_mdsc->get_member_name());
        fprintf(file, CR_2IND CR_2IND
                "for(int i=0; i<%lu; i++){\n",
                ent_mdsc->get_nmemb());
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "blen += %s[i].pretty_dump_to_file(f, false);\n",
                ent_mdsc->get_member_name());
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "if(%lu>i+1) blen += fprintf(f, \",\");\n",
                ent_mdsc->get_nmemb());
        fprintf(file, CR_2IND CR_2IND
                "}\n");
        fprintf(file, CR_2IND CR_2IND
                "blen += fprintf(f, \"]\");\n");
        fprintf(file, CR_2IND"}\n");
    } else {
        fprintf(file, CR_2IND
                "if(!is_zero_%s()) blen += %s.pretty_dump_to_file(f, false);\n",
                ent_mdsc->get_member_name(),
                ent_mdsc->get_member_name());
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntityNotZeroMode_Buff__CPP_
***********************************/
RetCode VLG_COMP_Gen_EntityNotZeroMode_Buff__CPP_(vlg::hash_map
                                                  &entitymap,
                                                  entity_desc_comp &ent_desc,
                                                  FILE *file)
{
    vlg::ascii_string tmp;
    vlg::hash_map &mmbr_map = ent_desc.get_map_id_MMBRDSC();
    member_desc_comp *mdsc = nullptr;
    mmbr_map.start_iteration();
    while(!mmbr_map.next(nullptr, &mdsc)) {
        if(mdsc->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdsc->get_field_type() != Type_ENTITY) {
            // built in type
            if(mdsc->get_field_type() == Type_ASCII && mdsc->get_nmemb() > 1) {
                fprintf(file,
                        CR_2IND"if(!is_zero_%s()) blen += sprintf(&buff[blen], \"%s=[%%s]\", %s);\n",
                        mdsc->get_member_name(),
                        mdsc->get_member_name(),
                        mdsc->get_member_name());
            } else {
                RETURN_IF_NOT_OK(printf_percent_from_VLG_TYPE(*mdsc, tmp))
                RETURN_IF_NOT_OK(VLG_COMP_Gen_ScalarMember_Buff_CPP_(mdsc,tmp, file))
            }
        } else {
            //entity
            entity_desc_comp *fdsc = nullptr;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &fdsc)) {
                switch(fdsc->get_nentity_type()) {
                    case NEntityType_NENUM:
                        RETURN_IF_NOT_OK(tmp.assign("d"))
                        RETURN_IF_NOT_OK(VLG_COMP_Gen_ScalarMember_Buff_CPP_(mdsc, tmp, file))
                        break;
                    case NEntityType_NCLASS:
                        RETURN_IF_NOT_OK(VLG_COMP_Gen_EntityMember_Buff_CPP_(ent_desc, mdsc, fdsc,
                                                                             entitymap, file))
                        break;
                    default:
                        return vlg::RetCode_KO;
                }
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntityNotZeroMode_FILE__CPP_
***********************************/
RetCode VLG_COMP_Gen_EntityNotZeroMode_FILE__CPP_(vlg::hash_map
                                                  &entitymap,
                                                  entity_desc_comp &ent_desc,
                                                  FILE *file)
{
    vlg::ascii_string tmp;
    vlg::hash_map &mmbr_map = ent_desc.get_map_id_MMBRDSC();
    member_desc_comp *mdsc = nullptr;
    mmbr_map.start_iteration();
    while(!mmbr_map.next(nullptr, &mdsc)) {
        if(mdsc->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdsc->get_field_type() != Type_ENTITY) {
            // built in type
            if(mdsc->get_field_type() == Type_ASCII && mdsc->get_nmemb() > 1) {
                fprintf(file,
                        CR_2IND"if(!is_zero_%s()) blen += fprintf(f, \"%s=[%%s]\", %s);\n",
                        mdsc->get_member_name(),
                        mdsc->get_member_name(),
                        mdsc->get_member_name());
            } else {
                RETURN_IF_NOT_OK(printf_percent_from_VLG_TYPE(*mdsc, tmp))
                RETURN_IF_NOT_OK(VLG_COMP_Gen_ScalarMember_FILE__CPP_(mdsc,tmp, file))
            }
        } else {
            //entity
            entity_desc_comp *fdsc = nullptr;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &fdsc)) {
                switch(fdsc->get_nentity_type()) {
                    case NEntityType_NENUM:
                        RETURN_IF_NOT_OK(tmp.assign("d"))
                        RETURN_IF_NOT_OK(VLG_COMP_Gen_ScalarMember_FILE__CPP_(mdsc, tmp, file))
                        break;
                    case NEntityType_NCLASS:
                        RETURN_IF_NOT_OK(VLG_COMP_Gen_EntityMember_FILE__CPP_(ent_desc, mdsc, fdsc,
                                                                              entitymap, file))
                        break;
                    default:
                        return vlg::RetCode_KO;
                }
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntitytPrintToBuff__CPP_
***********************************/
RetCode VLG_COMP_Gen_EntitytPrintToBuff__CPP_(vlg::hash_map
                                              &entitymap,
                                              entity_desc_comp &ent_desc,
                                              FILE *file)
{
    fprintf(file, CR_1IND "size_t blen = 0;\n");
    fprintf(file, CR_1IND "if(print_name) blen += sprintf(&buff[blen], \"%s\");\n",
            ent_desc.get_nentity_name());
    //class opening curl brace
    fprintf(file, CR_1IND "blen += sprintf(&buff[blen], \"{\");\n");
    RETURN_IF_NOT_OK(VLG_COMP_Gen_EntityNotZeroMode_Buff__CPP_(entitymap,
                                                               ent_desc, file))
    //class closing curl brace
    fprintf(file, CR_1IND "blen += sprintf(&buff[blen], \"}\");\n");
    fprintf(file, CR_1IND "return blen;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntityPrintToFile__CPP_
***********************************/
RetCode VLG_COMP_Gen_EntityPrintToFile__CPP_(vlg::hash_map
                                             &entitymap,
                                             entity_desc_comp &ent_desc,
                                             FILE *file)
{
    fprintf(file, CR_1IND "size_t blen = 0;\n");
    fprintf(file, CR_1IND "if(print_name) blen += fprintf(f, \"%s\");\n",
            ent_desc.get_nentity_name());
    //class opening curl brace
    fprintf(file, CR_1IND "blen += fprintf(f, \"{\");\n");
    RETURN_IF_NOT_OK(VLG_COMP_Gen_EntityNotZeroMode_FILE__CPP_(entitymap,
                                                               ent_desc, file))
    //class closing curl brace
    fprintf(file, CR_1IND "blen += fprintf(f, \"}\");\n");
    fprintf(file, CR_1IND "return blen;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_Allc__CPP_
***********************************/
RetCode VLG_COMP_Gen_Allc__CPP_(compile_unit &cunit, FILE *file)
{
    fprintf(file,   OPN_CMMNT_LN
            "CLASS ALLOCATORS\n"
            CLS_CMMNT_LN);
    vlg::hash_map &entitymap = cunit.get_entity_map();
    entitymap.start_iteration();
    entity_desc_comp *edsc = nullptr;
    while(!entitymap.next(nullptr, &edsc)) {
        if(edsc->get_nentity_type() != NEntityType_NCLASS) {
            continue;
        }
        vlg::ascii_string nmsp;
        RETURN_IF_NOT_OK(VLG_COMP_CPP_Calc_NMspc(*edsc, nmsp))
        fprintf(file, "namespace %s{\n", nmsp.length() ? nmsp.internal_buff() : "");
        fprintf(file, "void* %s_alloc_func(size_t type_size, const void *copy)\n{\n"
                CR_1IND "void *new_ptr = new %s%s%s();\n"
                CR_1IND "if(!new_ptr){\n"
                CR_2IND "return nullptr;\n"
                CR_1IND "}\n"
                CR_1IND "return new_ptr;\n"
                "}\n",
                edsc->get_nentity_name(),
                nmsp.length() ? nmsp.internal_buff() : "",
                nmsp.length() ? "::" : "",
                edsc->get_nentity_name());
        fprintf(file, "}");
        RETURN_IF_NOT_OK(put_newline(file))
    }
    return vlg::RetCode_OK;
}

#define VLG_COMP_CPP_ENTDESC_DECL \
"vlg::nentity_desc %s_EntityDesc\n"\
"(\n"\
 CR_1IND "%u,\n"\
 CR_1IND "%lu,\n"\
 CR_1IND "%lu,\n"\
 CR_1IND "%s,\n"\
 CR_1IND "\"%s\",\n"\
 CR_1IND "%s,\n"\
 CR_1IND "%s,\n"\
 CR_1IND "%u,\n"\
 CR_1IND "%s\n"\
");\n"

#define VLG_COMP_CPP_MMBRDESC_DECL \
"vlg::member_desc %s\n"  \
"(\n" \
 CR_1IND"%u,\n" \
 CR_1IND"%s,\n" \
 CR_1IND"\"%s\",\n" \
 CR_1IND"\"%s\",\n" \
 CR_1IND"%s,\n" \
 CR_1IND"0x%lx,\n" \
 CR_1IND"%lu,\n" \
 CR_1IND"%lu,\n" \
 CR_1IND"%u,\n" \
 CR_1IND"\"%s\",\n" \
 CR_1IND"%s,\n" \
 CR_1IND"%ld\n" \
");\n"

/***********************************
GEN- VLG_COMP_Gen_Descriptors__CPP_
***********************************/
RetCode VLG_COMP_Gen_Descriptors__CPP_(compile_unit &cunit,
                                       FILE *file)
{
    vlg::hash_map &entitymap = cunit.get_entity_map();
    entitymap.start_iteration();
    entity_desc_comp *edsc = nullptr;
    while(!entitymap.next(nullptr, &edsc)) {
        //create ent desc
        vlg::ascii_string ent_name, allcf;
        const char *ent_type = nullptr;
        ent_type = string_from_NEntityType(edsc->get_nentity_type());
        RETURN_IF_NOT_OK(ent_name.assign("\""))
        RETURN_IF_NOT_OK(ent_name.append(edsc->get_nentity_name()))
        RETURN_IF_NOT_OK(ent_name.append("\""))
        vlg::ascii_string nmsp;
        RETURN_IF_NOT_OK(VLG_COMP_CPP_Calc_NMspc(*edsc, nmsp))
        switch(edsc->get_nentity_type()) {
            case NEntityType_NENUM:
                fprintf(file,   OPN_CMMNT_LN
                        "ENUM %s DESC\n"
                        CLS_CMMNT_LN, edsc->get_nentity_name());
                fprintf(file, VLG_COMP_CPP_ENTDESC_DECL,   edsc->get_nentity_name(),
                        edsc->get_entityid(),
                        0UL,  //EntitySize not significant for enum
                        0UL,  //EntityMaxAlign not significant for enum
                        ent_type,
                        nmsp.length() ? nmsp.internal_buff() : "",
                        ent_name.internal_buff(),
                        "0",
                        0U,
                        "false");
                break;
            case NEntityType_NCLASS:
                fprintf(file,   OPN_CMMNT_LN
                        "CLASS %s DESC\n"
                        CLS_CMMNT_LN, edsc->get_nentity_name());
                RETURN_IF_NOT_OK(allcf.assign(nmsp.length() ? nmsp.internal_buff() : ""))
                RETURN_IF_NOT_OK(allcf.append("::"))
                RETURN_IF_NOT_OK(allcf.append(edsc->get_nentity_name()))
                RETURN_IF_NOT_OK(allcf.append("_alloc_func"))
                fprintf(file, "%s", VLG_COMP_DPND_x86_64_win_MSVC);
                fprintf(file, VLG_COMP_CPP_ENTDESC_DECL, edsc->get_nentity_name(),
                        edsc->get_entityid(),
                        edsc->get_size(VLG_COMP_ARCH_x86_64,
                                       VLG_COMP_OS_win,
                                       VLG_COMP_LANG_CPP,
                                       VLG_COMP_TCOMP_MSVC),
                        edsc->get_entity_max_align(VLG_COMP_ARCH_x86_64,
                                                   VLG_COMP_OS_win,
                                                   VLG_COMP_LANG_CPP,
                                                   VLG_COMP_TCOMP_MSVC),
                        ent_type,
                        nmsp.length() ? nmsp.internal_buff() : "",
                        ent_name.internal_buff(),
                        allcf.internal_buff(),
                        edsc->get_field_num(),
                        edsc->is_persistent() ? "true" : "false");
                fprintf(file, "#endif\n");
                fprintf(file, "%s", VLG_COMP_DPND_x86_64_unix_GCC);
                fprintf(file, VLG_COMP_CPP_ENTDESC_DECL, edsc->get_nentity_name(),
                        edsc->get_entityid(),
                        edsc->get_size(VLG_COMP_ARCH_x86_64,
                                       VLG_COMP_OS_unix,
                                       VLG_COMP_LANG_CPP,
                                       VLG_COMP_TCOMP_GCC),
                        edsc->get_entity_max_align(VLG_COMP_ARCH_x86_64,
                                                   VLG_COMP_OS_unix,
                                                   VLG_COMP_LANG_CPP,
                                                   VLG_COMP_TCOMP_GCC),
                        ent_type,
                        nmsp.length() ? nmsp.internal_buff() : "",
                        ent_name.internal_buff(),
                        allcf.internal_buff(),
                        edsc->get_field_num(),
                        edsc->is_persistent() ? "true" : "false");
                fprintf(file, "#endif\n");
                break;
            default:
                return vlg::RetCode_KO;
        }
        RETURN_IF_NOT_OK(put_newline(file))
        //create ent desc end
        vlg::ascii_string mmbr_tmp_str;
        vlg::hash_map &mmbr_map = edsc->get_map_id_MMBRDSC();
        mmbr_map.start_iteration();
        member_desc_comp *mdsc = nullptr;
        while(!mmbr_map.next(nullptr, &mdsc)) {
            if(mdsc->get_member_type() != MemberType_FIELD &&
                    mdsc->get_member_type() != MemberType_NENUM_VALUE) {
                continue;
            }
            RETURN_IF_NOT_OK(mmbr_tmp_str.assign(edsc->get_nentity_name()))
            RETURN_IF_NOT_OK(mmbr_tmp_str.append("_"))
            RETURN_IF_NOT_OK(mmbr_tmp_str.append(mdsc->get_member_name()))
            const char *mmbr_type = nullptr;
            mmbr_type = string_from_MemberType(mdsc->get_member_type());
            const char *mmbr_fld_type = nullptr;
            mmbr_fld_type = string_from_Type(mdsc->get_field_type());
            const char *mmbr_fld_ent_type = nullptr;
            mmbr_fld_ent_type = string_from_NEntityType(mdsc->get_field_entity_type());
            fprintf(file, "%s", VLG_COMP_DPND_x86_64_win_MSVC);
            fprintf(file, VLG_COMP_CPP_MMBRDESC_DECL,  mmbr_tmp_str.internal_buff(),
                    mdsc->get_member_id(),
                    mmbr_type,
                    mdsc->get_member_name(),
                    !mdsc->get_member_desc() ? "" : mdsc->get_member_desc(),
                    mmbr_fld_type,
                    mdsc->get_field_offset(VLG_COMP_ARCH_x86_64,
                                           VLG_COMP_OS_win,
                                           VLG_COMP_LANG_CPP,
                                           VLG_COMP_TCOMP_MSVC),
                    mdsc->get_field_type_size(VLG_COMP_ARCH_x86_64,
                                              VLG_COMP_OS_win,
                                              VLG_COMP_LANG_CPP,
                                              VLG_COMP_TCOMP_MSVC),
                    mdsc->get_nmemb(),
                    mdsc->get_field_nclassid(),
                    !mdsc->get_field_usr_str_type() ? "" : mdsc->get_field_usr_str_type(),
                    mmbr_fld_ent_type,
                    mdsc->get_enum_value());
            fprintf(file, "#endif\n");
            fprintf(file, "%s", VLG_COMP_DPND_x86_64_unix_GCC);
            fprintf(file, VLG_COMP_CPP_MMBRDESC_DECL,  mmbr_tmp_str.internal_buff(),
                    mdsc->get_member_id(),
                    mmbr_type,
                    mdsc->get_member_name(),
                    !mdsc->get_member_desc() ? "" : mdsc->get_member_desc(),
                    mmbr_fld_type,
                    mdsc->get_field_offset(VLG_COMP_ARCH_x86_64,
                                           VLG_COMP_OS_unix,
                                           VLG_COMP_LANG_CPP,
                                           VLG_COMP_TCOMP_GCC),
                    mdsc->get_field_type_size(VLG_COMP_ARCH_x86_64,
                                              VLG_COMP_OS_unix,
                                              VLG_COMP_LANG_CPP,
                                              VLG_COMP_TCOMP_GCC),
                    mdsc->get_nmemb(),
                    mdsc->get_field_nclassid(),
                    !mdsc->get_field_usr_str_type() ? "" : mdsc->get_field_usr_str_type(),
                    mmbr_fld_ent_type,
                    mdsc->get_enum_value());
            fprintf(file, "#endif\n");
        }
        //key descriptors
        if(edsc->get_nentity_type() == NEntityType_NCLASS && edsc->is_persistent()) {
            vlg::hash_map &kdesc_map = edsc->get_map_keyid_KDESC_mod();
            kdesc_map.start_iteration();
            key_desc_comp *kdesc = nullptr;
            while(!kdesc_map.next(nullptr, &kdesc)) {
                fprintf(file, "vlg::key_desc KEY_%s_%d", edsc->get_nentity_name(),
                        kdesc->get_key_id());
                RETURN_IF_NOT_OK(put_newline(file))
                fprintf(file, "(\n");
                fprintf(file, CR_1IND"%d,\n", kdesc->get_key_id());
                fprintf(file, CR_1IND"%s\n", kdesc->is_primary() ? "true" : "false");
                fprintf(file, ");\n");
                RETURN_IF_NOT_OK(put_newline(file))
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_GenMeths__CPP_
***********************************/
RetCode VLG_COMP_Gen_GenMeths__CPP_(compile_unit &cunit,
                                    entity_desc_comp &edsc,
                                    FILE *file)
{
    fprintf(file, OPN_CMMNT_LN "Getter(s) / Setter(s) / is_zero(s)\n" CLS_CMMNT_LN);
    vlg::hash_map &entitymap = cunit.get_entity_map();
    vlg::hash_map &mmbr_map = edsc.get_map_id_MMBRDSC();
    mmbr_map.start_iteration();
    member_desc_comp *mdsc = nullptr;
    while(!mmbr_map.next(nullptr, &mdsc)) {
        if(mdsc->get_member_type() != MemberType_FIELD) {
            continue;
        }
        /******************************************
        Getter
        ******************************************/
        vlg::ascii_string type_str;
        RETURN_IF_NOT_OK(target_type_from_VLG_TYPE(*mdsc, entitymap, type_str))
        vlg::ascii_string meth_name;
        RETURN_IF_NOT_OK(meth_name.assign(VLG_COMP_CPP_GETTER_PFX"_"))
        vlg::ascii_string fld_name;
        RETURN_IF_NOT_OK(fld_name.assign(mdsc->get_member_name()))
        //fld_name.first_char_uppercase();
        RETURN_IF_NOT_OK(meth_name.append(fld_name))
        if(mdsc->get_field_type() == Type_ENTITY) {
            entity_desc_comp *inner_edsc = nullptr;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &inner_edsc)) {
                if(inner_edsc->get_nentity_type() == NEntityType_NENUM) {
                    if(mdsc->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"%s ", type_str.internal_buff());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"%s* ", type_str.internal_buff());
                    }
                } else {
                    if(mdsc->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"%s* ", type_str.internal_buff());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"%s* ", type_str.internal_buff());
                    }
                }
            }
        } else if(mdsc->get_nmemb() > 1) {
            //primitive type nmemb > 1
            fprintf(file, EXPORT_SYMBOL"%s* ", type_str.internal_buff());
        } else {
            //primitive type nmemb == 1
            fprintf(file, EXPORT_SYMBOL"%s ", type_str.internal_buff());
        }
        fprintf(file, "%s::%s()\n", edsc.get_nentity_name(), meth_name.internal_buff());
        if(mdsc->get_field_type() == Type_ENTITY) {
            entity_desc_comp *edsc = nullptr;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &edsc)) {
                if(edsc->get_nentity_type() == NEntityType_NENUM && mdsc->get_nmemb() == 1) {
                    //enum  nmemb == 1
                    fprintf(file, "{\n" CR_1IND"return %s;\n}\n", mdsc->get_member_name());
                } else if(mdsc->get_nmemb() > 1) {
                    //class, struct o enum nmemb > 1
                    fprintf(file, "{\n" CR_1IND"return &%s[0];\n}\n", mdsc->get_member_name());
                } else {
                    //class, struct && nmemb == 1
                    fprintf(file, "{\n" CR_1IND"return &%s;\n}\n",  mdsc->get_member_name());
                }
            }
        } else if(mdsc->get_nmemb() > 1) {
            //primitive type nmemb > 1
            fprintf(file, "{\n" CR_1IND"return &%s[0];\n}\n",  mdsc->get_member_name());
        } else {
            //primitive type nmemb == 1
            fprintf(file, "{\n" CR_1IND"return %s;\n}\n",  mdsc->get_member_name());
        }
        /******************************************
        Getter method idx.
        ******************************************/
        if(mdsc->get_nmemb() > 1) {
            RETURN_IF_NOT_OK(meth_name.append("_idx"))
            if(mdsc->get_field_type() == Type_ENTITY) {
                entity_desc_comp *edsc = nullptr;
                if(!entitymap.get(mdsc->get_field_usr_str_type(), &edsc)) {
                    if(edsc->get_nentity_type() == NEntityType_NENUM) {
                        //enum
                        fprintf(file, EXPORT_SYMBOL"%s ", type_str.internal_buff());
                    } else {
                        //class, struct
                        fprintf(file, EXPORT_SYMBOL"%s* ", type_str.internal_buff());
                    }
                }
            } else {
                //primitive type
                fprintf(file, EXPORT_SYMBOL"%s ", type_str.internal_buff());
            }
            fprintf(file, "%s::%s(size_t idx)\n", edsc.get_nentity_name(),
                    meth_name.internal_buff());
            if(mdsc->get_field_type() == Type_ENTITY) {
                entity_desc_comp *edsc = nullptr;
                if(!entitymap.get(mdsc->get_field_usr_str_type(), &edsc)) {
                    if(edsc->get_nentity_type() == NEntityType_NENUM) {
                        //enum
                        fprintf(file, "{\n" CR_1IND"return %s[idx];\n}\n",  mdsc->get_member_name());
                    } else {
                        //class, struct
                        fprintf(file, "{\n" CR_1IND"return &%s[idx];\n}\n",  mdsc->get_member_name());
                    }
                }
            } else {
                //primitive type
                fprintf(file, "{\n" CR_1IND"return %s[idx];\n}\n",  mdsc->get_member_name());
            }
        }
        /******************************************
        Setter
        ******************************************/
        RETURN_IF_NOT_OK(meth_name.assign(VLG_COMP_CPP_SETTER_PFX"_"))
        RETURN_IF_NOT_OK(meth_name.append(fld_name))
        if(mdsc->get_field_type() == Type_ENTITY) {
            entity_desc_comp *inner_edsc = nullptr;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &inner_edsc)) {
                if(inner_edsc->get_nentity_type() == NEntityType_NENUM) {
                    if(mdsc->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(%s val)\n",
                                edsc.get_nentity_name(),
                                meth_name.internal_buff(),
                                type_str.internal_buff());
                        fprintf(file, "{\n" CR_1IND"%s = val;\n}\n", mdsc->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(const %s *val)\n",
                                edsc.get_nentity_name(),
                                meth_name.internal_buff(),
                                type_str.internal_buff());
                        fprintf(file,
                                "{\n" CR_1IND"memcpy((void*)&%s[0], (void*)val, sizeof(%s)*%lu);\n}\n",
                                mdsc->get_member_name(),
                                type_str.internal_buff(),
                                mdsc->get_nmemb());
                    }
                } else {
                    if(mdsc->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(const %s *val)\n",
                                edsc.get_nentity_name(),
                                meth_name.internal_buff(),
                                type_str.internal_buff());
                        fprintf(file, "{\n" CR_1IND"memcpy((void*)&%s, (void*)val, sizeof(%s));\n}\n",
                                mdsc->get_member_name(),
                                type_str.internal_buff());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(const %s *val)\n",
                                edsc.get_nentity_name(),
                                meth_name.internal_buff(),
                                type_str.internal_buff());
                        fprintf(file,
                                "{\n" CR_1IND"memcpy((void*)&%s[0], (void*)val, sizeof(%s)*%lu);\n}\n",
                                mdsc->get_member_name(),
                                type_str.internal_buff(),
                                mdsc->get_nmemb());
                    }
                }
            }
        } else if(mdsc->get_nmemb() == 1) {
            //primitive type nmemb == 1
            fprintf(file, EXPORT_SYMBOL"void %s::%s(%s val)\n",
                    edsc.get_nentity_name(),
                    meth_name.internal_buff(),
                    type_str.internal_buff());

            fprintf(file, "{\n" CR_1IND"%s = val;\n}\n", mdsc->get_member_name());
        } else {
            //primitive type nmemb > 1
            fprintf(file, EXPORT_SYMBOL"void %s::%s(const %s *val)\n",
                    edsc.get_nentity_name(),
                    meth_name.internal_buff(),
                    type_str.internal_buff());

            if(mdsc->get_field_type() != Type_ASCII) {
                fprintf(file,
                        "{\n" CR_1IND"memcpy((void*)&%s[0], (void*)val, sizeof(%s)*%lu);\n}\n",
                        mdsc->get_member_name(),
                        type_str.internal_buff(),
                        mdsc->get_nmemb());
            } else {
                fprintf(file,
                        "{\n" CR_1IND"memcpy((void*)&%s[0], (void*)val, sizeof(%s)*%lu-1);\n}\n",
                        mdsc->get_member_name(),
                        type_str.internal_buff(),
                        mdsc->get_nmemb());
            }
        }
        /******************************************
        Setter method idx.
        ******************************************/
        if(mdsc->get_nmemb() > 1) {
            RETURN_IF_NOT_OK(meth_name.append("_idx"))
            if(mdsc->get_field_type() == Type_ENTITY) {
                entity_desc_comp *inner_edsc = nullptr;
                if(!entitymap.get(mdsc->get_field_usr_str_type(), &inner_edsc)) {
                    if(inner_edsc->get_nentity_type() == NEntityType_NENUM) {
                        //enum
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(size_t idx, %s val)\n",
                                edsc.get_nentity_name(),
                                meth_name.internal_buff(),
                                type_str.internal_buff());
                        fprintf(file, "{\n" CR_1IND"%s[idx] = val;\n}\n", mdsc->get_member_name());
                    } else {
                        //class, struct
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(size_t idx, const %s *val)\n",
                                edsc.get_nentity_name(),
                                meth_name.internal_buff(),
                                type_str.internal_buff());
                        fprintf(file,
                                "{\n" CR_1IND"memcpy((void*)&%s[idx], (void*)val, sizeof(%s));\n}\n",
                                mdsc->get_member_name(),
                                type_str.internal_buff());
                    }
                }
            } else {
                //primitive type
                fprintf(file, EXPORT_SYMBOL"void %s::%s(size_t idx, %s val)\n",
                        edsc.get_nentity_name(),
                        meth_name.internal_buff(),
                        type_str.internal_buff());
                fprintf(file, "{\n" CR_1IND"%s[idx] = val;\n}\n", mdsc->get_member_name());
            }
        }
        /******************************************
        Zero Method
        ******************************************/
        fprintf(file, EXPORT_SYMBOL"bool %s::is_zero_%s() const\n",
                edsc.get_nentity_name(), mdsc->get_member_name());
        if(mdsc->get_field_type() == Type_ENTITY) {
            entity_desc_comp *inner_desc = nullptr;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &inner_desc)) {
                if(inner_desc->get_nentity_type() == NEntityType_NCLASS) {
                    vlg::ascii_string body;
                    RETURN_IF_NOT_OK(body.append("{"))
                    if(mdsc->get_nmemb() > 1) {
                        RETURN_IF_NOT_OK(body.append("\n" CR_1IND"for(int i = 0; i<%d; i++){\n"))
                        RETURN_IF_NOT_OK(body.append(CR_2IND"if(!%s[i].is_zero()) return false;\n"))
                        RETURN_IF_NOT_OK(body.append(CR_1IND"}\n"))
                        RETURN_IF_NOT_OK(body.append(CR_1IND"return true;\n}\n"))
                        fprintf(file, body.internal_buff(), mdsc->get_nmemb(), mdsc->get_member_name());
                    } else {
                        RETURN_IF_NOT_OK(body.append("\n" CR_1IND"return %s.is_zero();\n}\n"))
                        fprintf(file, body.internal_buff(), mdsc->get_member_name());
                    }
                } else {
                    //struct and enum can be treated as primitive types
                    fprintf(file,
                            "{\n" CR_1IND"return !memcmp(&%s%s, &ZERO_OBJ.%s%s, sizeof(%s)*%lu);\n}\n",
                            mdsc->get_member_name(),
                            (mdsc->get_nmemb() > 1) ? "[0]" : "",
                            mdsc->get_member_name(),
                            (mdsc->get_nmemb() > 1) ? "[0]" : "",
                            type_str.internal_buff(),
                            mdsc->get_nmemb());
                }
            } else {
                EXIT_ACTION_STDOUT(ENT_DESC_NOT_FND)
            }
        } else {
            //primitive type
            fprintf(file,
                    "{\n" CR_1IND"return !memcmp(&%s%s, &ZERO_OBJ.%s%s, sizeof(%s)*%lu);\n}\n",
                    mdsc->get_member_name(),
                    (mdsc->get_nmemb() > 1) ? "[0]" : "",
                    mdsc->get_member_name(),
                    (mdsc->get_nmemb() > 1) ? "[0]" : "",
                    type_str.internal_buff(),
                    mdsc->get_nmemb());
        }
        /******************************************
        Zero Method idx.
        ******************************************/
        if(mdsc->get_nmemb() > 1) {
            fprintf(file, EXPORT_SYMBOL"bool %s::is_zero_%s_idx(size_t idx) const\n",
                    edsc.get_nentity_name(),
                    mdsc->get_member_name());
            if(mdsc->get_field_type() == Type_ENTITY) {
                entity_desc_comp *inner_desc = nullptr;
                if(!entitymap.get(mdsc->get_field_usr_str_type(), &inner_desc)) {
                    if(inner_desc->get_nentity_type() == NEntityType_NCLASS) {
                        vlg::ascii_string body;
                        RETURN_IF_NOT_OK(body.append("{\n"))
                        RETURN_IF_NOT_OK(body.append(CR_1IND"return %s[idx].is_zero();\n}\n"))
                        fprintf(file, body.internal_buff(), mdsc->get_member_name());
                    } else {
                        //struct and enum can be treated as primitive types
                        fprintf(file,
                                "{\n" CR_1IND"return !memcmp(&%s%s, &ZERO_OBJ.%s%s, sizeof(%s));\n}\n",
                                mdsc->get_member_name(),
                                "[idx]",
                                mdsc->get_member_name(),
                                "[0]",
                                type_str.internal_buff());
                    }
                } else {
                    EXIT_ACTION_STDOUT(ENT_DESC_NOT_FND)
                }
            } else {
                //primitive type
                fprintf(file,
                        "{\n" CR_1IND"return !memcmp(&%s%s, &ZERO_OBJ.%s%s, sizeof(%s));\n}\n",
                        mdsc->get_member_name(),
                        "[idx]",
                        mdsc->get_member_name(),
                        "[0]",
                        type_str.internal_buff());
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_ZeroObj__CPP_
***********************************/
RetCode VLG_COMP_Gen_ZeroObj__CPP_(vlg::hash_map &entitymap,
                                   entity_desc_comp &class_desc,
                                   FILE *file)
{
    fprintf(file,   OPN_CMMNT_LN
            "ZERO %s OBJ\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, "const %s %s::ZERO_OBJ;\n\n", class_desc.get_nentity_name(),
            class_desc.get_nentity_name());
    return vlg::RetCode_OK;
}


/***********************************
GEN- VLG_COMP_StructCtor__CPP_
***********************************/
RetCode VLG_COMP_Struct_Ctor__CPP_(entity_desc_comp *edsc,
                                   FILE *file)
{
    fprintf(file,   OPN_CMMNT_LN
            "STRUCT %s CTOR\n"
            CLS_CMMNT_LN, edsc->get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"%s::%s()\n{\n"
            CR_1IND"memset(this, 0, sizeof(%s));\n}\n", edsc->get_nentity_name(),
            edsc->get_nentity_name(),
            edsc->get_nentity_name());
    RETURN_IF_NOT_OK(put_newline(file))
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Class_Ctor__CPP_
***********************************/
RetCode VLG_COMP_Class_Ctor__CPP_(compile_unit &cunit,
                                  entity_desc_comp *edsc,
                                  FILE *file)
{
    fprintf(file, OPN_CMMNT_LN
            "CLASS %s CTOR\n"
            CLS_CMMNT_LN, edsc->get_nentity_name());
    vlg::hash_map &entitymap = cunit.get_entity_map();
    //class_rep ctor BEGIN
    vlg::hash_map &mmbr_map = edsc->get_map_id_MMBRDSC();
    member_desc_comp *mdsc = nullptr;
    mmbr_map.start_iteration();
    //ctor
    vlg::ascii_string ctor_init_lst, def_val;
    bool first = true, put_coma = false;
    RETURN_IF_NOT_OK(ctor_init_lst.assign("()"))
    while(!mmbr_map.next(nullptr, &mdsc)) {
        RetCode res = vlg::RetCode_OK;
        if(mdsc->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdsc->get_nmemb() > 1) {
            continue;
        }
        if(!(res = get_zero_val_for_VLG_TYPE(mdsc->get_field_type(), def_val))) {
            //builtin type
            if(first) {
                RETURN_IF_NOT_OK(ctor_init_lst.append(":"))
                first = false;
            }
            if(!put_coma) {
                put_coma = true;
            } else {
                RETURN_IF_NOT_OK(ctor_init_lst.append(","))
            }
            RETURN_IF_NOT_OK(ctor_init_lst.append("\n"))
            RETURN_IF_NOT_OK(ctor_init_lst.append(CR_1IND))
            RETURN_IF_NOT_OK(ctor_init_lst.append(mdsc->get_member_name()))
            RETURN_IF_NOT_OK(ctor_init_lst.append("("))
            RETURN_IF_NOT_OK(ctor_init_lst.append(def_val))
            RETURN_IF_NOT_OK(ctor_init_lst.append(")"))
        } else if(res == vlg::RetCode_KO) {
            //entity
            //no need to explicit call def ctor.
        }
    }
    fprintf(file, EXPORT_SYMBOL"%s::%s%s\n", edsc->get_nentity_name(),
            edsc->get_nentity_name(),
            ctor_init_lst.internal_buff());
    vlg::ascii_string ctor_body;
    RETURN_IF_NOT_OK(ctor_body.append("{"))
    mmbr_map.start_iteration();
    first = true;
    while(!mmbr_map.next(nullptr, &mdsc)) {
        if(mdsc->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdsc->get_field_type() == Type_ENTITY) {
            entity_desc_comp *fedsc = nullptr;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &fedsc)) {
                if(fedsc->get_nentity_type() == NEntityType_NENUM) {
                    if(first) {
                        RETURN_IF_NOT_OK(ctor_body.append("\n"))
                        first = false;
                    }
                    if(mdsc->get_nmemb() == 1) {
                        RETURN_IF_NOT_OK(ctor_body.append(CR_1IND"memset(&"))
                        RETURN_IF_NOT_OK(ctor_body.append(mdsc->get_member_name()))
                        RETURN_IF_NOT_OK(ctor_body.append(", 0, sizeof(ENUM_B_SZ));\n"))
                    } else {
                        RETURN_IF_NOT_OK(ctor_body.append(CR_1IND"memset(&"))
                        RETURN_IF_NOT_OK(ctor_body.append(mdsc->get_member_name()))
                        RETURN_IF_NOT_OK(ctor_body.append("[0], 0, sizeof("))
                        RETURN_IF_NOT_OK(ctor_body.append(mdsc->get_member_name()))
                        RETURN_IF_NOT_OK(ctor_body.append("));\n"))
                    }
                }
            } else {
                EXIT_ACTION_STDOUT(ENT_DESC_NOT_FND)
            }
        } else {
            if(mdsc->get_nmemb() == 1) {
                continue;
            }
            if(first) {
                RETURN_IF_NOT_OK(ctor_body.append("\n"))
                first = false;
            }
            RETURN_IF_NOT_OK(ctor_body.append(CR_1IND"memset(&"))
            RETURN_IF_NOT_OK(ctor_body.append(mdsc->get_member_name()))
            RETURN_IF_NOT_OK(ctor_body.append("[0], 0, sizeof("))
            RETURN_IF_NOT_OK(ctor_body.append(mdsc->get_member_name()))
            RETURN_IF_NOT_OK(ctor_body.append("));\n"))
        }
    }
    RETURN_IF_NOT_OK(ctor_body.append("}"))
    fprintf(file, "%s", ctor_body.internal_buff());
    RETURN_IF_NOT_OK(put_newline(file))
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Class_Dtor__CPP_
***********************************/
RetCode VLG_COMP_Class_Dtor__CPP_(compile_unit &cunit,
                                  entity_desc_comp *edsc,
                                  FILE *file)
{
    fprintf(file, EXPORT_SYMBOL"%s::~%s(){}\n", edsc->get_nentity_name(),
            edsc->get_nentity_name());
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_IsZero_Body__CPP_
***********************************/
RetCode VLG_COMP_Gen_IsZero_Body__CPP_(vlg::hash_map &entitymap,
                                       entity_desc_comp &class_desc,
                                       FILE *file)
{
    vlg::hash_map &mmbr_map = class_desc.get_map_id_MMBRDSC();
    member_desc_comp *mdsc = nullptr;
    mmbr_map.start_iteration();
    while(!mmbr_map.next(nullptr, &mdsc)) {
        if(mdsc->get_member_type() != MemberType_FIELD) {
            continue;
        }
        fprintf(file, CR_1IND"if(!is_zero_%s())\n", mdsc->get_member_name());
        fprintf(file, CR_2IND"return false;\n");
    }
    fprintf(file, CR_1IND"return true;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_VirtualMeths__CPP_
***********************************/
RetCode VLG_COMP_Gen_VirtualMeths__CPP_(vlg::hash_map &entitymap,
                                        entity_desc_comp &class_desc,
                                        FILE *file)
{
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s get_nclass_id.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"unsigned int %s::get_nclass_id() const\n{\n",
            class_desc.get_nentity_name());
    fprintf(file,   CR_1IND "return %d;\n", class_desc.get_entityid());
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s get_compiler_version.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"unsigned int %s::get_compiler_version() const\n{\n",
            class_desc.get_nentity_name());
    fprintf(file,   CR_1IND "return %d;\n", 0);
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s get_size.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"size_t %s::get_size() const\n{\n",
            class_desc.get_nentity_name());
    fprintf(file,   CR_1IND "return sizeof(%s);\n", class_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s get_zero_object.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"const %s* %s::get_zero_object() const\n{\n",
            class_desc.get_nentity_name(),
            class_desc.get_nentity_name());
    fprintf(file,   CR_1IND "return &ZERO_OBJ;\n");
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s copy_to.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"void %s::copy_to(nclass *out) const\n{\n",
            class_desc.get_nentity_name());
    fprintf(file,   CR_1IND "memcpy((void*)(out), (void*)(this), sizeof(%s));\n",
            class_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s clone.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"%s* %s::clone() const\n{\n",
            class_desc.get_nentity_name(), class_desc.get_nentity_name());
    fprintf(file,   CR_1IND "%s *newptr = nullptr;\n"\
            CR_1IND "COMMAND_IF_NULL(newptr = new %s(), exit(1))\n"\
            CR_1IND "copy_to(newptr);\n"\
            CR_1IND "return newptr;\n", class_desc.get_nentity_name(),
            class_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s is_zero.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"bool %s::is_zero() const\n{\n",
            class_desc.get_nentity_name());
    RETURN_IF_NOT_OK(VLG_COMP_Gen_IsZero_Body__CPP_(entitymap, class_desc, file))
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s set_zero.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"void %s::set_zero()\n{\n",
            class_desc.get_nentity_name());
    fprintf(file, CR_1IND"memcpy((void*)(this), (void*)(&ZERO_OBJ), sizeof(%s));\n",
            class_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s set_from.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"void %s::set_from(const nclass *obj)\n{\n",
            class_desc.get_nentity_name());
    fprintf(file, CR_1IND"memcpy((void*)(this), (void*)(obj), sizeof(%s));\n",
            class_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s get_nentity_descriptor.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file,
            EXPORT_SYMBOL"const vlg::nentity_desc* %s::get_nentity_descriptor() const\n{\n",
            class_desc.get_nentity_name());
    fprintf(file, CR_1IND"return &%s_EntityDesc;\n", class_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s pretty_dump_to_buffer.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file,
            EXPORT_SYMBOL"size_t %s::pretty_dump_to_buffer(char *buff, bool print_name) const\n{\n",
            class_desc.get_nentity_name());
    RETURN_IF_NOT_OK(VLG_COMP_Gen_EntitytPrintToBuff__CPP_(entitymap, class_desc,
                                                           file))
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s pretty_dump_to_file.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file,
            EXPORT_SYMBOL"size_t %s::pretty_dump_to_file(FILE *f, bool print_name) const\n{\n",
            class_desc.get_nentity_name());
    RETURN_IF_NOT_OK(VLG_COMP_Gen_EntityPrintToFile__CPP_(entitymap, class_desc,
                                                          file))
    fprintf(file, "}\n");
    fprintf(file,   OPN_CMMNT_LN
            "CLASS %s serialize.\n"
            CLS_CMMNT_LN, class_desc.get_nentity_name());
    fprintf(file,
            EXPORT_SYMBOL"int %s::serialize(vlg::Encode enctyp, const nclass *prev_image, vlg::grow_byte_buffer *obb) const\n{\n",
            class_desc.get_nentity_name());
    RETURN_IF_NOT_OK(VLG_COMP_Gen_Serialize_new__CPP_(entitymap, class_desc,
                                                      file))
    fprintf(file, "}\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_ClassImpl__CPP_
***********************************/
RetCode VLG_COMP_Gen_ClassImpl__CPP_(compile_unit &cunit, FILE *file)
{
    vlg::hash_map &entitymap = cunit.get_entity_map();
    entitymap.start_iteration();
    entity_desc_comp *edsc = nullptr;
    while(!entitymap.next(nullptr, &edsc)) {
        if(edsc->get_nentity_type() != NEntityType_NCLASS) {
            continue;
        }
        vlg::ascii_string nmsp;
        RETURN_IF_NOT_OK(VLG_COMP_CPP_Calc_NMspc(*edsc, nmsp))
        fprintf(file, "namespace %s{\n", nmsp.internal_buff());
        //ctor
        RETURN_IF_NOT_OK(VLG_COMP_Class_Ctor__CPP_(cunit, edsc, file))
        //dtor
        RETURN_IF_NOT_OK(VLG_COMP_Class_Dtor__CPP_(cunit, edsc, file))
        RETURN_IF_NOT_OK(put_newline(file))
        //zero obj
        RETURN_IF_NOT_OK(VLG_COMP_Gen_ZeroObj__CPP_(entitymap, *edsc, file))
        //gen meths.
        RETURN_IF_NOT_OK(VLG_COMP_Gen_GenMeths__CPP_(cunit, *edsc, file))
        //struct base meths.
        fprintf(file, OPN_CMMNT_LN
                "CLASS %s VIRTUAL METHS.\n"
                CLS_CMMNT_LN, edsc->get_nentity_name());
        RETURN_IF_NOT_OK(VLG_COMP_Gen_VirtualMeths__CPP_(entitymap, *edsc, file))
        fprintf(file, "}\n");
    }
    return vlg::RetCode_OK;
}

extern int comp_ver[4];

/***********************************
GEN- VLG_COMP_Gen_ModelVersion__CPP_
***********************************/
RetCode VLG_COMP_Gen_ModelVersion__CPP_(compile_unit &cunit,
                                        FILE *file)
{
    fprintf(file,   OPN_CMMNT_LN
            "MODEL:%s VERSION\n"
            CLS_CMMNT_LN, cunit.model_name());
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, "const char* get_mdl_ver_%s()\n", cunit.model_name());
    fprintf(file, "{\n");
    fprintf(file,
            CR_1IND"return \"model.%s.ver.%s.compiler.ver.%d.%d.%d.%d.date:%s\";\n",
            cunit.model_name(),
            cunit.model_version(),
            comp_ver[0],
            comp_ver[1],
            comp_ver[2],
            comp_ver[3],
            __DATE__);
    fprintf(file, "}\n");
    fprintf(file, "}\n\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_ClassManager
***********************************/
RetCode VLG_COMP_Gen_ClassManager__CPP_(compile_unit &cunit,
                                        FILE *file)
{
    vlg::ascii_string name;
    vlg::ascii_string_tok tknz;
    RETURN_IF_NOT_OK(tknz.init(cunit.get_file_name()))
    RETURN_IF_NOT_OK(tknz.next_token(name, VLG_COMP_DOT))
    fprintf(file,   OPN_CMMNT_LN
            "MODEL:%s NEM\n"
            CLS_CMMNT_LN, cunit.model_name());
    fprintf(file, "vlg::nentity_manager NEM_%s;\n", cunit.model_name());
    return vlg::RetCode_OK;
}

#define VLG_COMP_CPP_ENTRY_PT_OPN \
"vlg::nentity_manager* get_em_%s()\n"\
"{\n"

#define VLG_COMP_CPP_ENTRY_PT_CLS \
 CR_1IND "return &NEM_%s;\n"\
"}\n"

#define VLG_COMP_CPP_ADD_MMBRDESC \
 CR_1IND "COMMAND_IF_NOT_OK(%s_EntityDesc.add_member_desc(&%s), exit(1))\n"\

#define VLG_COMP_CPP_ADD_ENTDESC \
 CR_1IND "COMMAND_IF_NOT_OK(NEM_%s.extend(&%s_EntityDesc), exit(1))\n"\

/***********************************
GEN- VLG_COMP_Gen_EntryPoint
***********************************/
RetCode VLG_COMP_Gen_EntryPoint__CPP_(compile_unit &cunit, FILE *file)
{
    vlg::ascii_string name;
    vlg::ascii_string_tok tknz;
    RETURN_IF_NOT_OK(tknz.init(cunit.get_file_name()))
    RETURN_IF_NOT_OK(tknz.next_token(name, VLG_COMP_DOT))
    fprintf(file,   OPN_CMMNT_LN
            "MODEL:%s ENTRYPOINT\n"
            CLS_CMMNT_LN, cunit.model_name());
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, EXPORT_SYMBOL VLG_COMP_CPP_ENTRY_PT_OPN, cunit.model_name());
    vlg::hash_map &entitymap = cunit.get_entity_map();
    entitymap.start_iteration();
    entity_desc_comp *edsc = nullptr;
    while(!entitymap.next(nullptr, &edsc)) {
        vlg::hash_map &mmbr_map = edsc->get_map_id_MMBRDSC();
        mmbr_map.start_iteration();
        member_desc_comp *mdsc = nullptr;
        while(!mmbr_map.next(nullptr, &mdsc)) {
            if(mdsc->get_member_type() != MemberType_FIELD) {
                continue;
            }
            vlg::ascii_string mmbr_name;
            RETURN_IF_NOT_OK(mmbr_name.assign(edsc->get_nentity_name()))
            RETURN_IF_NOT_OK(mmbr_name.append("_"))
            RETURN_IF_NOT_OK(mmbr_name.append(mdsc->get_member_name()))
            fprintf(file, VLG_COMP_CPP_ADD_MMBRDESC, edsc->get_nentity_name(),
                    mmbr_name.internal_buff());
        }
        //key descriptors
        if(edsc->get_nentity_type() == NEntityType_NCLASS && edsc->is_persistent()) {
            vlg::hash_map &kdesc_map = edsc->get_map_keyid_KDESC_mod();
            kdesc_map.start_iteration();
            key_desc_comp *kdesc = nullptr;
            while(!kdesc_map.next(nullptr, &kdesc)) {
                member_desc_comp *k_mdsc = nullptr;
                vlg::linked_list &kset = kdesc->get_key_member_set_m();
                kset.start_iteration();
                while(!kset.next(&k_mdsc)) {
                    vlg::ascii_string k_mmbr_name;
                    RETURN_IF_NOT_OK(k_mmbr_name.assign(edsc->get_nentity_name()))
                    RETURN_IF_NOT_OK(k_mmbr_name.append("_"))
                    RETURN_IF_NOT_OK(k_mmbr_name.append(k_mdsc->get_member_name()))
                    fprintf(file,
                            CR_1IND"COMMAND_IF_NOT_OK(KEY_%s_%d.add_member_desc(&%s), exit(1))\n",
                            edsc->get_nentity_name(),
                            kdesc->get_key_id(),
                            k_mmbr_name.internal_buff());
                }
                fprintf(file,
                        CR_1IND"COMMAND_IF_NOT_OK(%s_EntityDesc.add_key_desc(&KEY_%s_%d), exit(1))\n",
                        edsc->get_nentity_name(),
                        edsc->get_nentity_name(),
                        kdesc->get_key_id());
            }
        }
        fprintf(file, VLG_COMP_CPP_ADD_ENTDESC, cunit.model_name(),
                edsc->get_nentity_name());
    }
    fprintf(file, VLG_COMP_CPP_ENTRY_PT_CLS, cunit.model_name());
    fprintf(file, "}\n\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntryPoint_C__CPP_
***********************************/
RetCode VLG_COMP_Gen_EntryPoint_C__CPP_(compile_unit &cunit,
                                        FILE *file)
{
    fprintf(file, OPN_CMMNT_LN"MODEL:%s C ENTRYPOINT\n" CLS_CMMNT_LN, cunit.model_name());
    fprintf(file, "typedef void *nentity_manager_wr;\n");
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, "nentity_manager_wr get_c_em_%s()\n{\n", cunit.model_name());
    fprintf(file, CR_1IND"return (nentity_manager_wr)get_em_%s();\n", cunit.model_name());
    fprintf(file, "}\n");
    fprintf(file, "}\n\n");
    return vlg::RetCode_OK;
}

#define VLG_SER_SZS "\
#define ENTLN_B_SZ 4\n\
#define FLDLN_B_SZ 4\n\
#define ARRAY_B_SZ 4\n\
#define ARIDX_B_SZ 2\n\
#define MMBID_B_SZ 2\n\
#define ENUM_B_SZ 4\n\
"
#define C_F_N_OK "\
#define COMMAND_IF_NOT_OK(fun, cmd)\\\n\
{\\\n\
    int res;\\\n\
    if((res = fun)){\\\n\
        cmd;\\\n\
    }\\\n\
}\n\
"
#define C_F_NULL "\
#define COMMAND_IF_NULL(ptr, cmd)\\\n\
{\\\n\
    if(!(ptr)){\\\n\
        cmd;\\\n\
    }\\\n\
}\n\
"

/***********************************
GEN- VLG_COMP_Gen__CPP_
***********************************/
RetCode VLG_COMP_Gen__CPP_(compile_unit &cunit)
{
    vlg::ascii_string fname;
    vlg::ascii_string_tok tknz;
    RETURN_IF_NOT_OK(tknz.init(cunit.get_file_name()))
    RETURN_IF_NOT_OK(tknz.next_token(fname, VLG_COMP_DOT))
    RETURN_IF_NOT_OK(fname.append(VLG_COMP_DOT))
    RETURN_IF_NOT_OK(fname.append("cpp"))
    FILE *file = nullptr;
    COMMAND_IF_NOT_OK(open_output_file(fname.internal_buff(), &file),
                      EXIT_ACTION)
    //header
    RETURN_IF_NOT_OK(render_hdr(cunit, fname, file))
    RETURN_IF_NOT_OK(put_newline(file))
    vlg::ascii_string hname;
    vlg::ascii_string_tok htknz;
    RETURN_IF_NOT_OK(htknz.init(cunit.get_file_name()))
    RETURN_IF_NOT_OK(htknz.next_token(hname, VLG_COMP_DOT))
    RETURN_IF_NOT_OK(hname.append(VLG_COMP_DOT))
    RETURN_IF_NOT_OK(hname.append("h"))
    //include
    fprintf(file, "#include \"%s\"\n\n", hname.internal_buff());

    //macros
    fprintf(file, VLG_SER_SZS"\n");
    fprintf(file, C_F_N_OK"\n");
    fprintf(file, C_F_NULL"\n");

    //decl. g. b. buff.
    RETURN_IF_NOT_OK(VLG_COMP_Gen_Grow_Byte_Buffer__CPP_(file))

    //alloc fun for classes
    RETURN_IF_NOT_OK(VLG_COMP_Gen_Allc__CPP_(cunit, file))
    RETURN_IF_NOT_OK(put_newline(file))
    //entity descriptors
    RETURN_IF_NOT_OK(VLG_COMP_Gen_Descriptors__CPP_(cunit, file))
    RETURN_IF_NOT_OK(put_newline(file))
    //class impl.
    RETURN_IF_NOT_OK(VLG_COMP_Gen_ClassImpl__CPP_(cunit, file))
    RETURN_IF_NOT_OK(put_newline(file))
    //unit class mng.
    RETURN_IF_NOT_OK(VLG_COMP_Gen_ClassManager__CPP_(cunit, file))
    RETURN_IF_NOT_OK(put_newline(file))
    //model version.
    RETURN_IF_NOT_OK(VLG_COMP_Gen_ModelVersion__CPP_(cunit, file))
    RETURN_IF_NOT_OK(put_newline(file))
    //model entry point.
    RETURN_IF_NOT_OK(VLG_COMP_Gen_EntryPoint__CPP_(cunit, file))
    RETURN_IF_NOT_OK(put_newline(file))
    //model C entry point.
    RETURN_IF_NOT_OK(VLG_COMP_Gen_EntryPoint_C__CPP_(cunit, file))
    RETURN_IF_NOT_OK(put_newline(file))
    fclose(file);
    return vlg::RetCode_OK;
}

}
