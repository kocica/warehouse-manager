/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemLocation.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse location
 */

#pragma once

#include "UiWarehouseItem.h"
#include "UiWarehousePort.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseItemLocation_t : public UiWarehouseItem_t
        {
            public:
                UiWarehouseItemLocation_t(QGraphicsScene*, MainWindow*, QPoint, UiWarehouseItemType_t);
                ~UiWarehouseItemLocation_t() = default;
        };
    }
}