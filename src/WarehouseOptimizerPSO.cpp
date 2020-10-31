/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerPSO.cpp
 * @date    10/30/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using particle swarm optimization
 */

#ifdef WHM_OPT

#include <float.h>

#include "WarehouseOptimizerPSO.h"

namespace whm
{
    WarehouseOptimizerPSO_t::WarehouseOptimizerPSO_t(utils::WhmArgs_t args_)
        : WarehouseOptimizerBase_t{ args_ }
    {

    }

    void WarehouseOptimizerPSO_t::storeGlobalBest(std::vector<Solution_t>& pop)
    {
        Solution_t actualBest = pop[0];

        for(int32_t p = 1; p < cfg.getAs<int32_t>("numberParticles"); ++p)
        {
            if(pop[p].fitness < actualBest.fitness)
            {
                actualBest = pop[p];
            }
        }

        if(actualBest.fitness < globalBest.fitness)
        {
            globalBest = actualBest;
        }

        std::cout << "Best Fitness: " << globalBest.fitness << std::endl;
    }

    double WarehouseOptimizerPSO_t::getVelocity(Solution_t& sol, double weighing, double random)
    {
        //     (1 - (1 / sol.fitness)) * random * weighing;
        return (1 / sol.fitness) * random * weighing;
    }

    void WarehouseOptimizerPSO_t::updateVelocities(std::vector<Solution_t>& pop)
    {
        for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
        {
            double currentVel  = getVelocity(pop[p],          cfg.getAs<double>("w"),  1);
            double persBestVel = getVelocity(personalBest[p], cfg.getAs<double>("c1"), randomFromInterval(0, 1));
            double globBestVel = getVelocity(globalBest,      cfg.getAs<double>("c2"), randomFromInterval(0, 1));
            double totalVel    = currentVel + persBestVel + globBestVel;

            int32_t currLen = (currentVel  / totalVel) * cfg.getAs<int32_t>("numberDimensions");
            int32_t persLen = (persBestVel / totalVel) * cfg.getAs<int32_t>("numberDimensions");
            int32_t globLen = (globBestVel / totalVel) * cfg.getAs<int32_t>("numberDimensions");

            std::vector<int32_t> currPart    = getSolutionPart(pop[p].genes, currLen);
            std::vector<int32_t> persPart    = getSolutionPart(personalBest[p].genes, persLen);
            std::vector<int32_t> globPart    = getSolutionPart(globalBest.genes, globLen);
            std::vector<int32_t> mergedParts = mergeSolutionParts(currPart, persPart, globPart);

            std::vector<int32_t> leftover;

            for(int32_t i = cfg.getAs<int32_t>("problemMin"); i < cfg.getAs<int32_t>("problemMax"); ++i)
            {
                if(std::find(mergedParts.begin(), mergedParts.end(), i) == mergedParts.end())
                {
                    leftover.push_back(i);
                }
            }

            for(int32_t v : leftover)
            {
                if(static_cast<int32_t>(mergedParts.size()) == cfg.getAs<int32_t>("numberDimensions"))
                {
                    break;
                }

                if(std::find(pop[p].genes.begin(), pop[p].genes.end(), v) != pop[p].genes.end())
                {
                    mergedParts.push_back(v);
                }
            }

            pop[p].genes = mergedParts;
            pop[p].fitness = simulateWarehouse(pop[p].genes);

            if(pop[p].fitness < personalBest[p].fitness)
            {
                personalBest[p] = pop[p];
            }
        }
    }

    std::vector<int32_t> WarehouseOptimizerPSO_t::getSolutionPart(std::vector<int32_t>& sol, int32_t len)
    {
        std::vector<int32_t> res;

        if(len <= 0)
        {
            return res;
        }

        int32_t pos = randomFromInterval(0, cfg.getAs<int32_t>("numberDimensions") - len);

        for(int32_t i = pos; i < (pos + len); ++i)
        {
            res.push_back(sol.at(i));
        }

        return res;
    }

    std::vector<int32_t> WarehouseOptimizerPSO_t::mergeSolutionParts(std::vector<int32_t>& a,
                                                                     std::vector<int32_t>& b,
                                                                     std::vector<int32_t>& c)
    {
        std::vector<int32_t> res;

        for(int32_t v : a)
        {
            res.push_back(v);
        }

        for(int32_t v : b)
        {
            if(std::find(res.begin(), res.end(), v) == res.end())
            {
                res.push_back(v);
            }
        }

        for(int32_t v : c)
        {
            if(std::find(res.begin(), res.end(), v) == res.end())
            {
                res.push_back(v);
            }
        }

        return res;
    }

    void WarehouseOptimizerPSO_t::optimize()
    {
        std::vector<Solution_t> population(cfg.getAs<int32_t>("numberParticles"));

        if(cfg.isSet("initialWeights"))
        {
            initPopulationWeights(population);
        }
        else
        {
            initPopulationRand(population);
        }

        for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
        {
            population[p].fitness = simulateWarehouse(population[p].genes);
        }

        for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
        {
            personalBest.push_back(population[p]);
        }

        globalBest.fitness = DBL_MAX;

        storeGlobalBest(population);

        for(int32_t i = 0; i < cfg.getAs<int32_t>("maxIterations"); ++i)
        {
            std::cout << "Iteration " << i << std::endl;

            for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
            {
                std::cout << population[p].fitness << " ";
            }
            std::cout << std::endl;

            updateVelocities(population);

            storeGlobalBest(population);

            if((i % cfg.getAs<int32_t>("saveWeightsPeriod")) == 0)
            {
                saveBestSolution(globalBest.genes);
            }
        }

        saveBestSolution(globalBest.genes);
    }
}

#endif
