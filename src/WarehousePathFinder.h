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
#include <utility>
#include <cstdint>
#include <unordered_map>

namespace whm
{
    class WarehouseItem_t;

    using WarehousePath_t = std::vector<std::pair<int32_t, int32_t>>;

    struct WarehousePathInfo_t
    {
        int32_t targetWhItemID{ 0 };     //< Target warehouse item
        WarehousePath_t pathToTarget;    //< Path to go to reach target item (item ids and distances)
    };

    class WarehousePathFinder_t
    {
        public:
            WarehousePathFinder_t();
            ~WarehousePathFinder_t();

            void precalculatePaths(const std::vector<WarehouseItem_t*>&);
            WarehousePathInfo_t* getShortestPath(int32_t, int32_t) const;

            int32_t pathDistance(const WarehousePath_t& path) const;

            void dump() const;

        protected:
            void precalculatePaths(int32_t, const WarehouseItem_t*, WarehousePathInfo_t);

        private:
            std::unordered_map<int32_t, std::vector<WarehousePathInfo_t>> whPaths;
    };
}
