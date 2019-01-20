/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#include "vlg_persistence.h"
#include "vlg/prs_impl.h"

namespace vlg {

// CLASS persistence_manager

RetCode persistence_manager::load_driver_dynamic(const char *drivers[],
                                                 int drivers_num)
{
    return persistence_manager_impl::get_instance().load_pers_driver_dyna(drivers,
                                                                          drivers_num);
}

RetCode persistence_manager::load_driver(persistence_driver *drivers[],
                                         int drivers_num)
{
    return persistence_manager_impl::get_instance().persistence_driver_load(drivers,
                                                                            drivers_num);
}

RetCode persistence_manager::set_config_file_dir(const char *dir)
{
    return persistence_manager_impl::get_instance().set_cfg_file_dir(dir);
}

RetCode persistence_manager::set_config_file_path_name(const char *file_path)
{
    return persistence_manager_impl::get_instance().set_cfg_file_path_name(file_path);
}

RetCode persistence_manager::load_config(const char *file_name)
{
    return persistence_manager_impl::get_instance().load_cfg(file_name);
}

RetCode persistence_manager::start_all_drivers()
{
    return persistence_manager_impl::get_instance().start_all_drivers();
}

persistence_driver *persistence_manager::available_driver(unsigned int nclass_id)
{
    return persistence_manager_impl::get_instance().available_driver(nclass_id);
}

// CLASS persistence_connection

persistence_connection::persistence_connection(persistence_driver &driver) :
    driver_(driver)
{}

persistence_connection::~persistence_connection()
{}

persistence_driver &persistence_connection::get_driver()
{
    return driver_;
}

RetCode persistence_connection::create_entity_schema(PersistenceAlteringMode mode,
                                                     const nentity_manager &nem,
                                                     unsigned int nclass_id)
{
    persistence_connection_impl *pc = driver_.available_connection(nclass_id);
    if(!pc) {
        return RetCode_UNVRSC;
    }
    return pc->create_entity_schema(mode, nem, nclass_id);
}

RetCode persistence_connection::create_entity_schema(PersistenceAlteringMode mode,
                                                     const nentity_manager &nem,
                                                     const nentity_desc &desc)
{
    persistence_connection_impl *pc = driver_.available_connection(desc.get_nclass_id());
    if(!pc) {
        return RetCode_UNVRSC;
    }
    return pc->create_entity_schema(mode, nem, desc);
}

RetCode persistence_connection::save_obj(const nentity_manager &nem,
                                         unsigned int ts_0,
                                         unsigned int ts_1,
                                         const nclass &in)
{
    persistence_connection_impl *pc = driver_.available_connection(in.get_id());
    if(!pc) {
        return RetCode_UNVRSC;
    }
    return pc->save_entity(nem, ts_0, ts_1, in);
}

RetCode persistence_connection::update_obj(unsigned short key,
                                           const nentity_manager &nem,
                                           unsigned int ts_0,
                                           unsigned int ts_1,
                                           const nclass &in)
{
    persistence_connection_impl *pc = driver_.available_connection(in.get_id());
    if(!pc) {
        return RetCode_UNVRSC;
    }
    return pc->update_entity(key, nem, ts_0, ts_1, in);
}

RetCode persistence_connection::save_or_update_obj(unsigned short key,
                                                   const nentity_manager &nem,
                                                   unsigned int ts_0,
                                                   unsigned int ts_1,
                                                   const nclass &in)
{
    persistence_connection_impl *pc = driver_.available_connection(in.get_id());
    if(!pc) {
        return RetCode_UNVRSC;
    }
    return pc->save_or_update_entity(key, nem, ts_0, ts_1, in);
}

RetCode persistence_connection::remove_obj(unsigned short key,
                                           const nentity_manager &nem,
                                           unsigned int ts_0,
                                           unsigned int ts_1,
                                           PersistenceDeletionMode mode,
                                           const nclass &in)
{
    persistence_connection_impl *pc = driver_.available_connection(in.get_id());
    if(!pc) {
        return RetCode_UNVRSC;
    }
    return pc->remove_entity(key, nem, ts_0, ts_1, mode, in);
}

RetCode persistence_connection::load_obj(unsigned short key,
                                         const nentity_manager &nem,
                                         unsigned int &ts0_out,
                                         unsigned int &ts1_out,
                                         nclass &in_out)
{
    persistence_connection_impl *pc = driver_.available_connection(in_out.get_id());
    if(!pc) {
        return RetCode_UNVRSC;
    }
    return pc->load_entity(key, nem, ts0_out, ts1_out, in_out);
}

RetCode persistence_connection::execute_statement(const char *stmt, unsigned int nclass_id)
{
    persistence_connection_impl *pc = driver_.available_connection(nclass_id);
    if(!pc) {
        return RetCode_UNVRSC;
    }
    return pc->execute_statement(stmt);
}

// CLASS persistence_query

persistence_query::persistence_query(const nentity_manager &nem) :
    nem_(nem)
{}

persistence_query::~persistence_query()
{}

RetCode persistence_query::execute(unsigned int nclass_id, const char *sql)
{
    persistence_driver *driv = persistence_manager_impl::get_instance().available_driver(nclass_id);
    if(driv) {
        persistence_connection_impl *pcon = driv->available_connection(nclass_id);
        if(pcon) {
            return pcon->execute_query(sql, nem_, impl_);
        } else {
            return RetCode_UNVRSC;
        }
    } else {
        return RetCode_UNVRSC;
    }
}

const nentity_manager &persistence_query::get_nentity_manager() const
{
    return nem_;
}

RetCode persistence_query::next_obj(unsigned int &ts0_out,
                                    unsigned int &ts1_out, nclass &out)
{
    return impl_->load_next_entity(ts0_out, ts1_out, out);
}

RetCode persistence_query::release()
{
    return impl_->release();
}

}
