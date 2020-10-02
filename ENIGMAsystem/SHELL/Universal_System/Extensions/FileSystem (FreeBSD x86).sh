cd "${0%/*}"

mkdir "FileSystem (x86)"
mkdir "FileSystem (x86)/FreeBSD"
g++ "FileSystem.cpp" "FileSystem/FreeBSD/filesystem.cpp" "FileSystem/POSIX/filesystem.cpp" "FileSystem/Universal/filesystem.cpp" -o "FileSystem (x86)/FreeBSD/libfilesystem.so" -std=c++17 -shared -static-libgcc -static-libstdc++ -fPIC -m32
