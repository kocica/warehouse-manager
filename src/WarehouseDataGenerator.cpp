/**
 * Warehouse manager
 *
 * @file    WarehouseDataGenerator.cpp
 * @date    10/15/2020
 * @author  Filip Kocica
 * @brief   Module for generating of customer order data based on ADU and gauss distirbution
 */

#ifdef WHM_GEN

// Std
#include <cmath>
#include <random>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <algorithm>

// Local
#include "Logger.h"
#include "WarehouseOrder.h"
#include "WarehouseLayout.h"
#include "WarehouseDataGenerator.h"


namespace whm
{
    WarehouseDataGenerator_t::WarehouseDataGenerator_t(const utils::WhmArgs_t& args_)
        : cfg{ ConfigParser_t{ "cfg/generator.xml" } }
        , args{ args_ }
    {

    }

    WarehouseDataGenerator_t::~WarehouseDataGenerator_t()
    {

    }

    void WarehouseDataGenerator_t::generateData()
    {
        double mi = cfg.getAs<double>("mi");
        double sigma = cfg.getAs<double>("sigma");

        std::random_device rd;   // Source of 'true' randomness for initializing random seed
        std::mt19937 gen(rd());  // Mersenne twister pseudo-random number generator
        std::normal_distribution<> normalDist{mi, sigma}; // Normal (gauss) distribution setup

        // Import products
        std::vector<WarehouseProduct_t> whProducts;

        WarehouseLayout_t::getWhLayout().importArticles(args.articlesPath, whProducts);

        // Generate ADU (average daily units) for each product
        std::map<WarehouseProduct_t, int32_t> whProductsAdu;

        std::for_each(whProducts.begin(), whProducts.end(),
                      [&](const WarehouseProduct_t& whProduct)
                      {
                          whProductsAdu[whProduct] = std::round(normalDist(gen));
                      });

        for(auto& whProdAdu : whProductsAdu)
        {
            Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "Product <%s> = ADU <%d>", whProdAdu.first.c_str(), whProdAdu.second);
        }

        // Calculate probability of participation in order for each product
        int32_t adu = sumAdu(whProductsAdu);
        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "ADU: <%d>", adu);

        std::for_each(whProducts.begin(), whProducts.end(),
                      [&](const WarehouseProduct_t& whProduct)
                      {
                          whProductsProb[whProduct] = whProductsAdu[whProduct] / double(adu);
                      });

        // Generate orders in two (or more) runs with the same product probabilities for training and evaluation
        double ordersMi = double(adu) / cfg.getAs<int32_t>("orderCount");
        double ordersSigma = cfg.getAs<int32_t>("sigmaLines");

        dump();
        generateOrders(ordersMi, ordersSigma);
    }

    void WarehouseDataGenerator_t::generateOrders(double mi, double sigma)
    {
        auto& layout = WarehouseLayout_t::getWhLayout();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> normalDist{mi, sigma};
        std::uniform_real_distribution<> uniformDist{0, 1};
        std::vector<std::string> suffixes = { "_train", "_test" };

        // Generate set of orders for each suffix
        for(auto& suffix : suffixes)
        {
            layout.clearWhOrders();

            for(int32_t orderID = 0; orderID < cfg.getAs<int32_t>("orderCount"); ++orderID)
            {
                WarehouseOrder_t<WarehouseProduct_t> order;
                std::vector<WarehouseOrderLine_t<WarehouseProduct_t>> lines;

                order.setWhOrderID(orderID);

                // TODO: Accumulate error from round
                int32_t lineCount = std::round(normalDist(gen));

                for(int32_t lineID = 0; lineID < std::max(1, lineCount); ++lineID)
                {
                    WarehouseOrderLine_t<WarehouseProduct_t> line(order);

                    double prob = uniformDist(gen);

                    line.setWhLineID(lineID);
                    line.setArticle(lookupArticle(prob));

                    lines.emplace_back(std::move(line));
                }

                order.setWhOrderLines(lines);
                layout.addWhOrder(order);
            }

            std::string savePath = args.ordersPath;
            savePath.replace(savePath.find("."), 0, suffix);
            layout.exportCustomerOrders(savePath);
        }
    }

    WarehouseProduct_t WarehouseDataGenerator_t::lookupArticle(double prob)
    {
        double actProb{ 0. };

        for(auto& whProd : whProductsProb)
        {
            if(prob > actProb &&
               prob < actProb + whProd.second)
            {
                return whProd.first;
            }

            actProb += whProd.second;
        }

        return WarehouseProduct_t("---"); // Never gonna happen but shut up compiler
    }

    int32_t WarehouseDataGenerator_t::sumAdu(const std::map<WarehouseProduct_t, int32_t>& whProductsAdu)
    {
        int32_t adu{ 0 };

        for(auto& whProductAdu : whProductsAdu)
        {
            adu += whProductAdu.second;
        }

        return adu;
    }

    void WarehouseDataGenerator_t::dump() const
    {
        for(auto& whProd : whProductsProb)
        {
            Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "Product <%s> = Probability <%f>", whProd.first.c_str(), whProd.second);
        }
    }
}

#endif
