cd ..\test\wkdir\blztoolkit
break > _bsns.log
break > _uncollc.log

SET PATH=%PATH%;C:\Qt\Qt5.9.1\5.9.1\msvc2013_64\bin
SET PATH=%PATH%;..\..\..\contrib_lib\nt10_x64;..\..\..\bin\MSVC\x64\Debug\DynamicLibrary\v120
..\..\..\toolkit\build-blz_toolkit-Desktop_Qt_5_9_1_MSVC2013_64bit-Debug\debug\blz_toolkit.exe
REM ..\..\..\toolkit\build-blz_toolkit-Desktop_Qt_5_6_2_MSVC2013_64bit-Debug\debug\blz_toolkit.exe

REM **debug**
REM ..\..\toolkit\build-blz_toolkit-qt_source_md-Debug\debug\blz_toolkit.exe