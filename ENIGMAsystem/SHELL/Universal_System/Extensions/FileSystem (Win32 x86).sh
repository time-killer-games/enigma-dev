cd "${0%/*}"

mkdir "FileSystem (x86)"
mkdir "FileSystem (x86)/Win32"
g++ "FileSystem.cpp" "FileSystem/Win32/filesystem.cpp" "FileSystem/Universal/filesystem.cpp" -o "FileSystem (x86)/Win32/libfilesystem.dll" -std=c++17 -shared -static-libgcc -static-libstdc++ -static -fPIC -m32
