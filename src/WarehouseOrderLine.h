/**
 * Warehouse manager
 *
 * @file    WarehouseOrderLine.h
 * @date    11/09/2020
 * @author  Filip Kocica
 * @brief   Class representing one customer order line (product + requested quantity)
 */

#pragma once

namespace whm
{
    template<typename>
    class WarehouseOrder_t;

    template<typename T>
    class WarehouseOrderLine_t
    {
        public:
            WarehouseOrderLine_t(const WarehouseOrder_t<T>&);
            ~WarehouseOrderLine_t();

            T getArticle() const;
            void setArticle(const T&);

            int32_t getQuantity() const;
            void setQuantity(int32_t);

            WarehouseOrder_t<T> getWhOrder() const;

            void setWhLineID(int32_t);
            int32_t getWhLineID() const;

            void deserializeFromXml(tinyxml2::XMLElement*);

            void dump() const;

        private:
            T article;
            int32_t whLineID{ 0 };
            int32_t quantity{ 0 };
            WarehouseOrder_t<T> whOrder;
    };
}
