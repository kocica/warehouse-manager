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
#include "UiWarehouseOptimizerThread.h"

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
            scene = new CustomizedGraphicsView_t();

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
            scene->addLine(0,     0,    whX, 0,   pen);
            scene->addLine(0,     0,    0,   whY, pen);
            scene->addLine(0,     whY,  whX, whY, pen);
            scene->addLine(whX,   0,    whX, whY, pen);

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

            // Plot
            ui->fitnessPlot->addGraph();
            ui->fitnessPlot->xAxis->setLabel("Steps");
            ui->fitnessPlot->yAxis->setLabel("Fitness");
            ui->fitnessPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
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
            if(isSimulationActive())
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

        bool& MainWindow::isSimulationActive()
        {
            return this->simulationActive;
        }

        void MainWindow::simulationFinished()
        {
            isSimulationActive() = false;
        }

        void MainWindow::optimizationStep(double fitness)
        {
            static int32_t stepCounter{ 0 };
            static QVector<double> steps;
            static QVector<double> fitnesses;

            steps.append(++stepCounter);
            fitnesses.append(fitness);

            ui->fitnessPlot->graph(0)->setData(steps, fitnesses);
            ui->fitnessPlot->replot();
            ui->fitnessPlot->graph(0)->rescaleAxes(true);
            ui->fitnessPlot->update();

            ui->elapsedTime->setText(QString::number(optimizationElapsedTime.elapsed() / 1000.0) + " [s]");

            ui->optimizationProgressBar->setValue(100 * stepCounter / static_cast<double>(ui->iterations->value()));
        }

        void MainWindow::on_loadLayout_triggered()
        {
            if(isSimulationActive())
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
            if(isSimulationActive())
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

            isSimulationActive() = true;
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

            optimizationElapsedTime.start();

            auto* optimizerUi = new UiWarehouseOptimizerThread_t(cfg);

            connect(optimizerUi, SIGNAL(finished()),
                    optimizerUi, SLOT(deleteLater()));

            connect(optimizerUi, SIGNAL(optimizationFinished()),
                    this,        SLOT(simulationFinished()));

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

        void MainWindow::reset()
        {
            ::whm::WarehouseLayout_t::getWhLayout().clearWhLayout();
            ::whm::WarehouseLayout_t::getWhLayout().initFromGui(UiWarehouseLayout_t::getWhLayout());
            UiWarehouseLayout_t::getWhLayout().clearWhLayout();
            UiWarehouseLayout_t::getWhLayout().initFromTui(this->scene, this, ::whm::WarehouseLayout_t::getWhLayout());
        }

        void CustomizedGraphicsView_t::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
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
