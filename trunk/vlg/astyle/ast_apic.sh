#!/bin/bash
./bin/astyle -s4 --break-after-logical --suffix=none --unpad-paren --align-pointer=name --align-reference=name --style=kr --attach-namespaces --attach-classes --attach-extern-c --attach-inlines --indent-classes --indent-switches --indent-col1-comments --min-conditional-indent=2 --max-instatement-indent=100 --add-brackets --convert-tabs --lineend=linux --formatted \
 ../src/apic/*.cpp \
 ../public/c/*.h
echo apic formatting complete