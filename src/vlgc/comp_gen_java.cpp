/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "compiler.h"

#define VLG_COMP_JAVA_GETTER_PFX         "get"
#define VLG_COMP_JAVA_SETTER_PFX         "set"

namespace vlg {

extern std::string unit_nmspace;

/***********************************
- isJavaStringType
***********************************/
bool isJavaStringType(member_desc_comp *mdsc)
{
    return
        mdsc->get_nmemb() > 1 &&
        (mdsc->get_field_type() == Type_ASCII);
}

/***********************************
- rec_brackets_cond
***********************************/
const char *rec_brackets_cond(member_desc_comp *mdsc)
{
    if(!(mdsc->get_field_type() == Type_ASCII)) {
        return "[]";
    }
    return "";
}

/***********************************
GEN- VLG_COMP_JAVA_VLG_Entity_Ctor
***********************************/
RetCode VLG_COMP_JAVA_VLG_Entity_Ctor(compile_unit &cunit,
                                      entity_desc_comp &edsc,
                                      FILE *file)
{
    fprintf(file,  "/*****************************************************\n"
            "ctor\n"
            "*****************************************************/\n");
    fprintf(file, "public %s(){\n", edsc.get_nentity_name());
    auto &entitymap = cunit.get_nentity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, IND_1"%s = new %s();\n", mdesc->second->get_member_name(),
                                inner_edsc->second->get_nentity_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, IND_1"%s = new %s[%zu];\n", mdesc->second->get_member_name(),
                                inner_edsc->second->get_nentity_name(),
                                mdesc->second->get_nmemb());
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_2"%s[i] = new %s();\n", mdesc->second->get_member_name(),
                                inner_edsc->second->get_nentity_name());
                        fprintf(file, IND_1"}\n");
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, IND_1"%s = new %s();\n", mdesc->second->get_member_name(),
                                inner_edsc->second->get_nentity_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, IND_1"%s = new %s[%zu];\n", mdesc->second->get_member_name(),
                                inner_edsc->second->get_nentity_name(),
                                mdesc->second->get_nmemb());
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_2"%s[i] = new %s();\n", mdesc->second->get_member_name(),
                                inner_edsc->second->get_nentity_name());
                        fprintf(file, IND_1"}\n");
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            std::string ttype, zero_val;
            RET_ON_KO(target_type_from_builtin_VLG_TYPE(*mdesc->second, ttype))
            RET_ON_KO(get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), zero_val))
            fprintf(file, IND_1"%s = new %s(%s);\n", mdesc->second->get_member_name(),
                    ttype.c_str(),
                    zero_val.c_str());
        } else {
            //primitive type nmemb > 1
            if(isJavaStringType(mdesc->second)) {
                //strings
                fprintf(file, IND_1"%s = new String();\n", mdesc->second->get_member_name());
            } else {
                //primitive type nmemb > 1
                std::string ttype, zero_val;
                RET_ON_KO(target_type_from_builtin_VLG_TYPE(*mdesc->second, ttype))
                RET_ON_KO(get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), zero_val))
                fprintf(file, IND_1"%s = new %s[%zu];\n", mdesc->second->get_member_name(),
                        ttype.c_str(),
                        mdesc->second->get_nmemb());
                fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                        mdesc->second->get_member_name());
                fprintf(file, IND_2"%s[i] = new %s(%s);\n", mdesc->second->get_member_name(),
                        ttype.c_str(),
                        zero_val.c_str());
                fprintf(file, IND_1"}\n");
            }
        }
    }
    fprintf(file, "}");
    return vlg::RetCode_OK;
}

/*************************************************************
java NEntity abstract meths
**************************************************************/
#define ABSR_METH_ENT_COPY "public void copy(NEntity out){"
#define ABSR_METH_ENT_CLONE "public NEntity clone(){"
#define ABSR_METH_ENT_ISZERO "public boolean isZero(){"
#define ABSR_METH_ENT_SETZERO "public void setZero(){"
#define ABSR_METH_ENT_SET "public void set(NEntity in){"
#define ABSR_METH_ENT_GCOMPVER "public int getCompile_Ver(){"
#define ABSR_METH_ENT_GENTDSC "public NEntityDesc getEntityDesc(){"
#define ABSR_METH_ENT_PTOBUF "public int printToBuff(StringBuffer buff, boolean print_cname, int mode){"
#define ABSR_METH_ENT_SER "public int serialize(int enctyp, NEntity prev_image, VLGByteBuffer out_buf){"


/*************************************************************
java NClass abstract meths
**************************************************************/
#define ABSR_METH_GETENTID "public int getClassId(){"


