/**
 * Warehouse manager
 *
 * @file    Logger.h
 * @date    10/16/2020
 * @author  Filip Kocica
 * @brief   Simple logging module
 */

#pragma once

#include <ctime>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <stdarg.h>

#define LOG_LOC __FILE__, &__FUNCTION__[ 0 ], __LINE__

static std::time_t time_now = std::time(nullptr);

namespace whm
{
    enum class LogLevel_t
    {
        E_DEBUG,
        E_WARNING,
        E_ERROR
    };

    class Logger_t
    {
        public:
            Logger_t();
            ~Logger_t();

            static Logger_t& getLogger();

            void setVerbose(bool);
            bool isVerbose() const;

            void setStream(std::ostream&);

            const char* lvlStr(const LogLevel_t&);

            void print(const char*,
                       const char*,
                       uint32_t,
                       LogLevel_t,
                       const char*, ...);

        private:
            bool verbose{ false };
            std::ostream* os{ nullptr };
    };

    ///
    /// @date Jul 4 '13 at 11:59
    /// @author https://stackoverflow.com/users/939402/joel-sj%C3%B6gren - Joel Sjögren
    /// @link Q https://stackoverflow.com/q/2616906/8254699
    /// @link A https://stackoverflow.com/a/17469726/8254699
    ///
    namespace Color
    {
        enum Code
        {
            FG_RED      = 31,
            FG_GREEN    = 32,
            FG_BLUE     = 34,
            FG_DEFAULT  = 39,
            FG_YELLOW   = 33,
            FG_CYAN     = 36,
            BG_RED      = 41,
            BG_GREEN    = 42,
            BG_BLUE     = 44,
            BG_DEFAULT  = 49
        };

        class Modifier
        {
            private:
                Code code;
            public:
                Modifier(Code c) : code(c) {}

                friend std::ostream& operator<<(std::ostream& os, const Modifier& mod)
                {
                    return os << "\033[" << mod.code << "m";
                }
        };
    }
}
