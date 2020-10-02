#!/bin/bash
cd ~/
echo "Installing dependencies..."
sudo add-apt-repository ppa:maarten-fonville/protobuf
sudo apt-get update
sudo apt-get install wget git g++ make build-essential libprotobuf-dev protobuf-compiler zlib1g-dev libglew-dev libglm-dev libpng-dev libglu1-mesa-dev libopenal-dev libogg-dev libalure-dev libvorbisfile3 libvorbis-dev libbox2d-dev libdumb1-dev libsdl2-dev libfreetype6-dev libffi-dev libx11-dev libxrandr-dev libxinerama-dev libprocps-dev libepoxy-dev default-jre default-jdk pkg-config rapidjson-dev libyaml-cpp-dev libboost-dev libboost-filesystem-dev libboost-system-dev libboost-program-options-dev libboost-iostreams-dev pulseaudio libpugixml-dev zenity kdialog mpv
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
wget https://pastebin.com/raw/aBAU4j3C -O start.sh
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
