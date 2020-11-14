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

// Qt
#include <QMainWindow>
#include <QElapsedTimer>
#include <QGraphicsScene>

namespace Ui
{
    class MainWindow;
}

namespace whm
{
    namespace gui
    {
        class CustomizedGraphicsScene_t : public QGraphicsScene
        {
            void contextMenuEvent(QGraphicsSceneContextMenuEvent*);
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

                bool& isOptimizationActive();

            public slots:
                void optimizationFinished();
                void optimizationStep(double);
                void generatingFinished();
                void newGeneratedValue(int, int);
                void simulationFinished(double);
                void orderSimulationFinished(double);

            private slots:
                void mousePressEvent(QMouseEvent *);

                // Toolbar
                void on_loadLayout_triggered();
                void on_saveLayout_triggered();
                void on_clearLayout_triggered();
                void on_importLocations_triggered();
                void on_exportLocations_triggered();
                void on_simulationRun_triggered();
                void on_simulationStep_triggered();
                void on_simulationStop_triggered();

                void on_deletionMode_toggled(bool);
                void on_selectionMode_toggled(bool);
                void on_whItemLocation_toggled(bool);
                void on_whItemConveyor_toggled(bool);
                void on_whItemConveyorHub_toggled(bool);
                void on_whItemEntrance_toggled(bool);
                void on_whItemExit_toggled(bool);

                // Optimizer
                void on_ordersLoad_clicked();
                void on_articlesLoad_clicked();
                void on_locationsLoad_clicked();
                void on_configLoad_clicked();
                //void on_optimizeWarehouse_clicked();

                // Generator
                void on_ordersSaveGen_clicked();
                void on_articlesLoadGen_clicked();
                void on_configLoadGen_clicked();
                void on_generateOrders_clicked();

                // Simulator
                void on_ordersLoadSim_clicked();
                void on_locationsLoadSim_clicked();
                void on_configLoadSim_clicked();
                void on_simulateWarehouse_clicked();

            private:
                int32_t whR{ 0 };
                int32_t whX{ 0 };
                int32_t whY{ 0 };

                Ui::MainWindow* ui;
                CustomizedGraphicsScene_t* scene;

                QElapsedTimer generationElapsedTime;
                QElapsedTimer optimizationElapsedTime;

                bool optimizationActive{ false };

                QVector<double> xadu;
                QVector<double> xorl;

                QVector<double> steps;
                QVector<double> fitnesses;

                QVector<double> orders;
                QVector<double> processingDurations;
        };
    }
}
