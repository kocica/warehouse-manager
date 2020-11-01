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

    ProbGenes_t WarehouseOptimizerDE_t::mutate(const std::vector<Solution_t>& pop, int32_t xi)
    {
        int32_t r{ 0 };
        std::vector<int32_t> selectedInds{ xi };

        for(int32_t i = 0; i < 3; ++i)
        {
            do
            {
                r = randomFromInterval(0, cfg.getAs<int32_t>("populationSizeDE"));
            }
            while(std::find(selectedInds.begin(), selectedInds.end(), r) != selectedInds.end());

            selectedInds.push_back(r);
        }

        // Mutation = omega * x_a + scaling_factor * (x_b - x_c)
        // Where: xi /= a /= b /= c
        //        x_a = target vector
        //        x_b = randomly selected vector 1
        //        x_c = randomly selected vector 2
        //        omega = rand E [0,1]
        //        scaling_factor = constant E [0,2]

        return operatorAdd(operatorMul(genesToProbGenes(pop[selectedInds[1]].genes),
                                       randomFromInterval(0, 1)),
                           operatorMul(operatorSub(genesToProbGenes(pop[selectedInds[2]].genes),
                                                   genesToProbGenes(pop[selectedInds[3]].genes)),
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
            ProbGenes_t remainerSet;
            std::vector<ProbGenes_t> trailVector;

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
            {
                double alpha = randomFromInterval(0, 1);

                ProbGenes_t mutant = mutate(population, p);

                ProbGenes_t remainers = getRemainingSet(mutant, alpha);
                ProbGenes_t trail;

                for(int32_t i = 0; i < std::max((int32_t)remainers.size(), cfg.getAs<int32_t>("numberDimensions")); ++i)
                {
                    if(i >= (int32_t)remainers.size())
                    {
                        if(!remainerSet.empty())
                        {
                            // Take from remainers
                            trail.push_back(remainerSet.front());
                            remainerSet.erase(remainerSet.begin());
                        }
                        else
                        {
                            // Generate brand new
                            for(int32_t v = cfg.getAs<int32_t>("problemMin"); v < cfg.getAs<int32_t>("problemMax"); ++v)
                            {
                                bool found{ false };

                                for(auto t : trail)
                                {
                                    if(t.first == v)
                                    {
                                        found = true; break;
                                    }
                                }

                                if(!found) trail.push_back(std::make_pair(v, 1.0));

                                if((int32_t)trail.size() == cfg.getAs<int32_t>("numberDimensions")) break;
                            }
                        }
                    }
                    else if(i >= cfg.getAs<int32_t>("numberDimensions"))
                    {
                        remainerSet.push_back(remainers.at(i));
                    }
                    else
                    {
                        trail.push_back(remainers.at(i));
                    }
                }

                trailVector.push_back(trail);
            }

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
            {
                ProbGenes_t x_new = crossover(trailVector[p], genesToProbGenes(population[p].genes), p);

                auto genes = probGenesToGenes(x_new);
                double fx_new = simulateWarehouse(genes);

                if(fx_new <= population[p].fitness)
                {
                    population[p].fitness = fx_new;
                    population[p].genes = probGenesToGenes(x_new);
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
        }
    }
}

#endif
