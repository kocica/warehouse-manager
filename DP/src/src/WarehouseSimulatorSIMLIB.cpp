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
        : showStats{ true }
        , optimizationMode{ false }
        , multipleExperiments{ false }
        , cfg{ ConfigParser_t{ "cfg/simulator.xml" } }
        , whPathFinder{ new WarehousePathFinder_t() }
        , whLayout{ WarehouseLayout_t::getWhLayout() }
        , whOrders{ whLayout.getWhOrders() }
    {
        whPathFinder->precalculatePaths(whLayout.getWhItems());

        if(Logger_t::getLogger().isVerbose())
        {
            whPathFinder->dump();
        }

        args.stats = true;
        whPathFinderAco = new WarehousePathFinderACO_t(args);
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
        delete whPathFinderAco;

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
                // TODO: Calculate capacity of conv, or set in GUI
                whFacilities[whItemID] = new simlib3::Store(std::to_string(whItemID).c_str(), cfg.getAs<int32_t>("conveyorCapacity"));
            }
        }
    }

    void WarehouseSimulatorSIMLIB_t::normalPreprocessing()
    {
        whOrders.clear();
        whOrders = whLayout.getWhOrders();

        using Loc_t = std::pair<int32_t, int32_t>;

        int32_t whEntranceID = lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)->getWhItemID();

        for(auto& order : whOrders)
        {
            std::vector<WarehouseOrderLine_t> newLines;

            /*for(auto itLine = order.begin(); itLine != order.end(); ++itLine)
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
            }*/

            std::vector<Loc_t> locations;

            for(const auto& line : order)
            {
                int32_t locationID = lookupWhLocations(line.getArticle(), 0).at(0);

                auto it = std::find_if(locations.begin(), locations.end(),
                                       [=](const Loc_t& loc) -> bool
                                       {
                                           return loc.first == locationID;
                                       });

                if(it == locations.end())
                {
                    auto* p = whPathFinder->getShortestPath(whEntranceID, locationID);

                    if(p)
                    {
                        locations.emplace_back(std::make_pair(locationID, whPathFinder->pathDistance(p->pathToTarget)));
                    }
                }
            }

            std::sort(locations.begin(), locations.end(),
                      [=](const Loc_t& lhs, const Loc_t& rhs) -> bool
                      {
                          return lhs.second < rhs.second;
                      });

            for(const auto& loc : locations)
            {
                for(const auto& line : order)
                {
                    if(utils::contains(lookupWhLocations(line.getArticle(), 0), loc.first))
                    {
                        newLines.push_back(line);
                    }
                }
            }

            // Fix broken IDs
            for(auto itLine = newLines.begin(); itLine != newLines.end(); ++itLine)
            {
                itLine->setWhLineID(itLine - newLines.begin() /*+ 1*/);
            }

            order.setWhOrderLines(newLines);
        }
    }

    void WarehouseSimulatorSIMLIB_t::optimizedPreprocessing()
    {
        whOrders.clear();
        whOrders = whLayout.getWhOrders();

        for(size_t i = 0; i < whOrders.size(); ++i)
        {
            auto& order = whOrders.at(i);

            std::vector<WarehouseOrderLine_t> newLines;

            const auto& bestPath = whPathFinderAco->findPath(static_cast<int32_t>(i));

            // Erase first and last node from bestPath (entrance/exit)?

            for(const auto& locID : bestPath)
            {
                for(const auto& line : order)
                {
                    if(utils::contains(lookupWhLocations(line.getArticle(), 0), locID))
                    {
                        newLines.push_back(line);
                    }
                }
            }

            // Fix broken IDs
            for(auto itLine = newLines.begin(); itLine != newLines.end(); ++itLine)
            {
                itLine->setWhLineID(itLine - newLines.begin() /*+ 1*/);
            }

            order.setWhOrderLines(newLines);
        }
    }

    void WarehouseSimulatorSIMLIB_t::passivateProcess(int32_t locID, simlib3::Process* proc)
    {
        passivatedProcesses[locID].push_back(proc);
        proc->Passivate();
    }

    void WarehouseSimulatorSIMLIB_t::activateProcesses(int32_t locID)
    {
        for(simlib3::Process* proc : passivatedProcesses[locID])
        {
            proc->Activate();
        }

        passivatedProcesses[locID].clear();
    }

    double WarehouseSimulatorSIMLIB_t::runSimulation()
    {
        if(cfg.getAs<bool>("replenishment"))
        {
            // Reset locations in between runs to keep results consistent
            for(auto* item : whm::WarehouseLayout_t::getWhLayout().getWhItems())
            {
                if(item->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
                {
                    item->getWhLocationRack()->resetRack(cfg.getAs<int32_t>("initialSlotQty"));
                }
            }
        }

        if(!multipleExperiments)
        {
            prepareWhSimulation();

            if(optimizationModeActive())
            {
                multipleExperiments = !multipleExperiments;
            }
        }

        // Perform order preprocessing
        //    We need to do preprocessing in each simulation run, since optimizer moves products between
        //    locations, so the previous calculated optimal path may no longer be optimal
        if(cfg.getAs<std::string>("preprocessing") == "normal")
        {
            normalPreprocessing();
        }
        else if(cfg.getAs<std::string>("preprocessing") == "optimized")
        {
            optimizedPreprocessing();
        }

        //SetCalendar("cq");
        Init(0);
        clearSimulation();
        (new OrderRequest_t(*this))->Activate();
        Run();

        return Time;
    }

    void WarehouseSimulatorSIMLIB_t::orderFinished(double duration, int32_t distanceConv, int32_t distanceWorker)
    {
        stats.outboundsFinished++;
        stats.processingTime = duration;
        stats.distanceTraveledConv += distanceConv;
        stats.distanceTraveledWorker += distanceWorker;

#       ifdef WHM_GUI
        if(uiCallback)
        {
            uiCallback(stats, false);
        }
#       endif

        if(stats.outboundsFinished == whLayout.getWhOrders().size())
        {
            stats.processingTime = Time;

            for(auto& whFacility : whFacilities)
            {
                lookupWhLoc(whFacility.first)->setWorkload(whFacility.second->tstat.MeanValue() / whFacility.second->Capacity());
            }

#           ifdef WHM_GUI
            if(uiCallback)
            {
                uiCallback(stats, true);
            }
#           endif

            if(showStats)
            {
                for(auto& whItem : whLayout.getWhItems())
                {
                    Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "Workload <%d>: <%f>", whItem->getWhItemID(), whItem->getWorkload());
                }

                stats.dump();
            }

            stats.reset();
            Stop();
        }
    }

    void WarehouseSimulatorSIMLIB_t::replenishmentFinished()
    {
        ++ stats.replenishmentsFinished;
    }

    void WarehouseSimulatorSIMLIB_t::printStats(bool showStats_)
    {
        showStats = showStats_;
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
            throw std::runtime_error("Cannot reach target!");
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

        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Warehouse layout has no entrance/exit (should be checked in UI)!");
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

        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Failed to find location!");
        return nullptr;
    }

    void WarehouseSimulatorSIMLIB_t::SimulationStats_t::dump() const
    {
        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "=====================================================");
        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Processed outbound orders:        [-] <%d>", this->outboundsFinished);
        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Processed replenishment orders:   [-] <%d>", this->replenishmentsFinished);
        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Distance traveled (conveyor):     [m] <%d>", this->distanceTraveledConv);
        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Distance traveled (workes):       [m] <%d>", this->distanceTraveledWorker);
        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, " Simulation finished in:           [s] <%f>", this->processingTime);
        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "=====================================================");
    }

    void WarehouseSimulatorSIMLIB_t::SimulationStats_t::reset()
    {
        this->processingTime            = 0;
        this->outboundsFinished         = 0;
        this->replenishmentsFinished    = 0;
        this->distanceTraveledConv      = 0;
        this->distanceTraveledWorker    = 0;
    }

    // ================================================================================================================

    OrderProcessor_t::OrderProcessor_t(WarehouseOrder_t order_, WarehouseSimulatorSIMLIB_t& sim_)
        : order(order_)
        , sim(sim_)
    {

    }

    void OrderProcessor_t::Behavior()
    {
        switch(order.getWhOrderType())
        {
            case whm::WarehouseOrderType_t::E_OUTBOUND_ORDER:
            {
                outboundProcessing();
                break;
            }
            case whm::WarehouseOrderType_t::E_REPLENISHMENT_ORDER:
            {
                replenishmentProcessing();
                break;
            }
        }
    }

    void OrderProcessor_t::handleFacility(int32_t itemID, double waitDuration)
    {
        simlib3::Store* whFacility = sim.getWhItemFacility(itemID);

        Enter(*whFacility, 1);
        Wait(waitDuration / sim.getConfig().getAs<double>("simSpeedup"));
        Leave(*whFacility, 1);
    }

    void OrderProcessor_t::outboundProcessing()
    {
        int32_t locationID{ 0 };
        double waitDuration{ 0.0 };
        double processDuration{ Time };
        size_t totalDistanceConv{ 0 };
        size_t totalDistanceWorker{ 0 };

        // Simulate order start from entrance
        locationID = sim.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)->getWhItemID();

        // Process all order lines
        for(const WarehouseOrderLine_t& orderLine : order)
        {
            const std::vector<int32_t>& targetLocations = sim.lookupWhLocations(orderLine.getArticle(), 0);
            const WarehousePathInfo_t* shortestPath = sim.lookupShortestPath(locationID, targetLocations);

            // Reach target location using conveyor
            for(const std::pair<int32_t, int32_t>& pathItem : shortestPath->pathToTarget)
            {
                waitDuration = pathItem.second / sim.getConfig().getAs<double>("toteSpeed");

                totalDistanceConv += pathItem.second;

                handleFacility(pathItem.first, waitDuration);
            }

            locationID = shortestPath->targetWhItemID;

            WarehouseItem_t* whLoc = sim.lookupWhLoc(locationID);
            std::pair<size_t, size_t> slotPos;
            bool containsProduct = whLoc->getWhLocationRack()->containsArticle(orderLine.getArticle(), orderLine.getQuantity(), slotPos,
                                                                               sim.getConfig().getAs<bool>("replenishment"));

            if(sim.getConfig().getAs<bool>("replenishment") && !containsProduct)
            {
                // Create replenishment order and push to buffer / wait for reple to be processed
                WarehouseOrder_t replenishment;
                std::vector<WarehouseOrderLine_t> replenishmentLines;
                replenishment.setWhOrderType(WarehouseOrderType_t::E_REPLENISHMENT_ORDER);

                // Fill replenishment with products we currently need at this location
                auto* whRack = whLoc->getWhLocationRack();
                for(int32_t y = 0; y < whRack->getSlotCountY(); y++)
                {
                    for(int32_t x = 0; x < whRack->getSlotCountX(); x++)
                    {
                        if(whRack->at(x, y).getArticle() == orderLine.getArticle() ||
                          (whRack->at(x, y).isOccupied() && whRack->at(x, y).getQuantity() <= sim.getConfig().getAs<int32_t>("replenishmentThreshold")))
                        {
                            WarehouseOrderLine_t line(nullptr);
                            line.setArticle(whRack->at(x, y).getArticle());

                            int32_t requestedQuantity{ 0 };

                            if(whRack->at(x, y).getArticle() == orderLine.getArticle())
                            {
                                requestedQuantity = sim.getConfig().getAs<int32_t>("replenishmentQuantity") + orderLine.getQuantity();
                            }
                            else
                            {
                                requestedQuantity = sim.getConfig().getAs<int32_t>("replenishmentQuantity");
                            }

                            line.setQuantity(requestedQuantity);
                            replenishmentLines.emplace_back(std::move(line));
                        }
                    }
                }

                replenishment.setWhOrderLines(replenishmentLines);

                (new OrderProcessor_t(replenishment, sim))->Activate();

                // There might be multiple replenishmnent orders coming, so check if the product
                // required by this order was replenished, and if not, deactivate the process again
                while(!containsProduct)
                {
                    // Night night
                    sim.passivateProcess(locationID, dynamic_cast<simlib3::Process*>(this));
                    containsProduct = whLoc->getWhLocationRack()->containsArticle(orderLine.getArticle(), orderLine.getQuantity(), slotPos, true);
                }
            }

            // Pick article(s)
            const auto ratio = WarehouseLayout_t::getWhLayout().getRatio();
            const auto distance = ((slotPos.first  / static_cast<float>(whLoc->getWhLocationRack()->getSlotCountX())) * (whLoc->getW() / ratio) +
                                   (slotPos.second / static_cast<float>(whLoc->getWhLocationRack()->getSlotCountY())) * (whLoc->getH() / ratio));
            totalDistanceWorker += distance;
            waitDuration = distance / sim.getConfig().getAs<double>("workerSpeed");

            handleFacility(locationID, waitDuration);
        }

        // Move the order/carton to shipping
        int32_t dispatchID = sim.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_DISPATCH)->getWhItemID();

        const WarehousePathInfo_t* shortestPath = sim.lookupShortestPath(locationID, std::vector<int32_t>{ dispatchID });

        for(const std::pair<int32_t, int32_t>& pathItem : shortestPath->pathToTarget)
        {
            waitDuration = pathItem.second / sim.getConfig().getAs<double>("toteSpeed");

            totalDistanceConv += pathItem.second;

            handleFacility(pathItem.first, waitDuration);
        }

        // Ship order/carton
        locationID = dispatchID;

        waitDuration = (60 / sim.getConfig().getAs<int32_t>("totesPerMin"));

        handleFacility(locationID, waitDuration);

        // Trace finished order
        sim.orderFinished(Time - processDuration, totalDistanceConv, totalDistanceWorker);
    }

    void OrderProcessor_t::replenishmentProcessing()
    {
        auto locationID{ 0 };

        // Simulate replenishment order release from buffer
        locationID = sim.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_BUFFER)->getWhItemID();

        // Process all replenishment order lines
        for(const WarehouseOrderLine_t& orderLine : order)
        {
            const std::vector<int32_t>& targetLocations = sim.lookupWhLocations(orderLine.getArticle(), 0);
            const WarehousePathInfo_t* shortestPath = sim.lookupShortestPath(locationID, targetLocations);

            // Reach target location using conveyor
            for(const std::pair<int32_t, int32_t>& pathItem : shortestPath->pathToTarget)
            {
                handleFacility(pathItem.first, pathItem.second / sim.getConfig().getAs<double>("toteSpeed"));
            }

            locationID = shortestPath->targetWhItemID;

            WarehouseItem_t* whLoc = sim.lookupWhLoc(locationID);

            std::pair<size_t, size_t> slotPos;
            whLoc->getWhLocationRack()->replenishArticle(orderLine.getArticle(), orderLine.getQuantity(), slotPos);

            // Replenish article(s)
            const auto ratio = WarehouseLayout_t::getWhLayout().getRatio();
            const auto waitDuration = ((slotPos.first  / static_cast<float>(whLoc->getWhLocationRack()->getSlotCountX())) * (whLoc->getW() / ratio) +
                                       (slotPos.second / static_cast<float>(whLoc->getWhLocationRack()->getSlotCountY())) * (whLoc->getH() / ratio)) / sim.getConfig().getAs<double>("workerSpeed");

            handleFacility(locationID, waitDuration);
        }

        // Replenishment finished, wake up sleeping processes (orders at location waiting for replenishment)
        sim.activateProcesses(locationID);
        sim.replenishmentFinished();
    }

    // ================================================================================================================

    OrderRequest_t::OrderRequest_t(WarehouseSimulatorSIMLIB_t& sim_)
        : sim(sim_)
        , it(sim.getWhOrders().begin())
    {

    }

    void OrderRequest_t::Behavior()
    {
        (new OrderProcessor_t(*it, sim))->Activate();

        if(++it != sim.getWhOrders().end())
        {
            Activate(Time + /*Exponential*/(sim.getConfig().getAs<double>("orderRequestInterval")));
        }
    }
}

#endif
