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

namespace vlg {

//-----------------------------
// #VER#
//-----------------------------
#define VLG_COMP_STRT_STR "vlg.compiler.ver.%d.%d.%d.%d.date:" __DATE__
int comp_ver[4]  = {0,0,0,0};

vlg::config_loader conf_ldr;
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
            COMMAND_IF_NOT_OK(comp_cfg.path_list.push_back(value), exit(1))
        } else {
            fprintf(stderr, PAR_REQ_VAL_FMT, VLG_COMP_START_PAR_INCLUDE);
            EXIT_ACTION_STDOUT("")
        }
    }
    if(!strcmp(param, VLG_COMP_START_PAR_OUTPT_DIR)) {
        if(value) {
#ifdef WIN32
            comp_cfg.out_dir = _strdup(value);
#else
            comp_cfg.out_dir = strdup(value);
#endif
        } else {
            fprintf(stderr, PAR_REQ_VAL_FMT, VLG_COMP_START_PAR_OUTPT_DIR);
            EXIT_ACTION_STDOUT("")
        }
    }
    if(!strcmp(param, VLG_COMP_START_PAR_FILES)) {
        if(value) {
            vlg::ascii_string tkn;
            vlg::ascii_string_tok tknz;
            tknz.init(value);
            while(!tknz.next_token(tkn, CR_DF_DLMT)) {
                COMMAND_IF_NOT_OK(comp_cfg.file_list.push_back(tkn.internal_buff()), exit(1))
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
    vlg::comp_cfg.init();
    if(!strcmp("-file", argv[1])) {
        COMMAND_IF_NOT_OK(vlg::conf_ldr.init("params"),
                          EXIT_ACTION_STDOUT("reading parameters file" COMPILE_FAIL))
    } else {
        COMMAND_IF_NOT_OK(vlg::conf_ldr.init(argc, argv),
                          EXIT_ACTION_STDOUT("reading parameters" COMPILE_FAIL))
    }
    COMMAND_IF_NOT_OK(vlg::comp_cfg.path_list.push_back("."), exit(1))
    COMMAND_IF_NOT_OK(vlg::conf_ldr.load_config(),
                      EXIT_ACTION_STDOUT("loading configuration" COMPILE_FAIL))
    vlg::conf_ldr.dump_config();
    vlg::conf_ldr.enum_params(vlg::vlg_comp_param_clbk);
    char fname[CR_MAX_SRC_FILE_NAME_LEN];
    vlg::comp_cfg.file_list.start_iteration();
    while(!vlg::comp_cfg.file_list.next(fname)) {
        vlg::compile_unit cmpl_unit;
        COMMAND_IF_NOT_OK(cmpl_unit.init(fname),
                          EXIT_ACTION_STDOUT("init compilation unit" COMPILE_FAIL))
        printf(STG_FMT_0,
               VLG_COMP_INF_START,
               VLG_COMP_INF_PARS_FILE,
               fname);
        COMMAND_IF_NOT_OK(cmpl_unit.parse(),
                          EXIT_ACTION_STDOUT("parsing file" COMPILE_FAIL))
        printf(STG_FMT_0,
               VLG_COMP_INF_END,
               VLG_COMP_INF_PARS_FILE,
               fname);
        printf(STG_FMT_0,
               VLG_COMP_INF_START,
               VLG_COMP_INF_COMPL_FILE,
               fname);
        COMMAND_IF_NOT_OK(cmpl_unit.compile(),
                          EXIT_ACTION_STDOUT("compiling file" COMPILE_FAIL))
        printf(STG_FMT_0,
               VLG_COMP_INF_END,
               VLG_COMP_INF_COMPL_FILE,
               fname);
    }
    printf("\n%-11s %s [OK]\n",
           "",
           VLG_COMP_INF_JOB_DONE);
    return 0;
}