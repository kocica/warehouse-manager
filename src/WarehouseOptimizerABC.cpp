/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerABC.cpp
 * @date    10/30/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using artificial bee colony
 */

#ifdef WHM_OPT

#include "WarehouseOptimizerABC.h"

namespace whm
{
    WarehouseOptimizerABC_t::WarehouseOptimizerABC_t(utils::WhmArgs_t args_)
        : WarehouseOptimizerBase_t{ args_ }
    {

    }

    void WarehouseOptimizerABC_t::employedBeePhase(std::vector<Solution_t>& pop)
    {
        for(int p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
        {
            updateBee(pop, p);
        }
    }

    void WarehouseOptimizerABC_t::onlookerBeePhase(std::vector<Solution_t>& pop)
    {
        double sumFitness{ 0.0 };
        int32_t updateCounter{ 0 };

        for(const Solution_t& sol : pop)
        {
            sumFitness += sol.fitness;
        }

        do
        {
            for(int p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
            {
                double prob = pop[p].fitness / sumFitness;

                if(flipCoin(prob))
                {
                    updateCounter++;
                    updateBee(pop, p);

                    if(updateCounter == cfg.getAs<int32_t>("foodSize"))
                    {
                        break;
                    }
                }
            }
        }
        while(updateCounter < cfg.getAs<int32_t>("foodSize"));
    }

    void WarehouseOptimizerABC_t::updateBee(std::vector<Solution_t>& pop, int32_t p)
    {
        // Select random variable to change
        int32_t randVar = randomFromInterval(0, cfg.getAs<int32_t>("numberDimensions"));

        // Find available values
        std::vector<int32_t> availableVals;
        for(int32_t i = 0; i < cfg.getAs<int32_t>("problemMax"); ++i)
        {
            if(std::find(pop[p].genes.begin(), pop[p].genes.end(), i) == pop[p].genes.end())
            {
                availableVals.push_back(i);
            }
        }

        // Select random partner (/= actual one)
        int32_t randPar{ 0 };

        do
        {
            randPar = randomFromInterval(0, cfg.getAs<int32_t>("foodSize"));
        }
        while(pop[p].genes[randVar] == pop[randPar].genes[randVar]);

        // TODO: Create new food location (check how its solved TSP!)
        int32_t randLoc{ 0 };

        std::vector<int32_t> availableValsRange;

        if(pop[p].genes[randVar] < pop[randPar].genes[randVar])
        {
            for(int32_t i = pop[p].genes[randVar]; i < pop[randPar].genes[randVar]; ++i)
            {
                if(std::find(availableVals.begin(), availableVals.end(), i) != availableVals.end())
                {
                    availableValsRange.push_back(i);
                }
            }
        }
        else
        {
            for(int32_t i = pop[randPar].genes[randVar]; i < pop[p].genes[randVar]; ++i)
            {
                if(std::find(availableVals.begin(), availableVals.end(), i) != availableVals.end())
                {
                    availableValsRange.push_back(i);
                }
            }
        }

        if(!availableValsRange.empty())
        {
            int32_t randPos = randomFromInterval(0, availableValsRange.size());
            randLoc = availableValsRange[randPos];
        }
        else
        {
            int32_t randPos = randomFromInterval(0, availableVals.size());
            randLoc = availableVals[randPos];
        }

        // Create new allocation
        auto sol = pop[p].genes;
        sol[randVar] = randLoc;

        // Calculate fitness for new location
        double fitness = simulateWarehouse(sol);

        // In case its better solution, update food
        if(fitness < pop[p].fitness)
        {
            pop[p].fitness = fitness;
            pop[p].trialValue = 0;
            pop[p].genes = sol;
        }
        else
        {
            pop[p].trialValue++;
        }
    }

    void WarehouseOptimizerABC_t::scoutBeePhase(std::vector<Solution_t>& pop)
    {
        for(int32_t p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
        {
            if(pop[p].trialValue > cfg.getAs<int32_t>("maxTrialValue"))
            {
                pop[p].trialValue = 0;
                pop[p].genes = std::vector<int32_t>();

                initIndividualRand(pop[p].genes);

                pop[p].fitness = simulateWarehouse(pop[p].genes);
            }
        }
    }

    void WarehouseOptimizerABC_t::memorizeBestSolution(std::vector<Solution_t>& pop)
    {
        bestSolution = pop[0];

        for(int32_t p = 1; p < cfg.getAs<int32_t>("foodSize"); ++p)
        {
            if(pop[p].fitness < bestSolution.fitness)
            {
                bestSolution = pop[p];
            }
        }

        std::cout << "Best Fitness: " << bestSolution.fitness << std::endl;
    }

    void WarehouseOptimizerABC_t::optimize()
    {
        std::vector<Solution_t> population(cfg.getAs<int32_t>("foodSize"));

        if(cfg.isSet("initialWeights"))
        {
            initPopulationWeights(population);
        }
        else
        {
            initPopulationRand(population);
        }

        for(int32_t p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
        {
            // Fitness = objective function (simulation time)
            population[p].fitness = simulateWarehouse(population[p].genes);
        }

        for(int32_t i = 0; i < cfg.getAs<int32_t>("maxGenerations"); ++i)
        {
            std::cout << "Iteration " << i << std::endl;

            employedBeePhase(population);
            onlookerBeePhase(population);

            memorizeBestSolution(population);

            scoutBeePhase(population);
        }
    }
}

#endif
