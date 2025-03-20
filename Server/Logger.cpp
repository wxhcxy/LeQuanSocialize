

#include "Logger.h"
#include <stdarg.h>
#include <iostream>

Logger::Logger() {}

Logger& Logger::Instance()
{
	static Logger s_logger;
	return s_logger;
}

Logger::~Logger()
{

}


