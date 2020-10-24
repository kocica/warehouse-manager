/**
 * Warehouse manager
 *
 * @file    ConfigParser.h
 * @date    10/24/2020
 * @author  Filip Kocica
 * @brief   Simple configuration parser module
 */

#pragma once

// Std
#include <map>

namespace whm
{
    class ConfigParser_t
    {
        public:
            ConfigParser_t(const std::string&);
            ~ConfigParser_t() = default;

            template<typename U>
            U getAs(const std::string&);

        protected:
            void dump() const;
            std::string findByName(const std::string&);

        private:
            std::map<std::string, std::string> parsedValues;
    };
}