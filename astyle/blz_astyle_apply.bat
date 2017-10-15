@ECHO OFF 
.\bin\astyle --max-code-length=80 -s4 --break-after-logical --suffix=none --unpad-paren --align-pointer=name --align-reference=name --style=kr --attach-namespaces --attach-classes --attach-extern-c --attach-inlines --indent-classes --indent-switches --indent-col1-comments --min-conditional-indent=2 --max-instatement-indent=100 --add-brackets --convert-tabs --lineend=linux --formatted^
 ../src/*.h^
 ../src/blz_cpp_public_impl/*.cpp^
 ../src/blz_c_public_impl/*.cpp^
 ../src/blzc/*.cpp^
 ../src/blzdrisqlitelib/*.cpp^
 ../src/blzmodellib/*.cpp^
 ../src/blzpeerlib/*.cpp^
 ../src/blzperslib/*.cpp^
 ../src/crlib/*.h^
 ../src/crlib/*.cpp^
 ../public/cpp/*.h^
 ../public/c/*.h^
 ../test/src/*.cpp^
 ../toolkit/blz_toolkit/*.h^
 ../toolkit/blz_toolkit/*.cpp
ECHO blz formatting complete
pause