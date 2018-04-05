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

#include "vlg_c_model.h"
#include "vlg_c_persistence.h"
#include "vlg_c_peer.h"
#include "vlg_c_connection.h"
#include "vlg_c_transaction.h"
#include "vlg_c_subscription.h"
#include "vlg_drv_sqlite.h"
#include "vlg_model_sample.h"

const char *cpeer_name_handler(peer *p, void *ud){
    return "c_peer[" __DATE__"]";
}

const unsigned int *cpeer_version_handler(peer *p, void *ud){
    static unsigned int peer_c_ver_[4] = {0,0,0,0};
    return peer_c_ver_;
}

// MAIN
int main(int argc, char *argv[])
{
    nentity_manager *smplm = get_c_nem_smplmdl();
    persistence_driver *pd = get_pers_driv_sqlite();
    own_peer *op = peer_create();
    peer *p = own_peer_get_ptr(op);

    peer_set_name_handler(p, cpeer_name_handler, NULL);
    peer_set_version_handler(p, cpeer_version_handler, NULL);

    peer_extend_model_with_nem(p, smplm);
    persistence_manager_load_persistence_driver(&pd, 1);

    peer_start(p, argc, argv, 1);

    int i = 0;
    scanf("%d", &i);

    peer_destroy(op);
    return 0;
}
