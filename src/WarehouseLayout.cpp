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
#include "WarehouseConnection.h"

#ifdef WHM_GUI
#include "gui/UiWarehouseLayout.h"
#endif

#include "tinyxml2.h"

namespace whm
{
    WarehouseLayout_t::WarehouseLayout_t()
    {

    }

    WarehouseLayout_t::~WarehouseLayout_t()
    {
        // Delete all whItems & whConns
    }

    WarehouseLayout_t& WarehouseLayout_t::getWhLayout()
    {
        static WarehouseLayout_t l;
        return l;
    }

#ifdef WHM_GUI
    void WarehouseLayout_t::initFromGui(gui::UiWarehouseLayout_t& uiLayout)
    {
        this->clearWhLayout();

        auto uiItems = uiLayout.getWhItems();
        auto uiConns = uiLayout.getWhConns();

        for(auto* uiItem : uiItems)
        {
            whItems.emplace_back(new WarehouseItem_t{ *uiItem });
        }

        for(auto* uiConn : uiConns)
        {
            whConns.emplace_back(new WarehouseConnection_t{ *uiConn });
        }
    }
#endif

    void WarehouseLayout_t::serializeToXml(const std::string& xmlFilename)
    {
        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

        for(auto* whItem : whItems)
        {
            whItem->serializeToXml(doc);
        }

        doc->SaveFile( xmlFilename.c_str() );
    }

    WarehouseLayout_t::WarehouseItemContainer_t WarehouseLayout_t::getWhItems() const
    {
        return whItems;
    }

    WarehouseLayout_t::WarehouseConnContainer_t WarehouseLayout_t::getWhConns() const
    {
        return whConns;
    }

    void WarehouseLayout_t::dump() const
    {
        std::cout << std::endl << "Dump warehouse layout - start" << std::endl << std::endl;

        std::for_each(whItems.begin(), whItems.end(),
                      [](const WarehouseItem_t* whItem) -> void
                      {
                          whItem->dump();
                      });

        std::cout << std::endl << std::endl;

        std::for_each(whConns.begin(), whConns.end(),
                      [](const WarehouseConnection_t* whConn) -> void
                      {
                          whConn->dump();
                      });

        std::cout << std::endl << "Dump warehouse layout - end" << std::endl << std::endl << std::endl;
    }

    void WarehouseLayout_t::clearWhLayout()
    {
        for (WarehouseItem_t* whItem : whItems)
        {
            delete whItem;
        }

        for (WarehouseConnection_t* whConn : whConns)
        {
            delete whConn;
        }

        whItems.clear();
        whConns.clear();
    }
}