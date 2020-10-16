/**
 * Warehouse manager
 *
 * @file    Logger.cpp
 * @date    10/16/2020
 * @author  Filip Kocica
 * @brief   Simple logging module
 */

#include "Logger.h"

namespace
{
    using namespace whm::Color;
    using whm::Color::Modifier;

    Modifier r(FG_RED);
    Modifier b(FG_BLUE);
    Modifier c(FG_CYAN);
    Modifier g(FG_GREEN);
    Modifier y(FG_YELLOW);
    Modifier d(FG_DEFAULT);
}

namespace whm
{
    Logger_t::Logger_t()
        : os{ &std::cout }
    {

    }

    Logger_t::~Logger_t()
    {

    }

    Logger_t& Logger_t::getLogger()
    {
        static Logger_t l;
        return l;
    }

    void Logger_t::setStream(std::ostream& os_)
    {
        os = &os_;
    }

    void Logger_t::print(const char* file,
                         const char* func,
                         uint32_t    line,
                         LogLevel_t  level,
                         const char* format,
                         ...)
    {
        va_list args;

        va_start(args, format);

        char buffer[1024];
        vsnprintf(buffer, 1024, format, args);

        static auto ts = std::put_time(std::localtime(&time_now), "%y-%m-%d %OH:%OM:%OS");

        *os << b << "<" << ts << ">" << lvlStr(level) << c << file << " (" << func << "():" << line << ") >> " << d << buffer << std::endl;

        va_end(args);
    }

    const char* Logger_t::lvlStr(const LogLevel_t& level)
    {
        switch(level)
        {
            case LogLevel_t::E_DEBUG:
            {
                return "\033[1;32m [INFO] \033[0m";
            }
            case LogLevel_t::E_WARNING:
            {
                return "\033[1;33m [WARNING] \033[0m";
            }
            case LogLevel_t::E_ERROR:
            {
                return "\033[1;31m [ERROR] \033[0m";
            }
        }

        return "";
    }
}
