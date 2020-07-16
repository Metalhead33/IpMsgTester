TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lboost_system -pthread

SOURCES += \
        TcpClient.cpp \
        TcpServer.cpp \
        main.cpp

HEADERS += \
	TcpClient.hpp \
	TcpServer.hpp

DISTFILES += \
	README.md
