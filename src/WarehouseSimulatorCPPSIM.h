/**
 * Warehouse manager
 *
 * @file    WarehouseSimulatorCPPSIM.h
 * @date    10/24/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations (using C++SIM)
 */

#pragma once

#ifdef WHM_CPPSIM

// Std
#include <map>
#include <functional>

// C++SIM
#define HAVE_BOOLEAN_TYPE
#ifndef PROCESS_H_
//#  define Process CPPSIMProcess
#  include <ClassLib/Process.h>
//#  undef Process
#endif

#include <Event/Entity.h>

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
    class WarehouseSimulatorCPPSIM_t
    {
        public:
            /*WarehouseSimulatorCPPSIM_t();
            ~WarehouseSimulatorCPPSIM_t();*/

            double runSimulation();
            /*void orderFinished(double, double, int32_t);

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
            WarehousePathInfo_t* lookupShortestPath(int32_t, const std::vector<int32_t>&);*/

            static void init()
            {
                Thread::Initialize();
            }

        /*protected:
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

            std::map<int32_t, simlib3::Store*> whFacilities;*/
    };

    class WarehouseSimulatorCPPSIMInt_t : public Process
    {
        public:
            void Body() override;

            void Exit();
            void Await();
    };

    class OrderProcessorCPPSIM_t : public Process
    {
        public:
            OrderProcessorCPPSIM_t(WarehouseOrder_t, WarehouseSimulatorCPPSIMInt_t&);

        protected:
            void Body() override;

        private:
            WarehouseOrder_t order;
            WarehouseSimulatorCPPSIMInt_t& sim;
    };

    class OrderRequestCPPSIM_t : public Process
    {
        public:
            OrderRequestCPPSIM_t(WarehouseLayout_t&, WarehouseSimulatorCPPSIMInt_t&);

        protected:
            void Body() override;

        private:
            WarehouseLayout_t& layout;
            WarehouseSimulatorCPPSIMInt_t& sim;
            std::vector<WarehouseOrder_t>::const_iterator it;
    };
}

#endif
