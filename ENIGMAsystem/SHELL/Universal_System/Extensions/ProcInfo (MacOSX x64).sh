cd "${0%/*}"

mkdir "ProcInfo (x64)"
mkdir "ProcInfo (x64)/MacOSX"
clang++ -ObjC++ "ProcInfo.cpp" "ProcInfo/Universal/helpers.cpp" "ProcInfo/MacOSX/procinfo.mm" "ProcInfo/MacOSX/subclass.mm" "ProcInfo/POSIX/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" "procinfo.o" "subclass.o" -o "ProcInfo (x64)/MacOSX/libprocinfo.dylib" -std=c++17 -shared -framework Cocoa -fPIC -m64
