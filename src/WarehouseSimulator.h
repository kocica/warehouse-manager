/**
 * Warehouse manager
 *
 * @file    WarehouseSimulator.h
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations
 */

#pragma once

#include "Utils.h"
#include "WarehouseOrder.h"
#include "WarehouseLayout.h"
#include "WarehouseItemType.h"
#include "WarehousePathFinder.h"

namespace whm
{
    struct WarehouseSimOrderInfo_t
    {
        int32_t currentWhItemID{ 0 };
    };

    class WarehouseSimulator_t
    {
        public:
            WarehouseSimulator_t(const utils::SimArgs_t&);
            ~WarehouseSimulator_t();

            void runSimulation();

        protected:
            WarehouseItem_t* lookupWhGate(const WarehouseItemType_t&);
            std::vector<int32_t> lookupWhLocations(const std::string&, int32_t);

            void simulatePick();
            void simulateMovement();
            void simulateProcessing(WarehouseOrder_t<std::string>& whOrder);

        private:
            utils::SimArgs_t args;
            WarehouseLayout_t& whLayout;
            WarehousePathInfo_t* whPathInfo;
            WarehousePathFinder_t* whPathFinder;
            WarehouseSimOrderInfo_t whOrderInfo;
    };
}
