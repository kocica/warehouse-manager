/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerSLAP.cpp
 * @date    11/19/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using classical storage location assignment problem
 */

#ifdef WHM_OPT

// Std
#include <map>
#include <set>
#include <utility>
#include <iostream>
#include <algorithm>

// Local
#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseSimulator.h"
#include "WarehousePathFinder.h"
#include "WarehouseLocationRack.h"
#include "WarehouseOptimizerSLAP.h"

namespace whm
{
    WarehouseOptimizerSLAP_t::WarehouseOptimizerSLAP_t(const utils::WhmArgs_t& args_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        init();
    }

    WarehouseOptimizerSLAP_t::WarehouseOptimizerSLAP_t(const utils::WhmArgs_t& args_, const ConfigParser_t& cfg_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        cfg = cfg_;
        init();
    }

    void WarehouseOptimizerSLAP_t::init()
    {
        pathFinder.precalculatePaths(whm::WarehouseLayout_t::getWhLayout().getWhItems());
        pathFinder.dump();
    }

    std::vector<int32_t> WarehouseOptimizerSLAP_t::precalculateSlotHeats()
    {
        std::vector<int32_t> sortedLocationEncs;
        std::vector<std::pair<WarehouseLocationSlot_t*, int32_t>> locationHeatsMap;

        for(const auto* whItem : whm::WarehouseLayout_t::getWhLayout().getWhItems())
        {
            if(whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
            {
                int32_t distance = calculateDistanceToExits(whItem->getWhItemID());

                auto* whRack = whItem->getWhLocationRack();

                for(int32_t y = 0; y < whRack->getSlotCountY(); y++)
                {
                    for(int32_t x = 0; x < whRack->getSlotCountX(); x++)
                    {
                        locationHeatsMap.push_back(std::make_pair(&whRack->at(x, y), distance + x + y));
                    }
                }
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

        return sortedLocationEncs;
    }

    std::vector<int32_t> WarehouseOptimizerSLAP_t::precalculateArticleWeights()
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

    WarehouseItem_t* WarehouseOptimizerSLAP_t::lookupExit(WarehouseItemType_t exitType)
    {
        for(WarehouseItem_t* i : whm::WarehouseLayout_t::getWhLayout().getWhItems())
        {
            if(i->getType() == exitType)
            {
                return i;
            }
        }

        whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Warehouse layout has no exit!");
        return nullptr;
    }

    int32_t WarehouseOptimizerSLAP_t::calculateDistanceToExits(int32_t locID)
    {
        int32_t totalDistance{ 0 };
        int32_t entranceID = lookupExit(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)->getWhItemID();
        int32_t dispatchID = lookupExit(WarehouseItemType_t::E_WAREHOUSE_DISPATCH)->getWhItemID();

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

    void WarehouseOptimizerSLAP_t::optimize()
    {
        std::vector<Solution_t> population(1);
        auto& genes = population[0].genes;
        auto& fitness = population[0].fitness;

        auto sortedArticleEnc = precalculateArticleWeights();
        auto sortedLocationEnc = precalculateSlotHeats();

        genes = std::vector<int32_t>(cfg.getAs<int32_t>("numberDimensions"));

        for(int32_t i = 0; i < cfg.getAs<int32_t>("numberDimensions"); ++i)
        {
            genes.at(sortedArticleEnc.at(i)) = sortedLocationEnc.at(i);
        }

        fitness = simulateWarehouse(genes);

        updateAllocations(genes);

        for(int32_t gen = 0; gen < cfg.getAs<int32_t>("maxIterations"); ++gen)
        {
            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "[SLAP] [%3d] Best fitness: %f", gen, fitness);
            histFitness.push_back(fitness);

#           ifdef WHM_GUI
            if(uiCallback)
            {
                uiCallback(fitness);
            }
#           endif
        }

        saveFitnessPlot();
        saveBestSolution(genes);
    }
}

#endif