#include "TcpServer.hpp"
#include <iomanip>
#include <iostream>

namespace TCP {

Server::Server(boost::asio::io_context &iocontenxt, std::ofstream &&nfile, const boost::asio::ip::tcp::endpoint &ep)
	: context(iocontenxt), myfile(std::move(nfile)), acceptor(context)
{
	acceptor.open(ep.protocol());
	acceptor.set_option(boost::asio::ip::tcp::socket::reuse_address(true));
	acceptor.bind(ep);
}
Server::Server(boost::asio::io_context& iocontenxt, const char* nfile, const tcp::endpoint &ep)
	: context(iocontenxt), acceptor(context)
{
	myfile.open(nfile);
	acceptor.open(ep.protocol());
	acceptor.set_option(boost::asio::ip::tcp::socket::reuse_address(true));
	acceptor.bind(ep);
}

void Server::beginAccepting()
{
	acceptor.async_accept(
		[this](boost::system::error_code errcode,boost::asio::ip::tcp::socket gsock) {
			handleAcceptedClient(std::move(gsock),errcode);
		}
	);
}

void Server::handleAcceptedClient(tcp::socket &&client, const boost::system::error_code &error)
{
	if(!error) {
		clients.emplace_front(std::forward<tcp::socket>(client));
		auto reference = clients.begin();
		beginReading(reference);
	}
	beginAccepting();
}

void Server::beginReading(Server::ClientIterator it)
{
	/*boost::asio::async_read(it->sock,boost::asio::buffer(it->buff.data(),it->buff.size()),
					  [this,it](const boost::system::error_code& ec,size_t len) {
						  onReadBuffer(it,ec,len,this->myfile);
						  return size_t(it->buff.size() - len);
					  }
					  );*/
	/*boost::asio::async_read_until(it->sock,boost::asio::dynamic_buffer(it->buff),0x7F,
							[this,it](boost::system::error_code ec,size_t len) {
								onReadBuffer(it,ec,len);
							}
	);*/
	boost::asio::async_read_until(
		it->sock, boost::asio::dynamic_buffer(it->buff), 0x7E,[this,it](boost::system::error_code ec, size_t len){
			onReadBuffer(it, ec, len);
		}
		);
}

void Server::onReadBuffer(Server::ClientIterator it, const boost::system::error_code& ec, size_t len){
	if(!ec) {
		std::cout << "Received data of length " << len << std::endl;
		auto start = it->buff.begin();
		auto end = start;
		std::advance(end, len-1);

		if(it->between) {
			std::basic_string<uint8_t> res(start, end);
			res = uint8_t(0x7E) + res + uint8_t(0x7E);
			myfile << std::hex;
			myfile << "Input: ";
			for(const auto& character : res)
			{
				myfile << std::hex << std::setfill('0') << std::setw(2) << int(character) << " ";
			}
			myfile << std::endl;
			myfile << "Output: ";
			std::reverse(res.begin(), res.end());
			for(const auto& character : res)
			{
				myfile << std::hex << std::setfill('0') << std::setw(2) << int(character) << " ";
			}
			myfile << std::endl;

			boost::asio::write(it->sock, boost::asio::buffer( res ));
			it->between = false;
		} else {
			std::string res(start, end);
			boost::asio::write(it->sock, boost::asio::buffer( res ));
			it->between = true;
		}
		it->buff.erase(start, end);
		it->buff.erase(std::begin(it->buff),std::begin(it->buff)+1);
	} else {
		switch(ec.value()) {
		case boost::asio::error::eof:
			it->sock.close();
			clients.erase(it);
			return;
		case boost::asio::error::operation_aborted:
			clients.erase(it);
			return;
		default:
			if(!it->sock.is_open()) {
				clients.erase(it);
			}
		}
	}

	beginReading(it);
}

void Server::run()
{
	acceptor.listen(5);
	beginAccepting();
}

}
