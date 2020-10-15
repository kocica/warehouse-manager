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
                         "\t-h\tPrints this usage\n"
                         "\t-v\tTriggers verbose mode\n"
                         "\t-p\tPreprocess (optimize) orders\n"
                         "\t-s\tTote speed on conveyor\n"
                         "\t-w\tWorker speed during picking\n"
                         "\t-t\tDispatched totes per minute\n"
                         "\t-f\tSpeed up / Slow down the simulation by this factor\n"
                         "\t-o\tPath to a file with customer orders\n"
                         "\t-l\tPath to a file with warehouse layout description\n"
                         "\t-a\tPath to a file with article-slot allocation\n";
        }

        SimArgs_t parseArgs(int argc, char *argv[])
        {
            int c;
            SimArgs_t args;

            while ((c = getopt (argc, argv, "s:w:t:f:o:a:l:vp")) != -1)
            {
                switch(c)
                {
                    case 's':
                        args.toteSpeed = atof(optarg);
                        break;
                    case 'w':
                        args.workerSpeed = atof(optarg);
                        break;
                    case 't':
                        args.totesPerMin = atof(optarg);
                        break;
                    case 'f':
                        args.speedup = atof(optarg);
                        break;
                    case 'o':
                        args.ordersPath = optarg;
                        break;
                    case 'a':
                        args.articlesPath = optarg;
                        break;
                    case 'l':
                        args.layoutPath = optarg;
                        break;
                    case 'v':
                        args.verbose = true; // TODO: whm::Logger_t::getLogger().setVerbose(); (maybe also file stream)
                        break;
                    case 'p':
                        args.preprocess = true;
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
