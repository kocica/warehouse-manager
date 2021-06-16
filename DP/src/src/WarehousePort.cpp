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
#include "Logger.h"
#include "WarehousePort.h"
#include "WarehouseItem.h"
#include "WarehouseConnection.h"

#ifdef WHM_GUI
#include "gui/UiWarehousePort.h"
#endif

namespace whm
{
    WarehousePort_t::WarehousePort_t()
    {

    }

#ifdef WHM_GUI
    WarehousePort_t::WarehousePort_t(gui::UiWarehousePort_t& uiPort)
        : whPortID{ uiPort.getWhPortID() }
    {

    }
#endif

    WarehousePort_t::~WarehousePort_t()
    {
        if (this->isConnected())
        {
            delete whConn;
        }
    }

    void WarehousePort_t::disconnect()
    {
        if(this->isConnected())
        {
            whConn = nullptr;
        }
    }

    void WarehousePort_t::serializeToXml(tinyxml2::XMLElement* elem) const
    {
        elem->SetAttribute("id", getWhPortID());

        if (this->isConnected())
        {
            elem->SetAttribute("conn_id", this->getWhConn()->getWhConnID());
        }
    }

    void WarehousePort_t::deserializeFromXml(tinyxml2::XMLElement* elem)
    {
        whPortID = elem->IntAttribute("id");
    }

    void WarehousePort_t::dump() const
    {
        std::cout << "    - Dump warehouse port ID <" << this->whPortID << "> on item ID <" << this->getWhItem()->getWhItemID() << ">" << std::endl;

        if (this->isConnected())
        {
            whConn->dump();
        }
    }

    int32_t WarehousePort_t::getWhPortID() const
    {
        return whPortID;
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