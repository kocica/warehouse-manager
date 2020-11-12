/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemGate.h
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse entrance/dispatch gate
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
        class UiWarehouseItemGate_t : public UiWarehouseItem_t
        {
            public:
                // Construct from GUI
                UiWarehouseItemGate_t(QGraphicsScene*, MainWindow*, int32_t, int32_t, int32_t, int32_t, WarehouseItemType_t);
                // Construct from TUI
                UiWarehouseItemGate_t(QGraphicsScene*, MainWindow*, ::whm::WarehouseItem_t&);

                virtual void updateChildrenPositions(double, double) override;

                ~UiWarehouseItemGate_t() = default;
        };
    }
}