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
#include "UiWarehouseItemLocation.h"
#include "UiWarehouseItemConveyor.h"

// Qt
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QFileDialog>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QGraphicsScene>
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
            ui->warehouseLayoutArea->setStyleSheet("background-color: rgb(255, 255, 255)");

            setWindowTitle("Warehouse Manager");
            setFixedSize(1000, 700);

            // Get warehouse dimensions
            auto dialog = new QDialog(this);
            auto form   = new QFormLayout(dialog);
            auto whDimX = new QLineEdit(dialog);
            auto whDimY = new QLineEdit(dialog);

            form->addRow(new QLabel("Enter warehouse dimensions"));
            form->addRow(QString("Enter X dimenstion: "), whDimX);
            form->addRow(QString("Enter Y dimenstion: "), whDimY);

            QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
            QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
            QObject::connect(&buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
            form->addRow(&buttons);

            /*if (dialog->exec() == QDialog::Accepted)
            {
                ui->warehouseLayoutArea->setFixedSize(whDimX->text().toInt(), whDimY->text().toInt());
            }*/
            ui->warehouseLayoutArea->setFixedSize(2000, 1000);

            // Create grid
            auto scene = new QGraphicsScene();
            ui->view->setScene(scene);
            ui->view->setFixedSize(2000, 1000);

            QPen backgroundGrid;
            backgroundGrid.setColor("#d7d6d5");

            for (int32_t x = 0; x <= 2000; x += 20)
            {
                scene->addLine(x, 0, x, 1000, backgroundGrid);
            }

            for (int32_t y = 0; y <= 1000; y += 20)
            {
                scene->addLine(0, y, 2000, y, backgroundGrid);
            }

            // Enable zooming
            auto zoom = new UiGraphicsViewZoom_t(ui->view);
            zoom->setModifiers(Qt::NoModifier);

            setContextMenuPolicy(Qt::NoContextMenu);

            ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
            ui->layoutManagement->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
            ui->simulationManagement->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);

            ui->simulationRun->setIcon(QIcon(":/img/start.png"));
            ui->simulationStop->setIcon(QIcon(":/img/stop.png"));
            ui->simulationStep->setIcon(QIcon(":/img/step.png"));

            ui->saveLayout->setIcon(QIcon(":/img/save.png"));
            ui->loadLayout->setIcon(QIcon(":/img/load.png"));
            ui->clearLayout->setIcon(QIcon(":/img/clear.png"));
            ui->deletionMode->setIcon(QIcon(":/img/delete.png"));
            ui->selectionMode->setIcon(QIcon(":/img/select.png"));

            ui->selectionMode->toggle();
        }

        MainWindow::~MainWindow()
        {
            UiWarehouseLayout_t::getWhLayout().deleteAllWhItems();

            delete ui;
        }

        QPoint MainWindow::getWidgetPosition(QPoint p)
        {
            QPoint ret = ui->frame->mapFromGlobal(p);
            return ret;
        }

        void MainWindow::mousePressEvent(QMouseEvent *event)
        {
            static std::map<UiCursorMode_t, UiWarehouseItemType_t> convMap =
            {
                { UiCursorMode_t::E_MODE_WH_ITEM_CONV_R,   UiWarehouseItemType_t::E_CONVEYOR_R },
                { UiCursorMode_t::E_MODE_WH_ITEM_CONV_L,   UiWarehouseItemType_t::E_CONVEYOR_L },
                { UiCursorMode_t::E_MODE_WH_ITEM_CONV_U,   UiWarehouseItemType_t::E_CONVEYOR_U },
                { UiCursorMode_t::E_MODE_WH_ITEM_CONV_D,   UiWarehouseItemType_t::E_CONVEYOR_D },
                { UiCursorMode_t::E_MODE_WH_ITEM_CONV_HUB, UiWarehouseItemType_t::E_CONVEYOR_HUB }
            };

            if (event->button() == Qt::LeftButton)
            {
                auto cursorMode = UiCursor_t::getCursor().getMode();

                if (cursorMode == UiCursorMode_t::E_MODE_WH_ITEM_LOC)
                {
                    QPoint loc = QCursor::pos();
                    loc = ui->frame->mapFromGlobal(loc);

                    auto whItemLoc = new UiWarehouseItemLocation_t(ui->view, this, loc, UiWarehouseItemType_t::E_LOCATION_SHELF);
                    UiWarehouseLayout_t::getWhLayout().addWhItem(whItemLoc);

                    UiWarehouseLayout_t::getWhLayout().dump();
                }
                else if (convMap.find(cursorMode) != convMap.end())
                {
                    QPoint loc = QCursor::pos();
                    loc = ui->frame->mapFromGlobal(loc);

                    auto whItemConv = new UiWarehouseItemConveyor_t(ui->frame, this, loc, convMap[cursorMode]);
                    UiWarehouseLayout_t::getWhLayout().addWhItem(whItemConv);

                    UiWarehouseLayout_t::getWhLayout().dump();
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

        void MainWindow::on_whItemConveyorRight_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_CONV_R);
            }
        }

        void MainWindow::on_whItemConveyorLeft_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_CONV_L);
            }
        }

        void MainWindow::on_whItemConveyorUp_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_CONV_U);
            }
        }

        void MainWindow::on_whItemConveyorDown_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_CONV_D);
            }
        }

        void MainWindow::on_whItemConveyorHub_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_CONV_HUB);
            }
        }

        void MainWindow::on_whItemLocations_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_LOC);
            }
        }

        void MainWindow::on_loadLayout_triggered()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Load warehouse layout"), "./savedLayout.xml", tr("Warehouse layouts (*)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            // TODO: Load
        }

        void MainWindow::on_saveLayout_triggered()
        {
            QString file = QFileDialog::getSaveFileName(this, tr("Save warehouse layout"), "./savedLayout.xml", tr("Warehouse layouts (*)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            // TODO: Save
        }

        void MainWindow::on_clearLayout_triggered()
        {
            UiWarehouseLayout_t::getWhLayout().deleteAllWhItems();
        }

        void MainWindow::on_simulationRun_triggered()
        {

        }

        void MainWindow::on_simulationStep_triggered()
        {

        }

        void MainWindow::on_simulationStop_triggered()
        {

        }
    }
}
