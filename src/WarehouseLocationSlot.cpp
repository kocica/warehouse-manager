/**
 * Warehouse manager
 *
 * @file    WarehouseLocationSlot.cpp
 * @date    10/09/2020
 * @author  Filip Kocica
 * @brief   Class representing location slot containing product(s)
 */

#include <iostream>

#include "Logger.h"
#include "WarehouseItem.h"
#include "WarehouseLocationSlot.h"
#include "WarehouseLocationRack.h"

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
        return this->article;
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::setArticle(const T& a)
    {
        this->article = a;
    }

    template<typename T>
    int32_t WarehouseLocationSlot_t<T>::getQuantity() const
    {
        return this->quantity;
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::setQuantity(const int32_t q)
    {
        this->quantity = q;
    }

    template<typename T>
    std::pair<size_t, size_t> WarehouseLocationSlot_t<T>::getCoords() const
    {
        return std::make_pair(x, y);
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::setCoords(const std::pair<size_t, size_t>& coords)
    {
        this->x = coords.first;
        this->y = coords.second;
    }

    template<typename T>
    WarehouseLocationRack_t<T>* WarehouseLocationSlot_t<T>::getWhLocRack() const
    {
        return this->whLocRack;
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::setWhLocRack(WarehouseLocationRack_t<T>* whLocRack_)
    {
        this->whLocRack = whLocRack_;
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::dump() const
    {
        std::cout << this->article << " (" << this->quantity << ") ";
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::exportSlot(std::ostream& csvStream) const
    {
        csvStream << this->whLocRack->getWhItem()->getWhItemID() << ";"
                  << this->x << ";"
                  << this->y << ";"
                  << this->article << ";"
                  << this->quantity << std::endl;
    }

    template<typename T>
    void WarehouseLocationSlot_t<T>::importSlot(std::istream& csvStream)
    {
        std::string i, x, y, a, q;

        std::getline(csvStream, i, ';');
        std::getline(csvStream, x, ';');
        std::getline(csvStream, y, ';');
        std::getline(csvStream, a, ';');
        std::getline(csvStream, q);

        try
        {
            if (this->whLocRack->getWhItem()->getWhItemID() != std::stoi(i) ||
                this->x != std::stoul(x) ||
                this->y != std::stoul(y))
            {
                std::cerr << "Warehouse layout is not matching this csv!" << std::endl;
            }
            else
            {
                this->article = a;
                this->quantity = std::stoi(q);
            }
        }
        catch(std::invalid_argument& ia)
        {
            std::cerr << "Incorrect csv value!" << std::endl;
        }
    }

    template class WarehouseLocationSlot_t<std::string>;
}