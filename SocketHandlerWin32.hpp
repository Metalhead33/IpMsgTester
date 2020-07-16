#ifndef SOCKETHANDLERWIN32_HPP
#define SOCKETHANDLERWIN32_HPP
#ifdef _WIN32
#include <string>
extern "C" {
#include <winsock.h>
}

class SocketHandlerWin32
{
private:
	static WSADATA wsaData;
public:
	SocketHandlerWin32();
};

#endif
#endif // SOCKETHANDLERWIN32_HPP
