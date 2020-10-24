/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizer.cpp
 * @date    10/23/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using genetic algorithms
 */

#ifdef WHM_OPT

// Std
#include <utility>
#include <iostream>
#include <algorithm>
#include <functional>

// Local
#include "Logger.h"
#include "ConfigParser.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseSimulator.h"
#include "WarehouseOptimizer.h"
#include "WarehouseLocationRack.h"


/// @note: Choose mutation and crossover operators in constructor
std::function<void(std::vector<int32_t>&)>                            selectedMutation;
std::function<void(std::vector<int32_t>&, std::vector<int32_t>&)>     selectedCrossover;


namespace constants
{
    whm::ConfigParser_t cfg("cfg/optimizer.xml");

    static const auto numberDimensions       = cfg.getAs<int32_t>("numberDimensions");     ///< Number of dimensions (i.e. number of SKUs)
    static const auto problemMin             = cfg.getAs<int32_t>("problemMin");           ///< Slot min number
    static const auto problemMax             = cfg.getAs<int32_t>("problemMax");           ///< Slot max number
    static const auto populationSize         = cfg.getAs<int32_t>("populationSize");       ///< Population size
    static const auto selectionSize          = cfg.getAs<int32_t>("selectionSize");        ///< Size of selected part of population
    static const auto eliteSize              = cfg.getAs<int32_t>("eliteSize");            ///< Size of elite part of population (unchanged)
    static const auto maxGenerations         = cfg.getAs<int32_t>("maxGenerations");       ///< Maximal number of generations
    static const auto probCrossover          = cfg.getAs<double>("probCrossover");         ///< Probability of crossover
    static const auto probMutationInd        = cfg.getAs<double>("probMutationInd");       ///< Probability of individual mutation
    static const auto probMutationGene       = cfg.getAs<double>("probMutationGene");      ///< Probability of gene mutation
}


