cd "${0%/*}"

mkdir "VideoPlayer (x64)"
mkdir "VideoPlayer (x64)/MacOSX"
clang++ -ObjC -c "VideoPlayer/videoplayer.mm" -fPIC -m64
clang++ "VideoPlayer.cpp" "VideoPlayer/videoplayer.cpp" "videoplayer.o" -o "VideoPlayer (x64)/MacOSX/libvidplayer.dylib" -DVIDPLAYER_SELF_CONTAINED -std=c++17 -shared `pkg-config --libs --cflags mpv` -framework Cocoa -fPIC -m64
rm -f "videoplayer.o"
