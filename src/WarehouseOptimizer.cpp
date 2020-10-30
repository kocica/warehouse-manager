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
#include <map>
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


namespace
{
    whm::ConfigParser_t cfg("cfg/optimizer.xml");

    // Dynamically modifiable optimization parameters (getters for better readablity)

    auto numberDimensions       = [](){ return cfg.getAs<int32_t>("numberDimensions"); };    ///< Number of dimensions (i.e. number of SKUs)
    auto problemMin             = [](){ return cfg.getAs<int32_t>("problemMin");       };    ///< Slot min number
    auto problemMax             = [](){ return cfg.getAs<int32_t>("problemMax");       };    ///< Slot max number
    auto populationSize         = [](){ return cfg.getAs<int32_t>("populationSize");   };    ///< Population size
    auto selectionSize          = [](){ return cfg.getAs<int32_t>("selectionSize");    };    ///< Size of selected part of population
    auto eliteSize              = [](){ return cfg.getAs<int32_t>("eliteSize");        };    ///< Size of elite part of population (unchanged)
    auto maxGenerations         = [](){ return cfg.getAs<int32_t>("maxGenerations");   };    ///< Maximal number of generations
    auto saveWeightsAfter       = [](){ return cfg.getAs<int32_t>("saveWeightsAfter"); };    ///< Save weights after N generations
    auto maxTrialValue          = [](){ return cfg.getAs<int32_t>("maxTrialValue");    };    ///< New rand offspring after N iterations (same fitness)
    auto probCrossover          = [](){ return cfg.getAs<double>("probCrossover");     };    ///< Probability of crossover
    auto probMutationInd        = [](){ return cfg.getAs<double>("probMutationInd");   };    ///< Probability of individual mutation
    auto probMutationGene       = [](){ return cfg.getAs<double>("probMutationGene");  };    ///< Probability of gene mutation
}


namespace whm
{
    WarehouseOptimizer_t::WarehouseOptimizer_t(utils::WhmArgs_t args_)
        : args{ args_ }
    {
        std::map<std::string, MutationFunctor_t> mutatorMap =
        {
            { "mutateOrdered",     std::bind(&WarehouseOptimizer_t::mutateOrdered,    this, std::placeholders::_1) },
            { "mutateInverse",     std::bind(&WarehouseOptimizer_t::mutateInverse,    this, std::placeholders::_1) }
        };

        std::map<std::string, SelectionFunctor_t> selectorMap =
        {
            { "selectRank",        std::bind(&WarehouseOptimizer_t::selectRank,       this, std::placeholders::_1) },
            { "selectTrunc",       std::bind(&WarehouseOptimizer_t::selectTrunc,      this, std::placeholders::_1) },
            { "selectTournam",     std::bind(&WarehouseOptimizer_t::selectTournam,    this, std::placeholders::_1) },
            { "selectRoulette",    std::bind(&WarehouseOptimizer_t::selectRoulette,   this, std::placeholders::_1) }
        };

        std::map<std::string, CrossoverFunctor_t> crossoverMap =
        {
            { "crossoverOrdered",  std::bind(&WarehouseOptimizer_t::crossoverOrdered, this, std::placeholders::_1, std::placeholders::_2) }
        };

        mutationFunctor  = mutatorMap[cfg.getAs<std::string>("mutationFunctor")];
        selectionFunctor = selectorMap[cfg.getAs<std::string>("selectionFunctor")];
        crossoverFunctor = crossoverMap[cfg.getAs<std::string>("crossoverFunctor")];

        whm::WarehouseSimulator_t::getWhSimulator().printStats(false);

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
        for(int32_t d = 0; d < numberDimensions(); ++d)
        {
            int32_t r{ 0 };

            do
            {
                r = randomFromInterval(problemMin(), problemMax());
            }
            while(std::find(ind.begin(), ind.end(), r) != ind.end());

            ind.push_back(r);
        }
    }

    void WarehouseOptimizer_t::initPopulationRand(std::vector<Solution_t>& pop)
    {
        for(int32_t p = 0; p < static_cast<int32_t>(pop.size()); ++p)
        {
            initIndividualRand(pop[p].genes);
        }
    }

    void WarehouseOptimizer_t::initPopulationWeights(std::vector<Solution_t>& pop)
    {
        std::vector<std::string> articles;
        whm::WarehouseLayout_t::getWhLayout().importArticles(args.articlesPath, articles);
        whm::WarehouseLayout_t::getWhLayout().importLocationSlots(cfg.getAs<std::string>("initialWeights"));

        for(auto& article : articles)
        {
            for(auto& slot : slotEnc)
            {
                if(article == slot.second->getArticle())
                {
                    pop[0].genes.push_back(slot.first);
                }
            }
        }

        for(int32_t p = 1; p < static_cast<int32_t>(pop.size()); ++p)
        {
            initIndividualRand(pop[p].genes);
        }
    }

    // Don't select the best fitness to keep the diversity
    Solution_t WarehouseOptimizer_t::selectTrunc(const std::vector<Solution_t>& pop)
    {
        return pop.at(randomFromInterval(0, selectionSize()));
    }

