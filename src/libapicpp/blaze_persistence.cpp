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

#include "blaze_persistence.h"
#include "blz_persistence_int.h"

namespace blaze {

//-----------------------------
// CLASS persistence_manager
//-----------------------------

blaze::RetCode persistence_manager::load_persistence_driver_dynamic(
    const char *drivers[],
    int drivers_num)
{
    return persistence_manager_int::get_instance().load_pers_driver_dyna(drivers,
                                                                         drivers_num);
}

blaze::RetCode persistence_manager::load_persistence_driver(
    persistence_driver_int
    *drivers[], int drivers_num)
{
    return persistence_manager_int::get_instance().load_pers_driver(drivers,
                                                                    drivers_num);
}

blaze::RetCode persistence_manager::set_persistence_config_file_dir(
    const char *dir)
{
    return persistence_manager_int::get_instance().set_cfg_file_dir(dir);
}

blaze::RetCode persistence_manager::set_persistence_config_file_path_name(
    const char *file_path)
{
    return persistence_manager_int::get_instance().set_cfg_file_path_name(
               file_path);
}

blaze::RetCode persistence_manager::load_persistence_config(
    const char *file_name)
{
    return persistence_manager_int::get_instance().load_cfg(file_name);
}

blaze::RetCode persistence_manager::start_all_persistence_drivers()
{
    return persistence_manager_int::get_instance().start_all_drivers();
}

persistence_driver_int *persistence_manager::available_driver(
    unsigned int nclass_id)
{
    return persistence_manager_int::get_instance().available_driver(nclass_id);
}

//-----------------------------
// CLASS persistence_connection_impl
//-----------------------------

class persistence_connection_impl {
    public:
        persistence_connection_impl() : driv_(NULL), conn_(NULL) {}
        ~persistence_connection_impl() {}

        persistence_driver_int *get_driver() const {
            return driv_;
        }

        void set_driver(persistence_driver_int &val) {
            driv_ = &val;
        }

        blaze::RetCode set_connection(unsigned int nclass_id) {
            conn_ = driv_->available_connection(nclass_id);
            return conn_ ? blaze::RetCode_OK : blaze::RetCode_UNVRSC;
        }

        persistence_connection_int *get_conn_int() const {
            return conn_;
        }

