#!/bin/bash
export LC_LANG=en_GB
export OUT_ARCH_DIR=OSX_64
export OUT_TRGT_DIR=REL
export CC_DBG_REL_OPT=-O2
export SOLIB_EXT="dylib"
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
