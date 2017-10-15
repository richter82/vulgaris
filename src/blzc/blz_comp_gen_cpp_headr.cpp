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

#include "blz_compiler.h"

namespace blaze {

/***********************************
RENDER- BLZ_COMP_Render__CPP_
***********************************/
extern blaze::RetCode BLZ_COMP_Render__CPP_(compile_unit &cunit);

blaze::RetCode BLZ_COMP_CPP_Calc_NMspc(entity_desc_comp &edesc,
                                       blaze::ascii_string &out)
{
    blaze::ascii_string_tok nmtknz;
    blaze::ascii_string nmtkn, ent_nmspace;
    ent_nmspace.assign("");
    RETURN_IF_NOT_OK(nmtknz.init(edesc.get_entity_namespace()))
    bool first = true;
    while(!nmtknz.next_token(nmtkn, BLZ_COMP_DOT)) {
        if(!first) {
            RETURN_IF_NOT_OK(ent_nmspace.append("_"))
        } else {
            first = false;
        }
        RETURN_IF_NOT_OK(ent_nmspace.append(nmtkn))
    }
    RETURN_IF_NOT_OK(out.assign(ent_nmspace))
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_Define
***********************************/
blaze::RetCode BLZ_COMP_Render_Define(compile_unit &cunit, FILE *file)
{
    blaze::hash_map &definemap = cunit.get_define_map();
    definemap.start_iteration();
    char def[256], val[256];
    while(!definemap.next(def, val)) {
        fprintf(file, "#define %s %s\n", def, val);
    }
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_ModelVersion_decl__H_
***********************************/
blaze::RetCode BLZ_COMP_Render_ModelVersion_decl__H_(compile_unit &cunit,
                                                     FILE *file)
{
    blaze::ascii_string name;
    blaze::ascii_string_tok tknz;
    RETURN_IF_NOT_OK(tknz.init(cunit.get_file_name()))
    RETURN_IF_NOT_OK(tknz.next_token(name, BLZ_COMP_DOT))
    fprintf(file, OPN_CMMNT_LN
            "model version\n"
            CLS_CMMNT_LN);
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, EXPORT_SYMBOL"const char* get_mdl_ver_%s();\n",
            cunit.model_name());
    fprintf(file, "}\n\n");
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_EntryPoint_decl__H_
***********************************/
blaze::RetCode BLZ_COMP_Render_EntryPoint_decl__H_(compile_unit &cunit,
                                                   FILE *file)
{
    blaze::ascii_string name;
    blaze::ascii_string_tok tknz;
    RETURN_IF_NOT_OK(tknz.init(cunit.get_file_name()))
    RETURN_IF_NOT_OK(tknz.next_token(name, BLZ_COMP_DOT))
    fprintf(file, OPN_CMMNT_LN
            "BEM entry point\n"
            CLS_CMMNT_LN);
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, EXPORT_SYMBOL"blaze::entity_manager* get_em_%s();\n",
            cunit.model_name());
    fprintf(file, "}\n\n");
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_EntryPoint_C_decl__H_
***********************************/
blaze::RetCode BLZ_COMP_Render_EntryPoint_C_decl__H_(compile_unit &cunit,
                                                     FILE *file)
{
    blaze::ascii_string name;
    blaze::ascii_string_tok tknz;
    RETURN_IF_NOT_OK(tknz.init(cunit.get_file_name()))
    RETURN_IF_NOT_OK(tknz.next_token(name, BLZ_COMP_DOT))
    fprintf(file, "#else\n");
    fprintf(file, OPN_CMMNT_LN
            "BEM entry point C\n"
            CLS_CMMNT_LN);
    fprintf(file, "entity_manager_wr get_c_em_%s();\n", cunit.model_name());
    fprintf(file, "#endif\n");
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_Rep__H_
***********************************/
blaze::RetCode BLZ_COMP_Render_Rep__H_(blaze::hash_map &entitymap,
                                       entity_desc_comp &edesc,
                                       FILE *file)
{
    //render rep
    fprintf(file, OPN_CMMNT_LN
            "representation\n"
            CLS_CMMNT_LN);
    fprintf(file, "protected:\n");
    blaze::hash_map &mmbr_map = edesc.get_map_id_MMBRDSC();
    mmbr_map.start_iteration();
    member_desc_comp *mdsc = NULL;
    while(!mmbr_map.next(NULL, &mdsc)) {
        blaze::ascii_string type_str;
        RETURN_IF_NOT_OK(target_type_from_BLZ_TYPE(*mdsc, entitymap, type_str))
        fprintf(file,  "%s %s", type_str.internal_buff(), mdsc->get_member_name());
        if(mdsc->get_nmemb() > 1) {
            fprintf(file, "[%lu]",mdsc->get_nmemb());
        }
        fprintf(file, ";");
        RETURN_IF_NOT_OK(put_newline(file))
    }
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_ClassVirtualMeths__H_
***********************************/
blaze::RetCode BLZ_COMP_Render_ClassVirtualMeths__H_(blaze::hash_map &entitymap,
                                                     entity_desc_comp &class_desc,
                                                     FILE *file)
{
    fprintf(file, OPN_CMMNT_LN "virtual methods\n" CLS_CMMNT_LN);
    fprintf(file, EXPORT_SYMBOL"virtual unsigned int get_nclass_id() const;");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file,
            EXPORT_SYMBOL"virtual unsigned int get_compiler_version() const;");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual size_t get_entity_size() const;");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual const %s* get_zero_object() const;",
            class_desc.get_entity_name());
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file,
            EXPORT_SYMBOL"virtual void copy_to(blaze::nclass *obj) const;");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual %s* clone() const;",
            class_desc.get_entity_name());
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual bool is_zero() const;");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual void set_zero();");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file,
            EXPORT_SYMBOL"virtual void set_from(const blaze::nclass *obj);");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file,
            EXPORT_SYMBOL"virtual const blaze::entity_desc* get_entity_descriptor() const;");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file,
            EXPORT_SYMBOL"virtual size_t pretty_dump_to_buffer(char *buff, bool print_cname = true) const;");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file,
            EXPORT_SYMBOL"virtual size_t pretty_dump_to_file(FILE *f, bool print_cname = true) const;");
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file,
            EXPORT_SYMBOL"virtual int serialize(blaze::Encode enctyp, const blaze::nclass *prev_image, blaze::grow_byte_buffer *obb) const;");
    RETURN_IF_NOT_OK(put_newline(file))
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_GenMeths__H_
***********************************/
blaze::RetCode BLZ_COMP_Render_GenMeths__H_(compile_unit &cunit,
                                            entity_desc_comp &edsc,
                                            FILE *file)
{
    fprintf(file,  OPN_CMMNT_LN
            "getter(s) / setter(s) / is_zero(s)\n"
            CLS_CMMNT_LN);
    blaze::hash_map &entitymap = cunit.get_entity_map();
    blaze::hash_map &mmbr_map = edsc.get_map_id_MMBRDSC();
    mmbr_map.start_iteration();
    member_desc_comp *mdsc = NULL;
    while(!mmbr_map.next(NULL, &mdsc)) {
        if(mdsc->get_member_type() != MemberType_FIELD) {
            continue;
        }
        /******************************************
        Getter
        ******************************************/
        blaze::ascii_string type_str;
        RETURN_IF_NOT_OK(target_type_from_BLZ_TYPE(*mdsc, entitymap, type_str))
        blaze::ascii_string meth_name;
        RETURN_IF_NOT_OK(meth_name.assign(BLZ_COMP_CPP_GETTER_PFX"_"))
        blaze::ascii_string fld_name;
        RETURN_IF_NOT_OK(fld_name.assign(mdsc->get_member_name()))
        //fld_name.first_char_uppercase();
        RETURN_IF_NOT_OK(meth_name.append(fld_name))
        if(mdsc->get_field_type() == Type_ENTITY) {
            entity_desc_comp *inner_edsc = NULL;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &inner_edsc)) {
                if(inner_edsc->get_entity_type() == EntityType_ENUM) {
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
        fprintf(file, "%s();\n", meth_name.internal_buff());
        /******************************************
        Getter method idx.
        ******************************************/
        if(mdsc->get_nmemb() > 1) {
            RETURN_IF_NOT_OK(meth_name.append("_idx"))
            if(mdsc->get_field_type() == Type_ENTITY) {
                entity_desc_comp *edsc = NULL;
                if(!entitymap.get(mdsc->get_field_usr_str_type(), &edsc)) {
                    if(edsc->get_entity_type() == EntityType_ENUM) {
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
            fprintf(file, "%s(size_t idx);\n", meth_name.internal_buff());
        }
        /******************************************
        Setter
        ******************************************/
        RETURN_IF_NOT_OK(meth_name.assign(BLZ_COMP_CPP_SETTER_PFX"_"))
        RETURN_IF_NOT_OK(meth_name.append(fld_name))
        if(mdsc->get_field_type() == Type_ENTITY) {
            entity_desc_comp *edsc = NULL;
            if(!entitymap.get(mdsc->get_field_usr_str_type(), &edsc)) {
                if(edsc->get_entity_type() == EntityType_ENUM) {
                    if(mdsc->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"void %s(%s val);\n",  meth_name.internal_buff(),
                                type_str.internal_buff());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"void %s(const %s *val);\n",
                                meth_name.internal_buff(), type_str.internal_buff());
                    }
                } else {
                    if(mdsc->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"void %s(const %s *val);\n",
                                meth_name.internal_buff(), type_str.internal_buff());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"void %s(const %s *val);\n",
                                meth_name.internal_buff(), type_str.internal_buff());
                    }
                }
            }
        } else if(mdsc->get_nmemb() == 1) {
            //primitive type nmemb == 1
            fprintf(file, EXPORT_SYMBOL"void %s(%s val);\n", meth_name.internal_buff(),
                    type_str.internal_buff());
        } else {
            //primitive type nmemb > 1
            fprintf(file, EXPORT_SYMBOL"void %s(const %s *val);\n",
                    meth_name.internal_buff(), type_str.internal_buff());
        }
        /******************************************
        Setter method idx.
        ******************************************/
        if(mdsc->get_nmemb() > 1) {
            RETURN_IF_NOT_OK(meth_name.append("_idx"))
            if(mdsc->get_field_type() == Type_ENTITY) {
                entity_desc_comp *edsc = NULL;
                if(!entitymap.get(mdsc->get_field_usr_str_type(), &edsc)) {
                    if(edsc->get_entity_type() == EntityType_ENUM) {
                        //enum
                        fprintf(file, EXPORT_SYMBOL"void %s(size_t idx, %s val);\n",
                                meth_name.internal_buff(), type_str.internal_buff());
                    } else {
                        //class, struct
                        fprintf(file, EXPORT_SYMBOL"void %s(size_t idx, const %s *val);\n",
                                meth_name.internal_buff(),
                                type_str.internal_buff());
                    }
                }
            } else {
                //primitive type
                fprintf(file, EXPORT_SYMBOL"void %s(size_t idx, %s val);\n",
                        meth_name.internal_buff(), type_str.internal_buff());
            }
        }
        /******************************************
        Zero Method
        ******************************************/
        fprintf(file, EXPORT_SYMBOL"bool is_zero_%s() const;\n",
                mdsc->get_member_name());
        /******************************************
        Zero Method idx.
        ******************************************/
        if(mdsc->get_nmemb() > 1) {
            fprintf(file, EXPORT_SYMBOL"bool is_zero_%s_idx(size_t idx) const;\n",
                    mdsc->get_member_name());
        }
    }
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_Enum__H_
***********************************/
blaze::RetCode BLZ_COMP_Render_Enum__H_(compile_unit &cunit, FILE *file)
{
    blaze::hash_map &entitymap = cunit.get_entity_map();
    entitymap.start_iteration();
    entity_desc_comp *edsc = NULL;
    while(!entitymap.next(NULL, &edsc)) {
        if(edsc->get_entity_type() == EntityType_ENUM) {
            fprintf(file,   OPN_CMMNT_LN
                    "blaze_enum: %s\n"
                    CLS_CMMNT_LN, edsc->get_entity_name());
            fprintf(file, "enum %s{", edsc->get_entity_name());
            RETURN_IF_NOT_OK(put_newline(file))
            blaze::hash_map &mmbr_map = edsc->get_map_id_MMBRDSC();
            mmbr_map.start_iteration();
            member_desc_comp *mdsc = NULL;
            long expected_val = 0;
            while(!mmbr_map.next(NULL, &mdsc)) {
                if(expected_val == mdsc->get_enum_value()) {
                    fprintf(file, CR_1IND"%s,", mdsc->get_member_name());
                } else {
                    fprintf(file, CR_1IND"%s=%ld,",
                            mdsc->get_member_name(),
                            mdsc->get_enum_value());
                    expected_val = mdsc->get_enum_value();
                }
                expected_val++;
                RETURN_IF_NOT_OK(put_newline(file))
            }
            fprintf(file, "};");
            RETURN_IF_NOT_OK(put_newline(file))
            RETURN_IF_NOT_OK(put_newline(file))
        }
    }
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render_Class__H_
***********************************/
blaze::RetCode BLZ_COMP_Render_Class__H_(compile_unit &cunit, FILE *file)
{
    blaze::hash_map &entitymap = cunit.get_entity_map();
    entitymap.start_iteration();
    entity_desc_comp *edsc = NULL;
    while(!entitymap.next(NULL, &edsc)) {
        if(edsc->get_entity_type() == EntityType_NCLASS) {
            fprintf(file,   OPN_CMMNT_LN
                    "nclass: %s - ID: %u\n"
                    CLS_CMMNT_LN, edsc->get_entity_name(),
                    edsc->get_entityid());
            blaze::ascii_string nmsp;
            RETURN_IF_NOT_OK(BLZ_COMP_CPP_Calc_NMspc(*edsc, nmsp))
            fprintf(file, "namespace %s{\n", nmsp.internal_buff());
            RETURN_IF_NOT_OK(put_newline(file))
            fprintf(file, "#define %s_ENTITY_ID %u\n", edsc->get_entity_name(),
                    edsc->get_entityid());
            RETURN_IF_NOT_OK(put_newline(file))
            fprintf(file,
                    "class %s : public blaze::nclass{\nfriend class blaze::entity_manager;\n",
                    edsc->get_entity_name());
            fprintf(file,
                    "friend void* %s_alloc_func(size_t type_size, const void *copy);\n",
                    edsc->get_entity_name());
            fprintf(file, "public:\n");
            fprintf(file, EXPORT_SYMBOL"%s();\n", edsc->get_entity_name());
            fprintf(file, EXPORT_SYMBOL"virtual ~%s();\n", edsc->get_entity_name());
            RETURN_IF_NOT_OK(put_newline(file))
            fprintf(file, "public:\n");
            RETURN_IF_NOT_OK(BLZ_COMP_Render_ClassVirtualMeths__H_(entitymap, *edsc, file))
            RETURN_IF_NOT_OK(put_newline(file))
            RETURN_IF_NOT_OK(BLZ_COMP_Render_GenMeths__H_(cunit, *edsc, file))
            RETURN_IF_NOT_OK(put_newline(file))
            RETURN_IF_NOT_OK(BLZ_COMP_Render_Rep__H_(entitymap, *edsc, file))
            //zero obj
            fprintf(file, "\npublic:\n");
            fprintf(file, "static const %s ZERO_OBJ;\n", edsc->get_entity_name());
            fprintf(file, "};\n");
            fprintf(file, "}");
            RETURN_IF_NOT_OK(put_newline(file))
            RETURN_IF_NOT_OK(put_newline(file))
        }
    }
    return blaze::RetCode_OK;
}

/***********************************
RENDER- BLZ_COMP_Render__H_
***********************************/
blaze::RetCode BLZ_COMP_Render__H_(compile_unit &cunit)
{
    blaze::ascii_string fname;
    blaze::ascii_string_tok tknz;
    RETURN_IF_NOT_OK(tknz.init(cunit.get_file_name()))
    RETURN_IF_NOT_OK(tknz.next_token(fname, BLZ_COMP_DOT))
    RETURN_IF_NOT_OK(fname.append(BLZ_COMP_DOT))
    RETURN_IF_NOT_OK(fname.append("h"))
    FILE *file = NULL;
    COMMAND_IF_NOT_OK(open_output_file(fname.internal_buff(), &file), exit(1))
    //ifdef open
    blaze::ascii_string name;
    tknz.reset();
    RETURN_IF_NOT_OK(tknz.next_token(name, BLZ_COMP_DOT))
    name.uppercase();
    //header
    RETURN_IF_NOT_OK(render_hdr(cunit, fname, file))
    RETURN_IF_NOT_OK(put_newline(file))
    fprintf(file, "#ifndef BLZ_GEN_H_%s\n"
            "#define BLZ_GEN_H_%s\n", name.internal_buff(),
            name.internal_buff());

    fprintf(file, CPP_ONLY_BEGIN);
    //include
    fprintf(file, "#include \"blaze_model.h\"\n");
    //dll sign.
    fprintf(file, EXPORT_SYMBOL_DECL"\n");

    //render defines
    RETURN_IF_NOT_OK(BLZ_COMP_Render_Define(cunit, file))
    RETURN_IF_NOT_OK(put_newline(file))
    //render enums
    RETURN_IF_NOT_OK(BLZ_COMP_Render_Enum__H_(cunit, file))
    //render classes
    RETURN_IF_NOT_OK(BLZ_COMP_Render_Class__H_(cunit, file))
    //render model version
    RETURN_IF_NOT_OK(BLZ_COMP_Render_ModelVersion_decl__H_(cunit, file))
    //render entry point
    RETURN_IF_NOT_OK(BLZ_COMP_Render_EntryPoint_decl__H_(cunit, file))
    //render C entry point
    RETURN_IF_NOT_OK(BLZ_COMP_Render_EntryPoint_C_decl__H_(cunit, file))
    //ifdef close
    fprintf(file, "#endif\n");
    RETURN_IF_NOT_OK(put_newline(file))
    fclose(file);
    return blaze::RetCode_OK;
}

/***********************************
ENTRYPOINT- BLZ_COMP_Compile_CPP
***********************************/
blaze::RetCode compile_CPP(compile_unit &cunit)
{
    RETURN_IF_NOT_OK(BLZ_COMP_Render__H_(cunit))
    RETURN_IF_NOT_OK(BLZ_COMP_Render__CPP_(cunit))
    return blaze::RetCode_OK;
}

}
