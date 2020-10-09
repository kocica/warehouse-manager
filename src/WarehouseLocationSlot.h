/**
 * Warehouse manager
 *
 * @file    WarehouseLocationSlot.h
 * @date    10/09/2020
 * @author  Filip Kocica
 * @brief   Class representing location slot containing product(s)
 */

#pragma once

namespace whm
{
    template<typename T>
    class WarehouseLocationSlot_t
    {
        public:
            WarehouseLocationSlot_t();
            ~WarehouseLocationSlot_t();

            T getArticle() const;
            void setArticle(const T&);

            size_t getQuantity() const;
            void setQuantity(const size_t);

            void dump() const;

        private:
            T article;
            size_t quantity{ 0 };
    };
}