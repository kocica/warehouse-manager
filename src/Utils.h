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
         * @brief Class parsing input arguments & provides check if they are valid
         */
        class ArgsParser_t
        {
        public:
            explicit ArgsParser_t();
            virtual ~ArgsParser_t() = default;

            /**
             * @brief Plain old data filled with parsed arguments
             */
            struct AppArgs_t
            {

            };

            /**
             * @brief Parsing input arguments using getopt
             *
             * @param argc [in] Number of arguments
             * @param argv [in] Argument values
             * @return Parsed arguments
             */
            AppArgs_t parseArgs(int argc, char *argv[]);

            /**
             * @brief Prints help/usage to the given stream
             */
            void printHelp() const;

        private:
            AppArgs_t m_args = {};
        };
    }
}