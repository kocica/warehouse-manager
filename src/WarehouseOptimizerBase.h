/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerBase.h
 * @date    10/23/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse
 */

#ifdef WHM_OPT

#pragma once

#include <map>
#include <vector>
#include <random>

#include "Utils.h"
#include "ConfigParser.h"
#include "WarehouseLocationSlot.h"

namespace whm
{
    struct Solution_t
    {
        double fitness{ 0.0 };        //< How good solution is
        int32_t trialValue{ 0 };      //< Counter of iterations fitness did not improve
        std::vector<int32_t> genes;   //< Solution
    };

    class WarehouseOptimizerBase_t
    {
        public:
            WarehouseOptimizerBase_t() = delete;
            WarehouseOptimizerBase_t(utils::WhmArgs_t);
            virtual ~WarehouseOptimizerBase_t() = default;

            // Entry point
            virtual void optimize() = 0;

        protected:
            // Generators
            bool flipCoin(double);
            double randomGauss(double, double);
            double randomFromInterval(double, double);

            // Initialization
            void initIndividualRand(std::vector<int32_t>&);
            void initPopulationRand(std::vector<Solution_t>&);
            void initPopulationWeights(std::vector<Solution_t>&);

            // Simulation
            double simulateWarehouse(std::vector<int32_t>&);

            // Store results
            void saveBestSolution(std::vector<int32_t>&);
            void updateAllocations(std::vector<int32_t>&);

        protected:
            std::mt19937 rand;
            ConfigParser_t cfg;
            utils::WhmArgs_t args;

            // Generic sku/slot encoding
            std::map<int32_t, std::string> skuEnc;
            std::map<int32_t, WarehouseLocationSlot_t<std::string>*> slotEnc;
    };
}

#endif
