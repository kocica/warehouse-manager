/**
 * Warehouse manager
 *
 * @file    Utils.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Utilities module
 */

#include "Utils.h"
#include "Logger.h"

namespace whm
{
    namespace utils
    {
        void printHelp()
        {
            std::cout << "\nUsage: ./warehouse_manager\n\n"
                         "\t-h\tPrints this usage\n"
                         "\t-v\tTriggers verbose mode\n"
                         "\t-o\tPath to a file with customer orders\n"
                         "\t-a\tPath to a file with articles\n"
                         "\t-i\tPath to a file with article-slot allocation\n"

                         "\t-r\tRealistic simulation (seizing facilities, ...)\n"
                         "\t-p\tPreprocess (optimize) orders\n"
                         "\t-s\tTote speed on conveyor\n"
                         "\t-w\tWorker speed during picking\n"
                         "\t-t\tDispatched totes per minute\n"
                         "\t-f\tSpeed up / Slow down the simulation by this factor\n"
                         "\t-l\tPath to a file with warehouse layout description\n"
                        ;
        }

        WhmArgs_t parseArgs(int argc, char *argv[])
        {
            int c;
            WhmArgs_t args;

            // TODO: Handle catch invalid_argument exception and missing arguments

            while ((c = getopt (argc, argv, "s:w:t:f:o:a:i:l:vpr")) != -1)
            {
                switch(c)
                {
                    case 'o':
                        args.ordersPath = optarg;
                        break;
                    case 'a':
                        args.articlesPath = optarg;
                        break;
                    case 'i':
                        args.locationsPath = optarg;
                        break;

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
                    case 'l':
                        args.layoutPath = optarg;
                        break;
                    case 'p':
                        args.preprocess = true;
                        break;
                    case 'r':
                        args.realistic = true;
                        break;
                    case 'v':
                        whm::Logger_t::getLogger().setVerbose(true);
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
