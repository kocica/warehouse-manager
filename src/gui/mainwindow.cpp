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
#include "../WarehouseLayout.h"

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
            ui->view->setStyleSheet("background-color: rgb(255, 255, 255)");

            setWindowTitle("Warehouse Manager");
            setFixedSize(1920, 1080);

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

            // Create scene
            scene = new QGraphicsScene();
            /*if (dialog->exec() == QDialog::Accepted)
            {
                scene->setSceneRect(0, 0, whDimX->text().toInt(), whDimY->text().toInt());
            }
            else*/
            {
                scene->setSceneRect(0, 0, 10000, 10000);
            }
            ui->view->setScene(scene);

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
            UiWarehouseLayout_t::getWhLayout().clearWhLayout();

            delete ui;
        }

        QPoint MainWindow::getWidgetPosition(QPoint p)
        {
            QPoint ret = ui->frame->mapFromGlobal(p);
            return ret;
        }

        void MainWindow::mousePressEvent(QMouseEvent *event)
        {
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
                loc = ui->view->mapToScene(loc.x() - ui->mainToolBar->size().width(), loc.y() - ui->layoutManagement->size().height());

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
                    w = width->text().toInt();
                    h = height->text().toInt();
                }
                else
                {
                    std::cerr << "Invalid input, abort!" << std::endl;
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

        void MainWindow::on_loadLayout_triggered()
        {
            QString file = QFileDialog::getOpenFileName(this, tr("Load warehouse layout"), "", tr("Warehouse layouts (*.xml)"));
            if (file.cbegin() == file.cend())
            {
                return;
            }

            ::whm::WarehouseLayout_t::getWhLayout().deserializeFromXml(file.toUtf8().constData());
            ::whm::WarehouseLayout_t::getWhLayout().dump();
            UiWarehouseLayout_t::getWhLayout().initFromTui(this->scene, this, ::whm::WarehouseLayout_t::getWhLayout());
            UiWarehouseLayout_t::getWhLayout().dump();
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
            ::whm::WarehouseLayout_t::getWhLayout().clearWhLayout();
            UiWarehouseLayout_t::getWhLayout().clearWhLayout();
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
