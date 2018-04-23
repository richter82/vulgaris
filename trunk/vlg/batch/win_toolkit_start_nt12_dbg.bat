cd ..\test\wkdir\vlgtoolkit
break > _bsns.log
break > _uncollc.log

SET PATH=%PATH%;C:\Qt\Qt5.9.1\5.9.1\msvc2015_64\bin
SET PATH=%PATH%;..\..\..\contrib_lib\nt10_x64;..\..\..\bin\MSVC\x64\Debug\DynamicLibrary\v140
..\..\..\bin_toolkit\debug\vlg_toolkit.exe