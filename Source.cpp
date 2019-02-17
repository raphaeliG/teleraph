#include "teleraph.h"
#include <iostream>

using namespace net;

int main()
{
	Server server("36676");
	server.add_client();
	server.add_client();
	server.add_client();
	//server.wait_for_clients();
	std::cout << "done";
	return 0;
}