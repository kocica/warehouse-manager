/**
 * Warehouse manager
 *
 * @file    UiWarehouseGeneratorThread.h
 * @date    11/14/2020
 * @author  Filip Kocica
 * @brief   Class providing generating of orders in UI background
 */

#pragma once

#include <QThread>
#include "../ConfigParser.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseGeneratorThread_t : public QThread
        {
            Q_OBJECT

            public:
                UiWarehouseGeneratorThread_t() = delete;
                UiWarehouseGeneratorThread_t(const whm::ConfigParser_t&);

                void run() override;

            signals:
                void generatingFinished();
                void generatingStep();

            private:
                whm::ConfigParser_t cfg;
        };
    }
}
