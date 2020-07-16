#ifndef SOCKETHANDLERUNIX_HPP
#define SOCKETHANDLERUNIX_HPP
#ifdef __unix__
#include <string>
extern "C" {
#include <netinet/in.h>
}

class SocketHandlerUnix
{
public:
	enum class CONNECTION_TYPE : uint8_t {
		DISCONNECTED,
		CONNECTED,
		BOUND
	};
	union ReceptClass {
		timeval tim;
		int integer;
		uint64_t pad64;
	};
private:
	sockaddr_in socketAddress;
	int sockfd;
	CONNECTION_TYPE conType;
	SocketHandlerUnix(const SocketHandlerUnix& cpy) = delete; // No copy-constructor
	void operator=(const SocketHandlerUnix& cpy) = delete; // No copy assignment
public:
	~SocketHandlerUnix();
	SocketHandlerUnix();
	SocketHandlerUnix(const char* addr, int port, CONNECTION_TYPE nConType = CONNECTION_TYPE::DISCONNECTED);
	SocketHandlerUnix(SocketHandlerUnix&& mov); // Move constructor
	void operator=(SocketHandlerUnix&& mov); // Move assignment
	std::string getIpAddr() const;

	bool connectTo();
	bool connectTo(const char* addr, int port);
	bool bindTo();
	bool bindTo(const char* addr, int port);
	CONNECTION_TYPE getConType() const;

	long sendMessage(const void *msg, size_t len, int flags = 0) const;
	long receiveMessage(void *buf, size_t len, int flags = 0) const;
	int listenTo(int sockfd, int backlog) const;
	int acceptConnection();

	// Wrapper for setsocketopt and getsocketopt
	bool getDebug() const;
	void setDebug(bool nval);
	bool getBroadcast() const;
	void setBroadcast(bool nval);
	bool getReuseAddr() const;
	void setReuseAddr(bool nval);
	bool getKeepAlive() const;
	void setKeepAlive(bool nval);
	int getOobinline() const;
	void setOobinline(int nval);
	int getSndBuffSize() const;
	void setSndBuffSize(int nval);
	int getRcvBuffSize() const;
	void setRcvBuffSize(int nval);
	bool getDontRoute() const;
	void setDontRoute(bool nval);
	int getRclLoWat() const;
	void setRclLoWat(int nval);
	timeval getRcvTimeout() const;
	void setRcvTimeout(timeval nval);
	int getSndLoWat() const;
	void setSndLoWat(int nval);
	timeval getSndTimeout() const;
	void setSndTimeout(timeval nval);

	void setBlocking(bool isBlocking);
};

#endif
#endif // SOCKETHANDLERUNIX_HPP
