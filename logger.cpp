#include "teleraph.h"

net::Logger::Logger(std::string logName) : logName(logName), usable(true)
{
	std::ofstream temp(logName);
	if (temp.is_open())
		temp.close();
}

net::Logger::~Logger()
{
	usable = false;
	if (log.is_open())
		log.close();
}

void net::Logger::start(std::string logName)
{
	if (log.is_open())
		log.close();
	this->logName = logName;
	std::ofstream temp(logName);
	if (temp.is_open())
		temp.close();
	usable = true;
}