#include "../header/Logger.hpp"

#include <iostream>

Logger::Logger(const LogLevel level, const bool rewriteLine) : level_(level), rewriteLine_(rewriteLine)
{
}

Logger::~Logger()
{
#ifdef NDEBUG
	if (level_ == DEBUG)
	{
		return;
	}
#endif

	if (rewriteLine_)
	{
		std::cerr << "\r";
	}
	else
	{
		std::cerr << std::endl;
	}

	switch (level_)
	{
		case DEBUG:
			std::cerr << "[DEBUG] ";
			break;
		case INFO:
			std::cerr << "[INFO ] ";
			break;
		case WARNING:
			std::cerr << "[WARN ] ";
			break;
		case ERROR:
			std::cerr << "[ERROR] ";
			break;
	}

	std::cerr << stream_.str();
}
