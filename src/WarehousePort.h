/**
 * Warehouse manager
 *
 * @file    WarehousePort.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class used for interconnection between warehouse items
 */

#pragma once

#ifdef WHM_GUI
namespace whm
{
    namespace gui
    {
        class UiWarehousePort_t;
    }
}
#endif

namespace whm
{
    class WarehouseItem_t;
    class WarehouseConnection_t;

    class WarehousePort_t
    {
        public:
            ~WarehousePort_t() = default;

#ifdef WHM_GUI
            WarehousePort_t() = delete;
            WarehousePort_t(gui::UiWarehousePort_t&);
#else
            WarehousePort_t();
#endif
            void dump() const;

            void setWhItem(WarehouseItem_t*);
            WarehouseItem_t* getWhItem() const;

            void setWhConn(WarehouseConnection_t*);
            WarehouseConnection_t* getWhConn() const;

            bool isConnected() const;

        protected:
            WarehouseItem_t* whItem{ nullptr };
            WarehouseConnection_t* whConn{ nullptr };

            // TODO: Fields
    };
}