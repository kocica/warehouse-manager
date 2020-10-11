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
        auto args = whm::utils::ArgsParser_t{}.parseArgs(argc, argv);

        whm::WarehouseLayout_t::getWhLayout().deserializeFromXml("./data/layout.xml");
        whm::WarehouseLayout_t::getWhLayout().importLocationSlots("./data/products.csv");
        whm::WarehouseLayout_t::getWhLayout().importCustomerOrders("./data/orders.xml");

        whm::WarehouseSimulator_t{}.runSimulation();
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
