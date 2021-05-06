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
            WarehouseConnection_t();
            ~WarehouseConnection_t();

#ifdef WHM_GUI
            WarehouseConnection_t(gui::UiWarehouseConnection_t&);
#endif

            int32_t getWhConnID() const;
            WarehousePort_t* getTo() const;
            WarehousePort_t* getFrom() const;
            WarehousePort_t* lookupPort(int32_t, int32_t);

            void dump() const;

            // (De)-serialization
            void serializeToXml(tinyxml2::XMLDocument* doc) const;
            void deserializeFromXml(tinyxml2::XMLElement*);

        protected:
            WarehousePort_t* to{ nullptr };
            WarehousePort_t* from{ nullptr };

            int32_t whConnID{ 0 };
    };
}