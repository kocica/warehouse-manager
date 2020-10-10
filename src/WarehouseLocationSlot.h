/**
 * Warehouse manager
 *
 * @file    WarehouseLocationSlot.h
 * @date    10/09/2020
 * @author  Filip Kocica
 * @brief   Class representing location slot containing product(s)
 */

#pragma once

#include <utility>

namespace whm
{
    template<typename>
    class WarehouseLocationRack_t;

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

            std::pair<size_t, size_t> getCoords() const;
            void setCoords(const std::pair<size_t, size_t>&);

            WarehouseLocationRack_t<T>* getWhLocRack() const;
            void setWhLocRack(WarehouseLocationRack_t<T>*);

            void exportSlot(std::ostream&) const;
            void importSlot(std::istream&);

            void dump() const;

        private:
            T article;
            size_t x{ 0 };
            size_t y{ 0 };
            size_t quantity{ 0 };
            WarehouseLocationRack_t<T>* whLocRack{ nullptr };
    };
}