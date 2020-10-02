/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemLocation.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse location
 */

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>

#include "UiWarehouseLayout.h"
#include "UiWarehouseItemLocation.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseItemLocation_t::UiWarehouseItemLocation_t(QWidget* p, MainWindow* ui, QPoint loc, UiWarehouseItemType_t t)
            : UiWarehouseItem_t(p, ui, loc, t)
        {
            auto dialog = new QDialog(this);
            auto form   = new QFormLayout(dialog);
            auto locX   = new QLineEdit(dialog);
            auto locY   = new QLineEdit(dialog);

            form->addRow(QString("Enter X dimenstion: "), locX);
            form->addRow(QString("Enter Y dimenstion: "), locY);

            QDialogButtonBox buttons(QDialogButtonBox::Ok, Qt::Horizontal, dialog);
            QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
            QObject::connect(&buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
            form->addRow(&buttons);

            if (dialog->exec() == QDialog::Accepted)
            {
                sizeX = locX->text().toInt();
                sizeY = locY->text().toInt();
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
            this->setStyleSheet("background-color: green; font-size: 11px; border: 1px solid black; outline: none;");

            if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                QMessageBox err;
                err.critical(0, "Collision detected", "Item's cannot intersect.");
                err.setFixedSize(500,200);

                // TODO: Remove from layout?
            }
            else
            {
                this->show();
                whPort = new UiWarehousePort_t(this, ui, this, sizeX/2-(10/2), sizeY/2-(10/2));
            }
        }
    }
}