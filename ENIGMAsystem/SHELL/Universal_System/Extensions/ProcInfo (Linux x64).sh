cd "${0%/*}"

mkdir "ProcInfo (x64)"
mkdir "ProcInfo (x64)/Linux"
g++ "ProcInfo.cpp" "ProcInfo/Universal/helpers.cpp" "ProcInfo/POSIX/procinfo.cpp" "ProcInfo/xlib/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" -o "ProcInfo (x64)/Linux/libprocinfo.so" -std=c++17 -shared -static-libgcc -static-libstdc++ -lX11 -lprocps -lpthread -fPIC -m64
