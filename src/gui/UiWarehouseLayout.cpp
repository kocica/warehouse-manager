/**
 * Warehouse manager
 *
 * @file    UiWarehouseLayout.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse layout
 */

// Local
#include "UiWarehouseLayout.h"
#include "UiWarehouseItemLocation.h"
#include "UiWarehouseItemConveyor.h"

// Std
#include <vector>
#include <iostream>
#include <type_traits>

namespace whm
{
    namespace gui
    {
        UiWarehouseLayout_t::UiWarehouseLayout_t()
        {

        }

        UiWarehouseLayout_t& UiWarehouseLayout_t::getWhLayout()
        {
            static UiWarehouseLayout_t l;
            return l;
        }

        size_t UiWarehouseLayout_t::getWhItemCount() const
        {
            return whItems.size();
        }

        int32_t UiWarehouseLayout_t::getNextWhItemID()
        {
            return whItemIdSequence++;
        }

        void UiWarehouseLayout_t::addWhItem(UiWarehouseItem_t* i)
        {
            whItems.push_back(i);
        }

        void UiWarehouseLayout_t::eraseWhItem(UiWarehouseItem_t* i)
        {
            auto found = std::find_if(whItems.begin(), whItems.end(),
                         [&](UiWarehouseItem_t* whItem) -> bool
                         {
                            return whItem->getWhItemID() == i->getWhItemID();
                         });

            if (found != whItems.end())
            {
                whItems.erase(found);
            }
        }

        void UiWarehouseLayout_t::deleteAllWhItems()
        {
            for (UiWarehouseItem_t* whItem : whItems)
            {
                delete whItem;
            }

            whItems.clear();

            whItemIdSequence = 0;
        }

        void UiWarehouseLayout_t::dump() const
        {
            std::cout << std::endl << "Dump warehouse layout - start" << std::endl << std::endl;

            std::for_each(whItems.begin(), whItems.end(),
                          [](UiWarehouseItem_t* whItem) -> void
                          {
                              whItem->dump();
                          });

            std::cout << std::endl << "Dump warehouse layout - end" << std::endl << std::endl << std::endl;
        }

        bool UiWarehouseLayout_t::itemsIntersects(UiWarehouseItem_t* i) const
        {
            return std::any_of(whItems.begin(), whItems.end(),
                               [&](UiWarehouseItem_t *whItem) -> bool
                               {
                                   return whItem != i && whItem->geometry().intersects(i->geometry());
                               });
        }
    }
}
