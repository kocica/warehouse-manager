/**
 * Warehouse manager
 *
 * @file    WarehouseConnection.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class representing connections between items/ports
 */

// Std
#include <iostream>
#include <algorithm>

// Local
#include "WarehousePort.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseConnection.h"

#ifdef WHM_GUI
#include "gui/UiWarehousePort.h"
#include "gui/UiWarehouseItem.h"
#include "gui/UiWarehouseConnection.h"
#endif

namespace whm
{
#ifdef WHM_GUI
    WarehouseConnection_t::WarehouseConnection_t(gui::UiWarehouseConnection_t& uiConn)
    {
        to = this->lookupPort(uiConn.getTo()->getWhItem()->getID());
        from = this->lookupPort(uiConn.getFrom()->getWhItem()->getID());

        to->setWhConn(this);
        from->setWhConn(this);
    }
#else
    WarehouseConnection_t::WarehouseConnection_t()
    {

    }
#endif

    WarehouseConnection_t::~WarehouseConnection_t()
    {
        // TODO
        // delete to;
        // delete from;
    }
    
    WarehousePort_t* WarehouseConnection_t::getTo() const
    {
        return to;
    }

    WarehousePort_t* WarehouseConnection_t::getFrom() const
    {
        return from;
    }

    void WarehouseConnection_t::dump() const
    {
        std::cout << std::endl << "      = Connection from <" << from->getWhItem()->getID() << "> to <" <<
                                                                   to->getWhItem()->getID() << ">" << std::endl;
    }

    WarehousePort_t* WarehouseConnection_t::lookupPort(int32_t whItemID)
    {
        const auto& whItems = WarehouseLayout_t::getWhLayout().getWhItems();

        auto it = std::find_if(whItems.begin(), whItems.end(),
                               [&](WarehouseItem_t* i) -> bool
                               {
                                   return i->getID() == whItemID;
                               });

        if(it != whItems.end())
        {
            const auto& whPorts = (*it)->getWhPorts();

            auto itp = std::find_if(whPorts.begin(), whPorts.end(),
                                    [&](WarehousePort_t* p) -> bool
                                    {
                                        return !p->isConnected();
                                    });

            if(itp != whPorts.end())
            {
                return *itp;
            }
        }

        std::cerr << "Failed to lookup port!" << std::endl;
        return nullptr;
    }
}