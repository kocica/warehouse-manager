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

namespace whm
{
    namespace gui
    {
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

            std::map<std::string, int32_t> heatMap;

            for(auto& order : ::whm::WarehouseLayout_t::getWhLayout().getWhOrders())
            {
                for(auto& line : order.getWhOrderLines())
                {
                    heatMap[line.getArticle()]++;
                }
            }

            int32_t max{ std::numeric_limits<int32_t>::min() };
            int32_t min{ std::numeric_limits<int32_t>::max() };

            for(auto& m : heatMap)
            {
                max = m.second > max ? m.second : max;
                min = m.second < min ? m.second : min;
            }

            whm::WarehouseOptimizerBase_t* optimizer{ nullptr };

            switch(opt)
            {
                case 0: optimizer = new whm::WarehouseOptimizerGA_t {args, cfg}; break;
                case 1: optimizer = new whm::WarehouseOptimizerDE_t {args, cfg}; break;
                case 2: optimizer = new whm::WarehouseOptimizerABC_t{args, cfg}; break;
                case 3: optimizer = new whm::WarehouseOptimizerPSO_t{args, cfg}; break;
            }

            optimizer->setUiCallback([&](double fitness)
                                     {
                                        emit optimizationStep(fitness);

                                        auto items = ::whm::WarehouseLayout_t::getWhLayout().getWhItems();
                                        auto uiItems = UiWarehouseLayout_t::getWhLayout().getWhItems();

                                        for(auto* item : items)
                                        {
                                            if(item->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
                                            {
                                                for(auto* uiItem : uiItems)
                                                {
                                                    if(item->getWhItemID() == uiItem->getWhItemID())
                                                    {
                                                        dynamic_cast<UiWarehouseItemLocation_t*>(uiItem)->importSlots(*item);
                                                    }
                                                }
                                            }
                                        }

                                        for(auto* item : UiWarehouseLayout_t::getWhLayout().getWhItems())
                                        {
                                            if(item->getWhItemType() == WarehouseItemType_t::E_LOCATION_SHELF)
                                            {
                                                for(auto* slot : dynamic_cast<UiWarehouseItemLocation_t*>(item)->getSlots())
                                                {
                                                    std::string article = slot->getArticle();

                                                    if(!article.empty())
                                                    {
                                                        slot->setSlotHeat(max, min, heatMap[article]);
                                                    }
                                                }
                                            }
                                        }
                                     });
            optimizer->optimize();

            emit optimizationFinished();

            delete optimizer;
        }
    }
}
