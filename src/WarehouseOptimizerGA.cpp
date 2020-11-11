/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerGA.cpp
 * @date    10/23/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using genetic algorithms
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
#include "WarehouseOptimizerGA.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    WarehouseOptimizerGA_t::WarehouseOptimizerGA_t(utils::WhmArgs_t args_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        std::map<std::string, MutationFunctor_t> mutatorMap =
        {
            { "mutateOrdered",     std::bind(&WarehouseOptimizerGA_t::mutateOrdered,    this, std::placeholders::_1) },
            { "mutateInverse",     std::bind(&WarehouseOptimizerGA_t::mutateInverse,    this, std::placeholders::_1) }
        };

        std::map<std::string, SelectionFunctor_t> selectorMap =
        {
            { "selectRank",        std::bind(&WarehouseOptimizerGA_t::selectRank,       this, std::placeholders::_1) },
            { "selectTrunc",       std::bind(&WarehouseOptimizerGA_t::selectTrunc,      this, std::placeholders::_1) },
            { "selectTournam",     std::bind(&WarehouseOptimizerGA_t::selectTournam,    this, std::placeholders::_1) },
            { "selectRoulette",    std::bind(&WarehouseOptimizerGA_t::selectRoulette,   this, std::placeholders::_1) }
        };

        std::map<std::string, CrossoverFunctor_t> crossoverMap =
        {
            { "crossoverOrdered",  std::bind(&WarehouseOptimizerGA_t::crossoverOrdered, this, std::placeholders::_1, std::placeholders::_2) }
        };

        mutationFunctor  = mutatorMap[cfg.getAs<std::string>("mutationFunctor")];
        selectionFunctor = selectorMap[cfg.getAs<std::string>("selectionFunctor")];
        crossoverFunctor = crossoverMap[cfg.getAs<std::string>("crossoverFunctor")];
    }

    // Don't select the best fitness to keep the diversity
    Solution_t WarehouseOptimizerGA_t::selectTrunc(const std::vector<Solution_t>& pop)
    {
        return pop.at(randomFromInterval(0, cfg.getAs<int32_t>("selectionSize")));
    }

    Solution_t WarehouseOptimizerGA_t::selectTournam(const std::vector<Solution_t>& pop)
    {
        int32_t r{ 0 };
        std::vector<int32_t> generated;
        std::vector<Solution_t> selectedInds;

        for(int32_t i = 0; i < 5; ++i)
        {
            do
            {
                r = randomFromInterval(0, cfg.getAs<int32_t>("populationSize"));
            }
            while(std::find(generated.begin(), generated.end(), r) != generated.end());

            generated.push_back(r);
            selectedInds.push_back(pop.at(r));
        }

        std::sort(selectedInds.begin(), selectedInds.end(),
                  [](Solution_t& lhs, Solution_t& rhs)
                  -> bool
                  {
                      return lhs.fitness < rhs.fitness;
                  });

        return selectedInds.at(0);
    }

    Solution_t WarehouseOptimizerGA_t::selectRoulette(const std::vector<Solution_t>& pop)
    {
        // Since this is minimization problem, we need to invert fitnesses of each individual

        double maxFitness{ 0.0 };
        double sumFitness{ 0.0 };

        for(const Solution_t& ind : pop) maxFitness = std::max(ind.fitness, maxFitness);
        for(const Solution_t& ind : pop) sumFitness = sumFitness + maxFitness - ind.fitness;

        double it   = 0.0;
        double rand = randomFromInterval(0, sumFitness);

        for(const Solution_t& ind : pop)
        {
            it += maxFitness - ind.fitness;

            if(rand <= it)
            {
                return ind;
            }
        }

        return Solution_t();
    }

    Solution_t WarehouseOptimizerGA_t::selectRank(const std::vector<Solution_t>& pop)
    {
        // Note: This method has a slower convergance

        std::vector<Solution_t> popCopy = pop;

        std::sort(popCopy.begin(), popCopy.end(),
                  [](Solution_t& lhs, Solution_t& rhs)
                  -> bool
                  {
                      return lhs.fitness > rhs.fitness;
                  });

        int32_t sumFitness{ 0 };

        // Assign fitness 1 - N
        for(int32_t i = 0; i < cfg.getAs<int32_t>("populationSize"); ++i)
        {
            sumFitness += i;
            popCopy.at(i).fitness = i + 1;
        }

        int32_t it   = 0;
        int32_t rand = randomFromInterval(0, sumFitness);

        // TODO: Maybe shuffle popCopy before selection?

        for(const Solution_t& ind : popCopy)
        {
            it += ind.fitness;

            if(rand <= it)
            {
                return ind;
            }
        }

        return Solution_t();
    }

    void WarehouseOptimizerGA_t::crossoverAverage(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        for(int32_t i = 0; i < cfg.getAs<int32_t>("numberDimensions"); ++i)
        {
            auto tmp = lhsInd[i] + rhsInd[i];
            lhsInd[i] = 0.4 * tmp;
            rhsInd[i] = 0.6 * tmp;
        }
    }

    void WarehouseOptimizerGA_t::crossoverUniform(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        for(int32_t i = 0; i < cfg.getAs<int32_t>("numberDimensions"); i++)
        {
            if(flipCoin(0.5))
            {
                auto tmp  = lhsInd[i];
                lhsInd[i] = rhsInd[i];
                rhsInd[i] = tmp;
            }
        }
    }

    /// @see http://www.permutationcity.co.uk/projects/mutants/tsp.html
    void WarehouseOptimizerGA_t::crossoverOrdered(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        int32_t a, b;
        int32_t pos =  0;
        int32_t placeholder = -1;
        int32_t placeholderCount = 0;

        std::vector<int32_t> o1, o1_missing, o1_replacements;
        std::vector<int32_t> o2, o2_missing, o2_replacements;

        while(true)
        {
            do
            {
                a = randomFromInterval(pos, cfg.getAs<int32_t>("numberDimensions"));
                b = randomFromInterval(pos, cfg.getAs<int32_t>("numberDimensions"));
            }
            while(a == b);

            if(a > b) std::swap(a, b);

            // Insert from first parent
            for(int32_t i = pos; i < a; ++i)
            {
                o1.push_back(lhsInd.at(i));
                o2.push_back(rhsInd.at(i));
            }

            // Insert placeholders
            for(int32_t i = a; i < b; ++i)
            {
                ++placeholderCount;
                o1.push_back(placeholder);
                o2.push_back(placeholder);
            }

            if(b >= cfg.getAs<int32_t>("numberDimensions") - 1)
            {
                for(int32_t i = b; i < cfg.getAs<int32_t>("numberDimensions"); ++i)
                {
                    o1.push_back(lhsInd.at(i));
                    o2.push_back(rhsInd.at(i));
                }

                break;
            }
            else
            {
                pos = b;
            }
        }

        // Find missing elements
        for(int32_t i = 0; i < cfg.getAs<int32_t>("problemMax"); ++i)
        {
            if(std::find(o1.begin(), o1.end(), i) == o1.end()) o1_missing.push_back(i);
            if(std::find(o2.begin(), o2.end(), i) == o2.end()) o2_missing.push_back(i);
        }

        // Filter missing elements and leave only those which are in the second parent (keep the order)
        for(int32_t i = 0; i < static_cast<int32_t>(rhsInd.size()); i++)
        {
            if(std::find(o1_missing.begin(), o1_missing.end(), rhsInd.at(i)) != o1_missing.end()) o1_replacements.push_back(rhsInd.at(i));
        }

        // Filter missing elements and leave only those which are in the second parent (keep the order)
        for(int32_t i = 0; i < static_cast<int32_t>(lhsInd.size()); i++)
        {
            if(std::find(o2_missing.begin(), o2_missing.end(), lhsInd.at(i)) != o2_missing.end()) o2_replacements.push_back(lhsInd.at(i));
        }

        // Replace placeholders in offspring 1
        for(int32_t i = 0; i < placeholderCount; ++i)
        {
                auto it = std::find(o1.begin(), o1.end(), placeholder);
                *it     = o1_replacements.at(i);
        }

        // Replace placeholders in offspring 2
        for(int32_t i = 0; i < placeholderCount; ++i)
        {
                auto it = std::find(o2.begin(), o2.end(), placeholder);
                *it     = o2_replacements.at(i);
        }

        // Assign new offsprings
        lhsInd.assign(o1.begin(), o1.end());
        rhsInd.assign(o2.begin(), o2.end());
    }

    void WarehouseOptimizerGA_t::crossoverOnePoint(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        int32_t point = randomFromInterval(0, cfg.getAs<int32_t>("numberDimensions"));

        for(int32_t i = point; i < cfg.getAs<int32_t>("numberDimensions"); ++i)
        {
            auto tmp  = lhsInd[i];
            lhsInd[i] = rhsInd[i];
            rhsInd[i] = tmp;
        }
    }

    void WarehouseOptimizerGA_t::mutate(Solution_t& ind)
    {
        if(flipCoin(cfg.getAs<double>("probMutationInd")))
        {
            for(int32_t i = 0; i <= cfg.getAs<int32_t>("numberDimensions") * cfg.getAs<double>("probMutationGene"); i++)
            {
                mutationFunctor(ind.genes);
            }
        }
    }

    void WarehouseOptimizerGA_t::mutateRand(std::vector<int32_t>& ind)
    {
        int32_t pos = randomFromInterval(0, cfg.getAs<int32_t>("numberDimensions"));
        int32_t val = randomFromInterval(cfg.getAs<int32_t>("problemMin"), cfg.getAs<int32_t>("problemMax"));

        ind[pos] = val;
    }

    void WarehouseOptimizerGA_t::mutateGauss(std::vector<int32_t>& ind)
    {
        double sigma = (cfg.getAs<int32_t>("problemMax") - cfg.getAs<int32_t>("problemMin")) * 0.1;

        int32_t pos = randomFromInterval(0, cfg.getAs<int32_t>("numberDimensions"));
        int32_t val = randomGauss(ind[pos], sigma);

        if(val < cfg.getAs<int32_t>("problemMin")) val = cfg.getAs<int32_t>("problemMin");
        if(val > cfg.getAs<int32_t>("problemMax")) val = cfg.getAs<int32_t>("problemMax");

        ind[pos] = val;
    }

    void WarehouseOptimizerGA_t::mutateOrdered(std::vector<int32_t>& ind)
    {
        int32_t a, b;

        do
        {
            a = randomFromInterval(0, cfg.getAs<int32_t>("numberDimensions"));
            b = randomFromInterval(0, cfg.getAs<int32_t>("numberDimensions"));
        }
        while(a == b);

        if(a > b) std::swap(a, b);

        std::rotate(ind.begin() + a, ind.begin() + b, ind.begin() + b + 1);
    }

    void WarehouseOptimizerGA_t::mutateInverse(std::vector<int32_t>& ind)
    {
        int32_t a, b;

        do
        {
            a = randomFromInterval(0, cfg.getAs<int32_t>("numberDimensions"));
            b = randomFromInterval(0, cfg.getAs<int32_t>("numberDimensions"));
        }
        while(a == b);

        if(a > b) std::swap(a, b);

        std::reverse(ind.begin() + a, ind.begin() + b);
    }

    void WarehouseOptimizerGA_t::optimize()
    {
        std::vector<Solution_t> population(cfg.getAs<int32_t>("populationSize"));

        if(cfg.isSet("initialWeights"))
        {
            initPopulationWeights(population);
        }
        else
        {
            initPopulationRand(population);
        }

        for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSize"); ++p)
        {
            // Fitness = objective function (simulation time)
            population[p].fitness = simulateWarehouse(population[p].genes);
        }

        std::sort(population.begin(), population.end(),
                  [](Solution_t& lhs, Solution_t& rhs)
                  -> bool
                  {
                      return lhs.fitness < rhs.fitness;
                  });

        for(int32_t gen = 0; gen < cfg.getAs<int32_t>("maxIterations"); ++gen)
        {
            std::vector<Solution_t> newPopulation;

            for(int32_t p = 0; p < cfg.getAs<int32_t>("eliteSize"); ++p)
            {
                newPopulation.push_back(population[p]);
            }

            for(int32_t i = 0; i < (cfg.getAs<int32_t>("populationSize") - cfg.getAs<int32_t>("eliteSize")) / 2; i++)
            {
                Solution_t mum = selectionFunctor(population);
                Solution_t dad = selectionFunctor(population);

                if(flipCoin(cfg.getAs<double>("probCrossover")))
                {
                    crossoverFunctor(mum.genes, dad.genes);
                }

                Solution_t bro = mum;
                mutate(bro);

                Solution_t sis = dad;
                mutate(sis);

                newPopulation.push_back(sis);
                newPopulation.push_back(bro);
            }

            if((cfg.getAs<int32_t>("populationSize") - cfg.getAs<int32_t>("eliteSize")) & 1)
            {
                Solution_t mutant = selectionFunctor(population);

                mutate(mutant);

                newPopulation.push_back(mutant);
            }

            population = std::move(newPopulation);

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSize"); ++p)
            {
                double newFitness = simulateWarehouse(population[p].genes);

                population[p].trialValue = newFitness <= population[p].fitness ? population[p].trialValue + 1 : 0;

                population[p].fitness = newFitness;
            }

            // Discard chromosomes with trialValue > N and generate brand new offsprings (ignore elite part)
            for(int32_t p = cfg.getAs<int32_t>("eliteSize"); p < cfg.getAs<int32_t>("populationSize"); ++p)
            {
                if(population[p].trialValue > cfg.getAs<int32_t>("maxTrialValue"))
                {
                    population[p].trialValue = 0;
                    population[p].genes = std::vector<int32_t>();

                    initIndividualRand(population[p].genes);

                    population[p].fitness = simulateWarehouse(population[p].genes);
                }
            }

            std::sort(population.begin(), population.end(),
                    [](Solution_t& lhs, Solution_t& rhs)
                    -> bool
                    {
                        return lhs.fitness < rhs.fitness;
                    });

            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "[GA] [%3d] Best fitness: %f", gen, population.at(0).fitness);
            histFitness.push_back(population.at(0).fitness);

#           ifdef WHM_GUI
            if(uiCallback)
            {
                updateAllocations(population.at(0).genes);
                uiCallback(population.at(0).fitness);
            }
#           endif

            // Each N iterations save weights
            if((gen % cfg.getAs<int32_t>("saveWeightsPeriod")) == 0)
            {
                saveFitnessPlot();
                saveBestSolution(population.at(0).genes);
            }
        }

        saveFitnessPlot();
        saveBestSolution(population.at(0).genes);
    }
}

#endif
