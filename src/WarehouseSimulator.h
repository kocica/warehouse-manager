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
    - Kazda lokace/picker na lokaci a mozna i conveyor jako Facility
    - Jednotlive krabice/objednavky jako Process
    - Cas pro "Wait" pocitany obdobne, jako ted - m/s, kolik je metru, rychlost pickera, ze ktere lokace, vaha produktu apod.
*/

#pragma once

#include <map>

#include "Utils.h"
#include "WarehouseItem.h"
#include "WarehouseOrder.h"
#include "WarehouseLayout.h"
#include "WarehouseItemType.h"
#include "WarehousePathFinder.h"

#include "../include/simlib.h"

namespace whm
{
    class WarehouseSimulator_t
    {
        public:
            WarehouseSimulator_t();
            ~WarehouseSimulator_t();

            void runSimulation();

            utils::SimArgs_t getArguments();
            void setArguments(const utils::SimArgs_t&);

            static WarehouseSimulator_t& getWhSimulator();

            simlib3::Store* getWhItemFacility(int32_t);

            WarehouseItem_t* lookupWhGate(const WarehouseItemType_t&);
            std::vector<int32_t> lookupWhLocations(const std::string&, int32_t);
            WarehousePathInfo_t* lookupShortestPath(int32_t, const std::vector<int32_t>&);

        protected:
            void prepareWhSimulation();

        private:
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

                auto& sim = WarehouseSimulator_t::getWhSimulator();

                locationID = sim.lookupWhGate(WarehouseItemType_t::E_WAREHOUSE_ENTRANCE)->getWhItemID();

                for(const auto& orderLine : order)
                {
                    const auto& targetLocations = sim.lookupWhLocations(orderLine.getArticle(), orderLine.getQuantity());
                    const auto* shortestPath = sim.lookupShortestPath(locationID, targetLocations);

                    for(const auto& pathItem : shortestPath->pathToTarget)
                    {
                        int64_t waitDuration = pathItem.second / sim.getArguments().toteSpeed * 1000000;

                        simlib3::Store* whFacility = sim.getWhItemFacility(pathItem.first);

                        Enter(*whFacility, 1);
                        Wait(waitDuration);
                        Leave(*whFacility, 1);
                    }

                    // TODO: Simulate picking

                    locationID = shortestPath->targetWhItemID;
                }

                // TODO: Simulate shipping
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
                    Activate(Time+Exponential(1e3));
                }
            }

        public:
            OrderRequest_t(WarehouseLayout_t& layout_) : layout(layout_), it(layout.getWhOrders().begin()) {}
    };
}
