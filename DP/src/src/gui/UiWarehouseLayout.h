/**
 * Warehouse manager
 *
 * @file    UiWarehouseLayout.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse layout
 */

#pragma once

#include "../Logger.h"
#include "UiWarehouseItem.h"
#include "UiWarehouseConnection.h"

namespace whm
{
    class WarehouseLayout_t;

    namespace gui
    {
        class UiWarehouseLayout_t
        {
            using WarehouseDimensions_t = std::pair<int32_t, int32_t>;
            using UiWarehouseItemContainer_t = std::vector<UiWarehouseItem_t*>;
            using UiWarehouseConnContainer_t = std::vector<UiWarehouseConnection_t*>;

            public:
                UiWarehouseLayout_t();
                ~UiWarehouseLayout_t();

                static UiWarehouseLayout_t& getWhLayout();

                UiWarehouseItemContainer_t getWhItems() const;
                UiWarehouseConnContainer_t getWhConns() const;

                UiWarehouseItem_t* findWhItemByID(int32_t) const;

                void addWhItem(UiWarehouseItem_t*);
                void eraseWhItem(UiWarehouseItem_t*);

                void addWhConn(UiWarehouseConnection_t*);
                void eraseWhConn(UiWarehouseConnection_t*);

                int32_t getNextWhConnID();
                int32_t getNextWhItemID();

                int32_t getRatio() const;
                WarehouseDimensions_t getDimensions() const;

                void setRatio(int32_t);
                void setDimensions(const WarehouseDimensions_t&);

                void clearWhLayout();
                bool itemsIntersects(UiWarehouseItem_t*) const;
                void initFromTui(QGraphicsScene*, MainWindow*, ::whm::WarehouseLayout_t&);
                void dump() const;

            private:
                int32_t whRatio;
                WarehouseDimensions_t whDims;

                int32_t whItemIdSequence{ 0 };
                UiWarehouseItemContainer_t whItems;

                int32_t whConnIdSequence{ 0 };
                UiWarehouseConnContainer_t whConns;
        };
    }
}