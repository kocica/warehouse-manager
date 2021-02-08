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
#include <sstream>
#include <iostream>
#include <iterator>
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
            int32_t optimizer{ 0 };      //< Optimizer to be used
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
         * @brief Prints contents of vector
         */
        template <typename T>
        std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
        {
            if(!v.empty())
            {
                out << '[';
                std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
                out << "\b\b]";
            }

            return out;
        }

        /**
         * @brief Create string from standard vector
         */
        template <typename T>
        std::string toString(std::vector<T>& vec)
        {
            std::stringstream ss;

            for(auto v : vec)
            {
                ss << v;
            }

            return ss.str();
        }

        /**
         * @brief Return true, if vectors intersects, false otherwise
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
         * @brief Return true, if vector contains value, false otherwise
         */
        template<typename T>
        bool contains(std::vector<T> const& vec, T val)
        {
            return std::count(std::begin(vec), std::end(vec), val) != 0;
        }

        /**
         * @brief Convert generic string to lowercase
         */
        std::string toLower(std::string s);
    }
}