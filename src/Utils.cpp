/**
 * Warehouse manager
 *
 * @file    Utils.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Utilities module
 */

#include "Utils.h"

namespace whm
{
    namespace utils
    {
        ArgsParser_t::ArgsParser_t()
        {

        }

        ArgsParser_t::AppArgs_t ArgsParser_t::parseArgs(int argc, char *argv[])
        {
            int c;

            while ((c = getopt (argc, argv, "")) != -1)
            {
                switch(c)
                {
                    default:
                        exit(0);
                }
            }

            return m_args;
        }

        void ArgsParser_t::printHelp() const
        {
            std::cout << "\nUsage: ./warehouse_manager [...]\n\n"
                        "\t-h\tPrints usage\n" << std::endl;
        }
    }
}
