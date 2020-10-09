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
        to = this->lookupPort(uiConn.getTo()->getWhItem()->getID(), uiConn.getTo()->getWhPortID());
        from = this->lookupPort(uiConn.getFrom()->getWhItem()->getID(), uiConn.getFrom()->getWhPortID());

        to->setWhConn(this);
        from->setWhConn(this);

        whConnID = uiConn.getWhConnID();
    }
#else
    WarehouseConnection_t::WarehouseConnection_t()
    {

    }
#endif

    WarehouseConnection_t::~WarehouseConnection_t()
    {
        // TODO: Clean-up
        // delete to;
        // delete from;
    }

    void WarehouseConnection_t::serializeToXml(tinyxml2::XMLDocument* doc) const
    {
        tinyxml2::XMLNode* whConn = doc->InsertEndChild( doc->NewElement( "WarehouseConnection" ) );

        tinyxml2::XMLElement* whConnAttribs = doc->NewElement( "Attributes" );

        // Set attributes
        whConnAttribs->SetAttribute( "id", getWhConnID() );
        whConn->InsertEndChild( whConnAttribs );

        // Add both ports
        tinyxml2::XMLElement* whPortTo = doc->NewElement( "WarehousePortTo" );
        tinyxml2::XMLElement* whPortFrom = doc->NewElement( "WarehousePortFrom" );

        whPortFrom->SetAttribute( "item_id", from->getWhItem()->getID() );
        whPortFrom->SetAttribute( "port_id", from->getWhPortID() );

        whPortTo->SetAttribute( "item_id", to->getWhItem()->getID() );
        whPortTo->SetAttribute( "port_id", to->getWhPortID() );

        whConn->InsertEndChild( whPortTo );
        whConn->InsertEndChild( whPortFrom );
    }

    int32_t WarehouseConnection_t::getWhConnID() const
    {
        return whConnID;
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
        std::cout << std::endl << "      = Dump warehouse connection ID <" << getWhConnID() << ">" <<
                                  " from <" << from->getWhItem()->getID() << "> <" << from->getWhPortID() << ">" <<
                                  " to <"   <<   to->getWhItem()->getID() << "> <" <<   to->getWhPortID() << ">" << std::endl;
    }

    WarehousePort_t* WarehouseConnection_t::lookupPort(int32_t whItemID, int32_t whPortID)
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
}