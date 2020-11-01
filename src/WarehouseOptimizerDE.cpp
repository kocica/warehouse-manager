/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerDE.cpp
 * @date    10/30/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using differential evolution
 */

#ifdef WHM_OPT

#include <float.h>

#include "WarehouseOptimizerDE.h"

namespace whm
{
    WarehouseOptimizerDE_t::WarehouseOptimizerDE_t(utils::WhmArgs_t args_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        bestInd.fitness = DBL_MAX;
    }

    std::vector<int32_t> WarehouseOptimizerDE_t::rand(const std::vector<Solution_t>& pop, int32_t j)
    {
        int32_t rand{ 0 };

        do
        {
            rand = randomFromInterval(0, cfg.getAs<int32_t>("populationSizeDE"));
        }
        while(rand == j);

        return pop[rand].genes;
    }

    std::vector<int32_t> WarehouseOptimizerDE_t::best(const std::vector<Solution_t>& pop, int32_t j)
    {
        return bestInd.genes.empty() ? rand(pop, j) : bestInd.genes;
    }

    ProbGenes_t WarehouseOptimizerDE_t::mutate(const std::vector<Solution_t>& pop, int32_t j)
    {
        int32_t r{ 0 };
        std::vector<int32_t> selectedInds{ j };
        std::vector<int32_t> targetVector{ best(pop, j) };

        for(int32_t i = 0; i < 2; ++i)
        {
            do
            {
                r = randomFromInterval(0, cfg.getAs<int32_t>("populationSizeDE"));
            }
            while(std::find(selectedInds.begin(), selectedInds.end(), r) != selectedInds.end());

            selectedInds.push_back(r);
        }

        // Mutation computed using formula: v = omega * x_a + scale * (x_b - x_c)
        // Where: j =/= a =/= b =/= c
        //        x_a = target vector
        //        x_b = randomly selected vector 1
        //        x_c = randomly selected vector 2
        //        omega = rand  E [0,1]
        //        scale = const E [0,2]

        return operatorAdd(operatorMul(genesToProbGenes(targetVector), randomFromInterval(0, 1)),
                           operatorMul(operatorSub(genesToProbGenes(pop[selectedInds[1]].genes),
                                                   genesToProbGenes(pop[selectedInds[2]].genes)),
                                                   cfg.getAs<double>("scalingFactor")));
    }

    ProbGenes_t WarehouseOptimizerDE_t::crossover(const ProbGenes_t& v, const ProbGenes_t& x, int32_t j)
    {
        int32_t k = randomFromInterval(0, cfg.getAs<int32_t>("populationSizeDE"));

        if(flipCoin(cfg.getAs<double>("probCrossoverDE")) || (j == k))
        {
            return v;
        }
        else
        {
            return x;
        }
    }

    std::vector<int32_t> WarehouseOptimizerDE_t::crossoverOrdered(const std::vector<int32_t>& lhsInd, const std::vector<int32_t>& rhsInd)
    {
        if(!flipCoin(cfg.getAs<double>("probCrossoverDE")))
        {
            return lhsInd;
        }

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
        return o1;
    }

    ProbGenes_t WarehouseOptimizerDE_t::getRemainingSet(const ProbGenes_t& pgs, double alpha)
    {
        ProbGenes_t remainSet;

        for(const ProbGene_t& pg : pgs)
        {
            if(pg.second > alpha)
            {
                remainSet.push_back(pg);
            }
        }

        return remainSet;
    }

    ProbGenes_t WarehouseOptimizerDE_t::operatorMul(const ProbGenes_t& pgs, double factor)
    {
        ProbGenes_t pgsCopy = pgs;

        std::for_each(pgsCopy.begin(), pgsCopy.end(), [factor](ProbGene_t& pg){ pg.second *= factor; });

        return pgsCopy;
    }

