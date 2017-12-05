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

#include "vlg_globint.h"
#include "cr_ascii_string.h"

namespace vlg {

#define CR_BUF_SIZE_16      16
#define CR_BUF_SIZE_32      32
#define CR_BUF_SIZE_64      64
#define CR_BUF_SIZE_128     128
#define CR_BUF_SIZE_256     256
#define CR_BUF_SIZE_512     512
#define CR_BUF_SIZE_1024    1024
#define CR_BUF_SIZE_2048    2048

#define CPY_CONSTR_ERR  "ILLGL CPY CONSTR [%s]. EXITING..\n"

//32 MB
#define STR_MAX_SIZE 33554432


// ascii_str_impl

class ascii_string_impl {
    public:
        ascii_string_impl() :
            buf_(nullptr),
            len_(0),
            capacity_(0) {
        }

        ascii_string_impl(ascii_string_impl &oth) :
            buf_(nullptr),
            len_(0),
            capacity_(0) {
            EXIT_ACTION
        }

        ~ascii_string_impl() {
            if(buf_) {
                free(buf_);
            }
        }

        char char_at(size_t index) const {
            if(index >= len_) {
                return -1;
            }
            return buf_[index];;
        }

        RetCode clear() {
            len_ = 0;
            capacity_ = CR_BUF_SIZE_128;
            RETURN_IF_NOT_OK(set_buff_size(capacity_))
            buf_[len_] = '\0';
            return RetCode_OK;
        }

        int compare(const ascii_string_impl &str_rep) const {
            return strcmp(buf_, str_rep.buf_);
        }

        int compare(const char *str) const {
            return strcmp(buf_, str);
        }

        bool equals(const ascii_string_impl &str_rep) const {
            if(len_ != str_rep.len_) {
                return false;
            }
            return !strcmp(buf_, str_rep.buf_);
        }

        bool equals(const char *str) const {
            return !strcmp(buf_, str);
        }

        long index_of(char c) const {
            size_t i = 0;
            while(i < len_) {
                if(buf_[i] == c) {
                    return (long)i;
                }
                i++;
            }
            return -1;
        }

        long index_of(const char *str) const {
            size_t str_len = strlen(str), i = 0;
            while(i < len_ && str_len <= len_ - i) {
                if(!strncmp(&buf_[i], &str[0], str_len)) {
                    return (long)i;
                }
                i++;
            }
            return -1;
        }

        long index_of(const ascii_string_impl &str_rep) const {
            size_t i = 0;
            while(i < len_ && str_rep.len_ <= len_ - i) {
                if(!strncmp(&buf_[i], &str_rep.buf_[0], str_rep.len_)) {
                    return (long)i;
                }
                i++;
            }
            return -1;
        }

        long index_of(size_t start, char c) const {
            size_t i = start;
            while(i < len_) {
                if(buf_[i] == c) {
                    return (long)i;
                }
                i++;
            }
            return -1;
        }

        long index_of(size_t start, const char *str) const {
            size_t i = start, str_length = strlen(str);
            while(i < len_ && str_length <= len_ - i) {
                if(!strncmp(&buf_[i], &str[0], str_length)) {
                    return (long)i;
                }
                i++;
            }
            return -1;
        }

        long index_of(size_t start, const ascii_string_impl &str_rep) const {
            size_t i = start;
            while(i < len_ && str_rep.len_ <= len_ - i) {
                if(!strncmp(&buf_[i], &str_rep.buf_[0], str_rep.len_)) {
                    return (long)i;
                }
                i++;
            }
            return -1;
        }

        size_t length() const {
            return len_;
        }

        const char *c_buff() const {
            return &buf_[0];
        }

        char *c_buff_m() const {
            return &buf_[0];
        }

        void to_buff(char *copy) const {
            strncpy(&copy[0], &buf_[0], capacity_-1); //OK
            copy[capacity_-1] = '\0';
        }

        char *new_buff() const {
            char *new_str = (char *)malloc(len_+1);
            if(!new_str) {
                return nullptr;
            }
            strncpy(&new_str[0], &buf_[0], len_); //OK
            new_str[len_] = '\0';
            return new_str;
        }

