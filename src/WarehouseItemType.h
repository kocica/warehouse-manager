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
        E_LOCATION_SHELF,
        E_CONVEYOR,           //< Convs to all directions, so we can detect flow
        E_CONVEYOR_HUB,       //< Able to connect up to four convs
        E_WAREHOUSE_ENTRANCE,
        E_WAREHOUSE_DISPATCH
    };

    enum class WarehousePortType_t : int
    {
        E_PORT_MID,
        E_PORT_LEFT,
        E_PORT_RIGHT,
        E_PORT_TOP,
        E_PORT_BOTTOM
    };

    template <typename E>
    constexpr auto to_underlying(E e) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(e);
    }
}
