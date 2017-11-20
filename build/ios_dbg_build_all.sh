#!/bin/bash
export LC_LANG=en_GB
export OUT_ARCH_DIR=IOS_64
export OUT_TRGT_DIR=DBG
export CC_DBG_REL_OPT=-g
export LD_DBG_REL_OPT=-g
echo "@@@@@@ begin DEBUG build @@@@@@"
#make -f makefile_lib_cr all;
#make -f makefile_lib_vlg all; 
#make -f makefile_lib_sqlite all;
#make -f makefile_lib_drvsqlite all;
#make -f makefile_lib_apic all;
#make -f makefile_lib_apicpp all;
echo "@@@@@@ end DEBUG build @@@@@@"