    Solution_t WarehouseOptimizer_t::selectTournam(const std::vector<Solution_t>& pop)
    {
        int32_t r{ 0 };
        std::vector<int32_t> generated;
        std::vector<Solution_t> selectedInds;

        for(int32_t i = 0; i < 5; ++i)
        {
            do
            {
                r = randomFromInterval(problemMin(), problemMax());
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

    Solution_t WarehouseOptimizer_t::selectRoulette(const std::vector<Solution_t>& pop)
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

    Solution_t WarehouseOptimizer_t::selectRank(const std::vector<Solution_t>& pop)
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
        for(int32_t i = 0; i < populationSize(); ++i)
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

    void WarehouseOptimizer_t::crossoverAverage(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        for(int32_t i = 0; i < numberDimensions(); ++i)
        {
            auto tmp = lhsInd[i] + rhsInd[i];
            lhsInd[i] = 0.4 * tmp;
            rhsInd[i] = 0.6 * tmp;
        }
    }

    void WarehouseOptimizer_t::crossoverUniform(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        for(int32_t i = 0; i < numberDimensions(); i++)
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
    void WarehouseOptimizer_t::crossoverOrdered(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
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
                a = randomFromInterval(pos, numberDimensions());
                b = randomFromInterval(pos, numberDimensions());
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

            if(b >= numberDimensions() - 1)
            {
                for(int32_t i = b; i < numberDimensions(); ++i)
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
        for(int32_t i = 0; i < problemMax(); ++i)
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

    void WarehouseOptimizer_t::crossoverOnePoint(std::vector<int32_t>& lhsInd, std::vector<int32_t>& rhsInd)
    {
        int32_t point = randomFromInterval(0, numberDimensions());

        for(int32_t i = point; i < numberDimensions(); ++i)
        {
            auto tmp  = lhsInd[i];
            lhsInd[i] = rhsInd[i];
            rhsInd[i] = tmp;
        }
    }

    void WarehouseOptimizer_t::mutate(Solution_t& ind)
    {
        if(flipCoin(probMutationInd()))
        {
            for(int32_t i = 0; i <= numberDimensions() * probMutationGene(); i++)
            {
                mutationFunctor(ind.genes);
            }
        }
    }

    void WarehouseOptimizer_t::mutateRand(std::vector<int32_t>& ind)
    {
        int32_t pos = randomFromInterval(0, numberDimensions());
        int32_t val = randomFromInterval(problemMin(), problemMax());

        ind[pos] = val;
    }

    void WarehouseOptimizer_t::mutateGauss(std::vector<int32_t>& ind)
    {
        double sigma = (problemMax() - problemMin()) * 0.1;

        int32_t pos = randomFromInterval(0, numberDimensions());
        int32_t val = randomGauss(ind[pos], sigma);

        if(val < problemMin()) val = problemMin();
        if(val > problemMax()) val = problemMax();

        ind[pos] = val;
    }

    void WarehouseOptimizer_t::mutateOrdered(std::vector<int32_t>& ind)
    {
        int32_t a, b;

        do
        {
            a = randomFromInterval(0, numberDimensions());
            b = randomFromInterval(0, numberDimensions());
        }
        while(a == b);

        if(a > b) std::swap(a, b);

        std::rotate(ind.begin() + a, ind.begin() + b, ind.begin() + b + 1);
    }

    void WarehouseOptimizer_t::mutateInverse(std::vector<int32_t>& ind)
    {
        int32_t a, b;

        do
        {
            a = randomFromInterval(0, numberDimensions());
            b = randomFromInterval(0, numberDimensions());
        }
        while(a == b);

        if(a > b) std::swap(a, b);

        std::reverse(ind.begin() + a, ind.begin() + b);
    }

    void WarehouseOptimizer_t::evolve()
    {
        std::vector<Solution_t> population(populationSize());

        if(cfg.isSet("initialWeights"))
        {
            initPopulationWeights(population);
        }
        else
        {
            initPopulationRand(population);
        }

        for(int32_t p = 0; p < populationSize(); ++p)
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

        for(int32_t gen = 0; gen < maxGenerations(); ++gen)
        {
            std::vector<Solution_t> newPopulation;

            for(int32_t p = 0; p < eliteSize(); ++p)
            {
                newPopulation.push_back(population[p]);
            }

            for(int32_t i = 0; i < (populationSize() - eliteSize()) / 2; i++)
            {
                Solution_t mum = selectionFunctor(population);
                Solution_t dad = selectionFunctor(population);

                if(flipCoin(probCrossover()))
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

            if((populationSize() - eliteSize()) & 1)
            {
                Solution_t mutant = selectionFunctor(population);

                mutate(mutant);

                newPopulation.push_back(mutant);
            }

            population = std::move(newPopulation);

            for(int32_t p = 0; p < populationSize(); ++p)
            {
                double newFitness = simulateWarehouse(population[p].genes);

                population[p].trialValue = newFitness <= population[p].fitness ? population[p].trialValue + 1 : 0;

                population[p].fitness = newFitness;
            }

            std::sort(population.begin(), population.end(),
                    [](Solution_t& lhs, Solution_t& rhs)
                    -> bool
                    {
                        return lhs.fitness < rhs.fitness;
                    });

            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "==== Generation %2d ====", gen);
            std::for_each(population.begin(), population.end(), [](Solution_t& s){ std::cout << s.fitness << " "; });
            std::cout << std::endl;

            // Each N iterations save weights
            if((gen % saveWeightsAfter()) == 0)
            {
                saveBestChromosome(population.at(0).genes);
            }

            // Discard chromosomes with trialValue > N and generate brand new offsprings (ignore elite part)
            for(int32_t p = eliteSize(); p < populationSize(); ++p)
            {
                if(population[p].trialValue > maxTrialValue())
                {
                    population[p].fitness = 0;
                    population[p].trialValue = 0;
                    initIndividualRand(population[p].genes);
                }
            }
        }

        // Save the evolution best Solution_t
        saveBestChromosome(population.at(0).genes);
    }

    void WarehouseOptimizer_t::updateAllocations(std::vector<int32_t>& ind)
    {
        // Clear current allocations first
        for(auto& slot : slotEnc)
        {
            slot.second->setArticle(std::string());
        }

        // Set new allocations
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
