ifeq ($(UNIX_BASED), true)
	ifeq ($(OS), Darwin)
		SOURCES += Universal_System/Extensions/FileSystem/MacOSX/filesystem.cpp
	else
		ifeq ($(OS), Linux)
			SOURCES += Universal_System/Extensions/FileSystem/Linux/filesystem.cpp
		else ifeq ($(OS), FreeBSD)
			SOURCES += Universal_System/Extensions/FileSystem/FreeBSD/filesystem.cpp
			override LDLIBS += -lc
		endif
	endif
	SOURCES += Universal_System/Extensions/FileSystem/POSIX/filesystem.cpp
else
	SOURCES += Universal_System/Extensions/FileSystem/Win32/filesystem.cpp
endif
SOURCES += Universal_System/Extensions/FileSystem/Universal/filesystem.cpp
