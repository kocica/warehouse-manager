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
#include "UiWarehouseItemGate.h"
#include "UiWarehouseItemLocation.h"
#include "UiWarehouseItemConveyor.h"
#include "../WarehouseItem.h"
#include "../WarehouseLayout.h"
#include "../WarehouseConnection.h"

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

        UiWarehouseLayout_t::~UiWarehouseLayout_t()
        {
            this->clearWhLayout();
        }

        void UiWarehouseLayout_t::initFromTui(QGraphicsScene* s, MainWindow* ui, ::whm::WarehouseLayout_t& layout)
        {
            this->clearWhLayout();

            auto items = layout.getWhItems();

            this->whRatio = layout.getRatio(); 
            this->whDims = layout.getDimensions();

            for (auto* item : items)
            {
                UiWarehouseItem_t* newItem{ nullptr };
                auto tmpIdSequence{ whItemIdSequence };

                whItemIdSequence = item->getWhItemID();

                if (item->getType() == ::whm::WarehouseItemType_t::E_LOCATION_SHELF)
                {
                    newItem = new UiWarehouseItemLocation_t{ s, ui, *item };
                }
                else if (item->getType() == ::whm::WarehouseItemType_t::E_CONVEYOR ||
                         item->getType() == ::whm::WarehouseItemType_t::E_CONVEYOR_HUB)
                {
                    newItem = new UiWarehouseItemConveyor_t{ s, ui, *item };
                }
                else if (item->getType() == ::whm::WarehouseItemType_t::E_WAREHOUSE_ENTRANCE ||
                         item->getType() == ::whm::WarehouseItemType_t::E_WAREHOUSE_DISPATCH ||
                         item->getType() == ::whm::WarehouseItemType_t::E_WAREHOUSE_BUFFER)
                {
                    newItem = new UiWarehouseItemGate_t{ s, ui, *item };
                }
                else
                {
                    std::cerr << "Unknown warehouse item type!" << std::endl;
                }

                whItemIdSequence = std::max(tmpIdSequence, item->getWhItemID() + 1);

                whItems.emplace_back(newItem);
            }

            auto conns = layout.getWhConns();

            for (auto* conn : conns)
            {
                auto tmpIdSequence{ whConnIdSequence };

                whConnIdSequence = conn->getWhConnID();

                UiWarehouseConnection_t* newConn = new UiWarehouseConnection_t{ *conn };

                whConnIdSequence = std::max(tmpIdSequence, conn->getWhConnID() + 1);

                whConns.emplace_back(newConn);
            }
        }

        UiWarehouseLayout_t& UiWarehouseLayout_t::getWhLayout()
        {
            static UiWarehouseLayout_t l;
            return l;
        }

        UiWarehouseLayout_t::UiWarehouseItemContainer_t UiWarehouseLayout_t::getWhItems() const
        {
            return whItems;
        }

        UiWarehouseLayout_t::UiWarehouseConnContainer_t UiWarehouseLayout_t::getWhConns() const
        {
            return whConns;
        }

        int32_t UiWarehouseLayout_t::getNextWhItemID()
        {
            return whItemIdSequence++;
        }

        int32_t UiWarehouseLayout_t::getNextWhConnID()
        {
            return whConnIdSequence++;
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

        void UiWarehouseLayout_t::addWhConn(UiWarehouseConnection_t* c)
        {
            whConns.push_back(c);
        }

        void UiWarehouseLayout_t::eraseWhConn(UiWarehouseConnection_t* c)
        {
            auto found = std::find_if(whConns.begin(), whConns.end(),
                         [&](UiWarehouseConnection_t* whConn) -> bool
                         {
                            return whConn == c;
                         });

            if (found != whConns.end())
            {
                whConns.erase(found);
            }
        }

        void UiWarehouseLayout_t::clearWhLayout()
        {
            // TODO: Clear also selectedPort variable

            for (UiWarehouseItem_t* whItem : whItems)
            {
                delete whItem;
            }

            for (UiWarehouseConnection_t* whConn : whConns)
            {
                delete whConn;
            }

            whItems.clear();
            whConns.clear();

            whItemIdSequence = 0;
            whConnIdSequence = 0;
        }

        void UiWarehouseLayout_t::dump() const
        {
            std::cout << "*******************************************************" << std::endl;
            std::cout << " [UI] Dump warehouse layout <" << whDims.first << "> x <" << whDims.second
                      << "> ratio <" << whRatio << ">" << std::endl << std::endl;

            std::for_each(whItems.begin(), whItems.end(),
                          [](UiWarehouseItem_t* whItem) -> void
                          {
                              whItem->dump();
                          });

            std::cout << std::endl << std::endl;

            std::for_each(whConns.begin(), whConns.end(),
                          [](UiWarehouseConnection_t* whConn) -> void
                          {
                              whConn->dump();
                          });
        }

        bool UiWarehouseLayout_t::itemsIntersects(UiWarehouseItem_t* i) const
        {
            return std::any_of(whItems.begin(), whItems.end(),
                               [&](UiWarehouseItem_t *whItem) -> bool
                               {
                                   return whItem != i && whItem->getRect().intersects(i->getRect());
                               });
        }

        int32_t UiWarehouseLayout_t::getRatio() const
        {
            return this->whRatio;
        }

        UiWarehouseLayout_t::WarehouseDimensions_t UiWarehouseLayout_t::getDimensions() const
        {
            return this->whDims;
        }

        void UiWarehouseLayout_t::setRatio(int32_t r)
        {
            this->whRatio = r;
        }

        void UiWarehouseLayout_t::setDimensions(const UiWarehouseLayout_t::WarehouseDimensions_t& d)
        {
            this->whDims = d;
        }

        UiWarehouseItem_t* UiWarehouseLayout_t::findWhItemByID(int32_t whItemID) const
        {
            auto it = std::find_if(whItems.begin(), whItems.end(),
                                   [=](UiWarehouseItem_t* whItem)
                                   -> bool
                                   {
                                       return whItem->getWhItemID() == whItemID;
                                   });

            return it == whItems.end() ? nullptr : *it;
        }
    }
}