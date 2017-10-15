#!/bin/bash
cd ../test/wkdir/blzpeer_bth_static
cat /dev/null > _bsns.log
cat /dev/null > _uncollc.log
../../../bin/OSX_64/DBG/test/blzpeer_sta/blzstatpeer -file;
