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
#include "WarehouseOrder.h"
#include "WarehouseLayout.h"
#include "WarehouseDataGenerator.h"


namespace whm
{
    WarehouseDataGenerator_t::WarehouseDataGenerator_t(const utils::WhmArgs_t& args_)
        : args{args_}
    {

    }

    WarehouseDataGenerator_t::~WarehouseDataGenerator_t()
    {

    }

    void WarehouseDataGenerator_t::generateData(double mi, double sigma)
    {
        std::random_device rd;   // Source of 'true' randomness for initializing random seed
        std::mt19937 gen(rd());  // Mersenne twister pseudo-random number generator
        std::normal_distribution<> d{mi, sigma}; // Normal (gauss) distribution setup

        // Import products
        std::vector<std::string> whProducts;

        importProducts(whProducts);

        // Generate ADU (average daily units) for each product
        std::map<std::string, int32_t> whProductsAdu;

        std::for_each(whProducts.begin(), whProducts.end(),
                      [&](const std::string& whProduct)
                      {
                          whProductsAdu[whProduct] = std::round(d(gen));
                      });

        // Calculate probability of participation in order for each product
        int32_t adu = sumAdu(whProductsAdu);

        std::for_each(whProducts.begin(), whProducts.end(),
                      [&](const std::string& whProduct)
                      {
                          whProductsProb[whProduct] = whProductsAdu[whProduct] / double(adu);
                      });

        // Generate orders in two (or more) runs with the same product probabilities
        // i.e. one for training second for evaluation
        double ordersMi = adu / args.orderCount;
        double ordersSigma = sigma;

        generateOrders(ordersMi, ordersSigma);
    }

    void WarehouseDataGenerator_t::importProducts(std::vector<std::string>& whProducts)
    {
        std::ifstream csvStream;
        csvStream.open(args.articlesPath);

        // Ignore header
        std::string header;
        std::getline(csvStream, header);

        std::string dummy, article;

        while(csvStream)
        {
            std::string dummy, article;

            std::getline(csvStream, dummy,   ';');
            std::getline(csvStream, dummy,   ';');
            std::getline(csvStream, dummy,   ';');
            std::getline(csvStream, article, ';');
            std::getline(csvStream, dummy);

            if(!article.empty())
            {
                if(std::find(whProducts.begin(), whProducts.end(), article) == whProducts.end())
                {
                    whProducts.push_back(std::move(article));
                }
            }
        }

        csvStream.close();
    }

    void WarehouseDataGenerator_t::generateOrders(double mi, double sigma)
    {
        auto& layout = WarehouseLayout_t::getWhLayout();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> d{mi, sigma};

        for(int32_t orderID = 0; orderID < args.orderCount; ++orderID)
        {
            using U = std::string;
            WarehouseOrder_t<U> order;
            std::vector<WarehouseOrderLine_t<U>> lines;

            order.setWhOrderID(orderID);

            for(int32_t lineID = 0; lineID < std::round(d(gen)); ++lineID)
            {
                WarehouseOrderLine_t<U> line(order);

                // Insert products

                lines.emplace_back(std::move(line));
            }

            order.setWhOrderLines(lines);
            layout.addWhOrder(order);
        }
    }

    int32_t WarehouseDataGenerator_t::sumAdu(const std::map<std::string, int32_t>& whProductsAdu)
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
            std::cout << std::setw(32) << whProd.first << " = " << whProd.second << std::endl;
        }
    }
}

#endif
