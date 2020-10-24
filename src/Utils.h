/**
 * Warehouse manager
 *
 * @file    Utils.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Utilities module
 */

#pragma once

// Std
#include <vector>
#include <cctype>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <unordered_set>

namespace whm
{
    namespace utils
    {
        /**
         * @brief Plain old data filled with parsed arguments
         * 
         * @note Most of the parameters moved to each app configuration file in "cfg" directory
         */
        struct WhmArgs_t
        {
            std::string ordersPath;      //< Path to a file with customer orders
            std::string articlesPath;    //< Path to a file with articles
            std::string locationsPath;   //< Path to a file with article-slot allocation
            std::string layoutPath;      //< Path to a file with warehouse layout description
        };

        /**
         * @brief Parsing input arguments using getopt
         *
         * @param argc Argument count
         * @param argv Argument values
         * @return Parsed values
         */
        WhmArgs_t parseArgs(int argc, char *argv[]);

        /**
         * @brief Prints help/usage to the output stream
         */
        void printHelp();

        /**
         * @brief Return true, if vectors intersects
         */
        template<typename T>
        bool intersects(const std::vector<T>& lhs, const std::vector<T>& rhs)
        {
            std::unordered_set<T> set(lhs.begin(), lhs.end());

            for(auto i : rhs)
            {
                if(set.count(i))
                {
                    return true;
                }
            }

            return false;
        }

        /**
         * @brief Convert generic string to lowercase
         */
        std::string toLower(std::string s);
    }
}