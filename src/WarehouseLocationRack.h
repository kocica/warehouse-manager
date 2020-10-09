/**
 * Warehouse manager
 *
 * @file    WarehouseLocationRack.h
 * @date    10/09/2020
 * @author  Filip Kocica
 * @brief   Class representing location rack with slots containing products
 */

#pragma once

#include <vector>
#include "WarehouseLocationSlot.h"

namespace whm
{
    template<typename T>
    class WarehouseLocationRack_t
    {
        using LocationSlot_t = WarehouseLocationSlot_t<T>;
        using LocationSlots_t = std::vector<std::vector<LocationSlot_t>>;

        public:
            WarehouseLocationRack_t();
            ~WarehouseLocationRack_t();

            LocationSlot_t at(size_t, size_t) const;
            void setAt(size_t, size_t, const LocationSlot_t&);
            void init(size_t, size_t);

            void dump() const;

        private:
            LocationSlots_t slots;
    };
}