#!/bin/bash
export LC_LANG=en_GB
export OUT_ARCH_DIR=LNX_64
export OUT_TRGT_DIR=DBG
export CC_DBG_REL_OPT=-g
export LD_DBG_REL_OPT=-g
export LD_ARCH_LIB="-lrt -ldl"
export LDFLAGS_1_OPT=",-no-whole-archive,-error-unresolved-symbols"
export LDFLAGS_2_OPT=",-no-whole-archive"
echo "@@@@@@ begin DEBUG build @@@@@@"

if [ $1 == "vlg" ]; then 
make -f makefile_lib_vlg; 
make -f makefile_lib_apic;
make -f makefile_lib_apicpp;
make -f makefile_lib_sqlite;
make -f makefile_lib_drvsqlite;
make -f makefile_slib_drvsqlite;
make -f makefile_bin_vlgc;
elif [ $1 == "test" ]; then
make -f makefile_lib_testing_mdl;
make -f makefile_slib_testing_mdl;
make -f makefile_bin_broker;
make -f makefile_bin_broker_dyn;
fi

echo "@@@@@@ end DEBUG build @@@@@@"