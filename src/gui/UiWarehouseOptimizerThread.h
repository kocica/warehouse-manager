/**
 * Warehouse manager
 *
 * @file    UiWarehouseOptimizerThread.h
 * @date    11/11/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse in UI background
 */

#pragma once

#include <QThread>

namespace whm
{
    namespace gui
    {
        class UiWarehouseOptimizerThread_t : public QThread
        {
            public:
                void run() override;
        };
    }
}
