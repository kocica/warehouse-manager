/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerDE.h
 * @date    10/30/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using differential evolution
 */

#ifdef WHM_OPT

#pragma once

#include <utility>

#include "WarehouseOptimizerBase.h"

namespace whm
{
    using ProbGene_t  = std::pair<int32_t, double>;
    using ProbGenes_t = std::vector<ProbGene_t>;

    class WarehouseOptimizerDE_t : public WarehouseOptimizerBase_t
    {
        public:
            WarehouseOptimizerDE_t() = delete;
            WarehouseOptimizerDE_t(utils::WhmArgs_t);
            ~WarehouseOptimizerDE_t() = default;

            void optimize() override;

        protected:
            // Operators
            ProbGenes_t operatorMul(const ProbGenes_t&, double);
            ProbGenes_t operatorAdd(const ProbGenes_t&, const ProbGenes_t&);
            ProbGenes_t operatorSub(const ProbGenes_t&, const ProbGenes_t&);

            // Mutation
            ProbGenes_t mutate(const std::vector<Solution_t>&, int32_t);

            // Selection
            std::vector<int32_t> rand(const std::vector<Solution_t>&, int32_t);
            std::vector<int32_t> best(const std::vector<Solution_t>&, int32_t);

            // Crossover
            ProbGenes_t crossover(const ProbGenes_t&, const ProbGenes_t&, int32_t);
            std::vector<int32_t> crossoverOrdered(const std::vector<int32_t>&, const std::vector<int32_t>&);

            // Utils
            int32_t lookupOptimalSlot(const std::vector<int32_t>&);
            ProbGenes_t getRemainingSet(const ProbGenes_t&, double);
            ProbGenes_t genesToProbGenes(const std::vector<int32_t>&);
            std::vector<int32_t> probGenesToGenes(const ProbGenes_t&);

        private:
            Solution_t bestInd;
    };
}

#endif
