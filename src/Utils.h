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
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

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
            bool verbose{ false };       //< Print debug to an output stream
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
            bool preprocess{ false };    //< Preprocess (optimize) orders before starting/picking them
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
    }
}