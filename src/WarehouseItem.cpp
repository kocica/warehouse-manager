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
#include <algorithm>

// Local
#include "WarehouseItem.h"
#include "WarehousePort.h"

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

        auto uiPorts = uiItem.getWhPorts();
        std::for_each(uiPorts.begin(), uiPorts.end(),
                      [&](auto uiPort) -> void
                      {
                          auto newWhPort = new WarehousePort_t(*uiPort);
                          newWhPort->setWhItem(this);
                          whPorts.emplace_back(newWhPort);
                      });
    }
#else
    WarehouseItem_t::WarehouseItem_t()
    {

    }
#endif

    void WarehouseItem_t::serializeToXml(tinyxml2::XMLDocument* doc) const
    {
        tinyxml2::XMLNode* whItem = doc->InsertEndChild( doc->NewElement( "WarehouseItem" ) );
        tinyxml2::XMLElement* whItemAttribs = doc->NewElement( "Attributes" );

        // Set attributes
        whItemAttribs->SetAttribute( "id", getID() );
        whItemAttribs->SetAttribute( "type", getType() );
        whItemAttribs->SetAttribute( "x", getX() );
        whItemAttribs->SetAttribute( "y", getY() );
        whItemAttribs->SetAttribute( "w", getW() );
        whItemAttribs->SetAttribute( "h", getH() );

        whItem->InsertEndChild( whItemAttribs );

        // Add all ports
        std::for_each(whPorts.begin(), whPorts.end(),
                      [&](WarehousePort_t* whPort) -> void
                      {
                          tinyxml2::XMLElement* whPortAttribs = doc->NewElement( "WarehousePort" );
                          whPort->serializeToXml(whPortAttribs);
                          whItem->InsertEndChild( whPortAttribs );
                      });
    }

    WarehouseItem_t::WarehousePortContainer_t WarehouseItem_t::getWhPorts() const
    {
        return whPorts;
    }

    void WarehouseItem_t::dump() const
    {
        std::cout << "==============================================" << std::endl;
        std::cout << "  Warehouse item ID <" << itemID << "> Type <" << itemType << ">" << std::endl;
        std::cout << "==============================================" << std::endl;

        std::cout << "x: " << x << " y: " << y << " w: " << w << " h: " << h << std::endl;

        std::for_each(whPorts.begin(), whPorts.end(),
                      [](WarehousePort_t* p)
                      {
                          p->dump();
                      });

        std::cout << "==============================================" << std::endl;
    }
}