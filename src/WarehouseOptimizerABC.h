/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerABC.h
 * @date    10/30/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using artificial bee colony
 */

#ifdef WHM_OPT

#pragma once

#include <utility>

#include "WarehouseOptimizerBase.h"

namespace whm
{
    using SwapOperator_t = std::pair<int32_t, int32_t>;

    class WarehouseOptimizerABC_t : public WarehouseOptimizerBase_t
    {
        public:
            WarehouseOptimizerABC_t() = delete;
            WarehouseOptimizerABC_t(utils::WhmArgs_t);
            ~WarehouseOptimizerABC_t() = default;

            // Entry point
            void optimize() override;

        protected:
            // ABC phases
            void scoutBeePhase(std::vector<Solution_t>&);
            void employedBeePhase(std::vector<Solution_t>&);
            void onlookerBeePhase(std::vector<Solution_t>&);

            // Helpers
            int32_t selectTechnique();
            bool isBestSolution(const Solution_t&);
            void updateBee(std::vector<Solution_t>&, int32_t);
            void memorizeBestSolution(std::vector<Solution_t>&);

            // Swap operators
            std::vector<SwapOperator_t> filterSwap(const std::vector<SwapOperator_t>&);
            std::vector<SwapOperator_t> getSwap(std::vector<int32_t>&, std::vector<int32_t>&);
            std::vector<int32_t> applySwap(std::vector<int32_t>&, std::vector<SwapOperator_t>&);

        private:
            Solution_t bestSolution;
            Solution_t worstSolution;

            std::vector<int32_t> ruleCounters;
    };
}

#endif
