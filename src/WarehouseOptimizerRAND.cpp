/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerRAND.cpp
 * @date    11/19/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using random distribution (mostly for comparison purposes)
 */

#ifdef WHM_OPT

// Std
#include <map>
#include <utility>
#include <iostream>
#include <algorithm>

// Local
#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseSimulator.h"
#include "WarehouseLocationRack.h"
#include "WarehouseOptimizerRAND.h"

namespace whm
{
    WarehouseOptimizerRAND_t::WarehouseOptimizerRAND_t(const utils::WhmArgs_t& args_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        init();
    }

    WarehouseOptimizerRAND_t::WarehouseOptimizerRAND_t(const utils::WhmArgs_t& args_, const ConfigParser_t& cfg_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        cfg = cfg_;
        init();
    }

    void WarehouseOptimizerRAND_t::init()
    {
        bestSolution.fitness = std::numeric_limits<double>::max();
    }

    void WarehouseOptimizerRAND_t::optimize()
    {
        std::vector<Solution_t> population(cfg.getAs<int32_t>("populationSizeRand"));

        for(int32_t gen = 0; gen < cfg.getAs<int32_t>("maxIterations"); ++gen)
        {
            initPopulationRand(population);

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeRand"); ++p)
            {
                population[p].fitness = simulateWarehouse(population[p].genes);
            }

            std::sort(population.begin(), population.end(),
                    [](Solution_t& lhs, Solution_t& rhs)
                    -> bool
                    {
                        return lhs.fitness < rhs.fitness;
                    });

            if(population.at(0).fitness < bestSolution.fitness)
            {
                bestSolution = population.at(0);
            }

            population = std::vector<Solution_t>(cfg.getAs<int32_t>("populationSizeRand"));

            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "[RAND] [%3d] Best fitness: %f", gen, bestSolution.fitness);
            histFitness.push_back(bestSolution.fitness);

#           ifdef WHM_GUI
            if(uiCallback)
            {
                updateAllocations(bestSolution.genes);
                uiCallback(bestSolution.fitness);
            }
#           endif

            if((gen % cfg.getAs<int32_t>("saveWeightsPeriod")) == 0)
            {
                saveFitnessPlot();
                saveBestSolution(bestSolution.genes);
            }
        }

        saveFitnessPlot();
        saveBestSolution(bestSolution.genes);
    }
}

#endif
