/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemConveyor.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse conveyor
 */

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
        UiWarehouseItemConveyor_t::UiWarehouseItemConveyor_t(QWidget* p, MainWindow* ui, QPoint loc, UiWarehouseItemType_t t)
            : UiWarehouseItem_t(p, ui, loc, t)
        {
            auto dialog = new QDialog(this);
            auto form   = new QFormLayout(dialog);
            auto len    = new QLineEdit(dialog);

            form->addRow(QString("Enter conveyor length: "), len);

            QDialogButtonBox buttons(QDialogButtonBox::Ok, Qt::Horizontal, dialog);
            QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
            form->addRow(&buttons);

            /*if (dialog->exec() == QDialog::Accepted)
            {
                // TODO: Horizontal conv x = 10 y = length
                sizeX = len->text().toInt();
                sizeY = 10; // TODO: Port size
            }*/
            sizeX = 100;
            sizeY = 20;

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
            this->setStyleSheet("background-color: red; font-size: 11px; border: 1px solid black; outline: none;");

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
                whPort1 = new UiWarehousePort_t(this, ui, this, 0, sizeY/2-(10/2));
                whPort2 = new UiWarehousePort_t(this, ui, this, sizeX-10, sizeY/2-(10/2));
            }
        }
    }
}
