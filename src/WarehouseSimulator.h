/**
 * Warehouse manager
 *
 * @file    WarehouseSimulator.h
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations
 */

#pragma once

#ifndef WHM_GEN

// Std
#include <map>

// Simlib
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
    class WarehouseSimulator_t
    {
        public:
            WarehouseSimulator_t();
            ~WarehouseSimulator_t();

            double runSimulation();
            void orderFinished(double, double, int32_t);

            void printStats(bool);

            ConfigParser_t& getConfig();

            utils::WhmArgs_t getArguments() const;
            void setArguments(const utils::WhmArgs_t&);

            static WarehouseSimulator_t& getWhSimulator();

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

            ConfigParser_t cfg;
            utils::WhmArgs_t args;

            WarehouseLayout_t& whLayout;
            WarehousePathFinder_t* whPathFinder;

            std::map<int32_t, simlib3::Store*> whFacilities;
    };


    class OrderProcessor_t : public simlib3::Process
    {
        public:
            OrderProcessor_t(WarehouseOrder_t<std::string>);

        protected:
            void Behavior();

        private:
            WarehouseOrder_t<std::string> order;
    };


    class OrderRequest_t : public simlib3::Event
    {
        public:
            OrderRequest_t(WarehouseLayout_t&);

        protected:
            void Behavior();

        private:
            WarehouseLayout_t& layout;

            std::vector<WarehouseOrder_t<std::string>>::const_iterator it;
    };
}

#endif
