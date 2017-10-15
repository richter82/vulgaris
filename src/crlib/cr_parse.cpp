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

#include "blz_glob_int.h"
#include <ctype.h>

namespace blaze {

bool is_new_line(ascii_string &str)
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

bool string_is_int_number(const char *str)
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
RetCode load_file(FILE *fdesc, ascii_string &data)
{
    unsigned long linenum = 0;
    char line_buf[CR_MAX_SRC_LINE_LEN+2]; // CR LF windows
    while(!feof(fdesc)) {
        if(fgets(line_buf, CR_MAX_SRC_LINE_LEN+2, fdesc)) {
            linenum++;
            if(is_blank_line(line_buf)) {
                COMMAND_IF_NOT_OK(data.append("\n"), EXIT_ACTION("loading file"))
            } else {
                COMMAND_IF_NOT_OK(data.append(line_buf), EXIT_ACTION("loading file"))
            }
        }
    }
    COMMAND_IF_NOT_OK(fclose(fdesc), EXIT_ACTION("closing input file"))
    return RetCode_OK;
}

}
