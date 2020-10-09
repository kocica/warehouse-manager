/**
 * Warehouse manager
 *
 * @file    WarehousePort.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class used for interconnection between warehouse items
 */

#pragma once

#include "tinyxml2.h"

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
            WarehousePort_t();
            ~WarehousePort_t();

#ifdef WHM_GUI
            WarehousePort_t(gui::UiWarehousePort_t&);
#endif

            void dump() const;
            void disconnect();

            // (De)-serialization
            void serializeToXml(tinyxml2::XMLElement*) const;
            void deserializeFromXml(tinyxml2::XMLElement*);

            void setWhItem(WarehouseItem_t*);
            WarehouseItem_t* getWhItem() const;

            void setWhConn(WarehouseConnection_t*);
            WarehouseConnection_t* getWhConn() const;

            bool isConnected() const;
            int32_t getWhPortID() const;

        protected:
            WarehouseItem_t* whItem{ nullptr };
            WarehouseConnection_t* whConn{ nullptr };

            int32_t whPortID{ 0 };
    };
}