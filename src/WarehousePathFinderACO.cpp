/**
 * Warehouse manager
 *
 * @file    WarehousePathFinderACO.cpp
 * @date    02/07/2020
 * @author  Filip Kocica
 * @brief   Module used to find the shortest path for order picking using ant colony system
 */

#ifdef WHM_PAF

#include <iomanip>
#include <numeric>

#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseOrder.h"
#include "WarehouseTypes.h"
#include "WarehouseLayout.h"
#include "WarehouseOrderLine.h"
#include "WarehouseLocationRack.h"
#include "WarehousePathFinderACO.h"

namespace whm
{
    WarehousePathFinderACO_t::WarehousePathFinderACO_t(const utils::WhmArgs_t& args_)
        : cfg{ ConfigParser_t{ "cfg/pathfinder.xml" } }
        , args{ args_ }
    {
        init();
    }

    WarehousePathFinderACO_t::WarehousePathFinderACO_t(const utils::WhmArgs_t& args_, const ConfigParser_t& cfg_)
        : cfg{ cfg_ }
        , args{ args_ }
    {
        init();
    }

    WarehousePathFinderACO_t::~WarehousePathFinderACO_t()
    {

    }

    void WarehousePathFinderACO_t::init()
    {
        std::random_device rd;
        rand.seed(rd());

        auto whItems = whm::WarehouseLayout_t::getWhLayout().getWhItems();

        precalculatePaths(whItems);

        lookupStartFinish();

        for(auto* whItem : whItems)
        {
            if(whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
            {
                ++dimension;
                locations.push_back(whItem->getWhItemID());
            }
        }

        distances.resize(dimension * dimension, 0);

        for(int32_t i = 0; i < dimension; ++i)
        {
            for(int32_t j = 0; j < dimension; ++j)
            {
                auto* shortestPath = getShortestPath(locations.at(i), locations.at(j));

                if(shortestPath)
                {
                    distances.at(i * dimension + j) = pathDistance(shortestPath->pathToTarget);
                }
            }
        }

        findLocationsToVisit();

        for(int32_t it = 0; it < dimension; ++it)
        {
            nearestNeighbours.push_back(findNearestNeighbours(it));
        }

        for(int32_t it = 0; it < dimension * dimension; ++it)
        {
            heuristics.push_back(1.0 / std::pow(distances.at(it), cfg.getAs<double>("beta")));
        }

        this->probBest = cfg.getAs<double>("probBest");

        bestWhAnt.setCost(std::numeric_limits<int32_t>::max());

        if(Logger_t::getLogger().isVerbose())
        {
            dump();
        }
    }

    std::vector<int32_t> WarehousePathFinderACO_t::findNearestNeighbours(int32_t loc)
    {
        std::vector<int32_t> nns(dimension);

        std::iota(nns.begin(), nns.end(), 0);

        std::sort(nns.begin(), nns.end(),
                  [loc, this](int32_t l, int32_t r) -> bool
                  {
                      return getLocationsDistance(loc, l) <
                             getLocationsDistance(loc, r) ;
                  });

        return std::vector<int32_t>(nns.begin() + 1,
                                    nns.begin() + 1 + cfg.getAs<int32_t>("nearestNeighbours"));
    }

    void WarehousePathFinderACO_t::findLocationsToVisit()
    {
        auto whItems = whm::WarehouseLayout_t::getWhLayout().getWhItems();
        auto whOrders = whm::WarehouseLayout_t::getWhLayout().getWhOrders();

        auto selectedOrderID = cfg.getAs<int32_t>("selectedOrderID");

        if(selectedOrderID < 0 || selectedOrderID >= static_cast<int32_t>(whOrders.size()))
        {
            Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "PathFinder: selectedOrderID out of range!");
            throw std::runtime_error("PathFinder: selectedOrderID out of range!");
        }

        auto& whOrder = whOrders.at(selectedOrderID);

        for(auto& whLine : whOrder.getWhOrderLines())
        {
            auto article = whLine.getArticle();

            for(auto* whItem : whItems)
            {
                if(whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF &&
                    whItem->getWhLocationRack()->containsArticle(article, 0))
                {
                    if(!utils::contains(locationsToVisit, whItem->getWhItemID()))
                    {
                        locationsToVisit.push_back(whItem->getWhItemID());
                    }
                }
            }
        }
    }

