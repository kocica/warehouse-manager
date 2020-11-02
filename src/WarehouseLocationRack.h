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

    template<typename T>
    class WarehouseLocationRack_t
    {
        public:
            using LocationSlot_t = WarehouseLocationSlot_t<T>;
            using LocationSlots_t = std::vector<std::vector<LocationSlot_t>>;
            using SortedLocationSlots_t = std::vector<LocationSlot_t*>;

            WarehouseLocationRack_t(WarehouseItem_t*, size_t, size_t);
            ~WarehouseLocationRack_t();

            LocationSlot_t& at(size_t, size_t);
            void setAt(size_t, size_t, const LocationSlot_t&);

            void init(size_t, size_t);

            bool containsArticle(const T&, int32_t);
            bool containsArticle(const T&, int32_t, std::pair<size_t, size_t>&);

            void exportSlots(std::ostream&) const;
            void importSlots(std::istream&);

            WarehouseItem_t* getWhItem() const;

            int32_t getSlotCountX() const;
            int32_t getSlotCountY() const;

            const LocationSlots_t& getSlots() const;
            const SortedLocationSlots_t& getSortedSlots() const;

            int32_t getOccupationLevel() const;
            LocationSlot_t* getFirstFreeSlot();

            void dump() const;

        private:
            LocationSlots_t slots;
            SortedLocationSlots_t sortedSlots;

            WarehouseItem_t* whItem{ nullptr };
    };
}
