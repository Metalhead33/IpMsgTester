#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP
#include "TcpClient.hpp"
#include <list>
#include <fstream>

namespace TCP {
using boost::asio::ip::tcp;
class Server : boost::enable_shared_from_this<Server>
{
public:
	typedef boost::shared_ptr<Server> SharedPointer;
	typedef std::list<Client> ClientList;
	typedef ClientList::iterator ClientIterator;
private:
	boost::asio::io_context& context;
	std::ofstream myfile;
	tcp::acceptor acceptor;
	ClientList clients;
public:
	Server(boost::asio::io_context& iocontenxt, std::ofstream&& nfile, const tcp::endpoint &ep);
	Server(boost::asio::io_context& iocontenxt,const char* nfile, const boost::asio::ip::tcp::endpoint &ep);
	void beginAccepting();
	void handleAcceptedClient(tcp::socket&& client, const boost::system::error_code& error);
	void beginReading(ClientIterator it);
	void onReadBuffer(ClientIterator it, const boost::system::error_code& ec, size_t len);
};
typedef Server::SharedPointer sServer;
}
#endif // TCPSERVER_HPP
