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

#ifndef BLZ_MEM_H_
#define BLZ_MEM_H_
#include "vlg.h"

namespace vlg {

/** @brief collectable interface.
*/
class collector;
class collectable {
    public:
        virtual ~collectable();
        virtual collector &get_collector() = 0;
};

/** @brief collector class.
Collector class for raw pointers.
*/
class collector_impl;
class collector {
    public:
        typedef void(*enum_coll_inst)(const collector &coll,
                                      const void *ptr,
                                      const void *ref_count,
                                      void *ud);

    public:
        explicit collector(const char *class_name);
        virtual ~collector();

    public:
        const char  *get_class_name()    const;
        void        enum_collected_instances(enum_coll_inst clbk,
                                             void *ud = NULL);

        bool        is_instance_collected(collectable *ptr);
        void        retain(collectable *ptr);
        RetCode     release(collectable *ptr);

    private:
        collector_impl *impl_;
};

}

#endif
