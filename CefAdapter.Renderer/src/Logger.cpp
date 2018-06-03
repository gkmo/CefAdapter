#include "Logger.h"

Logger::Logger(const std::string filename)
{
    _file.open(filename);
}

Logger::~Logger()
{
    _file.close();
}

void Logger::Debug(const char* category, const char* message)
{
    _file << "[DEBUG][" << category << "] " << message << std::endl;
}