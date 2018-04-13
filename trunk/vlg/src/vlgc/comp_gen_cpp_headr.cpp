/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "compiler.h"

namespace vlg {

/***********************************
GEN- VLG_COMP_Gen__CPP_
***********************************/
extern RetCode VLG_COMP_Gen__CPP_(compile_unit &cunit);

RetCode VLG_COMP_CPP_Calc_NMspc(entity_desc_comp &edesc,
                                std::string &out)
{
    vlg::str_tok nmtknz(edesc.get_entity_namespace());
    std::string nmtkn, ent_nmspace;
    ent_nmspace.assign("");
    bool first = true;
    while(nmtknz.next_token(nmtkn, VLG_COMP_DOT)) {
        if(!first) {
            ent_nmspace.append("_");
        } else {
            first = false;
        }
        ent_nmspace.append(nmtkn);
    }
    out.assign(ent_nmspace);
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_Define
***********************************/
RetCode VLG_COMP_Gen_Define(compile_unit &cunit, FILE *file)
{
    std::map<std::string, std::string> &definemap = cunit.get_define_map();
    std::for_each(definemap.begin(), definemap.end(), [&](auto it) {
        fprintf(file, "#define %s %s\n", it.first.c_str(), it.second.c_str());
    });
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_ModelVersion_decl__H_
***********************************/
RetCode VLG_COMP_Gen_ModelVersion_decl__H_(compile_unit &cunit,
                                           FILE *file)
{
    std::string name;
    vlg::str_tok tknz(cunit.get_file_name());
    tknz.next_token(name, VLG_COMP_DOT);
    fprintf(file, OPN_CMMNT_LN "model version\n" CLS_CMMNT_LN);
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, EXPORT_SYMBOL"const char* get_mdl_ver_%s();\n", cunit.model_name());
    fprintf(file, "}\n\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntryPoint_decl__H_
***********************************/
RetCode VLG_COMP_Gen_EntryPoint_decl__H_(compile_unit &cunit,
                                         FILE *file)
{
    std::string name;
    vlg::str_tok tknz(cunit.get_file_name());
    tknz.next_token(name, VLG_COMP_DOT);
    fprintf(file, OPN_CMMNT_LN "NEM entry point\n" CLS_CMMNT_LN);
    fprintf(file, "extern \"C\"{\n");
    fprintf(file, EXPORT_SYMBOL"vlg::nentity_manager* get_nem_%s();\n", cunit.model_name());
    fprintf(file, "}\n\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_EntryPoint_C_decl__H_
***********************************/
RetCode VLG_COMP_Gen_EntryPoint_C_decl__H_(compile_unit &cunit,
                                           FILE *file)
{
    std::string name;
    vlg::str_tok tknz(cunit.get_file_name());
    tknz.next_token(name, VLG_COMP_DOT);
    fprintf(file, "#else\n");
    fprintf(file, OPN_CMMNT_LN "NEM entry point C\n" CLS_CMMNT_LN);
    fprintf(file, "nentity_manager* get_c_nem_%s();\n", cunit.model_name());
    fprintf(file, "#endif\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_Rep__H_
***********************************/
RetCode VLG_COMP_Gen_Rep__H_(std::map<std::string, entity_desc_comp *> &entitymap,
                             entity_desc_comp &edesc,
                             FILE *file)
{
    fprintf(file, OPN_CMMNT_LN "rep.\n" CLS_CMMNT_LN);
    auto &mmbrmap = edesc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        std::string type_str;
        RET_ON_KO(target_type_from_VLG_TYPE(*mdesc->second, entitymap, type_str))
        fprintf(file,  "%s %s", type_str.c_str(), mdesc->second->get_member_name());
        if(mdesc->second->get_nmemb() > 1) {
            fprintf(file, "[%zu]", mdesc->second->get_nmemb());
        }
        fprintf(file, ";");
        RET_ON_KO(put_newline(file))
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_ClassVirtualMeths__H_
***********************************/
RetCode VLG_COMP_Gen_ClassVirtualMeths__H_(std::map<std::string, entity_desc_comp *> &entitymap,
                                           entity_desc_comp &nclass_desc,
                                           FILE *file)
{
    fprintf(file, OPN_CMMNT_LN "virtual methods\n" CLS_CMMNT_LN);
    fprintf(file, EXPORT_SYMBOL"virtual unsigned int get_id() const;");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual unsigned int get_compiler_version() const;");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual size_t get_size() const;");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual const %s& get_zero_object() const;", nclass_desc.get_nentity_name());
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual void copy_to(vlg::nclass &obj) const;");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual std::unique_ptr<vlg::nclass> clone() const;");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual bool is_zero() const;");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual void set_zero();");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual void set_from(const vlg::nclass &obj);");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual const vlg::nentity_desc& get_nentity_descriptor() const;");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual size_t pretty_dump_to_buffer(char *buff, bool print_cname = true) const;");
    RET_ON_KO(put_newline(file))
    fprintf(file, EXPORT_SYMBOL"virtual size_t pretty_dump_to_file(FILE *f, bool print_cname = true) const;");
    RET_ON_KO(put_newline(file))
    fprintf(file,
            EXPORT_SYMBOL"virtual int serialize(vlg::Encode enctyp, const vlg::nclass *prev_image, vlg::g_bbuf *obb) const;");
    RET_ON_KO(put_newline(file))
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_GenMeths__H_
***********************************/
RetCode VLG_COMP_Gen_GenMeths__H_(compile_unit &cunit,
                                  entity_desc_comp &edsc,
                                  FILE *file)
{
    fprintf(file, OPN_CMMNT_LN"getter(s) / setter(s) / is_zero(s)\n" CLS_CMMNT_LN);
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
        fprintf(file, "%s();\n", meth_name.c_str());
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
            fprintf(file, "%s(size_t idx);\n", meth_name.c_str());
        }
        /******************************************
        Setter
        ******************************************/
        meth_name.assign(VLG_COMP_CPP_SETTER_PFX"_");
        meth_name.append(fld_name);
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto edsc = entitymap.end();
            if((edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"void %s(%s val);\n",
                                meth_name.c_str(),
                                type_str.c_str());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"void %s(const %s *val);\n",
                                meth_name.c_str(),
                                type_str.c_str());
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, EXPORT_SYMBOL"void %s(const %s *val);\n",
                                meth_name.c_str(),
                                type_str.c_str());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, EXPORT_SYMBOL"void %s(const %s *val);\n",
                                meth_name.c_str(),
                                type_str.c_str());
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            fprintf(file, EXPORT_SYMBOL"void %s(%s val);\n",
                    meth_name.c_str(),
                    type_str.c_str());
        } else {
            //primitive type nmemb > 1
            fprintf(file, EXPORT_SYMBOL"void %s(const %s *val);\n",
                    meth_name.c_str(),
                    type_str.c_str());
        }
        /******************************************
        Setter method idx.
        ******************************************/
        if(mdesc->second->get_nmemb() > 1) {
            meth_name.append("_idx");
            if(mdesc->second->get_field_type() == Type_ENTITY) {
                auto edsc = entitymap.end();
                if((edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                    if(edsc->second->get_nentity_type() == NEntityType_NENUM) {
                        //enum
                        fprintf(file, EXPORT_SYMBOL"void %s(size_t idx, %s val);\n",
                                meth_name.c_str(), type_str.c_str());
                    } else {
                        //class, struct
                        fprintf(file, EXPORT_SYMBOL"void %s(size_t idx, const %s *val);\n",
                                meth_name.c_str(),
                                type_str.c_str());
                    }
                }
            } else {
                //primitive type
                fprintf(file, EXPORT_SYMBOL"void %s(size_t idx, %s val);\n", meth_name.c_str(), type_str.c_str());
            }
        }
        /******************************************
        Zero Method
        ******************************************/
        fprintf(file, EXPORT_SYMBOL"bool is_zero_%s() const;\n", mdesc->second->get_member_name());
        /******************************************
        Zero Method idx.
        ******************************************/
        if(mdesc->second->get_nmemb() > 1) {
            fprintf(file, EXPORT_SYMBOL"bool is_zero_%s_idx(size_t idx) const;\n", mdesc->second->get_member_name());
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_Enum__H_
***********************************/
RetCode VLG_COMP_Gen_Enum__H_(compile_unit &cunit, FILE *file)
{
    auto &entitymap = cunit.get_entity_map();
    for(auto edsc = entitymap.begin(); edsc != entitymap.end(); edsc++) {
        if(edsc->second->get_nentity_type() == NEntityType_NENUM) {
            fprintf(file, OPN_CMMNT_LN "vlg_enum: %s\n" CLS_CMMNT_LN, edsc->second->get_nentity_name());
            fprintf(file, "enum %s{", edsc->second->get_nentity_name());
            RET_ON_KO(put_newline(file))
            auto &mmbrmap = edsc->second->get_map_id_MMBRDSC();
            long expected_val = 0;
            for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
                if(expected_val == mdesc->second->get_enum_value()) {
                    fprintf(file, CR_1IND"%s,", mdesc->second->get_member_name());
                } else {
                    fprintf(file, CR_1IND"%s=%ld,",
                            mdesc->second->get_member_name(),
                            mdesc->second->get_enum_value());
                    expected_val = mdesc->second->get_enum_value();
                }
                expected_val++;
                RET_ON_KO(put_newline(file))
            }
            fprintf(file, "};");
            RET_ON_KO(put_newline(file))
            RET_ON_KO(put_newline(file))
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen_Class__H_
***********************************/
RetCode VLG_COMP_Gen_Class__H_(compile_unit &cunit, FILE *file)
{
    std::map<std::string, entity_desc_comp *> &entitymap = cunit.get_entity_map();
    for(auto edsc = entitymap.begin(); edsc != entitymap.end(); edsc++) {
        if(edsc->second->get_nentity_type() == NEntityType_NCLASS) {
            fprintf(file, OPN_CMMNT_LN"nclass: %s - ID: %u\n"
                    CLS_CMMNT_LN, edsc->second->get_nentity_name(),
                    edsc->second->get_entityid());
            std::string nmsp;
            RET_ON_KO(VLG_COMP_CPP_Calc_NMspc(*edsc->second, nmsp))
            fprintf(file, "namespace %s{\n", nmsp.c_str());
            RET_ON_KO(put_newline(file))
            fprintf(file, "#define %s_ENTITY_ID %u\n", edsc->second->get_nentity_name(), edsc->second->get_entityid());
            RET_ON_KO(put_newline(file))
            fprintf(file, "struct %s : public vlg::nclass{\n", edsc->second->get_nentity_name());
            fprintf(file, EXPORT_SYMBOL"%s();\n", edsc->second->get_nentity_name());
            fprintf(file, EXPORT_SYMBOL"virtual ~%s();\n", edsc->second->get_nentity_name());
            RET_ON_KO(put_newline(file))
            RET_ON_KO(VLG_COMP_Gen_ClassVirtualMeths__H_(entitymap, *edsc->second, file))
            RET_ON_KO(put_newline(file))
            RET_ON_KO(VLG_COMP_Gen_GenMeths__H_(cunit, *edsc->second, file))
            RET_ON_KO(put_newline(file))
            RET_ON_KO(VLG_COMP_Gen_Rep__H_(entitymap, *edsc->second, file))
            //zero obj
            fprintf(file, "static const %s ZERO_OBJ;\n", edsc->second->get_nentity_name());
            fprintf(file, "};\n");
            fprintf(file, "}");
            RET_ON_KO(put_newline(file))
            RET_ON_KO(put_newline(file))
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_Gen__H_
***********************************/
RetCode VLG_COMP_Gen__H_(compile_unit &cunit)
{
    std::string fname;
    vlg::str_tok tknz(cunit.get_file_name());
    tknz.next_token(fname, VLG_COMP_DOT);
    fname.append(VLG_COMP_DOT);
    fname.append("h");
    FILE *file = nullptr;
    CMD_ON_KO(open_output_file(fname.c_str(), &file), exit(1))
    //ifdef open
    std::string name;
    tknz.reset();
    tknz.next_token(name, VLG_COMP_DOT);
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    //header
    RET_ON_KO(render_hdr(cunit, fname, file))
    RET_ON_KO(put_newline(file))
    fprintf(file, "#ifndef VLG_GEN_H_%s\n" "#define VLG_GEN_H_%s\n", name.c_str(), name.c_str());
    fprintf(file, CPP_ONLY_BEGIN);
    //include
    fprintf(file, "#include \"vlg_model.h\"\n");
    //dll sign.
    fprintf(file, EXPORT_SYMBOL_DECL"\n");
    //render defines
    RET_ON_KO(VLG_COMP_Gen_Define(cunit, file))
    RET_ON_KO(put_newline(file))
    //render enums
    RET_ON_KO(VLG_COMP_Gen_Enum__H_(cunit, file))
    //render classes
    RET_ON_KO(VLG_COMP_Gen_Class__H_(cunit, file))
    //render model version
    RET_ON_KO(VLG_COMP_Gen_ModelVersion_decl__H_(cunit, file))
    //render entry point
    RET_ON_KO(VLG_COMP_Gen_EntryPoint_decl__H_(cunit, file))
    //render C entry point
    RET_ON_KO(VLG_COMP_Gen_EntryPoint_C_decl__H_(cunit, file))
    //ifdef close
    fprintf(file, "#endif\n");
    RET_ON_KO(put_newline(file))
    fclose(file);
    return vlg::RetCode_OK;
}

/***********************************
ENTRYPOINT- VLG_COMP_Compile_CPP
***********************************/
RetCode compile_CPP(compile_unit &cunit)
{
    RET_ON_KO(VLG_COMP_Gen__H_(cunit))
    RET_ON_KO(VLG_COMP_Gen__CPP_(cunit))
    return vlg::RetCode_OK;
}

}
