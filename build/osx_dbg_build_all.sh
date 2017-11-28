#!/bin/bash
export LC_LANG=en_GB
export OUT_ARCH_DIR=OSX_64
export OUT_TRGT_DIR=DBG
export CC_DBG_REL_OPT=-g
export LD_DBG_REL_OPT=-g
export SOLIB_EXT="dylib"
echo "@@@@@@ begin DEBUG build @@@@@@"
make -f makefile_lib_cr all;
make -f makefile_lib_vlg all; 
make -f makefile_lib_sqlite all;
make -f makefile_lib_drvsqlite all;
make -f makefile_slib_drvsqlite all;
make -f makefile_lib_apic all;
make -f makefile_lib_apicpp all;
make -f makefile_bin_vlgc all;
make -f makefile_lib_testing_mdl all;
make -f makefile_slib_testing_mdl all;
make -f makefile_bin_peer all;
make -f makefile_bin_peer_dyn all;
echo "@@@@@@ end DEBUG build @@@@@@"
