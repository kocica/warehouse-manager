/**
 * Warehouse manager
 *
 * @file    UiWarehousePathFinderThread.cpp
 * @date    02/10/2021
 * @author  Filip Kocica
 * @brief   Class providing optimized path finding in UI background
 */

#include "UiWarehousePathFinderThread.h"

#include "../Utils.h"
#include "../WarehouseLayout.h"
#include "../WarehousePathFinderACO.h"

namespace whm
{
    namespace gui
    {
        UiWarehousePathFinderThread_t::UiWarehousePathFinderThread_t(const whm::ConfigParser_t& cfg_)
            : cfg(cfg_)
        {

        }

        void UiWarehousePathFinderThread_t::run()
        {
            whm::utils::WhmArgs_t args;

            args.articlesPath  = cfg.getAs<std::string>("articlesPath");
            args.locationsPath = cfg.getAs<std::string>("locationsPath");
            args.ordersPath    = cfg.getAs<std::string>("ordersPath");

            whm::WarehouseLayout_t::getWhLayout().importLocationSlots(args.locationsPath);
            whm::WarehouseLayout_t::getWhLayout().importCustomerOrders(args.ordersPath);

            whm::WarehousePathFinderACO_t pathFinder{args, cfg};

            pathFinder.setUiCallback([](int32_t cost, const std::vector<int32_t>& path)
                                     {
                                        
                                     });

            pathFinder.findPath();
        }
    }
}
