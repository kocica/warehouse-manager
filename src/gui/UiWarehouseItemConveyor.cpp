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
#include "../WarehouseItem.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseItemConveyor_t::UiWarehouseItemConveyor_t(QGraphicsScene* s, MainWindow* ui, ::whm::WarehouseItem_t& i)
            : UiWarehouseItem_t(s, ui, i.getX(), i.getY(), i.getW(), i.getH(), UiWarehouseItemType_t::E_CONVEYOR_R) // TODO item type TUI <-> GUI
        {
            ports.emplace_back(new UiWarehousePort_t(s, this, ui, this, i.getX(), i.getY() + 12));
            ports.emplace_back(new UiWarehousePort_t(s, this, ui, this, i.getX() + 75, i.getY() + 12));
        }

        UiWarehouseItemConveyor_t::UiWarehouseItemConveyor_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, UiWarehouseItemType_t t)
            : UiWarehouseItem_t(s, ui, x, y, w, h, t)
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
            }*/

            if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                std::cerr << "Collision detected - Item's cannot intersect." << std::endl;

                // TODO: Remove from layout and deleteLater()?
            }

            ports.emplace_back(new UiWarehousePort_t(s, this, ui, this, x,  y + 12));
            ports.emplace_back(new UiWarehousePort_t(s, this, ui, this, x + 75, y + 12));
        }
    }
}
