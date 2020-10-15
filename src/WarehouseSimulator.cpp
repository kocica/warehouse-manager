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

namespace whm
{
    WarehouseSimulator_t::WarehouseSimulator_t()
        : whLayout{ WarehouseLayout_t::getWhLayout() }
        , whPathFinder{ new WarehousePathFinder_t() }
    {

    }

    WarehouseSimulator_t& WarehouseSimulator_t::getWhSimulator()
    {
        static WarehouseSimulator_t s;
        return s;
    }

    simlib3::Store* WarehouseSimulator_t::getWhItemFacility(int32_t facilityID)
    {
        return whFacilities[facilityID];
    }

    WarehouseSimulator_t::~WarehouseSimulator_t()
    {
        delete whPathFinder;

        for(auto& whFacility : whFacilities)
        {
            delete whFacility.second;
        }

        whFacilities.clear();
    }

    void WarehouseSimulator_t::prepareWhSimulation()
    {
        for(const auto* whItem : whLayout.getWhItems())
        {
            auto whItemID = whItem->getWhItemID();

            if(whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
            {
                whFacilities[whItemID] = new simlib3::Store(std::to_string(whItemID).c_str(), 1);
            }
            else
            {
                whFacilities[whItemID] = new simlib3::Store(std::to_string(whItemID).c_str(), 1 /*TODO: Calculate*/);
            }
        }
    }

    void WarehouseSimulator_t::preprocessOrders()
    {
        auto& orders = const_cast<std::vector<WarehouseOrder_t<std::string>>&>(whLayout.getWhOrders());

        for(auto& order : orders)
        {
            std::vector<WarehouseOrderLine_t<std::string>> newLines;

            for(auto itLine = order.begin(); itLine != order.end(); ++itLine)
            {
                static const auto& lookup = [&](const WarehouseOrderLine_t<std::string>& line) -> bool
                                            {
                                                return line.getArticle() == itLine->getArticle();
                                            };

                auto itTargetLine = std::find_if(newLines.begin(), newLines.end(), lookup);

                if(itTargetLine != newLines.end())
                {
                    newLines.insert(itTargetLine, *itLine);
                }
                else
                {
                    newLines.push_back(*itLine);
                }
            }

            // Fix IDs broken by reordering
            for(auto itLine = newLines.begin(); itLine != newLines.end(); ++itLine)
            {
                itLine->setWhLineID(itLine - newLines.begin() /*+ 1*/);
            }

            order.setWhOrderLines(newLines);
        }
    }

    void WarehouseSimulator_t::runSimulation()
    {
        whPathFinder->precalculatePaths(whLayout.getWhItems());
        //whPathFinder->dump();

        prepareWhSimulation();

        if(args.preprocess)
        {
            preprocessOrders();
        }

        simStart = Time;

        Init(0);
        (new OrderRequest_t(whLayout))->Activate();
        Run();

        for(auto& whFacility : whFacilities)
        {
            //whFacility.second->Output();
        }
    }

    void WarehouseSimulator_t::orderFinished()
    {
        static size_t finished = 0;

        ++finished;

        if(finished == whLayout.getWhOrders().size())
        {
            std::cout << "Simulation finished in: <" << Time - simStart << ">\n";
        }
    }

    void WarehouseSimulator_t::setArguments(const utils::SimArgs_t& args_)
    {
        args = args_;
    }

    utils::SimArgs_t WarehouseSimulator_t::getArguments()
    {
        return args;
    }

    WarehousePathInfo_t* WarehouseSimulator_t::lookupShortestPath(int32_t currentLocID, const std::vector<int32_t>& targetLocIDs)
    {
        WarehousePathInfo_t* whPathInfo{ nullptr };

        std::for_each(targetLocIDs.begin(), targetLocIDs.end(),
                      [&](int32_t targetLocID)
                      {
                          auto* actPathInfo = whPathFinder->getShortestPath(currentLocID, targetLocID);

                          if(!whPathInfo || whPathFinder->pathDistance(actPathInfo->pathToTarget)
                                          < whPathFinder->pathDistance(whPathInfo->pathToTarget))
                          {
                              whPathInfo = actPathInfo;
                          }
                      });

        if(!whPathInfo)
        {
            throw std::runtime_error("Cannot pick article!");
        }

        return whPathInfo;
    }

    std::vector<int32_t> WarehouseSimulator_t::lookupWhLocations(const std::string& article, int32_t quantity)
    {
        (void)quantity;
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

    WarehouseItem_t* WarehouseSimulator_t::lookupWhLoc(int32_t locID)
    {
        for(WarehouseItem_t* i : whLayout.getWhItems())
        {
            if(i->getWhItemID() == locID)
            {
                return i;
            }
        }

        std::cerr << "Failed to find location!" << std::endl;
        return nullptr;
    }
}
