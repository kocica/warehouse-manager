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
        struct SimArgs_t
        {
            float toteSpeed{ 1.0 };      //< How fast tote moves on conv; default 0.5 [m/s]
            float workerSpeed{ 1.0 };    //< How fast picker performs material handling; default x [x]
            float totesPerMin{ 120.0 };  //< How many totes are dispatched in one minute; default 15 [totes/min]
            float speedup{ 1.0 };        //< If we want to fasten up/slower down the simulation
            float errorRate{ 0.1 };      //< Rate with which pickers do a mistake x [errors/min]
            std::string ordersPath;      //< Path to a file with customer orders
            std::string articlesPath;    //< Path to a file with article-slot allocation
            std::string layoutPath;      //< Path to a file with warehouse layout description
            bool verbose{ false };       //< Print debug to an output stream
            bool preprocess{ false };    //< Preprocess (optimize) orders before starting/picking them
        };

        /**
         * @brief Parsing input arguments using getopt
         *
         * @param argc Argument count
         * @param argv Argument values
         * @return Parsed values
         */
        SimArgs_t parseArgs(int argc, char *argv[]);

        /**
         * @brief Prints help/usage to the output stream
         */
        void printHelp();
    }
}