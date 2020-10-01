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
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Qt
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QMouseEvent>
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

            // Get warehouse dimensions
            auto dialog = new QDialog(this);
            auto form   = new QFormLayout(dialog);
            auto whDimX = new QLineEdit(dialog);
            auto whDimY = new QLineEdit(dialog);

            form->addRow(new QLabel("Enter warehouse dimensions"));
            form->addRow(QString("Enter X dimenstion: "), whDimX);
            form->addRow(QString("Enter Y dimenstion: "), whDimY);

            QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
            form->addRow(&buttons);
            QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
            QObject::connect(&buttons, SIGNAL(rejected()), dialog, SLOT(reject()));

            if (dialog->exec() == QDialog::Accepted)
            {
                ui->warehouseLayoutArea->setFixedSize(whDimX->text().toInt(), whDimY->text().toInt());
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
                QPoint loc = QCursor::pos();
                loc = ui->frame->mapFromGlobal(loc);

                std::cout << loc.x() << " " << loc.y() << std::endl;

                if (loc.x() < 0 || loc.x() > ui->frame->width() || loc.y() < 0 || loc.y() < ui->frame->height())
                {
                    std::cout << "outside" << std::endl;
                }
            }
        }

        void MainWindow::on_loadLayout_triggered()
        {
            std::cout << "loadLayout" << std::endl;
        }

        void MainWindow::on_saveLayout_triggered()
        {
            std::cout << "saveLayout" << std::endl;
        }

        void MainWindow::on_clearLayout_triggered()
        {
            std::cout << "clearLayout" << std::endl;
        }

        void MainWindow::on_simulationRun_triggered()
        {
            std::cout << "simRun" << std::endl;
        }

        void MainWindow::on_simulationStep_triggered()
        {
            std::cout << "simStep" << std::endl;
        }

        void MainWindow::on_simulationStop_triggered()
        {
            std::cout << "simStop" << std::endl;
        }

        void MainWindow::on_deletionMode_toggled(bool enabled)
        {
            std::cout << "delMode" << std::endl;
        }

        void MainWindow::on_selectionMode_toggled(bool enabled)
        {
            std::cout << "selMode" << std::endl;
        }

        void MainWindow::on_whItemConveyor_toggled(bool enabled)
        {
            std::cout << "whItemConv" << std::endl;
        }

        void MainWindow::on_whItemLocations_toggled(bool enabled)
        {
            std::cout << "whItemLoc" << std::endl;
        }
    }
}
