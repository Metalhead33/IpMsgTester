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
	acceptor.listen(5);
	while(true) {
	acceptor.async_accept(
		[this](boost::system::error_code errcode,boost::asio::ip::tcp::socket gsock) {
			handleAcceptedClient(std::move(gsock),errcode);
		}
	);
	}
}

void Server::handleAcceptedClient(tcp::socket &&client, const boost::system::error_code &error)
{
	if(!error) {
		clients.emplace_front(std::forward<tcp::socket>(client));
		auto reference = clients.begin();
		onAcceptedPeer(reference);
	}
	beginAccepting();
}

void Server::onAcceptedPeer(Server::ClientIterator it)
{
	boost::asio::read(it->sock,boost::asio::buffer(it->buff.data(),it->buff.size()),
					  [this,it](const boost::system::error_code& ec,size_t len) {
						  onReadBuffer(it,ec,len,this->myfile);
						  return size_t(it->buff.size() - len);
					  }
					  );
	/*boost::asio::async_read_until(it->sock,boost::asio::dynamic_buffer(it->buff),0x7F,
							[this,it](boost::system::error_code ec,size_t len) {
								onReadBuffer(it,ec,len);
							}
	);*/
}

void Server::onReadBuffer(Server::ClientIterator it, const boost::system::error_code& ec, size_t len, std::ofstream &out)
{
	std::cout << "I got it!" << len << std::endl;
	if(!ec) {
		Client::Iterator first=it->buff.end(),last=it->buff.end(); bool checkedOnce=false;
		for(auto zt = std::begin(it->buff); zt != std::end(it->buff); ++zt)
			{
				if(*zt == 0x7E)
				{
					if(checkedOnce)
					{
						last = zt;
						++last;
						break;
					}
					else {
						first = zt;
						checkedOnce = true;
					}
				}
			}
			if( (first == std::end(it->buff)) || (last == std::end(it->buff)) )
			{
				myfile << "Invalid output detected!\n";
			} else {
				it->outputter << "Received bytes:\n";
				for(auto dt = first; dt != last; ++dt)
					it->outputter << std::hex << std::setfill('0') << std::setw(2) << int(*dt) << " ";
				std::reverse(first,last);
				it->outputter << "Sent bytes:\n";
				for(auto dt = first; dt != last; ++dt)
					it->outputter << std::hex << std::setfill('0') << std::setw(2) << int(*dt) << " ";
				myfile << it->outputter.str();
				it->outputter.str("");
				boost::asio::write(it->sock,boost::asio::buffer(&*first,std::distance(first,last)));
			} } else switch(ec.value()) {
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
		} onAcceptedPeer(it);
}

}
