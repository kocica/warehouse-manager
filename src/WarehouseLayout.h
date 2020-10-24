/**
 * Warehouse manager
 *
 * @file    WarehouseLayout.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class managing whole warehouse layout (locations, connections, ...)
 */

#pragma once

#include <vector>
#include <utility>

#ifdef WHM_GUI
namespace whm
{
    namespace gui
    {
        class UiWarehouseLayout_t;
    }
}
#endif

namespace whm
{
    class WarehouseItem_t;
    class WarehouseConnection_t;
    template<typename>
    class WarehouseOrder_t;

    class WarehouseLayout_t
    {
        using WarehouseDimensions_t = std::pair<int32_t, int32_t>;
        using WarehouseItemContainer_t = std::vector<WarehouseItem_t*>;
        using WarehouseConnContainer_t = std::vector<WarehouseConnection_t*>;
        using WarehouseOrderContainer_t = std::vector<WarehouseOrder_t<std::string>>;

        public:
            WarehouseLayout_t();
            ~WarehouseLayout_t();

            static WarehouseLayout_t& getWhLayout();

#ifdef WHM_GUI
            void initFromGui(gui::UiWarehouseLayout_t&);
#endif

            // Layout (de-)serialization
            void serializeToXml(const std::string&);
            void deserializeFromXml(const std::string&);

            // Location Import/Export
            void importLocationSlots(const std::string&);
            void exportLocationSlots(const std::string&);

            // Customer order import
            void importCustomerOrders(const std::string&);
            void exportCustomerOrders(const std::string&);

            // Import articles
            void importArticles(const std::string&, std::vector<std::string>&);

            const WarehouseItemContainer_t& getWhItems() const;
            const WarehouseConnContainer_t& getWhConns() const;
            const WarehouseOrderContainer_t& getWhOrders() const;

            void addWhItem(WarehouseItem_t*);
            void addWhConn(WarehouseConnection_t*);
            void addWhOrder(const WarehouseOrder_t<std::string>&);

            void eraseWhItem(WarehouseItem_t*);
            void eraseWhConn(WarehouseConnection_t*);
            void eraseWhOrder(const WarehouseOrder_t<std::string>&);

            void dump() const;
            void clearWhOrders();
            void clearWhLayout();

            int32_t getRatio() const;
            WarehouseDimensions_t getDimensions() const;

        protected:
            int32_t whRatio;
            WarehouseDimensions_t whDims;

            WarehouseItemContainer_t whItems;
            WarehouseConnContainer_t whConns;
            WarehouseOrderContainer_t whOrders;
    };
}