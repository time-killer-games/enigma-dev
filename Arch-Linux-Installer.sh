#!/bin/bash
cd ~/
echo "Installing dependencies..."
sudo pacman -Sy wget git gcc gdb make pkg-config protobuf protobuf-c zlib glew glm libpng glu mesa openal libogg alure mpg123 fluidsynth libvorbis vorbis-tools box2d dumb sdl2 freetype2 libffi libx11 libxrandr libxinerama jre-openjdk jdk-openjdk pkg-config rapidjson yaml-cpp boost pulseaudio pugixml zenity kdialog mpv
echo "Downloading Enigma..."
git clone git://github.com/time-killer-games/enigma-dev-fork.git ~/enigma-dev
git clone git://github.com/time-killer-games/enigma-dev.git ~/enigma-dev.tmp
mv ~/enigma-dev.tmp/ENIGMAsystem/SHELL/Universal_System/Extensions/ProcInfo ~/enigma-dev/ENIGMAsystem/SHELL/Universal_System/Extensions/ProcInfo
mv ~/enigma-dev.tmp/ENIGMAsystem/SHELL/Universal_System/Extensions/FileSystem ~/enigma-dev/ENIGMAsystem/SHELL/Universal_System/Extensions/FileSystem
mv ~/enigma-dev.tmp/ENIGMAsystem/SHELL/Universal_System/Extensions/VideoPlayer ~/enigma-dev/ENIGMAsystem/SHELL/Universal_System/Extensions/VideoPlayer
rm -f ~/enigma-dev/ENIGMAsystem/SHELL/Widget_Systems/xlib/Info/About.ey
mv ~/enigma-dev.tmp/ENIGMAsystem/SHELL/Widget_Systems/xlib/Info/About.ey ~/enigma-dev/ENIGMAsystem/SHELL/Widget_Systems/xlib/Info/About.ey
rm -rf ~/enigma-dev.tmp
cd enigma-dev
echo "Downloading easy startup script..."
wget http://pastebin.com/raw/6ZmrzWKP -O start.sh
sed -i -e 's/\r$//' start.sh
echo "Correcting permissions..."
chmod +x start.sh
chmod +x install.sh
echo "Installing..."
./install.sh
echo "Rebuilding compiler..."
make clean
make
echo "Done, to start Enigma just run ~/enigma-dev/start.sh"
