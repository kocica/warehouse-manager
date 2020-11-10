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
    class WarehouseLocationRack_t;

    class WarehouseLocationSlot_t
    {
        public:
            WarehouseLocationSlot_t();
            ~WarehouseLocationSlot_t();

            bool isOccupied() const;

            std::string getArticle() const;
            void setArticle(const std::string&);

            int32_t getQuantity() const;
            void setQuantity(const int32_t);

            std::pair<size_t, size_t> getCoords() const;
            void setCoords(const std::pair<size_t, size_t>&);

            WarehouseLocationRack_t* getWhLocRack() const;
            void setWhLocRack(WarehouseLocationRack_t*);

            void exportSlot(std::ostream&) const;
            void importSlot(std::istream&);

            void dump() const;

        private:
            std::string article;
            size_t x{ 0 };
            size_t y{ 0 };
            int32_t quantity{ 0 };
            WarehouseLocationRack_t* whLocRack{ nullptr };
    };
}