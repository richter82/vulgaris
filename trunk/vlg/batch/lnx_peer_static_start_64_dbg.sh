#!/bin/bash
cd ../test/wkdir/blzpeer_bth_static
cat /dev/null > _bsns.log
cat /dev/null > _uncollc.log
LD_LIBRARY_PATH="../../../bin/LNX_64/DBG/libso"
export LD_LIBRARY_PATH
../../../bin/LNX_64/DBG/test/blzpeer_sta/blzstatpeer -file;
