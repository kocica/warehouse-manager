/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemLocation.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse location
 */

#pragma once

#include "UiWarehousePort.h"
#include "UiWarehouseItem.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseItemLocation_t : public UiWarehouseItem_t
        {
            Q_OBJECT

            public:
                UiWarehouseItemLocation_t(QWidget*, MainWindow*, QPoint, UiWarehouseItemType_t);
                virtual ~UiWarehouseItemLocation_t() = default;

            private:
                UiWarehousePort_t* whPort{ nullptr };
        };
    }
}