/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerDE.cpp
 * @date    10/30/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using differential evolution
 */

#ifdef WHM_OPT

#include <limits>
#include <cstdint>
#include <iostream>
#include <algorithm>

#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseOptimizerDE.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    WarehouseOptimizerDE_t::WarehouseOptimizerDE_t(const utils::WhmArgs_t& args_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        init();
    }

    WarehouseOptimizerDE_t::WarehouseOptimizerDE_t(const utils::WhmArgs_t& args_, const ConfigParser_t& cfg_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        cfg = cfg_;
        init();
    }

    void WarehouseOptimizerDE_t::init()
    {
        bestInd.fitness = std::numeric_limits<double>::max();

        std::map<std::string, CrossoverFunctor_t> crossoverMap =
        {
            { "crossoverOrdered",   std::bind(&WarehouseOptimizerDE_t::crossoverOrdered,   this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
            { "crossoverBinomical", std::bind(&WarehouseOptimizerDE_t::crossoverBinomical, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) }
        };

        crossoverFunctor = crossoverMap[cfg.getAs<std::string>("crossoverFunctorDE")];
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
        std::vector<int32_t> targetVector{ rand(pop, j) };

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
        // Where: j /= a /= b /= c
        //        x_a   = target vector
        //        x_b   = randomly selected vector 1
        //        x_c   = randomly selected vector 2
        //        omega = rand  E [0,1]
        //        scale = const E [0,2]

        return operatorAdd(operatorMul(genesToProbGenes(targetVector), randomFromInterval(0, 1)),
                           operatorMul(operatorSub(genesToProbGenes(pop[selectedInds[1]].genes),
                                                   genesToProbGenes(pop[selectedInds[2]].genes)),
                                                   cfg.getAs<double>("scalingFactor")));
    }

    std::vector<int32_t> WarehouseOptimizerDE_t::crossoverBinomical(const std::vector<int32_t>& v, const std::vector<int32_t>& x, int32_t j)
    {
        std::vector<int32_t> x_new;

        for(int32_t k = 0; k < cfg.getAs<int32_t>("numberDimensions"); ++k)
        {
            if(flipCoin(cfg.getAs<double>("probCrossoverDE")) || (j == k))
            {
                if(std::find(x_new.begin(), x_new.end(), v.at(k)) == x_new.end())
                {
                    x_new.push_back(v.at(k));
                }
                else
                {
                    x_new.push_back(lookupOptimalSlot(x_new));
                }
            }
            else
            {
                if(std::find(x_new.begin(), x_new.end(), x.at(k)) == x_new.end())
                {
                    x_new.push_back(x.at(k));
                }
                else
                {
                    x_new.push_back(lookupOptimalSlot(x_new));
                }
            }
        }

        return x_new;
    }

    std::vector<int32_t> WarehouseOptimizerDE_t::crossoverOrdered(const std::vector<int32_t>& lhsInd, const std::vector<int32_t>& rhsInd, int32_t j)
    {
        (void)j;

        if(!flipCoin(cfg.getAs<double>("probCrossoverDE")))
        {
            return lhsInd;
        }

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

    int32_t WarehouseOptimizerDE_t::lookupOptimalSlot(const std::vector<int32_t>& genes)
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

        auto n = cfg.getAs<int32_t>("procCount");

        for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
        {
            for(size_t i = 0; i < population.at(p).genes.size(); ++i)
            {
                auto s = write(simProcesses.at(p % n).outfd, &population[p].genes.at(i), sizeof(int32_t));

                if(s < 0)
                {
                    whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Write failed");
                    throw std::runtime_error("Write failed");
                }
            }
        }

        for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
        {
            auto s = read(simProcesses.at(p % n).infd, &population.at(p).fitness, sizeof(double));

            if(s <= 0)
            {
                whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Read failed");
                throw std::runtime_error("Read failed");
            }
        }

        for(int32_t gen = 0; gen < cfg.getAs<int32_t>("maxIterations"); ++gen)
        {
            std::vector<ProbGenes_t> trailVector;

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
            {
                ProbGenes_t mutant = mutate(population, p);
                ProbGenes_t remainers = getRemainingSet(mutant, randomFromInterval(0.2, 1));

                while(static_cast<int32_t>(remainers.size()) < cfg.getAs<int32_t>("numberDimensions"))
                {
                    int32_t slot = lookupOptimalSlot(probGenesToGenes(remainers));

                    remainers.push_back(std::make_pair(slot, 1.0));
                }

                trailVector.push_back(ProbGenes_t(remainers.begin(), remainers.begin() + cfg.getAs<int32_t>("numberDimensions")));
            }

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
            {
                auto x_new = crossoverFunctor(probGenesToGenes(trailVector[p]), population[p].genes, p);

                for(size_t i = 0; i < x_new.size(); ++i)
                {
                    auto s = write(simProcesses.at(p % n).outfd, &x_new.at(i), sizeof(int32_t));

                    if(s < 0)
                    {
                        whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Write failed");
                        throw std::runtime_error("Write failed");
                    }
                }
            }

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
            {
                double newFitness;
                auto s = read(simProcesses.at(p % n).infd, &newFitness, sizeof(double));

                if(s <= 0)
                {
                    whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Read failed");
                    throw std::runtime_error("Read failed");
                }

                if(newFitness <= population[p].fitness)
                {
                    population[p].genes   = crossoverFunctor(probGenesToGenes(trailVector[p]), population[p].genes, p);
                    population[p].fitness = newFitness;
                }
                else
                {
                    population[p].trialValue++;

                    if(population[p].trialValue > cfg.getAs<int32_t>("maxTrialValue"))
                    {
                        population[p].trialValue = 0;
                        population[p].genes = std::vector<int32_t>();

                        initIndividualRand(population[p].genes);

                        population[p].fitness = simulateWarehouse(population[p].genes);
                    }
                }
            }

            for(int32_t p = 0; p < cfg.getAs<int32_t>("populationSizeDE"); ++p)
            {
                if(population[p].fitness < bestInd.fitness)
                {
                    bestInd = population[p];
                }
            }

            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "[DE] [%3d] Best fitness: %f", gen, bestInd.fitness);
            histFitness.push_back(bestInd.fitness);

#           ifdef WHM_GUI
            if(uiCallback)
            {
                updateAllocations(bestInd.genes);
                uiCallback(bestInd.fitness);
            }
#           endif

            if((gen % cfg.getAs<int32_t>("saveWeightsPeriod")) == 0)
            {
                saveFitnessPlot();
                saveBestSolution(bestInd.genes);
            }
        }

        saveFitnessPlot();
        saveBestSolution(bestInd.genes);
    }
}

#endif
