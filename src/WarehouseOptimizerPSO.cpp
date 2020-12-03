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

#include "Utils.h"
#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseOptimizerPSO.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    WarehouseOptimizerPSO_t::WarehouseOptimizerPSO_t(const utils::WhmArgs_t& args_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        init();
    }

    WarehouseOptimizerPSO_t::WarehouseOptimizerPSO_t(const utils::WhmArgs_t& args_, const ConfigParser_t& cfg_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        cfg = cfg_;
        init();
    }

    void WarehouseOptimizerPSO_t::init()
    {
        globalBest.fitness = std::numeric_limits<double>::max();

        std::map<std::string, CrossoverFunctor_t> crossoverMap =
        {
            { "crossoverOrdered",   std::bind(&WarehouseOptimizerPSO_t::crossoverOrdered,   this, std::placeholders::_1, std::placeholders::_2) },
            { "crossoverHeuristic", std::bind(&WarehouseOptimizerPSO_t::crossoverHeuristic, this, std::placeholders::_1, std::placeholders::_2) }
        };

        crossoverFunctor = crossoverMap[cfg.getAs<std::string>("crossoverFunctorDE")];
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
    }

    std::vector<int32_t> WarehouseOptimizerPSO_t::crossoverHeuristic(const std::vector<int32_t>& x1,
                                                                     const std::vector<int32_t>& x2)
    {
        using namespace utils;

        int32_t v = randomFromInterval(cfg.getAs<int32_t>("problemMin"), cfg.getAs<int32_t>("problemMax"));

        std::vector<int32_t> x{ v };

        int32_t i = 0;
        int32_t j = 0;

        while(i < cfg.getAs<int32_t>("numberDimensions") &&
              j < cfg.getAs<int32_t>("numberDimensions"))
        {
            if(std::find(x.begin(), x.end(), x1.at(i)) != x.end() &&
               std::find(x.begin(), x.end(), x2.at(j)) != x.end())
            {
                ++i; ++j;
            }
            else if(std::find(x.begin(), x.end(), x1.at(i)) != x.end())
            {
                x.push_back(x2.at(j)); ++j;
            }
            else if(std::find(x.begin(), x.end(), x2.at(j)) != x.end())
            {
                x.push_back(x1.at(i)); ++i;
            }
            else
            {
                if(lookupOptimalSlot(x, x1.at(i), x2.at(j)) == 1)
                {
                    x.push_back(x1.at(i)); ++i;
                }
                else
                {
                    x.push_back(x2.at(j)); ++j;
                }
            }

            if(static_cast<int32_t>(x.size()) > cfg.getAs<int32_t>("numberDimensions"))
            {
                return std::vector<int32_t>(x.begin(), x.begin() + cfg.getAs<int32_t>("numberDimensions"));
            }
        }

        while(static_cast<int32_t>(x.size()) < cfg.getAs<int32_t>("numberDimensions"))
        {
            x.push_back(lookupOptimalSlot(x));
        }

        return x;
    }

    std::vector<int32_t> WarehouseOptimizerPSO_t::crossoverOrdered(const std::vector<int32_t>& lhsInd, const std::vector<int32_t>& rhsInd)
    {
        int32_t a, b;
        int32_t pos =  0;
        int32_t placeholder = -1;
        int32_t placeholderCount = 0;

        std::vector<int32_t> o, o_missing, o_replacements;

        while(true)
        {
            do
            {
                a = randomFromInterval(pos, cfg.getAs<int32_t>("numberDimensions"));
                b = randomFromInterval(pos, cfg.getAs<int32_t>("numberDimensions"));
            }
            while(a == b);

            if(a > b) std::swap(a, b);

            for(int32_t i = pos; i < a; ++i)
            {
                o.push_back(lhsInd.at(i));
            }

            for(int32_t i = a; i < b; ++i)
            {
                ++placeholderCount;
                o.push_back(placeholder);
            }

            if(b >= cfg.getAs<int32_t>("numberDimensions") - 1)
            {
                for(int32_t i = b; i < cfg.getAs<int32_t>("numberDimensions"); ++i)
                {
                    o.push_back(lhsInd.at(i));
                }

                break;
            }
            else
            {
                pos = b;
            }
        }

        for(int32_t i = 0; i < cfg.getAs<int32_t>("problemMax"); ++i)
        {
            if(std::find(o.begin(), o.end(), i) == o.end()) o_missing.push_back(i);
        }

        for(int32_t i = 0; i < static_cast<int32_t>(rhsInd.size()); i++)
        {
            if(std::find(o_missing.begin(), o_missing.end(), rhsInd.at(i)) != o_missing.end()) o_replacements.push_back(rhsInd.at(i));
        }

        for(int32_t i = 0; i < placeholderCount; ++i)
        {
                auto it = std::find(o.begin(), o.end(), placeholder);
                *it     = o_replacements.at(i);
        }

        return o;
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

    int32_t WarehouseOptimizerPSO_t::lookupOptimalSlot(const std::vector<int32_t>& genes, int32_t x1, int32_t x2)
    {
        auto x1slot = slotEnc[x1];
        auto x2slot = slotEnc[x2];

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
            auto slots = loc->getWhLocationRack()->getSortedSlots();

            for(auto* slot : slots)
            {
                if(slot == x1slot) return 1;
                if(slot == x2slot) return 2;
            }
        }

        return -1;
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

        auto n = cfg.getAs<int32_t>("procCount");

        for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
        {
            for(size_t i = 0; i < population.at(p).genes.size(); ++i)
            {
                auto s = write(simProcesses.at(p % n).outfd, &population[p].genes.at(i), sizeof(int32_t));

                if(s < (ssize_t)sizeof(int32_t))
                {
                    whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Write failed <%d>", errno);
                    throw std::runtime_error("Write failed");
                }
            }
        }

        for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
        {
            auto s = read(simProcesses.at(p % n).infd, &population.at(p).fitness, sizeof(double));

            if(s < (ssize_t)sizeof(double))
            {
                whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Read failed <%d>", errno);
                throw std::runtime_error("Read failed");
            }
        }

        for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
        {
            personalBest.push_back(population[p]);
        }

        storeGlobalBest(personalBest);

        std::map<std::string, double> chromosomeMap;

        for(int32_t i = 0; i < cfg.getAs<int32_t>("maxIterations"); ++i)
        {
            for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
            {
                if(population[p].trialValue > cfg.getAs<int32_t>("maxTrialValue"))
                {
                    population[p].genes = std::vector<int32_t>();
                    population[p].fitness = std::numeric_limits<double>::max();

                    initIndividualRand(population[p].genes);
                }
                else
                {
                    std::vector<int32_t> v;

                    if(flipCoin(0.5))
                    {
                        v = personalBest[p].genes;
                    }
                    else
                    {
                        v = globalBest.genes;
                    }

                    population[p].genes = crossoverFunctor(population[p].genes, v);
                }

                population[p].stringGenes = utils::toString(population[p].genes);

                if(chromosomeMap.find(population[p].stringGenes) == chromosomeMap.end())
                {
                    chromosomeMap.insert(std::make_pair(population[p].stringGenes, -1));

                    for(size_t i = 0; i < population[p].genes.size(); ++i)
                    {
                        auto s = write(simProcesses.at(p % n).outfd, &population[p].genes.at(i), sizeof(int32_t));

                        if(s < (ssize_t)sizeof(int32_t))
                        {
                            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Write failed <%d>", errno);
                            throw std::runtime_error("Write failed");
                        }
                    }
                }
            }

            for(int32_t p = 0; p < cfg.getAs<int32_t>("numberParticles"); ++p)
            {
                double newFitness = chromosomeMap[population[p].stringGenes];

                if(newFitness < 0)
                {
                    auto s = read(simProcesses.at(p % n).infd, &newFitness, sizeof(double));

                    if(s < (ssize_t)sizeof(double))
                    {
                        whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Read failed <%d>", errno);
                        throw std::runtime_error("Read failed");
                    }

                    chromosomeMap[population[p].stringGenes] = newFitness;
                }

                population[p].trialValue = newFitness < population[p].fitness ? 0 : population[p].trialValue + 1;

                population[p].fitness = newFitness;

                if(population[p].fitness <= personalBest[p].fitness)
                {
                    personalBest[p] = population[p];
                }
            }

            storeGlobalBest(personalBest);

            if((i % cfg.getAs<int32_t>("saveWeightsPeriod")) == 0)
            {
                saveFitnessPlot();
                saveBestSolution(globalBest.genes);
            }

#           ifdef WHM_GUI
            if(uiCallback)
            {
                simulateWarehouse(globalBest.genes);
                //updateAllocations(globalBest.genes);
                uiCallback(globalBest.fitness);
            }
#           endif

            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "[PSO] [%3d] Best fitness: %f", i, globalBest.fitness);
            histFitness.push_back(globalBest.fitness);
        }

        saveFitnessPlot();
        saveBestSolution(globalBest.genes);
    }
}

#endif
