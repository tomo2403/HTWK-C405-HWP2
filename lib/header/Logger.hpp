#pragma once

#include <sstream>

enum LogLevel
{
	DEBUG, INFO, WARNING, ERROR
};

class Logger
{
public:
	explicit Logger(LogLevel level, bool rewriteLine = false);

	~Logger();

	template<typename T>
	Logger &operator<<(const T &value)
	{
		stream_ << value;
		return *this;
	}

private:
	LogLevel level_;
	bool rewriteLine_;
	std::ostringstream stream_;
};

// Beispiel-Nutzung:
// Logger(DEBUG) << "Dies ist eine Debug-Nachricht mit Wert: " << 42;