    void WarehousePathFinderACO_t::lookupStartFinish()
    {
        auto whItems = whm::WarehouseLayout_t::getWhLayout().getWhItems();
        auto whOrders = whm::WarehouseLayout_t::getWhLayout().getWhOrders();

        for(auto* whItem : whItems)
        {
            if(whItem->getType() == WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)
            {
                whStart = dimension++;
                locations.push_back(whItem->getWhItemID());
                locationsToVisit.push_back(whItem->getWhItemID());
            }
            if(whItem->getType() == WarehouseItemType_t::E_WAREHOUSE_DISPATCH)
            {
                // Don't add to vector, or make sure to visit it as a last location
                whFinish = dimension++;
                locations.push_back(whItem->getWhItemID());
                locationsToVisit.push_back(whItem->getWhItemID());
            }
        }
    }

    int32_t WarehousePathFinderACO_t::getLocationsDistance(int32_t lhs, int32_t rhs)
    {
        return distances.at(lhs * dimension + rhs);
    }

    int32_t WarehousePathFinderACO_t::getPathDistance(const std::vector<int32_t>& path)
    {
        int32_t d{ 0 };

        if(!path.empty())
        {
            int32_t prevLoc = path.back();

            for(int32_t actLoc : path)
            {
                d += getLocationsDistance(prevLoc, actLoc);
                prevLoc = actLoc;
            }
        }

        return d;
    }

    double WarehousePathFinderACO_t::randomFromInterval(double a, double b)
    {
        std::uniform_real_distribution<double> dist(a, b);
        return dist(rand);
    }

    int32_t WarehousePathFinderACO_t::randomFromInterval(int32_t a, int32_t b)
    {
        std::uniform_int_distribution<int32_t> dist(a, b);
        return dist(rand);
    }

    int32_t WarehousePathFinderACO_t::rouletteSelection(double sum, std::vector<double> const& vals)
    {
        int32_t idx{ 0 };
        double iter{ 0.0 };
        double random{ randomFromInterval(0.0, 1.0) * sum }; // Make sure of using floating point overloaded func

        for(auto v : vals)
        {
            ++ idx;
            iter += v;

            if(Logger_t::getLogger().isVerbose())
            {
                std::cout << "RW: " << random << " / " << v << " / " << iter << std::endl;
            }

            if(random < iter)
            {
                break;
            }
        }

        if(Logger_t::getLogger().isVerbose())
        {
            std::cout << "RW return: " << idx - 1 << std::endl;
        }

        return idx - 1;
    }

    bool WarehousePathFinderACO_t::performNextAntStep(WarehouseAnt_t& whAnt)
    {
        auto lastVisitedLoc = whAnt.getVisited().back();
        auto lastVisitedLocNn = nearestNeighbours.at(lastVisitedLoc);

        std::vector<int32_t> candidates;

        for(auto loc : lastVisitedLocNn)
        {
            if(!whAnt.visited(loc))
            {
                candidates.push_back(loc);
            }
        }

        if(Logger_t::getLogger().isVerbose())
        {
            std::cout << "Perform WH ant step: Candidates: ";
            std::for_each(candidates.begin(), candidates.end(), [](auto c){ std::cout << c << ", "; });
            std::cout << std::endl;
        }

        auto nextLoc = lastVisitedLoc;

        if(!candidates.empty())
        {
            double pheromoneSum{ 0.0 };
            std::vector<double> pheromoneValues;

            for(auto c : candidates)
            {
                if(!whAnt.visited(c))
                {
                    double candidatePheromone = whPheromones->getEdgePheromones(lastVisitedLoc, c);
                    candidatePheromone *= heuristics.at(lastVisitedLoc * dimension + c);

                    pheromoneSum += candidatePheromone;
                    pheromoneValues.push_back(candidatePheromone);

                    if(Logger_t::getLogger().isVerbose())
                    {
                        std::cout << "Perform WH ant step: Candidate " << c << ": " << candidatePheromone << " (" << pheromoneSum << ")\n";
                    }
                }
            }

            // Roulette wheel selection of candidates (not yet visited nearest neighbour)
            nextLoc = candidates.at(rouletteSelection(pheromoneSum, pheromoneValues));
        }
        else
        {
            // Select best fitting not yet visited location (highest amount of pheromone on the way)
            double maxPheromone{ 0.0 };

            for(int32_t it = 0; it < dimension; ++it)
            {
                if(!whAnt.visited(it))
                {
                    double candidatePheromone = whPheromones->getEdgePheromones(lastVisitedLoc, it);
                    candidatePheromone *= heuristics.at(lastVisitedLoc * dimension + it);

                    if(candidatePheromone > maxPheromone)
                    {
                        maxPheromone = candidatePheromone;
                        nextLoc = it;
                    }

                    if(Logger_t::getLogger().isVerbose())
                    {
                        std::cout << "Perform WH ant step: Iteration " << it << ": " << candidatePheromone << " (" << maxPheromone << ")\n";
                    }
                }
            }
        }

        if(Logger_t::getLogger().isVerbose())
        {
            std::cout << "Perform WH ant step: Next location: " << nextLoc << std::endl;
        }

        if(nextLoc == lastVisitedLoc)
        {
            // Ant already visited all locations
            return false;
        }

        whAnt.visit(nextLoc);
        return true;
    }

#   ifdef WHM_GUI
    void WarehousePathFinderACO_t::setUiCallback(UiCallback_t c)
    {
        this->uiCallback = c;
    }
#   endif

