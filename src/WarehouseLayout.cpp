/**
 * Warehouse manager
 *
 * @file    WarehouseLayout.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class managing whole warehouse layout (locations, connections, ...)
 */

// Std
#include <iostream>
#include <algorithm>

// Local
#include "WarehouseItem.h"
#include "WarehouseLayout.h"

#ifdef WHM_GUI
#include "gui/UiWarehouseLayout.h"
#endif

namespace whm
{
#ifdef WHM_GUI
    WarehouseLayout_t::WarehouseLayout_t(gui::UiWarehouseLayout_t& uiLayout)
    {
        auto uiItems = uiLayout.getWhItems();

        for(auto* uiItem : uiItems)
        {
            whItems.emplace_back(new WarehouseItem_t{ *uiItem });
        }
    }
#else
    WarehouseLayout_t::WarehouseLayout_t()
    {

    }
#endif

    WarehouseLayout_t::WarehouseItemContainer_t WarehouseLayout_t::getWhItems() const
    {
        return whItems;
    }

    void WarehouseLayout_t::dump() const
    {
        std::cout << std::endl << "Dump warehouse layout - start" << std::endl << std::endl;

        std::for_each(whItems.begin(), whItems.end(),
                      [](const WarehouseItem_t* whItem) -> void
                      {
                          whItem->dump();
                      });

        std::cout << std::endl << "Dump warehouse layout - end" << std::endl << std::endl << std::endl;
    }
}