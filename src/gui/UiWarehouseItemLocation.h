/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemLocation.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse location
 */

#pragma once

// Local
#include "../Logger.h"
#include "UiWarehouseItem.h"
#include "UiWarehousePort.h"
#include "UiWarehouseSlot.h"

namespace whm
{
    class WarehouseItem_t;

    namespace gui
    {
        class UiWarehouseItemLocation_t : public UiWarehouseItem_t
        {
            public:
                // Construct from GUI
                UiWarehouseItemLocation_t(QGraphicsScene*, MainWindow*, int32_t, int32_t, int32_t, int32_t, WarehouseItemType_t);
                // Construct from TUI
                UiWarehouseItemLocation_t(QGraphicsScene*, MainWindow*, ::whm::WarehouseItem_t&);

                void importSlots(::whm::WarehouseItem_t&);
                std::vector<UiWarehouseSlot_t*> getSlots() const;

                ~UiWarehouseItemLocation_t() = default;

                int32_t getSlotCountX() const { return this->slotsX; }
                int32_t getSlotCountY() const { return this->slotsY; }
                void setDimensions(int32_t x, int32_t y) { slotsX = x; slotsY = y; }

                virtual void updateChildrenPositions(double, double) override;

            private:
                friend class UiWarehouseSlot_t;

                int32_t slotsX{ 0 };
                int32_t slotsY{ 0 };

                std::vector<UiWarehouseSlot_t*> whSlots;
        };
    }
}