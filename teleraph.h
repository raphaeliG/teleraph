#ifndef TELERAPH_H
#define TELERAPH_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "logging.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

namespace net
{
	typedef char byte;
	class Server
	{
	private:
		bool usable;
		WSADATA wsaData;
		SOCKET ListenSocket;
		std::vector<SOCKET> sockets;
		std::vector<std::thread> join_threads;
		std::vector<std::string> addresses;
		std::mutex mute;
		Logger logger;
		addrinfo* localData;
	public:
		/*
		Starts a server on the local machine, with a given port number.
		if any error occurs, the server will become unusable, and a proper error message will be sent to a log file.
		parameters:
		portNumber - the port number, given as a string. best when set to above 10000.
		*/
		Server(std::string portNumber);
		~Server();
		void add_client();//Non-blocking function to add a client.
		bool is_usable() const;//Determines whether the server is usable
		void stop_listening();
		void restart_listening();
		void wait_for_clients();//Pauses the thread this function is called from untill all existing join threads are completed
		std::vector<std::string> get_addresses() const;//Returns a vector containing all possible IPv4 addresses of the server
		friend void accept_client(Server& server);//Blocking function to add a client.
	};

	class Client;
}

#endif