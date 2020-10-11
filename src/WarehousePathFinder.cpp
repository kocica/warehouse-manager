/**
 * Warehouse manager
 *
 * @file    WarehousePathFinder.cpp
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Class used for finding paths between warehouse items
 */


#include <iostream>
#include <iterator>
#include <algorithm>

#include "WarehousePort.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseConnection.h"
#include "WarehousePathFinder.h"

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
{
  if ( !v.empty() )
  {
    out << '[';
    std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "]";
  }
  return out;
}

namespace whm
{
    WarehousePathFinder_t::WarehousePathFinder_t()
    {

    }

    WarehousePathFinder_t::~WarehousePathFinder_t()
    {

    }

    void WarehousePathFinder_t::precalculatePaths(const std::vector<WarehouseItem_t*>& whItems)
    {
        std::for_each(whItems.begin(), whItems.end(),
                      [&](const WarehouseItem_t* whItem)
                      {
                          this->precalculatePaths(whItem->getWhItemID(), whItem, WarehousePathInfo_t{});
                      });
    }

    void WarehousePathFinder_t::precalculatePaths(int32_t sourceWhItemID, const WarehouseItem_t* whItem, WarehousePathInfo_t pathInfo)
    {
        auto whItemID = whItem->getWhItemID();
        auto it = std::find(pathInfo.pathWhItemIDs.begin(), pathInfo.pathWhItemIDs.end(), whItemID);

        if(it != pathInfo.pathWhItemIDs.end())
        {
            //std::cout << "Loop detected, abort." << std::endl;
            return;
        }

        // Add new item to the map in case its not conv
        pathInfo.targetWhItemID = whItemID;
        whPaths[sourceWhItemID].push_back(pathInfo);

        // Update path info attributes
        if(sourceWhItemID != whItemID)
        {
            pathInfo.pathWhItemIDs.push_back(whItemID);
            pathInfo.distance += whItem->getW();
        }

        // Recursively call for each connected port
        const auto& whPorts = whItem->getWhPorts();
        std::for_each(whPorts.begin(), whPorts.end(),
                      [&](const auto* whPort)
                      {
                          if(whPort->isConnected())
                          {
                              const auto* whConn = whPort->getWhConn();

                              if (whConn->getTo()->getWhItem()->getWhItemID() != sourceWhItemID)
                              {
                                  precalculatePaths(sourceWhItemID, whConn->getTo()->getWhItem(), pathInfo);
                              }
                              if (whConn->getFrom()->getWhItem()->getWhItemID() != sourceWhItemID)
                              {
                                  precalculatePaths(sourceWhItemID, whConn->getFrom()->getWhItem(), pathInfo);
                              }
                          }
                      });
    }

    WarehousePathInfo_t* WarehousePathFinder_t::getShortestPath(int32_t lhsItemID, int32_t rhsItemID) const
    {
        WarehousePathInfo_t* shortestPath{ nullptr };

        auto it = whPaths.find(lhsItemID);

        if(it != whPaths.end())
        {
            for(const WarehousePathInfo_t& pathInfo : it->second)
            {
                if(pathInfo.targetWhItemID == rhsItemID)
                {
                    if(!shortestPath || (shortestPath->distance > pathInfo.distance))
                    {
                        shortestPath = const_cast<WarehousePathInfo_t*>(&pathInfo);
                    }
                }
            }
        }

        return shortestPath;
    }

    void WarehousePathFinder_t::dump() const
    {
        for(const auto& whPath : whPaths)
        {
            std::cout << "-------------------------------------" << std::endl;
            std::cout << "Warehouse path(s) from <" << whPath.first << ">:" << std::endl;

            for(const auto& whPathInfo : whPath.second)
            {
                std::cout << "  - Warehouse path to <" << whPathInfo.targetWhItemID
                          << "> goes through <"        << whPathInfo.pathWhItemIDs
                          << "> and takes <"           << whPathInfo.distance
                          << "> meters "               << std::endl;
            }
        }
    }
}
