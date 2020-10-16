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
#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehousePort.h"
#include "WarehouseLocationRack.h"

#ifdef WHM_GUI
#include "gui/UiWarehouseItem.h"
#include "gui/UiWarehouseItemLocation.h"
#endif

namespace whm
{
    WarehouseItem_t::WarehouseItem_t()
    {
        
    }

#ifdef WHM_GUI
    WarehouseItem_t::WarehouseItem_t(gui::UiWarehouseItem_t& uiItem)
    {
        int32_t o = uiItem.getO();
        uiItem.setO(0); // We need to set item to start position (no rotation) to get coorect coords
        setX(uiItem.getX());
        setY(uiItem.getY());
        setW(uiItem.getW());
        setH(uiItem.getH());
        uiItem.setO(o);
        setO(uiItem.getO());

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

        if (whItemType == WarehouseItemType_t::E_LOCATION_SHELF)
        {
            const auto& uiItemLoc = static_cast<gui::UiWarehouseItemLocation_t&>(uiItem);
            whLocRack = new WarehouseLocationRack_t<std::string>(this, uiItemLoc.getSlotCountX(), uiItemLoc.getSlotCountY());
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
        tinyxml2::XMLElement* whLocationRack = doc->NewElement( "LocationRackDimensions" );

        // Set attributes
        whItemAttribs->SetAttribute( "id", getWhItemID() );
        whItemAttribs->SetAttribute( "type", to_underlying(getType()) );
        whItemAttribs->SetAttribute( "x", getX() );
        whItemAttribs->SetAttribute( "y", getY() );
        whItemAttribs->SetAttribute( "w", getW() );
        whItemAttribs->SetAttribute( "h", getH() );
        whItemAttribs->SetAttribute( "o", getO() );
        whItem->InsertEndChild( whItemAttribs );

        // Add location rack dimensions
        if (whItemType == WarehouseItemType_t::E_LOCATION_SHELF)
        {
            whLocationRack->SetAttribute( "slots_x", whLocRack->getSlotCountX() );
            whLocationRack->SetAttribute( "slots_y", whLocRack->getSlotCountY() );
            whItem->InsertEndChild( whLocationRack );
        }

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
        this->setO(attribs->IntAttribute("o"));

        if (whItemType == WarehouseItemType_t::E_LOCATION_SHELF)
        {
            tinyxml2::XMLElement* locationRack = elem->FirstChildElement( "LocationRackDimensions" );

            int32_t slotsX = locationRack->IntAttribute("slots_x");
            int32_t slotsY = locationRack->IntAttribute("slots_y");

            whLocRack = new WarehouseLocationRack_t<std::string>(this, slotsX, slotsY);
        }

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

    WarehouseLocationRack_t<std::string>* WarehouseItem_t::getWhLocationRack() const
    {
        return whLocRack;
    }

    void WarehouseItem_t::dump() const
    {
        std::cout << "==============================================" << std::endl;
        std::cout << "  Warehouse item ID <" << whItemID << "> Type <" << to_underlying(whItemType) << ">" << std::endl;
        std::cout << "==============================================" << std::endl;

        std::cout << "x: " << x << " y: " << y << " w: " << w << " h: " << h << " o: " << o << std::endl;

        std::for_each(whPorts.begin(), whPorts.end(),
                      [](WarehousePort_t* p)
                      {
                          p->dump();
                      });

        if (whItemType == WarehouseItemType_t::E_LOCATION_SHELF)
        {
            std::cout << std::endl << "--- Location Rack ---" << std::endl << std::endl;
            whLocRack->dump();
        }
    }

    int32_t WarehouseItem_t::getSlotCountX() const
    {
        return whItemType == WarehouseItemType_t::E_LOCATION_SHELF ? whLocRack->getSlotCountX() : 0;
    }

    int32_t WarehouseItem_t::getSlotCountY() const
    {
        return whItemType == WarehouseItemType_t::E_LOCATION_SHELF ? whLocRack->getSlotCountY() : 0;
    }
}