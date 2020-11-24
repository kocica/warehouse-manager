/**
 * Warehouse manager
 *
 * @file    WarehouseSimulatorCPPSIM.h
 * @date    10/24/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations (using C++SIM)
 */

#pragma once

#ifdef WHM_SIM

// Std
#include <map>
#include <functional>

// C++SIM
#ifndef PROCESS_H_
#  define Process CPPSIMProcess
#  include <ClassLib/Process.h>
#  undef Process
#endif

// Local
#include "Utils.h"
#include "ConfigParser.h"
#include "WarehouseItem.h"
#include "WarehouseOrder.h"
#include "WarehouseLayout.h"
#include "WarehouseItemType.h"
#include "WarehousePathFinder.h"
#include "WarehouseLocationRack.h"



#endif
