cd "${0%/*}"

mkdir "ProcInfo (x86)"
mkdir "ProcInfo (x86)/FreeBSD"
g++ "ProcInfo.cpp" "ProcInfo/POSIX/procinfo.cpp" "ProcInfo/xlib/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" -o "ProcInfo (x86)/FreeBSD/libprocinfo.so" -std=c++17 -shared -static-libgcc -static-libstdc++ -lX11 -lprocstat -lutil -lc -lpthread -fPIC -m32
