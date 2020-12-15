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
#include <unistd.h>
#include <stdlib.h>
#include <functional>

#include "Utils.h"
#include "Solution.h"
#include "ConfigParser.h"
#include "WarehouseLocationSlot.h"
#include "WarehouseSimulatorSIMLIB.h"

namespace whm
{
    struct SimProcess_t
    {
        pid_t pid;
        int32_t infd;
        int32_t outfd;
    };

    class WarehouseOptimizerBase_t
    {
        public:
            WarehouseOptimizerBase_t() = delete;
            WarehouseOptimizerBase_t(const utils::WhmArgs_t&);
            WarehouseOptimizerBase_t(const utils::WhmArgs_t&, const ConfigParser_t&);
            virtual ~WarehouseOptimizerBase_t();

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
            void simulationService(int32_t, int32_t);

            // Store results
            void saveFitnessPlot() const;
            void saveBestSolution(std::vector<int32_t>&) const;
            void updateAllocations(const std::vector<int32_t>&) const;

            // Pre/post processing
            int32_t calculateDistanceToExits(int32_t);
            std::vector<int32_t> precalculateArticleWeights();
            std::vector<int32_t> sortLocationArticles(std::vector<int32_t>&);

            void init();

        protected:
            std::mt19937 rand;
            ConfigParser_t cfg;
            utils::WhmArgs_t args;

            std::vector<int32_t> randChromosome;

            // Generic sku/slot encoding
            std::map<int32_t, std::string> skuEnc;
            std::map<int32_t, WarehouseLocationSlot_t*> slotEnc;

            // Simulation handler
            WarehouseSimulatorSIMLIB_t simulator;

            // Simulation services
            std::vector<SimProcess_t> simProcesses;

            // Fitness history for plot
            std::vector<double> histFitness;

            WarehousePathFinder_t pathFinder;

#           ifdef WHM_GUI
            UiCallback_t uiCallback;
#           endif
    };
}

#endif
