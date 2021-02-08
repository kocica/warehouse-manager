/**
 * Warehouse manager
 *
 * @file    WarehousePathFinderACO.h
 * @date    02/07/2020
 * @author  Filip Kocica
 * @brief   Module used to find shortest path for order picking using ant system
 */

#ifdef WHM_PAF

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
    };

    class WarehousePathFinderACO_t : public WarehousePathFinder_t,
                                     public IWarehousePathFinderACO_t
    {
        private:
            class WarehouseAnt_t
            {
                public:
                    WarehouseAnt_t() = default;
                    void visit(int32_t);
                    bool visited(int32_t);
                    int32_t getLastVisited() const;

                private:
                    std::vector<int32_t> visitedLocations;
            };

            class WarehousePheromones_t
            {
                public:
                    WarehousePheromones_t(int32_t, double);
                    void deposit(int32_t, int32_t, double, double);
                    void evaporation(double, double);
                    double getEdgePheromones(int32_t, int32_t);

                private:
                    int32_t dimension;
                    std::vector<double> edgePheromones;
            };

        public:
            WarehousePathFinderACO_t(const utils::WhmArgs_t&);
            WarehousePathFinderACO_t(const utils::WhmArgs_t&, const ConfigParser_t&);
            ~WarehousePathFinderACO_t();

            // Rand
            double randomFromInterval(double, double);
            int32_t randomFromInterval(int32_t, int32_t);

            // Find
            void findLocationsToVisit();
            std::vector<int32_t> findNearestNeighbours(int32_t);

            // Selection
            int32_t rouletteSelection(double, std::vector<double> const&);

            // Ant actions
            void performNextAntStep(WarehouseAnt_t&);

            // Getters
            double getPathDistance(std::vector<int32_t>&);
            double getLocationsDistance(int32_t, int32_t);

            // Debug
            void dump() const;

            // Init
            void init();

            // Run ACO
            void findPath();

        protected:
            std::mt19937 rand;
            ConfigParser_t cfg;
            utils::WhmArgs_t args;

            int32_t dimension{ 0 };
            std::vector<double> distances;
            std::vector<double> heuristics;
            std::vector<int32_t> locations;
            std::vector<int32_t> locationsToVisit;
            std::vector<std::vector<int32_t>> nearestNeighbours;

            WarehousePheromones_t* pheromones{ nullptr };
    };
}

#endif
