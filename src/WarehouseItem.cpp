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
        setID(uiItem.getID());
        setType(uiItem.getWhItemType());

        auto uiPorts = uiItem.getWhPorts();
        std::for_each(uiPorts.begin(), uiPorts.end(),
                      [&](auto uiPort) -> void
                      {
                          auto newWhPort = new WarehousePort_t(*uiPort);
                          newWhPort->setWhItem(this);
                          whPorts.emplace_back(newWhPort);
                      });
    }
#endif

    WarehouseItem_t::~WarehouseItem_t()
    {
        for (WarehousePort_t* whPort : whPorts)
        {
            delete whPort;
        }

        whPorts.clear();
    }

    void WarehouseItem_t::serializeToXml(tinyxml2::XMLDocument* doc) const
    {
        tinyxml2::XMLNode* whItem = doc->InsertEndChild( doc->NewElement( "WarehouseItem" ) );
        tinyxml2::XMLElement* whItemAttribs = doc->NewElement( "Attributes" );

        // Set attributes
        whItemAttribs->SetAttribute( "id", getID() );
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

        this->setID(attribs->IntAttribute("id"));
        //this->setType(attribs->IntAttribute("type"));

        int32_t typeID = attribs->IntAttribute("type");
        if(typeID == 0)
        {
            this->setType(WarehouseItemType_t::E_LOCATION_SHELF);
        }
        if(typeID == 1)
        {
            this->setType(WarehouseItemType_t::E_CONVEYOR);
        }
        if(typeID == 2)
        {
            this->setType(WarehouseItemType_t::E_CONVEYOR_HUB);
        }
        if(typeID == 3)
        {
            this->setType(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE);
        }
        if(typeID == 4)
        {
            this->setType(WarehouseItemType_t::E_WAREHOUSE_EXIT);
        }

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
    }

    WarehouseItem_t::WarehousePortContainer_t WarehouseItem_t::getWhPorts() const
    {
        return whPorts;
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

        std::cout << "==============================================" << std::endl;
    }
}