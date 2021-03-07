cd "${0%/*}"

mkdir "ProcInfo (x64)"
mkdir "ProcInfo (x64)/Win32"
g++ "ProcInfo.cpp" "ProcInfo/Universal/helpers.cpp" "ProcInfo/Win32/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" -o "ProcInfo (x64)/Win32/libprocinfo.dll" -std=c++17 -shared -static-libgcc -static-libstdc++ -static -lole32 -lwbemuuid -m64
