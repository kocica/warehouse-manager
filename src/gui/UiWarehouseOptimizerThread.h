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
#include "../ConfigParser.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseOptimizerThread_t : public QThread
        {
            Q_OBJECT

            public:
                UiWarehouseOptimizerThread_t() = delete;
                UiWarehouseOptimizerThread_t(const whm::ConfigParser_t&, int32_t);

                void run() override;

            signals:
                void optimizationFinished();
                void optimizationStep(double);

            private:
                int32_t opt{ 0 };
                whm::ConfigParser_t cfg;
        };
    }
}
