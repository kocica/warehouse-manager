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
#include <thread>
#include <utility>
#include <iostream>
#include <algorithm>

// Local
#include "WarehouseItem.h"
#include "WarehouseSimulator.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    WarehouseSimulator_t::WarehouseSimulator_t(const utils::SimArgs_t& args_)
        : args{ args_ }
        , whPathFinder{ new WarehousePathFinder_t() }
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

        std::cout << "Simulation took <" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "> [µs]" << std::endl;
    }

    void WarehouseSimulator_t::simulatePick(WarehouseOrder_t<std::string>& whOrder)
    {
        std::cout << "Start picking orderID <" << whOrder.getWhOrderID() << ">\n";

        whOrderInfo.currentWhItemID = lookupWhEntrance()->getWhItemID();

        for(auto& whOrderLine : whOrder)
        {
            std::cout << "Start picking lineID <" << whOrderLine.getWhLineID() << ">\n";

            auto filteredLocIDs = this->lookupWhLocations(whOrderLine.getArticle(), whOrderLine.getQuantity());

            WarehousePathInfo_t* whTopPathInfo{ nullptr };

            std::for_each(filteredLocIDs.begin(), filteredLocIDs.end(),
                          [&](int32_t targetWhLocID)
                          {
                              auto whPathInfo = this->whPathFinder->getShortestPath(whOrderInfo.currentWhItemID, targetWhLocID);

                              if(!whTopPathInfo || whPathInfo->distance < whTopPathInfo->distance )
                              {
                                  whTopPathInfo = whPathInfo;
                              }
                          });

            if(!whTopPathInfo)
            {
                std::cerr << "Cannot pick this article!" << std::endl;
            }

            std::cout << "Movement for article <" << whOrderLine.getArticle() << ">: from <" << whOrderInfo.currentWhItemID << "> to <" << whTopPathInfo->targetWhItemID << ">\n";

            int64_t us = whTopPathInfo->distance / args.toteSpeed * 1000000;

            std::cout << "Sleep for <" << us << "> [µs]" << std::endl;

            std::this_thread::sleep_for(std::chrono::microseconds(us));

            whOrderInfo.currentWhItemID = whTopPathInfo->targetWhItemID;
        }
    }

    std::vector<int32_t> WarehouseSimulator_t::lookupWhLocations(const std::string& article, int32_t quantity)
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

    WarehouseItem_t* WarehouseSimulator_t::lookupWhEntrance()
    {
        for(WarehouseItem_t* i : whLayout.getWhItems())
        {
            if(i->getType() == WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)
            {
                return i;
            }
        }

        std::cerr << "Warehouse layout has no entrance (should be checked in UI afterall)!" << std::endl;
        return nullptr;
    }
}
