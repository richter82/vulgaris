#!/bin/bash
cd ../test/wkdir/blzpeer_bth
cat /dev/null > _bsns.log
cat /dev/null > _uncollc.log
DYLD_LIBRARY_PATH="../../../bin/OSX_64/DBG/libso"
export DYLD_LIBRARY_PATH
../../../bin/OSX_64/DBG/test/blzpeer_dyna/blzdynapeer -file;
