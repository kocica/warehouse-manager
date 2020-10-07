/**
 * Warehouse manager
 *
 * @file    WarehouseLayout.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class managing whole warehouse layout (locations, connections, ...)
 */

#pragma once

#include <vector>

#ifdef WHM_GUI
namespace whm
{
    namespace gui
    {
        class UiWarehouseLayout_t;
    }
}
#endif

namespace whm
{
    class WarehouseItem_t;

    class WarehouseLayout_t
    {
        using WarehouseItemContainer_t = std::vector<WarehouseItem_t*>;

        public:
            ~WarehouseLayout_t() = default;

#ifdef WHM_GUI
            WarehouseLayout_t() = delete;
            WarehouseLayout_t(gui::UiWarehouseLayout_t&);
#else
            WarehouseLayout_t();
#endif

            WarehouseItemContainer_t getWhItems() const;

            void dump() const;

        protected:
            WarehouseItemContainer_t whItems;
    };
}