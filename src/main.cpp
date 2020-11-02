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
#    include "WarehouseOptimizerDE.h"
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
            whm::WarehouseOptimizerBase_t* optimizer{ nullptr };

            switch(args.optimizer)
            {
                case 1: optimizer = new whm::WarehouseOptimizerGA_t{args};  break;
                case 2: optimizer = new whm::WarehouseOptimizerDE_t{args};  break;
                case 3: optimizer = new whm::WarehouseOptimizerABC_t{args}; break;
                case 4: optimizer = new whm::WarehouseOptimizerPSO_t{args}; break;
                default: whm::Logger_t::getLogger().print(LOG_LOC, whm::LogLevel_t::E_ERROR, "Unknown optimizer"); return 1;
            }

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
