cd ..\test\wkdir\vlgtoolkit
break > _bsns.log
break > _uncollc.log

SET PATH=%PATH%;C:\Qt\Qt5.11.2\5.11.2\msvc2017_64\bin
SET PATH=%PATH%;..\..\..\contrib_lib\nt10_x64;..\..\..\bin\MSVC\x64\Debug\DynamicLibrary
..\..\..\bin_toolkit\debug\vlg_toolkit.exe