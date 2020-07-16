TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        SocketHandlerUnix.cpp \
        SocketHandlerWin32.cpp \
        main.cpp

HEADERS += \
	SocketHandlerUnix.hpp \
	SocketHandlerWin32.hpp

DISTFILES += \
	README.md