namespace whm
{
    WarehouseOptimizer_t::WarehouseOptimizer_t(utils::WhmArgs_t args_)
        : args{ args_ }
    {
        selectedMutation  = std::bind(&WarehouseOptimizer_t::mutateInverse, this, std::placeholders::_1);
        selectedCrossover = std::bind(&WarehouseOptimizer_t::crossoverOrdered, this, std::placeholders::_1, std::placeholders::_2);

        std::random_device rd;
        rand.seed(rd());

        int32_t slotID{ 0 };
        int32_t articleID{ 0 };

        std::vector<std::string> articles;
        whm::WarehouseLayout_t::getWhLayout().importArticles(args.articlesPath, articles);

        for(auto& article : articles)
        {
            skuEnc.insert(std::make_pair(articleID++, article));
        }

        auto& items = whm::WarehouseLayout_t::getWhLayout().getWhItems();

        for(auto* item : items)
        {
            if(item->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
            {
                auto* rack = item->getWhLocationRack();

                for(int32_t y = 0; y < rack->getSlotCountY(); ++y)
                {
                    for(int32_t x = 0; x < rack->getSlotCountX(); ++x)
                    {
                        auto* item = &rack->at(x, y);

                        slotEnc.insert(std::make_pair(slotID++, item));
                    }
                }
            }
        }
    }

    WarehouseOptimizer_t::~WarehouseOptimizer_t()
    {

    }

    bool WarehouseOptimizer_t::flipCoin(double prob)
    {
        return randomFromInterval(0, 1) < prob;
    }

    double WarehouseOptimizer_t::randomGauss(double mi, double sigma)
    {
        std::normal_distribution<double> dist(mi, sigma);
        return dist(rand);
    }

    double WarehouseOptimizer_t::randomFromInterval(double a, double b)
    {
        std::uniform_real_distribution<> dist(a, b);
        return dist(rand);
    }

    void WarehouseOptimizer_t::initIndividualRand(std::vector<int32_t>& ind)
    {
        for(int32_t d = 0; d < constants::numberDimensions; ++d)
        {
            int32_t r{ 0 };

            do
            {
                r = randomFromInterval(constants::problemMin, constants::problemMax);
            }
            while(std::find(ind.begin(), ind.end(), r) != ind.end());

            ind.push_back(r);
        }
    }

    void WarehouseOptimizer_t::initPopulationRand(std::vector<Solution>& pop)
    {
        for(int32_t p = 0; p < static_cast<int32_t>(pop.size()); ++p)
        {
            initIndividualRand(pop[p].genes);
        }
    }

    Solution WarehouseOptimizer_t::selectTrunc(const std::vector<Solution>& pop)
    {
        return pop.at(randomFromInterval(0, constants::selectionSize));
    }

    Solution WarehouseOptimizer_t::selectTournam(const std::vector<Solution>& pop)
    {
        int32_t a = randomFromInterval(0, pop.size());
        int32_t b = randomFromInterval(0, pop.size());

        return pop[a].fitness < pop[b].fitness ? pop[a] : pop[b];
    }

    void WarehouseOptimizer_t::crossoverAverage(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        for(int32_t i = 0; i < constants::numberDimensions; ++i)
        {
            auto tmp = lhsInd[i] + rhsInd[i];
            lhsInd[i] = 0.4 * tmp;
            rhsInd[i] = 0.6 * tmp;
        }
    }

    void WarehouseOptimizer_t::crossoverUniform(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        for(int32_t i = 0; i < constants::numberDimensions; i++)
        {
            if(flipCoin(0.5))
            {
                auto tmp  = lhsInd[i];
                lhsInd[i] = rhsInd[i];
                rhsInd[i] = tmp;
            }
        }
    }

    void WarehouseOptimizer_t::crossoverOrdered(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        const int32_t placeholder = -1;
        int32_t a = randomFromInterval(0, constants::numberDimensions);
        int32_t b = randomFromInterval(0, constants::numberDimensions);

        std::vector<int32_t> o1, o1_missing;
        std::vector<int32_t> o2, o2_missing;

        if(a > b) std::swap(a, b);

        for(int32_t i = 0; i < a; ++i)
        {
            o1.push_back(lhsInd.at(i));
            o2.push_back(rhsInd.at(i));
        }

        // Insert placeholders
        for(int32_t i = a; i < b; ++i)
        {
            o1.push_back(placeholder);
            o2.push_back(placeholder);
        }

        for(int32_t i = b; i < constants::numberDimensions; ++i)
        {
            o1.push_back(lhsInd.at(i));
            o2.push_back(rhsInd.at(i));
        }

        // Find missing elements
        for(int32_t i = 0; i < constants::numberDimensions; ++i)
        {
            if(std::find(o1.begin(), o1.end(), i) == o1.end()) o1_missing.push_back(i);
            if(std::find(o2.begin(), o2.end(), i) == o2.end()) o2_missing.push_back(i);
        }

        // Replace placeholders
        for(int32_t i = 0; i < constants::numberDimensions; ++i)
        {
            if(std::find(o1_missing.begin(), o1_missing.end(), rhsInd.at(i)) != o1_missing.end())
            {
                auto it = std::find(o1.begin(), o1.end(), placeholder);
                *it     = rhsInd.at(i);
            }
            if(std::find(o2_missing.begin(), o2_missing.end(), lhsInd.at(i)) != o2_missing.end())
            {
                auto it = std::find(o2.begin(), o2.end(), placeholder);
                *it     = lhsInd.at(i);
            }
        }

        lhsInd.assign(o1.begin(), o1.end());
        rhsInd.assign(o2.begin(), o2.end());
    }

    void WarehouseOptimizer_t::crossoverOnePoint(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        int32_t point = randomFromInterval(0, constants::numberDimensions);

        for(int32_t i = point; i < constants::numberDimensions; ++i)
        {
            auto tmp  = lhsInd[i];
            lhsInd[i] = rhsInd[i];
            rhsInd[i] = tmp;
        }
    }

    void WarehouseOptimizer_t::mutate(Solution& ind)
    {
        if(flipCoin(constants::probMutationInd))
        {
            for(int32_t i = 0; i <= constants::numberDimensions * constants::probMutationGene; i++)
            {
                selectedMutation(ind.genes);
            }
        }
    }

    void WarehouseOptimizer_t::mutateRand(std::vector<int32_t>& ind)
    {
        int32_t pos = randomFromInterval(0, constants::numberDimensions);
        int32_t val = randomFromInterval(constants::problemMin, constants::problemMax);

        ind[pos] = val;
    }

    void WarehouseOptimizer_t::mutateGauss(std::vector<int32_t>& ind)
    {
        double sigma = (constants::problemMax - constants::problemMin) * 0.1;

        int32_t pos = randomFromInterval(0, constants::numberDimensions);
        int32_t val = randomGauss(ind[pos], sigma);

        if(val < constants::problemMin) val = constants::problemMin;
        if(val > constants::problemMax) val = constants::problemMax;

        ind[pos] = val;
    }

    void WarehouseOptimizer_t::mutateOrdered(std::vector<int32_t>& ind)
    {
        int32_t a = randomFromInterval(0, constants::numberDimensions);
        int32_t b = randomFromInterval(0, constants::numberDimensions);

        if(a == b) return;

        ind.insert(ind.begin() + b, ind.at(a));
        ind.erase (ind.begin() + a);
    }

    void WarehouseOptimizer_t::mutateInverse(std::vector<int32_t>& ind)
    {
        int32_t a = randomFromInterval(0, constants::numberDimensions);
        int32_t b = randomFromInterval(0, constants::numberDimensions);

        if(a > b) std::swap(a, b);

        std::reverse(ind.begin() + a, ind.begin() + b);
    }

    void WarehouseOptimizer_t::evolve()
    {
        std::vector<Solution> population(constants::populationSize);

        initPopulationRand(population);

        for(int32_t p = 0; p < constants::populationSize; ++p)
        {
            population[p].fitness = simulateWarehouse(population[p].genes);
        }

        std::sort(population.begin(), population.end(),
                  [](Solution& lhs, Solution& rhs)
                  -> bool
                  {
                      return lhs.fitness < rhs.fitness;
                  });

        for(int32_t gen = 0; gen < constants::maxGenerations; ++gen)
        {
            std::vector<Solution> newPopulation;

            for(int32_t p = 0; p < constants::eliteSize; ++p)
            {
                newPopulation.push_back(population[p]);
            }

            for(int32_t i = 0; i < (constants::populationSize - constants::eliteSize) / 2; i++)
            {
                Solution mum = selectTrunc(population);
                Solution dad = selectTrunc(population);

                if(flipCoin(constants::probCrossover))
                {
                    selectedCrossover(mum.genes, dad.genes);
                }

                Solution bro = mum;
                mutate(bro);

                Solution sis = dad;
                mutate(sis);

                newPopulation.push_back(sis);
                newPopulation.push_back(bro);
            }

            if((constants::populationSize - constants::eliteSize) & 1)
            {
                Solution mutant = selectTrunc(population);

                mutate(mutant);

                newPopulation.push_back(mutant);
            }

            population = std::move(newPopulation);

            for(int32_t p = 0; p < constants::populationSize; ++p)
            {
                population[p].fitness = simulateWarehouse(population[p].genes);
            }

            std::sort(population.begin(), population.end(),
                    [](Solution& lhs, Solution& rhs)
                    -> bool
                    {
                        return lhs.fitness < rhs.fitness;
                    });

            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "==== Generation %2d ====", gen);
        }

        saveBestChromosome(population.at(0).genes);
    }

    void WarehouseOptimizer_t::updateAllocations(std::vector<int32_t>& ind)
    {
        for(int32_t i = 0; i < static_cast<int32_t>(ind.size()); ++i)
        {
            auto itSku  = skuEnc.find(i);
            auto itSlot = slotEnc.find(ind.at(i));

            itSlot->second->setArticle(itSku->second);
        }
    }

    double WarehouseOptimizer_t::simulateWarehouse(std::vector<int32_t>& ind)
    {
        // Update article/location allocation
        updateAllocations(ind);

        // Run simulation and return elapsed time
        whm::WarehouseSimulator_t::getWhSimulator().setArguments(args);
        return whm::WarehouseSimulator_t::getWhSimulator().runSimulation();
    }

    void WarehouseOptimizer_t::saveBestChromosome(std::vector<int32_t>& chromosome)
    {
        // Update article/location allocation
        updateAllocations(chromosome);

        // Export allocation to a csv file
        whm::WarehouseLayout_t::getWhLayout().exportLocationSlots(args.locationsPath);
    }
}

#endif
