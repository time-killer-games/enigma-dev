cd "${0%/*}"

mkdir "VideoPlayer (x86)"
mkdir "VideoPlayer (x86)/Linux"
g++ "VideoPlayer.cpp" "VideoPlayer/videoplayer.cpp" -o "VideoPlayer (x86)/Linux/libvidplayer.so" -DVIDPLAYER_SELF_CONTAINED -std=c++17 -shared `pkg-config --libs --cflags mpv` -static-libgcc -static-libstdc++ -fPIC -m32
