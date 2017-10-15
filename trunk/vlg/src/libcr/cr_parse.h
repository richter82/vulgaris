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

#ifndef CR_PARSE_H_
#define CR_PARSE_H_
#include "blaze_ascii_string.h"

namespace blaze {

#define CR_MAX_SRC_FILE_NAME_LEN    512
#define CR_MAX_SRC_PATH_LEN        1024
#define CR_MAX_SRC_LINE_LEN         512

#ifdef WIN32
#define CR_FS_SEP       "\\"
#define CR_FS_SEP_C     '\\'
#else
#define CR_FS_SEP       "/"
#define CR_FS_SEP_C     '/'
#endif

/***********************************
SOME TOKEN AND UTILS
***********************************/

#define CR_TK_SP          " "
#define CR_TK_LF          "\n"            //line feed
#define CR_TK_RC          "\r"            //carriag. return
#define CR_TK_FF          "\f"            //form feed
#define CR_TK_TAB         "\t"            //tab
#define CR_TK_EQUAL       "="
#define CR_TK_COMA        ","
#define CR_TK_DOT         "."
#define CR_TK_QT          "\""            //double quote
#define CR_TK_RBR         "]"
#define CR_TK_RBL         "["
#define CR_TK_CBR         "}"
#define CR_TK_CBL         "{"
#define CR_TK_PLUS        "+"
#define CR_TK_COLON       ":"
#define CR_TK_SEMICOLON   ";"

#define CR_1IND               "    "
#define CR_2IND               CR_1IND \
                              CR_1IND

#define CR_3IND               CR_1IND \
                              CR_1IND \
                              CR_1IND

#define CR_4IND               CR_1IND \
                              CR_1IND \
                              CR_1IND \
                              CR_1IND

#define CR_5IND               CR_1IND \
                              CR_1IND \
                              CR_1IND \
                              CR_1IND \
                              CR_1IND

#define CR_6IND               CR_1IND \
                              CR_1IND \
                              CR_1IND \
                              CR_1IND \
                              CR_1IND \
                              CR_1IND

//default delimits
#define CR_DF_DLMT          CR_TK_SP \
                            CR_TK_LF \
                            CR_TK_RC \
                            CR_TK_FF \
                            CR_TK_TAB

//new line delimits
#define CR_NL_DLMT          CR_TK_LF \
                            CR_TK_RC \
                            CR_TK_FF

//space and tab delimits
#define CR_ST_DLMT          CR_TK_TAB \
                            CR_TK_SP

//rectangular braces
#define CR_RB_DLMT          CR_TK_RBR \
                            CR_TK_RBL

//curly braces
#define CR_CB_DLMT          CR_TK_CBR \
                            CR_TK_CBL

#define CR_SKIP_SPACES(tkn) \
if(tkn == CR_TK_SP){ \
    continue; \
}

#define CR_SKIP_SP_TABS(tkn) \
if(tkn == CR_TK_SP || tkn == CR_TK_TAB){ \
    continue; \
}

#define CR_SKIP_NEWLINE(tkn) \
if(is_new_line(tkn)){ \
    continue; \
}

#define CR_DO_CMD_ON_TKN(tkn, cmd) \
if(tkn == val){ \
    cmd; \
}

#define CR_DO_CMD_ON_NEWLINE(tkn, cmd) \
if(is_new_line(tkn)){ \
    cmd; \
}

#define CR_BREAK_ON_TKN(tkn, val) \
if(tkn == val){ \
    break; \
}

#define CR_SKIP_SP_TABS(tkn) \
if(tkn == CR_TK_SP || tkn == CR_TK_TAB){ \
    continue; \
}

bool is_new_line(ascii_string &str);
bool is_comment_line(const char *line);
bool is_blank_line(const char *line);
bool string_is_int_number(const char *str);
RetCode load_file(FILE *fdesc, ascii_string &data);

}

#endif
