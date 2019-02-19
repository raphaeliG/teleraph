#include "teleraph.h"

net::Client::Client(std::string portNumber, bool doesLog) : portNumber(portNumber), doesLog(doesLog)
{
	if (doesLog)
		logger.start("server-log_" + portNumber + ".log");

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		if (doesLog)
			logger << "WSAStartup failed: " << iResult << "\n";
		usable = false;
		return;
	}
}

net::Client::~Client()
{
	if (usable)
	{
		if (serverData)
			freeaddrinfo(serverData);
		WSACleanup();
		usable = false;
	}
}

bool net::Client::connect_to_server(std::string address)
{
	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(address.c_str(), portNumber.c_str(), &hints, &serverData);
	if (iResult != 0)
	{
		if (doesLog)
			logger << "getaddrinfo failed: " << iResult << '\n';
		return false;
	}


	// Attempt to connect to the first address returned by serverData
	SOCKET ConnectSocket = INVALID_SOCKET;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(serverData->ai_family, serverData->ai_socktype, serverData->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
	{
		if (doesLog)
			logger << "Error at socket(): " << WSAGetLastError() << '\n';
		return false;
	}

	// Connect to server.
	bool connected = false;
	for (addrinfo* ptr = serverData; ptr && !connected; ptr = ptr->ai_next)
	{
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}
		else
			connected = true;
	}

	if (ConnectSocket == INVALID_SOCKET || !connected)
	{
		if (doesLog)
			logger << "Unable to connect to server!\n";
		return false;
	}

	return true;
}
