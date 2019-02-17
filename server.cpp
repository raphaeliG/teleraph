#include "teleraph.h"

net::Server::Server(std::string portNumber)
{
	listening = false;
	{
		std::lock_guard<std::mutex> lock(mute);
		logger.start("server-log_" + portNumber + ".log");
	}

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		{
			std::lock_guard<std::mutex> lock(mute);
			logger << "WSAStartup failed: " << iResult << "\n";
		}
		usable = false;
		return;
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, portNumber.c_str(), &hints, &result);
	if (iResult)
	{
		{
			std::lock_guard<std::mutex> lock(mute);
			logger << "getaddrinfo failed: " << iResult << "\n";
		}
		WSACleanup();
		usable = false;
		return;
	}

	localData = result;

	// Create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET)
	{
		{
			std::lock_guard<std::mutex> lock(mute);
			logger << "Error at socket(): " << WSAGetLastError() << "\n";
		}
		freeaddrinfo(result);
		localData = NULL;
		WSACleanup();
		usable = false;
		return;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		{
			std::lock_guard<std::mutex> lock(mute);
			logger << "bind failed with error: " << WSAGetLastError() << "\n";
		}
		freeaddrinfo(result);
		localData = NULL;
		closesocket(ListenSocket);
		WSACleanup();
		usable = false;
		return;
	}

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		{
			std::lock_guard<std::mutex> lock(mute);
			logger << "Listen failed with error: " << WSAGetLastError() << "\n";
		}
		freeaddrinfo(result);
		localData = NULL;
		closesocket(ListenSocket);
		WSACleanup();
		usable = false;
		return;
	}
	usable = true;
}

net::Server::~Server()
{
	if (usable)
	{
		stop_listening();
		wait_for_clients();
		if (localData)
			freeaddrinfo(localData);
		WSACleanup();
		logger.close();
		usable = false;
	}
}

void net::accept_client(net::Server& server)
{
	if (server.usable)
	{
		// Accept a client socket
		SOCKET ClientSocket = accept(server.ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET)
		{
			std::lock_guard<std::mutex> lock(server.mute);
			int e = WSAGetLastError();
			server.logger << "accept failed: " << e << "\n";
			switch (e)
			{
			case WSAEINTR:
				server.logger << 'E' << e << ": server stopped listening to incoming connections while an accept request was open\n";
				break;
			default:
				server.logger << 'E' << e << ": unkown error message.\n";
			}
		}
		else
		{
			std::lock_guard<std::mutex> lock(server.mute);
			server.sockets.push_back(ClientSocket);
		}
	}
}

void net::Server::add_client()
{
	if (usable)
	{
		std::thread thread(std::thread(accept_client, std::ref(*this)));
		join_threads.push_back(std::move(thread));
	}
}

bool net::Server::is_usable() const
{
	return usable;
}

void net::Server::stop_listening()
{
	if (usable && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = INVALID_SOCKET;
	}
}

void net::Server::wait_for_clients()
{
	if (usable)
	{
		bool empty = false;
		while (!empty)
		{
			if (!join_threads.empty())
			{
				if (join_threads[0].joinable())
				{
					join_threads[0].join();
				}
				join_threads.erase(join_threads.begin());
			}
			else
				empty = true;
		}
	}
}
