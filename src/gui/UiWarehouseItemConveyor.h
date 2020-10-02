/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemConveyor.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse conveyor
 */

#pragma once

#include "UiWarehousePort.h"
#include "UiWarehouseItem.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseItemConveyor_t : public UiWarehouseItem_t
        {
            Q_OBJECT

            public:
                UiWarehouseItemConveyor_t(QWidget*, MainWindow*, QPoint, UiWarehouseItemType_t);
                virtual ~UiWarehouseItemConveyor_t() = default;

            private:
                UiWarehousePort_t* whPort1{ nullptr };
                UiWarehousePort_t* whPort2{ nullptr };
        };
    }
}