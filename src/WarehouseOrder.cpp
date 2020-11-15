/**
 * Warehouse manager
 *
 * @file    WarehouseOrder.cpp
 * @date    11/09/2020
 * @author  Filip Kocica
 * @brief   Class representing one customer order including order lines
 */

#include "Logger.h"
#include "WarehouseOrder.h"

namespace whm
{
    WarehouseOrder_t::WarehouseOrder_t()
    {

    }

    WarehouseOrder_t::~WarehouseOrder_t()
    {

    }

    void WarehouseOrder_t::setWhOrderID(int32_t whOrderID_)
    {
        this->whOrderID = whOrderID_;
    }

    int32_t WarehouseOrder_t::getWhOrderID() const
    {
        return this->whOrderID;
    }

    const std::vector<WarehouseOrderLine_t>& WarehouseOrder_t::getWhOrderLines() const
    {
        return this->whOrderLines;
    }

    void WarehouseOrder_t::setWhOrderLines(std::vector<WarehouseOrderLine_t> newLines)
    {
        this->whOrderLines = newLines;
    }

    void WarehouseOrder_t::dump() const
    {
        std::cout << "---------------------------------" << std::endl;
        std::cout << " OrderID <"   << this->getWhOrderID() << ">" << std::endl;

        std::for_each(this->whOrderLines.begin(), this->whOrderLines.end(),
                      [](const auto& whLine) -> void
                      {
                          whLine.dump();
                      });
    }

    void WarehouseOrder_t::deserializeFromXml(tinyxml2::XMLElement* elem)
    {
        this->whOrderID = elem->IntAttribute("id");

        for (tinyxml2::XMLElement* whLineXml = elem->FirstChildElement("WarehouseOrderLine"); whLineXml; whLineXml = whLineXml->NextSiblingElement("WarehouseOrderLine"))
        {
            WarehouseOrderLine_t whLine(this);
            whLine.deserializeFromXml(whLineXml);
            this->whOrderLines.emplace_back(std::move(whLine));
        }
    }

    void WarehouseOrder_t::serializeToXml(tinyxml2::XMLDocument* doc) const
    {
        tinyxml2::XMLElement* whOrder = doc->NewElement( "WarehouseOrder" );

        whOrder->SetAttribute( "id", getWhOrderID() );

        // Add all lines
        std::for_each(whOrderLines.begin(), whOrderLines.end(),
                      [&](auto whLine) -> void
                      {
                          tinyxml2::XMLElement* whLineXml = doc->NewElement( "WarehouseOrderLine" );
                          whLine.serializeToXml(whLineXml);
                          whOrder->InsertEndChild(whLineXml);
                      });

        doc->InsertEndChild(whOrder);
    }
}
