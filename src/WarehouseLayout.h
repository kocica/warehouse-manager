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
    class WarehouseConnection_t;

    class WarehouseLayout_t
    {
        using WarehouseItemContainer_t = std::vector<WarehouseItem_t*>;
        using WarehouseConnContainer_t = std::vector<WarehouseConnection_t*>;

        public:
            WarehouseLayout_t();
            ~WarehouseLayout_t();

            static WarehouseLayout_t& getWhLayout();

#ifdef WHM_GUI
            void initFromGui(gui::UiWarehouseLayout_t&);
#endif

            WarehouseItemContainer_t getWhItems() const;
            WarehouseConnContainer_t getWhConns() const;

            void dump() const;
            void clearWhLayout();

        protected:
            WarehouseItemContainer_t whItems;
            WarehouseConnContainer_t whConns;
    };
}