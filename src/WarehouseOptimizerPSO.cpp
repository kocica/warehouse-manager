/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerPSO.cpp
 * @date    10/30/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using particle swarm optimization
 */

#ifdef WHM_OPT

#include <limits>
#include <cstdint>
#include <iostream>
#include <algorithm>

#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseOptimizerPSO.h"
#include "WarehouseLocationRack.h"

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

        histFitness.push_back(globalBest.fitness);
    }

    double WarehouseOptimizerPSO_t::getVelocity(Solution_t& sol, double weighing, double random)
    {
        return (1 / sol.fitness) * random * weighing;
    }

    void WarehouseOptimizerPSO_t::updateVelocities(std::vector<Solution_t>& pop)
    {
        for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
        {
            double currentVel  = getVelocity(pop[p],          cfg.getAs<double>("weighing"), 1);
            double persBestVel = getVelocity(personalBest[p], cfg.getAs<double>("correctionFactor1"), randomFromInterval(0, 1));
            double globBestVel = getVelocity(globalBest,      cfg.getAs<double>("correctionFactor2"), randomFromInterval(0, 1));
            double totalVel    = currentVel + persBestVel + globBestVel;

            int32_t currLen = (currentVel  / totalVel) * cfg.getAs<int32_t>("numberDimensions");
            int32_t persLen = (persBestVel / totalVel) * cfg.getAs<int32_t>("numberDimensions");
            int32_t globLen = (globBestVel / totalVel) * cfg.getAs<int32_t>("numberDimensions");

            std::vector<int32_t> currPart    = getSolutionPart(pop[p].genes, currLen);
            std::vector<int32_t> persPart    = getSolutionPart(personalBest[p].genes, persLen);
            std::vector<int32_t> globPart    = getSolutionPart(globalBest.genes, globLen);
            std::vector<int32_t> mergedParts = mergeSolutionParts(currPart, persPart, globPart);

            while(static_cast<int32_t>(mergedParts.size()) < cfg.getAs<int32_t>("numberDimensions"))
            {
                int32_t slot = lookupOptimalSlot(mergedParts);

                mergedParts.push_back(slot);
            }

            pop[p].genes = mergedParts;
            double f = simulateWarehouse(pop[p].genes);

            if(f <= pop[p].fitness)
            {
                pop[p].trialValue++;

                if(pop[p].trialValue > cfg.getAs<int32_t>("maxTrialValue"))
                {
                    pop[p].trialValue = 0;
                    pop[p].genes = std::vector<int32_t>();

                    initIndividualRand(pop[p].genes);

                    pop[p].fitness = simulateWarehouse(pop[p].genes);
                }
            }
            else
            {
                pop[p].fitness = f;
            }

            if(pop[p].fitness < personalBest[p].fitness)
            {
                personalBest[p] = pop[p];
            }
        }
    }

    int32_t WarehouseOptimizerPSO_t::lookupOptimalSlot(const std::vector<int32_t>& genes)
    {
        std::vector<WarehouseItem_t*> locations;

        updateAllocations(genes);

        for(auto* item : whm::WarehouseLayout_t::getWhLayout().getWhItems())
        {
            if(item->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
            {
                locations.push_back(item);
            }
        }

        std::sort(locations.begin(), locations.end(), [](auto* lhs, auto* rhs) -> bool
                                                      {
                                                          return lhs->getWhLocationRack()->getOccupationLevel() <
                                                                 rhs->getWhLocationRack()->getOccupationLevel() ;
                                                      });

        for(auto const& loc : locations)
        {
            auto slot = loc->getWhLocationRack()->getFirstFreeSlot();

            if(slot)
            {
                for(auto e : slotEnc)
                {
                    if(slot == e.second)
                    {
                        return e.first;
                    }
                }
            }
        }

        return -1;
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

        globalBest.fitness = std::numeric_limits<double>::max();

        storeGlobalBest(population);

        for(int32_t i = 0; i < cfg.getAs<int32_t>("maxIterations"); ++i)
        {
            updateVelocities(population);

            storeGlobalBest(population);

            if((i % cfg.getAs<int32_t>("saveWeightsPeriod")) == 0)
            {
                saveFitnessPlot();
                saveBestSolution(globalBest.genes);
            }

            std::cout << "[PSO] Best Fitness: " << globalBest.fitness << std::endl;
        }

        saveFitnessPlot();
        saveBestSolution(globalBest.genes);
    }
}

#endif
