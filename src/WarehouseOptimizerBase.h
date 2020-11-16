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
#include <functional>

#include "Utils.h"
#include "Solution.h"
#include "ConfigParser.h"
#include "WarehouseSimulator.h"
#include "WarehouseLocationSlot.h"

namespace whm
{
    class WarehouseOptimizerBase_t
    {
        public:
            WarehouseOptimizerBase_t() = delete;
            WarehouseOptimizerBase_t(utils::WhmArgs_t);
            virtual ~WarehouseOptimizerBase_t() = default;

            // Entry point
            virtual void optimize() = 0;

#           ifdef WHM_GUI
            using UiCallback_t = std::function<void(double)>;
            void setUiCallback(UiCallback_t);
#           endif

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
            void saveFitnessPlot() const;
            void saveBestSolution(std::vector<int32_t>&) const;
            void updateAllocations(const std::vector<int32_t>&) const;

        protected:
            std::mt19937 rand;
            ConfigParser_t cfg;
            utils::WhmArgs_t args;

            // Generic sku/slot encoding
            std::map<int32_t, std::string> skuEnc;
            std::map<int32_t, WarehouseLocationSlot_t*> slotEnc;

            WarehouseSimulator_t simulator;

            // Fitness history for plot
            std::vector<double> histFitness;

#           ifdef WHM_GUI
            UiCallback_t uiCallback;
#           endif
    };
}

#endif
