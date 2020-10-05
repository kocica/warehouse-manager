/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemConveyor.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse conveyor
 */

#include <iostream>

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>

#include "UiWarehouseLayout.h"
#include "UiWarehouseItemConveyor.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseItemConveyor_t::UiWarehouseItemConveyor_t(QGraphicsScene* s, MainWindow* ui, QPoint loc, UiWarehouseItemType_t t)
            : UiWarehouseItem_t(s, ui, loc, t)
        {
            /*auto dialog = new QDialog(this);
            auto form   = new QFormLayout(dialog);
            auto len    = new QLineEdit(dialog);

            form->addRow(QString("Enter conveyor length: "), len);

            QDialogButtonBox buttons(QDialogButtonBox::Ok, Qt::Horizontal, dialog);
            QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
            form->addRow(&buttons);

            if (dialog->exec() == QDialog::Accepted)
            {
                // TODO: Horizontal conv x = 10 y = length
                sizeX = len->text().toInt();
                sizeY = 10; // TODO: Port size
            }

            if(t == UiWarehouseItemType_t::E_CONVEYOR_R || t == UiWarehouseItemType_t::E_CONVEYOR_L)
            {
                sizeX = 100;
                sizeY = 20;
            }
            else if(t == UiWarehouseItemType_t::E_CONVEYOR_U || t == UiWarehouseItemType_t::E_CONVEYOR_D)
            {
                sizeX = 20;
                sizeY = 100;
            }
            else // Hub
            {
                sizeX = 20;
                sizeY = 20;
            }



            if (loc.x() < sizeX/2)
            {
                loc.rx() = sizeX/2;
            }

            if (loc.y() < sizeY/2)
            {
                loc.ry() = sizeY/2;
            }

            if (loc.x() > p->minimumWidth() - sizeX/2)
            {
                p->setMinimumWidth(loc.x() + sizeX/2);
            }

            if (loc.y() > p->minimumHeight() - sizeY/2)
            {
                p->setMinimumHeight(loc.y() + sizeY/2);
            }

            loc.rx() -= sizeX/2;
            loc.ry() -= sizeY/2;

            loc.rx() -= loc.rx() % 20 - 1;
            loc.ry() -= loc.ry() % 20 - 1;

            this->move(loc);
            this->resize(sizeX, sizeY);
            this->setStyleSheet("border-style: outset; border-width: 2px; border-color: blue;");

            if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                //QMessageBox err;
                //err.critical(0, "Collision detected", "Item's cannot intersect.");
                //err.setFixedSize(500,200);

                std::cout << "Collision detected - Item's cannot intersect." << std::endl;

                // TODO: Remove from layout?
            }
            else
            {
                this->show();

                switch(this->getWhItemType())
                {
                    case UiWarehouseItemType_t::E_CONVEYOR_R:
                    case UiWarehouseItemType_t::E_CONVEYOR_L:
                        ports.emplace_back(new UiWarehousePort_t(this, ui, this, 0, sizeY/2-(10/2)));
                        ports.emplace_back(new UiWarehousePort_t(this, ui, this, sizeX-10, sizeY/2-(10/2)));
                        break;
                    case UiWarehouseItemType_t::E_CONVEYOR_U:
                    case UiWarehouseItemType_t::E_CONVEYOR_D:
                        ports.emplace_back(new UiWarehousePort_t(this, ui, this, sizeX/2-(10/2), 0));
                        ports.emplace_back(new UiWarehousePort_t(this, ui, this, sizeX/2-(10/2), sizeY-10));
                        break;
                    case UiWarehouseItemType_t::E_CONVEYOR_HUB:
                        ports.emplace_back(new UiWarehousePort_t(this, ui, this, 0, sizeY/2-(10/2)));
                        ports.emplace_back(new UiWarehousePort_t(this, ui, this, sizeX-10, sizeY/2-(10/2)));
                        ports.emplace_back(new UiWarehousePort_t(this, ui, this, sizeX/2-(10/2), 0));
                        ports.emplace_back(new UiWarehousePort_t(this, ui, this, sizeX/2-(10/2), sizeY-10));
                        break;
                    default:
                        throw std::exception();
                }
            }*/

            ports.emplace_back(new UiWarehousePort_t(s, this, ui, this, loc.x() + 50,  loc.y() + 80));
            ports.emplace_back(new UiWarehousePort_t(s, this, ui, this, loc.x() + 125, loc.y() + 80));
        }
    }
}
