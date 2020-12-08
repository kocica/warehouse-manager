/**
 * Warehouse manager
 *
 * @file    WarehouseOrder.h
 * @date    11/09/2020
 * @author  Filip Kocica
 * @brief   Class representing one customer order including order lines
 */

#pragma once

// Std
#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>

// Local
#include "tinyxml2.h"
#include "WarehouseTypes.h"
#include "WarehouseOrderLine.h"

namespace whm
{
    class WarehouseOrder_t
    {
        using WarehouseOrderLineContainer_t = std::vector<WarehouseOrderLine_t>;

        public:
            WarehouseOrder_t();
            ~WarehouseOrder_t();

            typename WarehouseOrderLineContainer_t::iterator begin() { return whOrderLines.begin(); }
            typename WarehouseOrderLineContainer_t::iterator end() { return whOrderLines.end(); }

            void setWhOrderID(int32_t);
            int32_t getWhOrderID() const;

            void setWhOrderType(WarehouseOrderType_t);
            WarehouseOrderType_t getWhOrderType() const;

            void setWhOrderLines(WarehouseOrderLineContainer_t);
            const WarehouseOrderLineContainer_t& getWhOrderLines() const;

            void deserializeFromXml(tinyxml2::XMLElement*);
            void serializeToXml(tinyxml2::XMLDocument* doc) const;
            void dump() const;

        private:
            int32_t whOrderID{ 0 };
            WarehouseOrderType_t whOrderType;
            WarehouseOrderLineContainer_t whOrderLines;
    };
}
