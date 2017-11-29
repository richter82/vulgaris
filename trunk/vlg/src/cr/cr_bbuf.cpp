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

#include "cr_bbuf.h"
#include "vlg_globint.h"

namespace vlg {

void *grow_buff_or_die(void *buf, size_t cur_size, size_t amnt)
{
    void *nout = NULL;
    if(buf) {
        COMMAND_IF_NULL(nout = realloc(buf, cur_size+amnt), EXIT_ACTION)
    } else {
        COMMAND_IF_NULL(nout = malloc(amnt), EXIT_ACTION)
    }
    return nout;
}

//-----------------------------
// grow_byte_buffer_rep
class grow_byte_buffer_impl {
    public:
        grow_byte_buffer_impl() :
            capcty_(0),
            pos_(0),
            limit_(0),
            mark_(0),
            buf_(0) {}

        ~grow_byte_buffer_impl() {
            if(buf_) {
                free(buf_);
            }
        }

        RetCode r_init(size_t capcty) {
            if(!capcty) {
                return RetCode_BADARG;
            }
            COMMAND_IF_NULL(buf_ = (unsigned char *)malloc(capcty), EXIT_ACTION)
            return RetCode_OK;
        }

        RetCode r_grow(size_t amnt) {
            if(!amnt) {
                return RetCode_BADARG;
            }
            capcty_ += amnt;
            COMMAND_IF_NULL(buf_ = (unsigned char *)realloc(buf_, capcty_), EXIT_ACTION)
            return RetCode_OK;
        }

        RetCode r_ens_cpcty(size_t capcty) {
            if(!capcty) {
                return RetCode_BADARG;
            }
            if(capcty_ < capcty) {
                r_grow(capcty - capcty_);
            }
            return RetCode_OK;
        }

        void r_reset() {
            pos_ = limit_ = mark_ = 0;
        }

        void r_flip() {
            pos_ = mark_;
        }

        RetCode r_apnd(const void *buf, size_t offst, size_t len) {
            if(!len) {
                return RetCode_BADARG;
            }
            if((capcty_ - pos_) < len) {
                r_grow(len);
            }
            memcpy(&buf_[pos_], &((unsigned char *)buf)[offst], len);
            pos_ += len;
            limit_ = pos_;
            return RetCode_OK;
        }

        RetCode r_apnd_ushort(unsigned short val) {
            if((capcty_ - pos_) < 2) {
                r_grow(2);
            }
            memcpy(&buf_[pos_], &val, 2);
            pos_ += 2;
            limit_ = pos_;
            return RetCode_OK;
        }

        RetCode r_apnd_uint(unsigned int val) {
            if((capcty_ - pos_) < 4) {
                r_grow(4);
            }
            memcpy(&buf_[pos_], &val, 4);
            pos_ += 4;
            limit_ = pos_;
            return RetCode_OK;
        }

        RetCode r_put(const void *buf, size_t pos, size_t len) {
            if(limit_ < pos) {
                return RetCode_BADARG;
            }
            memcpy(&buf_[pos], &((unsigned char *)buf)[0], len);
            return RetCode_OK;
        }

        size_t r_pos() const {
            return pos_;
        }

        size_t r_lim() const {
            return limit_;
        }

        size_t r_mrk() const {
            return mark_;
        }

        size_t r_cpcty() const {
            return capcty_;
        }

        size_t r_rem() const {
            return capcty_ - pos_;
        }

        unsigned char *r_buf() {
            return buf_;
        }

        unsigned int *r_buf_as_uint() {
            return (unsigned int *)buf_;
        }

        char *r_buf_as_char() {
            return (char *)buf_;
        }

        RetCode r_adv_pos_write(size_t amnt) {
            if((capcty_ - pos_) < amnt) {
                r_grow(amnt);
            }
            pos_ += amnt;
            limit_ = pos_;
            return RetCode_OK;
        }

        RetCode r_set_pos_write(size_t newpos) {
            if(capcty_ < newpos) {
                r_grow(pos_ - capcty_);
            }
            pos_ = limit_ = newpos;
            return RetCode_OK;
        }

        void r_mv_pos_write(size_t amnt) {
            pos_ += amnt;
            limit_ = pos_;
        }

        RetCode r_adv_pos_read(size_t amnt) {
            if((limit_ - pos_) < amnt) {
                return RetCode_BOVFL;
            }
            pos_ += amnt;
            return RetCode_OK;
        }

        RetCode r_set_pos_read(size_t newpos) {
            if(limit_ < newpos) {
                return RetCode_BOVFL;
            }
            pos_ = newpos;
            return RetCode_OK;
        }

        void r_set_mrk() {
            mark_ = pos_;
        }

        RetCode r_set_mrk(size_t newmrk) {
            if(limit_ < newmrk) {
                return RetCode_BADARG;
            }
            mark_ = newmrk;
            return RetCode_OK;
        }

        size_t r_frm_mrk() const {
            return pos_ - mark_;
        }

        size_t r_avl_read() {
            return limit_ - pos_;
        }

