/**
 * Warehouse manager
 *
 * @file    WarehousePathFinderACO.cpp
 * @date    02/07/2020
 * @author  Filip Kocica
 * @brief   Module used to find shortest path for order picking using ant system
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
        delete pheromones;
    }

    void WarehousePathFinderACO_t::init()
    {
        std::random_device rd;
        rand.seed(rd());

        auto whItems = whm::WarehouseLayout_t::getWhLayout().getWhItems();

        precalculatePaths(whItems);

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

        for(int32_t i = 0; i < dimension; ++i)
        {
            nearestNeighbours.push_back(findNearestNeighbours(i));
        }

        // TODO: Default pheromone value (cfg?)
        pheromones = new WarehousePheromones_t(dimension, 1.5);

        // TODO: Delete
        for(int32_t it = 0; it < dimension * dimension; ++it)
        {
            heuristics.push_back(1.75);
        }

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

        for(auto& whOrder : whOrders)
        {
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
    }

    double WarehousePathFinderACO_t::getLocationsDistance(int32_t lhs, int32_t rhs)
    {
        return distances.at(lhs * dimension + rhs);
    }

    double WarehousePathFinderACO_t::getPathDistance(std::vector<int32_t>& path)
    {
        double d{ 0. };

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

    void WarehousePathFinderACO_t::performNextAntStep(WarehouseAnt_t& ant)
    {
        auto lastVisitedLoc = ant.getLastVisited();
        auto lastVisitedLocNn = nearestNeighbours.at(lastVisitedLoc);

        std::vector<int32_t> candidates;

        for(auto loc : lastVisitedLocNn)
        {
            if(!ant.visited(loc))
            {
                candidates.push_back(loc);
            }
        }

        if(Logger_t::getLogger().isVerbose())
        {
            std::cout << "Perform ant step: Candidates: ";
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
                if(!ant.visited(c))
                {
                    double candidatePheromone = pheromones->getEdgePheromones(lastVisitedLoc, c);
                    candidatePheromone *= heuristics.at(lastVisitedLoc * dimension + c);

                    pheromoneSum += candidatePheromone;
                    pheromoneValues.push_back(candidatePheromone);

                    if(Logger_t::getLogger().isVerbose())
                    {
                        std::cout << "Perform ant step: Candidate " << c << ": " << candidatePheromone << " (" << pheromoneSum << ")\n";
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
                if(!ant.visited(it))
                {
                    double candidatePheromone = pheromones->getEdgePheromones(lastVisitedLoc, it);
                    candidatePheromone *= heuristics.at(lastVisitedLoc * dimension + it);

                    if(candidatePheromone > maxPheromone)
                    {
                        maxPheromone = candidatePheromone;
                        nextLoc = it;
                    }

                    if(Logger_t::getLogger().isVerbose())
                    {
                        std::cout << "Perform ant step: Iteration " << it << ": " << candidatePheromone << " (" << maxPheromone << ")\n";
                    }
                }
            }
        }

        if(Logger_t::getLogger().isVerbose())
        {
            std::cout << "Perform ant step: Next location: " << nextLoc << std::endl;
        }

        ant.visit(nextLoc);
    }

    void WarehousePathFinderACO_t::dump() const
    {
        std::cout << std::setw(15) << "";

        for(auto loc : locations)
        {
            std::cout << std::setw(15) << loc;
        }

        std::cout << std::endl;

        for(int32_t i = 0; i < dimension; ++i)
        {
            std::cout << std::setw(15) << locations.at(i);

            for(int32_t j = 0; j < dimension; ++j)
            {
                std::cout << std::setw(5) << distances.at(i * dimension + j)
                          << std::setw(5) << heuristics.at(i * dimension + j)
                          << std::setw(5) << pheromones->getEdgePheromones(i, j);
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
        WarehouseAnt_t ant;
        ant.visit(1);
        ant.visit(5);
        ant.visit(6);
        ant.visit(0);

        performNextAntStep(ant);
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

    int32_t WarehousePathFinderACO_t::WarehouseAnt_t::getLastVisited() const
    {
        return visitedLocations.back();
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
