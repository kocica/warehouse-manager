/**
 * Warehouse manager
 *
 * @file    WarehouseSimulatorSIMLIB.cpp
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations (using SIMLIB/C++)
 */

#ifdef WHM_SIM

// Std
#include <chrono>
#include <thread>
#include <limits>
#include <utility>
#include <iostream>
#include <algorithm>

// Local
#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseSimulatorSIMLIB.h"

namespace whm
{
    WarehouseSimulatorSIMLIB_t::WarehouseSimulatorSIMLIB_t()
        : stats{ true }
        , optimizationMode{ false }
        , multipleExperiments{ false }
        , cfg{ ConfigParser_t{ "cfg/simulator.xml" } }
        , whLayout{ WarehouseLayout_t::getWhLayout() }
        , whPathFinder{ new WarehousePathFinder_t() }
        , whOrders{ whLayout.getWhOrders() }
    {

    }

#   ifdef WHM_GUI
    void WarehouseSimulatorSIMLIB_t::setUiCallback(UiCallback_t c)
    {
        this->uiCallback = c;
    }
#   endif

    simlib3::Store* WarehouseSimulatorSIMLIB_t::getWhItemFacility(int32_t facilityID)
    {
        return whFacilities[facilityID];
    }

    const std::vector<WarehouseOrder_t>& WarehouseSimulatorSIMLIB_t::getWhOrders() const
    {
        return whOrders;
    }

    WarehouseSimulatorSIMLIB_t::~WarehouseSimulatorSIMLIB_t()
    {
        delete whPathFinder;

        for(auto& whFacility : whFacilities)
        {
            delete whFacility.second;
        }

        whFacilities.clear();
    }

