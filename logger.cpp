#include "teleraph.h"

net::Logger::Logger(std::string logName) : logName(logName)
{
	std::ofstream temp(logName);
	if (temp.is_open())
		temp.close();
}

net::Logger::~Logger()
{
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
}