        RetCode assign(const char *str) {
            size_t str_len = strlen(str);
            if(capacity_ < str_len+1) {
                capacity_ = str_len+1;
                RETURN_IF_NOT_OK(set_buff_size(capacity_))
            }
            strncpy(&buf_[0], &str[0], capacity_-1); //OK
            buf_[capacity_-1] = '\0';
            len_ = str_len;
            return RetCode_OK;
        }

        RetCode assign(const ascii_string_impl &str_rep) {
            if(capacity_ < str_rep.len_+1) {
                capacity_ = str_rep.len_+1;
                RETURN_IF_NOT_OK(set_buff_size(capacity_))
            }
            strncpy(&buf_[0], &str_rep.buf_[0], capacity_-1); //OK
            buf_[capacity_-1] = '\0';
            len_ = str_rep.len_;
            return RetCode_OK;
        }

        RetCode set_buff_size(size_t val) {
            if(val > STR_MAX_SIZE) {
                return RetCode_OVRSZ;
            }
            void *buf = nullptr;
            if(buf_) {
                if(!(buf = realloc(buf_, val))) {
                    return RetCode_MEMERR;
                }
            } else {
                if(!(buf = malloc(val))) {
                    return RetCode_MEMERR;
                }
            }
            buf_ = (char *)buf;
            return RetCode_OK;
        }

        RetCode append(char c) {
            size_t new_len = len_ + 1;
            if(capacity_ < new_len+1) {
                capacity_ = new_len+1;
                RETURN_IF_NOT_OK(set_buff_size(capacity_))
            }
            buf_[len_] = c;
            buf_[new_len] = '\0';
            len_ = new_len;
            return RetCode_OK;
        }

        RetCode append(const char *str) {
            size_t str_len = strlen(str);
            size_t new_len = len_ + str_len;
            if(capacity_ < new_len+1) {
                capacity_ = new_len+1;
                RETURN_IF_NOT_OK(set_buff_size(capacity_))
            }
            strncpy(&buf_[len_], &str[0], str_len); //ok..
            buf_[capacity_-1] = '\0';
            len_ = new_len;
            return RetCode_OK;
        }

        RetCode append(const ascii_string_impl &str_rep) {
            size_t new_len = len_ + str_rep.len_;
            if(capacity_ < new_len+1) {
                capacity_ = new_len+1;
                RETURN_IF_NOT_OK(set_buff_size(capacity_))
            }
            strncpy(&buf_[len_], str_rep.buf_, str_rep.len_); //ok..
            buf_[capacity_-1] = '\0';
            len_ = new_len;
            return RetCode_OK;
        }

        RetCode replace(size_t start, size_t end, const ascii_string_impl &str_rep) {
            if(start > len_ || start > end) {
                return RetCode_BADARG;
            }
            if(end < len_) {
                size_t new_length = start + str_rep.len_ + (len_ - end);
                if(capacity_ < new_length+1) {
                    capacity_ = new_length+1;
                    RETURN_IF_NOT_OK(set_buff_size(capacity_))
                }
                size_t offset = str_rep.len_ - (end - start);
                memmove(&buf_[end + offset], &buf_[end], len_ - end);
                len_ = new_length;
            } else {
                len_ = start + str_rep.len_;
                if(capacity_ < len_+1) {
                    capacity_ = len_+1;
                    RETURN_IF_NOT_OK(set_buff_size(capacity_))
                }
            }
            memcpy(&buf_[start], &str_rep.buf_[0], str_rep.len_);
            buf_[len_] = '\0';
            return RetCode_OK;
        }

        RetCode replace(size_t start, size_t end, const char *str) {
            if(start > len_ || start > end) {
                return RetCode_BADARG;
            }
            size_t str_len = strlen(str);
            if(end < len_) {
                size_t new_length = start + str_len + (len_ - end);
                if(capacity_ < new_length+1) {
                    capacity_ = new_length+1;
                    RETURN_IF_NOT_OK(set_buff_size(capacity_))
                }
                size_t offset = str_len - (end - start);
                memmove(&buf_[end + offset], &buf_[end], len_ - end);
                len_ = new_length;
            } else {
                len_ = start + str_len;
                if(capacity_ < len_+1) {
                    capacity_ = len_+1;
                    RETURN_IF_NOT_OK(set_buff_size(capacity_))
                }
            }
            memcpy(&buf_[start], &str[0], str_len);
            buf_[len_] = '\0';
            return RetCode_OK;
        }

