/**
 * Warehouse manager
 *
 * @file    WarehouseItemType.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Warehouse item types
 */

#pragma once

#include <type_traits>

namespace whm
{
    enum class WarehouseItemType_t : int
    {
        E_LOCATION_SHELF,     //< Location shelf
        E_CONVEYOR,           //< Conv
        E_CONVEYOR_HUB,       //< Able to connect up to four convs
        E_WAREHOUSE_ENTRANCE, //< Where outbound orders are started
        E_WAREHOUSE_DISPATCH, //< Where outbound orders are shipped
        E_WAREHOUSE_BUFFER    //< Where reprenishment orders are started
    };

    enum class WarehouseOrderType_t : int
    {
        E_OUTBOUND_ORDER,      //< Used to fulfill customer requirements
        E_REPLENISHMENT_ORDER  //< Used for replenishment of consumed products
    };

    enum class WarehousePortType_t : int
    {
        E_PORT_MID,            //< Port in the center of an item
        E_PORT_LEFT,           //< Port on the left of an item
        E_PORT_RIGHT,          //< Port on the right of an item
        E_PORT_TOP,            //< Port on the top of an item
        E_PORT_BOTTOM          //< Port on the bottom of an item
    };

    template <typename E>
    constexpr auto to_underlying(E e) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(e);
    }
}
