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
            : UiWarehouseItem_t(s, ui, i.getX(), i.getY(), i.getW(), i.getH(), i.getType())
        {
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, i.getX() + i.getW()/2 - portSizeX, i.getY() + i.getH()/2 - portSizeY/2, portSizeX, portSizeY));
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, i.getX() + i.getW()/2            , i.getY() + i.getH()/2 - portSizeY/2, portSizeX, portSizeY));

            this->setGraphicItemOrientation(i.getO()); // We have to rotate the object after its constructed

            setDimensions(i.getSlotCountX(), i.getSlotCountY());
        }

        UiWarehouseItemLocation_t::UiWarehouseItemLocation_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, WarehouseItemType_t t)
            : UiWarehouseItem_t(s, ui, x, y, w, h, t)
        {
            auto dialog = new QDialog(ui);
            auto form   = new QFormLayout(dialog);
            auto locX   = new QLineEdit(dialog);
            auto locY   = new QLineEdit(dialog);

            form->addRow(QString("Enter X slot dimenstion: "), locX);
            form->addRow(QString("Enter Y slot dimenstion: "), locY);

            QDialogButtonBox buttons(QDialogButtonBox::Ok, Qt::Horizontal, dialog);
            QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
            QObject::connect(&buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
            form->addRow(&buttons);

            if (dialog->exec() == QDialog::Accepted)
            {
                setDimensions(locX->text().toInt(), locY->text().toInt());
            }
            else
            {
                // Item cannot be constructed, deleteLater()
            }

            /*if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                std::cerr << "Collision detected - Item's cannot intersect." << std::endl;

                // TODO: Remove from layout and deleteLater()?
            }*/

            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x + w/2 - portSizeX, y + h/2 - portSizeY/2, portSizeX, portSizeY));
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, x + w/2            , y + h/2 - portSizeY/2, portSizeX, portSizeY));
        }
    }
}