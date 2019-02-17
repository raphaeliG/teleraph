#ifndef LOGGER_H
#define LOGGER_H
#include <fstream>
#include <string>
#include <mutex>

class Logger
{
private:
	std::mutex muteLog;
	std::string logName;
	std::fstream log;
	bool usable;
public:
	Logger() : usable(false) {}
	Logger(std::string logName) : logName(logName), usable(true)
	{
		std::lock_guard<std::mutex> lock(muteLog);
		std::ofstream temp(logName);
		if (temp.is_open())
			temp.close();
	}
	~Logger()
	{
		close();
	}
	void start(std::string logName)
	{
		std::lock_guard<std::mutex> lock(muteLog);
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
		std::lock_guard<std::mutex> lock(muteLog);
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
	void close()
	{
		std::lock_guard<std::mutex> lock(muteLog);
		usable = false;
	}
};

#endif