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

// Local
#include "Logger.h"
#include "ConfigParser.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseSimulator.h"
#include "WarehouseOptimizer.h"
#include "WarehouseLocationRack.h"


namespace constants
{
    whm::ConfigParser_t parser("cfg/optimizer.xml");

    static const int numberDimensions       = parser.getAs<int32_t>("numberDimensions");     ///< Number of dimensions (i.e. number of SKUs)
    static const int problemMin             = parser.getAs<int32_t>("problemMin");           ///< Slot min number
    static const int problemMax             = parser.getAs<int32_t>("problemMax");           ///< Slot max number
    static const int populationSize         = parser.getAs<int32_t>("populationSize");       ///< Population size
    static const int selectionSize          = parser.getAs<int32_t>("selectionSize");        ///< Size of selected part of population
    static const int eliteSize              = parser.getAs<int32_t>("eliteSize");            ///< Size of elite part of population (unchanged)
    static const int maxGenerations         = parser.getAs<int32_t>("maxGenerations");       ///< Maximal number of generations
    static const double probCrossover       = parser.getAs<double>("probCrossover");         ///< Probability of crossover
    static const double probMutationInd     = parser.getAs<double>("probMutationInd");       ///< Probability of individual mutation
    static const double probMutationGene    = parser.getAs<double>("probMutationGene");      ///< Probability of gene mutation
}

namespace whm
{
    WarehouseOptimizer_t::WarehouseOptimizer_t(utils::WhmArgs_t args_)
        : args{ args_ }
    {
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
                mutateInvert(ind.genes);
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

    void WarehouseOptimizer_t::mutateInvert(std::vector<int32_t>& ind)
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
                    //crossoverOnePoint(mum.genes, dad.genes);
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

            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "Generation %2d", gen);
            for (int p=0; p<constants::populationSize; p++) fprintf(stderr, "%.2f ", population[p].fitness);
            std::cout << std::endl;
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
