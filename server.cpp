#include "teleraph.h"

net::Server::Server(std::string portNumber, bool doesLog) : portNumber(portNumber), doesLog(doesLog)
{
	{
		std::lock_guard<std::mutex> lock(mute);
		if (doesLog)
			logger.start("server-log_" + portNumber + ".log");
	}

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		{
			std::lock_guard<std::mutex> lock(mute);
			if (doesLog)
				logger << "WSAStartup failed: " << iResult << "\n";
		}
		usable = false;
		return;
	}

	struct addrinfo *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, portNumber.c_str(), &hints, &localData);
	if (iResult)
	{
		{
			std::lock_guard<std::mutex> lock(mute);
			if (doesLog)
				logger << "getaddrinfo failed: " << iResult << "\n";
		}
		WSACleanup();
		usable = false;
		return;
	}

	DWORD rv, size;
	PIP_ADAPTER_ADDRESSES adapter_addresses, aa;
	PIP_ADAPTER_UNICAST_ADDRESS ua;
	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
	if (rv != ERROR_BUFFER_OVERFLOW) {
		addresses = std::vector<std::string>();
	}
	adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);
	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &size);
	if (rv != ERROR_SUCCESS) {
		free(adapter_addresses);
		addresses = std::vector<std::string>();
	}

	for (aa = adapter_addresses; aa != NULL; aa = aa->Next)
	{
		for (ua = aa->FirstUnicastAddress; ua != NULL; ua = ua->Next)
		{
			if (ua->Address.lpSockaddr->sa_family == AF_INET)
			{
				char buf[BUFSIZ] = { 0 };
				getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
				if ((std::string)buf != "127.0.0.1")
					addresses.push_back(buf);
			}
		}
	}
	free(adapter_addresses);

	usable = true;
	restart_listening();
}

net::Server::~Server()
{
	if (usable)
	{
		stop_listening();
		if (localData)
			freeaddrinfo(localData);
		WSACleanup();
		usable = false;
	}
}

void net::Server::accept_client(net::Server& server)
{
	if (server.usable)
	{
		// Accept a client socket
		SOCKET ClientSocket = accept(server.ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET)
		{
			std::lock_guard<std::mutex> lock(server.mute);
			int e = WSAGetLastError();
			if (server.doesLog)
				server.logger << "accept failed: " << e << "\n";
			switch (e)
			{
			case WSAEINTR:
				if (server.doesLog)
					server.logger << 'E' << e << ": server stopped listening to incoming connections while an accept request was open\n";
				break;
			default:
				if (server.doesLog)
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

void net::Server::stop_listening()
{
	if (usable)
	{
		if (ListenSocket != INVALID_SOCKET)
		{
			std::lock_guard<std::mutex> lock(mute);
			closesocket(ListenSocket);
			ListenSocket = INVALID_SOCKET;
		}
		wait_for_clients();
	}
}

void net::Server::restart_listening()
{
	if (usable)
	{
		stop_listening();
		wait_for_clients();
		// Create a SOCKET for the server to listen for client connections
		ListenSocket = socket(localData->ai_family, localData->ai_socktype, localData->ai_protocol);
		if (ListenSocket == INVALID_SOCKET)
		{
			{
				std::lock_guard<std::mutex> lock(mute);
				if (doesLog)
					logger << "Error at socket(): " << WSAGetLastError() << "\n";
			}
			return;
		}

		// Setup the TCP listening socket
		int iResult = bind(ListenSocket, localData->ai_addr, (int)localData->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			{
				std::lock_guard<std::mutex> lock(mute);
				if (doesLog)
					logger << "bind failed with error: " << WSAGetLastError() << "\n";
			}
			closesocket(ListenSocket);
			return;
		}

		if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			{
				std::lock_guard<std::mutex> lock(mute);
				if (doesLog)
					logger << "Listen failed with error: " << WSAGetLastError() << "\n";
			}
			closesocket(ListenSocket);
			return;
		}
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