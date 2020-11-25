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

int counter = 0;
int processedOrders = 0;

namespace whm
{
    void WarehouseSimulatorCPPSIM_t::Body()
    {
        auto* request = new OrderRequestCPPSIM_t(WarehouseLayout_t::getWhLayout(), *this);
        request->Activate();

        Scheduler::scheduler().Resume();

        Wait(10000);

        Scheduler::scheduler().reset();
        Scheduler::scheduler().Suspend();

        request->terminate();
    }

    // ================================================================================================================

    OrderRequestCPPSIM_t::OrderRequestCPPSIM_t(WarehouseLayout_t& layout_, WarehouseSimulatorCPPSIM_t& sim_)
        : layout(layout_)
        , sim(sim_)
        , it(layout.getWhOrders().begin())
    {
        c = counter++;
    }

    void OrderRequestCPPSIM_t::Body()
    {
        for(;processedOrders < 100;)
        {
            processedOrders++;

            std::cerr << "Hello C++SIM: " << c << std::endl;

            Hold(100.0);
            // Order processor call
        }
    }
}

#endif
