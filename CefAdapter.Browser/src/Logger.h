#pragma once

#include <fstream>

class Logger 
{
    public:
        Logger();
        ~Logger();

        void Info(const char* category, const char* message);
        void Warning(const char* category, const char* message);    
        void Error(const char* category, const char* message);
};
