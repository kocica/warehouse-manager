/**
 * Warehouse manager
 *
 * @file    WarehouseLocationSlot.cpp
 * @date    10/09/2020
 * @author  Filip Kocica
 * @brief   Class representing location slot containing product(s)
 */

#include <iostream>
#include "WarehouseLocationSlot.h"

namespace whm
{
    template<typename T>
    WarehouseLocationSlot_t<T>::WarehouseLocationSlot_t()
    {

    }

    template<typename T>
    WarehouseLocationSlot_t<T>::~WarehouseLocationSlot_t()
    {

    }

    template<typename T>
    T WarehouseLocationSlot_t<T>::getArticle() const
    {
        return article;
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::setArticle(const T& a)
    {
        article = a;
    }

    template<typename T>
    size_t WarehouseLocationSlot_t<T>::getQuantity() const
    {
        return quantity;
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::setQuantity(const size_t q)
    {
        quantity = q;
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::dump() const
    {
        std::cout << article << " (" << quantity << ") | ";
    }

    template class WarehouseLocationSlot_t<std::string>;
}