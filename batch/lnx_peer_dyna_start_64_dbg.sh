#!/bin/bash
cd ../test/wkdir/peer_dyn
cat /dev/null > _bsns.log
cat /dev/null > _uncollc.log
LD_LIBRARY_PATH="../../../bin/LNX_64/DBG/libso"
export LD_LIBRARY_PATH
../../../bin/LNX_64/DBG/bin/peer_dyn -file;
