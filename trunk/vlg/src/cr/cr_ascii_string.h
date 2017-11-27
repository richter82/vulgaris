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

#ifndef VLG_ASCIISTR_H_
#define VLG_ASCIISTR_H_
#include "vlg.h"

namespace vlg {

/** @brief Basic ascii string class.
*/
class ascii_string_impl;
class ascii_string {
    public:

        /**
        */
        explicit ascii_string();

        /**
        illegal ctor

        @param oth
        */
        explicit ascii_string(ascii_string &oth);

        /**
        */
        ~ascii_string();

    public:

        /**
        @param str
        @return
        */
        RetCode     assign(const char *str);

        /**
        @param str
        @return
        */
        RetCode     assign(const ascii_string &str);

        /**
        @param index
        @return
        */
        char        char_at(size_t index)               const;

        /**
        @return
        */
        RetCode     clear();

        /**
        @param str
        @return
        */
        int         compare(const ascii_string &str)    const;

        /**
        @param str
        @return
        */
        int         compare(const char *str)            const;

        /**
        @param str
        @return
        */
        bool        equals(const ascii_string &str)     const;

        /**
        @param str
        @return
        */
        bool        equals(const char *str)             const;

        /**
        @param c
        @return
        */
        long        index_of(char c)                    const;

        /**
        @param str
        @return
        */
        long        index_of(const char *str)           const;

        /**
        @param str
        @return
        */
        long        index_of(const ascii_string &str)   const;

        /**
        @param start
        @param c
        @return
        */
        long        index_of(size_t start, char c)      const;

        /**
        @param start
        @param str
        @return
        */
        long        index_of(size_t start,
                             const char *str)           const;

        /**
        @param start
        @param str
        @return
        */
        long        index_of(size_t start,
                             const ascii_string &str)   const;

        /**
        @return
        */
        size_t      length()                            const;

        /**
        @return
        */
        const char *internal_buff()                     const;

        /**
        @return
        */
        char       *internal_buff_m()                   const;

        /**
        @param copy
        @return
        */
        void        to_buffer(char *copy)               const;

        /**
        @return
        */
        char       *new_buffer()                        const;

        /**
        @param c
        @return
        */
        RetCode     append(char c);

        /**
        @param str
        @return
        */
        RetCode     append(const char *str);

        /**
        @param str
        @return
        */
        RetCode     append(const ascii_string &str);

        /**
        @param start
        @param end
        @param str
        @return
        */
        RetCode     replace(size_t start,
                            size_t end,
                            const char *str);

        /**
        @param start
        @param end
        @param str
        @return
        */
        RetCode     replace(size_t start,
                            size_t end,
                            const ascii_string &str);

        /**
        @param start
        @param end
        @param out
        @return
        */
        RetCode     substring(size_t start,
                              size_t end,
                              ascii_string &out)        const;

        /**
        @param start
        @param end
        @param out
        @return
        */
        RetCode     substring(size_t start,
                              size_t end,
                              char *out)                const;

        /**
        @param beginidx
        @param out
        @return
        */
        RetCode     substring(size_t beginidx,
                              ascii_string &out)        const;

        /**
        @param beginidx
        @param out
        @return
        */
        RetCode     substring(size_t beginidx,
                              char *out)                const;

        /**
        */
        void        lowercase();

        /**
        */
        void        uppercase();

        /**
        */
        void        first_char_uppercase();

        /**
        */
        void        trim();

    public:
        bool        operator == (const char *str)           const;
        bool        operator == (const ascii_string &str)   const;
        bool        operator != (const char *str)           const;
        bool        operator != (const ascii_string &str)   const;
        bool        operator > (const char *str)            const;
        bool        operator < (const char *str)            const;
        bool        operator > (const ascii_string &str)    const;
        bool        operator < (const ascii_string &str)    const;

    private:
        ascii_string_impl *impl_;
};

/** @brief Basic ascii string tokenizer class.

*/
class ascii_string_tok_impl;
class ascii_string_tok {
    public:

        /**
        */
        explicit ascii_string_tok();

        /**
        illegal ctor

        @param oth
        */
        explicit ascii_string_tok(ascii_string_tok &oth);

        /**
        */
        ~ascii_string_tok();

    public:

        /**
        @param str
        @return
        */
        RetCode init(const ascii_string &str);

        /**
        @param str
        @return
        */
        RetCode init(const char *str);

        /**
        @param out
        @param delimiters
        @param return_delimiters
        @return
        */
        RetCode next_token(ascii_string &out,
                           const char *delimiters = NULL,
                           bool return_delimiters = false);

        /**
        @param out
        @param delimiters
        @param return_delimiters
        @return
        */
        RetCode next_token(char *out,
                           const char *delimiters = NULL,
                           bool return_delimiters = false);

        /**
        @param return_delimiters
        @return
        */
        bool has_more_tokens(bool return_delimiters = false);

        /**
        */
        void reset();

    private:
        ascii_string_tok_impl *impl_;
};
}

#endif
