/**
 * Warehouse manager
 *
 * @file    WarehouseSimulatorSIMLIB.h
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations (using SIMLIB/C++)
 */

#pragma once

#ifdef WHM_SIM

// Std
#include <map>
#include <functional>

// SIMLIB/C++
#include <simlib.h>

// Local
#include "Utils.h"
#include "ConfigParser.h"
#include "WarehouseItem.h"
#include "WarehouseOrder.h"
#include "WarehouseLayout.h"
#include "WarehouseItemType.h"
#include "WarehousePathFinder.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    class WarehouseSimulatorSIMLIB_t
    {
        public:
            WarehouseSimulatorSIMLIB_t();
            ~WarehouseSimulatorSIMLIB_t();

            double runSimulation();
            void orderFinished(double, double, int32_t);

            void printStats(bool);

            bool& optimizationModeActive();

            ConfigParser_t& getConfig();
            void setConfig(const ConfigParser_t&);

            utils::WhmArgs_t getArguments() const;
            void setArguments(const utils::WhmArgs_t&);

#           ifdef WHM_GUI
            using UiCallback_t = std::function<void(double, bool)>;
            void setUiCallback(UiCallback_t);
#           endif

            simlib3::Store* getWhItemFacility(int32_t);

            WarehouseItem_t* lookupWhLoc(int32_t);
            WarehouseItem_t* lookupWhGate(const WarehouseItemType_t&);
            std::vector<int32_t> lookupWhLocations(const std::string&, int32_t);
            WarehousePathInfo_t* lookupShortestPath(int32_t, const std::vector<int32_t>&);

        protected:
            void clearSimulation();
            void preprocessOrders();
            void prepareWhSimulation();

        private:
            bool stats;
            bool optimizationMode;
            bool multipleExperiments;

            size_t ordersFinished{ 0 };

#           ifdef WHM_GUI
            UiCallback_t uiCallback;
#           endif

            ConfigParser_t cfg;
            utils::WhmArgs_t args;

            WarehouseLayout_t& whLayout;
            WarehousePathFinder_t* whPathFinder;

            std::map<int32_t, simlib3::Store*> whFacilities;
    };


    class OrderProcessor_t : public simlib3::Process
    {
        public:
            OrderProcessor_t(WarehouseOrder_t, WarehouseSimulatorSIMLIB_t&);

        protected:
            void Behavior();

        private:
            WarehouseOrder_t order;
            WarehouseSimulatorSIMLIB_t& sim;
    };


    class OrderRequest_t : public simlib3::Event
    {
        public:
            OrderRequest_t(WarehouseLayout_t&, WarehouseSimulatorSIMLIB_t&);

        protected:
            void Behavior();

        private:
            WarehouseLayout_t& layout;
            WarehouseSimulatorSIMLIB_t& sim;
            std::vector<WarehouseOrder_t>::const_iterator it;
    };
}

#endif
