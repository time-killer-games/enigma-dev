git clone git://github.com/time-killer-games/enigma-dev-fork.git /c/enigma-dev
git clone git://github.com/time-killer-games/enigma-dev.git /c/enigma-dev.tmp
mv /c/enigma-dev.tmp/ENIGMAsystem/SHELL/Universal_System/Extensions/ProcInfo /c/enigma-dev/ENIGMAsystem/SHELL/Universal_System/Extensions/ProcInfo
mv /c/enigma-dev.tmp/ENIGMAsystem/SHELL/Universal_System/Extensions/FileSystem /c/enigma-dev/ENIGMAsystem/SHELL/Universal_System/Extensions/FileSystem
mv /c/enigma-dev.tmp/ENIGMAsystem/SHELL/Universal_System/Extensions/VideoPlayer /c/enigma-dev/ENIGMAsystem/SHELL/Universal_System/Extensions/VideoPlayer
rm -f /c/enigma-dev/ENIGMAsystem/SHELL/Widget_Systems/xlib/Info/About.ey
mv /c/enigma-dev.tmp/ENIGMAsystem/SHELL/Widget_Systems/xlib/Info/About.ey /c/enigma-dev/ENIGMAsystem/SHELL/Widget_Systems/xlib/Info/About.ey
rm -rf /c/enigma-dev.tmp
