#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <array>
#include <sstream>

namespace TCP {

using boost::asio::ip::tcp;


class Client : boost::enable_shared_from_this<Client>
{
public:
	friend class Server;
	typedef boost::shared_ptr<Client> SharedPointer;
	typedef std::array<uint8_t,256> Arr;
	typedef Arr::iterator Iterator;
private:
	tcp::socket sock;
	//std::vector<uint8_t> buff;
	Arr buff;
	std::stringstream outputter;
public:
	Client(boost::asio::io_context& context);
	Client(tcp::socket&& context);


};
typedef Client::SharedPointer sClient;

}

#endif // TCPCLIENT_HPP
