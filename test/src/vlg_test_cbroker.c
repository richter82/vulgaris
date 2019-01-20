/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 * 
 */

#include <stdio.h>

#include "vlg_c_model.h"
#include "vlg_c_persistence.h"
#include "vlg_c_broker.h"
#include "vlg_c_connection.h"
#include "vlg_c_transaction.h"
#include "vlg_c_subscription.h"
#include "vlg_sqlite.h"
#include "vlg_model_sample.h"

const char *cbroker_name(broker *p, void *ud, void *ud2){
    return "c_broker[" __DATE__"]";
}

const unsigned int *cbroker_version(broker *p, void *ud, void *ud2){
    static unsigned int broker_c_ver_[4] = {0,0,0,0};
    return broker_c_ver_;
}

// MAIN
int main(int argc, char *argv[])
{
    own_broker *op = broker_create();
    broker *p = own_broker_get_ptr(op);
	logger *own_log = syslog_get_retained("vlglog");
	nentity_manager *smplm = get_c_nem_smplmdl(own_log);

    broker_set_name(p, cbroker_name, NULL, NULL);
    broker_set_version(p, cbroker_version, NULL, NULL);

    broker_extend_model_with_nem(p, smplm);
    persistence_manager_load_persistence_driver(get_pers_driv_sqlite(own_log));

    broker_start(p, argc, argv, 1);

	{
		own_nclass *own_usr = NULL;
		RetCode res = nentity_manager_new_nclass_instance(smplm, 800, &own_usr);
		if (!res) {
			nclass *usr = own_nclass_get_ptr(own_usr);
			nclass_set_field_by_name(usr, "name", "test", 4);
			printf("name: %s", (const char*)nclass_get_field_addr_by_name(usr, "name"));
		}
		own_nclass_release(own_usr);
	}

    int i = 0;
    scanf("%d", &i);
    broker_destroy(op);
	syslog_release_retained(own_log);
    return 0;
}
