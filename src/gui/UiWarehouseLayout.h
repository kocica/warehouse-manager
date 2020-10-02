/**
 * Warehouse manager
 *
 * @file    UiWarehouseLayout.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse layout
 */

#pragma once

#include "UiWarehouseItem.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseLayout_t
        {
            using WarehouseItemContainer_t = std::vector<UiWarehouseItem_t*>;

            public:
                UiWarehouseLayout_t();
                virtual ~UiWarehouseLayout_t() = default;

                static UiWarehouseLayout_t& getWhLayout();

                void addWhItem(UiWarehouseItem_t*);
                void eraseWhItem(UiWarehouseItem_t*);

                void dump();

                size_t getWhItemCount() const;
                int32_t getNextWhItemID();

                bool itemsIntersects(UiWarehouseItem_t*) const;

            private:
                int32_t whItemIdSequence{ 0 };
                WarehouseItemContainer_t whItems;
        };
    }
}