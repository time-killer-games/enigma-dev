#!/bin/bash
cd ~/
echo "Installing dependencies..."
sudo pkg ins wget git llvm gcc gdb gmake pkgconf protobuf protobuf-c lzlib glew glm png libGLU freeglut mesa-libs openal-soft libogg alure libsndfile freealut libvorbis vorbis-tools Box2D dumb sdl2 freetype2 libffi libX11 libXrandr libXinerama openjdk-jre openjdk8 rapidjson libyaml boost-libs pulseaudio pugixml yaml-cpp zenity kdialog mpv
echo "Downloading Enigma..."
git clone git://github.com/time-killer-games/enigma-dev-fork.git ~/enigma-dev
git clone git://github.com/time-killer-games/enigma-dev.git ~/enigma-dev.tmp
mv ~/enigma-dev.tmp/ENIGMAsystem/SHELL/Universal_System/Extensions/ProcInfo ~/enigma-dev/ENIGMAsystem/SHELL/Universal_System/Extensions/ProcInfo
mv ~/enigma-dev.tmp/ENIGMAsystem/SHELL/Universal_System/Extensions/FileSystem ~/enigma-dev/ENIGMAsystem/SHELL/Universal_System/Extensions/FileSystem
mv ~/enigma-dev.tmp/ENIGMAsystem/SHELL/Universal_System/Extensions/VideoPlayer ~/enigma-dev/ENIGMAsystem/SHELL/Universal_System/Extensions/VideoPlayer
rm -f ~/enigma-dev/ENIGMAsystem/SHELL/Widget_Systems/xlib/Info/About.ey
mv ~/enigma-dev.tmp/ENIGMAsystem/SHELL/Widget_Systems/xlib/Info/About.ey ~/enigma-dev/ENIGMAsystem/SHELL/Widget_Systems/xlib/Info/About.ey
rm -rf ~/enigma-dev.tmp
ln -s /usr/local/include/google ~/enigma-dev/shared/protos/google
ln -s /usr/local/include/google ~/enigma-dev/CommandLine/emake/google
cd enigma-dev
echo "Downloading easy startup script..."
wget https://pastebin.com/raw/aBAU4j3C -O start.sh
sed -i -e 's/\r$//' start.sh
echo "Correcting permissions..."
chmod +x start.sh
chmod +x install.sh
echo "Installing..."
./install.sh
echo "Rebuilding compiler..."
gmake clean
gmake
gmake emake
echo "Done, to start Enigma just run 'cd ~/enigma-dev;java -jar lateralgm.jar'"
