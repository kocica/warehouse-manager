/**
 * Warehouse manager
 *
 * @file    UiWarehouseItem.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse item (location, conv, ...)
 */

#include <iostream>

#include <QMessageBox>

#include "UiCursor.h"
#include "UiWarehouseItem.h"
#include "UiWarehousePort.h"
#include "UiWarehouseLayout.h"

namespace whm
{
    namespace gui
    {
        std::map<WarehouseItemType_t, std::vector<WarehouseItemType_t>> allowedCombinations =
        {
            { WarehouseItemType_t::E_CONVEYOR,           { WarehouseItemType_t::E_CONVEYOR, WarehouseItemType_t::E_LOCATION_SHELF, WarehouseItemType_t::E_CONVEYOR_HUB,
                                                           WarehouseItemType_t::E_WAREHOUSE_ENTRANCE, WarehouseItemType_t::E_WAREHOUSE_DISPATCH }},
            { WarehouseItemType_t::E_CONVEYOR_HUB,       { WarehouseItemType_t::E_CONVEYOR, WarehouseItemType_t::E_CONVEYOR_HUB }},
            { WarehouseItemType_t::E_LOCATION_SHELF,     { WarehouseItemType_t::E_CONVEYOR }},
            { WarehouseItemType_t::E_WAREHOUSE_ENTRANCE, { WarehouseItemType_t::E_CONVEYOR }},
            { WarehouseItemType_t::E_WAREHOUSE_DISPATCH, { WarehouseItemType_t::E_CONVEYOR }}
        };

        bool isWhItemCombinationAllowed(WarehouseItemType_t lhs, WarehouseItemType_t rhs)
        {
            std::vector<WarehouseItemType_t>& allowedTypes = allowedCombinations[lhs];

            return std::any_of(allowedTypes.begin(), allowedTypes.end(),
                               [&](WarehouseItemType_t& whItemType) -> bool
                               {
                                   return whItemType == rhs;
                               });
        }

        UiWarehouseItem_t::UiWarehouseItem_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, WarehouseItemType_t type)
            : BaseShapeGraphicItem_t(x, y, w, h, BaseShapeGraphicItem_t::ITEM_RECTANGLE, s)
            , ui(ui)
            , scene(s)
            , whItemID(UiWarehouseLayout_t::getWhLayout().getNextWhItemID())
            , whItemType(type)
        {
            
        }

        UiWarehouseItem_t::~UiWarehouseItem_t()
        {
            for (UiWarehousePort_t* whPort : whPorts)
            {
                delete whPort;
            }

            whPorts.clear();
        }

        WarehouseItemType_t UiWarehouseItem_t::getWhItemType() const
        {
            return whItemType;
        }

        UiWarehouseItem_t::UiWarehousePortContainer_t UiWarehouseItem_t::getWhPorts() const
        {
            return whPorts;
        }

        void UiWarehouseItem_t::eraseFromLayout()
        {
            UiWarehouseLayout_t::getWhLayout().eraseWhItem(this);
        }

        void UiWarehouseItem_t::mousePressEvent(QGraphicsSceneMouseEvent *event)
        {
            if (event->button() == Qt::LeftButton)
            {
                if(UiCursor_t::getCursor().getMode() == UiCursorMode_t::E_MODE_DELETE)
                {
                    eraseFromLayout();
                    scene->removeItem(this);
                    delete this;
                    return;
                }
            }
            else if (event->button() == Qt::RightButton)
            {
                /*QMenu contextMenu("Item manipulation", ui->getUi());
                contextMenu.setStyleSheet("background-color: rgb(150,150,150);");

                QAction actionDelete("Delete", ui->getUi());
                connect(&actionDelete, SIGNAL(triggered()), ui->getUi(), SLOT(deleteLater()));
                connect(&actionDelete, SIGNAL(triggered()), ui->getUi(), SLOT(eraseFromLayout()));
                contextMenu.addAction(&actionDelete);
                contextMenu.exec(mapToGlobal(event->pos()));*/

                // TODO: Resize, etc.
            }

            BaseGraphicItem_t::mousePressEvent(event);
        }

        void UiWarehouseItem_t::dump() const
        {
            std::cout << "==============================================" << std::endl;
            std::cout << "  Warehouse item ID <" << whItemID << "> Type <" << typeid(*this).name() << "> = <" << to_underlying(whItemType) << ">" << std::endl;
            std::cout << "==============================================" << std::endl;
 
            std::for_each(whPorts.begin(), whPorts.end(),
                          [](UiWarehousePort_t* p)
                          {
                              p->dump();
                          });

            std::cout << "==============================================" << std::endl;
        }
    }
}