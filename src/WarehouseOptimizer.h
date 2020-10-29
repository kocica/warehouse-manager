/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizer.h
 * @date    10/23/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using genetic algorithms
 */

#ifdef WHM_OPT

#pragma once

#include <map>
#include <vector>
#include <random>
#include <functional>

#include "Utils.h"
#include "WarehouseLocationSlot.h"

namespace whm
{
    struct Solution_t
    {
        double fitness{ 0.0 };        //< How good chromosome is
        std::vector<int32_t> genes;   //< Chromosome
    };

    using MutationFunctor_t  = std::function<void(std::vector<int32_t>&)>;
    using SelectionFunctor_t = std::function<Solution_t(const std::vector<Solution_t>&)>;
    using CrossoverFunctor_t = std::function<void(std::vector<int32_t>&, std::vector<int32_t>&)>;

    class WarehouseOptimizer_t
    {
        public:
            WarehouseOptimizer_t(utils::WhmArgs_t);
            ~WarehouseOptimizer_t();

            // Evolution
            void evolve();

        protected:

            // Generators
            bool flipCoin(double);
            double randomGauss(double, double);
            double randomFromInterval(double, double);

            // Initialize
            void initIndividualRand(std::vector<int32_t>&);
            void initPopulationRand(std::vector<Solution_t>&);

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

            // Simulation
            double simulateWarehouse(std::vector<int32_t>&);

            // Store results
            void updateAllocations(std::vector<int32_t>&);
            void saveBestChromosome(std::vector<int32_t>&);

        private:
            std::mt19937_64 rand;
            utils::WhmArgs_t args;

            std::map<int32_t, std::string> skuEnc;
            std::map<int32_t, WarehouseLocationSlot_t<std::string>*> slotEnc;

            MutationFunctor_t  mutationFunctor;
            SelectionFunctor_t selectionFunctor;
            CrossoverFunctor_t crossoverFunctor;
    };
}

#endif