        RetCode substring(size_t start, size_t end, ascii_string_impl &str_rep) const {
            if(start >= len_ || end > len_ || start > end) {
                return RetCode_BADARG;
            }
            str_rep.len_ = end - start;
            if(str_rep.capacity_ < str_rep.len_+1) {
                str_rep.capacity_ = str_rep.len_+1;
                RETURN_IF_NOT_OK(str_rep.set_buff_size(str_rep.capacity_))
            }
            memcpy(&str_rep.buf_[0], &buf_[start], str_rep.len_);
            str_rep.buf_[str_rep.len_] = '\0';
            return RetCode_OK;
        }

        RetCode substring(size_t start, size_t end, char *out) const {
            if(start >= len_ || end > len_ || start > end) {
                return RetCode_BADARG;
            }
            size_t len = end - start;
            memcpy(&out[0], &buf_[start], len);
            out[len] = '\0';
            return RetCode_OK;
        }

        RetCode substring(size_t beginidx, ascii_string_impl &str_rep) const {
            if(beginidx >= len_) {
                str_rep.assign("");
                return RetCode_OK;
            }
            str_rep.len_ = len_ - beginidx;
            if(str_rep.capacity_ < str_rep.len_+1) {
                str_rep.capacity_ = str_rep.len_+1;
                RETURN_IF_NOT_OK(str_rep.set_buff_size(str_rep.capacity_))
            }
            memcpy(&str_rep.buf_[0], &buf_[beginidx], str_rep.len_);
            str_rep.buf_[str_rep.len_] = '\0';
            return RetCode_OK;
        }

        RetCode substring(size_t beginidx, char *out) const {
            if(beginidx >= len_) {
                strncpy(out, "", capacity_-1);
                return RetCode_OK;
            }
            size_t len = len_ - beginidx;
            memcpy(&out[0], &buf_[beginidx], len);
            out[len] = '\0';
            return RetCode_OK;
        }

        void uppercase() {
            char *p = &buf_[0];
            while((*p=toupper(*p))) {
                p++;
            }
        }

        void first_char_uprc() {
            buf_[0] = toupper(buf_[0]);
        }

        void lowercase() {
            char *p = &buf_[0];
            while((*p=tolower(*p))) {
                p++;
            }
        }

        void trim() {
            size_t idx = 0;
            while(idx <= len_ && buf_[idx] == ' ') {
                idx++;
            }
            if(idx == len_) {
                len_ = 0;
            } else {
                len_ -= idx;
                memmove(&buf_[0], &buf_[idx], len_);
                while(buf_[len_-1] == ' ') {
                    len_--;
                }
            }
            buf_[len_] = '\0';
        }