    private:
        persistence_driver_int *driv_;
        persistence_connection_int *conn_;
};

//-----------------------------
// CLASS persistence_connection
//-----------------------------

persistence_connection::persistence_connection()
{
    impl_ = new persistence_connection_impl();
}

persistence_connection::~persistence_connection()
{
    if(impl_) {
        delete impl_;
    }
}

blaze::RetCode persistence_connection::bind(unsigned int nclass_id,
                                            persistence_driver_int &driver)
{
    impl_->set_driver(driver);
    return impl_->set_connection(nclass_id);
}

unsigned int persistence_connection::get_id() const
{
    return impl_->get_conn_int()->get_id();
}

persistence_connection_int *persistence_connection::get_connection_internal()
{
    return impl_->get_conn_int();
}

persistence_driver_int *persistence_connection::get_driver()
{
    return impl_->get_driver();
}

PersistenceConnectionStatus persistence_connection::get_status() const
{
    return impl_->get_conn_int()->status();
}

blaze::RetCode persistence_connection::create_entity_schema(
    PersistenceAlteringMode
    mode,
    const entity_manager &em,
    unsigned int nclass_id)
{
    return impl_->get_conn_int()->create_entity_schema(mode, em, nclass_id);
}

blaze::RetCode persistence_connection::create_entity_schema(
    PersistenceAlteringMode
    mode,
    const entity_manager &em,
    const entity_desc &desc)
{
    return impl_->get_conn_int()->create_entity_schema(mode, em, desc);
}

blaze::RetCode persistence_connection::save_entity(const entity_manager &em,
                                                   unsigned int ts0,
                                                   unsigned int ts1,
                                                   const nclass &in_obj)
{
    return impl_->get_conn_int()->save_entity(em, ts0, ts1, in_obj);
}

blaze::RetCode persistence_connection::update_entity(unsigned short key,
                                                     const entity_manager &em,
                                                     unsigned int ts0,
                                                     unsigned int ts1,
                                                     const nclass &in_obj)
{
    return impl_->get_conn_int()->update_entity(key, em, ts0, ts1, in_obj);
}

blaze::RetCode persistence_connection::save_or_update_entity(unsigned short key,
                                                             const entity_manager &em,
                                                             unsigned int ts0,
                                                             unsigned int ts1,
                                                             const nclass &in_obj)
{
    return impl_->get_conn_int()->save_or_update_entity(key, em, ts0, ts1, in_obj);
}

blaze::RetCode persistence_connection::remove_entity(unsigned short key,
                                                     const entity_manager &em,
                                                     unsigned int ts0,
                                                     unsigned int ts1,
                                                     PersistenceDeletionMode mode,
                                                     const nclass &in_obj)
{
    return impl_->get_conn_int()->remove_entity(key, em, ts0, ts1, mode, in_obj);
}

blaze::RetCode persistence_connection::load_entity(unsigned short key,
                                                   const entity_manager &em,
                                                   unsigned int &ts0_out,
                                                   unsigned int &ts1_out,
                                                   nclass &in_out_obj)
{
    return impl_->get_conn_int()->load_entity(key, em, ts0_out, ts1_out,
                                              in_out_obj);
}

blaze::RetCode persistence_connection::execute_statement(const char *stmt)
{
    return impl_->get_conn_int()->execute_statement(stmt);
}

//-----------------------------
// CLASS persistence_query_impl
//-----------------------------
class persistence_query_impl {
    public:
        persistence_query_impl(const entity_manager &em) : em_(em),
            int_(NULL) {}
        ~persistence_query_impl() {
            if(int_) {
                persistence_connection_int &conn_int_ref = int_->get_connection();
                persistence_connection_int *conn_int_ptr = &conn_int_ref;
                conn_int_ptr->destroy_query(int_, true);
            }
        }

        persistence_query_int *get_query_int() const {
            return int_;
        }

        void set_query_int(persistence_query_int *val) {
            int_ = val;
        }

        blaze::RetCode request_new_query_int(unsigned int nclass_id, const char *sql) {
            persistence_driver_int *driv =
                persistence_manager_int::get_instance().available_driver(nclass_id);
            if(driv) {
                persistence_connection_int *pcon = driv->available_connection(nclass_id);
                if(pcon) {
                    return pcon->execute_query(sql, em_, &int_);
                } else {
                    return blaze::RetCode_UNVRSC;
                }
            } else {
                return blaze::RetCode_UNVRSC;
            }
        }

    private:
        const entity_manager &em_;
        persistence_query_int *int_;
};

//-----------------------------
// CLASS persistence_query
//-----------------------------

persistence_query::persistence_query(const entity_manager &em)
{
    impl_ = new persistence_query_impl(em);
}

persistence_query::~persistence_query()
{
    if(impl_) {
        delete impl_;
    }
}

blaze::RetCode persistence_query::bind(unsigned int nclass_id, const char *sql)
{
    return impl_->request_new_query_int(nclass_id, sql);
}

unsigned int persistence_query::get_id() const
{
    return impl_->get_query_int()->get_id();
}

PersistenceQueryStatus persistence_query::get_status() const
{
    return impl_->get_query_int()->status();
}

const entity_manager &persistence_query::get_entity_manager() const
{
    return impl_->get_query_int()->get_em();
}

blaze::RetCode persistence_query::next_entity(unsigned int &ts0_out,
                                              unsigned int &ts1_out, nclass &out_obj)
{
    return impl_->get_query_int()->load_next_entity(ts0_out, ts1_out, out_obj);
}

blaze::RetCode persistence_query::release()
{
    return impl_->get_query_int()->release();
}

}
