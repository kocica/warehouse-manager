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

        void UiWarehouseLayout_t::dump()
        {
            std::for_each(whItems.begin(), whItems.end(),
                          [](UiWarehouseItem_t* whItem) -> void
                          {
                              std::cout << whItem->getWhItemID() << std::endl;
                              switch(whItem->getWhItemType())
                              {
                                  case UiWarehouseItemType_t::E_LOCATION_SHELF:
                                  {
                                      std::cout << "Location shelf" << std::endl;
                                      break;
                                  }
                                  case UiWarehouseItemType_t::E_CONVEYOR:
                                  {
                                      std::cout << "Conveyor" << std::endl;
                                      break;
                                  }
                              }
                          });
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
