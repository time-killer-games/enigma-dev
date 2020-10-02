cd "${0%/*}"

mkdir "FileSystem (x64)"
mkdir "FileSystem (x64)/Win32"
g++ "FileSystem.cpp" "FileSystem/Win32/filesystem.cpp" "FileSystem/Universal/filesystem.cpp" -o "FileSystem (x64)/Win32/libfilesystem.dll" -std=c++17 -shared -static-libgcc -static-libstdc++ -static -fPIC -m64
