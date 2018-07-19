/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 * 
 */

#include <stdio.h>

#include "vlg_c_model.h"
#include "vlg_c_persistence.h"
#include "vlg_c_peer.h"
#include "vlg_c_connection.h"
#include "vlg_c_transaction.h"
#include "vlg_c_subscription.h"
#include "vlg_drv_sqlite.h"
#include "vlg_model_sample.h"

const char *cpeer_name_handler(peer *p, void *usr_data){
    return "c_peer[" __DATE__"]";
}

const unsigned int *cpeer_version_handler(peer *p, void *usr_data){
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

	{
		own_nclass *own_usr = NULL;
		RetCode res = nentity_manager_new_nclass_instance(smplm, 800, &own_usr);
		if (!res) {
			nclass *usr = own_nclass_get_ptr(own_usr);
			nclass_set_field_by_name(usr, "name", "test", 4);
			printf("name: %s", (const char*)nclass_get_field_by_name(usr, "name"));
		}
		own_nclass_release(own_usr);
	}

    int i = 0;
    scanf("%d", &i);

    peer_destroy(op);
    return 0;
}
