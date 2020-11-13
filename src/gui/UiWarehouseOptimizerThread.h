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
            Q_OBJECT

            public:
                UiWarehouseOptimizerThread_t() = delete;
                UiWarehouseOptimizerThread_t(const std::string&, const std::string&, const std::string&);

                void run() override;

            signals:
                void optimizationFinished();
                void optimizationStep(double);

            private:
                std::string o, a, l;
        };
    }
}
