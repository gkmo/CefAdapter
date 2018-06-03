#pragma once

#include <fstream>

class Logger 
{
    public:
        Logger(const std::string filename);
        ~Logger();

        void Debug(const char* category, const char* message);

    private:
        std::ofstream _file;
};
