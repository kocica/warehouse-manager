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
#include "WarehouseLocationRack.h"

#ifdef WHM_GUI
#include "gui/UiWarehouseItem.h"
#endif

namespace whm
{
    WarehouseItem_t::WarehouseItem_t()
    {
        
    }

#ifdef WHM_GUI
    WarehouseItem_t::WarehouseItem_t(gui::UiWarehouseItem_t& uiItem)
    {
        setX(uiItem.getX());
        setY(uiItem.getY());
        setW(uiItem.getW());
        setH(uiItem.getH());
        setWhItemID(uiItem.getWhItemID());
        setType(uiItem.getWhItemType());

        auto uiPorts = uiItem.getWhPorts();
        std::for_each(uiPorts.begin(), uiPorts.end(),
                      [&](auto uiPort) -> void
                      {
                          auto newWhPort = new WarehousePort_t(*uiPort);
                          newWhPort->setWhItem(this);
                          whPorts.emplace_back(newWhPort);
                      });

        if (itemType == WarehouseItemType_t::E_LOCATION_SHELF)
        {
            whLocRack = new WarehouseLocationRack_t<std::string>(this, 2, 5); // TODO: User input location slots
        }
    }
#endif

    WarehouseItem_t::~WarehouseItem_t()
    {
        for (WarehousePort_t* whPort : whPorts)
        {
            delete whPort;
        }

        whPorts.clear();

        if (whLocRack)
        {
            delete whLocRack;
        }
    }

    void WarehouseItem_t::serializeToXml(tinyxml2::XMLDocument* doc) const
    {
        tinyxml2::XMLNode* whItem = doc->InsertEndChild( doc->NewElement( "WarehouseItem" ) );
        tinyxml2::XMLElement* whItemAttribs = doc->NewElement( "Attributes" );

        // Set attributes
        whItemAttribs->SetAttribute( "id", getWhItemID() );
        whItemAttribs->SetAttribute( "type", to_underlying(getType()) );
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

    void WarehouseItem_t::deserializeFromXml(tinyxml2::XMLElement* elem)
    {
        tinyxml2::XMLElement* attribs = elem->FirstChildElement( "Attributes" );

        this->setWhItemID(attribs->IntAttribute("id"));

        WarehouseItemType_t whItemType = static_cast<WarehouseItemType_t>(attribs->IntAttribute("type"));
        this->setType(whItemType);

        this->setX(attribs->IntAttribute("x"));
        this->setY(attribs->IntAttribute("y"));
        this->setW(attribs->IntAttribute("w"));
        this->setH(attribs->IntAttribute("h"));

        for (tinyxml2::XMLElement* whPortXml = elem->FirstChildElement("WarehousePort"); whPortXml; whPortXml = whPortXml->NextSiblingElement("WarehousePort"))
        {
            auto newWhPort = new WarehousePort_t();
            newWhPort->deserializeFromXml(whPortXml);
            newWhPort->setWhItem(this);
            whPorts.emplace_back(newWhPort);
        }

        if (itemType == WarehouseItemType_t::E_LOCATION_SHELF)
        {
            whLocRack = new WarehouseLocationRack_t<std::string>(this, 2, 5); // TODO: User input location slots
        }
    }

    WarehouseItem_t::WarehousePortContainer_t WarehouseItem_t::getWhPorts() const
    {
        return whPorts;
    }

    WarehouseLocationRack_t<std::string>* WarehouseItem_t::getWhLocationRack() const
    {
        return whLocRack;
    }

    void WarehouseItem_t::dump() const
    {
        std::cout << "==============================================" << std::endl;
        std::cout << "  Warehouse item ID <" << itemID << "> Type <" << to_underlying(itemType) << ">" << std::endl;
        std::cout << "==============================================" << std::endl;

        std::cout << "x: " << x << " y: " << y << " w: " << w << " h: " << h << std::endl;

        std::for_each(whPorts.begin(), whPorts.end(),
                      [](WarehousePort_t* p)
                      {
                          p->dump();
                      });

        if (itemType == WarehouseItemType_t::E_LOCATION_SHELF)
        {
            std::cout << std::endl << "Location Rack:" << std::endl << std::endl;
            whLocRack->dump();
        }

        std::cout << "==============================================" << std::endl;
    }
}