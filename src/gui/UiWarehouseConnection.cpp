/**
 * Warehouse manager
 *
 * @file    UiWarehouseConnection.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse connection between ports
 */

#include "UiWarehouseConnection.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseConnection_t::UiWarehouseConnection_t(QWidget *parent, MainWindow *ui, UiWarehousePort_t *from, UiWarehousePort_t *to)
            : QWidget(parent)
            , ui(ui)
            , to(to)
            , from(from)
        {

        }
    }
}