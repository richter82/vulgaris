#!/bin/bash
export LC_LANG=en_GB
export OUT_ARCH_DIR=OSX_64
export OUT_TRGT_DIR=REL
export CC_DBG_REL_OPT=-O2
echo "@@@@@@ begin DEBUG build @@@@@@"
make -f makefile_lib_cr all;
make -f makefile_lib_model all; 
make -f makefile_lib_peer all;
make -f makefile_lib_pers all;
make -f makefile_lib_sqlite all;
make -f makefile_lib_drvsqlite all;
make -f makefile_lib_blz_c_intrf all;
make -f makefile_lib_blz_cpp_intrf all;
make -f makefile_bin_blzc all;
make -f makefile_lib_testing_mdl all;
make -f makefile_bin_static_peer all;
echo "@@@@@@ end DEBUG build @@@@@@"
