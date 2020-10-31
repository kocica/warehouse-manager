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
#include "Logger.h"
#include "WarehouseLayout.h"
#ifdef WHM_GEN
#  include "WarehouseDataGenerator.h"
#else
#  include "WarehouseSimulator.h"
#  ifdef WHM_OPT
#    include "WarehouseOptimizerGA.h"
#    include "WarehouseOptimizerABC.h"
#    include "WarehouseOptimizerPSO.h"
#  endif
#endif

// Qt
#ifdef WHM_GUI
#  include <QStyle>
#  include <QApplication>
#  include <QDesktopWidget>
#  include "gui/mainwindow.h"
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
                qApp->desktop()->availableGeometry().size() /* * 0.75 */,
                qApp->desktop()->availableGeometry()
            )
        );

        window.show();
#else
        const auto& args = whm::utils::parseArgs(argc, argv);

#  ifdef WHM_GEN
            whm::WarehouseDataGenerator_t{args}.generateData();
#  else
            whm::WarehouseLayout_t::getWhLayout().deserializeFromXml(args.layoutPath);
            whm::WarehouseLayout_t::getWhLayout().importLocationSlots(args.locationsPath);
            whm::WarehouseLayout_t::getWhLayout().importCustomerOrders(args.ordersPath);

#    ifdef WHM_OPT
            // TODO: Decide optimizer using cfg or param

            whm::WarehouseOptimizerBase_t* optimizer = new whm::WarehouseOptimizerPSO_t{args};

            optimizer->optimize();

            delete optimizer;
#    else
            whm::WarehouseSimulator_t::getWhSimulator().setArguments(args);
            whm::WarehouseSimulator_t::getWhSimulator().runSimulation();
#    endif
#  endif
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
