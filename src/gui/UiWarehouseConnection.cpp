/**
 * Warehouse manager
 *
 * @file    UiWarehouseConnection.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse connection between ports
 */

#include <iostream>

#include "UiWarehousePort.h"
#include "UiWarehouseLayout.h"
#include "UiWarehouseConnection.h"

#include "../WarehousePort.h"
#include "../WarehouseItem.h"
#include "../WarehouseConnection.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseConnection_t::UiWarehouseConnection_t(UiWarehousePort_t *from, UiWarehousePort_t *to)
            : to(to)
            , from(from)
        {
            
        }

        UiWarehouseConnection_t::UiWarehouseConnection_t(::whm::WarehouseConnection_t& c)
        {
            to = this->lookupPort(c.getTo()->getWhItem()->getID(), c.getTo()->getWhPortID());
            from = this->lookupPort(c.getFrom()->getWhItem()->getID(), c.getFrom()->getWhPortID());

            to->setWhConn(this);
            from->setWhConn(this);
        }

        UiWarehousePort_t* UiWarehouseConnection_t::lookupPort(int32_t whItemID, int32_t whPortID)
        {
            const auto& whItems = UiWarehouseLayout_t::getWhLayout().getWhItems();

            auto it = std::find_if(whItems.begin(), whItems.end(),
                                  [&](UiWarehouseItem_t* i) -> bool
                                  {
                                      return i->getID() == whItemID;
                                  });

            if(it != whItems.end())
            {
                const auto& whPorts = (*it)->getWhPorts();

                auto itp = std::find_if(whPorts.begin(), whPorts.end(),
                                        [&](UiWarehousePort_t* p) -> bool
                                        {
                                            return p->getWhPortID() == whPortID;
                                        });

                if(itp != whPorts.end())
                {
                    return *itp;
                }
            }

            std::cerr << "Failed to lookup port!" << std::endl;
            return nullptr;
        }

        UiWarehousePort_t* UiWarehouseConnection_t::getTo() const
        {
            return to;
        }

        UiWarehousePort_t* UiWarehouseConnection_t::getFrom() const
        {
            return from;
        }

        UiWarehouseConnection_t::~UiWarehouseConnection_t()
        {
            to->disconnect();
            from->disconnect();

            UiWarehouseLayout_t::getWhLayout().eraseWhConn(this);
        }

        void UiWarehouseConnection_t::dump() const
        {
            std::cout << std::endl << "      = Dump warehouse connection from <" << from->getWhItem()->getWhItemID() << "> <" << from->getWhPortID() << "> to <" <<
                                                                                      to->getWhItem()->getWhItemID() << "> <" <<   to->getWhPortID() << ">" << std::endl;
        }
    }
}
