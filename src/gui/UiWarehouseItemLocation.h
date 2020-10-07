/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemLocation.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse location
 */

#pragma once

// Local
#include "UiWarehouseItem.h"
#include "UiWarehousePort.h"

namespace whm
{
    class WarehouseItem_t;

    namespace gui
    {
        class UiWarehouseItemLocation_t : public UiWarehouseItem_t
        {
            public:
                // Construct from GUI
                UiWarehouseItemLocation_t(QGraphicsScene*, MainWindow*, int32_t, int32_t, int32_t, int32_t, UiWarehouseItemType_t);
                // Construct from TUI
                UiWarehouseItemLocation_t(QGraphicsScene*, MainWindow*, ::whm::WarehouseItem_t&);

                ~UiWarehouseItemLocation_t() = default;
        };
    }
}