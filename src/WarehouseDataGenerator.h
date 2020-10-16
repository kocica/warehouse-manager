/**
 * Warehouse manager
 *
 * @file    WarehouseDataGenerator.h
 * @date    10/15/2020
 * @author  Filip Kocica
 * @brief   Module for generating of customer order data based on ADU and gauss distirbution
 */

#pragma once

#ifdef WHM_GEN

// Local
#include "Utils.h"

// Std
#include <map>
#include <vector>

namespace whm
{
    class WarehouseDataGenerator_t
    {
        public:
            WarehouseDataGenerator_t(const utils::WhmArgs_t&);
            ~WarehouseDataGenerator_t();

            /// Gauss distribution generator
            /// @param mi mean value
            /// @param sigma std. deviation
            void generateData(double mi, double sigma);

            void dump() const;

        private:
            void generateOrders(double mi, double sigma);

            void importProducts(std::vector<std::string>&);

            int32_t sumAdu(const std::map<std::string, int32_t>&);

        private:
            utils::WhmArgs_t args;
            std::map<std::string, double> whProductsProb;
    };
}

#endif
