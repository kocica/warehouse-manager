/**
 * Warehouse manager
 *
 * @file    UiWarehouseGeneratorThread.cpp
 * @date    11/14/2020
 * @author  Filip Kocica
 * @brief   Class providing generating of orders in UI background
 */

#include "UiWarehouseGeneratorThread.h"

#include "../WarehouseItem.h"
#include "../WarehouseOrder.h"
#include "../WarehouseLayout.h"
#include "../WarehouseOrderLine.h"
#include "../WarehouseDataGenerator.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseGeneratorThread_t::UiWarehouseGeneratorThread_t(const whm::ConfigParser_t& cfg_)
            : cfg(cfg_)
        {

        }

        void UiWarehouseGeneratorThread_t::run()
        {
            whm::utils::WhmArgs_t args;

            args.articlesPath  = cfg.getAs<std::string>("articlesPath");
            args.ordersPath    = cfg.getAs<std::string>("ordersPath");

            whm::WarehouseDataGenerator_t datagen{args, cfg};

            datagen.setUiCallback([&](int32_t val, int32_t graph)
                                  {
                                      emit newGeneratedValue(val, graph);
                                  });

            datagen.generateData();

            emit generatingFinished(args.ordersPath);
        }
    }
}
