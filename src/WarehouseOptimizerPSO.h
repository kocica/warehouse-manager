/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerPSO.h
 * @date    10/30/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using particle swarm optimization
 */

#ifdef WHM_OPT

#pragma once

#include "WarehouseOptimizerBase.h"

namespace whm
{
    class WarehouseOptimizerPSO_t : public WarehouseOptimizerBase_t
    {
        public:
            WarehouseOptimizerPSO_t() = delete;
            WarehouseOptimizerPSO_t(utils::WhmArgs_t);
            ~WarehouseOptimizerPSO_t() = default;

            void optimize() override;

        protected:
            void storeGlobalBest(std::vector<Solution_t>&);
            void updateVelocities(std::vector<Solution_t>&);
            double getVelocity(Solution_t&, double, double);

            std::vector<int32_t> getSolutionPart(std::vector<int32_t>&,
                                                 int32_t);
            std::vector<int32_t> mergeSolutionParts(std::vector<int32_t>&,
                                                    std::vector<int32_t>&,
                                                    std::vector<int32_t>&);

        private:
            Solution_t globalBest;
            std::vector<Solution_t> personalBest;
    };
}

#endif
