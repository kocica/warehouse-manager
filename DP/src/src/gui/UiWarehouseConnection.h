/**
 * Warehouse manager
 *
 * @file    UiWarehouseConnection.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse connection between ports
 */

#pragma once

#include "../Logger.h"
#include "mainwindow.h"

namespace whm
{
    class WarehouseConnection_t;

    namespace gui
    {
        class UiWarehousePort_t;

        class UiWarehouseConnection_t
        {
            public:
                // Construct from GUI
                UiWarehouseConnection_t(UiWarehousePort_t*, UiWarehousePort_t*);
                // Construct from TUI
                UiWarehouseConnection_t(::whm::WarehouseConnection_t&);

                ~UiWarehouseConnection_t();

                int32_t getWhConnID() const;
                UiWarehousePort_t* getTo() const;
                UiWarehousePort_t* getFrom() const;

                void dump() const;
                UiWarehousePort_t* lookupPort(int32_t, int32_t);

            private:
                UiWarehousePort_t* to{ nullptr };
                UiWarehousePort_t* from{ nullptr };

                int32_t whConnID{ 0 };
        };
    }
}