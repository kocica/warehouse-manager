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
#include <algorithm>

// Local
#include "Logger.h"
#include "matplotlibcpp.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseSimulator.h"
#include "WarehouseLocationRack.h"
#include "WarehouseOptimizerBase.h"

namespace whm
{
    WarehouseOptimizerBase_t::WarehouseOptimizerBase_t(utils::WhmArgs_t args_)
        : cfg{ ConfigParser_t{ "cfg/optimizer.xml" } }
        , args{ args_ }
    {
        whm::WarehouseSimulator_t::getWhSimulator().printStats(false);

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
    }

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
        whm::WarehouseSimulator_t::getWhSimulator().setArguments(args);
        return whm::WarehouseSimulator_t::getWhSimulator().runSimulation();
    }

    void WarehouseOptimizerBase_t::saveFitnessPlot() const
    {
        // Plot fitness convergence or just dump to output
        if(cfg.isSet("plotPath"))
        {
            matplotlibcpp::plot(histFitness);
            matplotlibcpp::save(cfg.getAs<std::string>("plotPath"));
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
}

#endif
