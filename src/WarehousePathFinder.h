/**
 * Warehouse manager
 *
 * @file    WarehousePathFinder.h
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Class used for finding paths between warehouse items
 */

#pragma once

#include <vector>
#include <cstdint>
#include <unordered_map>

namespace whm
{
    class WarehouseItem_t;

    struct WarehousePathInfo_t
    {
        int32_t targetWhItemID{ 0 };        //< Target warehouse item
        std::vector<int32_t> pathWhItemIDs; //< Path to go to reach target item
        int32_t distance{ 0 };              //< How many meters it takes to reach target item
    };

    class WarehousePathFinder_t
    {
        public:
            WarehousePathFinder_t();
            ~WarehousePathFinder_t();

            void precalculatePaths(const std::vector<WarehouseItem_t*>&);
            WarehousePathInfo_t* getShortestPath(int32_t, int32_t) const;

            void dump() const;

        protected:
            void precalculatePaths(int32_t, const WarehouseItem_t*, WarehousePathInfo_t);

        private:
            std::unordered_map<int32_t, std::vector<WarehousePathInfo_t>> whPaths;
    };
}
