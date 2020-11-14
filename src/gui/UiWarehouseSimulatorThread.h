/**
 * Warehouse manager
 *
 * @file    UiWarehouseSimulatorThread.h
 * @date    11/14/2020
 * @author  Filip Kocica
 * @brief   Class providing simulation of warehouse in UI background
 */

#pragma once

#include <QThread>
#include "../ConfigParser.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseSimulatorThread_t : public QThread
        {
            Q_OBJECT

            public:
                UiWarehouseSimulatorThread_t() = delete;
                UiWarehouseSimulatorThread_t(const whm::ConfigParser_t&);

                void run() override;

            signals:
                void simulationFinished(double);
                void orderSimulationFinished(double);

            private:
                whm::ConfigParser_t cfg;
        };
    }
}
