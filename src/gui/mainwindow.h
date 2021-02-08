/**
 * Warehouse manager
 *
 * @file    mainwindow.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Entry point of the graphical application
 */

#pragma once

// Local
#include "../Logger.h"
#include "UiWarehouseGeneratorThread.h"
#include "UiWarehouseSimulatorThread.h"
#include "UiWarehouseOptimizerThread.h"

// Qt
#include <QMainWindow>
#include "qcustomplot.h"
#include <QElapsedTimer>
#include <QGraphicsScene>
#include <QStandardItemModel>

namespace Ui
{
    class MainWindow;
}

Q_DECLARE_METATYPE(std::string);
Q_DECLARE_METATYPE(whm::gui::SimulationStats_t);

namespace whm
{
    class ConfigParser_t;

    namespace gui
    {
        class MainWindow;

        class CustomizedGraphicsScene_t : public QGraphicsScene
        {
            public:
                void setUi(MainWindow* ui_) { ui = ui_; }
            protected:
                void keyPressEvent(QKeyEvent*);
                void mousePressEvent(QGraphicsSceneMouseEvent*);
                void contextMenuEvent(QGraphicsSceneContextMenuEvent*);
            private:
                MainWindow* ui;
        };

        class MainWindow : public QMainWindow
        {
            Q_OBJECT

            public:
                explicit MainWindow(QWidget *parent = Q_NULLPTR);
                virtual ~MainWindow();

                void resizeEvent(QResizeEvent*);

                Ui::MainWindow* getUi() const { return ui; }

                void reset();

                QPoint getWidgetPosition(QPoint);

                void resetCursor();
                void importLocations();
                void showWhItemsWorkload();

                bool& isOptimizationActive();

            public slots:
                // Optimizer slots
                void optimizationFinished();
                void optimizationStep(double);

                // Generator slots
                void generatingFinished(std::string);
                void newGeneratedValue(int, int);

                // Simulator slots
                void simulationFinished(SimulationStats_t);
                void orderSimulationFinished(SimulationStats_t);

            private slots:
                void on_simulationSpeedup_valueChanged();
                void on_optimizersConfigsTab_currentChanged();
                void on_optimizerSelectionBox_currentIndexChanged();

                // Toolbar - layout control
                void on_newLayout_triggered();
                void on_loadLayout_triggered();
                void on_saveLayout_triggered();
                void on_clearLayout_triggered();

                // Toolbar - workspace control
                void on_deletionMode_toggled(bool);
                void on_selectionMode_toggled(bool);
                void on_whItemLocation_toggled(bool);
                void on_whItemConveyor_toggled(bool);
                void on_whItemConveyorHub_toggled(bool);
                void on_whItemEntrance_toggled(bool);
                void on_whItemExit_toggled(bool);
                void on_whItemBuffer_toggled(bool);

                // Optimizer
                void on_weightsLoad_clicked();
                void on_configLoadOpt_clicked();
                void on_configSaveOpt_clicked();
                void on_startOptimization_clicked();
                void on_stopOptimization_clicked();

                // Generator
                void on_configLoadGen_clicked();
                void on_configSaveGen_clicked();
                void on_startGenerating_clicked();
                void on_stopGenerating_clicked();

                // Simulator
                void on_configLoadSim_clicked();
                void on_configSaveSim_clicked();
                void on_startSimulation_clicked();
                void on_stopSimulation_clicked();

                // Path Finder
                void on_startPathFinder_clicked();
                void on_stopPathFinder_clicked();

                // Data - test orders
                void on_clearTestOrders_clicked();
                void on_clearTrainOrders_clicked();
                void on_importTestOrders_clicked();

                // Data - train orders
                void on_importTrainOrders_clicked();
                void on_exportTestOrders_clicked();
                void on_exportTrainOrders_clicked();

                // Data - articles
                void on_clearArticles_clicked();
                void on_importArticles_clicked();

                // Data - locations
                void on_clearLocations_clicked();
                void on_importLocations_clicked();
                void on_exportLocations_clicked();

            protected:
                std::string exportArticles();
                std::string exportLocations();
                std::string exportOrders(bool, std::string f = "");

                void importOrders(bool, std::string f = "");

                void stylePlot(QCustomPlot*);

                void enableManager();
                void disableManager();

                void createSceneBorders();
                void updateWarehouseDimensions();

                void exportConfig(std::function<void(ConfigParser_t&)>);
                void exportOptimizerConfig(ConfigParser_t&);
                void exportGeneratorConfig(ConfigParser_t&);
                void exportSimulatorConfig(ConfigParser_t&);

            private:
                int32_t whR{ 0 };
                int32_t whX{ 0 };
                int32_t whY{ 0 };
                int32_t orderCount{ 0 };

                Ui::MainWindow* ui;
                CustomizedGraphicsScene_t* scene;

                QElapsedTimer generationElapsedTime;
                QElapsedTimer optimizationElapsedTime;

                QStandardItemModel* articlesModel{ nullptr };
                QStandardItemModel* locationsModel{ nullptr };
                QStandardItemModel* ordersTestModel{ nullptr };
                QStandardItemModel* ordersTrainModel{ nullptr };

                UiWarehouseOptimizerThread_t* optimizerUi{ nullptr };
                UiWarehouseGeneratorThread_t* generatorUi{ nullptr };
                UiWarehouseSimulatorThread_t* simulatorUi{ nullptr };

                bool optimizationActive{ false };

                QVector<double> xadu;
                QVector<double> xadq;
                QVector<double> xorl;

                QVector<double> steps;
                QVector<double> fitnesses;

                QVector<double> orders;
                QVector<double> distancesConvs;
                QVector<double> distancesWorkers;
                QVector<double> processingDurations;

                const std::string tmpDir       = ".tmp";
                const std::string tmpOrders    = ".tmp/orders.xml";
                const std::string tmpArticles  = ".tmp/articles.csv";
                const std::string tmpLocations = ".tmp/locations.csv";
        };
    }
}