    private:
        char *buf_;
        // up to 33554431
        size_t len_;
        //up to 33554432 bytes / 32 MB
        size_t capacity_;
};


// ascii_str

ascii_string::ascii_string() : impl_(nullptr)
{
    if(!(impl_ = new ascii_string_impl())) {
        EXIT_ACTION
    }
}

ascii_string::ascii_string(ascii_string &oth) : impl_(nullptr)
{
    EXIT_ACTION
}

ascii_string::~ascii_string()
{
    if(impl_) {
        delete impl_;
    }
}

char ascii_string::char_at(size_t index) const
{
    return impl_->char_at(index);
}

RetCode ascii_string::clear()
{
    return impl_->clear();
}

int ascii_string::compare(const ascii_string &str) const
{
    return impl_->compare(*str.impl_);
}

int ascii_string::compare(const char *str) const
{
    return impl_->compare(str);
}

bool ascii_string::equals(const ascii_string &str) const
{
    return impl_->equals(*str.impl_);
}

bool ascii_string::equals(const char *str) const
{
    return impl_->equals(str);
}

long ascii_string::index_of(char c) const
{
    return impl_->index_of(c);
}

long ascii_string::index_of(const char *str) const
{
    return impl_->index_of(str);
}

long ascii_string::index_of(const ascii_string &str) const
{
    return impl_->index_of(*str.impl_);
}

long ascii_string::index_of(size_t start, char c) const
{
    return impl_->index_of(start, c);
}

long ascii_string::index_of(size_t start, const char *str) const
{
    return impl_->index_of(start, str);
}

long ascii_string::index_of(size_t start, const ascii_string &str) const
{
    return impl_->index_of(start, *str.impl_);
}

size_t ascii_string::length() const
{
    return impl_->length();
}

const char *ascii_string::internal_buff() const
{
    return impl_->c_buff();
}

char *ascii_string::internal_buff_m() const
{
    return impl_->c_buff_m();
}

void ascii_string::to_buffer(char *copy) const
{
    impl_->to_buff(copy);
}

char *ascii_string::new_buffer() const
{
    return impl_->new_buff();
}

RetCode ascii_string::assign(const char *str)
{
    return impl_->assign(str);
}

RetCode ascii_string::assign(const ascii_string &str)
{
    return impl_->assign(*str.impl_);
}

RetCode ascii_string::append(char c)
{
    return impl_->append(c);
}

RetCode ascii_string::append(const char *str)
{
    return impl_->append(str);
}

RetCode ascii_string::append(const ascii_string &str)
{
    return impl_->append(*str.impl_);
}

RetCode ascii_string::replace(size_t start, size_t end, const ascii_string &str)
{
    return impl_->replace(start, end, *str.impl_);
}

RetCode ascii_string::replace(size_t start, size_t end, const char *str)
{
    return impl_->replace(start, end, str);
}

RetCode ascii_string::substring(size_t start, size_t end,
                                ascii_string &out) const
{
    return impl_->substring(start, end, *out.impl_);
}

RetCode ascii_string::substring(size_t start, size_t end, char *out) const
{
    return impl_->substring(start, end, out);
}

RetCode ascii_string::substring(size_t beginidx, ascii_string &out) const
{
    return impl_->substring(beginidx, *out.impl_);
}

RetCode ascii_string::substring(size_t beginidx, char *out) const
{
    return impl_->substring(beginidx, out);
}

void ascii_string::uppercase()
{
    impl_->uppercase();
}

void ascii_string::first_char_uppercase()
{
    impl_->first_char_uprc();
}

void ascii_string::lowercase()
{
    impl_->lowercase();
}

void ascii_string::trim()
{
    impl_->trim();
}

bool ascii_string::operator ==(const char *str) const
{
    return impl_->equals(str);
}

bool ascii_string::operator ==(const ascii_string &str) const
{
    return impl_->equals(*str.impl_);
}

bool ascii_string::operator !=(const char *str) const
{
    return !impl_->equals(str);
}

bool ascii_string::operator !=(const ascii_string &str) const
{
    return !impl_->equals(*str.impl_);
}

bool ascii_string::operator >(const char *str) const
{
    return impl_->compare(str) > 0;
}

bool ascii_string::operator <(const char *str) const
{
    return impl_->compare(str) < 0;
}

bool ascii_string::operator >(const ascii_string &str) const
{
    return impl_->compare(*str.impl_) > 0;
}

bool ascii_string::operator <(const ascii_string &str) const
{
    return impl_->compare(*str.impl_) < 0;
}


// ascii_str_tk_impl

static const char *def_delims = " \t\n\r\f";

class ascii_string_tok_impl {
    public:
        ascii_string_tok_impl() :
            current_position_(0),
            max_position_(0),
            new_position_(-1),
            str_(),
            delimiters_(),
            ret_delims_(false),
            delims_changed_(false) {
        }

        ascii_string_tok_impl(ascii_string_tok_impl &oth) :
            current_position_(0),
            max_position_(0),
            new_position_(-1),
            str_(),
            delimiters_(),
            ret_delims_(false),
            delims_changed_(false) {
            EXIT_ACTION
        }

        ~ascii_string_tok_impl() {}

        RetCode next_token(ascii_string &out,
                           const char *delimiters,
                           bool return_delimiters) {
            if(delimiters) {
                RETURN_IF_NOT_OK(delimiters_.assign(delimiters))
                delims_changed_ = true;
            }
            ret_delims_ = return_delimiters;
            current_position_ = (new_position_ >= 0 && !delims_changed_) ?
                                new_position_ :
                                skip_delimit(current_position_);
            delims_changed_ = false;
            new_position_ = -1;
            if(current_position_ >= max_position_) {
                return RetCode_KO;
            }
            long start = current_position_;
            current_position_ = scan_token(current_position_);
            RETURN_IF_NOT_OK(str_.substring(start, current_position_, out))
            return RetCode_OK;
        }

