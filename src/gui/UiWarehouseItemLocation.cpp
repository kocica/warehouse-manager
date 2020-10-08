/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemLocation.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse location
 */

#include <iostream>

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>

#include "UiWarehouseLayout.h"
#include "UiWarehouseItemLocation.h"
#include "../WarehouseItem.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseItemLocation_t::UiWarehouseItemLocation_t(QGraphicsScene* s, MainWindow* ui, ::whm::WarehouseItem_t& i)
            : UiWarehouseItem_t(s, ui, i.getX(), i.getY(), i.getW(), i.getH(), UiWarehouseItemType_t::E_LOCATION_SHELF)
        {
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, i.getX(), i.getY() + 37));
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, i.getX() + 75, i.getY() + 37));
        }

        UiWarehouseItemLocation_t::UiWarehouseItemLocation_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, UiWarehouseItemType_t t)
            : UiWarehouseItem_t(s, ui, x, y, w, h, t)
        {
            /*auto dialog = new QDialog(this);
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
            }*/

            if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                std::cerr << "Collision detected - Item's cannot intersect." << std::endl;

                // TODO: Remove from layout and deleteLater()?
            }

            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x,  y + 37));
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, x + 75, y + 37));
        }
    }
}