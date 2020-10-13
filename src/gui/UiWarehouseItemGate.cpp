/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemGate.cpp
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse entrance/dispatch gate
 */

#include <iostream>

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>

#include "UiWarehouseLayout.h"
#include "UiWarehouseItemGate.h"
#include "../WarehouseItem.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseItemGate_t::UiWarehouseItemGate_t(QGraphicsScene* s, MainWindow* ui, ::whm::WarehouseItem_t& i)
            : UiWarehouseItem_t(s, ui, i.getX(), i.getY(), i.getW(), i.getH(), i.getType())
        {
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, i.getX() + 12, i.getY() + 12));
            this->setGraphicItemOrientation(i.getO()); // We have to rotate the object after its constructed
        }

        UiWarehouseItemGate_t::UiWarehouseItemGate_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, WarehouseItemType_t t)
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

            /*if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                std::cerr << "Collision detected - Item's cannot intersect." << std::endl;

                // TODO: Remove from layout and deleteLater()?
            }*/

            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x + 12,  y + 12));
        }
    }
}