        RetCode next_token(char *out,
                           const char *delimiters,
                           bool return_delimiters) {
            if(delimiters) {
                RETURN_IF_NOT_OK(delimiters_.assign(delimiters))
                delims_changed_ = true;
            }
            ret_delims_ = return_delimiters;
            current_position_ = (new_position_ >= 0 && !delims_changed_) ?
                                new_position_ :
                                skip_delimit(current_position_);
            delims_changed_ = false;
            new_position_ = -1;
            if(current_position_ >= max_position_) {
                return RetCode_KO;
            }
            long start = current_position_;
            current_position_ = scan_token(current_position_);
            RETURN_IF_NOT_OK(str_.substring(start, current_position_, out))
            return RetCode_OK;
        }

        bool has_more_tokens(bool return_delimiters) {
            ret_delims_ = return_delimiters;
            new_position_ = skip_delimit(current_position_);
            return (new_position_ < max_position_);
        }

        void reset() {
            current_position_ = 0;
        }

        RetCode init(const ascii_string &str) {
            RETURN_IF_NOT_OK(str_.assign(str))
            RETURN_IF_NOT_OK(delimiters_.assign(def_delims))
            ret_delims_ = false;
            current_position_ = 0;
            max_position_ = (long)str_.length();
            return RetCode_OK;
        }

        RetCode init(const char *str) {
            RETURN_IF_NOT_OK(str_.assign(str))
            RETURN_IF_NOT_OK(delimiters_.assign(def_delims))
            ret_delims_ = false;
            current_position_ = 0;
            max_position_ = (long)str_.length();
            return RetCode_OK;
        }

        long skip_delimit(long start_pos) {
            long position = start_pos;
            while(!ret_delims_ && position < max_position_) {
                if(delimiters_.index_of(str_.char_at(position)) < 0) {
                    break;
                }
                position++;
            }
            return position;
        }

        long scan_token(long start_pos) {
            long position = start_pos;
            while(position < max_position_) {
                if(delimiters_.index_of(str_.char_at(position)) >= 0) {
                    break;
                }
                position++;
            }
            if(ret_delims_ && (start_pos == position)) {
                if(delimiters_.index_of(str_.char_at(position)) >= 0) {
                    position++;
                }
            }
            return position;
        }

        //--rep
    private:
        long    current_position_,
                max_position_,
                new_position_;

        ascii_string str_;
        ascii_string delimiters_;
        bool ret_delims_, delims_changed_;
};


// ascii_str_tk

ascii_string_tok::ascii_string_tok() : impl_(nullptr)
{}

ascii_string_tok::ascii_string_tok(ascii_string_tok &oth) : impl_(nullptr)
{
    EXIT_ACTION
}

ascii_string_tok::~ascii_string_tok()
{
    if(impl_) {
        delete impl_;
    }
}

RetCode ascii_string_tok::next_token(ascii_string &out,
                                     const char *delimiters,
                                     bool return_delimiters)
{
    return impl_->next_token(out, delimiters, return_delimiters);
}

RetCode ascii_string_tok::next_token(char *out,
                                     const char *delimiters,
                                     bool return_delimiters)
{
    return impl_->next_token(out, delimiters, return_delimiters);
}

bool ascii_string_tok::has_more_tokens(bool return_delimiters)
{
    return impl_->has_more_tokens(return_delimiters);
}

void ascii_string_tok::reset()
{
    impl_->reset();
}

RetCode ascii_string_tok::init(const ascii_string &str)
{
    if(!impl_) {
        if(!(impl_ = new ascii_string_tok_impl())) {
            return RetCode_MEMERR;
        }
    }
    return impl_->init(str);
}

RetCode ascii_string_tok::init(const char *str)
{
    if(!impl_) {
        if(!(impl_ = new ascii_string_tok_impl())) {
            return RetCode_MEMERR;
        }
    }
    return impl_->init(str);
}

}
