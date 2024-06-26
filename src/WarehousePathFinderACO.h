/**
 * Warehouse manager
 *
 * @file    WarehousePathFinderACO.h
 * @date    02/07/2020
 * @author  Filip Kocica
 * @brief   Module used to find the shortest path for order picking using ant colony system
 *          Can be used within simulator for optimized path finding
 */

#if defined(WHM_PAF) || defined(WHM_SIM)

#pragma once

#include <map>
#include <vector>
#include <random>
#include <unistd.h>
#include <stdlib.h>
#include <functional>

#include "Utils.h"
#include "ConfigParser.h"
#include "WarehousePathFinder.h"

namespace whm
{
    class IWarehousePathFinderACO_t
    {
        public:
            virtual void findPath() = 0;
            virtual void calcStats() = 0;
    };

    class WarehousePathFinderACO_t : public WarehousePathFinder_t,
                                     public IWarehousePathFinderACO_t
    {
        private:
            class WarehouseAnt_t
            {
                    friend class WarehousePathFinderACO_t;

                public:
                    WarehouseAnt_t() = default;
                    void visit(int32_t);
                    void setCost(int32_t);
                    bool visited(int32_t);
                    int32_t getCost() const;
                    std::vector<int32_t> getVisited() const;

                    bool operator<(const WarehouseAnt_t& rhs) const { return cost < rhs.cost; }
                    bool operator>(const WarehouseAnt_t& rhs) const { return cost > rhs.cost; }

                private:
                    int32_t cost{ 0 };
                    std::vector<int32_t> visitedLocations;
            };

        public:
            WarehousePathFinderACO_t(const utils::WhmArgs_t&);
            WarehousePathFinderACO_t(const utils::WhmArgs_t&, const ConfigParser_t&);
            virtual ~WarehousePathFinderACO_t();

            // Rand
            double randomFromInterval(double, double);
            int32_t randomFromInterval(int32_t, int32_t);

            // Find
            void lookupStartFinish();
            void findLocationsToVisit();
            std::vector<int32_t> findNearestNeighbours(int32_t);

            // Selection
            int32_t rouletteSelection(double, std::vector<double> const&);

            // Ant actions
            bool performNextAntStep(WarehouseAnt_t&);

            // Callbacks
#           ifdef WHM_GUI
            using UiCallback_t = std::function<void(int32_t, const std::vector<int32_t>&)>;
            void setUiCallback(UiCallback_t);
#           endif

            // Decode
            std::vector<int32_t> decodePath(const std::vector<int32_t>&);

            // Limits
            void updatePheromoneMinMax(double);

            // Getters
            int32_t getBestPathCost() const;
            int32_t getPathDistance(const std::vector<int32_t>&);
            int32_t getLocationsDistance(int32_t, int32_t);
            std::vector<int32_t> getBestPath() const;

            // Debug
            void dump() const;

            // Pheromones
            void initPheromones();
            void evaporation(double);
            void deposit(int32_t, int32_t, double);

            // Clearing
            void clearPathFinder();

            // Init
            void init();
            void prepareOptimization();

            // ACO
            void findPath() override;
            std::vector<int32_t> findPath(int32_t);
            std::vector<int32_t> constructGreedySolution();

            // Statistics
            void calcStats() override;

        protected:
            std::mt19937 rand;
            ConfigParser_t cfg;
            utils::WhmArgs_t args;

            int32_t whStart{ 0 };
            int32_t whFinish{ 0 };
            int32_t dimension{ 0 };

            double probBest{ 0. };
            double pheromoneMax{ 0. };
            double pheromoneMin{ 0. };

            std::vector<int32_t> results;
            std::vector<int32_t> locations;

            std::vector<std::vector<double>> heuristics;
            std::vector<std::vector<double>> edgePheromones;

            std::vector<std::vector<int32_t>> distances;
            std::vector<std::vector<int32_t>> nearestNeighbours;

#           ifdef WHM_GUI
            UiCallback_t uiCallback;
#           endif

            WarehouseAnt_t bestWhAnt;
            std::vector<WarehouseAnt_t> whAnts;
    };
}

#endif
