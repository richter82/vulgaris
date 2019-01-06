#!/bin/bash
export LC_LANG=en_GB
export OUT_ARCH_DIR=LNX_64
export OUT_TRGT_DIR=REL
export CC_DBG_REL_OPT=-O2
export LD_ARCH_LIB="-lrt -ldl"
export LDFLAGS_1_OPT=",-no-whole-archive,-error-unresolved-symbols"
export LDFLAGS_2_OPT=",-no-whole-archive"
echo "@@@@@@ begin DEBUG build @@@@@@"
make -f makefile_lib_vlg all; 
make -f makefile_lib_sqlite all;
make -f makefile_lib_drvsqlite all;
make -f makefile_slib_drvsqlite all;
make -f makefile_lib_apic all;
make -f makefile_lib_apicpp all;
make -f makefile_bin_vlgc all;
make -f makefile_lib_testing_mdl all;
make -f makefile_slib_testing_mdl all;
make -f makefile_bin_broker all;
make -f makefile_bin_broker_dyn all;
echo "@@@@@@ end DEBUG build @@@@@@"