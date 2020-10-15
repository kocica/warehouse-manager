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
        using LocationSlot_t = WarehouseLocationSlot_t<T>;
        using LocationSlots_t = std::vector<std::vector<LocationSlot_t>>;

        public:
            WarehouseLocationRack_t(WarehouseItem_t*, size_t, size_t);
            ~WarehouseLocationRack_t();

            LocationSlot_t at(size_t, size_t) const;
            void setAt(size_t, size_t, const LocationSlot_t&);

            void init(size_t, size_t);

            bool containsArticle(const T&, int32_t, std::pair<size_t, size_t>&);

            void exportSlots(std::ostream&) const;
            void importSlots(std::istream&);

            WarehouseItem_t* getWhItem() const;

            int32_t getSlotCountX() const;
            int32_t getSlotCountY() const;

            void dump() const;

        private:
            LocationSlots_t slots;
            WarehouseItem_t* whItem{ nullptr };
    };
}