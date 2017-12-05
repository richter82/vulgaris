@ECHO OFF 
.\bin\astyle --max-code-length=120 -s4 --break-after-logical --suffix=none --unpad-paren --align-pointer=name --align-reference=name --style=kr --attach-namespaces --attach-classes --attach-extern-c --attach-inlines --indent-classes --indent-switches --indent-col1-comments --min-conditional-indent=2 --max-instatement-indent=100 --add-brackets --convert-tabs --lineend=linux --formatted^
 ../src/*.h^
 ../src/apicpp/*.cpp^
 ../src/apic/*.cpp^
 ../src/apiobjc/*.m^
 ../src/vlgc/*.h^
 ../src/vlgc/*.cpp^
 ../src/drisqlite/*.cpp^
 ../src/vlg/*.h^
 ../src/vlg/*.cpp^
 ../src/cr/*.h^
 ../src/cr/*.cpp^
 ../public/*.h^
 ../public/cpp/*.h^
 ../public/c/*.h^
 ../test/src/*.cpp^
 ../toolkit/*.h^
 ../toolkit/*.cpp
ECHO vlg formatting complete
pause