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
    class WarehouseOrder_t;

    class WarehouseOrderLine_t
    {
        public:
            WarehouseOrderLine_t(WarehouseOrder_t*);
            ~WarehouseOrderLine_t();

            std::string getArticle() const;
            void setArticle(const std::string&);

            int32_t getQuantity() const;
            void setQuantity(int32_t);

            WarehouseOrder_t& getWhOrder() const;

            void setWhLineID(int32_t);
            int32_t getWhLineID() const;

            void deserializeFromXml(tinyxml2::XMLElement*);
            void serializeToXml(tinyxml2::XMLElement*) const;

            void dump() const;

        private:
            std::string article;
            int32_t whLineID{ 0 };
            int32_t quantity{ 0 };
            WarehouseOrder_t* whOrder{ nullptr };
    };
}
