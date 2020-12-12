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
#include "WarehouseTypes.h"
#include "WarehousePathFinder.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    using PassivatedProcesses_t = std::map<int32_t, std::vector<simlib3::Process*>>;

    class WarehouseSimulatorSIMLIB_t
    {
        public:
            WarehouseSimulatorSIMLIB_t();
            ~WarehouseSimulatorSIMLIB_t();

            double runSimulation();
            void replenishmentFinished();
            void orderFinished(double, int32_t, int32_t);

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
            const std::vector<WarehouseOrder_t>& getWhOrders() const;

            WarehouseItem_t* lookupWhLoc(int32_t);
            WarehouseItem_t* lookupWhGate(const WarehouseItemType_t&);
            std::vector<int32_t> lookupWhLocations(const std::string&, int32_t);
            WarehousePathInfo_t* lookupShortestPath(int32_t, const std::vector<int32_t>&);

            void passivateProcess(int32_t, simlib3::Process*);
            void activateProcesses(int32_t);

        protected:
            void clearSimulation();
            void preprocessOrders();
            void prepareWhSimulation();

        private:
            bool showStats;
            bool optimizationMode;
            bool multipleExperiments;

#           ifdef WHM_GUI
            UiCallback_t uiCallback;
#           endif

            // Processes (orders) waiting for replenishment
            PassivatedProcesses_t passivatedProcesses;

            ConfigParser_t cfg;
            utils::WhmArgs_t args;

            WarehouseLayout_t& whLayout;
            WarehousePathFinder_t* whPathFinder;
            std::vector<WarehouseOrder_t> whOrders;
            std::map<int32_t, simlib3::Store*> whFacilities;

            // Stats
            struct SimulationStats_t
            {
                double processingTime{ 0.0 };
                size_t outboundsFinished{ 0 };
                size_t replenishmentsFinished{ 0 };
                size_t distanceTraveledConv{ 0 };
                size_t distanceTraveledWorker{ 0 };

                void dump() const;
                void reset();
            };

            SimulationStats_t stats;
    };


    class OrderProcessor_t : public simlib3::Process
    {
        public:
            OrderProcessor_t(WarehouseOrder_t, WarehouseSimulatorSIMLIB_t&);

        protected:
            void Behavior() override;
            void outboundProcessing();
            void replenishmentProcessing();

            void handleFacility(int32_t, double);

        private:
            WarehouseOrder_t order;
            WarehouseSimulatorSIMLIB_t& sim;
    };


    class OrderRequest_t : public simlib3::Event
    {
        public:
            OrderRequest_t(WarehouseSimulatorSIMLIB_t&);

        protected:
            void Behavior() override;

        private:
            WarehouseSimulatorSIMLIB_t& sim;
            std::vector<WarehouseOrder_t>::const_iterator it;
    };
}

#endif