        RetCode r_read(size_t len, void *out) {
            if(limit_ < (pos_ + len)) {
                return RetCode_BOVFL;
            }
            memcpy(&((unsigned char *)out)[0], &buf_[pos_], len);
            pos_ += len;
            return RetCode_OK;
        }

        RetCode r_read_ushort(unsigned short *out) {
            if(limit_ < (pos_ + 2)) {
                return RetCode_BOVFL;
            }
            memcpy(&((unsigned char *)out)[0], &buf_[pos_], 2);
            pos_ += 2;
            return RetCode_OK;
        }

        RetCode r_read_uint(unsigned int *out) {
            if(limit_ < (pos_ + 4)) {
                return RetCode_BOVFL;
            }
            memcpy(&((unsigned char *)out)[0], &buf_[pos_], 4);
            pos_ += 4;
            return RetCode_OK;
        }

        RetCode r_read_uint_to_sizet(size_t *out) {
            if(limit_ < (pos_ + 4)) {
                return RetCode_BOVFL;
            }
            *out = 0;
            memcpy(&((unsigned char *)out)[0], &buf_[pos_], 4);
            pos_ += 4;
            return RetCode_OK;
        }

    private:
        size_t capcty_;
        size_t pos_;
        size_t limit_;
        size_t mark_;
        unsigned char *buf_;
};

//-----------------------------
// grow_byte_buffer
grow_byte_buffer::grow_byte_buffer() : impl_(NULL) {}

grow_byte_buffer::~grow_byte_buffer()
{
    if(impl_) {
        delete impl_;
    }
}

RetCode grow_byte_buffer::init(size_t capcty)
{
    impl_ = new grow_byte_buffer_impl();
    return impl_->r_init(capcty);
}

RetCode grow_byte_buffer::grow(size_t amnt)
{
    return impl_->r_grow(amnt);
}

RetCode grow_byte_buffer::ensure_capacity(size_t capcty)
{
    return impl_->r_ens_cpcty(capcty);
}

void grow_byte_buffer::reset()
{
    impl_->r_reset();
}

void grow_byte_buffer::flip()
{
    impl_->r_flip();
}

RetCode grow_byte_buffer::append(const void *buf, size_t offst, size_t len)
{
    return impl_->r_apnd(buf, offst, len);
}

RetCode grow_byte_buffer::append_ushort(unsigned short val)
{
    return impl_->r_apnd_ushort(val);
}

RetCode grow_byte_buffer::append_uint(unsigned int val)
{
    return impl_->r_apnd_uint(val);
}

RetCode grow_byte_buffer::put(const void *buf, size_t pos, size_t len)
{
    return impl_->r_put(buf, pos, len);
}

size_t grow_byte_buffer::position() const
{
    return impl_->r_pos();
}

size_t grow_byte_buffer::limit() const
{
    return impl_->r_lim();
}

size_t grow_byte_buffer::mark() const
{
    return impl_->r_mrk();
}

size_t grow_byte_buffer::capacity() const
{
    return impl_->r_cpcty();
}

size_t grow_byte_buffer::remaining() const
{
    return impl_->r_rem();
}

unsigned char *grow_byte_buffer::buffer()
{
    return impl_->r_buf();
}

unsigned int *grow_byte_buffer::buffer_as_uint()
{
    return impl_->r_buf_as_uint();
}

char *grow_byte_buffer::buffer_as_char()
{
    return impl_->r_buf_as_char();
}

RetCode grow_byte_buffer::advance_pos_write(size_t amnt)
{
    return impl_->r_adv_pos_write(amnt);
}

RetCode grow_byte_buffer::set_pos_write(size_t newpos)
{
    return impl_->r_set_pos_write(newpos);
}

void grow_byte_buffer::move_pos_write(size_t amnt)
{
    impl_->r_mv_pos_write(amnt);
}

RetCode grow_byte_buffer::advance_pos_read(size_t amnt)
{
    return impl_->r_adv_pos_read(amnt);
}

RetCode grow_byte_buffer::set_pos_read(size_t newpos)
{
    return impl_->r_set_pos_read(newpos);
}

void grow_byte_buffer::set_mark()
{
    impl_->r_set_mrk();
}

RetCode grow_byte_buffer::set_mark(size_t newmrk)
{
    return impl_->r_set_mrk(newmrk);
}

size_t grow_byte_buffer::from_mark() const
{
    return impl_->r_frm_mrk();
}

size_t grow_byte_buffer::available_read()
{
    return impl_->r_avl_read();
}

RetCode grow_byte_buffer::read(size_t len, void *out)
{
    return impl_->r_read(len, out);
}

RetCode grow_byte_buffer::read_ushort(unsigned short *out)
{
    return impl_->r_read_ushort(out);
}

RetCode grow_byte_buffer::read_uint(unsigned int *out)
{
    return impl_->r_read_uint(out);
}

RetCode grow_byte_buffer::read_uint_to_sizet(size_t *out)
{
    return impl_->r_read_uint_to_sizet(out);
}

}
