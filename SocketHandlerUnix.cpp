#ifdef __unix__
#include "SocketHandlerUnix.hpp"
#include <cstring>
#include <stdexcept>
extern "C" {
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
}

SocketHandlerUnix::SocketHandlerUnix()
	: sockfd(socket(AF_INET, SOCK_STREAM, 0)), conType(CONNECTION_TYPE::DISCONNECTED)
{
	if(sockfd != -1) throw std::runtime_error("Could not open socket!\n");
}
SocketHandlerUnix::SocketHandlerUnix(const char *addr, int port, CONNECTION_TYPE nConType)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd != -1) throw std::runtime_error("Could not open socket!\n");
	socketAddress.sin_port = htons(port);
	inet_aton(addr,&(socketAddress.sin_addr));
	std::memset(socketAddress.sin_zero,0,8);
	switch (nConType) {
	case CONNECTION_TYPE::CONNECTED:
		{
		if(!connectTo())
			throw std::runtime_error("Could not connect to socket!\n");
		conType = CONNECTION_TYPE::CONNECTED;
		break;
		}
	case CONNECTION_TYPE::BOUND:
	{
		if(!bindTo())
			throw std::runtime_error("Could not connect to socket!\n");
		conType = CONNECTION_TYPE::BOUND;
		break;
	}
	default:
		conType = CONNECTION_TYPE::DISCONNECTED;
		break;
	}
}

SocketHandlerUnix::SocketHandlerUnix(SocketHandlerUnix &&mov)
	: sockfd(mov.sockfd), conType(mov.conType)
{
	mov.conType = CONNECTION_TYPE::DISCONNECTED;
	mov.sockfd = -1;
	std::memcpy(&socketAddress,&(mov.socketAddress),sizeof(socketAddress));
	std::memset(&(mov.socketAddress),0,sizeof(socketAddress));
}

SocketHandlerUnix::CONNECTION_TYPE SocketHandlerUnix::getConType() const
{
	return conType;
}

void SocketHandlerUnix::operator=(SocketHandlerUnix &&mov)
{
	this->sockfd = mov.sockfd;
	mov.sockfd = -1;
	this->conType = mov.conType;
	mov.conType = CONNECTION_TYPE::DISCONNECTED;
	std::memcpy(&socketAddress,&(mov.socketAddress),sizeof(socketAddress));
	std::memset(&(mov.socketAddress),0,sizeof(socketAddress));
}

SocketHandlerUnix::~SocketHandlerUnix()
{
	if(sockfd != -1) close(sockfd);
}

std::string SocketHandlerUnix::getIpAddr() const
{
	return std::string(inet_ntoa(socketAddress.sin_addr));
}

bool SocketHandlerUnix::connectTo()
{
	if(connect(sockfd,reinterpret_cast<struct sockaddr *>(&socketAddress),sizeof(socketAddress)) == -1)
	{
		conType = CONNECTION_TYPE::DISCONNECTED;
		return false;
	}
	else {
		conType = CONNECTION_TYPE::CONNECTED;
		return true;
	}
}

bool SocketHandlerUnix::connectTo(const char *addr, int port)
{
	socketAddress.sin_port = htons(port);
	inet_aton(addr,&(socketAddress.sin_addr));
	std::memset(socketAddress.sin_zero,0,8);
	if(connect(sockfd,reinterpret_cast<struct sockaddr *>(&socketAddress),sizeof(socketAddress)) == -1)
	{
		return false;
	}
	else {
		conType = CONNECTION_TYPE::CONNECTED;
		return true;
	}
}

bool SocketHandlerUnix::bindTo()
{
	if(bind(sockfd,reinterpret_cast<struct sockaddr *>(&socketAddress),sizeof(socketAddress)) == -1)
	{
		conType = CONNECTION_TYPE::DISCONNECTED;
		return false;
	}
	else {
		conType = CONNECTION_TYPE::BOUND;
		return true;
	}
}

bool SocketHandlerUnix::bindTo(const char *addr, int port)
{
	socketAddress.sin_port = htons(port);
	inet_aton(addr,&(socketAddress.sin_addr));
	std::memset(socketAddress.sin_zero,0,8);
	if(bind(sockfd,reinterpret_cast<struct sockaddr *>(&socketAddress),sizeof(socketAddress)) == -1)
	{
		return false;
	}
	else {
		conType = CONNECTION_TYPE::BOUND;
		return true;
	}
}

long SocketHandlerUnix::sendMessage(const void *msg, size_t len, int flags) const
{
	return send(sockfd,msg,len,flags);
}

