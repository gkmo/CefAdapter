#include "Logger.h"
#include "include/base/cef_logging.h"

Logger::Logger()
{    
}

Logger::~Logger()
{    
}

void Logger::Info(const char* category, const char* message)
{
    LOG(INFO) << "[" << category << "] " << message;
}

void Logger::Warning(const char* category, const char* message)
{
    LOG(WARNING) << "[" << category << "] " << message;
}

void Logger::Error(const char* category, const char* message)
{
    LOG(ERROR) << "[" << category << "] " << message;
}