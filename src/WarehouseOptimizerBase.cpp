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
#include <set>
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
    WarehouseOptimizerBase_t::WarehouseOptimizerBase_t(const utils::WhmArgs_t& args_)
        : cfg{ ConfigParser_t{ "cfg/optimizer.xml" } }
        , args{ args_ }
    {
        init();
    }

    WarehouseOptimizerBase_t::WarehouseOptimizerBase_t(const utils::WhmArgs_t& args_, const ConfigParser_t& cfg_)
        : cfg{ cfg_ }
        , args{ args_ }
    {
        init();
    }

    void WarehouseOptimizerBase_t::init()
    {
        simulator.printStats(false);
        simulator.setArguments(args);
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

        for(auto i = cfg.getAs<int32_t>("problemMin"); i < cfg.getAs<int32_t>("problemMax"); ++i)
        {
            randChromosome.push_back(i);
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
        /*for(int32_t d = 0; d < cfg.getAs<int32_t>("numberDimensions"); ++d)
        {
            int32_t r{ 0 };

            do
            {
                r = randomFromInterval(cfg.getAs<int32_t>("problemMin"), cfg.getAs<int32_t>("problemMax"));
            }
            while(std::find(ind.begin(), ind.end(), r) != ind.end());

            ind.push_back(r);
        }*/

        std::shuffle(randChromosome.begin(), randChromosome.end(), rand);

        ind = std::vector<int32_t>(randChromosome.begin(),
                                   randChromosome.begin() + cfg.getAs<int32_t>("numberDimensions"));
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

    std::vector<int32_t> WarehouseOptimizerBase_t::precalculateArticleWeights()
    {
        std::vector<int32_t> sortedArticleEncs;
        std::map<std::string, int32_t> articleWeightsMap;

        for(auto& order : whm::WarehouseLayout_t::getWhLayout().getWhOrders())
        {
            for(auto& line : order.getWhOrderLines())
            {
                articleWeightsMap[line.getArticle()]++;
            }
        }

        using Comparator_t = std::function<bool(std::pair<std::string, int32_t>, std::pair<std::string, int32_t>)>;

        Comparator_t compFunctor = [](std::pair<std::string, int32_t> lhs, std::pair<std::string, int32_t> rhs) -> bool
                                   {
                                       return lhs.second >= rhs.second;
                                   };

        std::set<std::pair<std::string, int32_t>, Comparator_t> articleWeightsSet(articleWeightsMap.begin(), articleWeightsMap.end(), compFunctor);

        for(const auto& [a1, _] : articleWeightsSet)
        {
            for(const auto& [enc, a2] : skuEnc)
            {
                if(a1 == a2)
                {
                    sortedArticleEncs.push_back(enc);
                }
            }
        }

        return sortedArticleEncs;
    }

    int32_t WarehouseOptimizerBase_t::calculateDistanceToExits(int32_t locID)
    {
        int32_t totalDistance{ 0 };
        int32_t entranceID = simulator.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)->getWhItemID();
        int32_t dispatchID = simulator.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_DISPATCH)->getWhItemID();

        const WarehousePathInfo_t* shortestPathEntrance = pathFinder.getShortestPath(locID, entranceID);
        const WarehousePathInfo_t* shortestPathDispatch = pathFinder.getShortestPath(locID, dispatchID);

        for(const auto& [_, distance] : shortestPathEntrance->pathToTarget)
        {
            totalDistance += distance;
        }

        for(const auto& [_, distance] : shortestPathDispatch->pathToTarget)
        {
            totalDistance += distance;
        }

        return totalDistance;
    }

    std::vector<int32_t> WarehouseOptimizerBase_t::sortLocationArticles(std::vector<int32_t>& ind)
    {
        updateAllocations(ind);

        auto genes = std::vector<int32_t>(cfg.getAs<int32_t>("numberDimensions"));

        pathFinder.clearPrecalculatedPaths();
        pathFinder.precalculatePaths(whm::WarehouseLayout_t::getWhLayout().getWhItems());

        auto articleWeights = precalculateArticleWeights();

        for(auto* whItem : WarehouseLayout_t::getWhLayout().getWhItems())
        {
            if(whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
            {
                std::vector<int32_t> sortedLocationEncs;

                std::vector<std::pair<WarehouseLocationSlot_t*, int32_t>> locationHeatsMap;

                int32_t distance = calculateDistanceToExits(whItem->getWhItemID());

                auto* whRack = whItem->getWhLocationRack();

                for(int32_t y = 0; y < whRack->getSlotCountY(); y++)
                {
                    for(int32_t x = 0; x < whRack->getSlotCountX(); x++)
                    {
                        locationHeatsMap.push_back(std::make_pair(&whRack->at(x, y), distance + x + y));
                    }
                }

                std::sort(locationHeatsMap.begin(), locationHeatsMap.end(), [](auto& lhs, auto& rhs) -> bool
                                                                    {
                                                                        return lhs.second < rhs.second;
                                                                    });

                for(const auto& [s1, _] : locationHeatsMap)
                {
                    for(const auto& [enc, s2] : slotEnc)
                    {
                        if(s1 == s2)
                        {
                            sortedLocationEncs.push_back(enc);
                        }
                    }
                }

                std::vector<int32_t> articleEncs;

                for(int32_t locEnc : sortedLocationEncs)
                {
                    auto* slot = slotEnc[locEnc];

                    std::string article = slot->getArticle();

                    for(auto& sku : skuEnc)
                    {
                        if(sku.second == article)
                        {
                            articleEncs.push_back(sku.first);
                        }
                    }
                }

                std::vector<int32_t> sortedArticleEncs;

                for(int32_t aw : articleWeights)
                {
                    if(std::find(articleEncs.begin(), articleEncs.end(), aw) != articleEncs.end())
                    {
                        sortedArticleEncs.push_back(aw);
                    }
                }

                auto it = sortedLocationEncs.begin();

                for(int32_t e : sortedArticleEncs)
                {
                    genes.at(e) = *it;
                    ++ it;
                }
            }
        }

        return genes;
    }

    void WarehouseOptimizerBase_t::simulationService(int32_t infd, int32_t outfd)
    {
        std::vector<int32_t> ind(skuEnc.size());

        for(;;)
        {
            for(size_t i = 0; i < skuEnc.size(); ++i)
            {
                auto s = read(infd, &ind.at(i), sizeof(int32_t));

                if(s == 0)
                {
                    close(infd);
                    exit(0);
                }
                else if(s < (ssize_t)sizeof(int32_t))
                {
                    whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Read failed <%d>", errno);
                    throw std::runtime_error("Read failed");
                }
            }

            double fitness = simulateWarehouse(ind);

            auto s = write(outfd, &fitness, sizeof(double));

            if(s < (ssize_t)sizeof(double))
            {
                whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Write failed <%d>", errno);
                throw std::runtime_error("Write failed");
            }
        }
    }
}

#endif
