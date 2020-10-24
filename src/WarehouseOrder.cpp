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
    template<typename T>
    WarehouseOrder_t<T>::WarehouseOrder_t()
    {

    }

    template<typename T>
    WarehouseOrder_t<T>::~WarehouseOrder_t()
    {

    }

    template<typename T>
    void WarehouseOrder_t<T>::setWhOrderID(int32_t whOrderID_)
    {
        this->whOrderID = whOrderID_;
    }

    template<typename T>
    int32_t WarehouseOrder_t<T>::getWhOrderID() const
    {
        return this->whOrderID;
    }

    template<typename T>
    const std::vector<WarehouseOrderLine_t<T>>& WarehouseOrder_t<T>::getWhOrderLines() const
    {
        return this->whOrderLines;
    }

    template<typename T>
    void WarehouseOrder_t<T>::setWhOrderLines(std::vector<WarehouseOrderLine_t<T>> newLines)
    {
        this->whOrderLines = newLines;
    }

    template<typename T>
    void WarehouseOrder_t<T>::dump() const
    {
        std::cout << "---------------------------------" << std::endl;
        std::cout << " OrderID <"   << this->getWhOrderID() << ">" << std::endl;

        std::for_each(this->whOrderLines.begin(), this->whOrderLines.end(),
                      [](const auto& whLine) -> void
                      {
                          whLine.dump();
                      });
    }

    template<typename T>
    void WarehouseOrder_t<T>::deserializeFromXml(tinyxml2::XMLElement* elem)
    {
        this->whOrderID = elem->IntAttribute("id");

        for (tinyxml2::XMLElement* whLineXml = elem->FirstChildElement("WarehouseOrderLine"); whLineXml; whLineXml = whLineXml->NextSiblingElement("WarehouseOrderLine"))
        {
            WarehouseOrderLine_t whLine(this);
            whLine.deserializeFromXml(whLineXml);
            this->whOrderLines.emplace_back(std::move(whLine));
        }
    }

    template<typename T>
    void WarehouseOrder_t<T>::serializeToXml(tinyxml2::XMLDocument* doc) const
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

    template class WarehouseOrder_t<std::string>;
}
