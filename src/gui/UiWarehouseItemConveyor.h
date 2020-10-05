/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemConveyor.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse conveyor system
 */

#pragma once

#include "UiWarehouseItem.h"
#include "UiWarehousePort.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseItemConveyor_t : public UiWarehouseItem_t
        {
            public:
                UiWarehouseItemConveyor_t(QGraphicsScene*, MainWindow*, QPoint, UiWarehouseItemType_t);
                ~UiWarehouseItemConveyor_t() = default;
        };
    }
}