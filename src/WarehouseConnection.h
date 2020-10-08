/**
 * Warehouse manager
 *
 * @file    WarehouseConnection.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class representing connections between items/ports
 */

#pragma once

#ifdef WHM_GUI
namespace whm
{
    namespace gui
    {
        class UiWarehouseConnection_t;
    }
}
#endif

namespace whm
{
    class WarehousePort_t;

    class WarehouseConnection_t
    {
        public:
            ~WarehouseConnection_t();

#ifdef WHM_GUI
            WarehouseConnection_t() = delete;
            WarehouseConnection_t(gui::UiWarehouseConnection_t&);
#else
            WarehouseConnection_t();
#endif

            WarehousePort_t* getTo() const;
            WarehousePort_t* getFrom() const;
            WarehousePort_t* lookupPort(int32_t, int32_t);

            void dump() const;

        protected:
            WarehousePort_t* to{ nullptr };
            WarehousePort_t* from{ nullptr };
    };
}