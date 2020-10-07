/**
 * Warehouse manager
 *
 * @file    WarehouseItem.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Base class for warehouse items (location shelf, conveyor, ...)
 */

// Std
#include <iostream>

// Local
#include "WarehouseItem.h"
#ifdef WHM_GUI
#include "gui/UiWarehouseItem.h"
#endif

namespace whm
{
#ifdef WHM_GUI
    WarehouseItem_t::WarehouseItem_t(gui::UiWarehouseItem_t& uiItem)
    {
        setX(uiItem.getX());
        setY(uiItem.getY());
        setW(uiItem.getW());
        setH(uiItem.getH());
        setID(uiItem.getID());

        // TODO item type TUI <-> GUI
        if (uiItem.getWhItemType() == gui::UiWarehouseItemType_t::E_LOCATION_SHELF)
        {
            setType(0);
        }
        else
        {
            setType(1);
        }
    }
#else
    WarehouseItem_t::WarehouseItem_t()
    {

    }
#endif

    void WarehouseItem_t::dump() const
    {
        std::cout << "==============================================" << std::endl;
        std::cout << "  Warehouse item ID <" << itemID << "> Type <" << itemType << ">" << std::endl;
        std::cout << "==============================================" << std::endl;

        std::cout << "x: " << x << " y: " << y << " w: " << w << " h: " << h << std::endl;

        /*std::for_each(ports.begin(), ports.end(),
                        [](UiWarehousePort_t* p)
                        {
                            p->dump();
                        });*/

        std::cout << "==============================================" << std::endl;
    }
}