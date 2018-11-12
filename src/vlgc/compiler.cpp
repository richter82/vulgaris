/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "compiler.h"

namespace vlg {

// #VER#

#define VLG_COMP_STRT_STR "vlg.compiler.ver.%d.%d.%d.%d.date:" __DATE__
int comp_ver[4]  = {0, 0, 0, 0};

vlg::cfg_ldr conf_ldr;
compiler_config comp_cfg;

#define PAR_REQ_VAL_FMT "parameter: %s - requires an argument"

void vlg_comp_param_clbk(int pnum, const char *param, const char *value)
{
    if(!strcmp(param, VLG_COMP_START_PAR_VRBLVL)) {
        if(value) {
            comp_cfg.verblvl = atoi(value);
        } else {
            fprintf(stderr, PAR_REQ_VAL_FMT, VLG_COMP_START_PAR_VRBLVL);
            EXIT_ACTION_STDOUT("")
        }
    }
    if(!strcmp(param, VLG_COMP_START_PAR_INCLUDE)) {
        if(value) {
            comp_cfg.path_list.push_back(value);
        } else {
            fprintf(stderr, PAR_REQ_VAL_FMT, VLG_COMP_START_PAR_INCLUDE);
            EXIT_ACTION_STDOUT("")
        }
    }
    if(!strcmp(param, VLG_COMP_START_PAR_OUTPT_DIR)) {
        if(value) {
            comp_cfg.out_dir = strdup(value);
        } else {
            fprintf(stderr, PAR_REQ_VAL_FMT, VLG_COMP_START_PAR_OUTPT_DIR);
            EXIT_ACTION_STDOUT("")
        }
    }
    if(!strcmp(param, VLG_COMP_START_PAR_FILES)) {
        if(value) {
            std::string tkn, v(value);
            vlg::str_tok tknz(v);
            while(tknz.next_token(tkn, DF_DLM)) {
                comp_cfg.file_list.push_back(tkn);
            }
        } else {
            fprintf(stderr, PAR_REQ_VAL_FMT, VLG_COMP_START_PAR_FILES);
            EXIT_ACTION_STDOUT("")
        }
    }
    if(!strcmp(param, VLG_COMP_START_PAR_LANG)) {
        if(value) {
            comp_cfg.lang = lang_from_str(value);
        } else {
            fprintf(stderr, PAR_REQ_VAL_FMT, VLG_COMP_START_PAR_LANG);
            EXIT_ACTION_STDOUT("")
        }
    }
}

}

/***********************************
VERSION AND USAGE
***********************************/
void print_comp_ver()
{
    printf("----------------------------------------------------\n");
    printf("- " VLG_COMP_STRT_STR"\n",
           vlg::comp_ver[0],
           vlg::comp_ver[1],
           vlg::comp_ver[2],
           vlg::comp_ver[3]);
    printf("----------------------------------------------------\n");
}

void print_usage()
{
    printf("Usage: vlgc <parameter> <argument> | <parameter argument list>\n");
    printf("where possible parameters include:\n");
    printf("  %-20s%s\n", "-file",
           "Read <parameter> <argument> | <parameter argument list> from a file named \"params\".");
    printf("  %-20s%s\n", "-I",
           "Specify where to find files to be compiled. Double quote is required on argument.");
    printf("  %-20s%s\n", "-O",
           "Specify where to put generated files. Double quote is required on argument.");
    printf("  %-20s%s\n", "-F",
           "Specify files to be compiled. Double quote is required on argument.");
    printf("  %-20s%s\n", "-lang",
           "Specify target programming language of compiled files. possible values are: CPP, JAVA, SWIFT");
}

/***********************************
main
***********************************/

int main(int argc, char *argv[])
{
    print_comp_ver();
    if(argc < 2) {
        print_usage();
        EXIT_ACTION_STDOUT("invalid parameters")
    }
    if(!strcmp("-file", argv[1])) {
        CMD_ON_KO(vlg::conf_ldr.init("params"), EXIT_ACTION_STDOUT("reading parameters file" COMPILE_FAIL))
    } else {
        CMD_ON_KO(vlg::conf_ldr.init(argc, argv), EXIT_ACTION_STDOUT("reading parameters" COMPILE_FAIL))
    }
    vlg::comp_cfg.path_list.push_back(".");
    CMD_ON_KO(vlg::conf_ldr.load_config(), EXIT_ACTION_STDOUT("loading configuration" COMPILE_FAIL))
    vlg::conf_ldr.dump_config();
    vlg::conf_ldr.enum_params(vlg::vlg_comp_param_clbk);

    for(auto it = vlg::comp_cfg.file_list.begin(); it != vlg::comp_cfg.file_list.end(); it++) {
        vlg::compile_unit cmpl_unit;
        CMD_ON_KO(cmpl_unit.init(it->c_str()), EXIT_ACTION_STDOUT("init compilation unit" COMPILE_FAIL))
        printf(STG_FMT_0,
               VLG_COMP_INF_START,
               VLG_COMP_INF_PARS_FILE,
               it->c_str());
        CMD_ON_KO(cmpl_unit.parse(), EXIT_ACTION_STDOUT("parsing file" COMPILE_FAIL))
        printf(STG_FMT_0,
               VLG_COMP_INF_END,
               VLG_COMP_INF_PARS_FILE,
               it->c_str());
        printf(STG_FMT_0,
               VLG_COMP_INF_START,
               VLG_COMP_INF_COMPL_FILE,
               it->c_str());
        CMD_ON_KO(cmpl_unit.compile(), EXIT_ACTION_STDOUT("compiling file" COMPILE_FAIL))
        printf(STG_FMT_0,
               VLG_COMP_INF_END,
               VLG_COMP_INF_COMPL_FILE,
               it->c_str());
    }
    printf("\n%-11s %s [OK]\n", "", VLG_COMP_INF_JOB_DONE);
    return 0;
}