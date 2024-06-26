/**
 * Warehouse manager
 *
 * @file    WarehouseItem.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Base class for warehouse items (location shelf, conveyor, ...)
 */

#pragma once

// Std
#include <cstdint>
#include <vector>

// Local
#include "tinyxml2.h"
#include "WarehouseTypes.h"

#ifdef WHM_GUI
namespace whm
{
    namespace gui
    {
        class UiWarehouseItem_t;
    }
}
#endif

namespace whm
{
    class WarehousePort_t;
    class WarehouseLocationRack_t;

    class WarehouseItem_t
    {
        using WarehousePortContainer_t = std::vector<WarehousePort_t*>;

        public:
            WarehouseItem_t();
            virtual ~WarehouseItem_t();

#ifdef WHM_GUI
            WarehouseItem_t(gui::UiWarehouseItem_t&);
#endif

            void setWhItemID(int32_t id)           { whItemID = id; }
            void setType(WarehouseItemType_t type) { whItemType = type; }
            void setX(int32_t x_)                  { x = x_; }
            void setY(int32_t y_)                  { y = y_; }
            void setW(int32_t w_)                  { w = w_; }
            void setH(int32_t h_)                  { h = h_; }
            void setO(int32_t o_)                  { o = o_; }
            void setWorkload(double w)             { workload = w; }

            int32_t getWhItemID() const         { return whItemID; }
            WarehouseItemType_t getType() const { return whItemType; }
            int32_t getX() const                { return x; }
            int32_t getY() const                { return y; }
            int32_t getW() const                { return w; }
            int32_t getH() const                { return h; }
            int32_t getO() const                { return o; }
            double getWorkload() const          { return workload; }

            WarehousePortContainer_t getWhPorts() const;
            WarehouseLocationRack_t* getWhLocationRack() const;

            void dump() const;

            // (De)-serialization
            void serializeToXml(tinyxml2::XMLDocument* doc) const;
            void deserializeFromXml(tinyxml2::XMLElement*);

            int32_t getSlotCountX() const;
            int32_t getSlotCountY() const;

        protected:
            int32_t x{ 0 };
            int32_t y{ 0 };
            int32_t w{ 0 };
            int32_t h{ 0 };
            int32_t o{ 0 };
            double workload{ 0.0 };

            int32_t whItemID{ 0 };
            WarehouseItemType_t whItemType;

            WarehousePortContainer_t whPorts;

            WarehouseLocationRack_t* whLocRack{ nullptr };
    };
}