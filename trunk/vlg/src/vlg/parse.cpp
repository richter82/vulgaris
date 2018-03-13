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

#include "glob.h"

namespace vlg {

static const char *def_delims = " \t\n\r\f";

str_tok::str_tok(std::string &str)  :
    current_position_(0),
    max_position_((long)str.length()),
    new_position_(-1),
    str_(str),
    delimiters_(def_delims),
    ret_delims_(false),
    delims_changed_(false)
{
}

str_tok::str_tok(const char *str) :
    current_position_(0),
    max_position_((long)strlen(str)),
    new_position_(-1),
    str_(str),
    delimiters_(def_delims),
    ret_delims_(false),
    delims_changed_(false)
{
}

str_tok::~str_tok()
{}

long str_tok::skip_delimit(long start_pos)
{
    long position = start_pos;
    while(!ret_delims_ && position < max_position_) {
        if(delimiters_.find(str_.at(position)) == std::string::npos) {
            break;
        }
        position++;
    }
    return position;
}

long str_tok::scan_token(long start_pos)
{
    long position = start_pos;
    while(position < max_position_) {
        if(delimiters_.find(str_.at(position)) != std::string::npos) {
            break;
        }
        position++;
    }
    if(ret_delims_ && (start_pos == position)) {
        if(delimiters_.find(str_.at(position)) != std::string::npos) {
            position++;
        }
    }
    return position;
}

bool str_tok::next_token(std::string &out,
                         const char *delimiters,
                         bool return_delimiters)
{
    if(delimiters) {
        delimiters_.assign(delimiters);
        delims_changed_ = true;
    }
    ret_delims_ = return_delimiters;
    current_position_ = (new_position_ >= 0 && !delims_changed_) ?
                        new_position_ :
                        skip_delimit(current_position_);
    delims_changed_ = false;
    new_position_ = -1;
    if(current_position_ >= max_position_) {
        return false;
    }
    long start = current_position_;
    current_position_ = scan_token(current_position_);
    out = str_.substr(start, current_position_ - start);
    return true;
}

bool str_tok::has_more_tokens(bool return_delimiters)
{
    ret_delims_ = return_delimiters;
    new_position_ = skip_delimit(current_position_);
    return (new_position_ < max_position_);
}

}

namespace vlg {

bool is_new_line(std::string &str)
{
    return (str == "\n" || str == "\r" || str == "\f");
}

bool is_blank_line(const char *line)
{
    if(line[0] == '\n') {
        return true;
    } else if(line[0] == ' ' || line[0] == '\t') {
        return is_blank_line(&line[1]);
    } else {
        return false;
    }
}

bool is_comment_line(const char *line)
{
    if(line[0] == '#') {
        return true;
    } else if(line[0] == ' ' || line[0] == '\t') {
        return is_comment_line(&line[1]);
    } else {
        return false;
    }
}

bool string_is_number(const char *str)
{
    int i = 0;
    bool lc = false;
    if(str[i] == '-') {
        i++;
    }
    while(str[i] != '\0' && (lc = isdigit(str[i++]) != 0));
    return lc;
}

/***********************************
LOAD- CR_LoadIFile
***********************************/
RetCode load_file(FILE *fdesc, std::string &data)
{
    unsigned long linenum = 0;
    char line_buf[CR_MAX_SRC_LINE_LEN+2]; // CR LF windows
    while(!feof(fdesc)) {
        if(fgets(line_buf, CR_MAX_SRC_LINE_LEN+2, fdesc)) {
            linenum++;
            if(is_blank_line(line_buf)) {
                data.append("\n");
            } else {
                data.append(line_buf);
            }
        }
    }
    CMD_ON_KO(fclose(fdesc), EXIT_ACTION)
    return RetCode_OK;
}

}