    void WarehouseSimulatorSIMLIB_t::prepareWhSimulation()
    {
        for(const auto* whItem : whLayout.getWhItems())
        {
            auto whItemID = whItem->getWhItemID();

            if(whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
            {
                whFacilities[whItemID] = new simlib3::Store(std::to_string(whItemID).c_str(), cfg.getAs<int32_t>("locationCapacity"));
            }
            else
            {
                whFacilities[whItemID] = new simlib3::Store(std::to_string(whItemID).c_str(), cfg.getAs<int32_t>("conveyorCapacity"));
            }
        }
    }

    void WarehouseSimulatorSIMLIB_t::preprocessOrders()
    {
        whOrders.clear();
        whOrders = whLayout.getWhOrders();

        for(auto& order : whOrders)
        {
            std::vector<WarehouseOrderLine_t> newLines;

            for(auto itLine = order.begin(); itLine != order.end(); ++itLine)
            {
                auto itTargetLine = std::find_if(newLines.begin(), newLines.end(),
                                                 [&](const WarehouseOrderLine_t& line) -> bool
                                                 {
                                                    return utils::intersects(lookupWhLocations(line.getArticle(), 0),
                                                                             lookupWhLocations(itLine->getArticle(), 0));
                                                 });

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

    double WarehouseSimulatorSIMLIB_t::runSimulation()
    {
        if(!multipleExperiments)
        {
            whPathFinder->precalculatePaths(whLayout.getWhItems());

            if(Logger_t::getLogger().isVerbose())
            {
                whPathFinder->dump();
            }

            prepareWhSimulation();

            if(optimizationModeActive())
            {
                multipleExperiments = !multipleExperiments;
            }
        }

        if(cfg.getAs<bool>("preprocess"))
        {
            preprocessOrders();
        }

        //SetCalendar("cq");
        Init(0);
        clearSimulation();
        (new OrderRequest_t(*this))->Activate();
        Run();

        return Time;
    }

    void WarehouseSimulatorSIMLIB_t::orderFinished(double duration, double durationNonSim, int32_t distance)
    {
#       ifndef WHM_GUI
        (void) duration;
#       endif
        (void) distance;
        (void) durationNonSim;

        ++ ordersFinished;

#       ifdef WHM_GUI
        if(uiCallback)
        {
            uiCallback(duration, false);
        }
#       endif

        if(ordersFinished == whLayout.getWhOrders().size())
        {
            for(auto& whFacility : whFacilities)
            {
                lookupWhLoc(whFacility.first)->setWorkload(whFacility.second->tstat.MeanValue() / whFacility.second->Capacity());
            }

#           ifdef WHM_GUI
            if(uiCallback)
            {
                uiCallback(Time, true);
            }
#           endif

            if(stats)
            {
                // Facility workload statistics dump
                for(auto& whItem : whLayout.getWhItems())
                {
                    Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "Workload <%d>: <%f>", whItem->getWhItemID(), whItem->getWorkload());
                }

                /*for(auto& whFacility : whFacilities)
                {
                    if(lookupWhLoc(whFacility.first)->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
                    {
                        whFacility.second->SetName(std::to_string(lookupWhLoc(whFacility.first)->getWhItemID()).c_str());
                        whFacility.second->Output();
                    }
                }*/

                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "=====================================================");
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Processed orders count:   [-] <%d>", ordersFinished);
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Simulation finished in:   [s] <%f>", Time);
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "=====================================================");
            }

            ordersFinished = 0;
            Stop();
        }
    }

    void WarehouseSimulatorSIMLIB_t::printStats(bool stats_)
    {
        stats = stats_;
    }

    bool& WarehouseSimulatorSIMLIB_t::optimizationModeActive()
    {
        return optimizationMode;
    }

    void WarehouseSimulatorSIMLIB_t::clearSimulation()
    {
        for(auto& whFacility : whFacilities)
        {
            whFacility.second->Clear();
        }
    }

    void WarehouseSimulatorSIMLIB_t::setArguments(const utils::WhmArgs_t& args_)
    {
        args = args_;
    }

    utils::WhmArgs_t WarehouseSimulatorSIMLIB_t::getArguments() const
    {
        return args;
    }

    ConfigParser_t& WarehouseSimulatorSIMLIB_t::getConfig()
    {
        return cfg;
    }

    void WarehouseSimulatorSIMLIB_t::setConfig(const ConfigParser_t& cfg_)
    {
        cfg = cfg_;
    }

    WarehousePathInfo_t* WarehouseSimulatorSIMLIB_t::lookupShortestPath(int32_t currentLocID, const std::vector<int32_t>& targetLocIDs)
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

    std::vector<int32_t> WarehouseSimulatorSIMLIB_t::lookupWhLocations(const std::string& article, int32_t quantity)
    {
        std::vector<int32_t> whLocIDs;

        for(const auto* whItem : whLayout.getWhItems())
        {
            if(whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF &&
               whItem->getWhLocationRack()->containsArticle(article, quantity))
            {
                whLocIDs.push_back(whItem->getWhItemID());
            }
        }

        return whLocIDs;
    }

    WarehouseItem_t* WarehouseSimulatorSIMLIB_t::lookupWhGate(const WarehouseItemType_t& whGateType)
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

    WarehouseItem_t* WarehouseSimulatorSIMLIB_t::lookupWhLoc(int32_t locID)
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

    // ================================================================================================================

    OrderRequest_t::OrderRequest_t(WarehouseSimulatorSIMLIB_t& sim_)
        : sim(sim_)
        , it(sim.getWhOrders().begin())
    {

    }

    void OrderProcessor_t::Behavior()
    {
        int32_t locationID = 0;
        int32_t totalDistance = 0;
        double  totalDuration = 0.0;
        double  waitDuration = 0.0;
        double  processDuration = Time;

        const auto& handleFacility = [&](int32_t itemID)
                                        {
                                            simlib3::Store* whFacility = sim.getWhItemFacility(itemID);

                                            Enter(*whFacility, 1);
                                            Wait(waitDuration / sim.getConfig().getAs<double>("simSpeedup"));
                                            Leave(*whFacility, 1);
                                        };

        locationID = sim.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)->getWhItemID();

        for(const WarehouseOrderLine_t& orderLine : order)
        {
            const std::vector<int32_t>& targetLocations = sim.lookupWhLocations(orderLine.getArticle(), orderLine.getQuantity());
            const WarehousePathInfo_t* shortestPath = sim.lookupShortestPath(locationID, targetLocations);

            for(const std::pair<int32_t, int32_t>& pathItem : shortestPath->pathToTarget)
            {
                waitDuration = pathItem.second / sim.getConfig().getAs<double>("toteSpeed");

                totalDuration += waitDuration;
                totalDistance += pathItem.second;

                handleFacility(pathItem.first);
            }

            locationID = shortestPath->targetWhItemID;

            WarehouseItem_t* whLoc = sim.lookupWhLoc(locationID);
            std::pair<size_t, size_t> slotPos;
            whLoc->getWhLocationRack()->containsArticle(orderLine.getArticle(), 0, slotPos);
            static const auto ratio = WarehouseLayout_t::getWhLayout().getRatio();
            waitDuration = ((slotPos.first  / static_cast<float>(whLoc->getWhLocationRack()->getSlotCountX())) * (whLoc->getW() / ratio) +
                            (slotPos.second / static_cast<float>(whLoc->getWhLocationRack()->getSlotCountY())) * (whLoc->getH() / ratio)) / sim.getConfig().getAs<double>("workerSpeed");

            totalDuration += waitDuration;
            handleFacility(locationID);
        }

        int32_t dispatchID = sim.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_DISPATCH)->getWhItemID();

        const WarehousePathInfo_t* shortestPath = sim.lookupShortestPath(locationID, std::vector<int32_t>{ dispatchID });

        for(const std::pair<int32_t, int32_t>& pathItem : shortestPath->pathToTarget)
        {
            waitDuration = pathItem.second / sim.getConfig().getAs<double>("toteSpeed");

            totalDuration += waitDuration;
            totalDistance += pathItem.second;

            handleFacility(pathItem.first);
        }

        locationID = dispatchID;

        waitDuration = (60 / sim.getConfig().getAs<int32_t>("totesPerMin"));

        totalDuration += waitDuration;

        handleFacility(locationID);

        sim.orderFinished(Time - processDuration, totalDuration, totalDistance);
    }

    OrderProcessor_t::OrderProcessor_t(WarehouseOrder_t order_, WarehouseSimulatorSIMLIB_t& sim_)
        : order(order_)
        , sim(sim_)
    {

    }

    void OrderRequest_t::Behavior()
    {
        (new OrderProcessor_t(*it, sim))->Activate();

        if(++it != sim.getWhOrders().end())
        {
            // TODO: Poisson distribution
            Activate(Time + (sim.getConfig().getAs<double>("orderRequestInterval")));
        }
    }
}

#endif
