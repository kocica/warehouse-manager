/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerRAND.cpp
 * @date    11/19/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using random distribution (mostly for comparison purposes)
 */

#ifdef WHM_OPT

// Std
#include <map>
#include <utility>
#include <iostream>
#include <algorithm>

// Local
#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseSimulator.h"
#include "WarehouseLocationRack.h"
#include "WarehouseOptimizerRAND.h"

namespace whm
{
    WarehouseOptimizerRAND_t::WarehouseOptimizerRAND_t(const utils::WhmArgs_t& args_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        init();
    }

    WarehouseOptimizerRAND_t::WarehouseOptimizerRAND_t(const utils::WhmArgs_t& args_, const ConfigParser_t& cfg_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        cfg = cfg_;
        init();
    }

    void WarehouseOptimizerRAND_t::init()
    {

    }

    void WarehouseOptimizerRAND_t::optimize()
    {

    }
}

#endif
