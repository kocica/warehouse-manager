/**
 * Warehouse manager
 *
 * @file    UiWarehousePathFinderThread.h
 * @date    02/10/2021
 * @author  Filip Kocica
 * @brief   Class providing optimized path finding in UI background
 */

#pragma once

#include <QThread>

#include "../ConfigParser.h"

namespace whm
{
    namespace gui
    {
        class UiWarehousePathFinderThread_t : public QThread
        {
            Q_OBJECT

            public:
                UiWarehousePathFinderThread_t() = delete;
                UiWarehousePathFinderThread_t(const whm::ConfigParser_t&);

                void run() override;

            signals:
                void pathFindingFinished();
                void pathFindingStep(int, const std::vector<int>&);

            private:
                whm::ConfigParser_t cfg;
        };
    }
}
