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

    template <typename E>
    constexpr auto to_underlying(E e) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(e);
    }
}
