/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizer.h
 * @date    10/23/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using genetic algorithms
 */

#pragma once

#include <map>
#include <vector>
#include <random>

#include "Utils.h"
#include "WarehouseLocationSlot.h"

namespace whm
{
    struct Solution
    {
        double fitness{ 0.0 };        //< How good chromosome is
        std::vector<int32_t> genes;   //< Chromosome
    };

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
            void initPopulationRand(std::vector<Solution>&);

            // Selection
            Solution selectTrunc(const std::vector<Solution>&);
            Solution selectTournam(const std::vector<Solution>&);

            // Crossover
            void crossoverAverage(std::vector<int32_t>&, std::vector<int32_t>&);
            void crossoverUniform(std::vector<int32_t>&, std::vector<int32_t>&);
            void crossoverOnePoint(std::vector<int32_t>&, std::vector<int32_t>&);

            // Mutate
            void mutate(Solution&);
            void mutateRand(std::vector<int32_t>&);
            void mutateGauss(std::vector<int32_t>&);
            void mutateInvert(std::vector<int32_t>&);

            // Simulation
            double simulateWarehouse(std::vector<int32_t>&);

            // Save result
            void saveBestChromosome(std::vector<int32_t>&);
            void updateAllocations(std::vector<int32_t>&);

            // Import articles
            void importProducts(std::vector<std::string>&);

        private:
            std::mt19937_64 rand;

            utils::WhmArgs_t args;

            std::map<int32_t, std::string> skuEnc;
            std::map<int32_t, WarehouseLocationSlot_t<std::string>*> slotEnc;
    };
}
