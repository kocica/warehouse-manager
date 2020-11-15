/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerGA.h
 * @date    10/23/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using genetic algorithms
 */

#ifdef WHM_OPT

#pragma once

#include <functional>

#include "WarehouseOptimizerBase.h"

namespace whm
{
    class WarehouseOptimizerGA_t : public WarehouseOptimizerBase_t
    {
        using MutationFunctor_t  = std::function<void(std::vector<int32_t>&)>;
        using SelectionFunctor_t = std::function<Solution_t(const std::vector<Solution_t>&)>;
        using CrossoverFunctor_t = std::function<void(std::vector<int32_t>&, std::vector<int32_t>&)>;

        public:
            WarehouseOptimizerGA_t() = delete;
            WarehouseOptimizerGA_t(const utils::WhmArgs_t&);
            WarehouseOptimizerGA_t(const utils::WhmArgs_t&, const ConfigParser_t&);
            ~WarehouseOptimizerGA_t() = default;

            // Evolution
            void optimize() override;

        protected:
            void init();

            // Selection
            Solution_t selectRank(const std::vector<Solution_t>&);
            Solution_t selectTrunc(const std::vector<Solution_t>&);
            Solution_t selectTournam(const std::vector<Solution_t>&);
            Solution_t selectRoulette(const std::vector<Solution_t>&);

            // Crossover
            void crossoverAverage(std::vector<int32_t>&, std::vector<int32_t>&);
            void crossoverUniform(std::vector<int32_t>&, std::vector<int32_t>&);
            void crossoverOrdered(std::vector<int32_t>&, std::vector<int32_t>&);
            void crossoverOnePoint(std::vector<int32_t>&, std::vector<int32_t>&);

            // Mutate
            void mutate(Solution_t&);
            void mutateRand(std::vector<int32_t>&);
            void mutateGauss(std::vector<int32_t>&);
            void mutateOrdered(std::vector<int32_t>&);
            void mutateInverse(std::vector<int32_t>&);

        private:
            MutationFunctor_t mutationFunctor;
            SelectionFunctor_t selectionFunctor;
            CrossoverFunctor_t crossoverFunctor;
    };
}

#endif
