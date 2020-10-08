/**
 * Warehouse manager
 *
 * @file    WarehousePort.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class used for interconnection between warehouse items
 */

// Std
#include <iostream>
#include <algorithm>

// Local
#include "WarehousePort.h"
#include "WarehouseItem.h"
#include "WarehouseConnection.h"

#ifdef WHM_GUI
#include "gui/UiWarehousePort.h"
#endif

namespace whm
{
#ifdef WHM_GUI
    WarehousePort_t::WarehousePort_t(gui::UiWarehousePort_t& uiPort)
    {
        (void) uiPort;
    }
#else
    WarehousePort_t::WarehousePort_t()
    {

    }
#endif

    void WarehousePort_t::dump() const
    {
        std::cout << std::endl << "    - Dump warehouse port on item ID <" << this->getWhItem()->getID() << ">" << std::endl;

        if(this->isConnected())
        {
            whConn->dump();
        }
    }

    WarehouseItem_t* WarehousePort_t::getWhItem() const
    {
        return whItem;
    }

    void WarehousePort_t::setWhItem(WarehouseItem_t* i)
    {
        whItem = i;
    }

    WarehouseConnection_t* WarehousePort_t::getWhConn() const
    {
        return whConn;
    }

    void WarehousePort_t::setWhConn(WarehouseConnection_t* c)
    {
        whConn = c;
    }

    bool WarehousePort_t::isConnected() const
    {
        return whConn != nullptr;
    }
}