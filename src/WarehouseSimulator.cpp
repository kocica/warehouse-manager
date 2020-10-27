/**
 * Warehouse manager
 *
 * @file    WarehouseSimulator.cpp
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations
 */

#ifndef WHM_GEN

// Std
#include <chrono>
#include <thread>
#include <utility>
#include <float.h>
#include <iostream>
#include <algorithm>

// Local
#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseSimulator.h"

namespace whm
{
    WarehouseSimulator_t::WarehouseSimulator_t()
        : stats{ true }
        , cfg{ ConfigParser_t{ "cfg/simulator.xml" } }
        , whLayout{ WarehouseLayout_t::getWhLayout() }
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
                whFacilities[whItemID] = new simlib3::Store(std::to_string(whItemID).c_str(), cfg.getAs<int32_t>("locationCapacity"));
            }
            else
            {
                whFacilities[whItemID] = new simlib3::Store(std::to_string(whItemID).c_str(), cfg.getAs<int32_t>("conveyorCapacity"));
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
                                                return utils::intersects(lookupWhLocations(line.getArticle(), 0),
                                                                         lookupWhLocations(itLine->getArticle(), 0));
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

    double WarehouseSimulator_t::runSimulation()
    {
        static bool multipleExperiment{ false };

        if(!multipleExperiment)
        {
            whPathFinder->precalculatePaths(whLayout.getWhItems());

            if(Logger_t::getLogger().isVerbose())
            {
                whPathFinder->dump();
            }

            prepareWhSimulation();

            if(cfg.getAs<bool>("preprocess"))
            {
                preprocessOrders();
            }

            multipleExperiment = true;
        }

        Init(0);
        clearSimulation();
        (new OrderRequest_t(whLayout))->Activate();
        Run();

        return Time;
    }

    void WarehouseSimulator_t::orderFinished(double duration, double durationNonSim, int32_t distance)
    {
        static size_t  finished     = 0;
        static double  minDuration  = DBL_MAX;
        static double  maxDuration  = DBL_MIN;
        static double  sumDuration  = 0.0;
        static double  sumDurNonSim = 0.0;
        static int32_t sumDistance  = 0;

        ++finished;
        minDuration  = duration < minDuration ? duration : minDuration;
        maxDuration  = duration > maxDuration ? duration : maxDuration;
        sumDuration  = duration + sumDuration ;
        sumDurNonSim = durationNonSim + sumDurNonSim;
        sumDistance  = distance + sumDistance ;

        if(finished == whLayout.getWhOrders().size())
        {
            if(stats)
            {
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "=====================================================");
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Processed orders count:   [-] <%d>", finished);
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Total traveled distance:  [m] <%d>", sumDistance);
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Duration non-realistic:   [s] <%f>", sumDurNonSim);
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Min order process time:   [s] <%f>", minDuration);
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Max order process time:   [s] <%f>", maxDuration);
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Sum order process time:   [s] <%f>", sumDuration);
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Simulation finished in:   [s] <%f>", Time);
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "=====================================================");

                // Facility load statistics
                for(auto& whFacility : whFacilities)
                {
                    // TODO: Print only Total facility load: <x> %
                    if(lookupWhLoc(whFacility.first)->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
                    {
                        whFacility.second->SetName(std::to_string(lookupWhLoc(whFacility.first)->getWhItemID()).c_str());
                        whFacility.second->Output();
                    }
                }
            }

            // Reset
            finished     = 0;
            minDuration  = DBL_MAX;
            maxDuration  = DBL_MIN;
            sumDuration  = 0.0;
            sumDurNonSim = 0.0;
            sumDistance  = 0;

            Stop(); // Abort();
        }
    }

    void WarehouseSimulator_t::printStats(bool stats_)
    {
        stats = stats_;
    }

    void WarehouseSimulator_t::clearSimulation()
    {
        for(auto& whFacility : whFacilities)
        {
            whFacility.second->Clear();
        }
    }

    void WarehouseSimulator_t::setArguments(const utils::WhmArgs_t& args_)
    {
        args = args_;
    }

    utils::WhmArgs_t WarehouseSimulator_t::getArguments() const
    {
        return args;
    }

    ConfigParser_t& WarehouseSimulator_t::getConfig()
    {
        return cfg;
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

    // ================================================================================================================

    OrderRequest_t::OrderRequest_t(WarehouseLayout_t& layout_)
        : layout(layout_)
        , it(layout.getWhOrders().begin())
    {

    }

    void OrderProcessor_t::Behavior()
    {
        int32_t locationID = 0;
        int32_t totalDistance = 0;
        double  totalDuration = 0.0;
        double  waitDuration = 0.0;
        double  processDuration = Time;
        auto&   sim = WarehouseSimulator_t::getWhSimulator();

        const auto& handleFacility = [&](int32_t itemID)
                                        {
                                        simlib3::Store* whFacility = sim.getWhItemFacility(itemID);

                                        Enter(*whFacility, 1);
                                        Wait(waitDuration / sim.getConfig().getAs<double>("simSpeedup"));
                                        Leave(*whFacility, 1);
                                        };

        locationID = sim.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)->getWhItemID();

        for(const WarehouseOrderLine_t<std::string>& orderLine : order)
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

    OrderProcessor_t::OrderProcessor_t(WarehouseOrder_t<std::string> order_)
        : order(order_)
    {

    }

    void OrderRequest_t::Behavior()
    {
        (new OrderProcessor_t(*it))->Activate();

        if(++it != layout.getWhOrders().end())
        {
            // TODO: Poisson distribution
            Activate(Time + (WarehouseSimulator_t::getWhSimulator().getConfig().getAs<double>("orderRequestInterval")));
        }
    }
}

#endif
