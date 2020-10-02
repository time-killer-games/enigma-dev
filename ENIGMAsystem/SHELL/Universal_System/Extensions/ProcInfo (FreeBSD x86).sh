cd "${0%/*}"

mkdir "ProcInfo (x86)"
mkdir "ProcInfo (x86)/FreeBSD"
g++ "ProcInfo.cpp" "ProcInfo/helpers.cpp" "ProcInfo/FreeBSD/procinfo.cpp" "ProcInfo/POSIX/procinfo.cpp" "ProcInfo/xlib/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" -o "ProcInfo (x86)/FreeBSD/libprocinfo.so" -DPROCINFO_SELF_CONTAINED -std=c++17 -shared -static-libgcc -static-libstdc++ -lX11 -lprocstat -lutil -lc -lpthread -fPIC -m32
