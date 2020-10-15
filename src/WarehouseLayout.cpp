/**
 * Warehouse manager
 *
 * @file    WarehouseLayout.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class managing whole warehouse layout (locations, connections, ...)
 */

// Std
#include <fstream>
#include <iostream>
#include <algorithm>

// Local
#include "WarehouseItem.h"
#include "WarehouseOrder.h"
#include "WarehouseLayout.h"
#include "WarehouseConnection.h"
#include "WarehousePathFinder.h"
#include "WarehouseLocationRack.h"

#ifdef WHM_GUI
#include "gui/UiWarehouseLayout.h"
#endif

#include "tinyxml2.h"

namespace whm
{
    WarehouseLayout_t::WarehouseLayout_t()
    {

    }

    WarehouseLayout_t::~WarehouseLayout_t()
    {
        this->clearWhLayout();
    }

    WarehouseLayout_t& WarehouseLayout_t::getWhLayout()
    {
        static WarehouseLayout_t l;
        return l;
    }

#ifdef WHM_GUI
    void WarehouseLayout_t::initFromGui(gui::UiWarehouseLayout_t& uiLayout)
    {
        this->clearWhLayout();

        auto uiItems = uiLayout.getWhItems();
        auto uiConns = uiLayout.getWhConns();

        this->whRatio = uiLayout.getRatio(); 
        this->whDims = uiLayout.getDimensions();

        for(auto* uiItem : uiItems)
        {
            whItems.emplace_back(new WarehouseItem_t{ *uiItem });
        }

        for(auto* uiConn : uiConns)
        {
            whConns.emplace_back(new WarehouseConnection_t{ *uiConn });
        }
    }
#endif

    void WarehouseLayout_t::serializeToXml(const std::string& xmlFilename)
    {
        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

        // Layout info
        tinyxml2::XMLNode* whLayout = doc->InsertEndChild( doc->NewElement( "WarehouseLayout" ) );
        tinyxml2::XMLElement* whLayoutAttribs = doc->NewElement( "Attributes" );
        whLayoutAttribs->SetAttribute( "dim_x", whDims.first );
        whLayoutAttribs->SetAttribute( "dim_y", whDims.second );
        whLayoutAttribs->SetAttribute( "ratio", whRatio );
        whLayout->InsertEndChild( whLayoutAttribs );

        // Items
        for(auto* whItem : whItems)
        {
            whItem->serializeToXml(doc);
        }

        // Connections
        for(auto* whConn : whConns)
        {
            whConn->serializeToXml(doc);
        }

        doc->SaveFile(xmlFilename.c_str());
        delete doc;
    }

    void WarehouseLayout_t::deserializeFromXml(const std::string& xmlFilename)
    {
        this->clearWhLayout();

        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

        doc->LoadFile(xmlFilename.c_str());

        // Layout info
        tinyxml2::XMLElement* whLayoutXml = doc->FirstChildElement("WarehouseLayout");
        tinyxml2::XMLElement* whLayoutAttribsXml = whLayoutXml->FirstChildElement( "Attributes" );
        this->whRatio = whLayoutAttribsXml->IntAttribute("ratio");
        this->whDims = std::make_pair(whLayoutAttribsXml->IntAttribute("dim_x"), whLayoutAttribsXml->IntAttribute("dim_y"));

        // Items
        for (tinyxml2::XMLElement* whItemXml = doc->FirstChildElement("WarehouseItem"); whItemXml; whItemXml = whItemXml->NextSiblingElement("WarehouseItem"))
        {
            WarehouseItem_t* whItem = new WarehouseItem_t();
            whItem->deserializeFromXml(whItemXml);
            addWhItem(whItem);
        }

        // Connections
        for (tinyxml2::XMLElement* whConnXml = doc->FirstChildElement("WarehouseConnection"); whConnXml; whConnXml = whConnXml->NextSiblingElement("WarehouseConnection"))
        {
            WarehouseConnection_t* whConn = new WarehouseConnection_t();
            whConn->deserializeFromXml(whConnXml);
            addWhConn(whConn);
        }

        delete doc;
    }

    void WarehouseLayout_t::importCustomerOrders(const std::string& xmlFilename)
    {
        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

        doc->LoadFile(xmlFilename.c_str());

        for (tinyxml2::XMLElement* whOrderXml = doc->FirstChildElement("WarehouseOrder"); whOrderXml; whOrderXml = whOrderXml->NextSiblingElement("WarehouseOrder"))
        {
            WarehouseOrder_t<std::string> whOrder;
            whOrder.deserializeFromXml(whOrderXml);
            addWhOrder(std::move(whOrder));
        }

        delete doc;
    }

