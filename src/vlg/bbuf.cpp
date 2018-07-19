/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "glob.h"

#define GBB_DF_CPTY 32

namespace vlg {

// grow_byte_buffer

g_bbuf::g_bbuf() :
    capcty_(GBB_DF_CPTY),
    pos_(0),
    limit_(0),
    mark_(0),
    buf_(0)
{
    CMD_ON_NUL(buf_ = (char *)malloc(capcty_), exit(1))
}

g_bbuf::g_bbuf(size_t initial_capacity) :
    capcty_(initial_capacity),
    pos_(0),
    limit_(0),
    mark_(0),
    buf_(0)
{
    CMD_ON_NUL(buf_ = (char *)malloc(capcty_), exit(1))
}

g_bbuf::g_bbuf(const g_bbuf &oth) :
    capcty_(oth.capcty_),
    pos_(oth.pos_),
    limit_(oth.limit_),
    mark_(oth.mark_),
    buf_(0)
{
    CMD_ON_NUL(buf_ = (char *)malloc(capcty_), exit(1))
    memcpy(buf_, oth.buf_, capcty_);
}

g_bbuf::g_bbuf(g_bbuf &&oth) :
    capcty_(oth.capcty_),
    pos_(oth.pos_),
    limit_(oth.limit_),
    mark_(oth.mark_),
    buf_(oth.buf_)
{
    oth.capcty_ = 0;
    oth.pos_ = 0;
    oth.limit_ = 0;
    oth.mark_ = 0;
    oth.buf_ = nullptr;
}

g_bbuf::~g_bbuf()
{
    if(buf_) {
        free(buf_);
    }
}

RetCode g_bbuf::grow(size_t amnt)
{
    if(!amnt) {
        return RetCode_BADARG;
    }
    capcty_ += amnt;
    CMD_ON_NUL(buf_ = (char *)realloc(buf_, capcty_), exit(1))
    return RetCode_OK;
}

RetCode g_bbuf::ensure_capacity(size_t capcty)
{
    if(!capcty) {
        return RetCode_BADARG;
    }
    if(capcty_ < capcty) {
        return grow(capcty - capcty_);
    } else {
        return RetCode_OK;
    }
}

RetCode g_bbuf::append(const void *buf, size_t offst, size_t len)
{
    if(!len) {
        return RetCode_BADARG;
    }
    if((capcty_ - pos_) < len) {
        grow(len*2);
    }
    memcpy(&buf_[pos_], &((unsigned char *)buf)[offst], len);
    pos_ += len;
    limit_ = pos_;
    return RetCode_OK;
}

RetCode g_bbuf::append(g_bbuf &oth)
{
    if(!oth.limit_) {
        return RetCode_BADARG;
    }
    if((capcty_ - pos_) < (oth.limit_ - oth.pos_)) {
        grow((oth.limit_ - oth.pos_)*2);
    }
    memcpy(&buf_[pos_], &((unsigned char *)oth.buf_)[oth.pos_], oth.limit_);
    pos_ += oth.limit_;
    limit_ = pos_;
    oth.pos_ = oth.limit_;
    return RetCode_OK;
}

RetCode g_bbuf::append_no_rsz(g_bbuf &oth)
{
    if(!oth.limit_) {
        return RetCode_BADARG;
    }
    if((capcty_ - pos_) < (oth.limit_ - oth.pos_)) {
        return RetCode_BOVFL;
    }
    memcpy(&buf_[pos_], &((unsigned char *)oth.buf_)[oth.pos_], oth.limit_);
    pos_ += oth.limit_;
    limit_ = pos_;
    oth.pos_ = oth.limit_;
    return RetCode_OK;
}

RetCode g_bbuf::append_ushort(unsigned short val)
{
    if((capcty_ - pos_) < 2) {
        grow(2*4);
    }
    memcpy(&buf_[pos_], &val, 2);
    pos_ += 2;
    limit_ = pos_;
    return RetCode_OK;
}

RetCode g_bbuf::append_uint(unsigned int val)
{
    if((capcty_ - pos_) < 4) {
        grow(4*4);
    }
    memcpy(&buf_[pos_], &val, 4);
    pos_ += 4;
    limit_ = pos_;
    return RetCode_OK;
}

RetCode g_bbuf::put(const void *buf, size_t pos, size_t len)
{
    if(limit_ < pos) {
        return RetCode_BADARG;
    }
    memcpy(&buf_[pos], &((unsigned char *)buf)[0], len);
    return RetCode_OK;
}

RetCode g_bbuf::advance_pos_write(size_t amnt)
{
    if((capcty_ - pos_) < amnt) {
        grow(amnt*2);
    }
    pos_ += amnt;
    limit_ = pos_;
    return RetCode_OK;
}

RetCode g_bbuf::set_pos_write(size_t newpos)
{
    if(capcty_ < newpos) {
        grow((newpos - capcty_)*2);
    }
    pos_ = limit_ = newpos;
    return RetCode_OK;
}

RetCode g_bbuf::advance_pos_read(size_t amnt)
{
    if((limit_ - pos_) < amnt) {
        return RetCode_BOVFL;
    }
    pos_ += amnt;
    return RetCode_OK;
}

RetCode g_bbuf::set_pos_read(size_t newpos)
{
    if(limit_ < newpos) {
        return RetCode_BOVFL;
    }
    pos_ = newpos;
    return RetCode_OK;
}

RetCode g_bbuf::set_mark(size_t newmrk)
{
    if(limit_ < newmrk) {
        return RetCode_BADARG;
    }
    mark_ = newmrk;
    return RetCode_OK;
}

RetCode g_bbuf::read(size_t len, void *out)
{
    if(limit_ < (pos_ + len)) {
        return RetCode_BOVFL;
    }
    memcpy(&((unsigned char *)out)[0], &buf_[pos_], len);
    pos_ += len;
    return RetCode_OK;
}

RetCode g_bbuf::read_ushort(unsigned short *out)
{
    if(limit_ < (pos_ + 2)) {
        return RetCode_BOVFL;
    }
    memcpy(&((unsigned char *)out)[0], &buf_[pos_], 2);
    pos_ += 2;
    return RetCode_OK;
}

RetCode g_bbuf::read_uint(unsigned int *out)
{
    if(limit_ < (pos_ + 4)) {
        return RetCode_BOVFL;
    }
    memcpy(&((unsigned char *)out)[0], &buf_[pos_], 4);
    pos_ += 4;
    return RetCode_OK;
}

RetCode g_bbuf::read_uint_to_sizet(size_t *out)
{
    if(limit_ < (pos_ + 4)) {
        return RetCode_BOVFL;
    }
    *out = 0;
    memcpy(&((unsigned char *)out)[0], &buf_[pos_], 4);
    pos_ += 4;
    return RetCode_OK;
}

size_t g_bbuf::position() const
{
    return pos_;
}

}
