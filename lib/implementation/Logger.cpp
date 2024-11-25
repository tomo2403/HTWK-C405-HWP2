#include "../header/Logger.hpp"

Logger::Logger(LogLevel level, bool rewriteLine) : level_(level), rewriteLine_(rewriteLine)
{ }

Logger::~Logger()
{
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
