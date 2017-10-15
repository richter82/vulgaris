call "%VS120COMNTOOLS%\vsvars32.bat" x86
msbuild.exe blz_system.sln /t:Clean /p:Platform=x64 /property:Configuration=Release
pause