cd "${0%/*}"

mkdir "ProcInfo (x64)"
mkdir "ProcInfo (x64)/MacOSX"
clang++ -c -ObjC "ProcInfo/MacOSX/Cocoa/procinfo.mm" "ProcInfo/MacOSX/Cocoa/subclass.mm" -fPIC -m64
clang++ "ProcInfo.cpp" "ProcInfo/helpers.cpp" "ProcInfo/MacOSX/procinfo.cpp" "ProcInfo/POSIX/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" "procinfo.o" "subclass.o" -o "ProcInfo (x64)/MacOSX/libprocinfo.dylib" -DPROCINFO_SELF_CONTAINED -std=c++17 -shared -framework Cocoa -fPIC -m64
rm -f "procinfo.o"
rm -f "subclass.o"
