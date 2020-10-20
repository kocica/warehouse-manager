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
            ui->view->setStyleSheet("background-color: rgb(255, 255, 255)");
            ui->view->setGeometry(0, 0, qApp->desktop()->availableGeometry().size().width() * 0.8,
                                        qApp->desktop()->availableGeometry().size().height() * 0.8);

            setWindowTitle("Warehouse Manager");

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
            scene = new QGraphicsScene();

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
                loc = ui->view->mapToScene(loc.x() - ui->mainToolBar->size().width() - ui->frame->window()->pos().x(),
                                           loc.y() - ui->layoutManagement->size().height() - ui->frame->window()->pos().y());

                if (loc.x() < 0 || loc.x() > whX ||
                    loc.y() < 0 || loc.y() > whY)
                {
                    std::cerr << "Outside of scene bounds!" << std::endl;
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
                    QMessageBox err;
                    err.critical(0, "Error", "Invalid dimenstions entered");
                    err.setFixedSize(500, 200);
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

        void MainWindow::on_loadLayout_triggered()
        {
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
