/**
 * Warehouse manager
 *
 * @file    WarehouseOrderLine.cpp
 * @date    11/09/2020
 * @author  Filip Kocica
 * @brief   Class representing one customer order line (product + requested quantity)
 */

#include "WarehouseOrder.h"
#include "WarehouseOrderLine.h"

namespace whm
{
    template<typename T>
    WarehouseOrderLine_t<T>::WarehouseOrderLine_t(const WarehouseOrder_t<T>& whOrder_)
        : whOrder{ whOrder_ }
    {

    }

    template<typename T>
    WarehouseOrderLine_t<T>::~WarehouseOrderLine_t()
    {

    }

    template<typename T>
    T WarehouseOrderLine_t<T>::getArticle() const
    {
        return this->article;
    }

    template<typename T>
    void WarehouseOrderLine_t<T>::setArticle(const T& a)
    {
        this->article = a;
    }

    template<typename T>
    int32_t WarehouseOrderLine_t<T>::getQuantity() const
    {
        return this->quantity;
    }

    template<typename T>
    void WarehouseOrderLine_t<T>::setQuantity(int32_t q)
    {
        this->quantity = q;
    }

    template<typename T>
    WarehouseOrder_t<T> WarehouseOrderLine_t<T>::getWhOrder() const
    {
        return this->whOrder;
    }

    template<typename T>
    void WarehouseOrderLine_t<T>::setWhLineID(int32_t newLineID)
    {
        this->whLineID = newLineID; // Reordered line during optimizations
    }

    template<typename T>
    int32_t WarehouseOrderLine_t<T>::getWhLineID() const
    {
        return this->whLineID;
    }

    template<typename T>
    void WarehouseOrderLine_t<T>::dump() const
    {
        std::cout << "    LineID <" << this->getWhLineID()
                  << "> Article <"  << this->getArticle()
                  << "> Quantity <" << this->getQuantity()
                  << ">"            << std::endl;
    }

    template<typename T>
    void WarehouseOrderLine_t<T>::deserializeFromXml(tinyxml2::XMLElement* elem)
    {
        this->whLineID = elem->IntAttribute("id");
        this->article  = elem->Attribute("article_name");
        this->quantity = elem->IntAttribute("requested_quantity");
    }

    template class WarehouseOrderLine_t<std::string>;
}
