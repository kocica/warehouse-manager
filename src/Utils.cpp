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
        void printHelp()
        {
            std::cout << "\nUsage: ./warehouse_manager\n\n"
                         "\t-h\tPrints usage\n"
                         "\t-s\tTote speed on conveyor\n" << std::endl;
        }

        SimArgs_t parseArgs(int argc, char *argv[])
        {
            int c;
            SimArgs_t args;

            while ((c = getopt (argc, argv, "s:o:p:l:v")) != -1)
            {
                switch(c)
                {
                    case 's':
                        args.toteSpeed = atof(optarg);
                        break;
                    case 'o':
                        args.ordersPath = optarg;
                        break;
                    case 'p':
                        args.productsPath = optarg;
                        break;
                    case 'l':
                        args.layoutPath = optarg;
                        break;
                    case 'v':
                        args.verbose = true; // TODO: whm::Logger_t::getLogger().setVerbose(); (maybe also file stream)
                        break;
                    case 'h':
                        printHelp();
                        exit(0);
                    default:
                        std::cerr << "Invalid arguments!" << std::endl;
                        printHelp();
                        exit(1);
                }
            }

            return args;
        }
    }
}
