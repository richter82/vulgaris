cd ..\test\wkdir\peer_dyn
break > _bsns.log
break > _uncollc.log
SET PATH=%PATH%;..\..\..\contrib_lib\nt10_x64;..\..\..\bin\MSVC\x64\Debug\DynamicLibrary\v140
..\..\..\bin\MSVC\x64\Debug\Application\v140\peer_dyn.exe -file