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
#include <experimental/filesystem>

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
#include "../WarehouseLocationRack.h"

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
        namespace fs = std::experimental::filesystem;

        MainWindow::MainWindow(QWidget *parent)
            : QMainWindow(parent)
            , ui(new Ui::MainWindow)
            , articlesModel(new QStandardItemModel())
            , locationsModel(new QStandardItemModel())
            , ordersTestModel(new QStandardItemModel())
            , ordersTrainModel(new QStandardItemModel())
        {
            ui->setupUi(this);

            QFile f(":qdarkstyle/style.qss");

            if(f.exists())
            {
                f.open(QFile::ReadOnly | QFile::Text);
                QTextStream ts(&f);
                qApp->setStyleSheet(ts.readAll());
            }

            qRegisterMetaType<std::string>();

            fs::create_directory(tmpDir);

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

            // Icons
            ui->startOptimization->setIcon(QIcon(":/img/start.png"));
            ui->stopOptimization->setIcon(QIcon(":/img/stop.png"));
            ui->startGenerating->setIcon(QIcon(":/img/start.png"));
            ui->stopGenerating->setIcon(QIcon(":/img/stop.png"));
            ui->startSimulation->setIcon(QIcon(":/img/start.png"));
            ui->stopSimulation->setIcon(QIcon(":/img/stop.png"));

            ui->saveLayout->setIcon(QIcon(":/img/save.png"));
            ui->loadLayout->setIcon(QIcon(":/img/load.png"));
            ui->clearLayout->setIcon(QIcon(":/img/clear.png"));

            ui->clearTestOrders->setIcon(QIcon(":/img/clear.png"));
            ui->importTestOrders->setIcon(QIcon(":/img/import.png"));
            ui->exportTestOrders->setIcon(QIcon(":/img/export.png"));
            ui->clearTrainOrders->setIcon(QIcon(":/img/clear.png"));
            ui->importTrainOrders->setIcon(QIcon(":/img/import.png"));
            ui->exportTrainOrders->setIcon(QIcon(":/img/export.png"));
            ui->clearArticles->setIcon(QIcon(":/img/clear.png"));
            ui->importArticles->setIcon(QIcon(":/img/import.png"));
            ui->clearLocations->setIcon(QIcon(":/img/clear.png"));
            ui->importLocations->setIcon(QIcon(":/img/import.png"));
            ui->exportLocations->setIcon(QIcon(":/img/export.png"));

            ui->deletionMode->setIcon(QIcon(":/img/delete.png"));
            ui->selectionMode->setIcon(QIcon(":/img/select.png"));

            ui->configLoadOpt->setIcon(QIcon(":/img/config.png"));
            ui->configLoadGen->setIcon(QIcon(":/img/config.png"));
            ui->configLoadSim->setIcon(QIcon(":/img/config.png"));
            ui->selectionMode->toggle();

            // Plots
            ui->fitnessPlot->addGraph();
            ui->fitnessPlot->xAxis->setLabel("Steps");
            ui->fitnessPlot->yAxis->setLabel("Fitness");
            ui->fitnessPlot->graph(0)->setPen(QPen(QColor(255, 102, 0)));

            ui->simulationPlot->addGraph();
            ui->simulationPlot->xAxis->setLabel("Order");
            ui->simulationPlot->yAxis->setLabel("Processing time [s]");
            ui->simulationPlot->graph(0)->setPen(QPen(QColor(255, 102, 0)));
            ui->simulationPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
            ui->simulationPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssPlus);

            ui->generatorPlot->addGraph();
            ui->generatorPlot->xAxis->setLabel("x");
            ui->generatorPlot->yAxis->setLabel("y");
            ui->generatorPlot->graph(0)->setPen(QPen(QColor(255, 102, 0)));
            ui->generatorPlot->graph(0)->setBrush(QBrush(QColor(255, 102, 0)));
            ui->generatorPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);

            ui->generatorPlot->addGraph();
            ui->generatorPlot->xAxis->setLabel("x");
            ui->generatorPlot->yAxis->setLabel("y");
            ui->generatorPlot->graph(1)->setPen(QPen(QColor(255, 102, 0)));
            ui->generatorPlot->graph(1)->setBrush(QBrush(QColor(255, 102, 0)));
            ui->generatorPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDiamond);

            stylePlot(ui->fitnessPlot);
            stylePlot(ui->generatorPlot);
            stylePlot(ui->simulationPlot);
        }

        MainWindow::~MainWindow()
        {
            UiWarehouseLayout_t::getWhLayout().clearWhLayout();

            if(fs::exists(tmpDir))
            {
                fs::remove_all(tmpDir);
            }

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
                loc = ui->view->mapToScene(loc.x() - ui->dataBrowserTab->size().width() - ui->frame->window()->pos().x(),
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

                if (cursorMode == UiCursorMode_t::E_MODE_WH_ITEM_LOC)
                {
                    ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());

                    importLocations();
                }
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
            enableManager();
            isOptimizationActive() = false;

            steps.clear();
            fitnesses.clear();

            ui->elapsedTime->setText("0 [s]");
            ui->optimizationProgressBar->setValue(0);
        }

        void MainWindow::generatingFinished(std::string f)
        {
            enableManager();

            auto itadu = std::find_if(xadu.begin(), xadu.end(), [](auto v) { return v < 0; });
            auto itorl = std::find_if(xorl.begin(), xorl.end(), [](auto v) { return v < 0; });

            if(itadu != xadu.end() || itorl != xorl.end())
            {
                QMessageBox::warning(nullptr, "Warning", "There have been some negative values generated, you can either proceed"
                                                         " (but the results might be biased/misleading), or tune parameters and run again.");
            }

            xadu.clear();
            xorl.clear();

            if(!f.empty())
            {
                std::string test = f;
                test.replace(test.find_last_of("."), 0, "_test");

                std::string train = f;
                train.replace(train.find_last_of("."), 0, "_train");

                importOrders(false, test);
                importOrders(true, train);
            }
        }

        void MainWindow::simulationFinished(double time)
        {
            enableManager();

            ui->simulationTime->setText(QString::number(time) + " [s]");

            orders.clear();
            processingDurations.clear();
        }

        void MainWindow::optimizationStep(double fitness)
        {
            if(!optimizerUi)
            {
                return;
            }

            // Stats update

            auto stepCount = steps.size();

            steps.append(stepCount++);
            fitnesses.append(fitness);

            ui->fitnessPlot->graph(0)->setData(steps, fitnesses);
            ui->fitnessPlot->replot();
            ui->fitnessPlot->graph(0)->rescaleAxes();

            double max = *std::max_element(fitnesses.begin(), fitnesses.end());

            ui->fitnessPlot->yAxis->setRange(0, max * 1.1);

            ui->fitnessPlot->update();

            ui->elapsedTime->setText(QString::number(optimizationElapsedTime.elapsed() / 1000.0) + " [s]");

            ui->optimizationProgressBar->setValue(100 * stepCount / static_cast<double>(ui->iterations->value()));

            // Model update

            auto items = ::whm::WarehouseLayout_t::getWhLayout().getWhItems();
            auto uiItems = UiWarehouseLayout_t::getWhLayout().getWhItems();

            QStringList labels = { "Location ID", "Slot x", "Slot y", "Article", "Quantity" };

            locationsModel->clear();
            locationsModel->setHorizontalHeaderLabels(labels);

            int32_t row{ 0 };

            for(auto* item : items)
            {
                if(item->getType() == WarehouseItemType_t::E_LOCATION_SHELF)
                {
                    auto* rack = item->getWhLocationRack();

                    for(int32_t r = 0; r < rack->getSlotCountY(); r++)
                    {
                        for(int32_t c = 0; c < rack->getSlotCountX(); c++)
                        {
                            auto* locationID = new QStandardItem(QString::number(item->getWhItemID()));
                            auto* slotx      = new QStandardItem(QString::number(c));
                            auto* sloty      = new QStandardItem(QString::number(r));
                            auto* article    = new QStandardItem(QString::fromStdString(rack->at(c, r).getArticle()));
                            auto* quantity   = new QStandardItem(QString::number(rack->at(c, r).getQuantity()));

                            locationsModel->setItem(row, 0, locationID);
                            locationsModel->setItem(row, 1, slotx);
                            locationsModel->setItem(row, 2, sloty);
                            locationsModel->setItem(row, 3, article);
                            locationsModel->setItem(row, 4, quantity);

                            ++row;
                        }
                    }
                }
            }

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

            for(auto* item : uiItems)
            {
                if(item->getWhItemType() == WarehouseItemType_t::E_LOCATION_SHELF)
                {
                    for(auto* slot : dynamic_cast<UiWarehouseItemLocation_t*>(item)->getSlots())
                    {
                        std::string article = slot->getArticle();

                        if(!article.empty())
                        {
                            slot->setSlotHeat(heatMax, heatMin, heatMap[article]);
                        }
                    }
                }
            }

            ui->locationsTableView->setModel(locationsModel);
        }

        void MainWindow::newGeneratedValue(int value, int graph)
        {
            if(!generatorUi)
            {
                return;
            }

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

        void MainWindow::orderSimulationFinished(double duration)
        {
            orders.append(orders.size() + 1);
            processingDurations.append(duration);

            ui->simulationPlot->graph(0)->setData(orders, processingDurations);
            ui->simulationPlot->replot();
            ui->simulationPlot->graph(0)->rescaleAxes();

            double max = *std::max_element(processingDurations.begin(), processingDurations.end());

            ui->simulationPlot->yAxis->setRange(0, max * 1.1);

            ui->simulationPlot->update();

            ui->simulationProgressBar->setValue(100 * orders.size() / orderCount);
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

            try
            {
                ::whm::WarehouseLayout_t::getWhLayout().deserializeFromXml(file.toUtf8().constData());
                ::whm::WarehouseLayout_t::getWhLayout().dump();
                UiWarehouseLayout_t::getWhLayout().initFromTui(this->scene, this, ::whm::WarehouseLayout_t::getWhLayout());
                UiWarehouseLayout_t::getWhLayout().dump();
            }
            catch(std::runtime_error& e)
            {
                QMessageBox::critical(nullptr, "Error", e.what());
            }

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

            importLocations();
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

            articlesModel->clear();
            locationsModel->clear();
            ordersTestModel->clear();
            ordersTrainModel->clear();
        }

        void MainWindow::on_startOptimization_clicked()
        {
            std::string a = exportArticles();
            std::string l = exportLocations();
            std::string o = exportOrders(true);

            if(o.empty() || a.empty() || l.empty())
            {
                QMessageBox::warning(nullptr, "Warning", "Load all required data first!");
                return;
            }

            disableManager();
            isOptimizationActive() = true;
            whm::WarehouseLayout_t::getWhLayout().clearWhLayout();
            whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());

            whm::ConfigParser_t cfg;

            // General
            cfg.set("numberDimensions",   std::to_string(ui->numberDimensions->value()));
            cfg.set("problemMin",         std::to_string(ui->problemMin->value()));
            cfg.set("problemMax",         std::to_string(ui->problemMax->value()));
            cfg.set("maxIterations",      std::to_string(ui->iterations->value()));
            cfg.set("saveWeightsPeriod",  std::to_string(ui->weights->value()));
            cfg.set("maxTrialValue",      std::to_string(ui->trialValue->value()));

            // GA
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

            // DE
            cfg.set("populationSizeDE",   std::to_string(ui->populationSizeDE->value()));
            cfg.set("scalingFactor",      std::to_string(ui->scalingFactorDE->value()));
            cfg.set("probCrossoverDE",    std::to_string(ui->probCrossoverDE->value()));

            if(ui->crossoverDE->currentIndex()  == 0) cfg.set("crossoverFunctorDE", "crossoverBinomical");
            if(ui->crossoverDE->currentIndex()  == 1) cfg.set("crossoverFunctorDE", "crossoverOrdered");

            // ABC
            cfg.set("foodSize",           std::to_string(ui->foodSize->value()));
            cfg.set("keepBest",           ui->keepBest->checkState() == Qt::Checked ? "true" : "false");

            // PSO
            cfg.set("numberParticles",    std::to_string(ui->numberParticles->value()));
            cfg.set("correctionFactor1",  std::to_string(ui->correctionFactorPer->value()));
            cfg.set("correctionFactor2",  std::to_string(ui->correctionFactorSoc->value()));
            cfg.set("weighing",           std::to_string(ui->inertiaWeight->value()));

            if(ui->crossoverPSO->currentIndex()  == 0) cfg.set("crossoverFunctorPSO", "crossoverHeuristic");
            if(ui->crossoverPSO->currentIndex()  == 1) cfg.set("crossoverFunctorPSO", "crossoverOrdered");

            // SLAP

            // RAND
            cfg.set("populationSizeRand", std::to_string(ui->populationSizeRand->value()));

            // Paths
            cfg.set("articlesPath", a);
            cfg.set("locationsPath", l);
            cfg.set("ordersPath", o);

            cfg.dump();

            optimizationElapsedTime.start();

            optimizerUi = new UiWarehouseOptimizerThread_t(cfg, ui->optimizerSelectionBox->currentIndex());

            connect(optimizerUi, SIGNAL(finished()),
                    optimizerUi, SLOT(deleteLater()));

            connect(optimizerUi, SIGNAL(optimizationFinished()),
                    this,        SLOT(optimizationFinished()));

            connect(optimizerUi, SIGNAL(optimizationStep(double)),
                    this,        SLOT(optimizationStep(double)));

            optimizerUi->start();
        }

        void MainWindow::on_stopOptimization_clicked()
        {
            if(optimizerUi)
            {
                auto dialog = new QProgressDialog();
                dialog->setWindowTitle("Wait");
                dialog->setRange(0, 0);
                dialog->setLabelText("Aborting optimization");
                dialog->setCancelButton(nullptr);

                dialog->resize(100, 30);
                dialog->show();

                optimizerUi->terminate();
                while (!optimizerUi->wait(100))
                {
                    QApplication::processEvents();
                }
                optimizerUi = nullptr;
                optimizationFinished();

                dialog->hide();
            }
        }

        void MainWindow::on_configLoadOpt_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Optimalizator configuration"), "", tr("Optimalizator configuration (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->configLineOpt->setText(file);

            try
            {
                whm::ConfigParser_t cfg(file.toUtf8().constData(), true);

                // General
                ui->numberDimensions->setValue(cfg.getAs<int32_t>("numberDimensions"));
                ui->problemMin->setValue(cfg.getAs<int32_t>("problemMin"));
                ui->problemMax->setValue(cfg.getAs<int32_t>("problemMax"));
                ui->iterations->setValue(cfg.getAs<int32_t>("maxIterations"));
                ui->weights->setValue(cfg.getAs<int32_t>("saveWeightsPeriod"));
                ui->trialValue->setValue(cfg.getAs<int32_t>("maxTrialValue"));

                // GA
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

                // DE
                ui->populationSizeDE->setValue(cfg.getAs<int32_t>("populationSizeDE"));
                ui->scalingFactorDE->setValue(cfg.getAs<double>("scalingFactor"));
                ui->probCrossoverDE->setValue(cfg.getAs<double>("probCrossoverDE"));

                if(cfg.getAs<std::string>("crossoverFunctorDE") == "crossoverBinomical") ui->crossoverDE->setCurrentIndex(0);
                if(cfg.getAs<std::string>("crossoverFunctorDE") == "crossoverOrdered")   ui->crossoverDE->setCurrentIndex(1);

                // ABC
                ui->foodSize->setValue(cfg.getAs<int32_t>("foodSize"));
                ui->keepBest->setCheckState(cfg.getAs<bool>("keepBest") ? Qt::Checked : Qt::Unchecked);

                // PSO
                ui->numberParticles->setValue(cfg.getAs<int32_t>("numberParticles"));
                ui->correctionFactorPer->setValue(cfg.getAs<double>("correctionFactor1"));
                ui->correctionFactorSoc->setValue(cfg.getAs<double>("correctionFactor2"));
                ui->inertiaWeight->setValue(cfg.getAs<double>("weighing"));

                if(cfg.getAs<std::string>("crossoverFunctorPSO") == "crossoverHeuristic") ui->crossoverPSO->setCurrentIndex(0);
                if(cfg.getAs<std::string>("crossoverFunctorPSO") == "crossoverOrdered")   ui->crossoverPSO->setCurrentIndex(1);

                // SLAP

                // RAND
                ui->populationSizeRand->setValue(cfg.getAs<int32_t>("populationSizeRand"));
            }
            catch(std::runtime_error&)
            {
                QMessageBox::critical(nullptr, "Error", "Invalid configuration selected.");
            }
        }

        void MainWindow::on_configLoadGen_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Generator configuration"), "", tr("Generator configuration (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->configLineGen->setText(file);

            try
            {
                whm::ConfigParser_t cfg(file.toUtf8().constData(), true);

                ui->orderCount->setValue(cfg.getAs<int32_t>("orderCount"));
                ui->aduMi->setValue(cfg.getAs<int32_t>("mi"));
                ui->aduSigma->setValue(cfg.getAs<int32_t>("sigma"));
                ui->orlCountMi->setValue(0);
                ui->orlCountSigma->setValue(cfg.getAs<int32_t>("sigmaLines"));
            }
            catch(std::runtime_error&)
            {
                QMessageBox::critical(nullptr, "Error", "Invalid configuration selected.");
            }
        }

        void MainWindow::on_startGenerating_clicked()
        {
            std::string o = tmpOrders;
            std::string a = exportArticles();

            if(a.empty())
            {
                QMessageBox::warning(nullptr, "Warning", "Load all required data first!");
                return;
            }

            disableManager();

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

            generatorUi = new UiWarehouseGeneratorThread_t(cfg);

            connect(generatorUi, SIGNAL(finished()),
                    generatorUi, SLOT(deleteLater()));

            connect(generatorUi, SIGNAL(generatingFinished(std::string)),
                    this,        SLOT(generatingFinished(std::string)));

            connect(generatorUi, SIGNAL(newGeneratedValue(int, int)),
                    this,        SLOT(newGeneratedValue(int, int)));

            generatorUi->start();
        }

        void MainWindow::on_stopGenerating_clicked()
        {
            if(generatorUi)
            {
                auto dialog = new QProgressDialog();
                dialog->setWindowTitle("Wait");
                dialog->setRange(0, 0);
                dialog->setLabelText("Aborting generating");
                dialog->setCancelButton(nullptr);

                dialog->resize(100, 30);
                dialog->show();

                generatorUi->terminate();
                while (!generatorUi->wait(100))
                {
                    QApplication::processEvents();
                }
                generatorUi = nullptr;
                generatingFinished(std::string());

                dialog->hide();
            }
        }

        void MainWindow::on_configLoadSim_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Simulator configuration"), "", tr("Simulator configuration (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ui->configLineSim->setText(file);

            try
            {
                whm::ConfigParser_t cfg(file.toUtf8().constData(), true);

                ui->toteSpeed->setValue(cfg.getAs<double>("toteSpeed"));
                ui->workerSpeed->setValue(cfg.getAs<double>("workerSpeed"));
                ui->totesPerMinute->setValue(cfg.getAs<int32_t>("totesPerMin"));
                ui->customerReqInterval->setValue(cfg.getAs<int32_t>("orderRequestInterval"));
                ui->locationCapacity->setValue(cfg.getAs<int32_t>("locationCapacity"));
                ui->conveyorCapacity->setValue(cfg.getAs<int32_t>("conveyorCapacity"));
                ui->simulationSpeedup->setValue(cfg.getAs<double>("simSpeedup"));
                ui->preprocessOrders->setCheckState(cfg.getAs<bool>("preprocess") ? Qt::Checked : Qt::Unchecked);
            }
            catch(std::runtime_error&)
            {
                QMessageBox::critical(nullptr, "Error", "Invalid configuration selected.");
            }
        }

        void MainWindow::on_startSimulation_clicked()
        {
            std::string l = exportLocations();
            std::string o = exportOrders(false);

            if(o.empty() || l.empty())
            {
                QMessageBox::warning(nullptr, "Warning", "Load all required data first!");
                return;
            }

            disableManager();
            whm::WarehouseLayout_t::getWhLayout().clearWhLayout();
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

            simulatorUi = new UiWarehouseSimulatorThread_t(cfg);

            connect(simulatorUi, SIGNAL(finished()),
                    simulatorUi, SLOT(deleteLater()));

            connect(simulatorUi, SIGNAL(simulationFinished(double)),
                    this,        SLOT(simulationFinished(double)));

            connect(simulatorUi, SIGNAL(orderSimulationFinished(double)),
                    this,        SLOT(orderSimulationFinished(double)));

            simulatorUi->start();
        }

        void MainWindow::on_stopSimulation_clicked()
        {
            if(simulatorUi)
            {
                auto dialog = new QProgressDialog();
                dialog->setWindowTitle("Wait");
                dialog->setRange(0, 0);
                dialog->setLabelText("Aborting simulation");
                dialog->setCancelButton(nullptr);

                dialog->resize(100, 30);
                dialog->show();

                simulatorUi->terminate();
                while (!simulatorUi->wait(100))
                {
                    QApplication::processEvents();
                }
                simulatorUi = nullptr;
                simulationFinished(0.0);

                dialog->hide();
            }
        }

        void MainWindow::reset()
        {
            ::whm::WarehouseLayout_t::getWhLayout().clearWhLayout();
            ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());
            UiWarehouseLayout_t::getWhLayout().clearWhLayout();
            UiWarehouseLayout_t::getWhLayout().initFromTui(this->scene, this, ::whm::WarehouseLayout_t::getWhLayout());
        }

        void MainWindow::on_importTestOrders_clicked()
        {
            importOrders(false);
        }

        void MainWindow::on_importTrainOrders_clicked()
        {
            importOrders(true);
        }

        void MainWindow::importOrders(bool train, std::string f)
        {
            if(f.empty())
            {
                QString file = QFileDialog::getOpenFileName(this, tr("Import orders"), "", tr("Orders (*.xml)"));
                if (file.cbegin() == file.cend())
                {
                    return;
                }

                f = file.toUtf8().constData();
            }

            QStringList labels = { "Order ID", "Line ID", "Product", "Quantity" };

            QStandardItemModel* m = train ? ordersTrainModel : ordersTestModel;

            m->clear();
            m->setHorizontalHeaderLabels(labels);

            whm::WarehouseLayout_t::getWhLayout().clearWhOrders();
            whm::WarehouseLayout_t::getWhLayout().importCustomerOrders(f);

            int32_t row{ 0 };

            for(auto& order : ::whm::WarehouseLayout_t::getWhLayout().getWhOrders())
            {
                for(auto& line : order.getWhOrderLines())
                {
                    auto* orderID   = new QStandardItem(QString::fromStdString(std::to_string(order.getWhOrderID())));
                    auto* lineID    = new QStandardItem(QString::fromStdString(std::to_string(line.getWhLineID())));
                    auto* article   = new QStandardItem(QString::fromStdString(line.getArticle()));
                    auto* quantity  = new QStandardItem(QString::fromStdString(std::to_string(line.getQuantity())));

                    m->setItem(row, 0, orderID);
                    m->setItem(row, 1, lineID);
                    m->setItem(row, 2, article);
                    m->setItem(row, 3, quantity);

                    ++row;
                }
            }

            if(train)
                ui->trainOrdersTableView->setModel(m);
            else
                ui->testOrdersTableView->setModel(m);
        }

        void MainWindow::on_exportTestOrders_clicked()
        {
            QString file = QFileDialog::getSaveFileName(this, tr("Export test orders"), "", tr("Orders (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            exportOrders(false, file.toUtf8().constData());
        }

        void MainWindow::on_exportTrainOrders_clicked()
        {
            QString file = QFileDialog::getSaveFileName(this, tr("Export train orders"), "", tr("Orders (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            exportOrders(true, file.toUtf8().constData());
        }

        void MainWindow::on_clearTestOrders_clicked()
        {
            ordersTestModel->clear();
        }

        void MainWindow::on_clearTrainOrders_clicked()
        {
            ordersTrainModel->clear();
        }

        void MainWindow::on_importArticles_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Import articles"), "", tr("Articles (*.csv)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            QStringList labels = { "Article name" };

            articlesModel->clear();
            articlesModel->setHorizontalHeaderLabels(labels);

            std::vector<std::string> articles;
            WarehouseLayout_t::getWhLayout().importArticles(file.toUtf8().constData(), articles);

            int32_t row{ 0 };

            for(auto& article : articles)
            {
                auto* articleName = new QStandardItem(QString::fromStdString(article));

                articlesModel->setItem(row++, 0, articleName);
            }

            ui->articlesTableView->setModel(articlesModel);
        }

        void MainWindow::on_clearArticles_clicked()
        {
            articlesModel->clear();
        }

        void MainWindow::on_importLocations_clicked()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Import location slots"), "", tr("Article location allocation (*.csv)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());
            ::whm::WarehouseLayout_t::getWhLayout().importLocationSlots(file.toUtf8().constData());

            importLocations();
        }

        void MainWindow::importLocations()
        {
            auto items = ::whm::WarehouseLayout_t::getWhLayout().getWhItems();
            auto uiItems = UiWarehouseLayout_t::getWhLayout().getWhItems();

            QStringList labels = { "Location ID", "Slot x", "Slot y", "Article", "Quantity" };

            locationsModel->clear();
            locationsModel->setHorizontalHeaderLabels(labels);

            int32_t row{ 0 };

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

                    auto* rack = item->getWhLocationRack();

                    for(int32_t r = 0; r < rack->getSlotCountY(); r++)
                    {
                        for(int32_t c = 0; c < rack->getSlotCountX(); c++)
                        {
                            auto* locationID = new QStandardItem(QString::number(item->getWhItemID()));
                            auto* slotx      = new QStandardItem(QString::number(c));
                            auto* sloty      = new QStandardItem(QString::number(r));
                            auto* article    = new QStandardItem(QString::fromStdString(rack->at(c, r).getArticle()));
                            auto* quantity   = new QStandardItem(QString::number(rack->at(c, r).getQuantity()));

                            locationsModel->setItem(row, 0, locationID);
                            locationsModel->setItem(row, 1, slotx);
                            locationsModel->setItem(row, 2, sloty);
                            locationsModel->setItem(row, 3, article);
                            locationsModel->setItem(row, 4, quantity);

                            ++row;
                        }
                    }
                }
            }

            ui->locationsTableView->setModel(locationsModel);
        }

        void MainWindow::on_exportLocations_clicked()
        {
            QString file = QFileDialog::getSaveFileName(this, tr("Export location slots"), "", tr("Article location allocation (*.csv)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());
            ::whm::WarehouseLayout_t::getWhLayout().exportLocationSlots(file.toUtf8().constData());
        }

        void MainWindow::on_clearLocations_clicked()
        {
            locationsModel->clear();
        }

        void MainWindow::stylePlot(QCustomPlot* p)
        {
            p->xAxis->setBasePen(QPen(Qt::white, 1));
            p->yAxis->setBasePen(QPen(Qt::white, 1));
            p->xAxis->setLabelColor(Qt::white);
            p->yAxis->setLabelColor(Qt::white);
            p->xAxis->setTickPen(QPen(Qt::white, 1));
            p->yAxis->setTickPen(QPen(Qt::white, 1));
            p->xAxis->setSubTickPen(QPen(Qt::white, 1));
            p->yAxis->setSubTickPen(QPen(Qt::white, 1));
            p->xAxis->setTickLabelColor(Qt::white);
            p->yAxis->setTickLabelColor(Qt::white);
            p->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
            p->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
            p->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
            p->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
            p->xAxis->grid()->setSubGridVisible(true);
            p->yAxis->grid()->setSubGridVisible(true);
            p->xAxis->grid()->setZeroLinePen(Qt::NoPen);
            p->yAxis->grid()->setZeroLinePen(Qt::NoPen);
            p->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
            p->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

            p->setBackground(QColor(25, 35, 45));
            p->axisRect()->setBackground(QColor(25, 35, 45));
        }

        void MainWindow::enableManager()
        {
            ui->loadLayout->setEnabled(true);
            ui->saveLayout->setEnabled(true);
            ui->clearLayout->setEnabled(true);
            ui->importTrainOrders->setEnabled(true);
            ui->exportTrainOrders->setEnabled(true);
            ui->clearTrainOrders->setEnabled(true);
            ui->importTestOrders->setEnabled(true);
            ui->exportTestOrders->setEnabled(true);
            ui->clearTestOrders->setEnabled(true);
            ui->importArticles->setEnabled(true);
            ui->clearArticles->setEnabled(true);
            ui->importLocations->setEnabled(true);
            ui->exportLocations->setEnabled(true);
            ui->clearLocations->setEnabled(true);
            ui->configLoadOpt->setEnabled(true);
            ui->configLoadGen->setEnabled(true);
            ui->configLoadSim->setEnabled(true);
            ui->startGenerating->setEnabled(true);
            ui->startSimulation->setEnabled(true);
            ui->startOptimization->setEnabled(true);
            ui->stopGenerating->setEnabled(false);
            ui->stopSimulation->setEnabled(false);
            ui->stopOptimization->setEnabled(false);
        }

        void MainWindow::disableManager()
        {
            ui->loadLayout->setEnabled(false);
            ui->saveLayout->setEnabled(false);
            ui->clearLayout->setEnabled(false);
            ui->importTrainOrders->setEnabled(false);
            ui->exportTrainOrders->setEnabled(false);
            ui->clearTrainOrders->setEnabled(false);
            ui->importTestOrders->setEnabled(false);
            ui->exportTestOrders->setEnabled(false);
            ui->clearTestOrders->setEnabled(false);
            ui->importArticles->setEnabled(false);
            ui->clearArticles->setEnabled(false);
            ui->importLocations->setEnabled(false);
            ui->exportLocations->setEnabled(false);
            ui->clearLocations->setEnabled(false);
            ui->configLoadOpt->setEnabled(false);
            ui->configLoadGen->setEnabled(false);
            ui->configLoadSim->setEnabled(false);
            ui->startGenerating->setEnabled(false);
            ui->startSimulation->setEnabled(false);
            ui->startOptimization->setEnabled(false);
            ui->stopGenerating->setEnabled(true);
            ui->stopSimulation->setEnabled(true);
            ui->stopOptimization->setEnabled(true);
        }

        std::string MainWindow::exportArticles()
        {
            std::string f = tmpArticles;
            std::vector<std::string> articles;

            if(articlesModel->rowCount() == 0)
            {
                return std::string();
            }

            for(int32_t r = 0; r < articlesModel->rowCount(); ++r)
            {
                QModelIndex index = articlesModel->index(r, 0);
                std::string article = articlesModel->data(index).toString().toUtf8().constData();
                articles.push_back(std::move(article));
            }

            WarehouseLayout_t::getWhLayout().exportArticles(f, articles);

            return f;
        }

        std::string MainWindow::exportLocations()
        {
            std::string f = tmpLocations;

            if(locationsModel->rowCount() == 0)
            {
                return std::string();
            }

            ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());
            ::whm::WarehouseLayout_t::getWhLayout().exportLocationSlots(f);

            return f;
        }

        std::string MainWindow::exportOrders(bool train, std::string f)
        {
            int32_t orderID{ -1 };
            std::vector<WarehouseOrderLine_t> lines;

            orderCount = 0;
            auto m = train ? ordersTrainModel : ordersTestModel;

            if(m->rowCount() == 0)
            {
                return std::string();
            }

            if(f.empty())
            {
                f = tmpOrders;
            }

            WarehouseLayout_t::getWhLayout().clearWhOrders();

            for(int32_t r = 0; r < m->rowCount(); ++r)
            {
                WarehouseOrderLine_t line(nullptr);

                auto actID = m->data(m->index(r, 0)).toInt();

                line.setWhLineID(m->data(m->index(r, 1)).toInt());
                line.setArticle(m->data(m->index(r, 2)).toString().toUtf8().constData());
                line.setQuantity(m->data(m->index(r, 3)).toInt());

                if(orderID != -1 && orderID != actID)
                {
                    WarehouseOrder_t order;

                    order.setWhOrderID(orderID);
                    order.setWhOrderLines(lines);

                    ++orderCount;
                    WarehouseLayout_t::getWhLayout().addWhOrder(std::move(order));

                    lines.clear();
                }

                lines.push_back(std::move(line));
                orderID = actID;
            }

            WarehouseOrder_t order;

            order.setWhOrderID(orderID);
            order.setWhOrderLines(lines);

            ++orderCount;
            WarehouseLayout_t::getWhLayout().addWhOrder(std::move(order));
            WarehouseLayout_t::getWhLayout().exportCustomerOrders(f);

            return f;
        }

        void CustomizedGraphicsScene_t::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
        {
            auto* item = itemAt(event->scenePos().toPoint(), QTransform());

            UiWarehouseItem_t* whItem{ nullptr };

            if(!item)
            {
                QGraphicsScene::contextMenuEvent(event);
                return;
            }

            auto* menu = new QMenu(event->widget());

            if((whItem = dynamic_cast<UiWarehouseItem_t*>(item)))
            {
                if(whItem->isConnected())
                {
                    QAction* disconnect = new QAction("Disconnect", this);
                    connect(disconnect, &QAction::triggered, [&whItem](){ whItem->disconnect(); });
                    menu->addAction(disconnect);
                }
            }
            else if(auto* whSlot = dynamic_cast<UiWarehouseSlot_t*>(item))
            {
                whItem = dynamic_cast<UiWarehouseItem_t*>(whSlot->getParent());

                if(whItem->isConnected())
                {
                    QAction* disconnect = new QAction("Disconnect", this);
                    connect(disconnect, &QAction::triggered, [&whItem](){ whItem->disconnect(); });
                    menu->addAction(disconnect);
                }
            }
            else
            {
                QGraphicsScene::contextMenuEvent(event);
                return;
            }

            menu->exec(event->screenPos());

            QGraphicsScene::contextMenuEvent(event);
        }
    }
}
