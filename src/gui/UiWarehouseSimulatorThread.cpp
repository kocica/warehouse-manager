/**
 * Warehouse manager
 *
 * @file    UiWarehouseSimulatorThread.h
 * @date    11/14/2020
 * @author  Filip Kocica
 * @brief   Class providing simulation of warehouse in UI background
 */

#include "UiWarehouseSimulatorThread.h"

#include "../WarehouseItem.h"
#include "../WarehouseOrder.h"
#include "../WarehouseLayout.h"
#include "../WarehouseOrderLine.h"
#include "../WarehouseSimulator.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseSimulatorThread_t::UiWarehouseSimulatorThread_t(const whm::ConfigParser_t& cfg_)
            : cfg(cfg_)
        {

        }

        void UiWarehouseSimulatorThread_t::run()
        {
            whm::utils::WhmArgs_t args;

            args.locationsPath  = cfg.getAs<std::string>("locationsPath");
            args.ordersPath     = cfg.getAs<std::string>("ordersPath");

            whm::WarehouseLayout_t::getWhLayout().importLocationSlots(args.locationsPath);
            whm::WarehouseLayout_t::getWhLayout().importCustomerOrders(args.ordersPath);

            whm::WarehouseSimulator_t::getWhSimulator().setConfig(cfg);
            whm::WarehouseSimulator_t::getWhSimulator().setArguments(args);
            whm::WarehouseSimulator_t::getWhSimulator().runSimulation();
        }
    }
}
