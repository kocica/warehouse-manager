/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerRAND.h
 * @date    11/19/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using random distribution (mostly for comparison purposes)
 */

#ifdef WHM_OPT

#pragma once

#include "WarehouseOptimizerBase.h"

namespace whm
{
    class WarehouseOptimizerRAND_t : public WarehouseOptimizerBase_t
    {
        public:
            WarehouseOptimizerRAND_t() = delete;
            WarehouseOptimizerRAND_t(const utils::WhmArgs_t&);
            WarehouseOptimizerRAND_t(const utils::WhmArgs_t&, const ConfigParser_t&);
            ~WarehouseOptimizerRAND_t() = default;

            void optimize() override;

        protected:
            void init();

        private:
            Solution_t bestSolution;
    };
}

#endif
