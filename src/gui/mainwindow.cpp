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

// Local
#include "UiCursor.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UiWarehouseLayout.h"
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

            auto scene = new QGraphicsScene();
            ui->view->setScene(scene);
            ui->view->setFixedSize(2000, 1000);

            for (int32_t x = 0; x <= 2000; x += 20)
            {
                scene->addLine(x, 0, x, 1000, QPen(Qt::green));
            }

            for (int32_t y = 0; y <= 1000; y += 20)
            {
                scene->addLine(0, y, 2000, y, QPen(Qt::red));
            }

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

        void MainWindow::mousePressEvent(QMouseEvent *event)
        {
            if (event->button() == Qt::LeftButton)
            {
                if (UiCursor_t::getCursor().getMode() == UiCursorMode_t::E_MODE_WH_ITEM_LOC)
                {
                    QPoint loc = QCursor::pos();
                    loc = ui->frame->mapFromGlobal(loc);

                    auto whItemLoc = new UiWarehouseItemLocation_t(ui->frame, this, loc, UiWarehouseItemType_t::E_LOCATION_SHELF);
                    UiWarehouseLayout_t::getWhLayout().addWhItem(whItemLoc);
                    UiWarehouseLayout_t::getWhLayout().dump();
                    std::cout << std::endl << std::endl;
                }
                else if (UiCursor_t::getCursor().getMode() == UiCursorMode_t::E_MODE_WH_ITEM_CONV)
                {
                    QPoint loc = QCursor::pos();
                    loc = ui->frame->mapFromGlobal(loc);

                    auto whItemConv = new UiWarehouseItemConveyor_t(ui->frame, this, loc, UiWarehouseItemType_t::E_CONVEYOR);
                    UiWarehouseLayout_t::getWhLayout().addWhItem(whItemConv);
                    UiWarehouseLayout_t::getWhLayout().dump();
                    std::cout << std::endl << std::endl;
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

        void MainWindow::on_whItemLocations_toggled(bool enabled)
        {
            if (enabled)
            {
                UiCursor_t::getCursor().setMode(UiCursorMode_t::E_MODE_WH_ITEM_LOC);
            }
        }

        void MainWindow::on_loadLayout_triggered()
        {

        }

        void MainWindow::on_saveLayout_triggered()
        {

        }

        void MainWindow::on_clearLayout_triggered()
        {

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
