cd "${0%/*}"

mkdir "ProcInfo (x64)"
mkdir "ProcInfo (x64)/Win32"
g++ "ProcInfo.cpp" "ProcInfo/helpers.cpp" "ProcInfo/Win32/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" -o "ProcInfo (x64)/Win32/libprocinfo.dll" -DPROCINFO_SELF_CONTAINED -DEXE_INCLUDES -std=c++17 -shared -static-libgcc -static-libstdc++ -static -lole32 -lwbemuuid -m64
g++ "ProcInfo/Win32/getprocinfo.cpp" "ProcInfo/helpers.cpp" "ProcInfo/Win32/procinfo.cpp" "ProcInfo/Universal/procinfo.cpp" -o "ProcInfo/Win32/getprocinfo64.exe" -DPROCINFO_SELF_CONTAINED -std=c++17 -static-libgcc -static-libstdc++ -static -lole32 -lwbemuuid -m64
cd "ProcInfo/Win32"
xxd -i 'getprocinfo64' | sed 's/\([0-9a-f]\)$/\0, 0x00/' > 'getprocinfo64.h'
rm -f "getprocinfo64.exe"
