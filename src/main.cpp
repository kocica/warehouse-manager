/**
 * Warehouse manager
 *
 * @file    main.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Entry point of the application
 */

// Std
#include <iostream>

// Local
#include "Utils.h"
#include "WarehouseLayout.h"
#include "WarehouseSimulator.h"

// Qt
#ifdef WHM_GUI
#include <QStyle>
#include <QApplication>
#include <QDesktopWidget>
#include "gui/mainwindow.h"
#endif

int main(int argc, char *argv[])
{
    try
    {
#ifdef WHM_GUI
        QApplication app(argc, argv);

        whm::gui::MainWindow window;

        window.setGeometry(
            QStyle::alignedRect(
                Qt::LeftToRight,
                Qt::AlignLeft,
                QSize{1, 1},
                qApp->desktop()->availableGeometry()
            )
        );

        window.show();
#else
        const auto& args = whm::utils::parseArgs(argc, argv);

        whm::WarehouseLayout_t::getWhLayout().deserializeFromXml(args.layoutPath);
        whm::WarehouseLayout_t::getWhLayout().importLocationSlots(args.productsPath);
        whm::WarehouseLayout_t::getWhLayout().importCustomerOrders(args.ordersPath);

        whm::WarehouseSimulator_t::getWhSimulator().setArguments(args);
        whm::WarehouseSimulator_t::getWhSimulator().runSimulation();
#endif

#ifdef WHM_GUI
        return app.exec();
#else
        return 0;
#endif
    }
    catch(std::exception& e)
    {
        std::cerr << "Unexpected problem has occured during the simulation ("
                  << e.what() << ")" << std::endl;
    }
}
