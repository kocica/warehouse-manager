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
        , whPathInfo{ nullptr }
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

        for(auto& whOrder : whLayout.getWhOrders()) { this->simulateProcessing(whOrder); }

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        std::cout << "Simulation took <" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "> [µs]" << std::endl;
    }

    void WarehouseSimulator_t::simulateProcessing(WarehouseOrder_t<std::string>& whOrder)
    {
        std::cout << std::endl << "Start picking orderID <" << whOrder.getWhOrderID() << ">\n";

        // Start in warehouse entrance
        whOrderInfo.currentWhItemID = lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)->getWhItemID();

        // Simulate picking of each line
        for(auto& whOrderLine : whOrder)
        {
            std::cout << " - Start picking lineID <" << whOrderLine.getWhLineID() << ">\n";

            whPathInfo = nullptr;

            auto articleLocIDs = this->lookupWhLocations(whOrderLine.getArticle(), whOrderLine.getQuantity());

            std::for_each(articleLocIDs.begin(), articleLocIDs.end(),
                          [&](int32_t targetWhLocID)
                          {
                              auto actPathInfo = this->whPathFinder->getShortestPath(whOrderInfo.currentWhItemID, targetWhLocID);

                              if(!whPathInfo || actPathInfo->distance < whPathInfo->distance )
                              {
                                  whPathInfo = actPathInfo;
                              }
                          });

            if(!whPathInfo)
            {
                std::cerr << "Cannot pick this article!" << std::endl;
            }
            else
            {
                simulateMovement();
                simulatePick();
            }
        }

        // Move to warehouse dispatch
        whPathInfo = this->whPathFinder->getShortestPath(whOrderInfo.currentWhItemID, lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_DISPATCH)->getWhItemID());

        if(!whPathInfo)
        {
            std::cerr << "Cannot move to dispatch!" << std::endl;
        }
        else
        {
            simulateMovement();
        }
    }

    void WarehouseSimulator_t::simulateMovement()
    {
        std::cout << "  - Movement from <" << whOrderInfo.currentWhItemID << "> to <" << whPathInfo->targetWhItemID << ">\n";

        int64_t us = whPathInfo->distance / args.toteSpeed * 1000000;

        std::cout << "  - Sleep for <" << us << "> [µs]" << std::endl;

        std::this_thread::sleep_for(std::chrono::microseconds(us));

        whOrderInfo.currentWhItemID = whPathInfo->targetWhItemID;
    }

    void WarehouseSimulator_t::simulatePick()
    {
        std::cout << "   - Picking from location <" << whOrderInfo.currentWhItemID << ">\n";

        int64_t us = whPathInfo->distance / args.toteSpeed * 1000000;

        std::cout << "   - Sleep for <" << us << "> [µs]" << std::endl;

        std::this_thread::sleep_for(std::chrono::microseconds(us));
    }

    std::vector<int32_t> WarehouseSimulator_t::lookupWhLocations(const std::string& article, int32_t quantity)
    {
        std::vector<int32_t> whLocIDs;
        std::pair<size_t, size_t> position;

        for(const auto* whItem : whLayout.getWhItems())
        {
            if(whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF &&
               whItem->getWhLocationRack()->containsArticle(article, 0 /*quantity*/, position))
            {
                whLocIDs.push_back(whItem->getWhItemID());
            }
        }

        return whLocIDs;
    }

    WarehouseItem_t* WarehouseSimulator_t::lookupWhGate(const WarehouseItemType_t& whGateType)
    {
        for(WarehouseItem_t* i : whLayout.getWhItems())
        {
            if(i->getType() == whGateType)
            {
                return i;
            }
        }

        std::cerr << "Warehouse layout has no entrance/exit (should be checked in UI)!" << std::endl;
        return nullptr;
    }
}