    ProbGenes_t WarehouseOptimizerDE_t::operatorAdd(const ProbGenes_t& a, const ProbGenes_t& b)
    {
        ProbGenes_t probGenes;

        for(const ProbGene_t& pga : a)
        {
            bool found{ false };

            for(const ProbGene_t& pgb : b)
            {
                if(pga.first == pgb.first)
                {
                    probGenes.push_back(ProbGene_t(pga.first, std::max(pga.second, pgb.second)));

                    found = true; break;
                }
            }

            if(!found)
            {
                probGenes.push_back(pga);
            }
        }

        for(const ProbGene_t& pgb : b)
        {
            bool found{ false };

            for(const ProbGene_t& pgs : probGenes)
            {
                if(pgs.first == pgb.first)
                {
                    found = true; break;
                }
            }

            if(!found)
            {
                probGenes.push_back(pgb);
            }
        }

        return probGenes;
    }

    ProbGenes_t WarehouseOptimizerDE_t::operatorSub(const ProbGenes_t& a, const ProbGenes_t& b)
    {
        ProbGenes_t probGenes;

        for(const ProbGene_t& pga : a)
        {
            bool found{ false };

            for(const ProbGene_t& pgb : b)
            {
                if(pga.first == pgb.first)
                {
                    found = true; break;
                }
            }

            if(!found)
            {
                probGenes.push_back(pga);
            }
        }

        return probGenes;
    }

    ProbGenes_t WarehouseOptimizerDE_t::genesToProbGenes(const std::vector<int32_t>& genes)
    {
        ProbGenes_t probGenes;

        std::for_each(genes.begin(), genes.end(), [&pg = probGenes](int32_t gene){ pg.emplace_back(ProbGene_t(gene, 1.0)); });

        return probGenes;
    }

    std::vector<int32_t> WarehouseOptimizerDE_t::probGenesToGenes(const ProbGenes_t& probGenes)
    {
        std::vector<int32_t> genes;

        std::for_each(probGenes.begin(), probGenes.end(), [&g = genes](const ProbGene_t& probGene){ g.push_back(probGene.first); });

        return genes;
    }

    void WarehouseOptimizerDE_t::optimize()
    {
        std::vector<Solution_t> population(cfg.getAs<int32_t>("populationSizeDE"));

        if(cfg.isSet("initialWeights"))
        {
            initPopulationWeights(population);
        }
        else
        {
            initPopulationRand(population);
        }

        for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
        {
            population[p].fitness = simulateWarehouse(population[p].genes);
        }

        for(int32_t gen = 0; gen < cfg.getAs<int32_t>("maxIterations"); ++gen)
        {
            std::vector<ProbGenes_t> trailVector;

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
            {
                ProbGenes_t mutant = mutate(population, p);
                ProbGenes_t remainers = getRemainingSet(mutant, randomFromInterval(0, 1));

                while(static_cast<int32_t>(remainers.size()) < cfg.getAs<int32_t>("numberDimensions"))
                {
                    int32_t rand{ 0 };

                    do
                    {
                        rand = randomFromInterval(cfg.getAs<int32_t>("problemMin"), cfg.getAs<int32_t>("problemMax"));
                    }
                    while(std::find_if(remainers.begin(), remainers.end(), [rand](auto v){ return v.first == rand; }) != remainers.end());

                    remainers.push_back(std::make_pair(rand, 1.0));
                }

                trailVector.push_back(ProbGenes_t(remainers.begin(), remainers.begin() + cfg.getAs<int32_t>("numberDimensions")));
            }

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
            {
                auto x_new  = crossoverOrdered(probGenesToGenes(trailVector[p]), population[p].genes);
                auto fx_new = simulateWarehouse(x_new);

                if(fx_new <= population[p].fitness)
                {
                    population[p].genes   = x_new;
                    population[p].fitness = fx_new;
                }
            }

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
            {
                if(population[p].fitness < bestInd.fitness)
                {
                    bestInd = population[p];
                }
            }

            std::cout << "Iteration " << gen << ": " << bestInd.fitness << std::endl;

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
