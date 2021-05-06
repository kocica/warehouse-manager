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
#include "ConfigParser.h"

// Std
#include <map>
#include <vector>
#include <functional>

namespace whm
{
    using WarehouseProduct_t = std::string;

    /// Generator based on gauss distribution
    /// Values mi and sigma are retrieved from configuration file
    class WarehouseDataGenerator_t
    {
        public:
            WarehouseDataGenerator_t(const utils::WhmArgs_t&);
            WarehouseDataGenerator_t(const utils::WhmArgs_t&, const ConfigParser_t&);
            ~WarehouseDataGenerator_t();

            void generateData();

            void dump() const;

#           ifdef WHM_GUI
            using UiCallback_t = std::function<void(int32_t, int32_t)>;
            void setUiCallback(UiCallback_t);
#           endif

        private:
            void generateOrders(double mi, double sigma);

            int32_t sumAdu(const std::map<WarehouseProduct_t, int32_t>&);

            WarehouseProduct_t lookupArticle(double);

        private:
            ConfigParser_t cfg;
            utils::WhmArgs_t args;

            std::map<WarehouseProduct_t, int32_t> whProductsAdu; // Average daily units
            std::map<WarehouseProduct_t, int32_t> whProductsAdq; // Average daily quantity
            std::map<WarehouseProduct_t, double> whProductsProb; // Product selection probability

#           ifdef WHM_GUI
            UiCallback_t uiCallback;
#           endif
    };
}

#endif
