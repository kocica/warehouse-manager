/**
 * Warehouse manager
 *
 * @file    WarehouseSimulator.cpp
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations
 */

// Std
#include <chrono>
#include <utility>
#include <iostream>
#include <algorithm>

// Local
#include "WarehouseItem.h"
#include "WarehouseSimulator.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    WarehouseSimulator_t::WarehouseSimulator_t()
        : whPathFinder{ new WarehousePathFinder_t() }
        , whLayout{ WarehouseLayout_t::getWhLayout() }
    {
        
    }

    WarehouseSimulator_t::~WarehouseSimulator_t()
    {
        delete whPathFinder;
    }

    void WarehouseSimulator_t::runSimulation()
    {
        whPathFinder->precalculatePaths(whLayout.getWhItems());
        whPathFinder->dump();

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        for(auto& whOrder : whLayout.getWhOrders()) { this->simulatePick(whOrder); }

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        std::cout << "Simulation took " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    }

    void WarehouseSimulator_t::simulatePick(WarehouseOrder_t<std::string>& whOrder)
    {
        std::cout << "Start picking orderID <" << whOrder.getWhOrderID() << ">\n";

        for(auto& whOrderLine : whOrder)
        {
            std::cout << "Start picking lineID <" << whOrderLine.getWhLineID() << ">\n";

            auto filteredLocIDs = this->lookupLocations(whOrderLine.getArticle(), whOrderLine.getQuantity());

            WarehousePathInfo_t* whPathInfo{ nullptr };

            std::for_each(filteredLocIDs.begin(), filteredLocIDs.end(),
                          [&](int32_t whLocID)
                          {
                              whPathInfo = this->whPathFinder->getShortestPath(0, whLocID);

                              if(whPathInfo)
                              {
                                  std::cout << "Shortest path: " << whOrderLine.getArticle() << " " << whPathInfo->targetWhItemID << " " << whPathInfo->distance << std::endl;
                              }
                          });
        }
    }

    std::vector<int32_t> WarehouseSimulator_t::lookupLocations(const std::string& article, int32_t quantity)
    {
        std::vector<int32_t> whLocIDs;
        std::pair<size_t, size_t> position;

        for(const auto* whItem : whLayout.getWhItems())
        {
            if(whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF &&
               whItem->getWhLocationRack()->containsArticle(article, quantity, position))
            {
                whLocIDs.push_back(whItem->getWhItemID());
            }
        }

        return whLocIDs;
    }
}
