/**
 * Warehouse manager
 *
 * @file    WarehouseLocationRack.h
 * @date    10/09/2020
 * @author  Filip Kocica
 * @brief   Class representing location rack with slots containing products
 */

#include <iostream>

#include "Logger.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    template<typename T>
    WarehouseLocationRack_t<T>::WarehouseLocationRack_t(WarehouseItem_t* whItem_, size_t slotsX, size_t slotsY)
        : whItem{ whItem_ }
    {
        init(slotsX, slotsY);
    }

    template<typename T>
    WarehouseLocationRack_t<T>::~WarehouseLocationRack_t()
    {

    }

    template<typename T>
    WarehouseItem_t* WarehouseLocationRack_t<T>::getWhItem() const
    {
        return this->whItem;
    }

    template<typename T>
    WarehouseLocationSlot_t<T> WarehouseLocationRack_t<T>::at(size_t x, size_t y) const
    {
        // TODO: assert; exception
        if (y > slots.size() || x > slots[0].size())
        {
            std::cerr << "Index is outside of bounds!" << std::endl;
        }

        return slots[y][x];
    }

    template<typename T>
    void WarehouseLocationRack_t<T>::setAt(size_t x, size_t y, const WarehouseLocationSlot_t<T>& s)
    {
        // TODO: assert; exception
        if (y > slots.size() || x > slots[0].size())
        {
            std::cerr << "Index is outside of bounds!" << std::endl;
        }

        slots[y][x] = s;
    }

    template<typename T>
    void WarehouseLocationRack_t<T>::init(size_t x, size_t y)
    {
        slots = LocationSlots_t(y, std::vector<WarehouseLocationSlot_t<T>>(x));

        for (size_t i = 0; i < slots.size(); i++)
        {
            for (size_t j = 0; j < slots[i].size(); j++)
            {
                slots[i][j].setWhLocRack(this);
                slots[i][j].setCoords(std::make_pair(j, i));
            }
        }
    }

    template<typename T>
    void WarehouseLocationRack_t<T>::dump() const
    {
        for (size_t i = 0; i < slots.size(); i++)
        {
            for (size_t j = 0; j < slots[i].size(); j++)
            {
                slots[i][j].dump();
            }
            std::cout << std::endl;
        }
    }

    template<typename T>
    void WarehouseLocationRack_t<T>::exportSlots(std::ostream& csvStream) const
    {
        for (size_t i = 0; i < slots.size(); i++)
        {
            for (size_t j = 0; j < slots[i].size(); j++)
            {
                slots[i][j].exportSlot(csvStream);
            }
        }
    }

    template<typename T>
    void WarehouseLocationRack_t<T>::importSlots(std::istream& csvStream)
    {
        for (size_t i = 0; i < slots.size(); i++)
        {
            for (size_t j = 0; j < slots[i].size(); j++)
            {
                slots[i][j].importSlot(csvStream);
            }
        }
    }

    template<typename T>
    bool WarehouseLocationRack_t<T>::containsArticle(const T& article, int32_t quantity)
    {
        std::pair<size_t, size_t> c;
        return containsArticle(article, quantity, c);
    }

    template<typename T>
    bool WarehouseLocationRack_t<T>::containsArticle(const T& article, int32_t quantity, std::pair<size_t, size_t>& coords)
    {
        (void)quantity;

        for (size_t i = 0; i < slots.size(); i++)
        {
            for (size_t j = 0; j < slots[i].size(); j++)
            {
                if(slots[i][j].getArticle() == article && slots[i][j].getQuantity() >= 0 /*quantity*/)
                {
                    coords = std::make_pair(j, i);
                    return true;
                }
            }
        }

        return false;
    }

    template<typename T>
    int32_t WarehouseLocationRack_t<T>::getSlotCountY() const
    {
        return static_cast<int32_t>(slots.size());
    }

    template<typename T>
    int32_t WarehouseLocationRack_t<T>::getSlotCountX() const
    {
        return static_cast<int32_t>(slots.size() > 0 ? slots[0].size() : 0);
    }

    template class WarehouseLocationRack_t<std::string>;
}