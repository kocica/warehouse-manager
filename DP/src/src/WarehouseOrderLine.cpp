/**
 * Warehouse manager
 *
 * @file    WarehouseOrderLine.cpp
 * @date    11/09/2020
 * @author  Filip Kocica
 * @brief   Class representing one customer order line (product + requested quantity)
 */

#include "Logger.h"
#include "WarehouseOrder.h"
#include "WarehouseOrderLine.h"

namespace whm
{
    WarehouseOrderLine_t::WarehouseOrderLine_t(WarehouseOrder_t* whOrder_)
        : whOrder{ whOrder_ }
    {

    }

    WarehouseOrderLine_t::~WarehouseOrderLine_t()
    {

    }

    std::string WarehouseOrderLine_t::getArticle() const
    {
        return this->article;
    }

    void WarehouseOrderLine_t::setArticle(const std::string& a)
    {
        this->article = a;
    }

    int32_t WarehouseOrderLine_t::getQuantity() const
    {
        return this->quantity;
    }

    void WarehouseOrderLine_t::setQuantity(int32_t q)
    {
        this->quantity = q;
    }

    WarehouseOrder_t& WarehouseOrderLine_t::getWhOrder() const
    {
        return *this->whOrder;
    }

    void WarehouseOrderLine_t::setWhLineID(int32_t newLineID)
    {
        this->whLineID = newLineID; // Reordered line during optimizations
    }

    int32_t WarehouseOrderLine_t::getWhLineID() const
    {
        return this->whLineID;
    }

    void WarehouseOrderLine_t::dump() const
    {
        std::cout << "    LineID <" << this->getWhLineID()
                  << "> Article <"  << this->getArticle()
                  << "> Quantity <" << this->getQuantity()
                  << ">"            << std::endl;
    }

    void WarehouseOrderLine_t::deserializeFromXml(tinyxml2::XMLElement* elem)
    {
        this->whLineID = elem->IntAttribute("id");
        this->article  = elem->Attribute("article_name");
        this->quantity = elem->IntAttribute("requested_quantity");
    }

    void WarehouseOrderLine_t::serializeToXml(tinyxml2::XMLElement* elem) const
    {
        elem->SetAttribute("id", getWhLineID());
        elem->SetAttribute("article_name", getArticle().c_str());
        elem->SetAttribute("requested_quantity", getQuantity());
    }
}
