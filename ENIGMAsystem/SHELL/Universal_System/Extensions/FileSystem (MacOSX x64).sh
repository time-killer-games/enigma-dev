cd "${0%/*}"

mkdir "FileSystem (x64)"
mkdir "FileSystem (x64)/MacOSX"
clang++ "FileSystem.cpp" "FileSystem/MacOSX/filesystem.cpp" "FileSystem/POSIX/filesystem.cpp" "FileSystem/Universal/filesystem.cpp" -o "FileSystem (x64)/MacOSX/libfilesystem.dylib" -std=c++17 -shared -fPIC -m64
