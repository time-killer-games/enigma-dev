cd "${0%/*}"

mkdir "VideoPlayer (x86)"
mkdir "VideoPlayer (x86)/Win32"
g++ "VideoPlayer.cpp" "VideoPlayer/videoplayer.cpp" -o "VideoPlayer (x86)/Win32/libvidplayer.dll" -DVIDPLAYER_SELF_CONTAINED -std=c++17 -shared `pkg-config --libs --cflags mpv` -static-libgcc -static-libstdc++ -fPIC -m32
