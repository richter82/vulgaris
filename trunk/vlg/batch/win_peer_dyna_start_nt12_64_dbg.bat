cd ..\test\wkdir\peer_dyn
break > _bsns.log
break > _uncollc.log
SET PATH=%PATH%;..\..\..\contrib_lib\nt10_x64;..\..\..\bin\MSVC\x64\Debug\DynamicLibrary
..\..\..\bin\MSVC\x64\Debug\Application\peer_dyn.exe -file