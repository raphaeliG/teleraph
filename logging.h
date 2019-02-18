#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

class Logger
{
private:
	std::string logName;
	std::fstream log;
	bool usable;
public:
	Logger() : usable(false) {}
	Logger(std::string logName) : logName(logName), usable(true)
	{
		std::ofstream temp(logName);
		if (temp.is_open())
			temp.close();
	}
	~Logger()
	{
		usable = false;
		if (log.is_open())
			log.close();
	}
	void start(std::string logName)
	{
		if (log.is_open())
			log.close();
		this->logName = logName;
		std::ofstream temp(logName);
		if (temp.is_open())
			temp.close();
		usable = true;
	}
	template <typename T>
	Logger& operator<<(T output)
	{
		if (usable)
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
		}
		return *this;
	}
};

#endif
