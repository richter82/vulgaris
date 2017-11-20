#!/bin/bash
cd ../test/wkdir/peer_dyn
cat /dev/null > _bsns.log
cat /dev/null > _uncollc.log
DYLD_LIBRARY_PATH="../../../bin/OSX_64/DBG/libso"
export DYLD_LIBRARY_PATH
../../../bin/OSX_64/DBG/bin/peer_dyn -file;
