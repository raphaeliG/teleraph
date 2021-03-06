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
#include <fstream>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

namespace net
{
	typedef char byte;

	enum PacketType { PROGRAM, SYSTEM };

	struct Packet
	{
		int packet_len;
		void* packet_data;
		PacketType packet_type;

	};

	class Logger
	{
	private:
		std::string logName;
		std::fstream log;
	public:
		Logger() {};
		Logger(std::string logName);
		~Logger();
		void start(std::string logName);
		template <typename T>
		Logger& operator<<(T output);
	};

	class Server
	{
	private:
		bool usable;
		bool doesLog;
		WSADATA wsaData;
		SOCKET ListenSocket;
		std::string portNumber;
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
		doesLog - decides whether the server will be using a log when it runs
		*/
		Server(std::string portNumber, bool doesLog = false);
		~Server();
		static void accept_client(Server& server);//Blocking function to add a client.
		void add_client();//Non-blocking function to add a client.
		bool is_usable() const { return usable; }//Determines whether the server is usable
		void stop_listening();
		void restart_listening();
		void wait_for_clients();//Pauses the thread this function is called from untill all existing join threads are completed
		std::vector<std::string> get_addresses() const { return addresses; }//Returns a vector containing all possible IPv4 addresses of the server
		std::string getPortNumber() const { return portNumber; }//Returns the port number of which the server is binded to
		void send_to_client(int clientNumber, const Packet& packet);
	};

	class Client
	{
	private:
		bool usable;
		bool doesLog;
		WSADATA wsaData;
		SOCKET connection;
		std::string portNumber;
		Logger logger;
		addrinfo* serverData;
	public:
		/*
		Starts a client on the local machine, with a given port number.
		if any error occurs, the client will become unusable, and a proper error message will be sent to a log file.
		parameters:
		portNumber - the port number, given as a string. best when set to above 10000.
		doesLog - decides whether the server will be using a log when it runs
		*/
		Client(std::string portNumber, bool doesLog = false);
		~Client();
		bool connect_to_server(std::string address);
		bool is_usable() const { return usable; }//Determines whether the server is usable
		std::string getPortNumber() const { return portNumber; }//Returns the port number of which the server is binded to
	};
}

template <typename T>
net::Logger& net::Logger::operator<<(T output)
{
	if (log.is_open())
	{
		log.close();
		log.open(logName, std::fstream::app | std::fstream::in | std::fstream::out);
		if (log.is_open())
		{
			log << output;
			log.close();
		}
	}
	else
	{
		log.open(logName, std::fstream::app | std::fstream::in | std::fstream::out);
		if (log.is_open())
		{
			log << output;
			log.close();
		}
	}
	return *this;
}

#endif