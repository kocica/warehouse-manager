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
    WarehouseLocationSlot_t::WarehouseLocationSlot_t()
    {

    }

    WarehouseLocationSlot_t::~WarehouseLocationSlot_t()
    {

    }

    bool WarehouseLocationSlot_t::isOccupied() const
    {
        return this->article != std::string();
    }

    std::string WarehouseLocationSlot_t::getArticle() const
    {
        return this->article;
    }

    void WarehouseLocationSlot_t::setArticle(const std::string& a)
    {
        this->article = a;
    }

    int32_t WarehouseLocationSlot_t::getQuantity() const
    {
        return this->quantity;
    }

    void WarehouseLocationSlot_t::setQuantity(const int32_t q)
    {
        this->quantity = q;
    }

    std::pair<size_t, size_t> WarehouseLocationSlot_t::getCoords() const
    {
        return std::make_pair(x, y);
    }

    void WarehouseLocationSlot_t::setCoords(const std::pair<size_t, size_t>& coords)
    {
        this->x = coords.first;
        this->y = coords.second;
    }

    WarehouseLocationRack_t* WarehouseLocationSlot_t::getWhLocRack() const
    {
        return this->whLocRack;
    }

    void WarehouseLocationSlot_t::setWhLocRack(WarehouseLocationRack_t* whLocRack_)
    {
        this->whLocRack = whLocRack_;
    }

    void WarehouseLocationSlot_t::dump() const
    {
        std::cout << this->article << " (" << this->quantity << ") ";
    }

    void WarehouseLocationSlot_t::exportSlot(std::ostream& csvStream) const
    {
        csvStream << this->whLocRack->getWhItem()->getWhItemID() << ";"
                  << this->x << ";"
                  << this->y << ";"
                  << this->article << ";"
                  << this->quantity << std::endl;
    }

    void WarehouseLocationSlot_t::importSlot(std::istream& csvStream)
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
}