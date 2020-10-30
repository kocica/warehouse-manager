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

#include "WarehouseOptimizerBase.h"

namespace whm
{
    class WarehouseOptimizerABC_t : public WarehouseOptimizerBase_t
    {
        public:
            WarehouseOptimizerABC_t() = delete;
            WarehouseOptimizerABC_t(utils::WhmArgs_t);
            ~WarehouseOptimizerABC_t() = default;

            // Entry point
            void optimize() override;

        protected:
            // Artificial bee colony phases
            void scoutBeePhase(std::vector<Solution_t>&);
            void employedBeePhase(std::vector<Solution_t>&);
            void onlookerBeePhase(std::vector<Solution_t>&);

        protected:
            bool isBestSolution(const Solution_t&);
            void updateBee(std::vector<Solution_t>&, int32_t);
            void memorizeBestSolution(std::vector<Solution_t>&);

        private:
            Solution_t bestSolution;
    };
}

#endif
