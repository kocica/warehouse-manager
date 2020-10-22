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
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <unordered_set>

namespace whm
{
    namespace utils
    {
        /**
         * @brief Plain old data filled with parsed arguments
         */
        struct WhmArgs_t
        {
            std::string ordersPath;      //< Path to a file with customer orders
            std::string articlesPath;    //< Path to a file with article-slot allocation
#ifdef WHM_GEN
            int32_t orderCount{ 1000 };  //< Number of generated orders; default 1000 orders per set
            double mi{ 10 };             //< Gauss distribution mi (mean value)
            double sigma{ 3 };           //< Gauss distribution sigma (standard deviation)
#else
            float toteSpeed{ 1.0 };      //< How fast tote moves on conv; default 0.5 [m/s]
            float workerSpeed{ 1.0 };    //< How fast picker performs material handling; default x [x]
            float totesPerMin{ 120.0 };  //< How many totes are dispatched in one minute; default 15 [totes/min]
            float speedup{ 1.0 };        //< If we want to fasten up/slower down the simulation
            float errorRate{ 0.1 };      //< Rate with which pickers do a mistake x [errors/min]
            std::string layoutPath;      //< Path to a file with warehouse layout description
            bool realistic{ false };     //< Triggers realistic wh simulation (seizing facilities, ...); disabled by default
            bool preprocess{ false };    //< Preprocess (optimize) orders before starting/picking them; disabled by default
#endif
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
    }
}