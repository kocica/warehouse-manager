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
#include "../WarehouseSimulatorSIMLIB.h"

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

            heatMap.clear();
            heatMax = std::numeric_limits<int32_t>::min();
            heatMin = std::numeric_limits<int32_t>::max();

            for(auto& order : ::whm::WarehouseLayout_t::getWhLayout().getWhOrders())
            {
                for(auto& line : order.getWhOrderLines())
                {
                    heatMap[line.getArticle()]++;
                }
            }

            for(auto& m : heatMap)
            {
                heatMax = m.second > heatMax ? m.second : heatMax;
                heatMin = m.second < heatMin ? m.second : heatMin;
            }

            whm::WarehouseSimulatorSIMLIB_t sim;

            sim.setUiCallback([&](double time, bool sim)
                              {
                                  if(sim)
                                      emit simulationFinished(time);
                                  else
                                      emit orderSimulationFinished(time);
                              });

            sim.setConfig(cfg);
            sim.setArguments(args);
            sim.optimizationModeActive() = false;
            sim.runSimulation();
        }
    }
}
