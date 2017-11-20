#!/bin/bash
cd ../test/wkdir/peer_sta
cat /dev/null > _bsns.log
cat /dev/null > _uncollc.log
../../../bin/OSX_64/DBG/bin/peer -file;
