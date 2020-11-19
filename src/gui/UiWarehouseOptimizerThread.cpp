/**
 * Warehouse manager
 *
 * @file    UiWarehouseOptimizerThread.cpp
 * @date    11/11/2020
 * @author  Filip Kocica
 * @brief   Class providing optimization of warehouse in UI background
 */

#include "UiWarehouseLayout.h"
#include "UiWarehouseItemLocation.h"
#include "UiWarehouseOptimizerThread.h"

#include "../WarehouseItem.h"
#include "../WarehouseOrder.h"
#include "../WarehouseLayout.h"
#include "../WarehouseOrderLine.h"
#include "../WarehouseOptimizerGA.h"
#include "../WarehouseOptimizerDE.h"
#include "../WarehouseOptimizerABC.h"
#include "../WarehouseOptimizerPSO.h"
#include "../WarehouseOptimizerSLAP.h"
#include "../WarehouseOptimizerRAND.h"

namespace whm
{
    namespace gui
    {
        int32_t heatMax;
        int32_t heatMin;
        std::map<std::string, int32_t> heatMap;

        UiWarehouseOptimizerThread_t::UiWarehouseOptimizerThread_t(const whm::ConfigParser_t& cfg_, int32_t opt_)
            : opt(opt_)
            , cfg(cfg_)
        {

        }

        void UiWarehouseOptimizerThread_t::run()
        {
            whm::utils::WhmArgs_t args;

            args.articlesPath  = cfg.getAs<std::string>("articlesPath");
            args.locationsPath = cfg.getAs<std::string>("locationsPath");
            args.ordersPath    = cfg.getAs<std::string>("ordersPath");

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

            whm::WarehouseOptimizerBase_t* optimizer{ nullptr };

            switch(opt)
            {
                case 0: optimizer = new whm::WarehouseOptimizerGA_t  {args, cfg}; break;
                case 1: optimizer = new whm::WarehouseOptimizerDE_t  {args, cfg}; break;
                case 2: optimizer = new whm::WarehouseOptimizerABC_t {args, cfg}; break;
                case 3: optimizer = new whm::WarehouseOptimizerPSO_t {args, cfg}; break;
                case 4: optimizer = new whm::WarehouseOptimizerSLAP_t{args, cfg}; break;
                case 5: optimizer = new whm::WarehouseOptimizerRAND_t{args, cfg}; break;
            }

            optimizer->setUiCallback([&](double fitness)
                                     {
                                        emit optimizationStep(fitness);
                                     });

            optimizer->optimize();

            emit optimizationFinished();

            delete optimizer;
        }
    }
}
