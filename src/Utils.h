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
            float toteSpeed{ 0.5 };      //< How fast tote moves on conv; default 0.5 [m/s]
            float pickerSpeed{ 0.5 };    //< How fast picker performs material handling; default x [x]
            float ratio{ 0.0 };          //< Ratio to real world size; defailt x [x]
            float errorRate{ 0.1 };      //< Rate with which pickers do a mistake
            std::string ordersPath;      //< Path to a file with customer orders
            std::string productsPath;    //< Path to a file with article-slot allocation
            std::string layoutPath;      //< Path to a file with warehouse layout description
            bool verbose{ false };       //< Print debug to an output stream
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