    std::vector<int32_t> WarehousePathFinderACO_t::decodePath(const std::vector<int32_t>& path)
    {
        std::vector<int32_t> decodedPath;

        for(int32_t p : path)
        {
            decodedPath.push_back(locations.at(p)); // locationsToVisit?
        }

        return std::move(decodedPath);
    }

    std::vector<int32_t> WarehousePathFinderACO_t::constructGreedySolution()
    {
        WarehouseAnt_t whAnt;

        whAnt.visit(whStart);

        auto actualLoc = whStart;

        // Skip first (start)
        for(int32_t it = 1; it < dimension; ++it)
        {
            bool found{ false };
            int32_t nextLoc{ -1 };
            auto const& locNns = nearestNeighbours.at(actualLoc);

            for(auto locNn : locNns)
            {
                if(!whAnt.visited(locNn))
                {
                    found = true;
                    nextLoc = locNn;
                    break;
                }
            }

            if(!found)
            {
                int32_t minDistance = std::numeric_limits<int32_t>::max();

                for(int32_t it2 = 0; it2 < dimension; ++it2)
                {
                    if(!whAnt.visited(it2))
                    {
                        auto d = this->getLocationsDistance(actualLoc, it2);

                        if(d < minDistance && d > 0)
                        {
                            nextLoc = it2;
                            minDistance = d;
                        }
                    }
                }
            }

            whAnt.visit(nextLoc);
            actualLoc = nextLoc;
        }

        return whAnt.getVisited();
    }

    void WarehousePathFinderACO_t::updatePheromoneMinMax(double c)
    {
        pheromoneMax = 1.0 / ((1.0 - cfg.getAs<double>("rho")) * c);
        double pBest = std::pow(this->probBest, 1.0 / static_cast<double>(dimension));
        pheromoneMin = std::min(pheromoneMax, pheromoneMax * (1 - pBest) / (((dimension / 2.0) - 1) * pBest));
    }

    void WarehousePathFinderACO_t::dump() const
    {
        std::cout << "WH Start:  " << whStart  << std::endl
                  << "WH Finish: " << whFinish << std::endl << std::endl;

        std::cout << std::setw(20) << "";

        for(auto loc : locations)
        {
            std::cout << std::setw(20) << loc;
        }

        std::cout << std::endl;

        for(int32_t i = 0; i < dimension; ++i)
        {
            std::cout << std::setw(20) << locations.at(i);

            for(int32_t j = 0; j < dimension; ++j)
            {
                std::cout << std::setw(4)  << distances.at(i * dimension + j)
                          << std::setw(12) << heuristics.at(i * dimension + j)
                          ; //<< std::setw(4)  << whPheromones ? whPheromones->getEdgePheromones(i, j) : 0;
            }

            std::cout << "     Nearest neighbours: ";
            for(auto nn : nearestNeighbours.at(i))
            {
                std::cout << nn << ", ";
            }
            std::cout << std::endl;
        }

        std::cout << std::endl << std::endl << "To visit: ";
        std::for_each(locationsToVisit.begin(), locationsToVisit.end(), [](auto l){ std::cout << l << ", "; });
        std::cout << std::endl << std::endl;
    }

