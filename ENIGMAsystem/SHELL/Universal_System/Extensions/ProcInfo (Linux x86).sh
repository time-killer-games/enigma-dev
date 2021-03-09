cd "${0%/*}"

mkdir "ProcInfo (x86)"
mkdir "ProcInfo (x86)/Linux"
g++ "ProcInfo.cpp" "ProcInfo/POSIX/procinfo.cpp" "ProcInfo/xlib/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" -o "ProcInfo (x86)/Linux/libprocinfo.so" -std=c++17 -shared -static-libgcc -static-libstdc++ -lX11 -lprocps -lpthread -fPIC -m32
