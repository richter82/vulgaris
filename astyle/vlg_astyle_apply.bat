@ECHO OFF 
.\bin\astyle --max-code-length=80 -s4 --break-after-logical --suffix=none --unpad-paren --align-pointer=name --align-reference=name --style=kr --attach-namespaces --attach-classes --attach-extern-c --attach-inlines --indent-classes --indent-switches --indent-col1-comments --min-conditional-indent=2 --max-instatement-indent=100 --add-brackets --convert-tabs --lineend=linux --formatted^
 ../src/*.h^
 ../src/libapicpp/*.cpp^
 ../src/libapic/*.cpp^
 ../src/libapiobjc/*.m^
 ../src/vlgc/*.h^
 ../src/vlgc/*.cpp^
 ../src/libdrisqlite/*.cpp^
 ../src/libvlg/*.h^
 ../src/libvlg/*.cpp^
 ../src/libcr/*.h^
 ../src/libcr/*.cpp^
 ../public/*.h^
 ../public/cpp/*.h^
 ../public/c/*.h^
 ../test/src/*.cpp^
 ../toolkit/vlg_toolkit/*.h^
 ../toolkit/vlg_toolkit/*.cpp
ECHO blz formatting complete
pause