/**
 * Warehouse manager
 *
 * @file    WarehouseOptimizerABC.cpp
 * @date    10/30/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse using artificial bee colony
 */

#ifdef WHM_OPT

#include "Utils.h"
#include "Logger.h"
#include "WarehouseOptimizerABC.h"

namespace whm
{
    WarehouseOptimizerABC_t::WarehouseOptimizerABC_t(const utils::WhmArgs_t& args_)
        : WarehouseOptimizerBase_t{ args_ }
    {
        init();
    }

    WarehouseOptimizerABC_t::WarehouseOptimizerABC_t(const utils::WhmArgs_t& args_, const ConfigParser_t& cfg_)
        : WarehouseOptimizerBase_t{ args_, cfg_ }
    {
        init();
    }

    void WarehouseOptimizerABC_t::init()
    {
        // Init rule counters of each equation to one, as per paper
        for(int32_t i = 0; i < 8; ++i)
        {
            ruleCounters.push_back(1);
        }
    }

    void WarehouseOptimizerABC_t::employedBeePhase(std::vector<Solution_t>& pop)
    {
        auto n = cfg.getAs<int32_t>("procCount");

        std::vector<std::vector<int32_t>> genes;
        std::unordered_map<std::string, double> chromosomeMap;

        for(int p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
        {
            auto g = updateBee(pop, p);

            pop[p].stringGenes = utils::toString(g);

            if(chromosomeMap.find(pop[p].stringGenes) == chromosomeMap.end())
            {
                chromosomeMap.insert(std::make_pair(pop[p].stringGenes, -1));

                for(size_t i = 0; i < g.size(); ++i)
                {
                    auto s = write(simProcesses.at(p % n).outfd, &g.at(i), sizeof(int32_t));

                    if(s < (ssize_t)sizeof(int32_t))
                    {
                        whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Write failed <%d>", errno);
                        throw std::runtime_error("Write failed");
                    }
                }
            }

            genes.push_back(std::move(g));
        }

        for(int p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
        {
            double newFitness = chromosomeMap[pop[p].stringGenes];

            if(newFitness < 0)
            {
                auto s = read(simProcesses.at(p % n).infd, &newFitness, sizeof(double));

                if(s < (ssize_t)sizeof(double))
                {
                    whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Read failed <%d>", errno);
                    throw std::runtime_error("Read failed");
                }

                chromosomeMap[pop[p].stringGenes] = newFitness;
            }

            // In case its better solution, update food
            if(newFitness < pop[p].fitness)
            {
                pop[p].trialValue = 0;
                pop[p].genes = genes.at(p);
                pop[p].fitness = newFitness;
            }
            else
            {
                pop[p].trialValue++;
            }
        }
    }

    void WarehouseOptimizerABC_t::onlookerBeePhase(std::vector<Solution_t>& pop)
    {
        double sumFitness{ 0.0 };
        int32_t updateCounter{ 0 };

        for(const Solution_t& sol : pop)
        {
            sumFitness += sol.fitness;
        }

        do
        {
            for(int p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
            {
                double prob = pop[p].fitness / sumFitness;

                if(flipCoin(prob))
                {
                    updateCounter++;
                    updateBee(pop, p);

                    if(updateCounter == cfg.getAs<int32_t>("foodSize"))
                    {
                        break;
                    }
                }
            }
        }
        while(updateCounter < cfg.getAs<int32_t>("foodSize"));
    }

    std::vector<int32_t> WarehouseOptimizerABC_t::updateBee(std::vector<Solution_t>& pop, int32_t p)
    {
        // Select random partner (/= actual one)
        int32_t partner_j{ 0 };
        int32_t partner_k{ 0 };

        do
        {
            partner_j = randomFromInterval(0, cfg.getAs<int32_t>("foodSize"));
            partner_k = randomFromInterval(0, cfg.getAs<int32_t>("foodSize"));
        }
        while(p == partner_j || p == partner_k || partner_j == partner_k);

        // Create new food location
        // https://www.sciencedirect.com/science/article/pii/S2210650216304588

        auto x_i = pop[p].genes;
        auto x_j = pop[partner_j].genes;
        auto x_k = pop[partner_k].genes;
        std::vector<int32_t> y_i;
        std::vector<SwapOperator_t> sos;

        switch(static_cast<int32_t>(randomFromInterval(0, 8)))
        {
            case 0:
                sos = getSwap(x_i, x_k);
                y_i = applySwap(x_j, sos);
                break;
            case 1:
                sos = getSwap(x_j, x_k);
                y_i = applySwap(x_i, sos);
                break;
            case 2:
                sos = getSwap(x_i, x_k);
                y_i = applySwap(bestSolution.genes, sos);
                break;
            case 3:
                sos = getSwap(x_i, bestSolution.genes);
                y_i = applySwap(x_i, sos);
                break;
            case 4:
                sos = getSwap(bestSolution.genes, x_k);
                y_i = applySwap(bestSolution.genes, sos);
                break;
            case 5:
                sos = getSwap(bestSolution.genes, worstSolution.genes);
                y_i = applySwap(x_i, sos);
                break;
            case 6:
            {
                sos = getSwap(x_k, x_i);
                auto sos2 = getSwap(bestSolution.genes, x_k);
                std::move(sos2.begin(), sos2.end(), std::back_inserter(sos));
                y_i = applySwap(x_i, sos);
                break;
            }
            case 7:
                sos = getSwap(bestSolution.genes, x_i);
                y_i = applySwap(x_j, sos);
                break;
        }

        return y_i;
    }

    std::vector<int32_t> WarehouseOptimizerABC_t::applySwap(std::vector<int32_t>& sol, std::vector<SwapOperator_t>& sos)
    {
        std::vector<int32_t> y_i = sol;

        // for(auto it = sos.rbegin(); it != sos.rend(); ++it)
        for(auto it = sos.begin(); it != sos.end(); ++it)
        {
            iter_swap(y_i.begin() + it->first, y_i.begin() + it->second);
        }

        return y_i;
    }

    std::vector<SwapOperator_t> WarehouseOptimizerABC_t::filterSwap(const std::vector<SwapOperator_t>& sos)
    {
        std::vector<SwapOperator_t> ret;

        double r = randomFromInterval(0, 1);

        for(const SwapOperator_t& so : sos)
        {
            if(flipCoin(r))
            {
                ret.push_back(so);
            }
        }

        return ret;
    }

    std::vector<SwapOperator_t> WarehouseOptimizerABC_t::getSwap(std::vector<int32_t>& lhs, std::vector<int32_t>& rhs)
    {
        std::vector<int32_t> lhsCopy = lhs;
        std::vector<int32_t> rhsCopy = rhs;
        std::vector<SwapOperator_t> swapOperators;

        using namespace utils;

        while(true)
        {
            std::vector<int32_t> lastLhs = lhsCopy;
            std::vector<int32_t> lastRhs = rhsCopy;

            for(int32_t i = cfg.getAs<int32_t>("problemMin"); i < cfg.getAs<int32_t>("problemMax"); ++i)
            {
                if(lhsCopy == rhsCopy)
                {
                    return swapOperators;
                }

                auto lhsIt = std::find(lhsCopy.begin(), lhsCopy.end(), i);
                auto rhsIt = std::find(rhsCopy.begin(), rhsCopy.end(), i);

                if(lhsIt != lhsCopy.end() && rhsIt != rhsCopy.end())
                {
                    auto lhsPos = lhsIt - lhsCopy.begin();
                    auto rhsPos = rhsIt - rhsCopy.begin();

                    if(lhsPos == rhsPos)
                    {
                        continue;
                    }

                    iter_swap(rhsCopy.begin() + lhsPos, rhsCopy.begin() + rhsPos);

                    swapOperators.emplace_back(std::make_pair(lhsPos, rhsPos));
                }
            }

            if(lastLhs == lhsCopy && lastRhs == rhsCopy)
            {
                return swapOperators;
            }
        }

        return swapOperators;
    }

    bool WarehouseOptimizerABC_t::isBestSolution(const Solution_t& solution)
    {
        return cfg.getAs<bool>("keepBest") && solution == bestSolution;
    }

    void WarehouseOptimizerABC_t::scoutBeePhase(std::vector<Solution_t>& pop)
    {
        std::vector<int32_t> scoutIndexes;

        auto n = cfg.getAs<int32_t>("procCount");

        for(int32_t p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
        {
            if(!isBestSolution(pop[p]) && pop[p].trialValue > cfg.getAs<int32_t>("maxTrialValue"))
            {
                pop[p].trialValue = 0;
                pop[p].genes = std::vector<int32_t>();

                initIndividualRand(pop[p].genes);

                for(size_t i = 0; i < pop.at(p).genes.size(); ++i)
                {
                    auto s = write(simProcesses.at(p % n).outfd, &pop[p].genes.at(i), sizeof(int32_t));

                    if(s < (ssize_t)sizeof(int32_t))
                    {
                        whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Write failed <%d>", errno);
                        throw std::runtime_error("Write failed");
                    }
                }

                scoutIndexes.push_back(p);
            }
        }

        for(int32_t p : scoutIndexes)
        {
            auto s = read(simProcesses.at(p % n).infd, &pop[p].fitness, sizeof(double));

            if(s < (ssize_t)sizeof(double))
            {
                whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Read failed <%d>", errno);
                throw std::runtime_error("Read failed");
            }
        }
    }

    void WarehouseOptimizerABC_t::memorizeBestSolution(std::vector<Solution_t>& pop)
    {
        bestSolution = pop[0];
        worstSolution = pop[0];

        for(int32_t p = 1; p < cfg.getAs<int32_t>("foodSize"); ++p)
        {
            if(pop[p].fitness < bestSolution.fitness)
            {
                bestSolution = pop[p];
            }
            else if(pop[p].fitness > worstSolution.fitness)
            {
                worstSolution = pop[p];
            }
        }
    }

    int32_t WarehouseOptimizerABC_t::selectTechnique()
    {
        int32_t sumCounters{ 0 };

        std::for_each(ruleCounters.begin(), ruleCounters.end(), [&sumCounters](int32_t c)
                                                                -> void
                                                                {
                                                                    sumCounters += c;
                                                                });

        while(true)
        {
            for(int32_t i = 0; i < 8; ++i)
            {
                double rho_i = ruleCounters[i] / static_cast<double>(sumCounters);

                if(flipCoin(rho_i))
                {
                    ruleCounters[i] += 1;
                    return i;
                }
            }
        }

        return -1;
    }

    void WarehouseOptimizerABC_t::optimize()
    {
        std::vector<Solution_t> population(cfg.getAs<int32_t>("foodSize"));

        if(cfg.isSet("initialWeights"))
        {
            initPopulationWeights(population);
        }
        else
        {
            initPopulationRand(population);
        }

        auto n = cfg.getAs<int32_t>("procCount");

        for(int32_t p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
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

        for(int32_t p = 0; p < cfg.getAs<int32_t>("foodSize"); ++p)
        {
            auto s = read(simProcesses.at(p % n).infd, &population.at(p).fitness, sizeof(double));

            if(s < (ssize_t)sizeof(double))
            {
                whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Read failed <%d>", errno);
                throw std::runtime_error("Read failed");
            }
        }

        memorizeBestSolution(population);

        for(int32_t i = 0; i < cfg.getAs<int32_t>("maxIterations"); ++i)
        {
            employedBeePhase(population);

            onlookerBeePhase(population);

            memorizeBestSolution(population);

            scoutBeePhase(population);

            if((i % cfg.getAs<int32_t>("saveWeightsPeriod")) == 0)
            {
                saveFitnessPlot();
                saveBestSolution(bestSolution.genes);
            }

            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "[ABC] [%3d] Best fitness: %f", i, bestSolution.fitness);
            histFitness.push_back(bestSolution.fitness);

#           ifdef WHM_GUI
            if(uiCallback)
            {
                simulateWarehouse(bestSolution.genes);
                //updateAllocations(bestSolution.genes);
                uiCallback(bestSolution.fitness);
            }
#           endif
        }

        saveFitnessPlot();
        saveBestSolution(bestSolution.genes);
    }
}

#endif
