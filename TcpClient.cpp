#include "TcpClient.hpp"

namespace TCP {

Client::Client(boost::asio::io_context &context)
	: sock(context), between(false)
{
	buff.reserve(256);
	outputter.str("");
}

Client::Client(tcp::socket &&context)
	: sock(std::move(context)), between(false)
{
	buff.reserve(256);
	outputter.str("");
}

}