/***********************************
GEN- ABSR_METH_ENT_COPY__Body_impl
***********************************/
RetCode ABSR_METH_ENT_COPY__Body_impl(compile_unit &cunit,
                                      entity_desc_comp &edsc,
                                      FILE *file)
{
    fprintf(file, IND_1"if(!(out instanceof %s)){\n"
            IND_2"throw new IllegalArgumentException(\"not instanceof %s\");\n"
            IND_1"}\n", edsc.get_nentity_name(), edsc.get_nentity_name());
    fprintf(file, IND_1"%s cstout = (%s)out;\n", edsc.get_nentity_name(),
            edsc.get_nentity_name());
    auto &entitymap = cunit.get_nentity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, IND_1"cstout.%s.setValue(%s.getValue());\n",
                                mdesc->second->get_member_name(), mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_2"cstout.%s[i].setValue(%s[i].getValue());\n",
                                mdesc->second->get_member_name(), mdesc->second->get_member_name());
                        fprintf(file, IND_1"}\n");
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, IND_2"%s.copy(cstout.%s);\n", mdesc->second->get_member_name(),
                                mdesc->second->get_member_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_2"%s[i].copy(cstout.%s[i]);\n", mdesc->second->get_member_name(),
                                mdesc->second->get_member_name());
                        fprintf(file, IND_1"}\n");
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            fprintf(file, IND_1"cstout.%s = %s;\n", mdesc->second->get_member_name(),
                    mdesc->second->get_member_name());
        } else {
            //primitive type nmemb > 1
            if(isJavaStringType(mdesc->second)) {
                //strings
                fprintf(file, IND_1"cstout.%s = %s;\n", mdesc->second->get_member_name(),
                        mdesc->second->get_member_name());
            } else {
                //primitive type nmemb > 1
                fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                        mdesc->second->get_member_name());
                fprintf(file, IND_2"cstout.%s[i] = %s[i];\n", mdesc->second->get_member_name(),
                        mdesc->second->get_member_name());
                fprintf(file, IND_1"}\n");
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_CLONE__Body_impl
***********************************/
RetCode ABSR_METH_ENT_CLONE__Body_impl(compile_unit &cunit,
                                       entity_desc_comp &edsc,
                                       FILE *file)
{
    fprintf(file, IND_1"NClass ne = new %s();\n", edsc.get_nentity_name());
    fprintf(file, IND_1"ne.set(this);\n");
    fprintf(file, IND_1"return ne;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_ISZERO__Body_impl
***********************************/
RetCode ABSR_METH_ENT_ISZERO__Body_impl(compile_unit &cunit,
                                        entity_desc_comp &edsc,
                                        FILE *file)
{
    std::string zero_val;
    auto &entitymap = cunit.get_nentity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, IND_1"if(%s.getValue() != 0) return false;\n",
                                mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_2"if(%s[i].getValue() != 0) return false;\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_1"}\n");
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, IND_2"if(!%s.isZero()) return false;\n",
                                mdesc->second->get_member_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_2"if(!%s[i].isZero()) return false;\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_1"}\n");
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            RET_ON_KO(get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), zero_val))
            fprintf(file, IND_1"if(%s != %s) return false;\n", mdesc->second->get_member_name(),
                    zero_val.c_str());
        } else {
            //primitive type nmemb > 1
            if(isJavaStringType(mdesc->second)) {
                //strings
                fprintf(file, IND_1"if(!\"\".equals(%s)) return false;\n",
                        mdesc->second->get_member_name());
            } else {
                //primitive type nmemb > 1
                RET_ON_KO(get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), zero_val))
                fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                        mdesc->second->get_member_name());
                fprintf(file, IND_2"if(%s[i] != %s) return false;\n", mdesc->second->get_member_name(),
                        zero_val.c_str());
                fprintf(file, IND_1"}\n");
            }
        }
    }
    fprintf(file, IND_1"return true;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_SETZERO__Body_impl
***********************************/
RetCode ABSR_METH_ENT_SETZERO__Body_impl(compile_unit &cunit,
                                         entity_desc_comp &edsc,
                                         FILE *file)
{
    std::string zero_val;
    auto &entitymap = cunit.get_nentity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, IND_1"%s.setValue(0);\n", mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                        fprintf(file, IND_2"%s[i].setValue(0);\n", mdesc->second->get_member_name());
                        fprintf(file, IND_1"}\n");
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, IND_2"%s.setZero();\n", mdesc->second->get_member_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                        fprintf(file, IND_2"%s[i].setZero();\n", mdesc->second->get_member_name());
                        fprintf(file, IND_1"}\n");
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            RET_ON_KO(get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), zero_val))
            fprintf(file, IND_1"%s = %s;\n", mdesc->second->get_member_name(),
                    zero_val.c_str());
        } else {
            //primitive type nmemb > 1
            if(isJavaStringType(mdesc->second)) {
                //strings
                fprintf(file, IND_1"%s = \"\";\n", mdesc->second->get_member_name());
            } else {
                //primitive type nmemb > 1
                RET_ON_KO(get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), zero_val))
                fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                        mdesc->second->get_member_name());
                fprintf(file, IND_2"%s[i] = %s;\n", mdesc->second->get_member_name(),
                        zero_val.c_str());
                fprintf(file, IND_1"}\n");
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_SET__Body_impl
***********************************/
RetCode ABSR_METH_ENT_SET__Body_impl(compile_unit &cunit,
                                     entity_desc_comp &edsc,
                                     FILE *file)
{
    fprintf(file, IND_1"if(!(in instanceof %s)){\n"
            IND_2"throw new IllegalArgumentException(\"not instanceof %s\");\n"
            IND_1"}\n", edsc.get_nentity_name(), edsc.get_nentity_name());
    fprintf(file, IND_1"%s cstin = (%s)in;\n", edsc.get_nentity_name(),
            edsc.get_nentity_name());
    auto &entitymap = cunit.get_nentity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, IND_1"%s.setValue(cstin.%s.getValue());\n",
                                mdesc->second->get_member_name(), mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_2"%s[i].setValue(cstin.%s[i].getValue());\n",
                                mdesc->second->get_member_name(), mdesc->second->get_member_name());
                        fprintf(file, IND_1"}\n");
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, IND_2"%s.set(cstin.%s);\n", mdesc->second->get_member_name(),
                                mdesc->second->get_member_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_2"%s[i].set(cstin.%s[i]);\n", mdesc->second->get_member_name(),
                                mdesc->second->get_member_name());
                        fprintf(file, IND_1"}\n");
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            fprintf(file, IND_1"%s = cstin.%s;\n", mdesc->second->get_member_name(), mdesc->second->get_member_name());
        } else {
            //primitive type nmemb > 1
            if(isJavaStringType(mdesc->second)) {
                //strings
                fprintf(file, IND_1"%s = cstin.%s;\n", mdesc->second->get_member_name(), mdesc->second->get_member_name());
            } else {
                //primitive type nmemb > 1
                fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                fprintf(file, IND_2"%s[i] = cstin.%s[i];\n", mdesc->second->get_member_name(), mdesc->second->get_member_name());
                fprintf(file, IND_1"}\n");
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_GCOMPVER__Body_impl
***********************************/
RetCode ABSR_METH_ENT_GCOMPVER__Body_impl(compile_unit &cunit,
                                          entity_desc_comp &edsc,
                                          FILE *file)
{
    fprintf(file, IND_1"return 0;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_GENTDSC__Body_impl
***********************************/
RetCode ABSR_METH_ENT_GENTDSC__Body_impl(compile_unit &cunit,
                                         entity_desc_comp &edsc,
                                         FILE *file)
{
    std::string ep_nm;
    ep_nm.append(cunit.model_name());
    fprintf(file, IND_1"return %s.getNEntityManager().getNEntityDesc(\"%s\");\n",
            ep_nm.c_str(),
            edsc.get_nentity_name());
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_PTOBUF__NotZeroMode
***********************************/
RetCode ABSR_METH_ENT_PTOBUF__NotZeroMode(compile_unit &cunit,
                                          entity_desc_comp &edsc,
                                          FILE *file)
{
    std::string mb_nm;
    auto &entitymap = cunit.get_nentity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        mb_nm.assign(mdesc->second->get_member_name());
        std::transform(mb_nm.begin(), mb_nm.begin(), mb_nm.begin(), ::toupper);
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, IND_2
                                "if(!isZero%s()){ buff.append(\"%s=\"); buff.append(%s.getValue()); buff.append(\"|\"); }\n",
                                mb_nm.c_str(),
                                mdesc->second->get_member_name(),
                                mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, IND_2 "if(!isZero%s()){\n", mb_nm.c_str());
                        fprintf(file, IND_3"frst_test = true;\n");
                        fprintf(file, IND_3"buff.append(\"%s=\");\n", mdesc->second->get_member_name());
                        fprintf(file, IND_3"buff.append(\"[\");\n");
                        fprintf(file, IND_3"for(int i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                        fprintf(file, IND_4"if(frst_test){frst_test = false;}else{ buff.append(\",\");}\n");
                        fprintf(file, IND_4"buff.append(%s[i].getValue());\n", mdesc->second->get_member_name());
                        fprintf(file, IND_4"buff.append(\",\");\n");
                        fprintf(file, IND_3"}\n");
                        fprintf(file, IND_3"buff.append(\"]\");\n");
                        fprintf(file, IND_2"}\n");
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, IND_2
                                "if(!isZero%s()){ buff.append(\"%s=\"); %s.printToBuff(buff, false, mode); buff.append(\"|\"); }\n",
                                mb_nm.c_str(),
                                mdesc->second->get_member_name(),
                                mdesc->second->get_member_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, IND_2 "if(!isZero%s()){\n", mb_nm.c_str());
                        fprintf(file, IND_4"frst_test = true;\n");
                        fprintf(file, IND_3"buff.append(\"%s=\");\n", mdesc->second->get_member_name());
                        fprintf(file, IND_3"buff.append(\"[\");\n");
                        fprintf(file, IND_3"for(int i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                        fprintf(file, IND_4"if(frst_test){frst_test = false;}else{ buff.append(\",\");}\n");
                        fprintf(file, IND_4 "%s[i].printToBuff(buff, false, mode);\n", mdesc->second->get_member_name());
                        fprintf(file, IND_3"}\n");
                        fprintf(file, IND_3"buff.append(\"]\");\n");
                        fprintf(file, IND_2"}\n");
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            fprintf(file, IND_2
                    "if(!isZero%s()){ buff.append(\"%s=\"); buff.append(%s); buff.append(\"|\"); }\n",
                    mb_nm.c_str(),
                    mdesc->second->get_member_name(),
                    mdesc->second->get_member_name());
        } else {
            //primitive type nmemb > 1
            if(isJavaStringType(mdesc->second)) {
                //strings
                fprintf(file, IND_2
                        "if(!isZero%s()){ buff.append(\"%s=[\"); buff.append(%s); buff.append(\"]\"); }\n",
                        mb_nm.c_str(),
                        mdesc->second->get_member_name(),
                        mdesc->second->get_member_name());
            } else {
                //primitive type nmemb > 1
                fprintf(file, IND_2 "if(!isZero%s()){\n", mb_nm.c_str());
                fprintf(file, IND_2"frst_test = true;\n");
                fprintf(file, IND_3"buff.append(\"%s=\");\n", mdesc->second->get_member_name());
                fprintf(file, IND_3"buff.append(\"[\");\n");
                fprintf(file, IND_3"for(int i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                fprintf(file, IND_4"if(frst_test){frst_test = false;}else{ buff.append(\",\");}\n");
                fprintf(file, IND_4"buff.append(%s[i]);\n", mdesc->second->get_member_name());
                fprintf(file, IND_3"}\n");
                fprintf(file, IND_3"buff.append(\"]\");\n");
                fprintf(file, IND_2"}\n");
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_PTOBUF__AllFildMode
***********************************/
RetCode ABSR_METH_ENT_PTOBUF__AllFildMode(compile_unit &cunit,
                                          entity_desc_comp &edsc,
                                          FILE *file)
{
    auto &entitymap = cunit.get_nentity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, IND_2 "buff.append(\"%s=\"); buff.append(%s.getValue()); buff.append(\"|\");\n",
                                mdesc->second->get_member_name(),
                                mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, IND_2"buff.append(\"%s=\");\n", mdesc->second->get_member_name());
                        fprintf(file, IND_2"buff.append(\"[\");\n");
                        fprintf(file, IND_2"for(int i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                        fprintf(file, IND_3"buff.append(%s[i].getValue());\n", mdesc->second->get_member_name());
                        fprintf(file, IND_3"buff.append(\",\");\n");
                        fprintf(file, IND_2"}\n");
                        fprintf(file, IND_2"buff.append(\"]\");\n");
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, IND_2
                                "buff.append(\"%s=\"); %s.printToBuff(buff, false, mode); buff.append(\"|\");\n",
                                mdesc->second->get_member_name(),
                                mdesc->second->get_member_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, IND_2"buff.append(\"%s=\");\n", mdesc->second->get_member_name());
                        fprintf(file, IND_2"buff.append(\"[\");\n");
                        fprintf(file, IND_2"for(int i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                        fprintf(file, IND_3 "%s[i].printToBuff(buff, false, mode);\n", mdesc->second->get_member_name());
                        fprintf(file, IND_2"}\n");
                        fprintf(file, IND_2"buff.append(\"]\");\n");
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            fprintf(file, IND_2
                    "buff.append(\"%s=\"); buff.append(%s); buff.append(\"|\");\n",
                    mdesc->second->get_member_name(),
                    mdesc->second->get_member_name());
        } else {
            //primitive type nmemb > 1
            if(isJavaStringType(mdesc->second)) {
                //strings
                fprintf(file, IND_2
                        "buff.append(\"%s=\"); buff.append(%s); buff.append(\"|\");\n",
                        mdesc->second->get_member_name(),
                        mdesc->second->get_member_name());
            } else {
                //primitive type nmemb > 1
                fprintf(file, IND_2"buff.append(\"%s=\");\n", mdesc->second->get_member_name());
                fprintf(file, IND_2"buff.append(\"[\");\n");
                fprintf(file, IND_2"for(int i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                fprintf(file, IND_3"buff.append(%s[i]);\n", mdesc->second->get_member_name());
                fprintf(file, IND_3"buff.append(\",\");\n");
                fprintf(file, IND_2"}\n");
                fprintf(file, IND_2"buff.append(\"]\");\n");
            }
        }
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_PTOBUF__Body_impl
***********************************/
RetCode ABSR_METH_ENT_PTOBUF__Body_impl(compile_unit &cunit,
                                        entity_desc_comp &edsc,
                                        FILE *file)
{
    fprintf(file, IND_1 "boolean frst_test = true;\n");
    fprintf(file, IND_1 "if(print_cname) buff.append(\"%s\");\n", edsc.get_nentity_name());
    //class opening curl brace
    fprintf(file, IND_1 "buff.append(\"{\");\n");
    fprintf(file, IND_1 "switch(mode){\n");
    fprintf(file, IND_2 "case Framework.PrintMode_Undef:\n");
    fprintf(file, IND_2 "case Framework.PrintMode_NotZero:\n");
    //NOTZERO MODE
    RET_ON_KO(ABSR_METH_ENT_PTOBUF__NotZeroMode(cunit, edsc, file))
    fprintf(file, IND_2 "break;\n");
    fprintf(file, IND_2 "case Framework.PrintMode_All:\n");
    //ALL MODE
    RET_ON_KO(ABSR_METH_ENT_PTOBUF__AllFildMode(cunit, edsc, file))
    fprintf(file, IND_2 "break;\n");
    fprintf(file, IND_2 "default:\n");
    fprintf(file, IND_2 "break;\n");
    fprintf(file, IND_1 "}\n");
    //class closing curl brace
    fprintf(file, IND_1 "buff.append(\"}\");\n");
    fprintf(file, IND_1 "return 0;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- ABSR_METH_ENT_SER__IndexedNotZero
***********************************/
RetCode ABSR_METH_ENT_SER__IndexedNotZero(compile_unit &cunit,
                                          entity_desc_comp &edsc,
                                          FILE *file)
{
    fprintf(file, IND_3"ByteBuffer bb = ByteBuffer.allocate(8);\n");
    fprintf(file, IND_3"bb.order(ByteOrder.LITTLE_ENDIAN);\n");
    fprintf(file, IND_3"int tlen_offst = out_buf.getPos();\n");
    fprintf(file, IND_3"out_buf.mvPos(Framework.ENTLN_B_SZ);\n");
    fprintf(file,
            IND_3"tlen = out_buf.getPos();  //we are reserving 4 bytes for total entity len.\n");
    std::string mb_nm;
    auto &entitymap = cunit.get_nentity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        mb_nm.assign(mdesc->second->get_member_name());
        std::transform(mb_nm.begin(), mb_nm.begin(), mb_nm.begin(), ::toupper);
        fprintf(file, IND_3 "if(!isZero%s()){\n", mb_nm.c_str());
        fprintf(file, IND_4"bb.putShort((short)%d).flip();\n"
                IND_4"out_buf.apndByteBuffer(bb);\n"
                IND_4"bb.clear();\n", mdesc->second->get_member_id());
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, IND_4"bb.putInt(%s.getValue()).flip();\n"
                                IND_4"out_buf.apndByteBuffer(bb);\n"
                                IND_4"bb.clear();\n", mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, IND_4"int alen = 0, alen_offst = out_buf.getPos();\n");
                        fprintf(file, IND_4"out_buf.mvPos(Framework.ARRAY_B_SZ);\n");
                        fprintf(file, IND_4"alen = out_buf.getPos();  //we are reserving 4 bytes for array len.\n");
                        fprintf(file, IND_4"for(short i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                        fprintf(file, IND_5 "if(get%s()[i].getValue() != 0){\n", mb_nm.c_str());
                        fprintf(file, IND_6"bb.putShort(i).flip();\n"
                                IND_6"out_buf.apndByteBuffer(bb);\n"
                                IND_6"bb.clear();\n");
                        fprintf(file, IND_6"bb.putInt(%s[i].getValue()).flip();\n"
                                IND_6"out_buf.apndByteBuffer(bb);\n"
                                IND_6"bb.clear();\n", mdesc->second->get_member_name());
                        fprintf(file, IND_5"}\n");
                        fprintf(file, IND_4"}\n");
                        fprintf(file, IND_4"bb.putInt(out_buf.getPos() - alen).flip();\n");
                        fprintf(file, IND_4"out_buf.putByteBuffer(bb, alen_offst, Framework.ARRAY_B_SZ);\n"
                                IND_4"bb.clear();\n");
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, IND_4"%s.serialize(enctyp, prev_image, out_buf);\n", mdesc->second->get_member_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, IND_4"int alen = 0, alen_offst = out_buf.getPos();\n");
                        fprintf(file, IND_4"out_buf.mvPos(Framework.ARRAY_B_SZ);\n");
                        fprintf(file, IND_4"alen = out_buf.getPos();  //we are reserving 4 bytes for array len.\n");
                        fprintf(file, IND_4"for(short i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                        fprintf(file, IND_5 "if(!get%s()[i].isZero()){\n", mb_nm.c_str());
                        fprintf(file, IND_6"bb.putShort(i).flip();\n"
                                IND_6"out_buf.apndByteBuffer(bb);\n"
                                IND_6"bb.clear();\n");
                        fprintf(file, IND_6"%s[i].serialize(enctyp, prev_image, out_buf);\n", mdesc->second->get_member_name());
                        fprintf(file, IND_5"}\n");
                        fprintf(file, IND_4"}\n");
                        fprintf(file, IND_4"bb.putInt(out_buf.getPos() - alen).flip();\n");
                        fprintf(file, IND_4"out_buf.putByteBuffer(bb, alen_offst, Framework.ARRAY_B_SZ);\n"
                                IND_4"bb.clear();\n");
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            switch(mdesc->second->get_field_type()) {
                case Type_BOOL:
                    fprintf(file, IND_4"bb.put(%s ? Framework.TRUE_BYTE : Framework.FALSE_BYTE).flip();\n"
                            IND_4"out_buf.apndByteBuffer(bb);\n"
                            IND_4"bb.clear();\n", mdesc->second->get_member_name());
                    break;
                case Type_INT16:
                case Type_UINT16:
                    fprintf(file, IND_4"bb.putShort(%s).flip();\n"
                            IND_4"out_buf.apndByteBuffer(bb);\n"
                            IND_4"bb.clear();\n", mdesc->second->get_member_name());
                    break;
                case Type_INT32:
                case Type_UINT32:
                    fprintf(file, IND_4"bb.putInt(%s).flip();\n"
                            IND_4"out_buf.apndByteBuffer(bb);\n"
                            IND_4"bb.clear();\n", mdesc->second->get_member_name());
                    break;
                case Type_INT64:
                case Type_UINT64:
                    fprintf(file, IND_4"bb.putLong(%s).flip();\n"
                            IND_4"out_buf.apndByteBuffer(bb);\n"
                            IND_4"bb.clear();\n", mdesc->second->get_member_name());
                    break;
                case Type_FLOAT32:
                    fprintf(file, IND_4"bb.putFloat(%s).flip();\n"
                            IND_4"out_buf.apndByteBuffer(bb);\n"
                            IND_4"bb.clear();\n", mdesc->second->get_member_name());
                    break;
                case Type_FLOAT64:
                    fprintf(file, IND_4"bb.putDouble(%s).flip();\n"
                            IND_4"out_buf.apndByteBuffer(bb);\n"
                            IND_4"bb.clear();\n", mdesc->second->get_member_name());
                    break;
                case Type_ASCII:
                    fprintf(file, IND_4"Short ascii_c = (short)%s.charValue();\n"
                            IND_4"bb.put(ascii_c.byteValue()).flip();\n"
                            IND_4"out_buf.apndByteBuffer(bb);\n"
                            IND_4"bb.clear();\n", mdesc->second->get_member_name());
                    break;
                default:
                    return vlg::RetCode_KO;
            }
        } else {
            //primitive type nmemb > 1
            if(isJavaStringType(mdesc->second)) {
                //strings
                switch(mdesc->second->get_field_type()) {
                    case Type_ASCII:
                        fprintf(file, IND_4"try{\n");
                        fprintf(file, IND_5"ByteBuffer str_bb = ByteBuffer.wrap(%s.getBytes(\"US-ASCII\"));\n",
                                mdesc->second->get_member_name());
                        fprintf(file, IND_5"bb.putInt(str_bb.limit()).flip();\n");
                        fprintf(file, IND_5"out_buf.apndByteBuffer(bb);\n"
                                IND_5"bb.clear();\n");
                        fprintf(file, IND_5"out_buf.apndByteBuffer(str_bb);\n");
                        fprintf(file, IND_4"}catch(UnsupportedEncodingException e){ e.printStackTrace(); }\n");
                        break;
                    default:
                        return vlg::RetCode_KO;
                }
            } else {
                //primitive type nmemb > 1
                std::string zero_val;
                RET_ON_KO(get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), zero_val))
                fprintf(file, IND_4"int alen = 0, alen_offst = out_buf.getPos();\n");
                fprintf(file, IND_4"out_buf.mvPos(Framework.ARRAY_B_SZ);\n");
                fprintf(file, IND_4"alen = out_buf.getPos();  //we are reserving 4 bytes for array len.\n");
                fprintf(file, IND_4"for(short i=0; i<%s.length; i++){\n", mdesc->second->get_member_name());
                fprintf(file, IND_5 "if(get%s()[i] != %s){\n", mb_nm.c_str(), zero_val.c_str());
                fprintf(file, IND_6"bb.putShort(i).flip();\n"
                        IND_6"out_buf.apndByteBuffer(bb);\n"
                        IND_6"bb.clear();\n");
                switch(mdesc->second->get_field_type()) {
                    case Type_BOOL:
                        fprintf(file, IND_6"bb.put(%s[i] ? Framework.TRUE_BYTE : Framework.FALSE_BYTE).flip();\n"
                                IND_6"out_buf.apndByteBuffer(bb);\n"
                                IND_6"bb.clear();\n", mdesc->second->get_member_name());
                        break;
                    case Type_INT16:
                    case Type_UINT16:
                        fprintf(file, IND_6"bb.putShort(%s[i]).flip();\n"
                                IND_6"out_buf.apndByteBuffer(bb);\n"
                                IND_6"bb.clear();\n", mdesc->second->get_member_name());
                        break;
                    case Type_INT32:
                    case Type_UINT32:
                        fprintf(file, IND_6"bb.putInt(%s[i]).flip();\n"
                                IND_6"out_buf.apndByteBuffer(bb);\n"
                                IND_6"bb.clear();\n", mdesc->second->get_member_name());
                        break;
                    case Type_INT64:
                    case Type_UINT64:
                        fprintf(file, IND_6"bb.putLong(%s[i]).flip();\n"
                                IND_6"out_buf.apndByteBuffer(bb);\n"
                                IND_6"bb.clear();\n", mdesc->second->get_member_name());
                        break;
                    case Type_FLOAT32:
                        fprintf(file, IND_6"bb.putFloat(%s[i]).flip();\n"
                                IND_6"out_buf.apndByteBuffer(bb);\n"
                                IND_6"bb.clear();\n", mdesc->second->get_member_name());
                        break;
                    case Type_FLOAT64:
                        fprintf(file, IND_6"bb.putDouble(%s[i]).flip();\n"
                                IND_6"out_buf.apndByteBuffer(bb);\n"
                                IND_6"bb.clear();\n", mdesc->second->get_member_name());
                        break;
                    default:
                        return vlg::RetCode_KO;
                }
                fprintf(file, IND_5"}\n");
                fprintf(file, IND_4"}\n");
                fprintf(file, IND_4"bb.putInt(out_buf.getPos() - alen).flip();\n");
                fprintf(file, IND_4"out_buf.putByteBuffer(bb, alen_offst, Framework.ARRAY_B_SZ);\n"
                        IND_4"bb.clear();\n");
            }
        }
        fprintf(file, IND_3 "}\n");
    }
    fprintf(file, IND_3"bb.putInt(out_buf.getPos() - tlen).flip();\n");
    fprintf(file,
            IND_3"out_buf.putByteBuffer(bb, tlen_offst, Framework.ENTLN_B_SZ);\n");
    fprintf(file, IND_3"bb.clear();\n");
    return vlg::RetCode_OK;
}


/***********************************
GEN- ABSR_METH_ENT_SER__Body_impl -*-SERIALIZE-*-
***********************************/
RetCode ABSR_METH_ENT_SER__Body_impl(compile_unit &cunit,
                                     entity_desc_comp &edsc,
                                     FILE *file)
{
    fprintf(file, IND_1"int tlen = 0;\n");
    fprintf(file, IND_1"switch(enctyp){\n");
    fprintf(file, IND_2"case Framework.Encode_IndexedNotZero:\n");
    RET_ON_KO(ABSR_METH_ENT_SER__IndexedNotZero(cunit, edsc, file))
    fprintf(file, IND_3"break;\n");
    fprintf(file, IND_2"default: return -1;\n");
    fprintf(file, IND_1"}\n");
    fprintf(file, IND_1"return out_buf.getPos();\n");
    return vlg::RetCode_OK;
}


/***********************************
GEN- VLG_COMP_JAVA_VLG_Entity_abs_meths
***********************************/
RetCode VLG_COMP_JAVA_VLG_Entity_abs_meths(compile_unit &cunit,
                                           entity_desc_comp &edsc,
                                           FILE *file)
{
    fprintf(file,  "/*****************************************************\n"
            "NClass methods\n"
            "*****************************************************/\n");
    fprintf(file,  ABSR_METH_ENT_COPY"\n");
    RET_ON_KO(ABSR_METH_ENT_COPY__Body_impl(cunit, edsc, file))
    fprintf(file, "}\n");
    fprintf(file,  ABSR_METH_ENT_CLONE"\n");
    RET_ON_KO(ABSR_METH_ENT_CLONE__Body_impl(cunit, edsc, file))
    fprintf(file, "}\n");
    fprintf(file,  ABSR_METH_ENT_ISZERO"\n");
    RET_ON_KO(ABSR_METH_ENT_ISZERO__Body_impl(cunit, edsc, file))
    fprintf(file, "}\n");
    fprintf(file,  ABSR_METH_ENT_SETZERO"\n");
    RET_ON_KO(ABSR_METH_ENT_SETZERO__Body_impl(cunit, edsc, file))
    fprintf(file, "}\n");
    fprintf(file,  ABSR_METH_ENT_SET"\n");
    RET_ON_KO(ABSR_METH_ENT_SET__Body_impl(cunit, edsc, file))
    fprintf(file, "}\n");
    fprintf(file,  ABSR_METH_ENT_GCOMPVER"\n");
    RET_ON_KO(ABSR_METH_ENT_GCOMPVER__Body_impl(cunit, edsc, file))
    fprintf(file, "}\n");
    fprintf(file,  ABSR_METH_ENT_GENTDSC"\n");
    RET_ON_KO(ABSR_METH_ENT_GENTDSC__Body_impl(cunit, edsc, file))
    fprintf(file, "}\n");
    fprintf(file,  ABSR_METH_ENT_PTOBUF"\n");
    RET_ON_KO(ABSR_METH_ENT_PTOBUF__Body_impl(cunit, edsc, file))
    fprintf(file, "}\n");
    fprintf(file,  ABSR_METH_ENT_SER"\n");
    RET_ON_KO(ABSR_METH_ENT_SER__Body_impl(cunit, edsc, file))
    fprintf(file, "}\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_JAVA_VLG_Class_abs_meths
***********************************/
RetCode VLG_COMP_JAVA_VLG_Class_abs_meths(compile_unit &cunit,
                                          entity_desc_comp &edsc,
                                          FILE *file)
{
    fprintf(file,  "/*****************************************************\n"
            "NClass methods\n"
            "*****************************************************/\n");
    fprintf(file,  ABSR_METH_GETENTID"\n");
    fprintf(file,  IND_1"return %d;\n", edsc.get_nclassid());
    fprintf(file, "}\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_JAVA_getter_setter
***********************************/
RetCode VLG_COMP_JAVA_getter_setter(compile_unit &cunit,
                                    entity_desc_comp &edsc,
                                    FILE *file)
{
    fprintf(file,  "/*****************************************************\n"
            "getter / setter / iszero\n"
            "*****************************************************/\n");
    auto &entitymap = cunit.get_nentity_map();
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        //getter
        std::string type_str;
        RET_ON_KO(target_type_from_VLG_TYPE(*mdesc->second, entitymap, type_str))
        std::string meth_name;
        meth_name.assign(VLG_COMP_JAVA_GETTER_PFX);
        std::string cameled_fld_name;
        cameled_fld_name.assign(mdesc->second->get_member_name());
        std::transform(cameled_fld_name.begin(), cameled_fld_name.begin(), cameled_fld_name.begin(), ::toupper);
        meth_name.append(cameled_fld_name);
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto inner_edsc = entitymap.end();
            if((inner_edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(inner_edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, "public %s ", type_str.c_str());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, "public %s[] ", type_str.c_str());
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, "public %s ", type_str.c_str());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, "public %s[] ", type_str.c_str());
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() > 1) {
            //primitive type nmemb > 1
            fprintf(file, "public %s%s ", type_str.c_str(),
                    rec_brackets_cond(mdesc->second));
        } else {
            //primitive type nmemb == 1
            fprintf(file, "public %s ", type_str.c_str());
        }
        fprintf(file, "%s(){\n", meth_name.c_str());
        fprintf(file, IND_1"return %s;\n", mdesc->second->get_member_name());
        fprintf(file, "}\n");
        //setter
        meth_name.assign(VLG_COMP_JAVA_SETTER_PFX);
        meth_name.append(cameled_fld_name);
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto edsc = entitymap.end();
            if((edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, "public void %s(%s val){\n", meth_name.c_str(), type_str.c_str());
                        fprintf(file, IND_1"%s = val;\n", mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, "public void %s(%s[] val){\n", meth_name.c_str(), type_str.c_str());
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n"
                                IND_2"%s[i].setValue(val[i].getValue());\n"
                                IND_1"}\n", mdesc->second->get_member_name(), mdesc->second->get_member_name());
                    }
                } else {
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, "public void %s(%s val){\n", meth_name.c_str(), type_str.c_str());
                        fprintf(file, IND_1"%s.set(val);\n", mdesc->second->get_member_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, "public void %s(%s[] val){\n", meth_name.c_str(), type_str.c_str());
                        fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n"
                                IND_2"%s[i].set(val[i]);\n"
                                IND_1"}\n", mdesc->second->get_member_name(),
                                mdesc->second->get_member_name());
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            fprintf(file, "public void %s(%s val){\n",  meth_name.c_str(), type_str.c_str());
            fprintf(file, IND_1"%s = val;\n", mdesc->second->get_member_name());
        } else {
            //primitive type nmemb > 1
            if(isJavaStringType(mdesc->second)) {
                fprintf(file, "public void %s(%s val){\n", meth_name.c_str(), type_str.c_str());
                fprintf(file, IND_1"NMemberDesc mdesc->second = getEntityDesc().getMmbrid_mdesc_Map().get((short)%d);\n",
                        mdesc->second->get_member_id());
                fprintf(file, IND_1"%s = val.substring(0, Math.min(val.length(), mdesc->second.getNmemb()));\n",
                        mdesc->second->get_member_name());
            } else {
                std::string ttype;
                RET_ON_KO(target_type_from_builtin_VLG_TYPE(*mdesc->second, ttype))
                //primitive type nmemb > 1
                fprintf(file, "public void %s(%s[] val){\n", meth_name.c_str(), type_str.c_str());
                fprintf(file, IND_1"for(int i=0; i<%s.length; i++){\n"
                        IND_2"%s[i] = val[i];\n"
                        IND_1"}\n", mdesc->second->get_member_name(), mdesc->second->get_member_name());
            }
        }
        fprintf(file, "}\n");
        //zero method
        fprintf(file, "public boolean isZero%s(){\n", cameled_fld_name.c_str());
        std::string zero_val;
        if(mdesc->second->get_field_type() == Type_ENTITY) {
            auto edsc = entitymap.end();
            if((edsc = entitymap.find(mdesc->second->get_field_usr_str_type())) != entitymap.end()) {
                if(edsc->second->get_nentity_type() == NEntityType_NENUM) {
                    if(mdesc->second->get_nmemb() == 1) {
                        //enum  nmemb == 1
                        fprintf(file, IND_1"return %s.getValue() == 0;\n", mdesc->second->get_member_name());
                    } else {
                        //enum  nmemb > 1
                        fprintf(file, IND_1"for(VLGEnum e : %s){\n"
                                IND_2"if(e.getValue() != 0) return false;\n"
                                IND_1"}\n"
                                IND_1"return true;\n", mdesc->second->get_member_name());
                    }
                } else {
                    //class, struct
                    if(mdesc->second->get_nmemb() == 1) {
                        //class, struct  nmemb == 1
                        fprintf(file, IND_1"return %s.isZero();\n", mdesc->second->get_member_name());
                    } else {
                        //class, struct  nmemb > 1
                        fprintf(file, IND_1"for(NEntity e : %s){\n"
                                IND_2"if(!(e.isZero())) return false;\n"
                                IND_1"}\n"
                                IND_1"return true;\n", mdesc->second->get_member_name());
                    }
                }
            }
        } else if(mdesc->second->get_nmemb() == 1) {
            //primitive type nmemb == 1
            RET_ON_KO(get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), zero_val))
            fprintf(file, IND_1"return %s == %s;\n", mdesc->second->get_member_name(),
                    zero_val.c_str());
        } else {
            if(isJavaStringType(mdesc->second)) {
                fprintf(file, IND_1"return \"\".equals(%s);\n", mdesc->second->get_member_name());
            } else {
                std::string ttype;
                RET_ON_KO(target_type_from_builtin_VLG_TYPE(*mdesc->second, ttype))
                RET_ON_KO(get_zero_val_for_VLG_TYPE(mdesc->second->get_field_type(), zero_val))
                //primitive type nmemb > 1
                fprintf(file, IND_1"for(%s e : %s){\n"
                        IND_2"if(e != %s) return false;\n"
                        IND_1"}\n"
                        IND_1"return true;\n", ttype.c_str(), mdesc->second->get_member_name(),
                        zero_val.c_str());
            }
        }
        fprintf(file, "}\n");
    }
    return vlg::RetCode_OK;
}


/***********************************
GEN- VLG_COMP_JAVA_rep
***********************************/
RetCode VLG_COMP_JAVA_rep(compile_unit &cunit,
                          entity_desc_comp &edsc,
                          FILE *file)
{
    fprintf(file,  "/*****************************************************\n"
            "rep.\n"
            "Fields have been declared public to efficiently perform\n"
            "NEntity.restore() method.\n"
            "**USE GETTERS AND SETTERS TO ACCESS REP FIELDS.**\n"
            "*****************************************************/\n");
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        if(mdesc->second->get_member_type() != MemberType_FIELD) {
            continue;
        }
        std::string type_str;
        RET_ON_KO(target_type_from_VLG_TYPE(*mdesc->second, cunit.get_nentity_map(), type_str))
        if(mdesc->second->get_nmemb() > 1) {
            //primitive type nmemb > 1
            fprintf(file, "public %s%s %s;", type_str.c_str(),
                    rec_brackets_cond(mdesc->second),  mdesc->second->get_member_name());
        } else {
            //primitive type nmemb == 1
            fprintf(file, "public %s %s;", type_str.c_str(),
                    mdesc->second->get_member_name());
        }
        RET_ON_KO(put_newline(file))
    }
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_JAVA_package_decl
***********************************/
RetCode VLG_COMP_JAVA_package_decl(FILE *file, const char *nmsp)
{
    fprintf(file, "package %s;", nmsp);
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_JAVA_vlg_import
***********************************/
RetCode VLG_COMP_JAVA_vlg_import(FILE *file)
{
    fprintf(file, "import java.io.*;\n");
    fprintf(file, "import java.nio.*;\n");
    fprintf(file, "import org.vlg.interf.*;\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_JAVA_VLGEnum
***********************************/
RetCode VLG_COMP_JAVA_VLGEnum(compile_unit &cunit,
                              entity_desc_comp &edsc,
                              FILE *efile)
{
    fprintf(efile, "public class %s extends NEnum{\n", edsc.get_nentity_name());
    fprintf(efile, "/*****************************************************\n"
            "Enum values\n"
            "*****************************************************/\n");
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        fprintf(efile, IND_1"public static final int %s = %ld;\n",
                mdesc->second->get_member_name(), mdesc->second->get_enum_value());
    }
    fprintf(efile, "}");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_JAVA_NClass
***********************************/
RetCode VLG_COMP_JAVA_NClass(compile_unit &cunit,
                             entity_desc_comp &edsc,
                             FILE *efile)
{
    fprintf(efile, "public class %s extends NClass{\n", edsc.get_nentity_name());
    fprintf(efile,  "/*****************************************************\n"
            "NClass ID\n"
            "*****************************************************/\n");
    fprintf(efile, "public static final int %s_NClass_ID=%u;\n",
            edsc.get_nentity_name(), edsc.get_nclassid());
    RET_ON_KO(VLG_COMP_JAVA_VLG_Entity_Ctor(cunit, edsc, efile))
    RET_ON_KO(put_newline(efile))
    RET_ON_KO(VLG_COMP_JAVA_VLG_Entity_abs_meths(cunit, edsc, efile))
    RET_ON_KO(put_newline(efile))
    RET_ON_KO(VLG_COMP_JAVA_VLG_Class_abs_meths(cunit, edsc, efile))
    RET_ON_KO(put_newline(efile))
    RET_ON_KO(VLG_COMP_JAVA_getter_setter(cunit, edsc, efile))
    RET_ON_KO(put_newline(efile))
    RET_ON_KO(VLG_COMP_JAVA_rep(cunit, edsc, efile))
    RET_ON_KO(put_newline(efile))
    fprintf(efile, "}");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_JAVA_NEntityDesc
***********************************/
RetCode VLG_COMP_JAVA_NEntityDesc(compile_unit &cunit,
                                  entity_desc_comp &edsc,
                                  FILE *file)
{
    fprintf(file,   IND_1"NEntityDesc %s_edesc = new NEntityDesc\n"
            IND_1"(\n"
            IND_2"%u, //int entityid\n"
            IND_2"%d, //int entitytype\n"
            IND_2"\"%s\", //String entity package\n"
            IND_2"\"%s\", //String nclassname\n"
            IND_2"%u, //int fild_num\n"
            IND_2"%s //boolean persistent\n"
            IND_1");\n",
            edsc.get_nentity_name(),
            edsc.get_nclassid(),
            edsc.get_nentity_type(),
            edsc.get_nentity_namespace(),
            edsc.get_nentity_name(),
            edsc.get_field_num(),
            edsc.is_persistent() ? "true" : "false");
    auto &mmbrmap = edsc.get_map_id_MMBRDSC();
    for(auto mdesc = mmbrmap.begin(); mdesc != mmbrmap.end(); mdesc++) {
        fprintf(file,   IND_1"NMemberDesc %s_%s_mdesc = new NMemberDesc\n"
                IND_1"(\n"
                IND_2"(short)%u, // short mmbrid\n"
                IND_2"%d, // int mmbr_type\n"
                IND_2"\"%s\", // String mmbr_name\n"
                IND_2"\"%s\", // String mmbr_desc\n"
                IND_2"%d, // int fild_type\n"
                IND_2"%zu, // int nmemb\n"
                IND_2"%u, // int field_nclass_id\n"
                IND_2"\"%s\", // String fild_usr_str_type\n"
                IND_2"%d, // int field_nentitytype\n"
                IND_2"%ld  // int enum_value\n"
                IND_1");\n",
                edsc.get_nentity_name(),
                mdesc->second->get_member_name(),
                mdesc->second->get_member_id(),
                mdesc->second->get_member_type(),
                mdesc->second->get_member_name(),
                mdesc->second->get_member_desc() ? mdesc->second->get_member_desc() : "",
                mdesc->second->get_field_type(),
                mdesc->second->get_nmemb(),
                mdesc->second->get_field_nclassid(),
                mdesc->second->get_field_usr_str_type() ? mdesc->second->get_field_usr_str_type() : "",
                mdesc->second->get_field_nentity_type(),
                mdesc->second->get_enum_value());
        fprintf(file, IND_1"%s_edesc.addMemberDesc(%s_%s_mdesc);\n",
                edsc.get_nentity_name(),
                edsc.get_nentity_name(),
                mdesc->second->get_member_name());
    }
    if(edsc.is_persistent()) {
        auto &kdesc_map = edsc.get_map_keyid_KDESC_mod();
        for(auto keydesc = kdesc_map.begin(); keydesc != kdesc_map.end(); keydesc++) {
            fprintf(file,
                    IND_1"NKeyDesc %s_%d_kdesc = new NKeyDesc((short)%d, %s);\n",
                    edsc.get_nentity_name(),
                    keydesc->second->get_key_id(),
                    keydesc->second->get_key_id(),
                    keydesc->second->is_primary() ? "true" : "false");
            std::set<member_desc_comp *> &kset = keydesc->second->get_key_member_set_m();
            for(auto it = kset.begin(); it != kset.end(); it++) {
                fprintf(file, IND_1"%s_%d_kdesc.addMemberDesc(%s_%s_mdesc);\n",
                        edsc.get_nentity_name(),
                        keydesc->second->get_key_id(),
                        edsc.get_nentity_name(),
                        (*it)->get_member_name());
            }
            fprintf(file, IND_1"%s_edesc.AddKeyDesc(%s_%d_kdesc);\n",
                    edsc.get_nentity_name(),
                    edsc.get_nentity_name(),
                    keydesc->second->get_key_id());
        }
    }
    return vlg::RetCode_OK;
}

extern int comp_ver[4];

/***********************************
GEN- VLG_COMP_Gen_ModelVersion__Java_
***********************************/
RetCode VLG_COMP_Gen_ModelVersion__Java_(compile_unit &cunit,
                                         FILE *file)
{
    fprintf(file,   "/*****************************************************\n"
            "MODEL:%s VERSION\n"
            "*****************************************************/\n", cunit.model_name());
    fprintf(file, "public static String get_MDL_Ver_%s()\n", cunit.model_name());
    fprintf(file, "{\n");
    fprintf(file,
            IND_1"return \"model.%s.ver.%s.compiler.ver.%d.%d.%d.%d.date:%s\";\n",
            cunit.model_name(),
            cunit.model_version(),
            comp_ver[0],
            comp_ver[1],
            comp_ver[2],
            comp_ver[3],
            __DATE__);
    fprintf(file, "}\n");
    return vlg::RetCode_OK;
}

/***********************************
GEN- VLG_COMP_JAVA_EntryPoint
***********************************/
RetCode VLG_COMP_JAVA_EntryPoint(compile_unit &cunit,
                                 std::string &dfile_nm,
                                 FILE *file)
{
    vlg::str_tok tknz(dfile_nm);
    std::string ep_name;
    tknz.next_token(ep_name, VLG_COMP_DOT);
    fprintf(file, "/*****************************************************\n"
            "nem entry point\n"
            "*****************************************************/\n");
    fprintf(file, "public final class %s{\n", ep_name.c_str());
    RET_ON_KO(put_newline(file))
    fprintf(file, "private static NEntityManager nem;\n");
    RET_ON_KO(put_newline(file))
    fprintf(file, "public static NEntityManager getNEntityManager(){\n");
    fprintf(file, IND_1"if(nem != null){\n"
            IND_2"return nem;\n"
            IND_1"}\n");
    fprintf(file, IND_1"nem = new NEntityManager();\n");
    auto &entitymap = cunit.get_nentity_map();
    for(auto edsc = entitymap.begin(); edsc != entitymap.end(); edsc++) {
        RET_ON_KO(VLG_COMP_JAVA_NEntityDesc(cunit, *edsc->second, file))
        fprintf(file, IND_1"nem.extend(%s_edesc);\n", edsc->second->get_nentity_name());
    }
    fprintf(file, IND_1"return nem;\n");
    fprintf(file, "}\n");
    RET_ON_KO(VLG_COMP_Gen_ModelVersion__Java_(cunit, file))
    fprintf(file, "}");
    return vlg::RetCode_OK;
}


/***********************************
ENTRYPOINT- VLG_COMP_Compile_Java
***********************************/
RetCode compile_Java(compile_unit &cunit)
{
    auto &entitymap = cunit.get_nentity_map();
    std::string efile_n;
    FILE *efile = nullptr;
    //render entities
    for(auto edsc = entitymap.begin(); edsc != entitymap.end(); edsc++) {
        efile_n.assign(edsc->second->get_nentity_name());
        efile_n.append(".java");
        CMD_ON_KO(open_output_file(efile_n.c_str(), &efile), exit(1))
        RET_ON_KO(render_hdr(cunit, efile_n, efile))
        RET_ON_KO(put_newline(efile))
        RET_ON_KO(VLG_COMP_JAVA_package_decl(efile, unit_nmspace.c_str()))
        RET_ON_KO(put_newline(efile))
        RET_ON_KO(VLG_COMP_JAVA_vlg_import(efile))
        RET_ON_KO(put_newline(efile))
        switch(edsc->second->get_nentity_type()) {
            case NEntityType_NENUM:
                RET_ON_KO(VLG_COMP_JAVA_VLGEnum(cunit, *edsc->second, efile))
                break;
            case NEntityType_NCLASS:
                RET_ON_KO(VLG_COMP_JAVA_NClass(cunit, *edsc->second, efile))
                break;
            default:
                return vlg::RetCode_KO;
        }
        RET_ON_KO(put_newline(efile))
        fclose(efile);
    }
    //render entities descriptors
    std::string dfile_nm;
    dfile_nm.append(cunit.model_name());
    dfile_nm.append(".java");
    FILE *dfile = nullptr;
    CMD_ON_KO(open_output_file(dfile_nm.c_str(), &dfile), exit(1))
    RET_ON_KO(render_hdr(cunit, dfile_nm, dfile))
    RET_ON_KO(put_newline(dfile))
    RET_ON_KO(VLG_COMP_JAVA_package_decl(dfile, unit_nmspace.c_str()))
    RET_ON_KO(put_newline(dfile))
    RET_ON_KO(VLG_COMP_JAVA_vlg_import(dfile))
    RET_ON_KO(put_newline(dfile))
    RET_ON_KO(VLG_COMP_JAVA_EntryPoint(cunit, dfile_nm, dfile))
    fclose(dfile);
    return vlg::RetCode_OK;
}

}