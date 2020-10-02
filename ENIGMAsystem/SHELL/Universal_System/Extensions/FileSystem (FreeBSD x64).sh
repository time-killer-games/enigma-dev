cd "${0%/*}"

mkdir "FileSystem (x64)"
mkdir "FileSystem (x64)/FreeBSD"
g++ "FileSystem.cpp" "FileSystem/FreeBSD/filesystem.cpp" "FileSystem/POSIX/filesystem.cpp" "FileSystem/Universal/filesystem.cpp" -o "FileSystem (x64)/FreeBSD/libfilesystem.so" -std=c++17 -shared -static-libgcc -static-libstdc++ -fPIC -m64