    void WarehouseLayout_t::importLocationSlots(const std::string& csvFilename)
    {
        std::ifstream csvStream;
        csvStream.open(csvFilename);

        // Ignore header
        std::string header;
        std::getline(csvStream, header);

        std::for_each(whItems.begin(), whItems.end(),
                      [&](const WarehouseItem_t* whItem) -> void
                      {
                          if (whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
                          {
                              whItem->getWhLocationRack()->importSlots(csvStream);
                          }
                      });

        csvStream.close();
    }

    void WarehouseLayout_t::exportLocationSlots(const std::string& csvFilename)
    {
        std::ofstream csvStream;
        csvStream.open(csvFilename);

        // Create header
        csvStream << "Warehouse location ID;Slot X;Slot Y;Article;Quantity" << std::endl;

        std::for_each(whItems.begin(), whItems.end(),
                      [&](const WarehouseItem_t* whItem) -> void
                      {
                          if (whItem->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
                          {
                              whItem->getWhLocationRack()->exportSlots(csvStream);
                          }
                      });

        csvStream.close();
    }

    void WarehouseLayout_t::addWhItem(WarehouseItem_t* i)
    {
        whItems.push_back(i);
    }

    void WarehouseLayout_t::addWhConn(WarehouseConnection_t* c)
    {
        whConns.push_back(c);
    }

    void WarehouseLayout_t::addWhOrder(const WarehouseOrder_t<std::string>& o)
    {
        whOrders.push_back(o);
    }

    WarehouseLayout_t::WarehouseItemContainer_t WarehouseLayout_t::getWhItems() const
    {
        return whItems;
    }

    WarehouseLayout_t::WarehouseConnContainer_t WarehouseLayout_t::getWhConns() const
    {
        return whConns;
    }

    const WarehouseLayout_t::WarehouseOrderContainer_t& WarehouseLayout_t::getWhOrders() const
    {
        return whOrders;
    }

    void WarehouseLayout_t::dump() const
    {
        std::cout << "*******************************************************" << std::endl;
        std::cout << " Dump warehouse layout <" << whDims.first << "> x <" << whDims.second
                  << "> ratio <" << whRatio << ">" << std::endl << std::endl;

        std::for_each(whItems.begin(), whItems.end(),
                      [](const WarehouseItem_t* whItem) -> void
                      {
                          whItem->dump();
                      });

        std::cout << std::endl << std::endl;

        std::for_each(whConns.begin(), whConns.end(),
                      [](const WarehouseConnection_t* whConn) -> void
                      {
                          whConn->dump();
                      });

        std::cout << std::endl << std::endl;

        std::for_each(whOrders.begin(), whOrders.end(),
                      [](const auto& whOrder) -> void
                      {
                          whOrder.dump();
                      });
    }

    void WarehouseLayout_t::clearWhLayout()
    {
        for (WarehouseItem_t* whItem : whItems)
        {
            delete whItem;
        }

        for (WarehouseConnection_t* whConn : whConns)
        {
            delete whConn;
        }

        whItems.clear();
        whConns.clear();
    }

    void WarehouseLayout_t::eraseWhItem(WarehouseItem_t* i)
    {
        auto found = std::find_if(whItems.begin(), whItems.end(),
                        [&](WarehouseItem_t* whItem) -> bool
                        {
                            return whItem->getWhItemID() == i->getWhItemID(); // whItem == i
                        });

        if (found != whItems.end())
        {
            whItems.erase(found);
        }
    }

    void WarehouseLayout_t::eraseWhConn(WarehouseConnection_t* c)
    {
        auto found = std::find_if(whConns.begin(), whConns.end(),
                        [&](WarehouseConnection_t* whConn) -> bool
                        {
                            return whConn == c;
                        });

        if (found != whConns.end())
        {
            whConns.erase(found);
        }
    }

    void WarehouseLayout_t::eraseWhOrder(const WarehouseOrder_t<std::string>& o)
    {
        auto found = std::find_if(whOrders.begin(), whOrders.end(),
                        [&](const auto& whOrder) -> bool
                        {
                            return whOrder.getWhOrderID() == o.getWhOrderID();
                        });

        if (found != whOrders.end())
        {
            whOrders.erase(found);
        }
    }

    int32_t WarehouseLayout_t::getRatio() const
    {
        return this->whRatio;
    }

    WarehouseLayout_t::WarehouseDimensions_t WarehouseLayout_t::getDimensions() const
    {
        return this->whDims;
    }
}
