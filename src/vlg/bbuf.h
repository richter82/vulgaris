/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "vlg.h"

namespace vlg {

/** @brief g_bbuf.
*/
struct g_bbuf {
        explicit g_bbuf();
        explicit g_bbuf(size_t initial_capacity);
        ~g_bbuf();

        void reset();
        void flip();
        RetCode grow(size_t amount);
        RetCode ensure_capacity(size_t capacity);

        RetCode append(const void *buffer,
                       size_t offset,
                       size_t length);

        RetCode append_ushort(unsigned short);
        RetCode append_uint(unsigned int);

        RetCode put(const void *buffer,
                    size_t gbb_offset,
                    size_t length);

        size_t position() const;
        size_t limit() const;
        size_t mark() const;
        size_t capacity() const;
        size_t remaining() const;
        unsigned char *buffer();
        char *buffer_as_char();
        unsigned int *buffer_as_uint();
        RetCode advance_pos_write(size_t amount);
        RetCode set_pos_write(size_t new_pos);
        void move_pos_write(size_t amount);
        RetCode advance_pos_read(size_t amount);
        RetCode set_pos_read(size_t new_pos);
        void set_mark();
        RetCode set_mark(size_t new_mark);
        size_t from_mark() const;
        size_t available_read();
        RetCode read(size_t length, void *out);
        RetCode read_ushort(unsigned short *);
        RetCode read_uint(unsigned int *);
        RetCode read_uint_to_sizet(size_t *);

        size_t capcty_;
        size_t pos_;
        size_t limit_;
        size_t mark_;
        char *buf_;

    private:
        RetCode init(size_t initial_capacity);
};
}
