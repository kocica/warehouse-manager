/**
 * Warehouse manager
 *
 * @file    WarehouseSimulatorCPPSIM.h
 * @date    10/24/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations (using C++SIM)
 */

#pragma once

#ifdef WHM_SIM_2

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
#include "WarehouseItemType.h"
#include "WarehousePathFinder.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    class WarehouseSimulatorCPPSIM_t : public Entity
    {
        public:
            void Body() override;
    };

    class OrderRequestCPPSIM_t : public Process
    {
        public:
            OrderRequestCPPSIM_t(WarehouseLayout_t&, WarehouseSimulatorCPPSIM_t&);

        protected:
            void Body() override;

        private:
            WarehouseLayout_t& layout;
            WarehouseSimulatorCPPSIM_t& sim;
            std::vector<WarehouseOrder_t>::const_iterator it;
            int c;
    };
}

#endif
