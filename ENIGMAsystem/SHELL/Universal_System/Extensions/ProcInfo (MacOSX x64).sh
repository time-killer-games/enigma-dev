cd "${0%/*}"

mkdir "ProcInfo (x64)"
mkdir "ProcInfo (x64)/MacOSX"
clang++ "ProcInfo.cpp" "ProcInfo/MacOSX/procinfo.mm" "ProcInfo/MacOSX/subclass.mm" "ProcInfo/POSIX/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" -o "ProcInfo (x64)/MacOSX/libprocinfo.dylib" -std=c++17 -ObjC++ -shared -framework Cocoa -fPIC -m64
