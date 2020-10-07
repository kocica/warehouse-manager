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
    class WarehouseLayout_t;

    namespace gui
    {
        class UiWarehouseLayout_t
        {
            using UiWarehouseItemContainer_t = std::vector<UiWarehouseItem_t*>;

            public:
                UiWarehouseLayout_t();
                ~UiWarehouseLayout_t() = default;

                static UiWarehouseLayout_t& getWhLayout();

                UiWarehouseItemContainer_t getWhItems() const;

                void addWhItem(UiWarehouseItem_t*);
                void eraseWhItem(UiWarehouseItem_t*);

                void dump() const;

                size_t getWhItemCount() const;
                int32_t getNextWhItemID();

                void deleteAllWhItems();

                bool itemsIntersects(UiWarehouseItem_t*) const;

                void initFromTui(QGraphicsScene*, MainWindow*, ::whm::WarehouseLayout_t&);

            private:
                int32_t whItemIdSequence{ 0 };
                UiWarehouseItemContainer_t whItems;
        };
    }
}