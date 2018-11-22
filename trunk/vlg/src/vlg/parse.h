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
struct str_tok {
        explicit str_tok(const std::string &str);

        ~str_tok();

        bool next_token(std::string &out,
                        const char *delimiters = nullptr,
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
        const std::string &str_;
        std::string delimiters_;
        bool ret_delims_, delims_changed_;
};
}

namespace vlg {

#if defined WIN32 && defined _MSC_VER
#define FS_SEP      "\\"
#define FS_SEP_CH   '\\'
#else
#define FS_SEP      "/"
#define FS_SEP_CH   '/'
#endif

/***********************************
SOME TOKEN AND UTILS
***********************************/

#define SP  " "
#define LF  "\n"    //line feed
#define RC  "\r"    //carriag. return
#define FF  "\f"    //form feed
#define TAB "\t"    //tab
#define EQ  "="
#define CM  ","
#define DT  "."
#define QT  "\""    //double quote
#define RBR "]"
#define RBL "["
#define CBR "}"
#define CBL "{"
#define PLUS    "+"
#define CLN ":"
#define SCLN    ";"

#define IND_1   "    "
#define IND_2   IND_1 \
                IND_1

#define IND_3   IND_1 \
                IND_1 \
                IND_1

#define IND_4   IND_1 \
                IND_1 \
                IND_1 \
                IND_1

#define IND_5   IND_1 \
                IND_1 \
                IND_1 \
                IND_1 \
                IND_1

#define IND_6   IND_1 \
                IND_1 \
                IND_1 \
                IND_1 \
                IND_1 \
                IND_1

//default delimits
#define DF_DLM  SP \
                LF \
                RC \
                FF \
                TAB

//new line delimits
#define NL_DLM  LF \
                RC \
                FF

//space and tab delimits
#define ST_DLM  TAB \
                SP

//rectangular braces
#define RB_DLM  RBR \
                RBL

//curly braces
#define CB_DLM  CBR \
                CBL

#define SKIP_SP(tkn) \
if(tkn == SP){ \
    continue; \
}

#define SKIP_SP_TAB(tkn) \
if(tkn == SP || tkn == TAB){ \
    continue; \
}

#define SKIP_NL(tkn) \
if(is_new_line(tkn)){ \
    continue; \
}

#define DO_CMD_ON_TKN(tkn, cmd) \
if(tkn == ){ \
    cmd; \
}

#define DO_CMD_ON_NL(tkn, cmd) \
if(is_new_line(tkn)){ \
    cmd; \
}

#define BRK_ON_TKN(tkn, val) \
if(tkn == val){ \
    break; \
}

inline bool is_new_line(const std::string &str)
{
    return (str == "\n" || str == "\r" || str == "\f");
}

bool is_comment_line(const char *line);
bool is_blank_line(const char *line);
bool string_is_number(const char *str);
RetCode load_file(FILE *fdesc, std::string &data);

}
