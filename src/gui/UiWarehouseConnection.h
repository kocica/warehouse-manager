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

        class UiWarehouseConnection_t : QWidget
        {
            Q_OBJECT

            public:
                UiWarehouseConnection_t(QWidget *parent, MainWindow *ui, UiWarehousePort_t *from, UiWarehousePort_t *to);
                virtual ~UiWarehouseConnection_t();

                void dump() const;

            private:
                MainWindow *ui{ nullptr };
                UiWarehousePort_t *to{ nullptr };
                UiWarehousePort_t *from{ nullptr };
        };
    }
}