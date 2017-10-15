cd ..\test\wkdir\blzpeer_bth
break > _bsns.log
break > _uncollc.log
SET PATH=%PATH%;..\..\..\contrib_lib\nt10_x64;..\..\..\bin\MSVC\x64\Debug\DynamicLibrary\v120
..\..\..\bin\MSVC\x64\Debug\Application\v120\peer_dyna.exe -file