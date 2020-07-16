TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        SocketHandlerUnix.cpp \
        SocketHandlerWin32.cpp \
        StdStream.cpp \
        main.cpp

HEADERS += \
	FIO.hpp \
	SocketHandlerUnix.hpp \
	SocketHandlerWin32.hpp \
	StdStream.hpp

DISTFILES += \
	README.md
