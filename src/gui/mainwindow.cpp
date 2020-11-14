/**
 * Warehouse manager
 *
 * @file    mainwindow.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Entry point of the graphical application
 */

// Standard
#include <iostream>
#include <algorithm>

// Local
#include "UiCursor.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UiWarehouseLayout.h"
#include "UiGraphicsViewZoom.h"
#include "UiWarehouseItemGate.h"
#include "BaseShapeGraphicItem.h"
#include "UiWarehouseItemLocation.h"
#include "UiWarehouseItemConveyor.h"

// Threads
#include "UiWarehouseSimulatorThread.h"
#include "UiWarehouseGeneratorThread.h"
#include "UiWarehouseOptimizerThread.h"

// TUI
#include "../WarehouseItem.h"
#include "../WarehouseOrder.h"
#include "../WarehouseLayout.h"
#include "../WarehouseOrderLine.h"

// Qt
#include <QMenu>
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QFileDialog>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QDialogButtonBox>

namespace whm
{
    namespace gui
    {
        MainWindow::MainWindow(QWidget *parent)
            : QMainWindow(parent)
            , ui(new Ui::MainWindow)
        {
            ui->setupUi(this);

            // Get warehouse dimensions
            auto dialog  = new QDialog(this);
            auto form    = new QFormLayout(dialog);
            auto whDimX  = new QLineEdit(dialog);
            auto whDimY  = new QLineEdit(dialog);
            auto whRatio = new QLineEdit(dialog);

            form->addRow(new QLabel("Enter warehouse dimensions and ratio"));
            form->addRow(QString("Enter X dimenstion [m]: "), whDimX);
            form->addRow(QString("Enter Y dimenstion [m]: "), whDimY);
            form->addRow(QString("Enter ratio 1 [m] = ? [points]: "), whRatio);

            QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
            QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
            QObject::connect(&buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
            form->addRow(&buttons);

            // Create scene
            scene = new CustomizedGraphicsScene_t();

            if (dialog->exec() == QDialog::Accepted)
            {
                whR = whRatio->text().toInt();
                whX = whDimX->text().toInt() * whR;
                whY = whDimY->text().toInt() * whR;
            }
            else
            {
                // Defaults for dev, otherwise show error
                whR = 50;
                whX = 1000 * whR;
                whY = 500  * whR;
            }

            UiWarehouseLayout_t::getWhLayout().setRatio(whR);
            UiWarehouseLayout_t::getWhLayout().setDimensions(std::make_pair(whX/whR, whY/whR));

            scene->setSceneRect(0, 0, whX, whY);
            ui->view->setScene(scene);

            // Create scene borders
            QPen pen;
            pen.setColor(Qt::black);
            pen.setWidth(whX / 100);
            scene->addLine(0,   0,   whX, 0,   pen);
            scene->addLine(0,   0,   0,   whY, pen);
            scene->addLine(0,   whY, whX, whY, pen);
            scene->addLine(whX, 0,   whX, whY, pen);

            // Enable zooming
            ui->ratioIndicator->setFixedWidth(ui->view->width()/5);
            ui->ratioText->setText(QString::number(((ui->view->width()/double(whX)) * (whX/whR))/5));
            auto zoom = new UiGraphicsViewZoom_t(ui->view, ui->ratioText);
            zoom->setModifiers(Qt::NoModifier);

            setContextMenuPolicy(Qt::NoContextMenu);

            ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
            ui->layoutManagement->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
            ui->simulationManagement->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);

            // Icons
            ui->simulationRun->setIcon(QIcon(":/img/start.png"));
            ui->simulationStop->setIcon(QIcon(":/img/stop.png"));
            ui->simulationStep->setIcon(QIcon(":/img/step.png"));

            ui->saveLayout->setIcon(QIcon(":/img/save.png"));
            ui->loadLayout->setIcon(QIcon(":/img/load.png"));
            ui->clearLayout->setIcon(QIcon(":/img/clear.png"));
            ui->importLocations->setIcon(QIcon(":/img/import.png"));
            ui->exportLocations->setIcon(QIcon(":/img/export.png"));

            ui->deletionMode->setIcon(QIcon(":/img/delete.png"));
            ui->selectionMode->setIcon(QIcon(":/img/select.png"));
            ui->selectionMode->toggle();

            // Plots
            ui->fitnessPlot->addGraph();
            ui->fitnessPlot->xAxis->setLabel("Steps");
            ui->fitnessPlot->yAxis->setLabel("Fitness");

            ui->generatorPlot->addGraph();
            ui->generatorPlot->xAxis->setLabel("x");
            ui->generatorPlot->yAxis->setLabel("y");
            ui->generatorPlot->graph(0)->setPen(QPen(QColor(0, 0, 255)));
            ui->generatorPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255)));
            ui->generatorPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);

            ui->generatorPlot->addGraph();
            ui->generatorPlot->xAxis->setLabel("x");
            ui->generatorPlot->yAxis->setLabel("y");
            ui->generatorPlot->graph(1)->setPen(QPen(QColor(255, 0, 0)));
            ui->generatorPlot->graph(1)->setBrush(QBrush(QColor(255, 0, 0)));
            ui->generatorPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDiamond);
        }

        MainWindow::~MainWindow()
        {
            UiWarehouseLayout_t::getWhLayout().clearWhLayout();

            delete ui;
        }

        QPoint MainWindow::getWidgetPosition(QPoint p)
        {
            QPoint ret = ui->frame->mapFromGlobal(p);
            return ret;
        }

        void MainWindow::resizeEvent(QResizeEvent* event)
        {
            QMainWindow::resizeEvent(event);

            ui->ratioIndicator->setFixedWidth(ui->view->width()/5);
            ui->ratioText->setText(QString::number(((ui->view->width()/double(whX)) * (whX/whR))/5));
        }

        void MainWindow::mousePressEvent(QMouseEvent *event)
        {
            if(isOptimizationActive())
            {
                QMessageBox::warning(nullptr, "Warning", "Cannot modify layout while simulation/optimization is running!");
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot modify layout while simulation/optimization is running!");
                return;
            }

            static std::map<UiCursorMode_t, WarehouseItemType_t> convMap =
            {
                { UiCursorMode_t::E_MODE_WH_ITEM_CONV,     WarehouseItemType_t::E_CONVEYOR },
                { UiCursorMode_t::E_MODE_WH_ITEM_CONV_HUB, WarehouseItemType_t::E_CONVEYOR_HUB }
            };

            static std::map<UiCursorMode_t, WarehouseItemType_t> gateMap =
            {
                { UiCursorMode_t::E_MODE_WH_ITEM_ENTRANCE, WarehouseItemType_t::E_WAREHOUSE_ENTRANCE },
                { UiCursorMode_t::E_MODE_WH_ITEM_DISPATCH, WarehouseItemType_t::E_WAREHOUSE_DISPATCH }
            };

            if (event->button() == Qt::LeftButton)
            {
                if (! UiCursor_t::getCursor().isItemSelected())
                {
                    return;
                }

                UiWarehouseItem_t* whItem{ nullptr };
                auto cursorMode = UiCursor_t::getCursor().getMode();

                QPointF loc = QCursor::pos();
                loc = ui->view->mapToScene(loc.x() - ui->mainToolBar->size().width() - ui->frame->window()->pos().x(),
                                           loc.y() - ui->layoutManagement->size().height() - ui->frame->window()->pos().y());

                if (loc.x() < 0 || loc.x() > whX ||
                    loc.y() < 0 || loc.y() > whY)
                {
                    QMessageBox::warning(nullptr, "Warning", "Outside of scene bounds!");
                    Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Outside of scene bounds!");
                    return;
                }

                auto dialog = new QDialog(this);
                auto form   = new QFormLayout(dialog);
                auto width  = new QLineEdit(dialog);
                auto height = new QLineEdit(dialog);

                form->addRow(QString("Enter item width: "), width);
                form->addRow(QString("Enter item height: "), height);

                QDialogButtonBox buttons(QDialogButtonBox::Ok, Qt::Horizontal, dialog);
                QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
                QObject::connect(&buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
                form->addRow(&buttons);

                int32_t w{ 0 };
                int32_t h{ 0 };

                if (dialog->exec() == QDialog::Accepted)
                {
                    w = width->text().toInt() * whR;
                    h = height->text().toInt() * whR;
                }
                else
                {
                    return;
                }

                if(w == 0 || h == 0)
                {
                    QMessageBox::critical(nullptr, "Error", "Invalid dimenstions entered!");
                    return;
                }

                if (cursorMode == UiCursorMode_t::E_MODE_WH_ITEM_LOC)
                {
                    whItem = new UiWarehouseItemLocation_t(scene, this, loc.x(), loc.y(), w, h, WarehouseItemType_t::E_LOCATION_SHELF);
                }
                else if (gateMap.find(cursorMode) != gateMap.end())
                {
                    whItem = new UiWarehouseItemGate_t(scene, this, loc.x(), loc.y(), w, h, gateMap[cursorMode]);
                }
                else if (convMap.find(cursorMode) != convMap.end())
                {
                    whItem = new UiWarehouseItemConveyor_t(scene, this, loc.x(), loc.y(), w, h, convMap[cursorMode]);
                }

                UiWarehouseLayout_t::getWhLayout().addWhItem(whItem);
            }
        }

        void MainWindow::on_deletionMode_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_DELETE);
            }
        }

        void MainWindow::on_selectionMode_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_SELECT);
            }
        }

        void MainWindow::on_whItemConveyor_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_CONV);
            }
        }

        void MainWindow::on_whItemEntrance_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_ENTRANCE);
            }
        }

        void MainWindow::on_whItemExit_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_DISPATCH);
            }
        }

        void MainWindow::on_whItemConveyorHub_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_CONV_HUB);
            }
        }

        void MainWindow::on_whItemLocation_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_LOC);
            }
        }

        bool& MainWindow::isOptimizationActive()
        {
            return this->optimizationActive;
        }

        void MainWindow::optimizationFinished()
        {
            isOptimizationActive() = false;

            steps.clear();
            fitnesses.clear();
        }

        void MainWindow::generatingFinished()
        {
            xadu.clear();
            xorl.clear();
        }

        void MainWindow::optimizationStep(double fitness)
        {
            auto stepCount = steps.size();

            steps.append(stepCount++);
            fitnesses.append(fitness);

            ui->fitnessPlot->graph(0)->setData(steps, fitnesses);
            ui->fitnessPlot->replot();
            ui->fitnessPlot->graph(0)->rescaleAxes(true);

            double max = *std::max_element(fitnesses.begin(), fitnesses.end());

            ui->fitnessPlot->yAxis->setRange(0, max * 1.1);

            ui->fitnessPlot->update();

            ui->elapsedTime->setText(QString::number(optimizationElapsedTime.elapsed() / 1000.0) + " [s]");

            ui->optimizationProgressBar->setValue(100 * stepCount / static_cast<double>(ui->iterations->value()));
        }

        void MainWindow::newGeneratedValue(int value, int graph)
        {
            if(graph == 0)
            {
                xadu.append(value);

                auto [min, max] = std::minmax_element(xadu.begin(), xadu.end());

                double step = (*max - *min) / 10;

                QVector<double> x_hist;
                QVector<double> y_hist;

                for(double x = *min; x < *max; x += step)
                {
                    double c{ 0 };
                    for(auto v : xadu)
                    {
                        if(v >= x && v <= (x + step))
                        {
                            ++c;
                        }
                    }
                    x_hist.push_back(x);
                    y_hist.push_back(c);
                }

                ui->generatorPlot->graph(0)->setData(x_hist, y_hist);
                ui->generatorPlot->replot();
                ui->generatorPlot->graph(0)->rescaleAxes();
            }
            else
            {
                xorl.append(value);

                auto [min, max] = std::minmax_element(xorl.begin(), xorl.end());

                double step = (*max - *min) / 10;

                QVector<double> x_hist;
                QVector<double> y_hist;

                for(double x = *min; x < *max; x += step)
                {
                    double c{ 0 };
                    for(auto v : xorl)
                    {
                        if(v >= x && v <= (x + step))
                        {
                            ++c;
                        }
                    }
                    x_hist.push_back(x);
                    y_hist.push_back(c);
                }

                ui->generatorPlot->graph(1)->setData(x_hist, y_hist);
                ui->generatorPlot->replot();
                ui->generatorPlot->graph(1)->rescaleAxes();
            }

            auto [minadu, maxadu] = std::minmax_element(xadu.begin(), xadu.end());
            auto [minorl, maxorl] = std::minmax_element(xorl.begin(), xorl.end());

            ui->generatorPlot->xAxis->setRange(std::min(*minadu, *minorl) - 10, std::max(*maxadu, *maxorl) + 10);

            ui->generatorPlot->update();

            ui->elapsedTimeGen->setText(QString::number(generationElapsedTime.elapsed() / 1000.0) + " [s]");

            ui->progressBarGen->setValue(100 * xorl.size() / static_cast<double>(ui->orderCount->value()));
        }

        void MainWindow::simulationFinished(double time)
        {
            ui->simulationTime->setText(QString::number(time) + " [s]");
        }

        void MainWindow::on_loadLayout_triggered()
        {
            if(isOptimizationActive())
            {
                QMessageBox::warning(nullptr, "Warning", "Cannot modify layout while simulation/optimization is running!");
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot modify layout while simulation/optimization is running!");
                return;
            }

            QString file = QFileDialog::getOpenFileName(this, tr("Load warehouse layout"), "", tr("Warehouse layouts (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            // Clear previous scene
            UiWarehouseLayout_t::getWhLayout().clearWhLayout();
            ui->view->scene()->clear();
            ui->view->scene()->update();

            ::whm::WarehouseLayout_t::getWhLayout().deserializeFromXml(file.toUtf8().constData());
            ::whm::WarehouseLayout_t::getWhLayout().dump();
            UiWarehouseLayout_t::getWhLayout().initFromTui(this->scene, this, ::whm::WarehouseLayout_t::getWhLayout());
            UiWarehouseLayout_t::getWhLayout().dump();

            int32_t whR = UiWarehouseLayout_t::getWhLayout().getRatio();
            int32_t whX = UiWarehouseLayout_t::getWhLayout().getDimensions().first * whR;
            int32_t whY = UiWarehouseLayout_t::getWhLayout().getDimensions().second * whR;

            ui->view->scene()->setSceneRect(0, 0, whX, whY);

            QPen pen;
            pen.setColor(Qt::black);
            pen.setWidth(whX / 100);
            ui->view->scene()->addLine(0,     0,    whX, 0,   pen);
            ui->view->scene()->addLine(0,     0,    0,   whY, pen);
            ui->view->scene()->addLine(0,     whY,  whX, whY, pen);
            ui->view->scene()->addLine(whX,   0,    whX, whY, pen);

            // Enable zooming
            ui->ratioIndicator->setFixedWidth(ui->view->width()/5);
            ui->ratioText->setText(QString::number(((ui->view->width()/double(whX)) * (whX/whR))/5));
            // TODO: Maybe there will be incorrect start ratio
        }

        void MainWindow::on_saveLayout_triggered()
        {
            QString file = QFileDialog::getSaveFileName(this, tr("Save warehouse layout"), "", tr("Warehouse layouts (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());
            ::whm::WarehouseLayout_t::getWhLayout().dump();
            ::whm::WarehouseLayout_t::getWhLayout().serializeToXml(file.toUtf8().constData());
        }

        void MainWindow::on_clearLayout_triggered()
        {
            if(isOptimizationActive())
            {
                QMessageBox::warning(nullptr, "Warning", "Cannot modify layout while simulation/optimization is running!");
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot modify layout while simulation/optimization is running!");
                return;
            }

            ::whm::WarehouseLayout_t::getWhLayout().clearWhLayout();
            UiWarehouseLayout_t::getWhLayout().clearWhLayout();
        }

        void MainWindow::on_importLocations_triggered()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Import location slots"), "", tr("Article location allocation (*.csv)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());
            ::whm::WarehouseLayout_t::getWhLayout().importLocationSlots(file.toUtf8().constData());

            auto items = ::whm::WarehouseLayout_t::getWhLayout().getWhItems();
            auto uiItems = UiWarehouseLayout_t::getWhLayout().getWhItems();

            for(auto* item : items)
            {
                if(item->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
                {
                    for(auto* uiItem : uiItems)
                    {
                        if(item->getWhItemID() == uiItem->getWhItemID())
                        {
                            dynamic_cast<UiWarehouseItemLocation_t*>(uiItem)->importSlots(*item);
                        }
                    }
                }
            }
        }

        void MainWindow::on_exportLocations_triggered()
        {
            QString file = QFileDialog::getSaveFileName(this, tr("Export location slots"), "", tr("Article location allocation (*.csv)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());
            ::whm::WarehouseLayout_t::getWhLayout().exportLocationSlots(file.toUtf8().constData());
        }

        void MainWindow::on_simulationRun_triggered()
        {
            std::string o = ui->ordersLine->text().toUtf8().constData();
            std::string a = ui->articlesLine->text().toUtf8().constData();
            std::string l = ui->locationsLine->text().toUtf8().constData();

            if(o.empty() || a.empty() || l.empty())
            {
                QMessageBox::warning(nullptr, "Warning", "Load all required data first!");
                return;
            }

            isOptimizationActive() = true;
            whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());

            whm::ConfigParser_t cfg;

            // Set config according to the UI input widgets
            cfg.set("numberDimensions",   std::to_string(ui->numberDimensions->value()));
            cfg.set("problemMin",         std::to_string(ui->problemMin->value()));
            cfg.set("problemMax",         std::to_string(ui->problemMax->value()));
            cfg.set("maxIterations",      std::to_string(ui->iterations->value()));
            cfg.set("saveWeightsPeriod",  std::to_string(ui->weights->value()));
            cfg.set("maxTrialValue",      std::to_string(ui->trialValue->value()));

            cfg.set("populationSize",     std::to_string(ui->populationSize->value()));
            cfg.set("selectionSize",      std::to_string(ui->selectionSize->value()));
            cfg.set("eliteSize",          std::to_string(ui->eliteSize->value()));
            cfg.set("probCrossover",      std::to_string(ui->probCrossover->value()));
            cfg.set("probMutationInd",    std::to_string(ui->probMutationInd->value()));
            cfg.set("probMutationGene",   std::to_string(ui->probMutationGene->value()));

            if(ui->crossoverGa->currentIndex() == 0) cfg.set("crossoverFunctor", "crossoverOrdered");
            if(ui->mutationGa->currentIndex()  == 0) cfg.set("mutationFunctor",  "mutateInverse");
            if(ui->mutationGa->currentIndex()  == 1) cfg.set("mutationFunctor",  "mutateOrdered");
            if(ui->selectionGa->currentIndex() == 0) cfg.set("selectionFunctor", "selectTournam");
            if(ui->selectionGa->currentIndex() == 1) cfg.set("selectionFunctor", "selectTrunc");
            if(ui->selectionGa->currentIndex() == 2) cfg.set("selectionFunctor", "selectRoulette");
            if(ui->selectionGa->currentIndex() == 3) cfg.set("selectionFunctor", "selectRank");

            cfg.set("articlesPath", a);
            cfg.set("locationsPath", l);
            cfg.set("ordersPath", o);

            cfg.dump();

            optimizationElapsedTime.start();

            auto* optimizerUi = new UiWarehouseOptimizerThread_t(cfg);

            connect(optimizerUi, SIGNAL(finished()),
                    optimizerUi, SLOT(deleteLater()));

            connect(optimizerUi, SIGNAL(optimizationFinished()),
                    this,        SLOT(optimizationFinished()));

            connect(optimizerUi, SIGNAL(optimizationStep(double)),
                    this,        SLOT(optimizationStep(double)));

            optimizerUi->start();
        }

        void MainWindow::on_simulationStep_triggered()
        {

        }

        void MainWindow::on_simulationStop_triggered()
        {

        }

        void MainWindow::on_ordersLoad_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Import orders"), "", tr("Orders (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->ordersLine->setText(file);
        }

        void MainWindow::on_articlesLoad_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Import articles"), "", tr("Articles (*.csv)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->articlesLine->setText(file);
        }

        void MainWindow::on_locationsLoad_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Import locations"), "", tr("Locations (*.csv)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->locationsLine->setText(file);
        }

        void MainWindow::on_configLoad_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Optimalizator configuration"), "", tr("Optimalizator configuration (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->configLoadLine->setText(file);

            whm::ConfigParser_t cfg(file.toUtf8().constData());

            ui->numberDimensions->setValue(cfg.getAs<int32_t>("numberDimensions"));
            ui->problemMin->setValue(cfg.getAs<int32_t>("problemMin"));
            ui->problemMax->setValue(cfg.getAs<int32_t>("problemMax"));
            ui->iterations->setValue(cfg.getAs<int32_t>("maxIterations"));
            ui->weights->setValue(cfg.getAs<int32_t>("saveWeightsPeriod"));
            ui->trialValue->setValue(cfg.getAs<int32_t>("maxTrialValue"));

            ui->populationSize->setValue(cfg.getAs<int32_t>("populationSize"));
            ui->selectionSize->setValue(cfg.getAs<int32_t>("selectionSize"));
            ui->eliteSize->setValue(cfg.getAs<int32_t>("eliteSize"));
            ui->probCrossover->setValue(cfg.getAs<double>("probCrossover"));
            ui->probMutationInd->setValue(cfg.getAs<double>("probMutationInd"));
            ui->probMutationGene->setValue(cfg.getAs<double>("probMutationGene"));

            if(cfg.getAs<std::string>("crossoverFunctor") == "crossoverOrdered") ui->crossoverGa->setCurrentIndex(0);
            if(cfg.getAs<std::string>("mutationFunctor")  == "mutateInverse")    ui->mutationGa->setCurrentIndex(0);
            if(cfg.getAs<std::string>("mutationFunctor")  == "mutateOrdered")    ui->mutationGa->setCurrentIndex(1);
            if(cfg.getAs<std::string>("selectionFunctor") == "selectTournam")    ui->selectionGa->setCurrentIndex(0);
            if(cfg.getAs<std::string>("selectionFunctor") == "selectTrunc")      ui->selectionGa->setCurrentIndex(1);
            if(cfg.getAs<std::string>("selectionFunctor") == "selectRoulette")   ui->selectionGa->setCurrentIndex(2);
            if(cfg.getAs<std::string>("selectionFunctor") == "selectRank")       ui->selectionGa->setCurrentIndex(3);
        }

        void MainWindow::on_ordersSaveGen_clicked()
        {
            QString file = QFileDialog::getSaveFileName(this, tr("Save generated orders"), "", tr("Orders (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->ordersLineGen->setText(file);
        }

        void MainWindow::on_articlesLoadGen_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Import articles"), "", tr("Articles (*.csv)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->articlesLineGen->setText(file);
        }

        void MainWindow::on_configLoadGen_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Generator configuration"), "", tr("Generator configuration (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->configLineGen->setText(file);

            whm::ConfigParser_t cfg(file.toUtf8().constData());

            ui->orderCount->setValue(cfg.getAs<int32_t>("orderCount"));
            ui->aduMi->setValue(cfg.getAs<int32_t>("mi"));
            ui->aduSigma->setValue(cfg.getAs<int32_t>("sigma"));
            ui->orlCountMi->setValue(0);
            ui->orlCountSigma->setValue(cfg.getAs<int32_t>("sigmaLines"));
        }

        void MainWindow::on_generateOrders_clicked()
        {
            std::string o = ui->ordersLineGen->text().toUtf8().constData();
            std::string a = ui->articlesLineGen->text().toUtf8().constData();

            if(o.empty() || a.empty())
            {
                QMessageBox::warning(nullptr, "Warning", "Load all required data first!");
                return;
            }

            whm::ConfigParser_t cfg;

            // Set config according to the UI input widgets
            cfg.set("orderCount",   std::to_string(ui->orderCount->value()));
            cfg.set("mi",           std::to_string(ui->aduMi->value()));
            cfg.set("sigma",        std::to_string(ui->aduSigma->value()));
            cfg.set("sigmaLines",   std::to_string(ui->orlCountSigma->value()));

            cfg.set("articlesPath", a);
            cfg.set("ordersPath", o);

            cfg.dump();

            generationElapsedTime.start();

            auto* generatorUi = new UiWarehouseGeneratorThread_t(cfg);

            connect(generatorUi, SIGNAL(finished()),
                    generatorUi, SLOT(deleteLater()));

            connect(generatorUi, SIGNAL(generatingFinished()),
                    this,        SLOT(generatingFinished()));

            connect(generatorUi, SIGNAL(newGeneratedValue(int, int)),
                    this,        SLOT(newGeneratedValue(int, int)));

            generatorUi->start();
        }

        void MainWindow::on_ordersLoadSim_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Import orders"), "", tr("Orders (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->ordersLineSim->setText(file);
        }

        void MainWindow::on_locationsLoadSim_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Import locations"), "", tr("Locations (*.csv)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->locationsLineSim->setText(file);
        }

        void MainWindow::on_configLoadSim_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Simulator configuration"), "", tr("Simulator configuration (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->configLineSim->setText(file);

            whm::ConfigParser_t cfg(file.toUtf8().constData());

            ui->toteSpeed->setValue(cfg.getAs<double>("toteSpeed"));
            ui->workerSpeed->setValue(cfg.getAs<double>("workerSpeed"));
            ui->totesPerMinute->setValue(cfg.getAs<int32_t>("totesPerMin"));
            ui->customerReqInterval->setValue(cfg.getAs<int32_t>("orderRequestInterval"));
            ui->locationCapacity->setValue(cfg.getAs<int32_t>("locationCapacity"));
            ui->conveyorCapacity->setValue(cfg.getAs<int32_t>("conveyorCapacity"));
            ui->simulationSpeedup->setValue(cfg.getAs<double>("simSpeedup"));
            ui->preprocessOrders->setCheckState(cfg.getAs<bool>("preprocess") ? Qt::Checked : Qt::Unchecked);
        }

        void MainWindow::on_simulateWarehouse_clicked()
        {
            std::string o = ui->ordersLineSim->text().toUtf8().constData();
            std::string l = ui->locationsLineSim->text().toUtf8().constData();

            if(o.empty() || l.empty())
            {
                QMessageBox::warning(nullptr, "Warning", "Load all required data first!");
                return;
            }

            whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());

            whm::ConfigParser_t cfg;

            // Set config according to the UI input widgets
            cfg.set("toteSpeed",              std::to_string(ui->toteSpeed->value()));
            cfg.set("workerSpeed",            std::to_string(ui->workerSpeed->value()));
            cfg.set("totesPerMin",            std::to_string(ui->totesPerMinute->value()));
            cfg.set("orderRequestInterval",   std::to_string(ui->customerReqInterval->value()));
            cfg.set("locationCapacity",       std::to_string(ui->locationCapacity->value()));
            cfg.set("conveyorCapacity",       std::to_string(ui->conveyorCapacity->value()));
            cfg.set("simSpeedup",             std::to_string(ui->simulationSpeedup->value()));
            cfg.set("preprocess",             ui->preprocessOrders->checkState() == Qt::Checked ? "true" : "false");

            cfg.set("locationsPath", l);
            cfg.set("ordersPath", o);

            cfg.dump();

            optimizationElapsedTime.start();

            auto* simulatorUi = new UiWarehouseSimulatorThread_t(cfg);

            connect(simulatorUi, SIGNAL(finished()),
                    simulatorUi, SLOT(deleteLater()));

            connect(simulatorUi, SIGNAL(simulationFinished(double)),
                    this,        SLOT(simulationFinished(double)));

            //connect(simulatorUi, SIGNAL(orderSimulationFinished(double)),
            //        this,        SLOT(orderSimulationFinished(double)));

            simulatorUi->start();
        }

        void MainWindow::reset()
        {
            ::whm::WarehouseLayout_t::getWhLayout().clearWhLayout();
            ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());
            UiWarehouseLayout_t::getWhLayout().clearWhLayout();
            UiWarehouseLayout_t::getWhLayout().initFromTui(this->scene, this, ::whm::WarehouseLayout_t::getWhLayout());
        }

        void CustomizedGraphicsScene_t::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
        {
            auto* item = itemAt(event->scenePos().toPoint(), QTransform());

            if(!item)
            {
                return;
            }

            auto* menu = new QMenu(event->widget());

            if(auto* whItem = dynamic_cast<UiWarehouseItem_t*>(item))
            {
                if(whItem->isConnected())
                {
                    QAction* disconnect = new QAction("Disconnect", this);
                    connect(disconnect, &QAction::triggered, [&whItem](){ whItem->disconnect(); });
                    menu->addAction(disconnect);
                }
            }

            menu->exec(event->screenPos());
        }
    }
}
