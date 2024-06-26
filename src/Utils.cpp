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
            std::cout << "\nUsage:\n\n"
                         "\t-h\tPrints this usage\n"
                         "\t-v\tTriggers verbose mode\n"
                         "\t-o\tPath to a file with customer orders\n"
                         "\t-a\tPath to a file with articles\n"
                         "\t-i\tPath to a file with article-slot allocation\n"
                         "\t-l\tPath to a file with warehouse layout description\n"
                         "\t-s\tShow aggregated statistics for path finder\n"
                         "\t-O\tOptimizer to be used: [ 1: GA | 2: DE | 3: ABC | 4:PSO | 5:SLAP | 6:RAND ]\n"
                        ;
        }

        WhmArgs_t parseArgs(int argc, char *argv[])
        {
            int c;
            WhmArgs_t args;

            while ((c = getopt (argc, argv, "o:a:i:l:O:sv")) != -1)
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
                    case 'l':
                        args.layoutPath = optarg;
                        break;
                    case 's':
                        args.stats = true;
                        break;
                    case 'O':
                        args.optimizer = std::stoi(optarg);
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

        std::string toLower(std::string s)
        {
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c)
                           -> unsigned char
                           {
                               return std::tolower(c);
                           });

            return s;
        }
    }
}
