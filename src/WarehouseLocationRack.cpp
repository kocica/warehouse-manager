/**
 * Warehouse manager
 *
 * @file    WarehouseLocationRack.h
 * @date    10/09/2020
 * @author  Filip Kocica
 * @brief   Class representing location rack with slots containing products
 */

#include <iostream>
#include <algorithm>

#include "Logger.h"
#include "WarehouseLocationRack.h"

namespace whm
{
    WarehouseLocationRack_t::WarehouseLocationRack_t(WarehouseItem_t* whItem_, size_t slotsX, size_t slotsY)
        : whItem{ whItem_ }
    {
        init(slotsX, slotsY);

        for(size_t y = 0; y < slotsY; ++y)
        {
            for(size_t x = 0; x < slotsX; ++x)
            {
                sortedSlots.push_back(&this->at(x, y));
            }
        }

        std::sort(sortedSlots.begin(), sortedSlots.end(),
                  [x = slotsX, y = slotsY](WarehouseLocationSlot_t* lhs, WarehouseLocationSlot_t* rhs)
                  -> bool
                  {
                      return (lhs->getCoords().first / double(x)) + (lhs->getCoords().second / double(y)) <
                             (rhs->getCoords().first / double(x)) + (rhs->getCoords().second / double(y)) ;
                  });
    }

    WarehouseLocationRack_t::~WarehouseLocationRack_t()
    {

    }

    WarehouseItem_t* WarehouseLocationRack_t::getWhItem() const
    {
        return this->whItem;
    }

    WarehouseLocationSlot_t& WarehouseLocationRack_t::at(size_t x, size_t y)
    {
        return whSlots[y][x];
    }

    void WarehouseLocationRack_t::setAt(size_t x, size_t y, const WarehouseLocationSlot_t& s)
    {
        whSlots[y][x] = s;
    }

    void WarehouseLocationRack_t::init(size_t x, size_t y)
    {
        whSlots = LocationSlots_t(y, std::vector<WarehouseLocationSlot_t>(x));

        for (size_t i = 0; i < whSlots.size(); i++)
        {
            for (size_t j = 0; j < whSlots[i].size(); j++)
            {
                whSlots[i][j].setWhLocRack(this);
                whSlots[i][j].setCoords(std::make_pair(j, i));
            }
        }
    }

    void WarehouseLocationRack_t::dump() const
    {
        for (size_t i = 0; i < whSlots.size(); i++)
        {
            for (size_t j = 0; j < whSlots[i].size(); j++)
            {
                whSlots[i][j].dump();
            }
            std::cout << std::endl;
        }
    }

    void WarehouseLocationRack_t::exportSlots(std::ostream& csvStream) const
    {
        for (size_t i = 0; i < whSlots.size(); i++)
        {
            for (size_t j = 0; j < whSlots[i].size(); j++)
            {
                whSlots[i][j].exportSlot(csvStream);
            }
        }
    }

    void WarehouseLocationRack_t::importSlots(std::istream& csvStream)
    {
        for (size_t i = 0; i < whSlots.size(); i++)
        {
            for (size_t j = 0; j < whSlots[i].size(); j++)
            {
                whSlots[i][j].importSlot(csvStream);
            }
        }
    }

    bool WarehouseLocationRack_t::containsArticle(const std::string& article, int32_t quantity)
    {
        std::pair<size_t, size_t> c;
        return containsArticle(article, quantity, c);
    }

    bool WarehouseLocationRack_t::containsArticle(const std::string& article, int32_t quantity, std::pair<size_t, size_t>& coords)
    {
        for (size_t i = 0; i < whSlots.size(); i++)
        {
            for (size_t j = 0; j < whSlots[i].size(); j++)
            {
                if(whSlots[i][j].getArticle() == article && whSlots[i][j].getQuantity() >= quantity)
                {
                    whSlots[i][j] -= quantity;
                    coords = std::make_pair(j, i);
                    return true;
                }
            }
        }

        return false;
    }

    void WarehouseLocationRack_t::replenishArticle(const std::string& article, int32_t quantity, std::pair<size_t, size_t>& coords)
    {
        for (size_t i = 0; i < whSlots.size(); i++)
        {
            for (size_t j = 0; j < whSlots[i].size(); j++)
            {
                if(whSlots[i][j].getArticle() == article)
                {
                    whSlots[i][j] += quantity;
                    coords = std::make_pair(j, i);
                }
            }
        }
    }

    void WarehouseLocationRack_t::resetRack()
    {
        for (size_t i = 0; i < whSlots.size(); i++)
        {
            for (size_t j = 0; j < whSlots[i].size(); j++)
            {
                whSlots[i][j].resetSlot();
            }
        }
    }

    int32_t WarehouseLocationRack_t::getSlotCountY() const
    {
        return static_cast<int32_t>(whSlots.size());
    }

    int32_t WarehouseLocationRack_t::getSlotCountX() const
    {
        return static_cast<int32_t>(whSlots.size() > 0 ? whSlots[0].size() : 0);
    }

    int32_t WarehouseLocationRack_t::getOccupationLevel() const
    {
        int32_t occLevel{ 0 };

        for (size_t i = 0; i < whSlots.size(); i++)
        {
            for (size_t j = 0; j < whSlots[i].size(); j++)
            {
                if(whSlots[i][j].isOccupied())
                {
                    occLevel++;
                }
            }
        }

        return occLevel;
    }

    WarehouseLocationSlot_t* WarehouseLocationRack_t::getFirstFreeSlot()
    {
        for (auto* slot : sortedSlots)
        {
            if(!slot->isOccupied())
            {
                return slot;
            }
        }

        return nullptr;
    }

    const typename WarehouseLocationRack_t::LocationSlots_t& WarehouseLocationRack_t::getSlots() const
    {
        return whSlots;
    }

    const typename WarehouseLocationRack_t::SortedLocationSlots_t& WarehouseLocationRack_t::getSortedSlots() const
    {
        return sortedSlots;
    }
}