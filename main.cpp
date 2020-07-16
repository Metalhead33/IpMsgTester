#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "StdStream.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>

using namespace std;
using boost::asio::ip::tcp;

enum CmdId :uint8_t {
	LISTENER_IP=0,
	LISTENER_PORT,
	JOURNAL_PATH,
	NONE
};

const std::string WrongFormat = "Invalid output detected!\n";

CmdId getIdentifierType(const char* argument);
bool isIpAddress(const char *string);
bool isPort(const char* string, int* portDest);
int main(int argc, char *argv[])
{
	std::vector<uint8_t> buff(256);
	stringstream outputter;
	boost::asio::io_context io_context;
	tcp::resolver resolver(io_context);
	outputter.str("");
	// Wrong number of arguments
	if(argc < 7)
	{
		cout << "This program requires six command line arguments.\n"
			"Correct usage of this program is:\n"
			"-ip <IP address of the listener>\n"
			"-p <Port of the listener>\n"
			"-j <path to the journal file>\n"
			"These arguments can be given in any order, but all of them must be given.\n";
		return 0;
	}
	else {
		char* identifiers[3] = { nullptr, nullptr, nullptr };
		// We see which one of the command line arguments represents what.
		CmdId caughtIdentifier = getIdentifierType(argv[1]);
		if(caughtIdentifier != CmdId::NONE) identifiers[caughtIdentifier] = argv[2];
		caughtIdentifier = getIdentifierType(argv[3]);
		if(caughtIdentifier != CmdId::NONE) identifiers[caughtIdentifier] = argv[4];
		caughtIdentifier = getIdentifierType(argv[5]);
		if(caughtIdentifier != CmdId::NONE) identifiers[caughtIdentifier] = argv[6];
		// Now we validate.
		if(!( identifiers[0] && identifiers[1] && identifiers[2]))
		{
			cout << "Something went wrong during the processing of your command-line arguments. You probably entered them in a wrong order.\n";
			return 0;
		}
		if( getIdentifierType(identifiers[0]) != CmdId::NONE || getIdentifierType(identifiers[1]) != CmdId::NONE || getIdentifierType(identifiers[2]) != CmdId::NONE)
		{
			cout << "Something went wrong during the processing of your command-line arguments. You probably entered them in a wrong order.\n";
			return 0;
		}
		// Okay, so all arguments were given, now it's time to check if they are in the proper format.
		int portAddr;
		if(!isIpAddress(identifiers[CmdId::LISTENER_IP]))
		{
			cout << "Listener's IP address is in incorrect format: " << identifiers[CmdId::LISTENER_IP] << "\n"
				"A proper format is [0-255].[0-255].[0-255].[0-255] - \n"
				"In other words - four numbers between 0 and 255, spearated by a dot.\n";
			return 0;
		}
		if(!isPort(identifiers[CmdId::LISTENER_PORT],&portAddr))
		{
			cout << "Listener's port is in incorrect format:" << identifiers[CmdId::LISTENER_PORT] << "\n"
				"A proper format would be a number - any number between 0 and 65535.\n";
			return 0;
		}
		// Okay, everything should be in order.
		StdStream logfile(identifiers[CmdId::JOURNAL_PATH],false,true);
		tcp::resolver::results_type endpoints = resolver.resolve(identifiers[CmdId::LISTENER_IP], identifiers[CmdId::LISTENER_PORT]);
		/*std::vector<boost::asio::ip::tcp::endpoint> endpoints;
		endpoints.push_back(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(identifiers[CmdId::LISTENER_IP]),portAddr));*/
		tcp::socket socket(io_context);
		boost::asio::connect(socket, endpoints);
		while(true)
		{
			const long messageReceipt = socket.read_some(boost::asio::buffer(buff.data(),buff.size()));
			if(messageReceipt > 0 ) {
			std::vector<uint8_t>::iterator first=buff.end(),last=buff.end(); bool checkedOnce=false;
			for(auto it = std::begin(buff); it != std::end(buff); ++it)
			{
				if(*it == 0x7E)
				{
					if(checkedOnce)
					{
						last = it;
						break;
					}
					else {
						first = it;
						checkedOnce = true;
					}
				}
			}
			if( (first == std::end(buff)) || (last == std::end(buff)) )
			{
				logfile.write(WrongFormat.data(),WrongFormat.size()-1);
			} else {
				outputter << "Received bytes:\n";
				for(auto it = first; it != last; ++it)
					outputter << hex << setfill('0') << setw(2) << int(*it) << " ";
				outputter << "7e\n";
				std::reverse(first,last);
				outputter << "Sent bytes:\n";
				for(auto it = first; it != last; ++it)
					outputter << hex << setfill('0') << setw(2) << int(*it) << " ";
				outputter << "7e\n";
				const auto tmpstr = outputter.str();
				outputter.str("");
				logfile.write(tmpstr.data(),tmpstr.size()-1);
				socket.write_some(boost::asio::buffer(&*first,std::distance(first,last)));
			}
		}
		}
	}
	return 0;
}
CmdId getIdentifierType(const char* argument)
{
	if(!strcmp(argument,"-ip"))
		return CmdId::LISTENER_IP;
	else if(!strcmp(argument,"-p"))
		return CmdId::LISTENER_PORT;
	else if(!strcmp(argument,"-j"))
		return CmdId::JOURNAL_PATH;
	else return CmdId::NONE;
}
bool isIpAddress(const char* string)
{
	bool wasDot=true;
	for(;*string;++string)
	{
		if(wasDot && !isdigit(*string)) return false;
		else wasDot = (*string == '.');
	}
	return true;
}
bool isPort(const char* string, int *portDest)
{
	const int converted = atoi(string);
	for(;*string;++string)
	{
		if(!isdigit(*string)) return false;
	}
	if(converted <= 0xFFFF) {
		*portDest = converted;
		return true;
	}
	else return false;
}
