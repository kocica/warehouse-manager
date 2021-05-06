/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemConveyor.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse conveyor system
 */

#pragma once

// Local
#include "../Logger.h"
#include "UiWarehouseItem.h"
#include "UiWarehousePort.h"

namespace whm
{
    class WarehouseItem_t;

    namespace gui
    {
        class UiWarehouseItemConveyor_t : public UiWarehouseItem_t
        {
            public:
                // Construct from GUI
                UiWarehouseItemConveyor_t(QGraphicsScene*, MainWindow*, int32_t, int32_t, int32_t, int32_t, WarehouseItemType_t);
                // Construct from TUI
                UiWarehouseItemConveyor_t(QGraphicsScene*, MainWindow*, ::whm::WarehouseItem_t&);

                virtual void updateChildrenPositions(double, double) override;

                ~UiWarehouseItemConveyor_t() = default;
        };
    }
}