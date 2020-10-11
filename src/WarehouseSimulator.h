/**
 * Warehouse manager
 *
 * @file    WarehouseSimulator.h
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations
 */

#pragma once

#include "WarehouseOrder.h"
#include "WarehouseLayout.h"
#include "WarehousePathFinder.h"

namespace whm
{
    class WarehouseSimulator_t
    {
        public:
            WarehouseSimulator_t();
            ~WarehouseSimulator_t();

            void runSimulation();

        protected:
            void simulatePick(WarehouseOrder_t<std::string>& whOrder);
            std::vector<int32_t> lookupLocations(const std::string&, int32_t);

            // TODO: calcTimePicking (calculates time of picking at location which depends on slot position x/y, etc.)
            // TODO: calcTimeMovement (calculates time of movement on conveyor which depends on length, etc.)

        private:
            // TODO: orderInfo (where current order actually is located, etc.)
            WarehouseLayout_t& whLayout;
            WarehousePathFinder_t* whPathFinder;
    };
}
