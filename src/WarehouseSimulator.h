/**
 * Warehouse manager
 *
 * @file    WarehouseSimulator.h
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Module responsible for simulation of the warehouse operations
 */

/*
  TODO:

    - Pri pickovani na stanici se podivat, jestli tam nelze napickovat i nejakou jinou lajnu
    - Flag u order line - processed, znacici ze je jiz zpracovana a pri vyhledavani dalsi lajny brat jen processed = false
    - Init = MAX
        double simStart = Time
        double simStop = Time
        Calculate how long it takes to process orders
*/


#pragma once

#include <map>

#include "Utils.h"
#include "WarehouseItem.h"
#include "WarehouseOrder.h"
#include "WarehouseLayout.h"
#include "WarehouseItemType.h"
#include "WarehousePathFinder.h"
#include "WarehouseLocationRack.h"

#include "../include/simlib.h"

namespace whm
{
    class WarehouseSimulator_t
    {
        public:
            WarehouseSimulator_t();
            ~WarehouseSimulator_t();

            void runSimulation();

            void orderFinished();

            utils::SimArgs_t getArguments();
            void setArguments(const utils::SimArgs_t&);

            static WarehouseSimulator_t& getWhSimulator();

            simlib3::Store* getWhItemFacility(int32_t);

            WarehouseItem_t* lookupWhLoc(int32_t);
            WarehouseItem_t* lookupWhGate(const WarehouseItemType_t&);
            std::vector<int32_t> lookupWhLocations(const std::string&, int32_t);
            WarehousePathInfo_t* lookupShortestPath(int32_t, const std::vector<int32_t>&);

        protected:
            void prepareWhSimulation();

        private:
            double simStart;
            utils::SimArgs_t args;

            WarehouseLayout_t& whLayout;
            WarehousePathFinder_t* whPathFinder;

            std::map<int32_t, simlib3::Store*> whFacilities;
    };


    class OrderProcessor_t : public simlib3::Process
    {
        private:
            int32_t locationID{ 0 };
            WarehouseOrder_t<std::string> order;

            void Behavior()
            {
                double waitDuration = 0.0;
                auto& sim = WarehouseSimulator_t::getWhSimulator();

                const auto& handleFacility = [&](int32_t itemID)
                                             {
                                                simlib3::Store* whFacility = sim.getWhItemFacility(itemID);

                                                Enter(*whFacility, 1);
                                                Wait(waitDuration * sim.getArguments().speedup);
                                                Leave(*whFacility, 1);
                                             };

                locationID = sim.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)->getWhItemID();

                for(const WarehouseOrderLine_t<std::string>& orderLine : order)
                {
                    const std::vector<int32_t>& targetLocations = sim.lookupWhLocations(orderLine.getArticle(), orderLine.getQuantity());
                    const WarehousePathInfo_t* shortestPath = sim.lookupShortestPath(locationID, targetLocations);

                    for(const std::pair<int32_t, int32_t>& pathItem : shortestPath->pathToTarget)
                    {
                        waitDuration = pathItem.second / sim.getArguments().toteSpeed;

                        handleFacility(pathItem.first);
                    }

                    locationID = shortestPath->targetWhItemID;

                    WarehouseItem_t* whLoc = sim.lookupWhLoc(locationID);
                    std::pair<size_t, size_t> slotPos;
                    whLoc->getWhLocationRack()->containsArticle(orderLine.getArticle(), 0, slotPos);
                    waitDuration = ((slotPos.first  / static_cast<float>(whLoc->getWhLocationRack()->getSlotCountX())) +
                                    (slotPos.second / static_cast<float>(whLoc->getWhLocationRack()->getSlotCountY()))) / sim.getArguments().pickerSpeed;

                    handleFacility(locationID);
                }

                int32_t dispatchID = sim.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_DISPATCH)->getWhItemID();

                const WarehousePathInfo_t* shortestPath = sim.lookupShortestPath(locationID, std::vector<int32_t>{ dispatchID });

                for(const std::pair<int32_t, int32_t>& pathItem : shortestPath->pathToTarget)
                {
                    waitDuration = pathItem.second / sim.getArguments().toteSpeed;

                    handleFacility(pathItem.first);
                }

                locationID = dispatchID;

                waitDuration = (sim.getArguments().totesPerMin / 60);

                handleFacility(locationID);

                sim.orderFinished();
            }

        public:
            OrderProcessor_t(WarehouseOrder_t<std::string> order_) : order(order_) { }
    };


    class OrderRequest_t : public simlib3::Event
    {
        private:
            WarehouseLayout_t& layout;

            std::vector<WarehouseOrder_t<std::string>>::const_iterator it;

            void Behavior()
            {
                (new OrderProcessor_t(*it))->Activate();

                if(++it != layout.getWhOrders().end())
                {
                    Activate(Time + Uniform(10, 20));
                }
            }

        public:
            OrderRequest_t(WarehouseLayout_t& layout_) : layout(layout_), it(layout.getWhOrders().begin()) {}
    };
}
