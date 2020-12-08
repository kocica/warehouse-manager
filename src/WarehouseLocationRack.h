/**
 * Warehouse manager
 *
 * @file    WarehouseLocationRack.h
 * @date    10/09/2020
 * @author  Filip Kocica
 * @brief   Class representing location rack with slots containing products
 */

#pragma once

// Std
#include <vector>

// Local
#include "WarehouseLocationSlot.h"

namespace whm
{
    class WarehouseItem_t;

    class WarehouseLocationRack_t
    {
        public:
            using SortedLocationSlots_t = std::vector<WarehouseLocationSlot_t*>;
            using LocationSlots_t = std::vector<std::vector<WarehouseLocationSlot_t>>;

            WarehouseLocationRack_t(WarehouseItem_t*, size_t, size_t);
            ~WarehouseLocationRack_t();

            WarehouseLocationSlot_t& at(size_t, size_t);
            void setAt(size_t, size_t, const WarehouseLocationSlot_t&);

            void init(size_t, size_t);

            bool containsArticle(const std::string&, int32_t);
            bool containsArticle(const std::string&, int32_t, std::pair<size_t, size_t>&);

            void replenishArticle(const std::string&, int32_t, std::pair<size_t, size_t>&);

            void exportSlots(std::ostream&) const;
            void importSlots(std::istream&);

            WarehouseItem_t* getWhItem() const;

            int32_t getSlotCountX() const;
            int32_t getSlotCountY() const;

            const LocationSlots_t& getSlots() const;
            const SortedLocationSlots_t& getSortedSlots() const;

            int32_t getOccupationLevel() const;
            WarehouseLocationSlot_t* getFirstFreeSlot();

            void resetRack();
            void dump() const;

        private:
            LocationSlots_t whSlots;
            SortedLocationSlots_t sortedSlots;

            WarehouseItem_t* whItem{ nullptr };
    };
}
