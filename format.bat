@ECHO OFF 
astyle --project ^
 ./public/c/*.h^
 ./public/*.h^
 ./public/cpp/*.h^
 ./src/*.h^
 ./src/apicpp/*.cpp^
 ./src/apic/*.cpp^
 ./src/apiobjc/*.m^
 ./src/vlgc/*.h^
 ./src/vlgc/*.cpp^
 ./src/drisqlite/*.cpp^
 ./src/vlg/*.h^
 ./src/vlg/*.cpp^
 ./test/src/*.cpp^
 ./toolkit/*.h^
 ./toolkit/*.cpp
ECHO vlg formatting complete
pause