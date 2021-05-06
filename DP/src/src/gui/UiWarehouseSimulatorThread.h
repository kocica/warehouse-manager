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
#include "../WarehouseSimulatorSIMLIB.h"

namespace whm
{
    namespace gui
    {
        using SimulationStats_t = whm::WarehouseSimulatorSIMLIB_t::SimulationStats_t;

        extern int32_t heatMax;
        extern int32_t heatMin;
        extern std::map<std::string, int32_t> heatMap;

        class UiWarehouseSimulatorThread_t : public QThread
        {
            Q_OBJECT

            public:
                UiWarehouseSimulatorThread_t() = delete;
                UiWarehouseSimulatorThread_t(const whm::ConfigParser_t&);

                void run() override;

            signals:
                void simulationFinished(SimulationStats_t);
                void orderSimulationFinished(SimulationStats_t);

            private:
                whm::ConfigParser_t cfg;
        };
    }
}
