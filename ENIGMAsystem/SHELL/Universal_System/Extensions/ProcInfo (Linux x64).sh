cd "${0%/*}"

mkdir "ProcInfo (x64)"
mkdir "ProcInfo (x64)/Linux"
g++ "ProcInfo.cpp" "ProcInfo/helpers.cpp" "ProcInfo/Linux/procinfo.cpp" "ProcInfo/POSIX/procinfo.cpp" "ProcInfo/xlib/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" -o "ProcInfo (x64)/Linux/libprocinfo.so" -DPROCINFO_SELF_CONTAINED -std=c++17 -shared -static-libgcc -static-libstdc++ -lX11 -lprocps -lpthread -fPIC -m64
