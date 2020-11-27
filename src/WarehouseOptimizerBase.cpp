/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerBase.cpp
 * @date    10/23/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse
 */

#ifdef WHM_OPT

// Std
#include <map>
#include <utility>
#include <iostream>
#include <signal.h>
#include <algorithm>

// Local
#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseLocationRack.h"
#include "WarehouseOptimizerBase.h"
#include "WarehouseSimulatorSIMLIB.h"

#ifdef WHM_PLOT
#include "matplotlibcpp.h"
#endif

namespace whm
{
    WarehouseOptimizerBase_t::WarehouseOptimizerBase_t(utils::WhmArgs_t args_)
        : cfg{ ConfigParser_t{ "cfg/optimizer.xml" } }
        , args{ args_ }
    {
        simulator.printStats(false);
        simulator.setArguments(args);
        simulator.preprocessOrders();
        simulator.optimizationModeActive() = true;

        std::random_device rd;
        rand.seed(rd());

        int32_t slotID{ 0 };
        int32_t articleID{ 0 };

        std::vector<std::string> articles;
        whm::WarehouseLayout_t::getWhLayout().importArticles(args.articlesPath, articles);

        for(auto& article : articles)
        {
            skuEnc.insert(std::make_pair(articleID++, article));
        }

        for(auto* item : whm::WarehouseLayout_t::getWhLayout().getWhItems())
        {
            if(item->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
            {
                for(auto* slot : item->getWhLocationRack()->getSortedSlots())
                {
                    slotEnc.insert(std::make_pair(slotID++, slot));
                }
            }
        }

        for(int32_t i = 0; i < cfg.getAs<int32_t>("procCount"); ++i)
        {
            int fd1[2];
            int fd2[2];

            if(pipe(fd1) || pipe(fd2))
            {
                whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Pipe failed");
                throw std::runtime_error("Pipe failed");
            }

            pid_t pid = fork();

            if(pid != 0)
            {
                close(fd1[0]);
                close(fd2[1]);

                SimProcess_t simProcess;

                simProcess.pid = pid;
                simProcess.infd = fd2[0];
                simProcess.outfd = fd1[1];

                simProcesses.push_back(simProcess);
            }
            else
            {
                close(fd1[1]);
                close(fd2[0]);

                simulationService(fd1[0], fd2[1]);
            }
        }
    }

    WarehouseOptimizerBase_t::~WarehouseOptimizerBase_t()
    {
        // Kill all simulation services
        for(auto& simProcess : simProcesses)
        {
            (void) kill(simProcess.pid, SIGINT);
        }
    }

#   ifdef WHM_GUI
    void WarehouseOptimizerBase_t::setUiCallback(UiCallback_t c)
    {
        this->uiCallback = c;
    }
#   endif

    bool WarehouseOptimizerBase_t::flipCoin(double prob)
    {
        return randomFromInterval(0, 1) < prob;
    }

    double WarehouseOptimizerBase_t::randomGauss(double mi, double sigma)
    {
        std::normal_distribution<double> dist(mi, sigma);
        return dist(rand);
    }

    double WarehouseOptimizerBase_t::randomFromInterval(double a, double b)
    {
        std::uniform_real_distribution<> dist(a, b);
        return dist(rand);
    }

    void WarehouseOptimizerBase_t::initIndividualRand(std::vector<int32_t>& ind)
    {
        for(int32_t d = 0; d < cfg.getAs<int32_t>("numberDimensions"); ++d)
        {
            int32_t r{ 0 };

            do
            {
                r = randomFromInterval(cfg.getAs<int32_t>("problemMin"), cfg.getAs<int32_t>("problemMax"));
            }
            while(std::find(ind.begin(), ind.end(), r) != ind.end());

            ind.push_back(r);
        }
    }

    void WarehouseOptimizerBase_t::initPopulationRand(std::vector<Solution_t>& pop)
    {
        for(int32_t p = 0; p < static_cast<int32_t>(pop.size()); ++p)
        {
            initIndividualRand(pop[p].genes);
        }
    }

    void WarehouseOptimizerBase_t::initPopulationWeights(std::vector<Solution_t>& pop)
    {
        std::vector<std::string> articles;
        whm::WarehouseLayout_t::getWhLayout().importArticles(args.articlesPath, articles);
        whm::WarehouseLayout_t::getWhLayout().importLocationSlots(cfg.getAs<std::string>("initialWeights"));

        for(auto& article : articles)
        {
            for(auto& slot : slotEnc)
            {
                if(article == slot.second->getArticle())
                {
                    pop[0].genes.push_back(slot.first);
                }
            }
        }

        for(int32_t p = 1; p < static_cast<int32_t>(pop.size()); ++p)
        {
            initIndividualRand(pop[p].genes);
        }
    }

    void WarehouseOptimizerBase_t::updateAllocations(const std::vector<int32_t>& ind) const
    {
        // Clear current allocations first
        for(auto& slot : slotEnc)
        {
            slot.second->setArticle(std::string());
        }

        // Set new allocations
        for(int32_t i = 0; i < static_cast<int32_t>(ind.size()); ++i)
        {
            auto itSku  = skuEnc.find(i);
            auto itSlot = slotEnc.find(ind.at(i));

            itSlot->second->setArticle(itSku->second);
        }
    }

    double WarehouseOptimizerBase_t::simulateWarehouse(std::vector<int32_t>& ind)
    {
        // Update article/location allocation
        updateAllocations(ind);

        // Run simulation and return elapsed time
        return simulator.runSimulation();
    }

    void WarehouseOptimizerBase_t::saveFitnessPlot() const
    {
        // Plot fitness convergence or just dump to output
        if(cfg.isSet("plotPath"))
        {
#           ifdef WHM_PLOT
            matplotlibcpp::plot(histFitness);
            matplotlibcpp::save(cfg.getAs<std::string>("plotPath"));
#           endif
        }
        else
        {
            std::for_each(histFitness.begin(), histFitness.end(), [](double v){ std::cout << v << ", "; });
            std::cout << std::endl;
        }
    }

    void WarehouseOptimizerBase_t::saveBestSolution(std::vector<int32_t>& chromosome) const
    {
        // Update article/location allocation
        updateAllocations(chromosome);

        // Export allocation to a csv file
        whm::WarehouseLayout_t::getWhLayout().exportLocationSlots(args.locationsPath);
    }

    void WarehouseOptimizerBase_t::simulationService(int32_t infd, int32_t outfd)
    {
        std::vector<int32_t> ind(skuEnc.size());

        for(;;)
        {
            for(size_t i = 0; i < skuEnc.size(); ++i)
            {
                auto s = read(infd, &ind.at(i), sizeof(int32_t));

                if(s < 0)
                {
                    whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Read failed");
                    throw std::runtime_error("Read failed");
                }
                else if(s == 0)
                {
                    close(infd);
                    exit(0);
                }
            }

            double fitness = simulateWarehouse(ind);

            auto s = write(outfd, &fitness, sizeof(double));

            if(s < 0)
            {
                whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Write failed");
                throw std::runtime_error("Write failed");
            }
        }
    }
}

#endif
