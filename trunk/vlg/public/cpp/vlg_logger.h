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

#ifndef VLG_LOG_H_
#define VLG_LOG_H_
#include "vlg.h"

namespace vlg {

/** @brief class appender.
*/
class appender_impl;
class appender {
    public:

        explicit        appender();
        explicit        appender(const char *fname);
        explicit        appender(FILE *fd);
        virtual         ~appender();

    public:
        virtual void    flush();

    public:
        virtual size_t  put_msg(TraceLVL lvl,
                                const char *sign,
                                uint16_t sign_len,
                                uint32_t id,
                                const char *msg);

        virtual size_t  put_msg_plain(const char *msg);

        virtual size_t  put_msg_va(TraceLVL lvl,
                                   const char *sign,
                                   uint16_t sign_len,
                                   uint32_t id,
                                   const char *msg,
                                   va_list args);

        virtual size_t  put_msg_va_plain(const char *msg,
                                         va_list args);

    protected:
        virtual size_t  render_msg_pln(const char *msg,
                                       char *buff,
                                       uint16_t *buff_len);

        virtual size_t  render_msg(const char *lvl_str,
                                   const char *sign,
                                   uint16_t sign_len,
                                   uint32_t id,
                                   const char *msg,
                                   char *buff,
                                   uint16_t *buff_len);

        virtual size_t  render_msg_va_pln(const char *msg,
                                          char *buff,
                                          uint16_t *buff_len,
                                          va_list args);

        virtual size_t  render_msg_va(const char *lvl_str,
                                      const char *sign,
                                      uint16_t sign_len,
                                      uint32_t id,
                                      const char *msg,
                                      char *buff,
                                      uint16_t *buff_len,
                                      va_list args);
    private:
        appender_impl *impl_;
};

/** @brief class logger.
*/
class logger_impl;
class logger {

    public:
        static RetCode      set_logger_cfg_file_dir(const char *dir);
        static RetCode      set_logger_cfg_file_path_name(const char *file_path);
        static RetCode      load_logger_config();
        static RetCode      load_logger_config(const char *fname);
        static RetCode      get_logger(const char *logger_name,
                                       logger **logger);

        static logger       *get_logger(const char *logger_name);
        static RetCode      add_appender_to_all_loggers(appender *apnd);
        static RetCode      remove_last_appender_from_all_loggers();
        static void         set_level_for_all_loggers(TraceLVL lvl);

    public:
        explicit logger();
        virtual ~logger();

    public:
        void        set_rep_from(logger &other);

        RetCode     init(TraceLVL level,
                         const char *sign,
                         appender *apnds[],
                         uint16_t apnds_n);

    public:
        TraceLVL    level();
        logger_impl *get_opaque();

        appender    **get_appenders();
        uint16_t    get_appender_count();

        void        set_level(TraceLVL level);
        RetCode     add_appender(appender *apnd);
        RetCode     remove_last_appender();

    public:
        size_t log(TraceLVL level, uint32_t id, const char *msg, ...);
        size_t pln(const char *msg, ...);

        size_t low(uint32_t id, const char *msg, ...);
        size_t trc(uint32_t id, const char *msg, ...);
        size_t dbg(uint32_t id, const char *msg, ...);
        size_t inf(uint32_t id, const char *msg, ...);
        size_t wrn(uint32_t id, const char *msg, ...);
        size_t err(uint32_t id, const char *msg, ...);
        size_t cri(uint32_t id, const char *msg, ...);
        size_t fat(uint32_t id, const char *msg, ...);

    protected:
        size_t plnm(const char *msg);
        size_t lowm(uint32_t id, const char *msg);
        size_t trcm(uint32_t id, const char *msg);
        size_t dbgm(uint32_t id, const char *msg);
        size_t infm(uint32_t id, const char *msg);
        size_t wrnm(uint32_t id, const char *msg);
        size_t errm(uint32_t id, const char *msg);
        size_t crim(uint32_t id, const char *msg);
        size_t fatm(uint32_t id, const char *msg);

    private:
        logger_impl *impl_;
};

}

namespace vlg {

/** @brief  nclass_logger is a convenient logger with specialized methods
            accepting nclass objects.
*/
class nclass_logger : public vlg::logger {
    public:
        //---ctors
        explicit nclass_logger();
        ~nclass_logger();

        //---pub meths
        size_t trc_class(uint32_t id,
                         const nclass *obj,
                         bool print_class_name,
                         const char *fmt,
                         ...);

        size_t dbg_class(uint32_t id,
                         const nclass *obj,
                         bool print_class_name,
                         const char *fmt,
                         ...);

        size_t inf_class(uint32_t id,
                         const nclass *obj,
                         bool print_class_name,
                         const char *fmt,
                         ...);

        size_t wrn_class(uint32_t id,
                         const nclass *obj,
                         bool print_class_name,
                         const char *fmt,
                         ...);

        size_t err_class(uint32_t id,
                         const nclass *obj,
                         bool print_class_name,
                         const char *fmt,
                         ...);

        size_t cri_class(uint32_t id,
                         const nclass *obj,
                         bool print_class_name,
                         const char *fmt,
                         ...);

        size_t fat_class(uint32_t id,
                         const nclass *obj,
                         bool print_class_name,
                         const char *fmt,
                         ...);
};

nclass_logger *get_nclass_logger(const char *logger_name);
}


#endif
