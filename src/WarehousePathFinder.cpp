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

#include "Logger.h"
#include "WarehousePort.h"
#include "WarehouseItem.h"
#include "WarehouseLayout.h"
#include "WarehouseConnection.h"
#include "WarehousePathFinder.h"

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<std::pair<T, T>>& v)
{
  if ( !v.empty() )
  {
    out << '[';
    std::for_each(v.begin(), v.end(), [](const std::pair<T, T>& p)
                                      { std::cout << "(" << p.first << ", " << p.second << "); "; });
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

    void WarehousePathFinder_t::clearPrecalculatedPaths()
    {
        whPaths.clear();
    }

    void WarehousePathFinder_t::precalculatePaths(const std::vector<WarehouseItem_t*>& whItems)
    {
        std::for_each(whItems.begin(), whItems.end(),
                      [&](const WarehouseItem_t* whItem)
                      {
                          if(whItem->getType() != WarehouseItemType_t::E_CONVEYOR &&
                             whItem->getType() != WarehouseItemType_t::E_CONVEYOR_HUB)
                          {
                            this->precalculatePaths(whItem->getWhItemID(), whItem, WarehousePathInfo_t{});
                          }
                      });

        std::unordered_map<int32_t, std::vector<WarehousePathInfo_t>> whPathsFiltered;

        for(auto* whItemLhs : whItems)
        {
            if(whItemLhs->getType() != WarehouseItemType_t::E_CONVEYOR &&
               whItemLhs->getType() != WarehouseItemType_t::E_CONVEYOR_HUB)
            {
                for(auto* whItemRhs : whItems)
                {
                    if(whItemRhs->getType() != WarehouseItemType_t::E_CONVEYOR &&
                       whItemRhs->getType() != WarehouseItemType_t::E_CONVEYOR_HUB)
                    {
                        auto* shortestPath = getShortestPath(whItemLhs->getWhItemID(), whItemRhs->getWhItemID());

                        if(shortestPath)
                        {
                            whPathsFiltered[whItemLhs->getWhItemID()].push_back(*shortestPath);
                        }
                        else
                        {
                            Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_ERROR, "Failed to filter paths");
                        }
                    }
                }
            }
        }

        whPaths = std::move(whPathsFiltered);
    }

    void WarehousePathFinder_t::precalculatePaths(int32_t sourceWhItemID, const WarehouseItem_t* whItem, WarehousePathInfo_t pathInfo)
    {
        auto whItemID = whItem->getWhItemID();
        auto it = std::find_if(pathInfo.pathToTarget.begin(), pathInfo.pathToTarget.end(),
                               [&whItemID](auto& pathItem) -> bool
                               {
                                   return pathItem.first == whItemID;
                               });

        if(it != pathInfo.pathToTarget.end())
        {
            // Loop detected, abort.
            return;
        }

        // Add new item to the map in case its not conv
        if(whItem->getType() != WarehouseItemType_t::E_CONVEYOR &&
           whItem->getType() != WarehouseItemType_t::E_CONVEYOR_HUB)
        {
            pathInfo.targetWhItemID = whItemID;
            whPaths[sourceWhItemID].push_back(pathInfo);
            if(sourceWhItemID != whItemID)
            {
                return;
            }
        }

        // Update path info attributes
        if(sourceWhItemID != whItemID)
        {
            pathInfo.pathToTarget.emplace_back(std::make_pair(whItemID, whItem->getW() / WarehouseLayout_t::getWhLayout().getRatio()));
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
                    if(!shortestPath || (pathDistance(shortestPath->pathToTarget) > pathDistance(pathInfo.pathToTarget)))
                    {
                        shortestPath = const_cast<WarehousePathInfo_t*>(&pathInfo);
                    }
                }
            }
        }

        return shortestPath;
    }

    int32_t WarehousePathFinder_t::pathDistance(const WarehousePath_t& path) const
    {
        int32_t summedPathDistance{ 0 };

        for(const auto& pathItem : path)
        {
            summedPathDistance += pathItem.second;
        }

        return summedPathDistance;
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
                          << "> through path: "        << whPathInfo.pathToTarget
                          << ", total distance: <"     << pathDistance(whPathInfo.pathToTarget)
                          << ">"                       << std::endl;
            }
        }
    }
}
