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

#include "compiler.h"

#define ENT_DESC_NOT_FND "entity descriptor not found, exiting.."

#define VLG_COMP_DPND_x86_64_win_MSVC "#if defined WIN32 && defined _MSC_VER\n"
#define VLG_COMP_DPND_x86_64_unix_GCC "#if !defined WIN32 && defined __GNUG__\n"
#define VLG_COMP_DPND_strict_linux "#if defined __GNUG__ && defined __linux\n"

namespace vlg {

extern RetCode VLG_COMP_CPP_Calc_NMspc(entity_desc_comp &edesc,
                                       std::string &out);

#define BBUF_DECL \
"namespace vlg { struct g_bbuf {\n"\
 CR_1IND"explicit g_bbuf();\n"\
 CR_1IND"~g_bbuf();\n"\
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
GEN- VLG_COMP_Gen_g_bbuf__CPP_
***********************************/
RetCode VLG_COMP_Gen_g_bbuf__CPP_(FILE *file)
{
    fprintf(file, OPN_CMMNT_LN "Dynamic Byte Buffer\n" CLS_CMMNT_LN);
    fprintf(file, BBUF_DECL);
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_Serialize_new__CPP_
***********************************/
RetCode VLG_COMP_Gen_Serialize_new__CPP_(std::map<std::string, entity_desc_comp *> &entitymap,
                                         entity_desc_comp &edesc,
                                         FILE *file)
{
    //fprintf(file, VLG_COMP_1IND"unsigned short fidx = 0;\n");
    fprintf(file, CR_1IND"size_t tlen_offst = obb->position(), tlen = 0;\n");
    fprintf(file, CR_1IND"switch(enctyp){\n");
    fprintf(file, CR_2IND"case vlg::Encode_INDEXED_NOT_ZERO:\n");
    fprintf(file, CR_3IND"obb->advance_pos_write(ENTLN_B_SZ); tlen = obb->position();\n");
    auto &mmbrmap = edesc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() != Type_ENTITY) {
            //primitive type
            if(mdesc->second->get_field_type() == Type_ASCII && mdesc->second->get_nmemb() > 1) {
                //for strings we send only significative bytes
                fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdesc->second->get_member_name());
                fprintf(file, CR_4IND"size_t fsize = strnlen(%s, %zu);\n",
                        mdesc->second->get_member_name(), get_network_type_size(mdesc->second->get_field_type())
                        * mdesc->second->get_nmemb());
                //allocating space for: field index, field length, field size
                fprintf(file, CR_4IND"obb->grow(MMBID_B_SZ+FLDLN_B_SZ+fsize);\n");
                fprintf(file, CR_4IND"obb->append_ushort(%d);\n", mdesc->second->get_member_id());
                fprintf(file, CR_4IND"obb->append_uint((unsigned int)fsize);\n");
                fprintf(file, CR_4IND"obb->append(%s, 0, fsize);\n" CR_3IND"}\n", mdesc->second->get_member_name());
#if 0
            } else if(mdesc->second->get_field_type() == Type_BYTE && mdesc->second->get_nmemb() > 1) {
                //for byte buffers we send the entire buffer if not zero.
                fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdesc->second->get_member_name());
                fprintf(file, CR_4IND"obb->grow(MMBID_B_SZ+%zu);\n", get_network_type_size(mdesc->second->get_field_type()));
                fprintf(file, CR_4IND"obb->append_ushort(%d);\n", mdesc->second->get_member_id());
                fprintf(file, CR_4IND"obb->append(&%s, 0, %zu);\n" CR_3IND"}\n", mdesc->second->get_member_name(),
                        (get_network_type_size(mdesc->second->get_field_type()) * mdesc->second->get_nmemb()));
#endif
            } else {
                if(mdesc->second->get_nmemb() == 1) {
                    //primitive type nmemb == 1
                    fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdesc->second->get_member_name());
                    fprintf(file, CR_4IND"obb->grow(MMBID_B_SZ+%zu);\n", get_network_type_size(mdesc->second->get_field_type()));
                    fprintf(file, CR_4IND"obb->append_ushort(%d);\n", mdesc->second->get_member_id());
                    fprintf(file, CR_4IND"obb->append(&%s, 0, %zu);\n" CR_3IND"}\n", mdesc->second->get_member_name(),
                            get_network_type_size(mdesc->second->get_field_type()));
                } else {
                    //primitive type nmemb > 1
                    fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdesc->second->get_member_name());
                    fprintf(file, CR_4IND"obb->append_ushort(%d);\n", mdesc->second->get_member_id());
                    fprintf(file, CR_4IND"size_t alen_offst = obb->position();\n");
                    fprintf(file, CR_4IND"obb->advance_pos_write(ARRAY_B_SZ); size_t alen = obb->position();\n");
                    fprintf(file, CR_4IND"for(int i = 0; i<%zu; i++){\n", mdesc->second->get_nmemb());
                    fprintf(file, CR_5IND"if(!is_zero_%s_idx(i)){\n", mdesc->second->get_member_name());
                    fprintf(file, CR_6IND"obb->append_ushort(i);\n");
                    fprintf(file, CR_6IND"obb->append(&%s[i], 0, %zu); }\n", mdesc->second->get_member_name(),
                            get_network_type_size(mdesc->second->get_field_type()));
                    fprintf(file, CR_5IND"}\n");
                    fprintf(file, CR_5IND"alen = (obb->position() - alen); obb->put(&alen, alen_offst, ARRAY_B_SZ);\n");
                    fprintf(file, CR_4IND"}\n");
                }
            }
        } else {
            auto fdesc = entitymap.end();
            if((fdesc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(fdesc->second->get_nentity_type() == NEntityType_NENUM) {
                    //enum, treat this as primitive type
                    size_t fsize = ENUM_B_SZ;
                    if(mdesc->second->get_nmemb() == 1) {
                        //primitive type nmemb == 1
                        fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdesc->second->get_member_name());
                        fprintf(file, CR_4IND"obb->grow(MMBID_B_SZ+%zu);\n", fsize);
                        fprintf(file, CR_4IND"obb->append_ushort(%d);\n", mdesc->second->get_member_id());
                        fprintf(file, CR_4IND"obb->append(&%s, 0, %zu);\n" CR_3IND"}\n", mdesc->second->get_member_name(), fsize);
                    } else {
                        //primitive type nmemb > 1
                        fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdesc->second->get_member_name());
                        fprintf(file, CR_4IND"obb->append_ushort(%d); //put fidx.\n", mdesc->second->get_member_id());
                        fprintf(file, CR_4IND"size_t alen_offst = obb->position();\n");
                        fprintf(file, CR_4IND"obb->advance_pos_write(ARRAY_B_SZ); size_t alen = obb->position();\n");
                        fprintf(file, CR_4IND"for(int i = 0; i<%zu; i++){\n", mdesc->second->get_nmemb());
                        fprintf(file, CR_5IND"if(!is_zero_%s_idx(i)){\n", mdesc->second->get_member_name());
                        fprintf(file, CR_6IND"obb->append_ushort(i);\n");
                        fprintf(file, CR_6IND"obb->append(&%s[i], 0, %zu); }\n", mdesc->second->get_member_name(), fsize);
                        fprintf(file, CR_5IND"}\n");
                        fprintf(file, CR_5IND"alen = (obb->position() - alen); obb->put(&alen, alen_offst, ARRAY_B_SZ);\n");
                        fprintf(file, CR_4IND"}\n");
                    }
                } else {
                    //struct, class
                    if(mdesc->second->get_nmemb() == 1) {
                        fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdesc->second->get_member_name());
                        fprintf(file, CR_4IND"obb->append_ushort(%d);\n", mdesc->second->get_member_id());
                        fprintf(file, CR_4IND"%s.serialize(enctyp, nullptr, obb); }\n", mdesc->second->get_member_name());
                    } else {
                        fprintf(file, CR_3IND"if(!is_zero_%s()){\n", mdesc->second->get_member_name());
                        fprintf(file, CR_4IND"obb->append_ushort(%d);\n", mdesc->second->get_member_id());
                        fprintf(file, CR_4IND"size_t alen_offst = obb->position();\n");
                        fprintf(file, CR_4IND"obb->advance_pos_write(ARRAY_B_SZ); size_t alen = obb->position();\n");
                        fprintf(file, CR_4IND"for(int i = 0; i<%zu; i++){\n", mdesc->second->get_nmemb());
                        fprintf(file, CR_5IND"if(!is_zero_%s_idx(i)){\n", mdesc->second->get_member_name());
                        fprintf(file, CR_6IND"obb->append_ushort(i);\n");
                        fprintf(file, CR_6IND"%s[i].serialize(enctyp, nullptr, obb); }\n", mdesc->second->get_member_name());
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
                                            std::string &tmp,
                                            FILE *file)
{
    if(mdsc->get_nmemb() > 1) {
        fprintf(file, CR_2IND"if(!is_zero_%s()){\n" CR_2IND CR_2IND
                "blen += sprintf(&buff[blen], \"%s=[\");\n",
                mdsc->get_member_name(),
                mdsc->get_member_name());
        fprintf(file, CR_2IND
                CR_2IND
                "for(int i=0; i<%zu; i++){\n",
                mdsc->get_nmemb());
        if(mdsc->get_field_type() == Type_INT64 ||
                mdsc->get_field_type() == Type_UINT64) {
            std::string tmp_lnx;
            fprintf(file, "%s", VLG_COMP_DPND_strict_linux);
            RET_ON_KO(printf_percent_from_VLG_TYPE(*mdsc, tmp_lnx, true))
            fprintf(file, CR_2IND CR_2IND CR_2IND
                    "blen += sprintf(&buff[blen], \"%%%s\", %s[i]);\n",
                    tmp_lnx.c_str(),
                    mdsc->get_member_name());
            fprintf(file, "#else\n");
            fprintf(file, CR_2IND CR_2IND CR_2IND
                    "blen += sprintf(&buff[blen], \"%%%s\", %s[i]);\n",
                    tmp.c_str(),
                    mdsc->get_member_name());
            fprintf(file, "#endif\n");
        } else {
            fprintf(file, CR_2IND CR_2IND CR_2IND
                    "blen += sprintf(&buff[blen], \"%%%s\", %s[i]);\n",
                    tmp.c_str(),
                    mdsc->get_member_name());
        }
        fprintf(file, CR_2IND
                CR_2IND
                CR_2IND
                "if(%zu>i+1) blen += sprintf(&buff[blen], \",\");\n",
                mdsc->get_nmemb());
        fprintf(file, CR_2IND
                CR_2IND
                "}\n");
        fprintf(file,  CR_2IND
                CR_2IND
                "blen += sprintf(&buff[blen], \"]\");\n");
        fprintf(file, CR_2IND"}\n");
    } else {
        if(mdsc->get_field_type() == Type_INT64 ||
                mdsc->get_field_type() == Type_UINT64) {
            std::string tmp_lnx;
            fprintf(file, "%s", VLG_COMP_DPND_strict_linux);
            RET_ON_KO(printf_percent_from_VLG_TYPE(*mdsc, tmp_lnx, true))
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += sprintf(&buff[blen], \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp_lnx.c_str(),
                    mdsc->get_member_name());
            fprintf(file, "#else\n");
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += sprintf(&buff[blen], \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp.c_str(),
                    mdsc->get_member_name());
            fprintf(file, "#endif\n");
        } else {
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += sprintf(&buff[blen], \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp.c_str(),
                    mdsc->get_member_name());
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntityMember_Buff_CPP_
***********************************/
RetCode VLG_COMP_Gen_EntityMember_Buff_CPP_(entity_desc_comp &nclass_desc,
                                            member_desc_comp *ent_mdsc,
                                            entity_desc_comp *ent_desc,
                                            std::map<std::string, entity_desc_comp *> &entitymap,
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
                "for(int i=0; i<%zu; i++){\n",
                ent_mdsc->get_nmemb());
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "blen += %s[i].pretty_dump_to_buffer(&buff[blen], false);\n",
                ent_mdsc->get_member_name());
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "if(%zu>i+1) blen += sprintf(&buff[blen], \",\");\n",
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
                                             std::string &tmp,
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
                "for(int i=0; i<%zu; i++){\n",
                mdsc->get_nmemb());
        if(mdsc->get_field_type() == Type_INT64 ||
                mdsc->get_field_type() == Type_UINT64) {
            std::string tmp_lnx;
            fprintf(file, "%s", VLG_COMP_DPND_strict_linux);
            RET_ON_KO(printf_percent_from_VLG_TYPE(*mdsc, tmp_lnx, true))
            fprintf(file, CR_2IND CR_2IND CR_2IND
                    "blen += fprintf(f, \"%%%s\", %s[i]);\n",
                    tmp_lnx.c_str(),
                    mdsc->get_member_name());
            fprintf(file, "#else\n");
            fprintf(file, CR_2IND CR_2IND CR_2IND
                    "blen += fprintf(f, \"%%%s\", %s[i]);\n",
                    tmp.c_str(),
                    mdsc->get_member_name());
            fprintf(file, "#endif\n");
        } else {
            fprintf(file, CR_2IND CR_2IND CR_2IND
                    "blen += fprintf(f, \"%%%s\", %s[i]);\n",
                    tmp.c_str(),
                    mdsc->get_member_name());
        }
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "if(%zu>i+1) blen += fprintf(f, \",\");\n",
                mdsc->get_nmemb());
        fprintf(file, CR_2IND CR_2IND
                "}\n");
        fprintf(file, CR_2IND CR_2IND
                "blen += fprintf(f, \"]\");\n");
        fprintf(file, CR_2IND"}\n");
    } else {
        if(mdsc->get_field_type() == Type_INT64 ||
                mdsc->get_field_type() == Type_UINT64) {
            std::string tmp_lnx;
            fprintf(file, "%s", VLG_COMP_DPND_strict_linux);
            RET_ON_KO(printf_percent_from_VLG_TYPE(*mdsc, tmp_lnx, true))
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += fprintf(f, \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp_lnx.c_str(),
                    mdsc->get_member_name());
            fprintf(file, "#else\n");
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += fprintf(f, \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp.c_str(),
                    mdsc->get_member_name());
            fprintf(file, "#endif\n");
        } else {
            fprintf(file,
                    CR_2IND"if(!is_zero_%s()) blen += fprintf(f, \"%s=%%%s|\", %s);\n",
                    mdsc->get_member_name(),
                    mdsc->get_member_name(),
                    tmp.c_str(),
                    mdsc->get_member_name());
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntityMember_FILE__CPP_
***********************************/
RetCode VLG_COMP_Gen_EntityMember_FILE__CPP_(entity_desc_comp &nclass_desc,
                                             member_desc_comp *ent_mdsc,
                                             entity_desc_comp *ent_desc,
                                             std::map<std::string, entity_desc_comp *> &entitymap,
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
                "for(int i=0; i<%zu; i++){\n",
                ent_mdsc->get_nmemb());
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "blen += %s[i].pretty_dump_to_file(f, false);\n",
                ent_mdsc->get_member_name());
        fprintf(file, CR_2IND CR_2IND CR_2IND
                "if(%zu>i+1) blen += fprintf(f, \",\");\n",
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
RetCode VLG_COMP_Gen_EntityNotZeroMode_Buff__CPP_(std::map<std::string, entity_desc_comp *> &entitymap,
                                                  entity_desc_comp &ent_desc,
                                                  FILE *file)
{
    std::string tmp;
    auto &mmbrmap = ent_desc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() != Type_ENTITY) {
            // built in type
            if(mdesc->second->get_field_type() == Type_ASCII && mdesc->second->get_nmemb() > 1) {
                fprintf(file, CR_2IND"if(!is_zero_%s()) blen += sprintf(&buff[blen], \"%s=[%%s]\", %s);\n",
                        mdesc->second->get_member_name(),
                        mdesc->second->get_member_name(),
                        mdesc->second->get_member_name());
            } else {
                RET_ON_KO(printf_percent_from_VLG_TYPE(*mdesc->second, tmp))
                RET_ON_KO(VLG_COMP_Gen_ScalarMember_Buff_CPP_(mdesc->second,tmp, file))
            }
        } else {
            //entity
            auto fdesc = entitymap.end();
            if((fdesc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                switch(fdesc->second->get_nentity_type()) {
                    case NEntityType_NENUM:
                        tmp.assign("d");
                        RET_ON_KO(VLG_COMP_Gen_ScalarMember_Buff_CPP_(mdesc->second, tmp, file))
                        break;
                    case NEntityType_NCLASS:
                        RET_ON_KO(VLG_COMP_Gen_EntityMember_Buff_CPP_(ent_desc,
                                                                      mdesc->second,
                                                                      fdesc->second,
                                                                      entitymap,
                                                                      file))
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
RetCode VLG_COMP_Gen_EntityNotZeroMode_FILE__CPP_(std::map<std::string, entity_desc_comp *> &entitymap,
                                                  entity_desc_comp &ent_desc,
                                                  FILE *file)
{
    std::string tmp;
    auto &mmbrmap = ent_desc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() != Type_ENTITY) {
            // built in type
            if(mdesc->second->get_field_type() == Type_ASCII && mdesc->second->get_nmemb() > 1) {
                fprintf(file, CR_2IND"if(!is_zero_%s()) blen += fprintf(f, \"%s=[%%s]\", %s);\n",
                        mdesc->second->get_member_name(),
                        mdesc->second->get_member_name(),
                        mdesc->second->get_member_name());
            } else {
                RET_ON_KO(printf_percent_from_VLG_TYPE(*mdesc->second, tmp))
                RET_ON_KO(VLG_COMP_Gen_ScalarMember_FILE__CPP_(mdesc->second,tmp, file))
            }
        } else {
            //entity
            auto fdesc = entitymap.end();
            if((fdesc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                switch(fdesc->second->get_nentity_type()) {
                    case NEntityType_NENUM:
                        tmp.assign("d");
                        RET_ON_KO(VLG_COMP_Gen_ScalarMember_FILE__CPP_(mdesc->second, tmp, file))
                        break;
                    case NEntityType_NCLASS:
                        RET_ON_KO(VLG_COMP_Gen_EntityMember_FILE__CPP_(ent_desc,
                                                                       mdesc->second,
                                                                       fdesc->second,
                                                                       entitymap,
                                                                       file))
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
RetCode VLG_COMP_Gen_EntitytPrintToBuff__CPP_(std::map<std::string, entity_desc_comp *> &entitymap,
                                              entity_desc_comp &ent_desc,
                                              FILE *file)
{
    fprintf(file, CR_1IND "size_t blen = 0;\n");
    fprintf(file, CR_1IND "if(print_name) blen += sprintf(&buff[blen], \"%s\");\n", ent_desc.get_nentity_name());
    //class opening curl brace
    fprintf(file, CR_1IND "blen += sprintf(&buff[blen], \"{\");\n");
    RET_ON_KO(VLG_COMP_Gen_EntityNotZeroMode_Buff__CPP_(entitymap, ent_desc, file))
    //class closing curl brace
    fprintf(file, CR_1IND "blen += sprintf(&buff[blen], \"}\");\n");
    fprintf(file, CR_1IND "return blen;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntityPrintToFile__CPP_
***********************************/
RetCode VLG_COMP_Gen_EntityPrintToFile__CPP_(std::map<std::string, entity_desc_comp *> &entitymap,
                                             entity_desc_comp &ent_desc,
                                             FILE *file)
{
    fprintf(file, CR_1IND "size_t blen = 0;\n");
    fprintf(file, CR_1IND "if(print_name) blen += fprintf(f, \"%s\");\n", ent_desc.get_nentity_name());
    //class opening curl brace
    fprintf(file, CR_1IND "blen += fprintf(f, \"{\");\n");
    RET_ON_KO(VLG_COMP_Gen_EntityNotZeroMode_FILE__CPP_(entitymap, ent_desc, file))
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
    fprintf(file, OPN_CMMNT_LN"NCLASS ALLOCATORS\n" CLS_CMMNT_LN);
    auto &entitymap = cunit.get_entity_map();
    for(auto edsc = entitymap.begin(); edsc != entitymap.end(); edsc++) {
        if(edsc->second->get_nentity_type() != NEntityType_NCLASS) {
            continue;
        }
        std::string nmsp;
        RET_ON_KO(VLG_COMP_CPP_Calc_NMspc(*edsc->second, nmsp))
        fprintf(file, "namespace %s{\n", nmsp.length() ? nmsp.c_str() : "");
        fprintf(file, "void* %s_alloc_func()\n{\n"
                CR_1IND "return new %s%s%s();\n"
                "}\n",
                edsc->second->get_nentity_name(),
                nmsp.length() ? nmsp.c_str() : "",
                nmsp.length() ? "::" : "",
                edsc->second->get_nentity_name());
        fprintf(file, "}");
        RET_ON_KO(put_newline(file))
    }
    return vlg::RetCode_OK;
}

#define VLG_COMP_CPP_ENTDESC_DECL \
"vlg::nentity_desc %s_EntityDesc\n"\
"(\n"\
 CR_1IND "%u,\n"\
 CR_1IND "%zu,\n"\
 CR_1IND "%zu,\n"\
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
 CR_1IND"0x%zx,\n" \
 CR_1IND"%zu,\n" \
 CR_1IND"%zu,\n" \
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
    auto &entitymap = cunit.get_entity_map();
    for(auto edsc = entitymap.begin(); edsc != entitymap.end(); edsc++) {
        //create ent desc
        std::string ent_name, allcf;
        const char *ent_type = nullptr;
        ent_type = string_from_NEntityType(edsc->second->get_nentity_type());
        ent_name.assign("\"");
        ent_name.append(edsc->second->get_nentity_name());
        ent_name.append("\"");
        std::string nmsp;
        RET_ON_KO(VLG_COMP_CPP_Calc_NMspc(*edsc->second, nmsp))
        switch(edsc->second->get_nentity_type()) {
            case NEntityType_NENUM:
                fprintf(file, OPN_CMMNT_LN "ENUM %s DESC\n" CLS_CMMNT_LN, edsc->second->get_nentity_name());
                fprintf(file, VLG_COMP_CPP_ENTDESC_DECL,
                        edsc->second->get_nentity_name(),
                        edsc->second->get_entityid(),
                        (size_t)0LLU,  //EntitySize not significant for enum
                        (size_t)0LLU,  //EntityMaxAlign not significant for enum
                        ent_type,
                        nmsp.length() ? nmsp.c_str() : "",
                        ent_name.c_str(),
                        "0",
                        0U,
                        "false");
                break;
            case NEntityType_NCLASS:
                fprintf(file, OPN_CMMNT_LN "CLASS %s DESC\n" CLS_CMMNT_LN, edsc->second->get_nentity_name());
                allcf.assign(nmsp.length() ? nmsp.c_str() : "");
                allcf.append("::");
                allcf.append(edsc->second->get_nentity_name());
                allcf.append("_alloc_func");
                fprintf(file, "%s", VLG_COMP_DPND_x86_64_win_MSVC);
                fprintf(file, VLG_COMP_CPP_ENTDESC_DECL, edsc->second->get_nentity_name(),
                        edsc->second->get_entityid(),
                        edsc->second->get_size(VLG_COMP_ARCH_x86_64,
                                               VLG_COMP_OS_win,
                                               VLG_COMP_LANG_CPP,
                                               VLG_COMP_TCOMP_MSVC),
                        edsc->second->get_entity_max_align(VLG_COMP_ARCH_x86_64,
                                                           VLG_COMP_OS_win,
                                                           VLG_COMP_LANG_CPP,
                                                           VLG_COMP_TCOMP_MSVC),
                        ent_type,
                        nmsp.length() ? nmsp.c_str() : "",
                        ent_name.c_str(),
                        allcf.c_str(),
                        edsc->second->get_field_num(),
                        edsc->second->is_persistent() ? "true" : "false");
                fprintf(file, "#endif\n");
                fprintf(file, "%s", VLG_COMP_DPND_x86_64_unix_GCC);
                fprintf(file, VLG_COMP_CPP_ENTDESC_DECL, edsc->second->get_nentity_name(),
                        edsc->second->get_entityid(),
                        edsc->second->get_size(VLG_COMP_ARCH_x86_64,
                                               VLG_COMP_OS_unix,
                                               VLG_COMP_LANG_CPP,
                                               VLG_COMP_TCOMP_GCC),
                        edsc->second->get_entity_max_align(VLG_COMP_ARCH_x86_64,
                                                           VLG_COMP_OS_unix,
                                                           VLG_COMP_LANG_CPP,
                                                           VLG_COMP_TCOMP_GCC),
                        ent_type,
                        nmsp.length() ? nmsp.c_str() : "",
                        ent_name.c_str(),
                        allcf.c_str(),
                        edsc->second->get_field_num(),
                        edsc->second->is_persistent() ? "true" : "false");
                fprintf(file, "#endif\n");
                break;
            default:
                return vlg::RetCode_KO;
        }
        RET_ON_KO(put_newline(file))
        //create ent desc end
        std::string mmbr_tmp_str;
        auto &mmbrmap = edsc->second->get_map_id_MMBRDSC();
        for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
            if(mdesc->second->get_member_type() != MemberType_FIELD &&
                    mdesc->second->get_member_type() != MemberType_NENUM_VALUE) {
                continue;
            }
            mmbr_tmp_str.assign(edsc->second->get_nentity_name());
            mmbr_tmp_str.append("_");
            mmbr_tmp_str.append(mdesc->second->get_member_name());
            const char *mmbr_type = nullptr;
            mmbr_type = string_from_MemberType(mdesc->second->get_member_type());
            const char *mmbr_fld_type = nullptr;
            mmbr_fld_type = string_from_Type(mdesc->second->get_field_type());
            const char *mmbr_fld_ent_type = nullptr;
            mmbr_fld_ent_type = string_from_NEntityType(mdesc->second->get_field_entity_type());
            fprintf(file, "%s", VLG_COMP_DPND_x86_64_win_MSVC);
            fprintf(file, VLG_COMP_CPP_MMBRDESC_DECL,  mmbr_tmp_str.c_str(),
                    mdesc->second->get_member_id(),
                    mmbr_type,
                    mdesc->second->get_member_name(),
                    !mdesc->second->get_member_desc() ? "" : mdesc->second->get_member_desc(),
                    mmbr_fld_type,
                    mdesc->second->get_field_offset(VLG_COMP_ARCH_x86_64,
                                                    VLG_COMP_OS_win,
                                                    VLG_COMP_LANG_CPP,
                                                    VLG_COMP_TCOMP_MSVC),
                    mdesc->second->get_field_type_size(VLG_COMP_ARCH_x86_64,
                                                       VLG_COMP_OS_win,
                                                       VLG_COMP_LANG_CPP,
                                                       VLG_COMP_TCOMP_MSVC),
                    mdesc->second->get_nmemb(),
                    mdesc->second->get_field_nclassid(),
                    !mdesc->second->get_field_usr_str_type() ? "" : mdesc->second->get_field_usr_str_type(),
                    mmbr_fld_ent_type,
                    mdesc->second->get_enum_value());
            fprintf(file, "#endif\n");
            fprintf(file, "%s", VLG_COMP_DPND_x86_64_unix_GCC);
            fprintf(file, VLG_COMP_CPP_MMBRDESC_DECL,  mmbr_tmp_str.c_str(),
                    mdesc->second->get_member_id(),
                    mmbr_type,
                    mdesc->second->get_member_name(),
                    !mdesc->second->get_member_desc() ? "" : mdesc->second->get_member_desc(),
                    mmbr_fld_type,
                    mdesc->second->get_field_offset(VLG_COMP_ARCH_x86_64,
                                                    VLG_COMP_OS_unix,
                                                    VLG_COMP_LANG_CPP,
                                                    VLG_COMP_TCOMP_GCC),
                    mdesc->second->get_field_type_size(VLG_COMP_ARCH_x86_64,
                                                       VLG_COMP_OS_unix,
                                                       VLG_COMP_LANG_CPP,
                                                       VLG_COMP_TCOMP_GCC),
                    mdesc->second->get_nmemb(),
                    mdesc->second->get_field_nclassid(),
                    !mdesc->second->get_field_usr_str_type() ? "" : mdesc->second->get_field_usr_str_type(),
                    mmbr_fld_ent_type,
                    mdesc->second->get_enum_value());
            fprintf(file, "#endif\n");
        }
        //key descriptors
        if(edsc->second->get_nentity_type() == NEntityType_NCLASS && edsc->second->is_persistent()) {
            auto &kdesc_map = edsc->second->get_map_keyid_KDESC_mod();
            for(auto keydesc = kdesc_map.begin(); keydesc != kdesc_map.end(); keydesc++) {
                fprintf(file, "vlg::key_desc KEY_%s_%d", edsc->second->get_nentity_name(), keydesc->second->get_key_id());
                RET_ON_KO(put_newline(file))
                fprintf(file, "(\n");
                fprintf(file, CR_1IND"%d,\n", keydesc->second->get_key_id());
                fprintf(file, CR_1IND"%s\n", keydesc->second->is_primary() ? "true" : "false");
                fprintf(file, ");\n");
                RET_ON_KO(put_newline(file))
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
    std::map<std::string, entity_desc_comp *> &entitymap = cunit.get_entity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        /******************************************
        Getter
        ******************************************/
        std::string type_str;
        RET_ON_KO(target_type_from_VLG_TYPE(*mdesc->second, entitymap, type_str))
        std::string meth_name;
        meth_name.assign(VLG_COMP_CPP_GETTER_PFX"_");
        std::string fld_name;
        fld_name.assign(mdesc->second->get_member_name());
        //fld_name.first_char_uppercase();
        meth_name.append(fld_name);
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"%s ", type_str.c_str());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"%s* ", type_str.c_str());
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"%s* ", type_str.c_str());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"%s* ", type_str.c_str());
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() > 1) {
            //primitive type nmemb > 1
            fprintf(file, EXPORT_SYMBOL"%s* ", type_str.c_str());
        } else {
            //primitive type nmemb == 1
            fprintf(file, EXPORT_SYMBOL"%s ", type_str.c_str());
        }
        fprintf(file, "%s::%s()\n", edsc.get_nentity_name(), meth_name.c_str());
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto edsc = entitymap.end();
            if((edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(edsc->second->get_nentity_type() == NEntityType_NENUM && mdesc->second->get_nmemb() == 1) {
                    //enum  nmemb == 1
                    fprintf(file, "{\n" CR_1IND"return %s;\n}\n", mdesc->second->get_member_name());
                } else if(mdesc->second->get_nmemb() > 1) {
                    //class, struct o enum nmemb > 1
                    fprintf(file, "{\n" CR_1IND"return &%s[0];\n}\n", mdesc->second->get_member_name());
                } else {
                    //class, struct && nmemb == 1
                    fprintf(file, "{\n" CR_1IND"return &%s;\n}\n",  mdesc->second->get_member_name());
                }
            }
        } else if(mdesc->second->get_nmemb() > 1) {
            //primitive type nmemb > 1
            fprintf(file, "{\n" CR_1IND"return &%s[0];\n}\n",  mdesc->second->get_member_name());
        } else {
            //primitive type nmemb == 1
            fprintf(file, "{\n" CR_1IND"return %s;\n}\n",  mdesc->second->get_member_name());
        }
        /******************************************
        Getter method idx.
        ******************************************/
        if(mdesc->second->get_nmemb() > 1) {
            meth_name.append("_idx");
            if(mdesc->second->get_field_type() == Type_ENTITY) {
                auto edsc = entitymap.end();
                if((edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                    if(edsc->second->get_nentity_type() == NEntityType_NENUM) {
                        //enum
                        fprintf(file, EXPORT_SYMBOL"%s ", type_str.c_str());
                    } else {
                        //class, struct
                        fprintf(file, EXPORT_SYMBOL"%s* ", type_str.c_str());
                    }
                }
            } else {
                //primitive type
                fprintf(file, EXPORT_SYMBOL"%s ", type_str.c_str());
            }
            fprintf(file, "%s::%s(size_t idx)\n", edsc.get_nentity_name(), meth_name.c_str());
            if(mdesc->second->get_field_type() == Type_ENTITY) {
                auto edsc = entitymap.end();
                if((edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                    if(edsc->second->get_nentity_type() == NEntityType_NENUM) {
                        //enum
                        fprintf(file, "{\n" CR_1IND"return %s[idx];\n}\n",  mdesc->second->get_member_name());
                    } else {
                        //class, struct
                        fprintf(file, "{\n" CR_1IND"return &%s[idx];\n}\n",  mdesc->second->get_member_name());
                    }
                }
            } else {
                //primitive type
                fprintf(file, "{\n" CR_1IND"return %s[idx];\n}\n",  mdesc->second->get_member_name());
            }
        }
        /******************************************
        Setter
        ******************************************/
        meth_name.assign(VLG_COMP_CPP_SETTER_PFX"_");
        meth_name.append(fld_name);
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(%s val)\n",
                                edsc.get_nentity_name(),
                                meth_name.c_str(),
                                type_str.c_str());
                        fprintf(file, "{\n" CR_1IND"%s = val;\n}\n", mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(const %s *val)\n",
                                edsc.get_nentity_name(),
                                meth_name.c_str(),
                                type_str.c_str());
                        fprintf(file, "{\n" CR_1IND"memcpy((void*)&%s[0], (void*)val, sizeof(%s)*%zu);\n}\n",
                                mdesc->second->get_member_name(),
                                type_str.c_str(),
                                mdesc->second->get_nmemb());
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(const %s *val)\n",
                                edsc.get_nentity_name(),
                                meth_name.c_str(),
                                type_str.c_str());
                        fprintf(file, "{\n" CR_1IND"memcpy((void*)&%s, (void*)val, sizeof(%s));\n}\n",
                                mdesc->second->get_member_name(),
                                type_str.c_str());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(const %s *val)\n",
                                edsc.get_nentity_name(),
                                meth_name.c_str(),
                                type_str.c_str());
                        fprintf(file, "{\n" CR_1IND"memcpy((void*)&%s[0], (void*)val, sizeof(%s)*%zu);\n}\n",
                                mdesc->second->get_member_name(),
                                type_str.c_str(),
                                mdesc->second->get_nmemb());
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            fprintf(file, EXPORT_SYMBOL"void %s::%s(%s val)\n",
                    edsc.get_nentity_name(),
                    meth_name.c_str(),
                    type_str.c_str());

            fprintf(file, "{\n" CR_1IND"%s = val;\n}\n", mdesc->second->get_member_name());
        } else {
            //primitive type nmemb > 1
            fprintf(file, EXPORT_SYMBOL"void %s::%s(const %s *val)\n",
                    edsc.get_nentity_name(),
                    meth_name.c_str(),
                    type_str.c_str());

            if(mdesc->second->get_field_type() != Type_ASCII) {
                fprintf(file, "{\n" CR_1IND"memcpy((void*)&%s[0], (void*)val, sizeof(%s)*%zu);\n}\n",
                        mdesc->second->get_member_name(),
                        type_str.c_str(),
                        mdesc->second->get_nmemb());
            } else {
                fprintf(file, "{\n" CR_1IND"memcpy((void*)&%s[0], (void*)val, sizeof(%s)*%zu-1);\n}\n",
                        mdesc->second->get_member_name(),
                        type_str.c_str(),
                        mdesc->second->get_nmemb());
            }
        }
        /******************************************
        Setter method idx.
        ******************************************/
        if(mdesc->second->get_nmemb() > 1) {
            meth_name.append("_idx");
            if(mdesc->second->get_field_type() == Type_ENTITY) {
                auto inner_edsc = entitymap.end();
                if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                    if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                        //enum
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(size_t idx, %s val)\n",
                                edsc.get_nentity_name(),
                                meth_name.c_str(),
                                type_str.c_str());
                        fprintf(file, "{\n" CR_1IND"%s[idx] = val;\n}\n", mdesc->second->get_member_name());
                    } else {
                        //class, struct
                        fprintf(file, EXPORT_SYMBOL"void %s::%s(size_t idx, const %s *val)\n",
                                edsc.get_nentity_name(),
                                meth_name.c_str(),
                                type_str.c_str());
                        fprintf(file, "{\n" CR_1IND"memcpy((void*)&%s[idx], (void*)val, sizeof(%s));\n}\n",
                                mdesc->second->get_member_name(),
                                type_str.c_str());
                    }
                }
            } else {
                //primitive type
                fprintf(file, EXPORT_SYMBOL"void %s::%s(size_t idx, %s val)\n",
                        edsc.get_nentity_name(),
                        meth_name.c_str(),
                        type_str.c_str());
                fprintf(file, "{\n" CR_1IND"%s[idx] = val;\n}\n", mdesc->second->get_member_name());
            }
        }
        /******************************************
        Zero Method
        ******************************************/
        fprintf(file, EXPORT_SYMBOL"bool %s::is_zero_%s() const\n", edsc.get_nentity_name(),
                mdesc->second->get_member_name());
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NCLASS) {
                    std::string body;
                    body.append("{");
                    if(mdesc->second->get_nmemb() > 1) {
                        body.append("\n" CR_1IND"for(int i = 0; i<%d; i++){\n");
                        body.append(CR_2IND"if(!%s[i].is_zero()) return false;\n");
                        body.append(CR_1IND"}\n");
                        body.append(CR_1IND"return true;\n}\n");
                        fprintf(file, body.c_str(), mdesc->second->get_nmemb(), mdesc->second->get_member_name());
                    } else {
                        body.append("\n" CR_1IND"return %s.is_zero();\n}\n");
                        fprintf(file, body.c_str(), mdesc->second->get_member_name());
                    }
                } else {
                    //struct and enum can be treated as primitive types
                    fprintf(file, "{\n" CR_1IND"return !memcmp(&%s%s, &ZERO_OBJ.%s%s, sizeof(%s)*%zu);\n}\n",
                            mdesc->second->get_member_name(), (mdesc->second->get_nmemb() > 1) ? "[0]" : "",
                            mdesc->second->get_member_name(), (mdesc->second->get_nmemb() > 1) ? "[0]" : "",
                            type_str.c_str(),
                            mdesc->second->get_nmemb());
                }
            } else {
                EXIT_ACTION_STDOUT(ENT_DESC_NOT_FND)
            }
        } else {
            //primitive type
            fprintf(file, "{\n" CR_1IND"return !memcmp(&%s%s, &ZERO_OBJ.%s%s, sizeof(%s)*%zu);\n}\n",
                    mdesc->second->get_member_name(),
                    (mdesc->second->get_nmemb() > 1) ? "[0]" : "", mdesc->second->get_member_name(),
                    (mdesc->second->get_nmemb() > 1) ? "[0]" : "", type_str.c_str(),
                    mdesc->second->get_nmemb());
        }
        /******************************************
        Zero Method idx.
        ******************************************/
        if(mdesc->second->get_nmemb() > 1) {
            fprintf(file, EXPORT_SYMBOL"bool %s::is_zero_%s_idx(size_t idx) const\n",
                    edsc.get_nentity_name(),
                    mdesc->second->get_member_name());
            if(mdesc->second->get_field_type() == Type_ENTITY) {
                auto inner_edsc = entitymap.end();
                if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                    if(inner_edsc->second->get_nentity_type() == NEntityType_NCLASS) {
                        std::string body;
                        body.append("{\n");
                        body.append(CR_1IND"return %s[idx].is_zero();\n}\n");
                        fprintf(file, body.c_str(), mdesc->second->get_member_name());
                    } else {
                        //struct and enum can be treated as primitive types
                        fprintf(file, "{\n" CR_1IND"return !memcmp(&%s%s, &ZERO_OBJ.%s%s, sizeof(%s));\n}\n",
                                mdesc->second->get_member_name(),
                                "[idx]",
                                mdesc->second->get_member_name(),
                                "[0]",
                                type_str.c_str());
                    }
                } else {
                    EXIT_ACTION_STDOUT(ENT_DESC_NOT_FND)
                }
            } else {
                //primitive type
                fprintf(file, "{\n" CR_1IND"return !memcmp(&%s%s, &ZERO_OBJ.%s%s, sizeof(%s));\n}\n",
                        mdesc->second->get_member_name(),
                        "[idx]",
                        mdesc->second->get_member_name(),
                        "[0]",
                        type_str.c_str());
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_ZeroObj__CPP_
***********************************/
RetCode VLG_COMP_Gen_ZeroObj__CPP_(std::map<std::string, entity_desc_comp *> &entitymap,
                                   entity_desc_comp &nclass_desc,
                                   FILE *file)
{
    fprintf(file, OPN_CMMNT_LN "ZERO %s OBJ\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, "const %s %s::ZERO_OBJ;\n\n", nclass_desc.get_nentity_name(), nclass_desc.get_nentity_name());
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Class_Ctor__CPP_
***********************************/
RetCode VLG_COMP_Class_Ctor__CPP_(compile_unit &cunit,
                                  entity_desc_comp *edsc,
                                  FILE *file)
{
    fprintf(file, OPN_CMMNT_LN "CLASS %s CTOR\n" CLS_CMMNT_LN, edsc->get_nentity_name());
    auto &entitymap = cunit.get_entity_map();
    auto &mmbrmap = edsc->get_map_id_MMBRDSC();
    std::string ctor_init_lst, def_val;
    bool first = true, put_coma = false;
    ctor_init_lst.assign("()");
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        RetCode res = vlg::RetCode_OK;
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_nmemb() > 1) {
            continue;
        }
        if(!(res = get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), def_val))) {
            //builtin type
            if(first) {
                ctor_init_lst.append(":");
                first = false;
            }
            if(!put_coma) {
                put_coma = true;
            } else {
                ctor_init_lst.append(",");
            }
            ctor_init_lst.append("\n");
            ctor_init_lst.append(CR_1IND);
            ctor_init_lst.append(mdesc->second->get_member_name());
            ctor_init_lst.append("(");
            ctor_init_lst.append(def_val);
            ctor_init_lst.append(")");
        } else if(res == vlg::RetCode_KO) {
            //entity
            //no need to explicit call def ctor.
        }
    }
    fprintf(file, EXPORT_SYMBOL"%s::%s%s\n", edsc->get_nentity_name(),
            edsc->get_nentity_name(),
            ctor_init_lst.c_str());
    std::string ctor_body;
    ctor_body.append("{");
    first = true;
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto fedsc = entitymap.end();
            if((fedsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(fedsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(first) {
                        ctor_body.append("\n");
                        first = false;
                    }
                    if(mdesc->second->get_nmemb() == 1) {
                        ctor_body.append(CR_1IND"memset(&");
                        ctor_body.append(mdesc->second->get_member_name());
                        ctor_body.append(", 0, sizeof(ENUM_B_SZ));\n");
                    } else {
                        ctor_body.append(CR_1IND"memset(&");
                        ctor_body.append(mdesc->second->get_member_name());
                        ctor_body.append("[0], 0, sizeof(");
                        ctor_body.append(mdesc->second->get_member_name());
                        ctor_body.append("));\n");
                    }
                }
            } else {
                EXIT_ACTION_STDOUT(ENT_DESC_NOT_FND)
            }
        } else {
            if(mdesc->second->get_nmemb() == 1) {
                continue;
            }
            if(first) {
                ctor_body.append("\n");
                first = false;
            }
            ctor_body.append(CR_1IND"memset(&");
            ctor_body.append(mdesc->second->get_member_name());
            ctor_body.append("[0], 0, sizeof(");
            ctor_body.append(mdesc->second->get_member_name());
            ctor_body.append("));\n");
        }
    }
    ctor_body.append("}");
    fprintf(file, "%s", ctor_body.c_str());
    RET_ON_KO(put_newline(file))
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Class_Dtor__CPP_
***********************************/
RetCode VLG_COMP_Class_Dtor__CPP_(compile_unit &cunit,
                                  entity_desc_comp *edsc,
                                  FILE *file)
{
    fprintf(file, EXPORT_SYMBOL"%s::~%s(){}\n", edsc->get_nentity_name(), edsc->get_nentity_name());
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_IsZero_Body__CPP_
***********************************/
RetCode VLG_COMP_Gen_IsZero_Body__CPP_(std::map<std::string, entity_desc_comp *> &entitymap,
                                       entity_desc_comp &nclass_desc,
                                       FILE *file)
{
    auto &mmbrmap = nclass_desc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        fprintf(file, CR_1IND"if(!is_zero_%s())\n", mdesc->second->get_member_name());
        fprintf(file, CR_2IND"return false;\n");
    }
    fprintf(file, CR_1IND"return true;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_VirtualMeths__CPP_
***********************************/
RetCode VLG_COMP_Gen_VirtualMeths__CPP_(std::map<std::string, entity_desc_comp *> &entitymap,
                                        entity_desc_comp &nclass_desc,
                                        FILE *file)
{
    fprintf(file, OPN_CMMNT_LN "CLASS %s get_id.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"unsigned int %s::get_id() const\n{\n", nclass_desc.get_nentity_name());
    fprintf(file, CR_1IND "return %d;\n", nclass_desc.get_entityid());
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s get_compiler_version.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"unsigned int %s::get_compiler_version() const\n{\n", nclass_desc.get_nentity_name());
    fprintf(file, CR_1IND "return %d;\n", 0);
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s get_size.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"size_t %s::get_size() const\n{\n", nclass_desc.get_nentity_name());
    fprintf(file, CR_1IND "return sizeof(%s);\n", nclass_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s get_zero_object.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"const %s& %s::get_zero_object() const\n{\n",
            nclass_desc.get_nentity_name(),
            nclass_desc.get_nentity_name());
    fprintf(file, CR_1IND "return ZERO_OBJ;\n");
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s copy_to.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"void %s::copy_to(nclass &out) const\n{\n", nclass_desc.get_nentity_name());
    fprintf(file, CR_1IND "memcpy((void*)(&out), (void*)(this), sizeof(%s));\n", nclass_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s clone.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"std::unique_ptr<vlg::nclass> %s::clone() const\n{\n",
            nclass_desc.get_nentity_name());
    fprintf(file,   CR_1IND "%s *newptr = nullptr;\n"\
            CR_1IND "COMMAND_IF_NULL(newptr = new %s(), exit(1))\n"\
            CR_1IND "copy_to(*newptr);\n"\
            CR_1IND "return std::unique_ptr<%s>(newptr);\n",
            nclass_desc.get_nentity_name(),
            nclass_desc.get_nentity_name(),
            nclass_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s is_zero.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"bool %s::is_zero() const\n{\n", nclass_desc.get_nentity_name());
    RET_ON_KO(VLG_COMP_Gen_IsZero_Body__CPP_(entitymap, nclass_desc, file))
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s set_zero.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"void %s::set_zero()\n{\n", nclass_desc.get_nentity_name());
    fprintf(file, CR_1IND"memcpy((void*)(this), (void*)(&ZERO_OBJ), sizeof(%s));\n", nclass_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s set_from.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"void %s::set_from(const nclass &obj)\n{\n", nclass_desc.get_nentity_name());
    fprintf(file, CR_1IND"memcpy((void*)(this), (void*)(&obj), sizeof(%s));\n", nclass_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s get_nentity_descriptor.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"const vlg::nentity_desc& %s::get_nentity_descriptor() const\n{\n",
            nclass_desc.get_nentity_name());
    fprintf(file, CR_1IND"return %s_EntityDesc;\n", nclass_desc.get_nentity_name());
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s pretty_dump_to_buffer.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"size_t %s::pretty_dump_to_buffer(char *buff, bool print_name) const\n{\n",
            nclass_desc.get_nentity_name());
    RET_ON_KO(VLG_COMP_Gen_EntitytPrintToBuff__CPP_(entitymap, nclass_desc, file))
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s pretty_dump_to_file.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file, EXPORT_SYMBOL"size_t %s::pretty_dump_to_file(FILE *f, bool print_name) const\n{\n",
            nclass_desc.get_nentity_name());
    RET_ON_KO(VLG_COMP_Gen_EntityPrintToFile__CPP_(entitymap, nclass_desc, file))
    fprintf(file, "}\n");
    fprintf(file, OPN_CMMNT_LN "CLASS %s serialize.\n" CLS_CMMNT_LN, nclass_desc.get_nentity_name());
    fprintf(file,
            EXPORT_SYMBOL"int %s::serialize(vlg::Encode enctyp, const nclass *prev_image, vlg::g_bbuf *obb) const\n{\n",
            nclass_desc.get_nentity_name());
    RET_ON_KO(VLG_COMP_Gen_Serialize_new__CPP_(entitymap, nclass_desc, file))
    fprintf(file, "}\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_ClassImpl__CPP_
***********************************/
RetCode VLG_COMP_Gen_ClassImpl__CPP_(compile_unit &cunit, FILE *file)
{
    auto &entitymap = cunit.get_entity_map();
    for(auto edsc = entitymap.begin(); edsc != entitymap.end(); edsc++) {
        if(edsc->second->get_nentity_type() != NEntityType_NCLASS) {
            continue;
        }
        std::string nmsp;
        RET_ON_KO(VLG_COMP_CPP_Calc_NMspc(*edsc->second, nmsp))
        fprintf(file, "namespace %s{\n", nmsp.c_str());
        //ctor
        RET_ON_KO(VLG_COMP_Class_Ctor__CPP_(cunit, edsc->second, file))
        //dtor
        RET_ON_KO(VLG_COMP_Class_Dtor__CPP_(cunit, edsc->second, file))
        RET_ON_KO(put_newline(file))
        //zero obj
        RET_ON_KO(VLG_COMP_Gen_ZeroObj__CPP_(entitymap, *edsc->second, file))
        //gen meths.
        RET_ON_KO(VLG_COMP_Gen_GenMeths__CPP_(cunit, *edsc->second, file))
        //struct base meths.
        fprintf(file, OPN_CMMNT_LN "CLASS %s VIRTUAL METHS.\n" CLS_CMMNT_LN, edsc->second->get_nentity_name());
        RET_ON_KO(VLG_COMP_Gen_VirtualMeths__CPP_(entitymap, *edsc->second, file))
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
    fprintf(file, OPN_CMMNT_LN "MODEL:%s VERSION\n" CLS_CMMNT_LN, cunit.model_name());
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, "const char* get_mdl_ver_%s()\n", cunit.model_name());
    fprintf(file, "{\n");
    fprintf(file, CR_1IND"return \"model.%s.ver.%s.compiler.ver.%d.%d.%d.%d.date:%s\";\n",
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
    std::string name;
    vlg::str_tok tknz(cunit.get_file_name());
    tknz.next_token(name, VLG_COMP_DOT);
    fprintf(file, OPN_CMMNT_LN "MODEL:%s NEM\n" CLS_CMMNT_LN, cunit.model_name());
    fprintf(file, "vlg::nentity_manager NEM_%s;\n", cunit.model_name());
    return vlg::RetCode_OK;
}

#define VLG_COMP_CPP_ENTRY_PT_OPN \
"vlg::nentity_manager* get_nem_%s()\n"\
"{\n"

#define VLG_COMP_CPP_ENTRY_PT_CLS \
 CR_1IND "return &NEM_%s;\n"\
"}\n"

#define VLG_COMP_CPP_ADD_MMBRDESC \
 CR_1IND "COMMAND_IF_NOT_OK(%s_EntityDesc.add_member_desc(%s), exit(1))\n"\

#define VLG_COMP_CPP_ADD_ENTDESC \
 CR_1IND "COMMAND_IF_NOT_OK(NEM_%s.extend(%s_EntityDesc), exit(1))\n"\

/***********************************
GEN- VLG_COMP_Gen_EntryPoint
***********************************/
RetCode VLG_COMP_Gen_EntryPoint__CPP_(compile_unit &cunit, FILE *file)
{
    std::string name;
    vlg::str_tok tknz(cunit.get_file_name());
    tknz.next_token(name, VLG_COMP_DOT);
    fprintf(file, OPN_CMMNT_LN "MODEL:%s ENTRYPOINT\n" CLS_CMMNT_LN, cunit.model_name());
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, EXPORT_SYMBOL VLG_COMP_CPP_ENTRY_PT_OPN, cunit.model_name());
    std::map<std::string, entity_desc_comp *> &entitymap = cunit.get_entity_map();
    for(auto edsc = entitymap.begin(); edsc != entitymap.end(); edsc++) {
        auto &mmbrmap = edsc->second->get_map_id_MMBRDSC();
        for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
            if(mdesc->second->get_member_type() != MemberType_FIELD) {
                continue;
            }
            std::string mmbr_name;
            mmbr_name.assign(edsc->second->get_nentity_name());
            mmbr_name.append("_");
            mmbr_name.append(mdesc->second->get_member_name());
            fprintf(file, VLG_COMP_CPP_ADD_MMBRDESC, edsc->second->get_nentity_name(), mmbr_name.c_str());
        }
        //key descriptors
        if(edsc->second->get_nentity_type() == NEntityType_NCLASS && edsc->second->is_persistent()) {
            auto &kdesc_map = edsc->second->get_map_keyid_KDESC_mod();
            for(auto keydesc = kdesc_map.begin(); keydesc != kdesc_map.end(); keydesc++) {
                std::set<member_desc_comp *> &kset = keydesc->second->get_key_member_set_m();
                for(auto it = kset.begin(); it != kset.end(); it++) {
                    std::string k_mmbr_name;
                    k_mmbr_name.assign(edsc->second->get_nentity_name());
                    k_mmbr_name.append("_");
                    k_mmbr_name.append((*it)->get_member_name());
                    fprintf(file, CR_1IND"COMMAND_IF_NOT_OK(KEY_%s_%d.add_member_desc(%s), exit(1))\n",
                            edsc->second->get_nentity_name(),
                            keydesc->second->get_key_id(),
                            k_mmbr_name.c_str());
                }
                fprintf(file, CR_1IND"COMMAND_IF_NOT_OK(%s_EntityDesc.add_key_desc(KEY_%s_%d), exit(1))\n",
                        edsc->second->get_nentity_name(),
                        edsc->second->get_nentity_name(),
                        keydesc->second->get_key_id());
            }
        }
        fprintf(file, VLG_COMP_CPP_ADD_ENTDESC, cunit.model_name(),
                edsc->second->get_nentity_name());
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
    fprintf(file, "typedef struct nentity_manager nentity_manager;\n");
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, "nentity_manager* get_c_nem_%s()\n{\n", cunit.model_name());
    fprintf(file, CR_1IND"return (nentity_manager*)get_nem_%s();\n", cunit.model_name());
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
    std::string fname;
    vlg::str_tok tknz(cunit.get_file_name());
    tknz.next_token(fname, VLG_COMP_DOT);
    fname.append(VLG_COMP_DOT);
    fname.append("cpp");
    FILE *file = nullptr;
    CMD_ON_KO(open_output_file(fname.c_str(), &file), EXIT_ACTION)
    //header
    RET_ON_KO(render_hdr(cunit, fname, file))
    RET_ON_KO(put_newline(file))
    std::string hname;
    vlg::str_tok htknz(cunit.get_file_name());
    htknz.next_token(hname, VLG_COMP_DOT);
    hname.append(VLG_COMP_DOT);
    hname.append("h");
    //include
    fprintf(file, "#include \"%s\"\n\n", hname.c_str());

    //macros
    fprintf(file, VLG_SER_SZS"\n");
    fprintf(file, C_F_N_OK"\n");
    fprintf(file, C_F_NULL"\n");

    //decl. g. b. buff.
    RET_ON_KO(VLG_COMP_Gen_g_bbuf__CPP_(file))

    //alloc fun for classes
    RET_ON_KO(VLG_COMP_Gen_Allc__CPP_(cunit, file))
    RET_ON_KO(put_newline(file))
    //entity descriptors
    RET_ON_KO(VLG_COMP_Gen_Descriptors__CPP_(cunit, file))
    RET_ON_KO(put_newline(file))
    //class impl.
    RET_ON_KO(VLG_COMP_Gen_ClassImpl__CPP_(cunit, file))
    RET_ON_KO(put_newline(file))
    //unit class mng.
    RET_ON_KO(VLG_COMP_Gen_ClassManager__CPP_(cunit, file))
    RET_ON_KO(put_newline(file))
    //model version.
    RET_ON_KO(VLG_COMP_Gen_ModelVersion__CPP_(cunit, file))
    RET_ON_KO(put_newline(file))
    //model entry point.
    RET_ON_KO(VLG_COMP_Gen_EntryPoint__CPP_(cunit, file))
    RET_ON_KO(put_newline(file))
    //model C entry point.
    RET_ON_KO(VLG_COMP_Gen_EntryPoint_C__CPP_(cunit, file))
    RET_ON_KO(put_newline(file))
    fclose(file);
    return vlg::RetCode_OK;
}

}
