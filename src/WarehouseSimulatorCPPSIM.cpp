/**
 * Warehouse manager
 *
 * @file    WarehouseSimulatorCPPSIM.cpp
 * @date    10/24/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations (using C++SIM)
 */

#ifdef WHM_SIM_2

// Std
#include <chrono>
#include <thread>
#include <limits>
#include <thread>
#include <utility>
#include <iostream>
#include <algorithm>

// Local
#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseSimulatorCPPSIM.h"

namespace whm
{
    int32_t ordersProcessed{ 0 };

    double WarehouseSimulatorCPPSIM_t::runSimulation()
    {
        whm::WarehouseSimulatorCPPSIMInt_t simulator;
        simulator.Await();
        simulator.Exit();

        return 0.0;
    }

    void WarehouseSimulatorCPPSIMInt_t::Body()
    {
        auto* request = new OrderRequestCPPSIM_t(WarehouseLayout_t::getWhLayout(), *this);
        request->Activate();

        Scheduler::scheduler().Resume();

        while(ordersProcessed < 100)
        {
            Hold(100);
        }

        // TODO: Save simulation time

        Scheduler::scheduler().reset();
        Scheduler::scheduler().Suspend();

        request->terminate();

        if(request)
        {
            delete request;
        }

        Thread::mainResume();
    }

    void WarehouseSimulatorCPPSIMInt_t::Await()
    {
        Resume();
        Thread::Self()->Suspend();
    }

    void WarehouseSimulatorCPPSIMInt_t::Exit()
    {
        Thread::Exit();
    }

    // ================================================================================================================

    OrderProcessorCPPSIM_t::OrderProcessorCPPSIM_t(WarehouseOrder_t order_, WarehouseSimulatorCPPSIMInt_t& sim_)
        : order(order_)
        , sim(sim_)
    {

    }

    void OrderProcessorCPPSIM_t::Body()
    {
        /*int32_t locationID = 0;
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

        sim.orderFinished(Time - processDuration, totalDuration, totalDistance);*/
    }

    OrderRequestCPPSIM_t::OrderRequestCPPSIM_t(WarehouseLayout_t& layout_, WarehouseSimulatorCPPSIMInt_t& sim_)
        : layout(layout_)
        , sim(sim_)
        , it(layout.getWhOrders().begin())
    {

    }

    void OrderRequestCPPSIM_t::Body()
    {
        for(;;)
        {
            (new OrderProcessorCPPSIM_t(*it, sim))->Activate();

            ordersProcessed++;

            if(++it != layout.getWhOrders().end())
            {
                Hold(1.0);
            }
        }
    }
}

#endif
