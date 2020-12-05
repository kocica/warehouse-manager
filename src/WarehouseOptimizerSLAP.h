/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerSLAP.h
 * @date    11/19/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using classical storage location assignment problem
 */

#ifdef WHM_OPT

#pragma once

#include "WarehouseOptimizerBase.h"

namespace whm
{
    class WarehouseOptimizerSLAP_t : public WarehouseOptimizerBase_t
    {
        public:
            WarehouseOptimizerSLAP_t() = delete;
            WarehouseOptimizerSLAP_t(const utils::WhmArgs_t&);
            WarehouseOptimizerSLAP_t(const utils::WhmArgs_t&, const ConfigParser_t&);
            ~WarehouseOptimizerSLAP_t() = default;

            void optimize() override;

        protected:
            void init();

            std::vector<int32_t> precalculateSlotHeats();
            std::vector<int32_t> precalculateArticleWeights();

            int32_t calculateDistanceToExits(int32_t);
            int32_t lookupOptimalSlot(const std::vector<int32_t>&);

            WarehouseItem_t* lookupExit(WarehouseItemType_t);

        private:
            WarehousePathFinder_t pathFinder;
    };
}

#endif
