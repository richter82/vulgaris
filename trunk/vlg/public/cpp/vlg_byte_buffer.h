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

#ifndef BLZ_BBUF_H_
#define BLZ_BBUF_H_
#include "vlg.h"

namespace vlg {

/** @brief class grow_byte_buffer.
*/
class grow_byte_buffer_rep;
class grow_byte_buffer {
    public:

        /**
        */
        explicit grow_byte_buffer();

        /**
        */
        ~grow_byte_buffer();

        /**
        @param initial_capacity
        @return
        */
        RetCode         init(size_t initial_capacity);

        /**
        */
        void            reset();

        /**
        */
        void            flip();

        /**
        @param amount
        @return
        */
        RetCode         grow(size_t amount);

        /**
        @param capacity
        @return
        */
        RetCode         ensure_capacity(size_t capacity);

        /**
        @param buffer
        @param offset
        @param length
        @return
        */
        RetCode         append(const void *buffer,
                               size_t offset,
                               size_t length);

        /**
        @param value
        @return
        */
        RetCode         append_ushort(unsigned short value);

        /**
        @param value
        @return
        */
        RetCode         append_uint(unsigned int value);

        /**
        @param buffer
        @param gbb_offset
        @param length
        @return
        */
        RetCode         put(const void *buffer,
                            size_t gbb_offset,
                            size_t length);

        /**
        @return
        */
        size_t          position()      const;

        /**
        @return
        */
        size_t          limit()         const;

        /**
        @return
        */
        size_t          mark()          const;

        /**
        @return
        */
        size_t          capacity()      const;

        /**
        @return
        */
        size_t          remaining()     const;

        /**
        @return
        */
        unsigned char  *buffer();

        /**
        @return
        */
        char           *buffer_as_char();

        /**
        @return
        */
        unsigned int   *buffer_as_uint();

        /**
        @param amount
        @return
        */
        RetCode         advance_pos_write(size_t amount);

        /**
        @param new_pos
        @return
        */
        RetCode         set_pos_write(size_t new_pos);

        /**
        @param amount
        @return
        */
        void            move_pos_write(size_t amount);

        /**
        @param amount
        @return
        */
        RetCode         advance_pos_read(size_t amount);

        /**
        @param new_pos
        @return
        */
        RetCode         set_pos_read(size_t new_pos);

        /**
        */
        void            set_mark();

        /**
        @param new_mark
        @return
        */
        RetCode         set_mark(size_t new_mark);

        /**
        @return
        */
        size_t          from_mark()     const;

        /**
        @return
        */
        size_t          available_read();

        /**
        @param length
        @param out
        @return
        */
        RetCode         read(size_t length,
                             void *out);

        /**
        @param out
        @return
        */
        RetCode         read_ushort(unsigned short *out);

        /**
        @param out
        @return
        */
        RetCode         read_uint(unsigned int *out);

        /**
        @param out
        @return
        */
        RetCode         read_uint_to_sizet(size_t *out);

    private:
        grow_byte_buffer_rep *impl_;
};
}

#endif