    void WarehousePathFinderACO_t::findPath()
    {
        auto const& sol = this->constructGreedySolution();
        double evalSol  = this->getPathDistance(sol);

        updatePheromoneMinMax(evalSol);

        whPheromones = new WarehousePheromones_t(dimension, pheromoneMax);

        for(int32_t it = 0; it < cfg.getAs<int32_t>("maxIterations"); ++it)
        {
            for(int32_t a = 0; a < cfg.getAs<int32_t>("antCount"); ++a)
            {
                WarehouseAnt_t whAnt;
                whAnt.visit(whStart);
                while(performNextAntStep(whAnt));
                whAnt.setCost(getPathDistance(whAnt.getVisited()));
                whAnts.emplace_back(std::move(whAnt));
            }

            WarehouseAnt_t iterationBestWhAnt;
            iterationBestWhAnt.setCost(std::numeric_limits<int32_t>::max());

            for(WarehouseAnt_t& whAnt : whAnts)
            {
                if(whAnt < iterationBestWhAnt)
                {
                    iterationBestWhAnt = whAnt;
                }
                if(whAnt < bestWhAnt)
                {
                    bestWhAnt = whAnt;

                    if(randomFromInterval(0.0, 1.0) < cfg.getAs<double>("probUseIterationBest"))
                    {
                        updatePheromoneMinMax(iterationBestWhAnt.getCost());
                    }
                    else
                    {
                        updatePheromoneMinMax(bestWhAnt.getCost());
                    }
                }
            }

#           ifdef WHM_GUI
                this->uiCallback(bestWhAnt.getCost(), decodePath(bestWhAnt.getVisited()));
#           endif

            // Evaporate from all edges
            whPheromones->evaporation(1.0 - cfg.getAs<double>("rho"), pheromoneMin);

            // Deposit pheromone on all edges that 'iteration best' ant visited
            auto prevLocation = iterationBestWhAnt.getVisited().back();
            for(auto location : iterationBestWhAnt.getVisited())
            {
                whPheromones->deposit(prevLocation, location, 1.0 / iterationBestWhAnt.getCost(), pheromoneMax);

                prevLocation = location;
            }

            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "[ACO] [%3d] Best cost: %d", it, bestWhAnt.getCost());
        }

        delete whPheromones;
    }

    void WarehousePathFinderACO_t::WarehouseAnt_t::visit(int32_t loc)
    {
        if(!visited(loc))
        {
            visitedLocations.push_back(loc);
        }
    }

    bool WarehousePathFinderACO_t::WarehouseAnt_t::visited(int32_t loc)
    {
        return utils::contains(visitedLocations, loc);
    }

    std::vector<int32_t> WarehousePathFinderACO_t::WarehouseAnt_t::getVisited() const
    {
        return visitedLocations;
    }

    void WarehousePathFinderACO_t::WarehouseAnt_t::setCost(int32_t c)
    {
        this->cost = c;
    }

    int32_t WarehousePathFinderACO_t::WarehouseAnt_t::getCost() const
    {
        return this->cost;
    }

    WarehousePathFinderACO_t::WarehousePheromones_t::WarehousePheromones_t(int32_t d, double i)
        : dimension{ d }
    {
        for(int32_t it = 0; it < d * d; ++it)
        {
            edgePheromones.push_back(i);
        }
    }

    double WarehousePathFinderACO_t::WarehousePheromones_t::getEdgePheromones(int32_t lhs, int32_t rhs)
    {
        return edgePheromones.at(lhs * dimension + rhs);
    }

    void WarehousePathFinderACO_t::WarehousePheromones_t::evaporation(double rate, double min)
    {
        std::for_each(edgePheromones.begin(), edgePheromones.end(),
                      [=](double& edgePheromone) -> void
                      {
                          edgePheromone = std::max((1 - rate) * edgePheromone, min);
                      });
    }

    void WarehousePathFinderACO_t::WarehousePheromones_t::deposit(int32_t lhs, int32_t rhs, double increase, double max)
    {
        double actEdgePheromones = getEdgePheromones(lhs, rhs);

        edgePheromones.at(lhs * dimension + rhs) = std::min(actEdgePheromones + increase, max);
    }
}

#endif
