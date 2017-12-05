cd ..\test\wkdir\vlgtoolkit
break > _bsns.log
break > _uncollc.log

SET PATH=%PATH%;C:\Qt\Qt5.9.1\5.9.1\msvc2013_64\bin
SET PATH=%PATH%;..\..\..\contrib_lib\nt10_x64;..\..\..\bin\MSVC\x64\Debug\DynamicLibrary\v120
..\..\..\bin_toolkit\debug\vlg_toolkit.exe