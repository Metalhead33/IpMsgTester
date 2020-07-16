#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include "TcpServer.hpp"


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
	boost::asio::io_context io_context;
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
		TCP::Server serv(io_context,identifiers[CmdId::JOURNAL_PATH],tcp::endpoint(tcp::v4(), portAddr));
		serv.beginAccepting();
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
