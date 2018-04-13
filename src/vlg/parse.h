/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "vlg.h"

namespace vlg {

/** @brief string tokenizer class.
*/
class str_tok {
    public:
        explicit str_tok(const char *str);
        explicit str_tok(std::string &str);
        ~str_tok();

    public:
        bool next_token(std::string &out,
                        const char *delimiters = NULL,
                        bool return_delimiters = false);

        bool has_more_tokens(bool return_delimiters = false);

        void reset() {
            current_position_ = 0;
        }

    private:
        long skip_delimit(long start_pos);
        long scan_token(long start_pos);

    private:
        long current_position_, max_position_, new_position_;
        std::string str_;
        std::string delimiters_;
        bool ret_delims_, delims_changed_;
};
}

namespace vlg {

#if defined WIN32 && defined _MSC_VER
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
if(tkn == ){ \
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

bool is_new_line(std::string &str);
bool is_comment_line(const char *line);
bool is_blank_line(const char *line);
bool string_is_number(const char *str);
RetCode load_file(FILE *fdesc, std::string &data);

}
