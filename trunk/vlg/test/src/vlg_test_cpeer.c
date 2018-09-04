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

const char *cpeer_name(peer *p, void *ud, void *ud2){
    return "c_peer[" __DATE__"]";
}

const unsigned int *cpeer_version(peer *p, void *ud, void *ud2){
    static unsigned int peer_c_ver_[4] = {0,0,0,0};
    return peer_c_ver_;
}

// MAIN
int main(int argc, char *argv[])
{
    nentity_manager *smplm = get_c_nem_smplmdl();
    own_peer *op = peer_create();
    peer *p = own_peer_get_ptr(op);

    peer_set_name(p, cpeer_name, NULL, NULL);
    peer_set_version(p, cpeer_version, NULL, NULL);

    peer_extend_model_with_nem(p, smplm);
    persistence_manager_load_persistence_driver(get_pers_driv_sqlite());

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
