cd "${0%/*}"

mkdir "VideoPlayer (x86)"
mkdir "VideoPlayer (x86)/FreeBSD"
g++ "VideoPlayer.cpp" "VideoPlayer/videoplayer.cpp" -o "VideoPlayer (x86)/FreeBSD/libvidplayer.so" -DVIDPLAYER_SELF_CONTAINED -std=c++17 -shared `pkg-config --libs --cflags mpv` -static-libgcc -static-libstdc++ -fPIC -m32
