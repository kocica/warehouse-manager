/**
 * Warehouse manager
 *
 * @file    UiWarehouseConnection.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse connection between ports
 */

#pragma once

#include "mainwindow.h"

namespace whm
{
    namespace gui
    {
        class UiWarehousePort_t;

        class UiWarehouseConnection_t
        {
            public:
                UiWarehouseConnection_t(UiWarehousePort_t *from, UiWarehousePort_t *to);
                ~UiWarehouseConnection_t();

                void dump() const;

            private:
                UiWarehousePort_t *to{ nullptr };
                UiWarehousePort_t *from{ nullptr };
        };
    }
}