long SocketHandlerUnix::receiveMessage(void *buf, size_t len, unsigned int flags) const
{
	return recv(sockfd,buf,len,flags);
}

int SocketHandlerUnix::listenTo(int sockfd, int backlog) const
{
	return listen(sockfd,backlog);
}

int SocketHandlerUnix::acceptConnection()
{
	socklen_t sizeToPass = sizeof(socketAddress);
	return accept(sockfd,reinterpret_cast<struct sockaddr*>(&socketAddress),&sizeToPass);
}

bool SocketHandlerUnix::getDebug() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_DEBUG,&retval,&len) == -1) return false;
	else return bool(retval.integer);
}
void SocketHandlerUnix::setDebug(bool nval)
{
	int setto = nval;
	setsockopt(sockfd,SOL_SOCKET,SO_DEBUG,&setto,sizeof(int));
}
bool SocketHandlerUnix::getBroadcast() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&retval,&len) == -1) return false;
	else return bool(retval.integer);
}
void SocketHandlerUnix::setBroadcast(bool nval)
{
	int setto = nval;
	setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&setto,sizeof(int));
}
bool SocketHandlerUnix::getReuseAddr() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&retval,&len) == -1) return false;
	else return bool(retval.integer);
}
void SocketHandlerUnix::setReuseAddr(bool nval)
{
	int setto = nval;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&setto,sizeof(int));
}
bool SocketHandlerUnix::getKeepAlive() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,&retval,&len) == -1) return false;
	else return bool(retval.integer);
}
void SocketHandlerUnix::setKeepAlive(bool nval)
{
	int setto = nval;
	setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,&setto,sizeof(int));
}
int SocketHandlerUnix::getOobinline() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_OOBINLINE,&retval,&len) == -1) return -1;
	else return retval.integer;
}
void SocketHandlerUnix::setOobinline(int nval)
{
	setsockopt(sockfd,SOL_SOCKET,SO_OOBINLINE,&nval,sizeof(int));
}
int SocketHandlerUnix::getSndBuffSize() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,&retval,&len) == -1) return -1;
	else return retval.integer;
}
void SocketHandlerUnix::setSndBuffSize(int nval)
{
	setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,&nval,sizeof(int));
}
int SocketHandlerUnix::getRcvBuffSize() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&retval,&len) == -1) return -1;
	else return retval.integer;
}
void SocketHandlerUnix::setRcvBuffSize(int nval)
{
	setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&nval,sizeof(int));
}
bool SocketHandlerUnix::getDontRoute() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_DONTROUTE,&retval,&len) == -1) return false;
	else return bool(retval.integer);
}
void SocketHandlerUnix::setDontRoute(bool nval)
{
	int setto = nval;
	setsockopt(sockfd,SOL_SOCKET,SO_DONTROUTE,&setto,sizeof(int));
}
int SocketHandlerUnix::getRclLoWat() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_RCVLOWAT,&retval,&len) == -1) return -1;
	else return retval.integer;
}
void SocketHandlerUnix::setRclLoWat(int nval)
{
	setsockopt(sockfd,SOL_SOCKET,SO_RCVLOWAT,&nval,sizeof(int));
}
timeval SocketHandlerUnix::getRcvTimeout() const
{
	ReceptClass retval;
	socklen_t len=sizeof(timeval);
	if(getsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&retval,&len) == -1) return timeval();
	else return retval.tim;
}
void SocketHandlerUnix::setRcvTimeout(timeval nval)
{
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&nval,sizeof(timeval));
}
int SocketHandlerUnix::getSndLoWat() const
{
	ReceptClass retval;
	socklen_t len=sizeof(int);
	if(getsockopt(sockfd,SOL_SOCKET,SO_SNDLOWAT,&retval,&len) == -1) return -1;
	else return retval.integer;
}
void SocketHandlerUnix::setSndLoWat(int nval)
{
	setsockopt(sockfd,SOL_SOCKET,SO_SNDLOWAT,&nval,sizeof(int));
}
timeval SocketHandlerUnix::getSndTimeout() const
{
	ReceptClass retval;
	socklen_t len=sizeof(timeval);
	if(getsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&retval,&len) == -1) return timeval();
	else return retval.tim;
}
void SocketHandlerUnix::setSndTimeout(timeval nval)
{
	setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&nval,sizeof(timeval));
}

void SocketHandlerUnix::setBlocking(bool isBlocking)
{
	unsigned long n=isBlocking;
	ioctl(sockfd,FIONBIO,&n);
}

#endif
